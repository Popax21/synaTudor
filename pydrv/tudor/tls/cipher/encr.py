from __future__ import annotations

import secrets
import logging
import cryptography.hazmat.primitives.ciphers as cipher
import cryptography.hazmat.primitives.hashes as hashes
import cryptography.hazmat.primitives.hmac as hmac
from .. import data

class TlsEncryptionAlgorithm:
    def __init__(self, premaster_secret : bytes, client_random : data.TlsRandom, server_random : data.TlsRandom, hash_algo : hashes.HashAlgorithm):
        #Calculate master secret
        rstream = data.TlsDataWriteStream()
        client_random.write(rstream)
        server_random.write(rstream)
        self.derive_input = rstream.data
        self.master_secret = data.tls_prf(premaster_secret, "master secret", rstream.data, 48, hash_algo)

    def encrypt(self, compr : data.TlsCompressed) -> data.TlsCiphertext: raise NotImplementedError()
    def decrypt(self, ctext : data.TlsCiphertext) -> data.TlsCompressed: raise NotImplementedError()

class TlsBlockEncryptionAlgorithm(TlsEncryptionAlgorithm):
    def __init__(self, premaster_secret : bytes, client_random : data.TlsRandom, server_random : data.TlsRandom, encr_algo : type, encr_mode : type, hash_algo : type, encr_key_size : int, hmac_key_size : int):
        super().__init__(premaster_secret, client_random, server_random, hash_algo())

        self.encr_algo = encr_algo
        self.encr_mode = encr_mode
        self.hash_algo = hash_algo

        self.encr_seq_num = 0
        self.decr_seq_num = 0

        #Generate keys
        kstream = data.TlsDataReadStream(data.tls_prf(self.master_secret, "key expansion", self.derive_input, 2 * hmac_key_size + 2 * encr_key_size, hash_algo()))
        self.encr_mac_key = kstream.read(hmac_key_size)
        self.decr_mac_key = kstream.read(hmac_key_size)
        self.encr_key = kstream.read(encr_key_size)
        self.decr_key = kstream.read(encr_key_size)

    def encrypt(self, compr : data.TlsCompressed) -> data.TlsCiphertext:
        ostream = data.TlsDataWriteStream()

        #Create random IV & add it to output
        iv = secrets.token_bytes(len(self.encr_key))
        ostream.write(iv)

        #Create cipher encryptor
        encr = cipher.Cipher(self.encr_algo(self.encr_key), self.encr_mode(iv)).encryptor()

        #Add fragment to encrypted data
        encr.update(compr.fragment)

        #Add MAC to encrypted data
        h = hmac.HMAC(self.encr_mac_key, self.hash_algo())
        h.update(self.encr_seq_num.to_bytes(8, "big"))
        cdata = data.TlsDataWriteStream()
        compr.write(cdata)
        h.update(cdata.data)

        ostream.write(encr.update(h.finalize()))

        #Add padding to encrypted data
        pad_size = 15 - (len(compr.frag) % 16)
        ostream.write(encr.update(bytes([pad_size] * (pad_size+1))) + encr.finalize())

        self.encr_seq_num += 1
        return data.TlsCiphertext(compr.content_type, compr.proto_ver, ostream.data)

    def decrypt(self, ctext : data.TlsCiphertext) -> data.TlsCompressed:
        istream = data.TlsDataReadStream(ctext.fragment)

        #Get IV
        iv = istream.read(len(self.decr_key))

        #Decrypt encrypted data
        if (len(istream) % len(self.decr_key)) != 0: raise data.TlsAlertException(data.TlsAlertDescription.BAD_RECORD_MAC)
        decr = cipher.Cipher(self.encr_algo(self.decr_key), self.encr_mode(iv)).decryptor()
        ddata = decr.update(istream.read(len(istream))) + decr.finalize()

        #Check padding
        pad_size = ddata[-1]
        if not (0 <= pad_size < len(self.decr_key)): raise data.TlsAlertException(data.TlsAlertDescription.BAD_RECORD_MAC)

        #Create TLSCompressed
        h = hmac.HMAC(self.decr_mac_key, self.hash_algo())
        compr = data.TlsCompressed(ctext.content_type, ctext.proto_ver, ddata[:-(h.algorithm.digest_size + pad_size)])

        #Verify HMAC
        #Yes, by only verifying the MAC now, we are vulnerable to timing attacks
        #But I don't care
        h.update(self.decr_seq_num.to_bytes(8, "big"))
        cdata = data.TlsDataWriteStream()
        compr.write(cdata)
        h.update(cdata.data)

        if h.finalize() != ddata[-(h.algorithm.digest_size + pad_size):-pad_size]: raise data.TlsAlertException(data.TlsAlertDescription.BAD_RECORD_MAC)

        self.decr_seq_num += 1
        return compr

