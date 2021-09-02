import os
from distutils.core import setup

def list_files(d):
    fs = []
    for (root, dirs, files) in os.walk(d):
        for f in files: fs.append(os.path.join(root, f))
    return fs

setup(name="tudor", version="1.0.0", packages=["tudor"], package_data={'': list_files("tudor")}, python_requires='>3',install_requires=["cryptography", "pyusb", "matplotlib"])