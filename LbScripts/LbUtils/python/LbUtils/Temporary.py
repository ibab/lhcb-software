""" create temporary structure for remote buids """

from shutil import rmtree

import os
import tempfile
if not 'mkdtemp' in dir(tempfile):
    # mkdtemp has been introduced in python 2.3, I simulate it
    import warnings
    warnings.filterwarnings(action = 'ignore', message = '.*tmpnam.*', category = RuntimeWarning)
    def mkdtemp():
        name = os.tmpnam()
        os.mkdir(name,0700)
        return name
    def mkstemp():
        name = os.tmpnam()
        return (os.open(name,os.O_CREAT | os.O_RDWR | os.O_EXCL, 0600), 
name)
else:
    # use the real mkdtemp
    from tempfile import mkdtemp, mkstemp


class TempDir(object):
    """ class to create a temporary directory """
    def __init__(self, basename=None):
        """ constructor """
        if (basename):
            self._name = mkdtemp("tempdir", basename)
        else:
            self._name = mkdtemp("tempdir")
    def __del__(self):
        """ destructor """
        rmtree(self._name)
    def __str__(self):
        """ returns string representation """
        return self.getName()
    def getName(self):
        """returns the name of the temporary directory"""
        return self._name


class TempFile:
    """ class to create a temporary file """
    def __init__(self):
        """ Constructor """
        self._file = None
        self.name = ''
        
        fd, name = mkstemp()
        self.name = name
        self._file = os.fdopen(fd,"w+")
                
    def __del__(self):
        """ Destructor """
        if self._file:
            self._file.close()
            os.remove(self.name)

    def __getattr__(self,attr):
        return getattr(self._file,attr)

