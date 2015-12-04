""" general configuration for data packages """
#@PydevCodeAnalysisIgnore

from LbConfiguration.Repository import getRepositories
from LbConfiguration.External import external_projects, distribution_url
from LbUtils.VCS import splitlines


import logging
import sys, os

# ------------------------------------------------------------------------------------
# list of unversioned data projects
project_names = ["DBASE", "PARAM", "TOOLS" ]


# ------------------------------------------------------------------------------------
# list of original package names
package_names = ['XmlDDDB',
                 'DecFiles',
                 'FieldMap',
                 'BooleDQ',
                 'BrunelDQ',
                 'ParamFiles',
                 'XmlConditions',
                 'SQLDDDB',
                 'VeloSQLDDDB',
                 'STSQLDDDB',
                 'BcVegPyData',
                 'MCatNLOData',
                 'LHCbBkg',
                 'Geant4Files',
                 'MIBData',
                 'L0TCK',
                 'HltTCK',
                 'AppConfig',
                 'ChargedProtoANNPIDParam',
                 'Emacs',
                 'eclipse',
                 'jre',
                 'CPConfig',
                 'RDConfig',
                 'CharmConfig',
                 'TMVAWeights',
                 'ProdConf',
                 'GenXiccData',
                 'InstallAreaPatch',
                 'ExoticaConfig',
                 'EWConfig',
                 'SemilepConfig',
                 'BandQConfig',
                 'PRConfig',
                 'B2OCConfig',
                 'MCStatTools',
                 'GDMLData',
                 'QMTestFiles',
                 'Ariadne',
                 'RawEventFormat',
                 'LHAPDFSets',
                 'BNoCConfig',
                 'HLTConfig',
                 'PGunsData',
                 'B2CCConfig',
                 'TurboStreamProd',
                 'PIDCalib']

class PackageConfException(Exception):
    pass

class PackageConf(object):
    def __init__(self, packagename):
        self._name = packagename
        self._hat = None
        self._project = None
        self._tarballname = None
        self._release_area = None
        if os.environ.has_key("LHCBRELEASES") :
            self._release_area = os.environ["LHCBRELEASES"]
        if os.environ.has_key("LHCBTAR") :
            self._dist_loc = os.environ["LHCBTAR"]
        self._dereference_tar = False
    def Name(self):
        return self._name
    def name(self):
        return self._name.lower()
    def NAME(self):
        return self._name.upper()
    def FullName(self):
        if self._hat :
            return "/".join([self._hat, self._name])
        else :
            return self._name
    def setHat(self, hat):
        self._hat = hat
    def hat(self):
        return self._hat
    def setProject(self, project):
        if project in project_names :
            self._project = project
        else :
            raise PackageConfException
    def project(self):
        return self._project
    def setReleaseArea(self, release_area):
        self._release_area = release_area
    def ReleaseArea(self):
        return self._release_area
    def setDistLocation(self, dist_loc):
        self._dist_loc = dist_loc
    def DistLocation(self):
        return self._dist_loc
    def tarBallName(self, version=None, full=False):
        tbname = "_".join([self._project, self.FullName().replace("/", "_")])
        if version :
            tbname += "_%s" % version
            if full :
                tbname += ".tar.gz"
        return tbname
    def DistPrefix(self):
        return self._project.upper()
    def TarBallDir(self):
        return os.path.join(self.DistLocation(), self.DistPrefix())
    def LCGTarBallDir(self):
        return os.path.join(self.DistLocation(), "source")
    def HTMLDir(self):
        return os.path.join(self.DistLocation(), "html")
    def releasePrefix(self, version=None):
        prefix = None
        if self.hat() :
            prefix = os.path.join(self.project(), self.hat(), self.Name() )
        else :
            prefix = os.path.join(self.project(), self.Name() )
        if version :
            prefix = os.path.join(prefix, version)
        return prefix

    def md5FileName(self, version=None):
        mfname = self.tarBallName(version, full=False)
        mfname += ".md5"
        return mfname
    def dereferenceTar(self):
        return self._dereference_tar
    def enableDereferenceTar(self):
        self._dereference_tar = True
    def disableDereferenceTar(self):
        self._dereference_tar = False


