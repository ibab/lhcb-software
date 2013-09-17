""" general configuration for projects """
#@PydevCodeAnalysisIgnore

from LbConfiguration.Repository import getRepositories
from LbConfiguration.Platform import binary_list, getBinaryOpt
from LbConfiguration.External import external_projects, distribution_url

from LbUtils.VCS import splitlines
from LbUtils.afs.volume import MAX_NAME_LENGTH, BadVolumeName

import logging
import sys
import os

# ------------------------------------------------------------------------------------
# list of original project names
# please keep this list in the hierarchical order of the dependencies
project_names = [ "Gaudi", "LHCb", "Lbcom", "Rec", "Boole", "Brunel" ,
                  "Gauss", "Phys", "Analysis", "Hlt", "Alignment", "Moore",
                  "Online", "Euler", "Geant4", "DaVinci", "Bender", "Orwell",
                  "Panoramix", "LbScripts", "Dirac", "LHCbGrid", "Panoptes",
                  "Curie", "Vetra", "VetraTB", "Compat", "VanDerMeer", "Ganga",
                  "LHCbDirac", "Integration", "Erasmus", "Feicim",
                  "Stripping", "LHCbExternals", "Urania", "VMDirac", "LHCbVMDirac", "Noether", "Tesla",
                  "MooreOnline", "BeautyDirac" ]

afs_volume_short_names = { "Gaudi"      : "GA",
                           "LHCb"       : "LH",
                           "Lbcom"      : "LO",
                           "Rec"        : "RE",
                           "Boole"      : "BO",
                           "Brunel"     : "BR",
                           "Gauss"      : "GS",
                           "Phys"       : "PH",
                           "Analysis"   : "AN",
                           "Hlt"        : "HL",
                           "Alignment"  : "AL",
                           "MOORE"      : "MO",
                           "Online"     : "ON",
                           "Euler"      : "EU",
                           "Geant4"     : "G4",
                           "DaVinci"    : "DV",
                           "Bender"     : "BE",
                           "Orwell"     : "OR",
                           "Panoramix"  : "PA",
                           "LbScripts"  : "LB",
                           "Dirac"      : "DI",
                           "LHCbExternals"   : "LE",
                           "LHCbGrid"   : "LG",
                           "Panoptes"   : "PT",
                           "Curie"      : "CI",
                           "Vetra"      : "VE",
                           "VetraTB"    : "VT",
                           "Compat"     : "CO",
                           "VanDerMeer" : "VA",
                           "Ganga"      : "GG",
                           "LHCbDirac"  : "LD",
                           "Integration": "IN",
                           "Erasmus"    : "ER",
                           "Feicim"     : "FE",
                           "Stripping"  : "ST",
                           "VMDirac"    : "VM",
                           "LHCbVMDirac": "LV",
                           "Noether"    : "NO",
                           "Tesla"      : "TE",
                           "Urania"     : "UR",
                           "MooreOnline": "ML",
                           "BeautyDirac": "BD"}

# ------------------------------------------------------------------------------------

class ProjectConfException(Exception):
    pass

