#!/usr/bin/env python
""" install_project.py post install script for the LbScripts project """

import sys
import os

# bootstraping the location of the file
_this_file = os.path.abspath(sys.argv[0]) # this very file full location
_cmt_dir = os.path.dirname(_this_file) # location of the parent directory to this file aka "cmt"
_sys_dir = os.path.dirname(_cmt_dir) # location of the Sys package aka <project>Sys
_prj_dir = os.path.dirname(_sys_dir) # location of the project instance
_ia_dir = os.path.join(_prj_dir, "InstallArea" ) # location of the project InstallArea
_iapy_dir = os.path.join(_ia_dir, "python") # location of the python dir in the IA
_iascr_dir = os.path.join(_ia_dir, "scripts") # location of the scripts dir in the IA

if os.path.isdir(_iapy_dir) : # in order to use my own tools
    sys.path.insert(0, _iapy_dir)

from LbUtils.Script import Script
from LbUtils.Processes import callCommand
from LbUtils.File import removeTree

import logging
import shutil

__version__ = "$Id$"

class PostInstallScript(Script):
    _version = __version__
    _description = __doc__
    def __init__(self, usage):
        Script.__init__(self, usage=usage)
        self._mysiteroot = os.environ.get("MYSITEROOT", None)
        self._scripts_version = None
        self.scriptVersion()
        self._prodlink = os.path.join(os.path.dirname(_prj_dir), "prod")

    def defineOpts(self):
        parser = self.parser

    def scriptVersion(self):
        """
        Extract the version of LbScripts from the requirements file.
        """
        if not self._scripts_version :
            for l in  open(os.path.join(_cmt_dir, "requirements")).readlines() :
                l = l.strip()
                if l.startswith("version") :
                    self._scripts_version = l.split()[1]
        return self._scripts_version

    def generateScripts(self):
        rc = 0
        opts = self.options
        log = logging.getLogger()
        if not self._mysiteroot :
            log.fatal("Cannot find a valid MYSITEROOT environment variable.")
            rc = 1
            return rc

        if not self._scripts_version :
            log.fatal("Cannot find the version of LbScripts")
            rc = 2
            return rc

        genlogscript = os.path.join(_ia_dir, "scripts", "generateLogin")
        gencmd = [ genlogscript ]
        if opts.log_level == "DEBUG" :
            gencmd.append("--debug")
        gencmd.append("--without-python")
        gencmd.append("--no-cache")
        gencmd.append("-m %s" % self._mysiteroot)
        gencmd.append("--login-version=%s" % self._scripts_version)
        gencmd.append(_ia_dir)
        log.info("Running: python %s" % gencmd)
        rc = callCommand("python %s" % " ".join(gencmd), report_failure=True)

        return rc

    def createProdLink(self):
        """
        create link called "prod" for the latest release of LbScripts
        """
        log = logging.getLogger()
        rc = 0
        prodlink = self._prodlink
        if sys.platform != "win32" :
            if not os.path.exists(prodlink) : # fix for broken links (exists dereferences)
                if os.path.islink(prodlink) :
                    os.remove(prodlink)
            if os.path.exists(prodlink) :
                if os.path.islink(prodlink) :
                    os.remove(prodlink)
                    os.symlink("LBSCRIPTS" + "_" + self._scripts_version, prodlink)
                    log.debug("linking %s -> %s" % (prodlink, "LBSCRIPTS" + '_' + self._scripts_version))
                else :
                    log.error("%s is not a link. Please remove this file/directory" % prodlink)
                    rc = 3

        else :
            if os.path.exists(prodlink) :
                if os.path.isdir(prodlink) :
                    removeTree(prodlink)
                    shutil.copytree("LBSCRIPTS" + "_" + self._scripts_version, prodlink)
                    log.debug("Copying %s to %s" % ("LBSCRIPTS" + "_" + self._scripts_version, prodlink))
                else :
                    log.error("%s is not a directory. Please remove this file" % prodlink)
                    rc = 3

        return rc


    def createTopLinks(self):
        log = logging.getLogger()
        rc = 0
        my_dir = self._mysiteroot.split(os.pathsep)[0]
        prodlink = self._prodlink
        selected_script_dir = os.path.join(prodlink, "InstallArea", "scripts")
        if not os.path.exists(selected_script_dir) :
            selected_script_dir = os.path.join(_prj_dir, "InstallArea", "scripts")
        for f in os.listdir(selected_script_dir) :
            if f.startswith("LbLogin.") and not (f.endswith(".zsh") or f.endswith(".py")):
                sourcef = os.path.join(selected_script_dir, f)
                targetf = os.path.join(my_dir, f)
                if os.path.islink(targetf) or os.path.isfile(targetf):
                    os.remove(targetf)
                if sys.platform == "win32" :
                    shutil.copy(sourcef, targetf)
                    log.debug("copying %s into %s" % (sourcef, targetf))
                else :
                    sourcef = sourcef.replace(my_dir, "", 1)
                    while sourcef.startswith("/") or sourcef.startswith("\\") :
                        sourcef = sourcef[1:]
                    os.symlink(sourcef, targetf)
                    log.debug("linking %s -> %s" % (targetf, sourcef))
        return rc

    def createEtcLinks(self):
        log = logging.getLogger()
        rc = 0
        etc_scripts = [ "LbLogin", "group_login", "group_shell", "LHCb"]
        my_etc_dir = os.path.join(self._mysiteroot.split(os.pathsep)[0], "etc")
        prodlink = self._prodlink
        selected_script_dir = os.path.join(prodlink, "InstallArea", "scripts")
        if not os.path.exists(selected_script_dir) :
            selected_script_dir = os.path.join(_prj_dir, "InstallArea", "scripts")
        for s in etc_scripts :
            for f in os.listdir(selected_script_dir) :
                if f.startswith("%s." % s) and not (f.endswith(".zsh") or f.endswith(".py")):
                    sourcef = os.path.join(selected_script_dir, f)
                    targetf = os.path.join(my_etc_dir, f)
                    if os.path.islink(targetf) or os.path.isfile(targetf):
                        os.remove(targetf)
                    if sys.platform == "win32" :
                        shutil.copy(sourcef, targetf)
                        log.debug("copying %s into %s" % (sourcef, targetf))
                    else :
                        sourcef = sourcef.replace(my_dir, "", 1)
                        while sourcef.startswith("/") or sourcef.startswith("\\") :
                            sourcef = sourcef[1:]
                        sourcef = os.path.join(os.pardir, sourcef)
                        os.symlink(sourcef, targetf)
                        log.debug("linking %s -> %s" % (targetf, sourcef))
        return rc

    def main(self):
        """
        Core of the script
        """
        opts = self.options
        args = self.args
        log = logging.getLogger()

        rc = self.generateScripts()
        if rc != 0 :
            return rc

        rc = self.createProdLink()
        if rc != 0 :
            return rc

        rc = self.createTopLinks()
        if rc != 0 :
            return rc

        rc = self.createEtcLinks()
        if rc != 0 :
            return rc

        return 0

if __name__ == '__main__':
    s = PostInstallScript(usage="python %prog [options]")
    sys.exit(s.run())

