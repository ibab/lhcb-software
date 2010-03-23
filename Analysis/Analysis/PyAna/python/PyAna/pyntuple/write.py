'''
'''

__author__ = 'Juan Palacios <juan.palacios@nikhef.nl>'
__version__ = '1.0'

__all__ = ('gzip_save', 'save')

def gzip_save(obj, filename = "file.ntp") :
    import gzip
    import cPickle
    file = gzip.open(filename, 'wb')
    cPickle.dump(obj, file, 2)
    file.close()

def save(obj, filename = "file.ntp") :
    import pickle
    file = open(filename, 'w')
    pickle.dump(obj, file)
    file.close()