class ProjectBaseConf(object):
    def __init__(self, projectname):
        """ default constructor """
        self._propdict = {}
        self._name = projectname
        self._release_area = None
        if os.environ.has_key("LHCBRELEASES") :
            self._release_area = os.environ["LHCBRELEASES"]
        self._dist_loc = None
        if os.environ.has_key("LHCBTAR") :
            self._dist_loc = os.environ["LHCBTAR"]
    def setProperty(self, name, value):
        self._propdict[name] = value
    def getProperty(self, name):
        return self._propdict[name]
    def delProperty(self, name):
        del self._propdict[name]
    def Name(self):
        """ return original name """
        return self._name
    def name(self):
        """ return name in lowercase """
        return self._name.lower()
    def NAME(self):
        """ return name in uppercase """
        return self._name.upper()
    def setReleaseArea(self, release_area):
        self._release_area = release_area
        self.setProperty("ReleaseArea", self._release_area)
    def ReleaseArea(self):
        return self._release_area
    def setDistLocation(self, dist_loc):
        self._dist_loc = dist_loc
        self.setProperty("DistLocation", self._dist_loc)
    def DistLocation(self):
        return self._dist_loc
    def DistPrefix(self):
        return self.NAME()
    def TarBallDir(self):
        return os.path.join(self.DistLocation(), self.DistPrefix())
    def LCGTarBallDir(self):
        return os.path.join(self.DistLocation(), "source")
    def HTMLDir(self):
        return os.path.join(self.DistLocation(), "html")
    def __str__(self):
        """ return string representation for printing """
        rep = ""
        if self._name:
            rep += "Name\t\t\t: %s\n" % self._name
        if self._release_area :
            rep += "Release Area\t\t: %s\n" % self._release_area
        return rep


