""" General LHCb platform configuration """

import sys, platform, os
import re
# CMTCONFIG extraction

def isNewStyleBinary(cmtconfig):
    """ check if the CMTCONFIG value is new styled """
    newstyle = False
    if len(cmtconfig.split("-")) > 1 :
        newstyle = True
    return newstyle

def isBinaryDbg(cmtconfig):
    """ check if the CMTCONFIG value is a debug one """
    bindbg = True
    if isNewStyleBinary(cmtconfig) :
        if not cmtconfig.endswith("-dbg") :
            bindbg = False
    else :
        if not cmtconfig.endswith("_dbg") :
            bindbg = False
    return bindbg

def isBinaryOpt(cmtconfig):
    """ check if the CMTCONFIG value is an optimized one """
    binopt = True
    if isBinaryDbg(cmtconfig) :
        binopt = False
    return binopt


def getBinaryDbg(cmtconfig):
    """ convert CMTCONFIG to debug """
    cmtdbg = cmtconfig
    if not isBinaryDbg(cmtconfig) :
        if isNewStyleBinary(cmtconfig) :
            if cmtconfig.endswith("-opt") :
                cmtdbg = "-".join(cmtconfig.split("-")[:-1]) + "-dbg"
            else :
                cmtdbg += "-dbg"
        else :
            cmtdbg += "_dbg"
    return cmtdbg

def getBinaryOpt(cmtconfig):
    """ convert CMTCONFIG to optimized """
    cmtopt = cmtconfig
    if isBinaryDbg(cmtconfig) :
        if isNewStyleBinary(cmtconfig) :
            cmtopt = "-".join(cmtconfig.split("-")[:-1]) + "-opt"
        else :
            cmtopt = "_".join(cmtconfig.split("_")[:-1])
    return cmtopt

def getCompiler(cmtconfig):
    """ extract compiler from CMTCONFIG """
    compdef = None
    if isNewStyleBinary(cmtconfig) :
        compdef = cmtconfig.split("-")[2]
    else :
        if not cmtconfig.startswith("win") :
            compdef = cmtconfig.split("_")[2]
        else :
            compdef = cmtconfig.split("_")[1]
    return compdef

def getPlatformType(cmtconfig):
    """ extract platform type (slc5, slc4, etc) from CMTCONFIG """
    platformtype = None
    if isNewStyleBinary(cmtconfig) :
        platformtype = cmtconfig.split("-")[1]
    else :
        platformtype = cmtconfig.split("_")[0]
    if platformtype == "sl5" :
        platformtype = "slc5"
    if platformtype == "sl4" :
        platformtype = "slc4"
    if platformtype == "sl3" :
        platformtype = "slc3"
    return platformtype


def getArchitecture(cmtconfig):
    """ extract architecture from CMTCONFIG """
    architecture = None
    if isNewStyleBinary(cmtconfig) :
        architecture = cmtconfig.split("-")[0]
        if architecture == "ia32" :
            architecture = "i686"
        if architecture == "amd64" :
            architecture = "x86_64"
    else :
        archlist = cmtconfig.split("_")
        if not archlist[0].startswith("win") :
            architecture = archlist[1]
            if architecture == "i686" :
                architecture = "ia32"
            if architecture == "x86_64" :
                architecture = "i686"
    return architecture

def getConfig(architecture, platformtype, compiler, debug=False):
    cmtconfig = None
    if platformtype == "slc5" :
        if architecture == "ia32" :
            architecture = "i686"
        if architecture == "amd64" :
            architecture = "x86_64"
        if debug :
            cmtconfig = "-".join([architecture, platformtype, compiler, "dbg"])
        else :
            cmtconfig = "-".join([architecture, platformtype, compiler, "opt"])
    elif not platformtype.startswith("win") :
        if architecture == "i686" :
            architecture = "ia32"
        if architecture == "x86_64" :
            architecture = "i686"
        cmtconfig = "_".join([platformtype, architecture, compiler])
        if debug :
            cmtconfig += "_dbg"
    else :
        cmtconfig = "_".join([platformtype, compiler])
        if debug :
            cmtconfig += "_dbg"
    return cmtconfig

binary_opt_list = ["slc3_ia32_gcc323",
                   "slc4_ia32_gcc34", "slc4_amd64_gcc34",
                   "x86_64-slc5-gcc34-opt", "i686-slc5-gcc34-opt",
                   "x86_64-slc5-gcc43-opt", "i686-slc5-gcc43-opt",
                   "win32_vc71"]

binary_dbg_list = [ getBinaryDbg(x) for x in binary_opt_list ]


binary_list = binary_opt_list + binary_dbg_list

# supported shells
supported_shells = ["csh", "sh", "bat"]


# Native platform guessing


linux_release_files = [ "/etc/redhat-release" ,
                        "/etc/system-release",
                        "/etc/SuSE-release",
                        "/etc/issue", "/etc/issue.net" ]

