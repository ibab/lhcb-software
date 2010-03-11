""" general configuration for projects """
#@PydevCodeAnalysisIgnore

from LbConfiguration.Platform import binary_list

import logging
import sys
import os

# ------------------------------------------------------------------------------------
# list of original project names
project_names = ["Gaudi", "LHCb", "Lbcom", "Rec", "Boole", "Brunel" ,
                 "Gauss", "Phys", "Analysis", "Hlt", "Alignment", "Moore",
                 "Online", "Euler", "Geant4", "DaVinci", "Bender", "Orwell",
                 "Panoramix", "LbScripts", "Dirac", "LHCbGrid", "Panoptes",
                 "Curie", "Vetra", "Compat", "VanDerMeer", "Ganga",
                 "LHCbDirac", "Integration", "Erasmus" ]

# ------------------------------------------------------------------------------------

class ProjectConfException(Exception):
    pass

class ProjectBaseConf(object):
    def __init__(self, projectname):
        """ default constructor """
        self._name = projectname
        self._release_area = None
        if os.environ.has_key("LHCBRELEASES") :
            self._release_area = os.environ["LHCBRELEASES"]
        self._dist_loc = None
        if os.environ.has_key("LHCBTAR") :
            self._dist_loc = os.environ["LHCBTAR"]
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
    def ReleaseArea(self):
        return self._release_area
    def setDistLocation(self, dist_loc):
        self._dist_loc = dist_loc
    def DistLocation(self):
        return self._dist_loc
    def TarBallDir(self):
        return os.path.join(self.DistLocation(), self.NAME())
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
        self._fullsize = 5000000
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
        self._hasbinary = True
        self.enableSetenvAlias()
        self.enableSetupAlias()
    def setCMTExtraTags(self, taglist):
        """ set the list of CMTEXTRATAGS needed for the project build """
        self._cmtextratags = taglist
    def addCMTExtraTags(self, *taglist):
        """ add tags to the list of CMTEXTRATAGS for the build """
        self._cmtextratags += taglist
    def CMTExtraTags(self):
        """ return the list of CMTEXTRATAGS used for the build """
        return self._cmtextratags
    def setFullSize(self, size):
        """ set the foreseen fullsize of a project release """
        self._fullsize = size
    def FullSize(self):
        """ returns the foreseen fullsize of a project release """
        return self._fullsize
    def setBaseName(self, basename):
        """ set the base name for the project """
        self._basename = basename
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
    def enableSetupAlias(self):
        """ enable the Setup Alias """
        self._setupalias = True
        self._aliases["Setup%s" % self._name] = "SetupProject %s" % self._name
    def disableSetupAlias(self):
        """ disable the Setup Alias """
        self._setupalias = False
        del self._aliases["Setup%s" % self._name]
    def setSteeringPackage(self, packname):
        """ set the name of the steering package for the build """
        self._steeringpackage = packname
    def SteeringPackage(self):
        """ returns the name of the steering package for the build """
        return self._steeringpackage
    def setApplicationPackage(self, packname):
        """set the name of the package which contains the application"""
        self._applicationpackage = packname
    def ApplicationPackage(self):
        return self._applicationpackage
    def setAFSVolumeName(self, volname):
        """set the name of the volume name to be used for new AFS volume"""
        self._afsvolumename = volname
    def AFSVolumeName(self):
        return self._afsvolumename
    def setAFSVolumeRoot(self, volroot):
        """set the name of the volume name to be used for new AFS volume"""
        self._afsvolumeroot = volroot
    def AFSVolumeRoot(self):
        return self._afsvolumeroot
    def setAFSLibrarianGroup(self, group):
        """set the name of the volume name to be used for new AFS volume"""
        self._afslibgroup = group
    def AFSLibrarianGroup(self):
        return self._afslibgroup
    def setExtraExe(self, exedict):
        """ set the list of extra executable to be build in the project """
        self._extraexe = exedict
    def addExtraExe(self, exename, taglist=None):
        """ add executables to the list of extra executables for the build """
        if taglist :
            self._extraexe[exename] = taglist
        else :
            self._extraexe[exename] = []
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
                if cmtconfig not in binary_list :
                    log.error("the CMTCONFIG value %s is not known" % cmtconfig)
            if full :
                tbname += ".tar.gz"

        return tbname
    def md5FileName(self, version=None, cmtconfig=None):
        mfname = self.tarBallName(version, cmtconfig, full=False)
        mfname += ".md5"
        return mfname

    def releasePrefix(self, version):
        return os.path.join(self.NAME(), "_".join([self.NAME(),version]) )
    def enableHasBinary(self):
        """ enable binary nature of the project """
        self._hasbinary = True
    def disableHasBinary(self):
        """ disable the binary nature of the project """
        self._hasbinary = False
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

def getProject(projectname):
    """ return the static instance of the project configuration by name """
    pj = None
    for p in project_list:
        if p.name() == projectname.lower():
            pj = p
    if pj :
        return pj
    else:
        raise ProjectConfException, "No such project configuration"

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
Alignment.setFullSize(800000)#IGNORE:E0602

# Analysis
Analysis.setAFSVolumeName("ANALY")#IGNORE:E0602
Analysis.setFullSize(2000000)#IGNORE:E0602