class ProjectConf(ProjectBaseConf):
    def __init__(self, projectname):
        """ default constructor """
        super(ProjectConf, self).__init__(projectname)
        self._cmtextratags = []
        self._fullsize = 15000000
        self._steeringpackage = projectname + "Sys"
        self._afsvolumename = self.NAME()
        self._afsvolumeroot = "lhcb"
        self._afslibgroup = "z5"
        self._aliases = dict()
        self._setenvalias = True
        self._setupalias = True
        self._applicationpackage = projectname
        self._extraexe = {}
        self._basename = ""
        self._tarballname = "_".join([self.NAME(), self.NAME()])
        self._lcgtarballname = None
        self._hasbinary = True
        self.enableSetenvAlias()
        self.enableSetupAlias()
    def setCMTExtraTags(self, taglist):
        """ set the list of CMTEXTRATAGS needed for the project build """
        self._cmtextratags = taglist
        self.setProperty("CMTEXTRATAGS", self._cmtextratags)
    def addCMTExtraTags(self, *taglist):
        """ add tags to the list of CMTEXTRATAGS for the build """
        self._cmtextratags += taglist
        self.setProperty("CMTEXTRATAGS", self._cmtextratags)
    def CMTExtraTags(self):
        """ return the list of CMTEXTRATAGS used for the build """
        return self._cmtextratags
    def setFullSize(self, size):
        """ set the foreseen fullsize of a project release """
        self._fullsize = size
        self.setProperty("FullSize", self._fullsize)
    def FullSize(self):
        """ returns the foreseen fullsize of a project release """
        return self._fullsize
    def setBaseName(self, basename):
        """ set the base name for the project """
        self._basename = basename
        self.setProperty("BaseName", self._basename)
    def BaseName(self):
        """ returns the base name of the project """
        return self._basename
    def Aliases(self):
        """ return the shell aliases of the project """
        return self._aliases
    def enableSetenvAlias(self):
        """ enable the setenv Alias """
        self._setenvalias = True
        self._aliases["setenv%s" % self._name] = "setenvProject %s" % self._name
    def disableSetenvAlias(self):
        """ disable the setenv Alias """
        self._setenvalias = False
        del self._aliases["setenv%s" % self._name]
        self.setProperty("SetenvAlias", self._setenvalias)
    def enableSetupAlias(self):
        """ enable the Setup Alias """
        self._setupalias = True
        self._aliases["Setup%s" % self._name] = "SetupProject %s" % self._name
    def disableSetupAlias(self):
        """ disable the Setup Alias """
        self._setupalias = False
        del self._aliases["Setup%s" % self._name]
        self.setProperty("SetupAlias", self._setupalias)
    def setSteeringPackage(self, packname):
        """ set the name of the steering package for the build """
        self._steeringpackage = packname
        self.setProperty("SteeringPackage", self._steeringpackage)
    def SteeringPackage(self):
        """ returns the name of the steering package for the build """
        return self._steeringpackage
    def setApplicationPackage(self, packname):
        """set the name of the package which contains the application"""
        self._applicationpackage = packname
        self.setProperty("ApplicationPackage", self._applicationpackage)
    def ApplicationPackage(self):
        return self._applicationpackage
    def setAFSVolumeName(self, volname):
        """set the name of the volume name to be used for new AFS volume"""
        self._afsvolumename = volname
        self.setProperty("AFSVolumeName", self._afsvolumename)
    def AFSVolumeName(self):
        return self._afsvolumename
    def AFSVolumeShortName(self):
        return afs_volume_short_names[self.Name()]
    def setAFSVolumeRoot(self, volroot):
        """set the name of the volume name to be used for new AFS volume"""
        self._afsvolumeroot = volroot
        self.setProperty("AFSVolumeRoot", self._afsvolumeroot)
    def AFSVolumeRoot(self):
        return self._afsvolumeroot
    def setAFSLibrarianGroup(self, group):
        """set the name of the volume name to be used for new AFS volume"""
        self._afslibgroup = group
    def AFSLibrarianGroup(self):
        return self._afslibgroup
    def AFSReleaseVolumeName(self, version):
        log = logging.getLogger()
        volname = "q.%s.%s_%s" % (self.AFSVolumeRoot(), self.AFSVolumeName(), version)
        if len(volname) > MAX_NAME_LENGTH :
            log.warn("The volume name %s is too long" % volname)
            volname = "q.%s.%s_%s" % (self.AFSVolumeRoot(), self.AFSVolumeShortName(), version)
            log.warning("Using %s instead" % volname)
            if len(volname) > MAX_NAME_LENGTH:
                raise BadVolumeName, "%s is still too long" % volname
        return volname
    def setExtraExe(self, exedict):
        """ set the list of extra executable to be build in the project """
        self._extraexe = exedict
        self.setProperty("ExtraExe", self._extraexe)
    def addExtraExe(self, exename, taglist=None):
        """ add executables to the list of extra executables for the build """
        if taglist :
            self._extraexe[exename] = taglist
        else :
            self._extraexe[exename] = []
        self.setProperty("ExtraExe", self._extraexe)
    def ExtraExe(self):
        """ return the list of extra executable used for the build """
        return self._extraexe
    def tarBallName(self, version=None, cmtconfig=None, full=False):
        log = logging.getLogger()
        tbname = self._tarballname
        if version :
            tbname += "_%s" % version
            if cmtconfig :
                tbname += "_%s" % cmtconfig
            if full :
                tbname += ".tar.gz"

        return tbname
    def setLCGTarBallName(self, name):
        self._lcgtarballname = name
        self.setProperty("LCGTarBallName", self._lcgtarballname)
    def LCGTarBallName(self, version=None, cmtconfig=None, full=False):
        """ returns the name of the LCG tarball dependency """
        log = logging.getLogger()
        tbname = self._lcgtarballname
        if version :
            tbname += "_%s" % version
            if cmtconfig :
                if cmtconfig not in binary_list :
                    log.error("The CMTCONFIG value %s is not known" % cmtconfig)
                else :
                    cmtconfig = getBinaryOpt(cmtconfig)
                    tbname += "_%s" % cmtconfig
                if full :
                    tbname += ".tar.gz"
        return tbname

    def md5FileName(self, version=None, cmtconfig=None):
        mfname = self.tarBallName(version, cmtconfig, full=False)
        mfname += ".md5"
        return mfname
    def LCGmd5FileName(self, version=None, cmtconfig=None):
        mfname = self.LCGTarBallName(version, cmtconfig, full=False)
        mfname += ".md5"
        return mfname
    def htmlFileName(self, version=None, cmtconfig=None):
        hname  = self.tarBallName(version, cmtconfig, full=False)
        hname += ".html"
        return hname
    def releasePrefix(self, version):
        return os.path.join(self.NAME(), "_".join([self.NAME(),version]) )
    def enableHasBinary(self):
        """ enable binary nature of the project """
        self._hasbinary = True
    def disableHasBinary(self):
        """ disable the binary nature of the project """
        self._hasbinary = False
        self.setProperty("HasBinary", self._hasbinary)
    def hasBinary(self):
        """ Access function for the binary nature """
        return self._hasbinary



    def __str__(self):
        """ return string representation for printing """
        rep = super(ProjectConf, self).__str__()
        rep += "Size\t\t\t: %s\n" % self._fullsize
        rep += "Steering Package\t: %s\n" % self._steeringpackage
        rep += "Application Package\t: %s\n" % self._applicationpackage
        if self._cmtextratags:
            rep += "CMT Extra Tags\t\t: %s\n" % ", ".join(self._cmtextratags)
        return rep
