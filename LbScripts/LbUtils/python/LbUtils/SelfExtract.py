# $Id$
# module to create self extracting script. Based on the shell
# makeself application (http://megastep.org/makeself)
#
# @author: Hubert Degaudenzi

from LbUtils.Tar import createTarBall
from LbUtils.Temporary import TempFile
from LbUtils.File import genTemplateFile

import logging

try:
    from base64 import standard_b64encode as encode
    from base64 import standard_b64decode as decode
except ImportError:
    from base64 import encode, decode


import os

supported_compression = ["gzip", "bzip", "plain"]

default_template = os.path.join(os.environ.get("LBUTILSROOT", ""), "data", "SelfExtract.tmpl")

def createSelfExtractFile(input_dir, target_filename, description, script=None, script_args=None,
                          compression_type="gzip", target_dir=None, use_temp_dir=True,
                          template=None, update_url=None, script_version=None):
    """ function to create a self extracting script.
    @return: status code for the whole process 0: success other:failed
    @param input_dir: directory to be tarred and placed at the end of the script as a string
    @param target_filename: name for the self extracting script
    @param description: short description of the self extracting script
    @param script: internal script to be called after the untar. the path must be relative to the
                   root of the input_dir
    @param script_args: arguments to the internal scripts
    @param compression_type: type of compression to be used for the tar payload
    @param target_dir: name of the directory where the untarring is done
    @param use_temp_dir: usage of a temporary directory or not.
    @param template: template file to be used as a base for the self extracting script
    @param update_url: url to self update the self extracting script
    @param script_version: sets the version of the self extracting script
    """

    log = logging.getLogger()

    status = 0
    if not template :
        template = default_template

    tmpl_dict = {}

    if not os.path.exists(os.path.join(input_dir, script)) :
        status = 1
        log.error("The file %s doesn't exist in the %s directory" % (script, input_dir))
        return status
    tmpl_dict["script"] = script

    tarball = TempFile()
    createTarBall(tarball.name, input_dir)
    tmpl_dict["tarball"] = encode(open(tarball.name, "r").read())
    del tarball

    tmpl_dict["description"] = description

    genTemplateFile(template, tmpl_dict, target_filename)

    return status