package_list = []
for _pak in package_names:
    package_list.append(PackageConf(_pak))
del _pak#IGNORE:W0631

def _extractPkgFromProperty(packagename, property_content):
    """
    extract the package conf instance from the file-like property
    @param packagename: name of the package
    @param property_content: checkout of the packages property
    """
    pj = None
    for l in splitlines(property_content) :
        hat = None
        package, project = l.split()[:2]
        if "/" in package :
            hat, package = package.split("/")
        if packagename.lower() == package.lower() and project.upper() in [x.upper() for x in project_names] :
            pj = PackageConf(package)
            # BC: here we need to make sure the project name is in uppercase,
            # As this is done for all data packages defined in this file
            pj.setProject(project.upper())
            if hat :
                pj.setHat(hat)
            break
    return pj


def _getSVNPackage(packagename, dist_url=distribution_url, force_svn=False):
    """
    extract package informations directly from SVN top properties. This
    function is used as a fallback if this very file doesn't contain any
    informations about that packagename.
    Because some machine do not have SVN installed, it relies on the
    cache that a cron job is dumping
    @param packagename: name of the package for the conf retrieving.
    @type packagename: string
    """
    log = logging.getLogger()
    pj = None
    if packagename.upper() not in [ x.upper() for x in external_projects] :
        try :
            from urllib import urlopen
            property_content = urlopen("/".join([dist_url, "conf", "packages"])).read()
            pj = _extractPkgFromProperty(packagename, property_content)
        except :
            pass
        if not pj and force_svn :
            reps = getRepositories(protocol="anonymous")
            for name in reps:
                url = str(reps[name])
                log.debug("Looking for package '%s' in '%s' (%s)", packagename, name, url)
                rep = reps[name]
                if hasattr(rep, "getProperty") :
                    property_content = rep.getProperty("packages")
                    if property_content :
                        pj = _extractPkgFromProperty(packagename, property_content)
                        break
    return pj


def getPackage(packagename, svn_fallback=False, raise_exception=True):
    """ return the static instance of the package configuration by name """
    pj = None
    for p in package_list:
        if p.name() == packagename.lower():
            pj = p
    if pj :
        return pj
    elif svn_fallback:
        pj = _getSVNPackage(packagename)
        if not pj and raise_exception :
            raise PackageConfException, "No such package configuration"
        else :
            return pj
    elif raise_exception :
        raise PackageConfException, "No such package configuration"



def isPackage(packagename):
    ispackage = True
    try :
        getPackage(packagename)
    except PackageConfException :
        ispackage = False
    return ispackage

def getPackageList(projectname=None, hatname=None):
    plist = package_list
    if projectname :
        plist = [ p for p in plist if p.project() == projectname ]
    if hatname :
        plist = [ p for p in plist if p.hat() == hatname ]
    return plist

def getTarBallName(packagename, version):
    """ contruct the tarball name from the components """
    filename = None
    log = logging.getLogger()
    pak = getPackage(packagename)
    filename = pak.tarBallName(version, full=True)
    if filename :
        log.debug("The tarball name is %s" % filename)
    return filename

# create static instance of the package configuration with the correct name
for _pn in package_names:
    setattr(sys.modules[__name__], _pn, getPackage(_pn))
del _pn#IGNORE:W0631

XmlDDDB.setHat("Det")#IGNORE:E0602
XmlDDDB.setProject("DBASE")#IGNORE:E0602

DecFiles.setHat("Gen")#IGNORE:E0602
DecFiles.setProject("DBASE")#IGNORE:E0602

FieldMap.setProject("DBASE")#IGNORE:E0602

BooleDQ.setHat("DQ")#IGNORE:E0602
BooleDQ.setProject("DBASE")#IGNORE:E0602

BrunelDQ.setHat("DQ")#IGNORE:E0602
BrunelDQ.setProject("DBASE")#IGNORE:E0602

ParamFiles.setProject("PARAM")#IGNORE:E0602

XmlConditions.setHat("Det")#IGNORE:E0602
XmlConditions.setProject("DBASE")#IGNORE:E0602