# ------------------------------------------------------------------------------------

project_list = []
for _proj in project_names:
    project_list.append(ProjectConf(_proj))
del _proj#IGNORE:W0631

# ------------------------------------------------------------------------------------
# XML Tools

import xml.dom


def dumpXMLConf():
    impl = xml.dom.getDOMImplementation()
    doc  = impl.createDocument(None, "ProjectConfiguration", None)
    de   = doc.documentElement
    de.setAttribute("version", "1.0")
    for p in project_list :
        pe = doc.createElement("project")
        pe.setAttribute("Name", p.Name())
        de.appendChild(pe)
        for prop in p._propdict.keys() :
            ppe = doc.createElement(prop)
            if type(p._propdict[prop]) is dict:
                ppe.setAttribute("Type", "dict")
            if type(p._propdict[prop]) is list:
                ppe.setAttribute("Type", "list")
            ppt = doc.createTextNode(str(p._propdict[prop]))
            ppe.appendChild(ppt)
            pe.appendChild(ppe)
    return doc.toprettyxml(encoding="UTF-8")
# ------------------------------------------------------------------------------------

def _extractPrjFromProperty(projectname, property_content):
    pj = None
    for l in splitlines(property_content) :
        project = l.strip()
        if projectname.upper() == project.upper() :
            pj = ProjectConf(project)
            break

    return pj

def _getSVNProject(projectname, dist_url=distribution_url, force_svn=False):
    """
    extract project informations directly from SVN top properties. This
    function is used as a fallback if this very file doesn't contain any
    informations about that projectname.
    @param projectname: name of the project to get the configuration
    @type projectname: string
    """
    pj = None
    log = logging.getLogger()
    if projectname.upper() not in [ x.upper() for x in external_projects] :
        try :
            from urllib import urlopen
            property_content = urlopen("/".join([dist_url, "conf", "projects"])).read()
            pj = _extractPrjFromProperty(projectname, property_content)
        except:
            pass
        if not pj and force_svn :
            reps = getRepositories(protocol="anonymous")
            for name in reps:
                url = str(reps[name])
                log.debug("Looking for package '%s' in '%s' (%s)", projectname, name, url)
                rep = reps[name]
                if hasattr(rep, "getProperty") :
                    property_content = rep.getProperty("projects")
                    if property_content :
                        pj = _extractPrjFromProperty(projectname, property_content)
                        break
    return pj


def getProject(projectname, svn_fallback=False, raise_exception=True):
    """ return the static instance of the project configuration by name """
    pj = None
    for p in project_list:
        if p.name() == projectname.lower():
            pj = p
    if pj :
        return pj
    elif svn_fallback:
        pj = _getSVNProject(projectname)
        if not pj and raise_exception :
            raise ProjectConfException, "No such project configuration"
        else :
            return pj
    elif raise_exception :
        raise ProjectConfException, "No such project configuration"


def isProject(projectname):
    isproject = True
    try :
        getProject(projectname)
    except ProjectConfException :
        isproject = False
    return isproject


def getProjectList():
    return project_list