class NativeMachine:
    def __init__(self):
        self._arch = None
        self._ostype = None
        self._machine = None
        self._osflavor = None
        self._osversion = None
        self._sysinfo = platform.uname()
        if sys.platform == "win32" :
            self._arch = "32"
            self._ostype = "Windows"
            self._machine = "i686"
        elif sys.platform == "win64" :
            self._arch = "64"
            self._ostype = "Windows"
            self._machine = "x86_64"
        else :
            self._ostype = self._sysinfo[0]
            self._machine = self._sysinfo[4]
            if self._ostype in ["Linux", "LynxOS", "Darwin"] :
                if self._machine == "x86_64" or self._machine == "ia64" :
                    self._arch = "64"
                else :
                    self._arch = "32"
    def sysInfo(self):
        """ full platform.uname() list """
        return self._sysinfo
    def arch(self):
        """ returns 32 or 64 """
        return self._arch
    def OSType(self):
        """ returns Linux, Darwin, Windows """
        return self._ostype
    def machine(self):
        """ returns i386, i486, i686, x86_64, ia64, power mac """
        return self._machine
    def system(self):
        """ return Linux-i386, Windows-x86_64 ... """
        return "%s-%s" % (self._ostype, self._machine)
    # OS extraction
    def OSFlavour(self, teststring=None):
        if not self._osflavor or teststring:
            if self._ostype == "Windows" :
                self._osflavor = self._sysinfo[2]
                self._osversion = self._sysinfo[3]
            elif self._ostype == "SunOS" :
                self._osflavor = "sun"
                self._osversion = "4.x"
            elif self._ostype == "Darwin" :
                verList = [ int(v) for v in self._sysinfo[2].split('.') ]
                if len(verList) > 2:
                    osMajRelease = verList[0] - 4
                    osMinRelease = verList[1]
                    if osMajRelease == 3:
                        self._osflavor = 'Panther'
                    elif osMajRelease == 4:
                        self._osflavor = 'Tiger'
                    elif osMajRelease == 5:
                        self._osflavor = 'Leopard'
                    elif osMajRelease == 6:
                        self._osflavor = 'Snow Leopard'
                    self._osversion = "10.%d.%d" % (osMajRelease, osMinRelease)

            elif self._ostype == "Linux" :
                for f in linux_release_files :
                    if os.path.exists(f) :
                        cont = " ".join(open(f).readlines())
                        break
                if teststring :
                    cont = teststring
                oslist = ["Scientific Linux", "Red Hat Enterprise",
                          "Redhat Enterprise", "SuSE", "CentOS", "Fedora",
                          "Ubuntu", "Debian", "Mandriva Linux" ]
                for o in oslist :
                    if cont.upper().find(o.upper()) != -1 :
                        self._osflavor = o
                        if self._osflavor == "Red Hat Enterprise" :
                            self._osflavor = "Redhat Enterprise"
                        break
                vmatch = re.compile("\ +([0-9]+(?:\.[0-9]+)+)\ +")
                m = vmatch.search(cont)
                if m :
                    self._osversion = m.group(1)

        return self._osflavor

    def OSVersion(self, teststring=None):
        if not self._osversion :
            if self._ostype == "Windows" :
                self._osversion = self._sysinfo[3]
            elif self._ostype == "SunOS" :
                self._osversion = "4.x"
            elif self._ostype == "Darwin" :
                verList = [ int(v) for v in self._sysinfo[2].split(".") ]
                if len(verList) > 2:
                    osMajRelease = verList[0] - 4
                    osMinRelease = verList[1]
                    self._osversion = "10.%d.%d" % (osMajRelease, osMinRelease)
            elif self._ostype == "Linux" :
                for f in linux_release_files :
                    if os.path.exists(f) :
                        cont = " ".join(open(f).readlines())
                        break
                if teststring :
                    cont = teststring
                vmatch = re.compile("\ +([0-9]+(?:\.[0-9]+)+)\ +")
                m = vmatch.search(cont)
                if m :
                    self._osversion = m.group(1)

        return self._osversion
    # CMT derived informations
    def CMTArchitecture(self):
        """ returns the CMT architecture """
        arch = "ia32"
        if re.compile('i\d86').match(self._machine) :
            arch = "ia32"
        elif re.compile('x86_64').match(self._machine) :
            arch = "amd64"
        elif re.compile('ia64').match(self._machine) :
            arch = "ia64"
        elif re.compile('power mac', re.I).match(self._machine) :
            arch = "ppc"
        elif self._ostype == "Windows" :
            arch = sys.platform
        return arch

    def CMTSystem(self):
        """ returns the CMTBIn variable used by CMT itself """
        cmtsystem = None
        if self._ostype == "Windows" :
            cmtsystem = "VisualC"
        elif self._ostype == "Darwin" :
            cmtsystem = "Darwin-i386"
        else :
            if self._machine in ["i386", "i486", "i586"] :
                cmtsystem = "%s-i386" % self._ostype
            else :
                cmtsystem = "%s-%s" % (self._ostype, self._machine)
        return cmtsystem

    def CMTOSFlavour(self):
        pass
