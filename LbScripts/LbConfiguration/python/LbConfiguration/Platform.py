""" General LHCb platform configuration """

import sys, platform, os
import re
import logging

# CMTCONFIG extraction

def isNewStyleBinary(cmtconfig):
    """ check if the CMTCONFIG value is new styled """
    newstyle = False
    if len(cmtconfig.split("-")) > 1 :
        newstyle = True
    return newstyle

def isOldStyleBinary(cmtconfig):
    """ check if the CMTCONFIG value is new styled """
    return not isNewStyleBinary(cmtconfig)

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
    if platformtype == "sl6" :
        platformtype = "slc6"
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
    if platformtype.startswith("win") :
        if compiler :
            cmtconfig = "_".join([platformtype, compiler])
            if compiler.startswith("vc9") :
                if architecture == "ia32" :
                    architecture = "i686"
                elif architecture == "amd64" :
                    architecture = "x86_64"
                cmtconfig = "-".join([architecture, "winxp", compiler, "opt"])
        else :
            cmtconfig = platformtype

    else :
        if architecture == "ia32" :
            architecture = "i686"
        elif architecture == "amd64" :
            architecture = "x86_64"
        if compiler :
            cmtconfig = "-".join([architecture, platformtype, compiler, "opt"])
            if platformtype == "slc4" or platformtype == "slc3" or platformtype == "osx105":
                if architecture in arch_runtime_compatiblity["ia32"] :
                    architecture = "ia32"
                elif architecture == "x86_64" :
                    architecture = "amd64"
                cmtconfig = "_".join([platformtype, architecture, compiler])
        else :
            cmtconfig = "-".join([architecture, platformtype, "opt"])
            if platformtype == "slc4" or platformtype == "slc3" or platformtype == "osx105":
                if architecture in arch_runtime_compatiblity["ia32"] :
                    architecture = "ia32"
                elif architecture == "x86_64" :
                    architecture = "amd64"
                cmtconfig = "_".join([platformtype, architecture])


    if debug :
        cmtconfig = getBinaryDbg(cmtconfig)

    return cmtconfig

# officially supported binaries
binary_opt_list = ["slc4_ia32_gcc34", "slc4_amd64_gcc34",
                   "x86_64-slc5-gcc43-opt", "i686-slc5-gcc43-opt",
                   "x86_64-slc5-gcc46-opt", "i686-slc5-gcc46-opt",
                   "win32_vc71", "i686-winxp-vc9-opt",
                   "x86_64-slc5-icc11-opt", "i686-slc5-icc11-opt",
                   "x86_64-slc6-gcc46-opt", "i686-slc6-gcc46-opt",
                   "x86_64-slc6-gcc47-opt", "x86_64-slc6-gcc48-opt",
                   "x86_64-slc6-gcc49-opt", "x86_64-slc6-gcc50-opt",
                   "x86_64-slc6-icc13-opt", "x86_64-slc6-clang32-opt",
                   "x86_64-slc6-clang32-opt", "x86_64-slc6-clang34-opt",
                   "x86_64-slc6-clang35-opt" ]
                   
# future possible supported binaries
extra_binary_opt_list = ["slc3_ia32_gcc323",
                         "x86_64-slc5-gcc34-opt", "i686-slc5-gcc34-opt",
                         "i686-slc5-gcc43-opt",
                         "i686-winxp-vc90-opt", "x86_64-winxp-vc90-opt",
                         "osx105_ia32_gcc401", "x86_64-osx106-gcc42-opt"]

binary_dbg_list = [ getBinaryDbg(x) for x in binary_opt_list ]
extra_binary_dbg_list = [ getBinaryDbg(x) for x in extra_binary_opt_list ]

binary_list = binary_opt_list + binary_dbg_list
extra_binary_list = extra_binary_opt_list + extra_binary_dbg_list


