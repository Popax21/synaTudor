from __future__ import annotations

from .stream import *

class TlsDataType:
    def read(self, stream : TlsDataReadStream) -> TlsDataInstance: raise NotImplementedError()

    def convert(self, instance : TlsDataInstance) -> TlsDataInstance:
        assert instance.data_type == self
        return instance

class TlsDataInstance:
    data_type : TlsDataType
    def write(self, stream : TlsDataWriteStream): raise NotImplementedError()

class TlsData(TlsDataInstance):
    class metaclass(type, TlsDataType): 
        def read(self, stream : TlsDataReadStream): raise NotImplementedError()
    __metaclass__ = metaclass

    def __init__(self): self.data_type = self.__class__

    @staticmethod
    def read(stream : TlsDataReadStream) -> TlsData: raise NotImplementedError()
    
    def write(self, stream : TlsDataWriteStream): raise NotImplementedError()

    @classmethod
    def convert(cls, instance : TlsData) -> TlsData:
        assert isinstance(instance, cls)
        return instance

class TlsIntegerType(TlsDataType):
    def __init__(self, size : int):
        self.size = size

    def __repr__(self): return "Integer(size=%d)" % self.size

    def read(self, stream : TlsDataReadStream):
        return TlsInteger(stream.read_int(self.size), int_type=self)

    def convert(self, i : int) -> TlsInteger:
        assert isinstance(i, int)
        return TlsInteger(i, int_type=self)

class TlsInteger(int, TlsDataInstance):
    data_type : TlsIntegerType

    def __new__(cls, *args, **kwargs):
        itype = kwargs["int_type"]
        del kwargs["int_type"]
        i = int.__new__(cls, *args, **kwargs)
        i.data_type : TlsIntegerType = itype
        return i

    def write(self, stream : TlsDataWriteStream):
        stream.write_int(self.data_type.size, self)

class TlsVectorType(TlsDataType):
    def __init__(self, inner_type : TlsDataType, **kwargs):
        self.inner_type = inner_type
        self.fixed_size : int = kwargs["fixed_size"] if "fixed_size" in kwargs else None
        self.min_size : int = kwargs["min_size"] if "min_size" in kwargs else None
        self.max_size : int = kwargs["max_size"] if "max_size" in kwargs else None
        
        if self.fixed_size != None: assert self.min_size == None and self.max_size == None
        else: 
            assert self.max_size != None
            self.size_size = self.max_size.bit_length() // 8 + (self.max_size.bit_length() % 8 > 0)

    def __repr__(self): 
        s = "Vector(inner=%r" % self.inner_type
        if self.fixed_size != None: s += " size=0x%x" % self.fixed_size
        if self.min_size != None: s += " min_size=0x%x" % self.min_size
        if self.max_size != None: s += " max_size=0x%x" % self.max_size
        return s + ")"

    def read(self, stream : TlsDataReadStream): return TlsVector.read(stream, self)
    def convert(self, vec : TlsVector) -> TlsVector:
        assert isinstance(vec.data_type, TlsVector)
        return TlsVector(self, [self.inner_type.convert(e) for e in vec.elements])

class TlsVector(TlsDataInstance):
    def __init__(self, vec_type : TlsVectorType, elements : list):
        self.data_type = vec_type
        self.elements = elements
    
    def __getitem__(self, idx): return self.elements[idx]
    def __setitem__(self, idx, val): self.elements[idx] = val
    def __len__(self): return len(self.elements)
    def __str__(self): return str(self.elements)
    def __repr__(self): return repr(self.elements)

    @staticmethod
    def read(stream : TlsDataReadStream, vec_type : TlsVectorType):
        #Get vector size
        size = vec_type.fixed_size if vec_type.fixed_size != None else stream.read_int(vec_type.size_size)
        if vec_type.min_size != None: assert vec_type.min_size <= size
        if vec_type.max_size != None: assert size <= vec_type.max_size

        #Create substream
        estream = stream.create_substream(size)

        #Read elements from substream
        elements = []
        while len(estream) > 0: elements.append(vec_type.inner_type.read(estream))

        return TlsVector(vec_type, elements)

    def write(self, stream : TlsDataWriteStream):
        #Write elements to element substream
        estream = TlsDataWriteStream()
        for e in self.elements: 
            self.data_type.inner_type.convert(e).write(estream)

        #Write elements size
        if self.data_type.fixed_size != None: assert len(estream) == self.data_type.fixed_size
        if self.data_type.min_size != None: assert self.data_type.min_size <= len(estream)
        if self.data_type.max_size != None: assert len(estream) <= self.data_type.max_size

        if self.data_type.size_size != None: 
            stream.write_int(self.data_type.size_size, len(estream))

        #Write elements data
        stream.write(estream.data)