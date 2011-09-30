# $Id$
# module to create self extracting script. Based on the shell
# makeself application (http://megastep.org/makeself)
#
# @author: Hubert Degaudenzi

from LbUtils.Tar import createTarBall, supported_compression
from LbUtils.Temporary import TempFile
from LbUtils.File import genTemplateFile, computeMD5Sum

import logging

try:
    from base64 import standard_b64encode as encode
    from base64 import standard_b64decode as decode
except ImportError:
    from base64 import encode, decode #@UnusedImport


import os




default_template = os.path.join(os.environ.get("LBUTILSROOT", ""), "data", "SelfExtract.tmpl")

def createSelfExtractFile(input_dir, target_filename, description="", script="", script_args="",
                          compression_type=supported_compression[0], target_dir="", use_temp_dir=True,
                          template=None, update_url="", script_version="",
                          use_sha1_check=True, use_md5_check=False):
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

    tmpl_dict["description"] = description

    if script and not os.path.exists(os.path.join(input_dir, script)) :
        status = 1
        log.error("The file %s doesn't exist in the %s directory" % (script, input_dir))
        return status
    tmpl_dict["script"] = script
    tmpl_dict["script_args"] = script_args

    tmpl_dict["compression_type"] = compression_type

    tmpl_dict["target_dir"] = target_dir

    tmpl_dict["use_temp"] = str(use_temp_dir)

    tmpl_dict["update_url"] = update_url

    tmpl_dict["version"] = script_version

    tarball = TempFile()
    createTarBall(tarball.name, input_dir, compression_type=compression_type)
    tmpl_dict["tarball"] = encode(open(tarball.name, "r").read())
    tmpl_dict["md5_sum"] = ""
    if use_md5_check :
        tmpl_dict["md5_sum"] = computeMD5Sum(tarball.name)

    del tarball



    genTemplateFile(template, tmpl_dict, target_filename)

    return status
