from sys import argv, exit
from cryptography.hazmat.primitives.hmac import *
from cryptography.hazmat.primitives.hashes import *
from cryptography.hazmat.primitives.asymmetric.ec import *
from cryptography.hazmat.primitives.serialization import *

#Used for generating the HS key located inside of this folder (hskey.pem)
#It's used during pairing for signing the host's certificate
#This generation algorithm is a copy of the Windows driver one

if len(argv) < 2:
    print("Usage: %s <output key file>" % argv[0])
    exit()

#Hardcoded parameters
secret = bytes.fromhex("717cd72d0962bc4a2846138dbb2c2419")
prf_input = bytes.fromhex("2512a76407065f383846139d4bec2033aaaa")
prf_label = "HS_KEY_PAIR_GEN"

#Run TLS1.2 PRF with SHA256 to generate private key secret
prf_input = prf_label.encode("ascii") + prf_input

prf_output = bytes()
prf_a = prf_input
while len(prf_output) < 32:
    #Update A
    h = HMAC(secret, SHA256())
    h.update(prf_a)
    prf_a = h.finalize()

    #Update output
    h = HMAC(secret, SHA256())
    h.update(prf_a)
    h.update(prf_input)
    prf_output += h.finalize()
prf_output = prf_output[:32]

#Parse PRF output as private key
priv_key = derive_private_key(int.from_bytes(prf_output, "little"), SECP256R1())

#Write private key to file
with open(argv[1], "wb") as f:
    f.write(priv_key.private_bytes(Encoding.PEM, PrivateFormat.TraditionalOpenSSL, NoEncryption()))