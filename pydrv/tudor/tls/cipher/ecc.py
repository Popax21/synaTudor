from __future__ import annotations

import enum
import cryptography.hazmat.primitives.hashes as hashes
import cryptography.hazmat.primitives.asymmetric.ec as ecc
import cryptography.hazmat.primitives.asymmetric.utils as utils
import cryptography.x509 as x509
import tudor.sensor
from ..handshake import *
from .suite import *
from .remote_key import *

class TlsECPoint(data.TlsData):
    def __init__(self, x : int, y : int):
        super().__init__()
        self.x = x
        self.y = y
    
    @staticmethod
    def read(stream : data.TlsDataReadStream):
        #Must be in uncompressed format
        if stream.read_int(1) != 0x4: raise data.TlsAlertException(data.TlsAlertDescription.DECODE_ERROR)
        return TlsECPoint(stream.read_int(32), stream.read_int(32))

    def write(self, stream : data.TlsDataWriteStream):
        stream.write_int(1, 0x4)
        stream.write_int(32, self.x)
        stream.write_int(32, self.y)


#BROKEN This key exchange algorithm is not according to standard
class TlsEcdhKeyExchangeAlgorithm(TlsCipherSuite):
    def __init__(self, remote_key : TlsEccRemoteKey):
        self.remote_key = remote_key
        self.in_handshake = False 
        self.premaster_secret = None
        
    def advertise_extensions(self) -> list: 
        return [
            data.TlsHandshakeExtensionSupportedGroups([23]),
            data.TlsHandshakeExtensionECPointFormats([data.TlsHandshakeExtensionECPointFormats.UNCOMPRESSED])
        ]

    def start_handshake(self, handshake_proto : TlsHandshakeProtocol, server_hello : data.TlsHandshakeServerHello):
        assert not self.in_handshake
        self.in_handshake = True
        self.handshake_proto = handshake_proto
        self.cert_request : data.TlsHandshakeCertificateRequest = None

    def receive(self, msg : data.TlsDataInstance): 
        assert self.in_handshake
        if isinstance(msg, data.TlsHandshakeCertificateRequest):
            if self.cert_request != None: raise data.TlsAlertException(data.TlsAlertDescription.UNEXPECTED_MESSAGE)
            self.cert_request : data.TlsHandshakeCertificateRequest = msg

    def end_handshake(self): 
        assert self.in_handshake
        self.in_handshake = False

        #The server must have requested a certificate
        if self.cert_request == None or not data.TlsCertificateType.ECDSA_SIGN in self.cert_request.cert_types: 
            raise data.TlsAlertException(data.TlsAlertDescription.HANDSHAKE_FAILURE)

        #Send certificate
        self.handshake_proto.send(data.TlsHandshakeMessage(data.TlsHandshakeCertificate(
            data.TlsCertificate(self.remote_key.client_cert.tobytes())
        )))

        #Create ephemeral ECC key pair & send it to the server
        eph_priv_key = ecc.generate_private_key(ecc.SECP256R1())

        kstream = data.TlsDataWriteStream()
        TlsECPoint(eph_priv_key.public_key().public_numbers().x, eph_priv_key.public_key().public_numbers().y).write(kstream)
        self.handshake_proto.send(data.TlsHandshakeMessage(
            data.TlsHandshakeClientKeyExchange(kstream.data)
        ))

        #Send certificate verify data
        self.handshake_proto.send(data.TlsHandshakeMessage(
            data.TlsHandshakeCertificateVerify(self.remote_key.priv_key.sign(self.handshake_proto.msg_digest.digest(), ecc.ECDSA(utils.Prehashed(hashes.SHA256()))))
        ))

        #Calculate premaster secret
        self.premaster_secret = eph_priv_key.exchange(ecc.ECDH(), self.remote_key.remote_cert.pub_key)

    def create_encryption_algo(self) -> TlsEncryptionAlgorithm: raise NotImplementedError()

#BROKEN Even though it advertises as using SHA1, the Windows driver uses SHA256
@data.tls_cipher_suite(0xc005)
class TlsEccAes256CbcSha1CipherSuite(TlsEcdhKeyExchangeAlgorithm):
    def __init__(self, remote_key : TlsEccRemoteKey):
        super().__init__(remote_key)
        self.hash_algo = hashes.SHA256()

    def create_encryption_algo(self) -> TlsEncryptionAlgorithm:
        assert not self.in_handshake
        assert self.premaster_secret != None
        return TlsBlockEncryptionAlgorithm(self.premaster_secret, self.handshake_proto.client_random, self.handshake_proto.server_random, cipher.algorithms.AES, cipher.modes.CBC, hashes.SHA256, 32, 32)

@data.tls_cipher_suite(0xc02e)
class TlsEccAes256GcmSha384CipherSuite(TlsEcdhKeyExchangeAlgorithm):
    def __init__(self, remote_key : TlsEccRemoteKey):
        super().__init__(remote_key)
        self.hash_algo = hashes.SHA384()

    def create_encryption_algo(self) -> TlsEncryptionAlgorithm:
        assert not self.in_handshake
        assert self.premaster_secret != None
        return TlsAEADEncryptionAlgorithm(self.premaster_secret, self.handshake_proto.client_random, self.handshake_proto.server_random, cipher.algorithms.AES, cipher.modes.GCM, hashes.SHA384, 32, 4, 16)

class TlsEccRemoteKey(TlsRemoteKey):
    def __init__(self, priv_key : ecc.EllipticCurvePrivateKey, client_cert : tudor.sensor.SensorCertificate, remote_cert : tudor.sensor.SensorCertificate):
        assert isinstance(priv_key.curve, ecc.SECP256R1)
        self.priv_key = priv_key
        self.client_cert = client_cert
        self.remote_cert = remote_cert
        
        self.cipher_suites = [TlsEccAes256CbcSha1CipherSuite(self), TlsEccAes256GcmSha384CipherSuite(self)]