from __future__ import annotations

import struct

class WinTagValContainer:
    def __init__(self, vals : dict = None): self.vals = dict(vals) if vals != None else dict()
    def __contains__(self, tag : int): return tag in self.vals
    def __getitem__(self, tag : int): return self.vals[tag]
    def __setitem__(self, tag : int, data : bytes): self.vals[tag] = data
    def __delitem__(self, tag : int): del self.vals[tag]

    @staticmethod
    def frombytes(data : bytes) -> WinTagValContainer:
        vals = {}
        while len(data) > 0:
            tag, val_size = struct.unpack("<HI", data[:6])
            assert not tag in vals
            vals[tag] = data[6:6+val_size]

            data = data[6+val_size:]
        return WinTagValContainer(vals)        

    def tobytes(self) -> bytes:
        data = bytes()
        for tag, val in self.vals.items(): data += struct.pack("<HI", tag, len(val)) + val
        return data