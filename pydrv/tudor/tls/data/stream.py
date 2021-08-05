from __future__ import annotations

class TlsDataReadStream:
    def __init__(self, data : bytes):
        self.data = data
    
    def read(self, size : int):
        if size > len(self.data): raise EOFError()
        d = self.data[:size]
        self.data = self.data[size:]
        return d
    
    def read_int(self, size : int):
        return int.from_bytes(self.read(size), "big")

    def create_substream(self, size : int):
        return TlsDataReadStream(self.read(size))

    def __len__(self): return len(self.data)
    
class TlsDataWriteStream:
    def __init__(self):
        self.data = bytes()
    
    def write(self, data : bytes):
        self.data += data
    
    def write_int(self, size : int, val : int):
        assert 0 <= val and val < 256**size
        self.write(val.to_bytes(size, "big"))
    
    def __len__(self): return len(self.data)

class TlsDataPipeStream(TlsDataReadStream, TlsDataWriteStream):
    def __init__(self):
        TlsDataWriteStream.__init__(self)
        TlsDataReadStream.__init__(self, self.data)

    def reset(self):
        d = self.data
        self.data = bytes()
        return d