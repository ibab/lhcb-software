import os
import cPickle as pickle
import logging

class File(object):
    def __init__(self, application, name):
        self._application = application
        self._name = name
    def fileName(self):
        pth = os.path.join(os.environ["HOME"], ".cache", self._application)
        if not os.path.exists(pth):
            os.makedirs(pth)
        return os.path.join(pth, "%s.pkl" % self._name)
    def dump(self, *objs):
        log = logging.getLogger()
        f = open(self.fileName(), "w")
        for o in objs:
            pickle.dump(o, f)
            log.info("Cache dumping to %s" % self.fileName())
        f.close()
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
            