def pathBinaryMatch(path, cmtconfig):
    """ returns True if the path belong to the cmtconfig distribution
    @param path: file/path to be tested
    @param cmtconfig: target cmtconfig
    """
    selected = False
    log = logging.getLogger()
    if cmtconfig not in binary_list :
        log.error("the value of CMTCONFIG %s is not supported" % cmtconfig)
    else :
        match_str = "%s" % cmtconfig
        if isOldStyleBinary(cmtconfig) and isBinaryOpt(cmtconfig):
            match_str = "%s(?!_dbg)" % cmtconfig
        cfg_match = re.compile(match_str)
        if cfg_match.search(path) :
            selected = True
    return selected

def pathSharedMatch(path, cmtconfig=None):
    """ select path with are not part of a binary distribution
    @param path: file/dir path to be tested
    @param cmtconfig: optional parameter to exclude specific files for a given cmtconfig
    """
    selected = True
    for b in binary_list :
        if pathBinaryMatch(path, b) :
            selected = False
            break
    return selected

def pathMatch(path, cmtconfig, shared=False):
    """
    return True if the path belong to the CMTCONFIG.
    """
    selected = False
    if not shared :
        selected = pathBinaryMatch(path, cmtconfig)
    else :
        selected = pathSharedMatch(path, cmtconfig)
    return selected

def pathFilter(pathlist, cmtconfig, shared=False):
    return [ p for p in pathlist if pathMatch(p, cmtconfig, shared) ]

# supported shells
supported_shells = ["csh", "sh", "bat"]


# Native platform guessing


linux_release_files = [ "/etc/redhat-release" ,
                        "/etc/system-release",
                        "/etc/SuSE-release",
                        "/etc/issue", "/etc/issue.net" ]

linux_flavour_aliases = {
                         "slc"    : ["Scientific Linux"],
                         "rhel"   : ["Redhat Enterprise", "Red Hat Enterprise"],
                         "rh"     : ["Redhat", "Red Hat"],
                         "fc"     : ["Fedora", "Fedora Core"],
                         "suse"   : ["SuSE"],
                         "centos"     : ["CentOS"],
                         "deb"    : ["Debian"],
                         "ubuntu" : ["Ubuntu"],
                         "ml"     : ["Mandriva Linux"],
                         "bb"     : ["Big Box Linux"]
                        }

lsb_flavour_aliases   = {
                         "sl"   : ["ScientificSL"],
                         "slc"  : ["ScientificCERNSLC"],
                         "fc"   : ["Fedora", "Fedora Core"],
                         "centos"   : ["CentOS"]
                        }

flavor_runtime_compatibility = {
                                "slc6"  : ["slc6", "slc5"],
                                "slc5"  : ["slc5", "slc4"],
                                "slc4"  : ["slc4", "slc3"],
                                "slc3"  : ["slc3"],
                                "rh73"  : ["rh73"],
                                "win32" : ["win32"],
                                "win64" : ["win64"],
                                "osx105": ["osx105"],
                                "osx106": ["osx105", "osx106"]
                                }

arch_runtime_compatiblity = {
                                "x86_64" : ["x86_64", "i686", "i586", "i486", "i386"],
                                "ia64" :   ["ia64", "i686", "i586", "i486", "i386"],
                                "ia32" :   ["ia32", "i686", "i586", "i486", "i386"],
                                "i686" :   ["i686", "i586", "i486", "i386"],
                                "i586" :   ["i586", "i486", "i386"],
                                "i486" :   ["i486", "i386"],
                                "i386" :   ["i386"],
                                "ppc"  :   ["ppc"]
                                }

flavor_runtime_equivalence = {
                              "slc6"  : ["slc6", "co6", "rhel6"],
                              "slc5"  : ["slc5", "co5", "rhel5", "ub9", "fc13", "fc12", "fc11", "fc10"],
                              "slc4"  : ["slc4", "co4", "rhel4", "deb4"],
                              "slc3"  : ["slc3", "suse90", "suse100"],
                              "rh73"  : ["rh73", "suse80", "suse81", "suse82", "suse83"],
                              "win32" : ["win32"],
                              "win64" : ["win64"],
                              "osx105": ["osx105"],
                              "osx106": ["osx106"]
                             }