# Bender
Bender.setApplicationPackage("Phys/Bender")#IGNORE:E0602
Bender.setFullSize(500000)#IGNORE:E0602
Bender.setBaseName("DAVINCI")#IGNORE:E0602

# Boole
Boole.setApplicationPackage("Digi/Boole")#IGNORE:E0602
Boole.setFullSize(1500000)#IGNORE:E0602

# Brunel
Brunel.setApplicationPackage("Rec/Brunel")#IGNORE:E0602
Brunel.setFullSize(700000)#IGNORE:E0602

# Curie
Curie.setFullSize(50000)#IGNORE:E0602
Curie.disableHasBinary()#IGNORE:E0602

# DaVinci
DaVinci.setApplicationPackage("Phys/DaVinci")#IGNORE:E0602
DaVinci.setAFSVolumeName("DAV")#IGNORE:E0602
DaVinci.setFullSize(800000)#IGNORE:E0602

# Dirac
Dirac.disableSetenvAlias()#IGNORE:E0602
Dirac.setFullSize(500000)#IGNORE:E0602

# LHCbDirac
LHCbDirac.disableSetenvAlias()#IGNORE:E0602
LHCbDirac.setFullSize(500000)#IGNORE:E0602
LHCbDirac.setAFSVolumeName("LBDIRAC")#IGNORE:E0602

# Erasmus
Erasmus.setAFSVolumeName("ERASM")#IGNORE:E0602


# Euler
Euler.setApplicationPackage("Trig/Euler")#IGNORE:E0602
Euler.setFullSize(6000)#IGNORE:E0602

# Gaudi
Gaudi.setSteeringPackage("GaudiRelease") #IGNORE:E0602
Gaudi.setApplicationPackage("GaudiExamples")#IGNORE:E0602
if os.environ.has_key("GAUDISOFT") :
    Gaudi.setReleaseArea(os.environ["GAUDISOFT"])#IGNORE:E0602
Gaudi.setAFSVolumeRoot("Gaudi") #IGNORE:E0602
Gaudi.setAFSLibrarianGroup("gaudi") #IGNORE:E0602
Gaudi.setFullSize(3000000)#IGNORE:E0602
Gaudi.setBaseName("LCGCMT")#IGNORE:E0602

#Gauss
Gauss.setApplicationPackage("Sim/Gauss") #IGNORE:E0602
Gauss.setFullSize(700000) #IGNORE:E0602
Gauss.addExtraExe("Hijing", ["Hijing"] ) #IGNORE:E0602
Gauss.addExtraExe("Herwig", ["Herwig"] ) #IGNORE:E0602
Gauss.addExtraExe("BcVegPy", ["BcVegPy"] ) #IGNORE:E0602
Gauss.addExtraExe("HidValley", ["HidValley"] ) #IGNORE:E0602
Gauss.addExtraExe("AlpGen", ["AlpGen"] ) #IGNORE:E0602

# Geant4
Geant4.setAFSVolumeName("G4")#IGNORE:E0602
Geant4.setFullSize(2000000)#IGNORE:E0602
Geant4.setBaseName("GAUDI")#IGNORE:E0602

# Hlt
Hlt.setFullSize(2500000)#IGNORE:E0602

# Integration
Integration.setFullSize(2500000)#IGNORE:E0602
Integration.setAFSVolumeName("INTEG")#IGNORE:E0602


# Lbcom
Lbcom.setFullSize(1500000)#IGNORE:E0602

# LbScripts
LbScripts.setAFSVolumeName("LB")#IGNORE:E0602
LbScripts.setFullSize(10000)#IGNORE:E0602
LbScripts.disableHasBinary()#IGNORE:E0602

# LHCb
LHCb.setApplicationPackage("Ex")#IGNORE:E0602
LHCb.setFullSize(6000000)#IGNORE:E0602
LHCb.setBaseName("GAUDI")#IGNORE:E0602


# LHCbGrid
LHCbGrid.setFullSize(100000)#IGNORE:E0602

# Moore
Moore.setApplicationPackage("Hlt/Moore")#IGNORE:E0602
Moore.setFullSize(700000)#IGNORE:E0602

# Online
Online.setFullSize(1500000)#IGNORE:E0602

# Orwell
Orwell.setApplicationPackage("Calib/Orwell")#IGNORE:E0602
Orwell.setFullSize(50000)#IGNORE:E0602

# Panoptes
Panoptes.setApplicationPackage("Rich/Panoptes")#IGNORE:E0602
Panoptes.setFullSize(50000)#IGNORE:E0602

# Panoramix
Panoramix.setApplicationPackage("Vis/Panoramix")#IGNORE:E0602
Panoramix.setAFSVolumeName("PANORA") #IGNORE:E0602
Panoramix.setFullSize(1000000)#IGNORE:E0602

# Phys
Phys.setFullSize(2000000)#IGNORE:E0602

# Rec
Rec.setFullSize(6000000)#IGNORE:E0602

# VanDerMeer
VanDerMeer.setFullSize(50000)#IGNORE:E0602
VanDerMeer.setAFSVolumeName("VDM")#IGNORE:E0602

