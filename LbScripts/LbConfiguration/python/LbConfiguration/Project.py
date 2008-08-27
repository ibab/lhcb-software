""" general configuration for projects """
#@PydevCodeAnalysisIgnore

import sys
import os

# list of original project names
project_names = ["Gaudi", "LHCb", "Lbcom", "Rec", "Boole", "Brunel" , 
                 "Gauss", "Phys", "Analysis", "Hlt", "Alignment", "Moore",
                 "Online", "Euler", "Geant4", "DaVinci", "Bender", "Orwell",
                 "Panoramix", "LbScripts"]

binary_list = ["slc3_ia32_gcc323", "slc3_ia32_gcc323_dbg",
               "slc4_ia32_gcc34", "slc4_ia32_gcc34_dbg",
               "slc4_amd64_gcc34", "slc4_amd64_gcc34_dbg",
               "win32_vc71", "win32_vc71_dbg"]

class ProjectConfException(Exception):
    pass

class ProjectBaseConf(object):
    def __init__(self, projectname):
        """ default constructor """
        self._name = projectname
        if os.environ.has_key("LHCBRELEASES") :
            self._release_area = os.environ["LHCBRELEASES"]
        else :
            self._release_area = None
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
        self._aliases = dict()
        self._applicationpackage = projectname
        self._aliases["setenv%s" % projectname] = "setenvProject %s" % projectname
        self._aliases["Setup%s" % projectname] = "SetupProject %s" % projectname
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
    def Aliases(self):
        """ return the shell aliases of the project """
        return self._aliases
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
    def setReleaseArea(self, release_area):
        self._release_area = release_area
    def ReleaseArea(self):
        return self._release_area
    def __str__(self):
        """ return string representation for printing """
        rep = super(ProjectConf, self).__str__()
        rep += "Size\t\t\t: %s\n" % self._fullsize
        rep += "Steering Package\t: %s\n" % self._steeringpackage
        rep += "Application Package\t: %s\n" % self._applicationpackage
        if self._cmtextratags:
            rep += "CMT Extra Tags\t\t: %s\n" % ", ".join(self._cmtextratags)
        return rep

project_list = []
for _proj in project_names:
    project_list.append(ProjectConf(_proj))
del _proj

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

# create static instance of the project configuration with the correct name
for _pn in project_names:
    setattr(sys.modules[__name__], _pn, getProject(_pn))
del _pn


# Extra configuration for selected projects
# Gaudi
Gaudi.setSteeringPackage("GaudiRelease") #IGNORE:E0602
Gaudi.setApplicationPackage("GaudiExamples")#IGNORE:E0602
if os.environ.has_key("GAUDISOFT") :
    Gaudi.setReleaseArea(os.environ["GAUDISOFT"])#IGNORE:E0602


# LHCb
LHCb.setApplicationPackage("Ex")#IGNORE:E0602


#Gauss
Gauss.addCMTExtraTags("Hijing", "Herwig", #IGNORE:E0602
                      "BcVegPy", "HidValley", "AlpGen")#IGNORE:E0602
Gauss.setApplicationPackage("Sim/Gauss")#IGNORE:E0602


# Boole
Boole.setApplicationPackage("Digi/Boole")#IGNORE:E0602

# Brunel
Brunel.setApplicationPackage("Rec/Brunel")#IGNORE:E0602

#DaVinci
DaVinci.setApplicationPackage("Phys/DaVinci")#IGNORE:E0602

#Moore
Moore.setApplicationPackage("Hlt/Moore")#IGNORE:E0602

# Euler
Euler.setApplicationPackage("Trig/Euler")#IGNORE:E0602

# Alignment
Alignment.setApplicationPackage("Alignment/Escher")#IGNORE:E0602

# Orwell
Orwell.setApplicationPackage("Calib/Orwell")#IGNORE:E0602

# Panoramix
Panoramix.setApplicationPackage("Vis/Panoramix")#IGNORE:E0602

# Bender
Bender.setApplicationPackage("Phys/Bender")#IGNORE:E0602

