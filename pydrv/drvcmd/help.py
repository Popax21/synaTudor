import inspect

from .cmd import *

@cmd("help")
class CmdHelp(Command):
    """
    Prints info about commands
    Usage: help [command]
    """

    def run(self, _, args : list):
        if len(args) <= 0:
            print("Available commands:")
            print(" ".join(Command.commands.keys()))
        else:
            print(inspect.cleandoc(Command.commands[args[0].lower().strip()].__doc__).strip())