SQLDDDB.setHat("Det")#IGNORE:E0602
SQLDDDB.setProject("DBASE")#IGNORE:E0602

VeloSQLDDDB.setHat("Det")#IGNORE:E0602
VeloSQLDDDB.setProject("DBASE")#IGNORE:E0602

STSQLDDDB.setHat("Det")#IGNORE:E0602
STSQLDDDB.setProject("DBASE")#IGNORE:E0602


BcVegPyData.setProject("PARAM")#IGNORE:E0602

MCatNLOData.setProject("PARAM")#IGNORE:E0602

LHCbBkg.setProject("PARAM")#IGNORE:E0602

Geant4Files.setProject("PARAM")#IGNORE:E0602
Geant4Files.disableDereferenceTar()#IGNORE:E0602

MIBData.setProject("PARAM")#IGNORE:E0602

TMVAWeights.setProject("PARAM")#IGNORE:E0602

ProdConf.setProject("DBASE")#IGNORE:E0602

GenXiccData.setProject("PARAM")#IGNORE:E0602

InstallAreaPatch.setProject("PARAM")#IGNORE:E0602

L0TCK.setHat("TCK")#IGNORE:E0602
L0TCK.setProject("DBASE")#IGNORE:E0602

HltTCK.setHat("TCK")#IGNORE:E0602
HltTCK.setProject("DBASE")#IGNORE:E0602

AppConfig.setProject("DBASE")#IGNORE:E0602

ChargedProtoANNPIDParam.setProject("PARAM")#IGNORE:E0602

Emacs.setHat("Tools")#IGNORE:E0602
Emacs.setProject("TOOLS")#IGNORE:E0602

jre.setHat("Java")#IGNORE:E0602
jre.setProject("TOOLS")#IGNORE:E0602

eclipse.setProject("TOOLS")#IGNORE:E0602

CPConfig.setHat("WG")#IGNORE:E0602
CPConfig.setProject("DBASE")#IGNORE:E0602

RDConfig.setHat("WG")#IGNORE:E0602
RDConfig.setProject("DBASE")#IGNORE:E0602

CharmConfig.setHat("WG")#IGNORE:E0602
CharmConfig.setProject("DBASE")#IGNORE:E0602

ExoticaConfig.setHat("WG")#IGNORE:E0602
ExoticaConfig.setProject("DBASE")#IGNORE:E0602

EWConfig.setHat("WG")#IGNORE:E0602
EWConfig.setProject("DBASE")#IGNORE:E0602

SemilepConfig.setHat("WG")#IGNORE:E0602
SemilepConfig.setProject("DBASE")#IGNORE:E0602

BandQConfig.setHat("WG")#IGNORE:E0602$
BandQConfig.setProject("DBASE")#IGNORE:E0602$

PRConfig.setProject("DBASE")#IGNORE:E0602

B2OCConfig.setHat("WG")#IGNORE:E0602$
B2OCConfig.setProject("DBASE")#IGNORE:E0602$

MCStatTools.setProject("DBASE")#IGNORE:E0602

GDMLData.setHat("Det")#IGNORE:E0602$
GDMLData.setProject("DBASE")#IGNORE:E0602$

QMTestFiles.setProject("PARAM")#IGNORE:E0602

Ariadne.setProject('TOOLS')#IGNORE:E0602

RawEventFormat.setProject("DBASE")#IGNORE:E0602

LHAPDFSets.setProject("DBASE")#IGNORE:E0602

BNoCConfig.setHat("WG")#IGNORE:E0602$
BNoCConfig.setProject("DBASE")#IGNORE:E0602$

HLTConfig.setHat("WG")#IGNORE:E0602$
HLTConfig.setProject("DBASE")#IGNORE:E0602$

PGunsData.setHat("Gen")#IGNORE:E0602$
PGunsData.setProject("DBASE")#IGNORE:E0602$

B2CCConfig.setHat("WG")#IGNORE:E0602$
B2CCConfig.setProject("DBASE")#IGNORE:E0602$

TurboStreamProd.setProject("DBASE")#IGNORE:E0602

PIDCalib.setHat("WG")#IGNORE:E0602$
PIDCalib.setProject("DBASE")#IGNORE:E0602$