def getTarBallName(projectname, version, cmtconfig=None):
    """ contruct the tarball name from the components """
    filename = None
    log = logging.getLogger()
    proj = getProject(projectname)
    filename = proj.tarBallName(version, cmtconfig, full=True)
    if filename :
        log.debug("The tarball name is %s" % filename)
    return filename


# create static instance of the project configuration with the correct name
for _pn in project_names:
    setattr(sys.modules[__name__], _pn, getProject(_pn))
del _pn#IGNORE:W0631

# ------------------------------------------------------------------------------------

# Extra configuration for selected projects (in alphabetical order)

# Alignment
Alignment.setApplicationPackage("Alignment/Escher")#IGNORE:E0602
Alignment.setAFSVolumeName("ALIGN")#IGNORE:E0602
Alignment.setFullSize(15000000)#IGNORE:E0602

# Analysis
Analysis.setAFSVolumeName("ANALY")#IGNORE:E0602
Analysis.setFullSize(15000000)#IGNORE:E0602

# Bender
Bender.setApplicationPackage("Phys/Bender")#IGNORE:E0602
Bender.setFullSize(15000000)#IGNORE:E0602
Bender.setBaseName("DAVINCI")#IGNORE:E0602

# Boole
Boole.setApplicationPackage("Digi/Boole")#IGNORE:E0602
Boole.setFullSize(15000000)#IGNORE:E0602

# Brunel
Brunel.setApplicationPackage("Rec/Brunel")#IGNORE:E0602
Brunel.setFullSize(15000000)#IGNORE:E0602

# Curie
Curie.setFullSize(15000000)#IGNORE:E0602
Curie.disableHasBinary()#IGNORE:E0602

# DaVinci
DaVinci.setApplicationPackage("Phys/DaVinci")#IGNORE:E0602
DaVinci.setAFSVolumeName("DAV")#IGNORE:E0602
DaVinci.setFullSize(15000000)#IGNORE:E0602

# Dirac
Dirac.disableSetenvAlias()#IGNORE:E0602
Dirac.disableSetupAlias()#IGNORE:E0602
Dirac.setFullSize(500000)#IGNORE:E0602

# VMDirac
VMDirac.disableSetenvAlias()#IGNORE:E0602
VMDirac.disableSetupAlias()#IGNORE:E0602
VMDirac.setFullSize( 500000 )#IGNORE:E0602


# LHCbDirac
LHCbDirac.disableSetenvAlias()#IGNORE:E0602
LHCbDirac.setFullSize(15000000)#IGNORE:E0602
LHCbDirac.setAFSVolumeName("LBDIRAC")#IGNORE:E0602
LHCbDirac.setLCGTarBallName("LCGGrid")#IGNORE:E0602

# LHCbVMDirac
LHCbVMDirac.disableSetenvAlias()#IGNORE:E0602
LHCbVMDirac.setFullSize( 15000000 )#IGNORE:E0602
LHCbVMDirac.setAFSVolumeName( "LVDIRAC" )#IGNORE:E0602

# Erasmus
Erasmus.setAFSVolumeName("ERASM")#IGNORE:E0602

# Urania
Urania.setAFSVolumeName("URAN")#IGNORE:E0602

# Euler
Euler.setApplicationPackage("Trig/Euler")#IGNORE:E0602
Euler.setFullSize(15000000)#IGNORE:E0602

#Feicim
Feicim.setApplicationPackage("Feicim")#IGNORE:E0602

# Ganga
Ganga.setLCGTarBallName("LCGGanga")#IGNORE:E0602

# Gaudi
Gaudi.setSteeringPackage("GaudiRelease") #IGNORE:E0602
Gaudi.setApplicationPackage("GaudiExamples")#IGNORE:E0602
if os.environ.has_key("GAUDISOFT") :
    Gaudi.setReleaseArea(os.environ["GAUDISOFT"])#IGNORE:E0602
