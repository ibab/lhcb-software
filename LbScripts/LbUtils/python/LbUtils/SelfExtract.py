from LbUtils.Tar import createTarBall
from LbUtils.Temporary import TempFile

try:
    from base64 import standard_b64encode as encode
except ImportError:
    from base64 import encode
    





def createSelfExtractFile(filename, sourcedir, execscript):
    tarball = TempFile()
    createTarBall(tarball.name, sourcedir)
    tbstr = encode(open(tarball.name, "r").read())
    del tarball
    f = open(filename,"wb+")
    f.write(_script_body % (execscript, tbstr))
    f.close()




_script_body = """ 
#!/usr/bin/env python

import os
from tempfile import mkdtemp, mkstemp
try:
    from base64 import standard_b64decode as decode
except ImportError:
    from base64 import decode

if __name__ == '__main__':
    pass






_sef_script = "%s"
_sef_tarball = "%s"




"""
