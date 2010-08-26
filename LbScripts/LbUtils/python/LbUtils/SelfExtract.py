from LbUtils.Tar import createTarBall
from LbUtils.Temporary import TempFile
from LbUtils.File import genTemplateFile

try:
    from base64 import standard_b64encode as encode
except ImportError:
    from base64 import encode
    
import os

def createSelfExtractFile(filename, sourcedir, execscript):
    tmpl_dict = {}
    tmpl_dict["script"] = execscript
    tarball = TempFile()
    createTarBall(tarball.name, sourcedir)
    tmpl_dict["tarball"] = encode(open(tarball.name, "r").read())
    del tarball
    tmpl_file = os.path.join(os.environ["LBUTILSROOT"], "data", "SelfExtract-header.tmpl")
    genTemplateFile(tmpl_file, tmpl_dict, filename)