Gaudi.setAFSVolumeRoot("Gaudi") #IGNORE:E0602
Gaudi.setAFSLibrarianGroup("gaudi") #IGNORE:E0602
Gaudi.setFullSize(15000000)#IGNORE:E0602
Gaudi.setBaseName("LCGCMT")#IGNORE:E0602
Gaudi.setLCGTarBallName("LCGCMT")#IGNORE:E0602

#Gauss
Gauss.setApplicationPackage("Sim/Gauss") #IGNORE:E0602
Gauss.setFullSize(15000000) #IGNORE:E0602
Gauss.addExtraExe("Hijing", ["Hijing"] ) #IGNORE:E0602
Gauss.addExtraExe("Herwig", ["Herwig"] ) #IGNORE:E0602
Gauss.addExtraExe("BcVegPy", ["BcVegPy"] ) #IGNORE:E0602
Gauss.addExtraExe("HidValley", ["HidValley"] ) #IGNORE:E0602
Gauss.addExtraExe("AlpGen", ["AlpGen"] ) #IGNORE:E0602

# Geant4
Geant4.setAFSVolumeName("G4")#IGNORE:E0602
Geant4.setFullSize(15000000)#IGNORE:E0602
Geant4.setBaseName("GAUDI")#IGNORE:E0602

# Hlt
Hlt.setFullSize(15000000)#IGNORE:E0602

# Integration
Integration.setFullSize(15000000)#IGNORE:E0602
Integration.setAFSVolumeName("INTEG")#IGNORE:E0602


# Lbcom
Lbcom.setFullSize(15000000)#IGNORE:E0602

# LbScripts
LbScripts.setAFSVolumeName("LB")#IGNORE:E0602
LbScripts.setFullSize(15000000)#IGNORE:E0602
LbScripts.disableHasBinary()#IGNORE:E0602

# LHCb
LHCb.setApplicationPackage("Ex")#IGNORE:E0602
LHCb.setFullSize(20000000)#IGNORE:E0602
LHCb.setBaseName("GAUDI")#IGNORE:E0602


# LHCbGrid
LHCbGrid.setFullSize(15000000)#IGNORE:E0602
LHCbGrid.setLCGTarBallName("LCGGrid")#IGNORE:E0602
# Moore
Moore.setApplicationPackage("Hlt/Moore")#IGNORE:E0602
Moore.setFullSize(15000000)#IGNORE:E0602

# Online
Online.setFullSize(15000000)#IGNORE:E0602

# Orwell
Orwell.setApplicationPackage("Calib/Orwell")#IGNORE:E0602
Orwell.setFullSize(15000000)#IGNORE:E0602

# Panoptes
Panoptes.setApplicationPackage("Rich/Panoptes")#IGNORE:E0602
Panoptes.setFullSize(15000000)#IGNORE:E0602

# Panoramix
Panoramix.setApplicationPackage("Vis/Panoramix")#IGNORE:E0602
Panoramix.setAFSVolumeName("PANORA") #IGNORE:E0602
Panoramix.setFullSize(15000000)#IGNORE:E0602

# Phys
Phys.setFullSize(15000000)#IGNORE:E0602

# Rec
Rec.setFullSize(30000000)#IGNORE:E0602

# Stripping
Stripping.setFullSize(15000000)#IGNORE:E0602
Stripping.setAFSVolumeName("STRIP")#IGNORE:E0602

# VanDerMeer
VanDerMeer.setFullSize(15000000)#IGNORE:E0602
VanDerMeer.setAFSVolumeName("VDM")#IGNORE:E0602

# VetraTB
VetraTB.setAFSVolumeName("VETRTB")#IGNORE:E0602

# Noether
Noether.setAFSVolumeName("NOET")#IGNORE:E0602

# Tesla
Tesla.setAFSVolumeName("TESL")#IGNORE:E0602

# MooreOnline
MooreOnline.setAFSVolumeName("MOL")#IGNORE:E0602

# BeautyDirac
BeautyDirac.setAFSVolumeName("BDIRAC")#IGNORE:E0602