supported_compilers = {
                       "slc6"   : ["gcc48", "gcc46","gcc45", "gcc44", "gcc47", "icc11", "icc13",
                                   "clang32", "clang34", "clang35", "gcc49", "gcc50"],
                       "slc5"   : ["gcc46", "gcc43", "gcc45", "icc11"] ,
                       "slc4"   : ["gcc34"],
                       "slc3"   : ["gcc323"],
                       "win32"  : ["vc71", "vc9"],
                       "win64"  : ["vc71", "vc9"],
                       "osx104" : ["gcc40"],
                       "osx105" : ["gcc401"],
                       "osx106" : ["gcc42"]
                       }
class NativeMachine:
    def __init__(self):
        """
        constructor
        """
        self._arch = None
        self._ostype = None
        self._machine = None
        self._osflavor = None
        self._osversion = None
        self._compversion = None
        self._compiler = None
        self._sysinfo = None
        self._lsb_distributor_id = None
        self._lsb_description = None
        self._lsb_release = None
        self._lsb_codename = None
        if hasattr(platform, "uname") :
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
                if self._ostype == "Darwin" and os.popen("uname -p").read()[:-1] == "powerpc" :
                    self._arch = "ppc"
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
                found = False
                for f in linux_flavour_aliases :
                    if not found :
                        for s in linux_flavour_aliases[f]:
                            if not found :
                                if cont.upper().find(s.upper()) != -1 :
                                    self._osflavor = linux_flavour_aliases[f][0]
                                    found = True
                                    break
                            else :
                                break
                    else :
                        break
                vmatch = re.compile("\ +(\d+(?:\.\d+)*)")
                m = vmatch.search(cont)
                if m :
                    self._osversion = m.group(1)

        return self._osflavor

    def OSVersion(self, position=None, teststring=None):
        """
        returns version of the OS up to position
        @param position: last position of version (no included)
        @param teststring: test entry
        """
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
                vmatch = re.compile("\ +(\d+(?:\.\d+)*)")
                m = vmatch.search(cont)
                if m :
                    self._osversion = m.group(1)

        osver = self._osversion

        # returns at most the number of position specified.
        if position :
            osver = ".".join(self._osversion.split(".")[:position])

        return osver
    def nativeCompilerVersion(self, position=None):
        """
        return the native compiler version
        @param position: if not None returns up to the nth position. ie for gcc 3.4.5 with
        position=2, it returns 3.4
        """
        if not self._compversion :
            if self._ostype == "Windows" :
                self._compversion = "vc9"
            else:
                try:
                    gpp = (c for c in
                           [os.path.join(d, "g++")
                            for d in os.environ["PATH"].split(os.pathsep)
                            if d.startswith('/usr')]
                           if os.path.exists(c)).next()
                    compstr = " ".join(os.popen(gpp + " --version" ).readlines())[:-1]
                    m = re.search(r"\ +(\d+(?:\.\d+)*)", compstr)
                    if m :
                        self._compversion = m.group(1)
                    else:
                        self._compversion = None
                except StopIteration:
                    self._compversion = None
        ncv = self._compversion

        if position :
            try :
                ncv = ".".join(self._compversion.split(".")[:position])
            except AttributeError:
                ncv = None
        return ncv

    def nativeCompiler(self):
        if not self._compiler :
            if self._ostype == "Windows" :
                self._compiler = self.nativeCompilerVersion()
            else :
                try :
                    cvers = [int(c) for c in self.nativeCompilerVersion(position=2).split(".")]
                    self._compiler = "gcc%d%d" % (cvers[0], cvers[1])
                    if cvers[0] == 3 and cvers[1] < 4 :
                        self._compiler = "gcc%s" % self.nativeCompilerVersion(position=3).replace(".","")
                    if self._ostype == "Darwin" and self.OSVersion(position=2) == "10.5" :
                        self._compiler = "gcc%s" % self.nativeCompilerVersion(position=3).replace(".","")
                except :
                    self._compiler = None
        return self._compiler
    # CMT derived informations
    def CMTArchitecture(self):
        """ returns the CMT architecture """
        arch = "ia32"
        if re.compile('i\d86').match(self.machine()) :
            arch = "ia32"
        elif re.compile('x86_64').match(self.machine()) :
            arch = "amd64"
        elif re.compile('ia64').match(self.machine()) :
            arch = "ia64"
        elif re.compile('power mac', re.I).match(self.machine()) :
            arch = "ppc"
        elif self.OSType() == "Windows" :
            arch = sys.platform
        return arch

    def CMTSystem(self):
        """ returns the CMTBIn variable used by CMT itself """
        cmtsystem = None
        if self.OSType() == "Windows" :
            cmtsystem = "VisualC"
        elif self.OSType() == "Darwin" :
            cmtsystem = "Darwin-i386"
        else :
            if self.machine() in arch_runtime_compatiblity["i586"] :
                cmtsystem = "%s-i386" % self.OSType()
            else :
                cmtsystem = "%s-%s" % (self.OSType(), self.machine())
        return cmtsystem

    def CMTOSFlavour(self):
        """ returns the CMT short name for the OS flavour and version """
        cmtflavour = None
        if self.OSType() == "Windows" :
            cmtflavour = "win%s" % self.arch()
        elif self.OSType() == "SunOS" :
            cmtflavour = self.OSFlavour() + self.OSVersion(position=2).replace(".", "")
        elif self.OSType() == "Darwin" :
            cmtflavour = "osx%s" % self.OSVersion(position=2).replace(".", "")
        elif self.OSType() == "Linux" :
            for f in linux_flavour_aliases :
                if self.OSFlavour() == linux_flavour_aliases[f][0] :
                    cmtflavour = f + self.OSVersion(position=1)
                    if self.OSFlavour() in ("SuSE", "Redhat", "Ubuntu"):
                        cmtflavour = f + self.OSVersion(position=2)
                    if self.OSFlavour() == "SuSE" and int(self.OSVersion(position=1)) > 10 :
                        cmtflavour = f + self.OSVersion(position=1)
        return cmtflavour

    def CMTOSEquivalentFlavour(self):
        """ returns the CMT short name for the OS compatible flavour and version """
        cmtflavour = None
        for f in flavor_runtime_equivalence :
            if self.CMTOSFlavour() in flavor_runtime_equivalence[f] :
                cmtflavour = f
                break
        return cmtflavour

    def CMTCompatibleConfig(self, debug=False):
        """ return the list of compatible CMT configs """
        compatibles = []
        equiv = self.CMTOSEquivalentFlavour()
        machine = self.machine()
        if equiv in flavor_runtime_compatibility :
            for f in flavor_runtime_compatibility[equiv] :
                for m in arch_runtime_compatiblity[machine] :
                    allcomp = []
                    if f in supported_compilers :
                        allcomp += supported_compilers[f]
                    # add the native compiler
                    nc = self.nativeCompiler()
                    if nc :
                        allcomp.append(nc)
                    for c in allcomp :
                        n = getConfig(m, f, c, debug=False)
                        if n not in compatibles :
                            compatibles.append(n)
                        if debug :
                            n = getConfig(m, f, c, debug=True)
                            if n not in compatibles :
                                compatibles.append(n)

        return compatibles

    def CMTSupportedConfig(self, debug=False):
        """
        returns the list of supported CMT configs among the compatible ones. This
        means the ones which are shipped and usable on a local site.
        @param debug: if True returns also the debug configs. Otherwise only the opt ones.
        """
        compatibles = self.CMTCompatibleConfig(debug)
        supported = []
        for c in compatibles :
            if c in binary_list and c not in supported:
                supported.append(c)
        return supported
    def CMTNativeConfig(self, debug=False):
        """
        Returns the native configuration if possible. Guess also the compiler
        on linux platforms
        @param debug: if True returns also the debug configs. Otherwise only the opt ones.
        """
        comp = self.nativeCompiler()
        mach = self.machine()
        osflav = self.CMTOSFlavour()
        natconf = getConfig(architecture=mach, platformtype=osflav,
                            compiler=comp, debug=debug)
        return natconf
    def DiracPlatform(self):
        """
        return Dirac-style platform
        """
        platformlist = [ platform.system(), platform.machine() ]
        if self.OSType() == "Linux" :
            # get version of highest libc installed
            if self.arch() == "64":
                lib = '/lib64'
            else:
                lib = '/lib'
            libs = []
            for l in os.listdir( lib ):
                if l.find('libc-') == 0 or l.find('libc.so') == 0 :
                    libs.append( os.path.join(lib, l) )
            libs.sort()
            platformlist.append( '-'.join( platform.libc_ver(libs[-1])) )
        elif self.OSType() == "Darwin" :
            platformlist.append( '.'.join(platform.mac_ver()[0].split( "." )[:2]) )
        elif self.OSType() == "Windows" :
            platformlist.append(platform.win32_ver()[0])
        else :
            platformlist.append( platform.release() )


        platformstr = "_".join(platformlist)
        return platformstr
    def numberOfCPUs(self):
        """ Number of virtual or physical CPUs on this system, i.e.
        user/real as output by time(1) when called with an optimally scaling userspace-only program"""
        res = 1
        # Python 2.6+
        try:
            import multiprocessing
            return multiprocessing.cpu_count()
        except (ImportError, NotImplementedError):
            pass

        # POSIX
        try:
            res = int(os.sysconf('SC_NPROCESSORS_ONLN'))

            if res > 0:
                return res
        except (AttributeError, ValueError):
            pass

        # Windows
        try:
            res = int(os.environ['NUMBER_OF_PROCESSORS'])

            if res > 0:
                return res
        except (KeyError, ValueError):
            pass
        # BSD
        try:
            import subprocess
            sysctl = subprocess.Popen(['sysctl', '-n', 'hw.ncpu'], stdout=subprocess.PIPE)
            scStdout = sysctl.communicate()[0]
            res = int(scStdout)

            if res > 0:
                return res
        except (OSError, ValueError, ImportError):
            pass

        # Linux
        try:
            res = open('/proc/cpuinfo').read().count('processor\t:')

            if res > 0:
                return res
        except IOError:
            pass

        # Solaris
        try:
            pseudoDevices = os.listdir('/devices/pseudo/')
            expr = re.compile('^cpuid@[0-9]+$')

            res = 0
            for pd in pseudoDevices:
                if expr.match(pd) != None:
                    res += 1

            if res > 0:
                return res
        except OSError:
            pass

        # Other UNIXes (heuristic)
        try:
            try:
                dmesg = open('/var/run/dmesg.boot').read()
            except IOError:
                dmesgProcess = subprocess.Popen(['dmesg'], stdout=subprocess.PIPE)
                dmesg = dmesgProcess.communicate()[0]

            res = 0
            while '\ncpu' + str(res) + ':' in dmesg:
                res += 1

            if res > 0:
                return res
        except OSError:
            pass

        return res
    def LSBDistributorID(self):
        """
        wrapper around lsb_release -i
        """
        if not self._lsb_distributor_id and self.OSType() == "Linux" :
            if os.path.exists("/usr/bin/lsb_release") :
                lsbstr = os.popen("lsb_release -i").read()[:-1]
                if lsbstr :
                    self._lsb_distributor_id = lsbstr.split(":")[-1].strip()
        return self._lsb_distributor_id
    def LSBDescription(self):
        """
        wrapper around lsb_release -d
        """
        if not self._lsb_description and self.OSType() == "Linux" :
            if os.path.exists("/usr/bin/lsb_release") :
                lsbstr = os.popen("lsb_release -d").read()[:-1]
                if lsbstr :
                    self._lsb_description = lsbstr.split(":")[-1].strip()
        return self._lsb_description
    def LSBRelease(self):
        """
        wrapper around lsb_release -r
        """
        if not self._lsb_release and self.OSType() == "Linux" :
            if os.path.exists("/usr/bin/lsb_release") :
                lsbstr = os.popen("lsb_release -r").read()[:-1]
                if lsbstr :
                    self._lsb_release = lsbstr.split(":")[-1].strip()
        return self._lsb_release
    def LSBCodeName(self):
        """
        wrapper around lsb_release -c
        """
        if not self._lsb_codename and self.OSType() == "Linux" :
            if os.path.exists("/usr/bin/lsb_release") :
                lsbstr = os.popen("lsb_release -c").read()[:-1]
                if lsbstr :
                    self._lsb_codename = lsbstr.split(":")[-1].strip()
        return self._lsb_codename

