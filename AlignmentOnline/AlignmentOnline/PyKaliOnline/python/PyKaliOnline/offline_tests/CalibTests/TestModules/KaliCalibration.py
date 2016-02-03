class Iterator(object):
    ## This module can either return a python string or a function that takes no
    ## arguments and returns the correct string.
    def __init__(self, directory):
        self.__directory = directory

    def __call__(self):
        python_string = "from PyKaliOnline import Iterator; Iterator.run('{0}')"
        return python_string.format(self.__directory)

    def gaudi(self):
        return False

class Analyzer(object):
    ## This module can either return a python string or a function that takes a
    ## single integer argument and returns the correct string.
    def __init__(self, input_files):
        self.__input_files = input_files

    def __call__(self,n):
        python_string = "from PyKaliOnline import Analyzer; Analyzer.run({0},'{1}')"
        return python_string.format(n,self.__input_files[n])

    def gaudi(self):
        return False
