from .context import *

class Command:
    commands = {}
    def run(self, ctx : CmdContext, args : list): raise NotImplementedError()

def cmd(name : str):
    def wrapper(ctype : type):
        Command.commands[name] = ctype()
        return ctype
    return wrapper