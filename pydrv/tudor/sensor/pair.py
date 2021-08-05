from __future__ import annotations

import io
import struct
import pathlib
import cryptography.hazmat.primitives.asymmetric.ec as ecc
import cryptography.hazmat.primitives.hashes as hashes
import cryptography.hazmat.primitives.serialization as ser
import cryptography.x509 as x509

#It's called that in the Windows driver
def load_hs_key() -> ecc.EllipticCurvePrivateKey:
    with open(str(pathlib.Path(__file__).parent) + "/sensor_keys/hskey.pem", "rb") as f:
        hs_key = ser.load_pem_private_key(f.read(), None)
        assert isinstance(hs_key, ecc.EllipticCurvePrivateKey)
        assert isinstance(hs_key.curve, ecc.SECP256R1)
        return hs_key

class SensorCertificate:
    def __init__(self, cert_type : int, pub_key : ecc.EllipticCurvePublicKey, signature : bytes):
        self.cert_type = cert_type
        self.pub_key = pub_key
        self.signature = signature

    @staticmethod
    def create_host_cert(pub_key : ecc.EllipticCurvePublicKey):
        assert isinstance(pub_key.curve, ecc.SECP256R1)

        #Generate certificate with bogus signature
        cert = SensorCertificate(0, pub_key, bytes())

        #Manually generate signature field using HS key
        cert.signature = load_hs_key().sign(cert.signbytes(), ecc.ECDSA(hashes.SHA256()))

        return cert

    @staticmethod
    def frombytes(data : bytes):
        assert len(data) == 400
        magic, curve, pub_x, pub_y, cert_type, sign_size, sign = struct.unpack("<HH68s68sxBH256s", data)
        assert magic == 0x5f3f
        assert curve == 23
        
        return SensorCertificate(cert_type,
            ecc.EllipticCurvePublicNumbers(int.from_bytes(pub_x, "little"), int.from_bytes(pub_y, "little"), ecc.SECP256R1()).public_key(),
            sign[:sign_size]
        )

    def tobytes(self) -> bytes:
        return struct.pack("<HH68s68sxBH256s", 
            0x5f3f, 23, 
            self.pub_key.public_numbers().x.to_bytes(68, "little"), self.pub_key.public_numbers().y.to_bytes(68, "little"),
            self.cert_type, len(self.signature), self.signature
        )
    
    def signbytes(self) -> bytes:
        return struct.pack("HH68s68sxB", 
            0x5f3f, 23, 
            self.pub_key.public_numbers().x.to_bytes(68, "little"), self.pub_key.public_numbers().y.to_bytes(68, "little"),
            self.cert_type
        )

class SensorPairingData:
    def __init__(self, priv_key : ecc.EllipticCurvePrivateKey, host_cert : SensorCertificate, sensor_cert : SensorCertificate):
        assert isinstance(priv_key.curve, ecc.SECP256R1)
        self.priv_key = priv_key
        self.host_cert = host_cert
        self.sensor_cert = sensor_cert

    @staticmethod
    def load(bio : io.BytesIO) -> SensorPairingData:
        #Load private key
        priv_key = ecc.derive_private_key(int.from_bytes(bio.read(0x44), "little"), ecc.SECP256R1())

        #Load host certificate
        host_cert = SensorCertificate.frombytes(bio.read(400))

        #Load sensor certificate
        sensor_cert = SensorCertificate.frombytes(bio.read(400))

        return SensorPairingData(priv_key, host_cert, sensor_cert)

    def save(self, bio : io.BytesIO):
        #Save private key
        bio.write(self.priv_key.private_numbers().private_value.to_bytes(0x44, "little"))

        #Save host certificate
        assert len(self.host_cert.tobytes()) == 400
        bio.write(self.host_cert.tobytes())

        #Save sensor certificate
        assert len(self.sensor_cert.tobytes()) == 400
        bio.write(self.sensor_cert.tobytes())