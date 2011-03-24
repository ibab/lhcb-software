from LbUtils.Tar import createTarBall
from LbUtils.Temporary import TempFile
from LbUtils.File import genTemplateFile

try:
    from base64 import standard_b64encode as encode
    from base64 import standard_b64decode as decode
except ImportError:
    from base64 import encode
    from base64 import decode

import os

supported_compression = ["gzip", "bzip", "plain"]

default_template = os.path.join(os.environ.get("LBUTILSROOT", ""), "data", "SelfExtract.tmpl")

def createSelfExtractFile(input_dir, target_filename, description, script=None, script_args=None,
                          compression_type="gzip", target_dir=None, use_temp_dir=True,
                          template=None):
    status = 0
    if not template :
        template = default_template

    tmpl_dict = {}
    tmpl_dict["script"] = script
    tarball = TempFile()
    createTarBall(tarball.name, input_dir)
    tmpl_dict["tarball"] = encode(open(tarball.name, "r").read())
    del tarball
    tmpl_file = os.path.join(os.environ["LBUTILSROOT"], "data", "SelfExtract-header.tmpl")
    genTemplateFile(tmpl_file, tmpl_dict, target_filename)

    return status