class TlsAEADEncryptionAlgorithm(TlsEncryptionAlgorithm):
    def __init__(self, premaster_secret : bytes, client_random : data.TlsRandom, server_random : data.TlsRandom, encr_algo : type, encr_mode : type, hash_algo : type, key_size : int, iv_size : int, tag_size : int):
        super().__init__(premaster_secret, client_random, server_random, hash_algo())

        self.encr_algo = encr_algo
        self.encr_mode = encr_mode
        self.tag_size = tag_size

        self.encr_seq_num = 0
        self.decr_seq_num = 0

        #Generate keys
        kstream = data.TlsDataReadStream(data.tls_prf(self.master_secret, "key expansion", self.derive_input, 4 * key_size, hash_algo()))
        self.encr_key = kstream.read(key_size)
        self.decr_key = kstream.read(key_size)
        self.encr_iv = kstream.read(iv_size)
        self.decr_iv = kstream.read(iv_size)

    def encrypt(self, compr : data.TlsCompressed) -> data.TlsCiphertext:
        ostream = data.TlsDataWriteStream()

        #Create random nonce & add it to output
        nonce = secrets.token_bytes(8)
        ostream.write(nonce)

        #Create cipher encryptor
        cdata = data.TlsDataWriteStream()
        compr.write(cdata)
        encr = cipher.Cipher(self.encr_algo(self.encr_key), self.encr_mode(self.encr_iv + nonce)).encryptor()
        encr.authenticate_additional_data(self.encr_seq_num.to_bytes(8, "big") + cdata.data[:5])

        #Encrypt fragment and add to output
        ostream.write(encr.update(compr.fragment) + encr.finalize())
        ostream.write(encr.tag[:self.tag_size])

        self.encr_seq_num += 1
        return data.TlsCiphertext(compr.content_type, compr.proto_ver, ostream.data)

    def decrypt(self, ctext : data.TlsCiphertext) -> data.TlsCompressed:
        try:
            istream = data.TlsDataReadStream(ctext.fragment)

            #Get nonce
            nonce = istream.read(8)

            #Create cipher decryptor
            cdata = data.TlsDataWriteStream()
            data.TlsCompressed(ctext.content_type, ctext.proto_ver, bytes(len(ctext.fragment) - 8 - self.tag_size)).write(cdata) #We just need the header
            decr = cipher.Cipher(self.encr_algo(self.decr_key), self.encr_mode(self.decr_iv + nonce)).decryptor()
            decr.authenticate_additional_data(self.decr_seq_num.to_bytes(8, "big") + cdata.data[:5])

            #Decrypt fragment
            dfrag = decr.update(istream.read(len(istream) - self.tag_size)) + decr.finalize_with_tag(istream.read(self.tag_size))

            self.decr_seq_num += 1
            return data.TlsCompressed(ctext.content_type, ctext.proto_ver, dfrag)
        except Exception as e:
            logging.error("Error during decryption")
            raise data.TlsAlertException(data.TlsAlertDescription.BAD_RECORD_MAC) from e