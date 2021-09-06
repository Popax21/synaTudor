#!/usr/bin/env python3

import os
import sys
import urllib
import urllib.request
import re

error = False

try:
    response = urllib.request.urlopen('https://gitlab.freedesktop.org/libfprint/wiki/-/wikis/Unsupported-Devices.md')
except:
    print("Could not download current list of unsupported devices, skipping test.")
    sys.exit(77)
data = response.read().decode('utf-8')

devices = []
devices_re = re.compile(r'^.*([0-9a-fA-F]{4}):([0-9a-fA-F]{4}).*$', re.MULTILINE)
for m in devices_re.finditer(data):
    vid = m.group(1)
    pid = m.group(2)
    devices.append((vid, pid))

generator = open(os.path.join(os.path.dirname(__file__), '..', 'libfprint', 'fprint-list-udev-hwdb.c')).read()

id_re = re.compile('  { .vid = 0x([a-fA-F0-9]*), .pid = 0x([a-fA-F0-9]*) }')
# Check everything is the same
started = False
for l in generator.split('\n'):
    m = id_re.match(l)
    if m is None:
        # Stop on the first line that does not match anymore
        if started:
            break
        continue
    else:
        started = True

    vid_pid = (m.group(1), m.group(2))
    try:
        devices.remove(vid_pid)
    except ValueError:
        print("Generator has entry that is not on wiki: {}:{}".format(*vid_pid))
        error = True

for vid_pid in devices:
    print("New entry from wiki is missing: {}:{}".format(*vid_pid))
    error = True

if error:
    sys.exit(1)
else:
    sys.exit(0)
