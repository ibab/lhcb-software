import os
import cPickle as pickle
import logging

_defaultcache = None


class File(object):
    def __init__(self, application=None, name=None):
        self._application = application
        self._name = name
        self._tobeupdated = False
        self._filename = None
        global _defaultcache
        if _defaultcache is not self :
            _defaultcache = self
    def fileName(self):
        if not self._filename :
            pth = os.path.join(os.environ["HOME"], ".cache")
            if self._application :
                pth = os.path.join(pth,self._application)
            if not os.path.exists(pth):
                os.makedirs(pth)
            if not self._name :
                self._name = "default"
            self._filename = os.path.join(pth, "%s.pkl" % self._name) 
        return self._filename
    def dump(self, *objs):
        log = logging.getLogger()
        f = open(self.fileName(), "w")
        for o in objs:
            pickle.dump(o, f)
            log.info("Cache dumping to %s" % self.fileName())
        f.close()
    def needsUpdate(self):
        return self._tobeupdated
    def toBeUpdated(self):
        self._tobeupdated = True
    def load(self):
        log = logging.getLogger()
        log.info("Cache Load from %s" % self.fileName())
        f = open(self.fileName(), "r")
        while True:
            try:
                yield pickle.load(f)
            except EOFError:
                break
        f.close()

def getDefaultCache():
    global _defaultcache
    if not _defaultcache :
        _defaultcache = File()
    return _defaultcache

