""" Module to load Packages and Projects from XML configuration"""

import logging
import inspect
import pprint
import os

from xml import sax
from xml.dom.minidom import Document

from Project import ProjectConf
from Package import PackageConf


log = logging.getLogger()
logging.basicConfig()
log.setLevel(logging.CRITICAL)

# Constants for the factory
CONFIG_ENV_VAR="LHCBCONFIG"
CONFIG_WEB_LOCATION_PREFIX="http://bcouturi.web.cern.ch/bcouturi/config/"
CONFIG_LOCAL_LOCATION_PREFIX="/afs/cern.ch/user/b/bcouturi/public/config/"
CONFIG_MAIN_FILENAME="MainConfig.xml"
CONFIG_PROJECT_FILENAME="ProjectConfig.xml"
CONFIG_PACKAGE_FILENAME="PackageConfig.xml"


#
# Container class for the main configuration parameters
#
class MainConfig:

    def __init__(self):
        """ Constructor for the MainConfig class """
        self.distribution_url = None
        self.Python_version = None
        self.CMT_version = None
        self.tbroadcast_version = None
        self.doxygen_version = None
        self.external_projects = []
        self.lcg_projects = []

# SAX handler for the project tag in the project XML description
# After parsing, call the getProjectList method to get the list
# of initialized ProjectConf projects
class SAXMainConfigHandler(sax.handler.ContentHandler):

    # Constructor
    def __init__(self):
        """ Constructor for the Project SAX Handler """
        self.getChars = 0
        self.buffer = ""
        self.config = MainConfig()
        self.inExternalProjects = False
        self.inLCGProjects = False

    def getResult(self):
        """ Method that returns the actual values for the configuration """
        return self.config

    # Methods needed for SAX parsing
    def startElement(self, name, attributes):
        """ Method for SAX Parsing """
        log.debug("StartElement %s" % name)
        self.currentTag = name
        self.getChars = 0

        if name == "mainConfiguration":
            # Container tag, just ignore
            None
        elif name == "externalProjects":
            self.inExternalProjects = True
        elif name == "LCGProjects":
            self.inLCGProjects = True
        else:
            self.buffer = ""
            self.getChars = 1


    def characters(self, data):
        """ Method for SAX Parsing """
        log.debug("characters %s" % self.currentTag)
        if self.getChars:
            self.buffer += data

    def endElement(self, name):
        """ Method for SAX Parsing """
        log.debug("endElement %s %s" % (name, self.currentTag))

        # Cleaning up the buffer if needed
        if self.buffer != None:
            self.buffer.strip()

        # No need to keep chars any more, startElement can reset it if needed
        self.getChars = 0

        # Checking the current tag to set the data accordingly
        if name == "mainConfiguration":
            # Container tag just ignore
            None
        elif name == "distributionUrl":
            self.config.distribution_url = self.buffer
        elif name == "pythonVersion":
            self.config.Python_version = self.buffer
        elif name == "CMTVersion":
            self.config.CMT_version = self.buffer
        elif name == "tbroadcastVersion":
            self.config.tbroadcast_version = self.buffer
        elif name == "doxygenVersion":
            self.config.doxygen_version = self.buffer
        elif name == "externalProjects":
            self.inExternalProjects = False
        elif name == "LCGProjects":
            self.inLCGProjects = False
        elif name == "project":
            log.debug("project tag External: %s LCG: %s" % (str(self.inExternalProjects), str(self.inLCGProjects)))
            if self.inExternalProjects:
                self.config.external_projects.append(self.buffer)
            elif self.inLCGProjects:
                self.config.lcg_projects.append(self.buffer)
        else:
            log.error("Unhandled tag:" +  name)

# SAX handler for the project tag in the project XML description
# After parsing, call the getProjectList method to get the list
# of initialized ProjectConf projects
class SAXProjectHandler(sax.handler.ContentHandler):

    # Constructor
    def __init__(self):
        """ Constructor for the Project SAX Handler """
        self.getChars = 0
        self.buffer = ""
        self.projectList = []

    # Methods needed for SAX parsing
    def startElement(self, name, attributes):
        """ Method for SAX Parsing """
        log.debug("StartElement %s" % name)
        self.currentTag = name
        self.getChars = 0

        if name == "projectConfiguration":
            # Container tag, just ignore
            None
        elif name == "project":
            self.projectName = attributes["name"]
            try:
                self.projectShortName = attributes["shortName"]
            except KeyError:
                # We just ignore in this case
                None

            self.project = self.createProject(self.projectName, self.projectShortName)
            log.info("Processing project " + self.projectName + "/" + self.projectShortName)
        elif name == "extraexe":
            self.extraexename = attributes["name"]
            self.buffer = ""
            self.getChars = 1
        else:
            self.buffer = ""
            self.getChars = 1


    def characters(self, data):
        """ Method for SAX Parsing """
        log.debug("characters %s" % self.currentTag)
        if self.getChars:
            self.buffer += data

    def endElement(self, name):
        """ Method for SAX Parsing """
        log.debug("endElement %s %s" % (name, self.currentTag))
        self.getChars = 0
        if name == "project":
            log.info("Finished processing project " + self.projectName + "/" + self.projectShortName)
            self.projectList.append(self.project)
        elif name == "projectConfiguration":
            # Container tag just ignore
            None
        elif name == "extraexe":
            self.project.addExtraExe(self.extraexename, [ self.buffer.strip() ])
        else:
            self.setProjectAttribute(self.project, self.currentTag, self.buffer.strip())

    def getResult(self):
        """ Returns the lit of projects after parsing """
        return self.projectList

    # creates a ProjectConf instance based on the name and short names retrieved
    def createProject(self, name, short_name):
        """ Create the ProjectConf """
        # TODO: Set short name, ignored for the moment
        # Not set method on the ProjectConf at the moment
        p = ProjectConf(name)
        return p

    # Sets the attributes on the projects for each of the element
    def setProjectAttribute(self, project, name, value):
        """ Sets an attribute on a ProjectConf class """
        log.info("Project %s setProjectAttribute on %s/%s" % (project.Name(), name, value))
        # 1-First checking the value for boolean attributes
        # We cast from string to boolean and find the appropriate method (enable/disable accordinmgly
        boolAttrs = ["setenvAlias", "setupAlias", "hasBinary"]
        if name in boolAttrs:
            boolValue = None
            if value != None and value.lower().strip() == "true":
                boolValue = True
            else:
                boolValue = False

            if boolValue == None:
                log.error("Could not set attribute:" + name)
                #TO DO check waht we should do...raise Exception, "Could not parse value:" + value + " for boolean tag " + name
                return

            if boolValue:
                boolMethod = findMethod(project, "enable", name)
            else:
                boolMethod = findMethod(project, "disable", name)
            # now call the method
            boolMethod()
            return

        # 2- Handle the attributes which have to be stored in a list
        # Not needed, cannot be done this way as we set key/value pairs
        #listAttrs = ["extraexe"]
        #if name in listAttrs:
        #    listMethod = findMethod(project, "add", name)
        #    # now call the method
        #    if listMethod != None:
        #         listMethod(value)
        #    else:
        #        log.error("Could not set attribute:" + name)
        #    return


        # 3 - For all others use the set method on the project class
        method = findMethod(project, "set", name)
        if method != None:
            method(value)
        else:
            log.error("Could not set attribute:" + name)

# SAX handler for the package tag in the package XML description
# After parsing, call the getPackageList method to get the list
# of initialized PackageConf projects
class SAXPackageHandler(sax.handler.ContentHandler):

    # Constructor
    def __init__(self):
        """ Constructor for the Project SAX Handler """
        self.getChars = 0
        self.buffer = ""
        self.packageList = []

    # Methods needed for SAX parsing
    def startElement(self, name, attributes):
        """ Method for SAX Parsing """
        log.debug("StartElement %s" % name)
        self.currentTag = name
        self.getChars = 0

        if name == "packageConfiguration":
            # Container tag, just ignore
            None
        elif name == "package":
            self.packageName = attributes["name"]
            hat = None
            try:
                hat = attributes["hat"]
            except KeyError:
                # Ignore, just means that the attribute wasn't set
                None

            projectName = attributes["project"]
            self.package = self.createPackage(self.packageName, hat, projectName)
            log.info("# Processing package " + self.packageName)
        else:
            self.buffer = ""
            self.getChars = 1


    def characters(self, data):
        """ Method for SAX Parsing """
        log.debug("characters %s %s" % (self.getChars, self.currentTag))
        if self.getChars:
            self.buffer += data

    def endElement(self, name):
        """ Method for SAX Parsing """
        log.debug("endElement %s %s" % (name, self.currentTag))
        self.getChars = 0
        if name == "package":
            log.info("# Finished processing package " + self.packageName)
            self.packageList.append(self.package)
        elif name == "packageConfiguration":
            # Container tag just ignore
            None
        else:
            self.setPackageAttribute(self.package, self.currentTag, self.buffer)

    def getResult(self):
        """ Returns the lit of projects after parsing """
        return self.packageList

    # creates a ProjectConf instance based on the name and short names retrieved
    def createPackage(self, name, hat, projectName):
        """ Create the PackageConf """
        p = PackageConf(name)
        if hat != None:
            p.setHat(hat)
        if projectName != None:
            p.setProject(projectName)
        return p

    # Sets the attributes on the projects for each of the element
    def setPackageAttribute(self, project, name, value):
        """ Sets an attribute on a PackageConf class """
        log.info("Project %s setPackageAttribute on %s/%s" % (project.Name(), name, value))
        # 1-First checking the value for boolean attributes
        # We cast from string to boolean and find the appropriate method (enable/disable accordinmgly
        boolAttrs = ["dereferenceTar"]
        if name in boolAttrs:
            boolValue = None
            if value != None and value.lower().strip() == "true":
                boolValue = True
            else:
                boolValue = False

            if boolValue == None:
                log.error("Could not set attribute:" + name)
                return

            if boolValue:
                boolMethod = findMethod(project, "enable", name)
            else:
                boolMethod = findMethod(project, "disable", name)
            # now call the method
            boolMethod()
            return

        # 2 - For all others use the set method on the project class
        method = findMethod(project, "set", name)
        if method != None:
            method(value)
        else:
            log.error("Could not set attribute:" + name)

# Method that looks for a method name in the object passed
# and returns it in order to set the attribute
def findMethod(obj, prefix, partialMethodName):
    """ Find the method named prefix + partialMethodName in the object instance """
    allmethods = inspect.getmembers(obj, predicate=inspect.ismethod)
    foundMethod = None
    for m in allmethods:
            if m[0].lower() == (prefix + partialMethodName.lower()):
                log.debug("## Found Method " + m[0])
                foundMethod = m[1]
    return foundMethod


# Mapping between type of config and the associated filename

# Base class for loading configuration
# Only specialized version should be used
class ConfigLoader(object):
    """ Ancestor for configuration loaders, should not be used directly but sub class should be used instead """
    def __init__(self):
        self.loadMethods = [self.__envConfigLocation, self.__webConfigLocation, self.__localConfigLocation]


    def __envConfigLocation(self):
        """ Method checking whether an environment variable has been defined for the config file location"""
        result = None
        try:
            conflocation = os.environ[CONFIG_ENV_VAR]
            if (conflocation != None):
                result = conflocation + os.sep + self.filename
        except:
            # We ignore key errors...
            pass
        return result

    def __webConfigLocation(self):
        """ Default URL for main config on the web """
        return CONFIG_WEB_LOCATION_PREFIX + self.filename

    def __localConfigLocation(self):
        """ Default URL for local config """
        return CONFIG_LOCAL_LOCATION_PREFIX + self.filename

    def __parse(self):
        """ Parse from a specific file """
        parser = sax.make_parser()
        parser.setContentHandler(self.handler)
        parser.parse(self.configURL)

    #
    #
    # Main method to looks for the config files in various places
    def load(self, configURL=None):
        """ Load configuration from the specified file """
        result = None
        if configURL != None:
            self.configURL = configURL
            self.__parse()
            result = self.handler.getResult()
        else:
            for m in self.loadMethods:
                loc = m()
                if loc == None:
                    continue
                try:
                    self.configURL = loc
                    self.__parse()
                    result = self.handler.getResult()
                    log.info("Using configuration from %s" % self.configURL)
                except:
                    pass
                    # We ignore and try next algorithm
        return result

class MainConfigLoader(ConfigLoader):
    """ Class to load the Main Config """
    def __init__(self):
        super(MainConfigLoader, self).__init__()
        self.handler = SAXMainConfigHandler()
        self.filename = CONFIG_MAIN_FILENAME

class ProjectLoader(ConfigLoader):
    """ Class to load the Projects config """
    def __init__(self):
        super(ProjectLoader, self).__init__()
        self.handler = SAXProjectHandler()
        self.filename = CONFIG_PROJECT_FILENAME

class PackageLoader(ConfigLoader):
    """ Class to load the package config """
    def __init__(self):
        super(PackageLoader, self).__init__()
        self.handler = SAXPackageHandler()
        self.filename = CONFIG_PACKAGE_FILENAME

#
# Util method to load the main config from a specific file
def loadMainConfig(configURL = None):
    """ Load main config  from the specified file """
    p = MainConfigLoader()
    return p.load(configURL)

# Util method to load the projects from a specific file
def loadProjects(configURL):
    """ Load projects from the specified file """
    p = ProjectLoader()
    return p.load(configURL)

# Util method to load packages from a specific file
def loadPackages(configURL):
    """ Load packages from the specified file """
    p = PackageLoader()
    return p.load(configURL)


# Utility method to serialize a list of packages
# Used to generate first version of XML file
def serializePackages(packageList):
    """ Method to serialize a list of packages to the appropriate format """
    doc = Document()
    # Create the base element
    packageConfiguration = doc.createElement("packageConfiguration")
    doc.appendChild(packageConfiguration)

    # Iterate on the list of packages
    for p in packageList:
        # Project element
        pack = doc.createElement("package")
        packageConfiguration.appendChild(pack)
        pack.setAttribute("name", p.Name())
        if p.hat() != None:
                pack.setAttribute("hat", p.hat())
        if p.project() != None:
                pack.setAttribute("project", p.project())

        # Release area
        if p.ReleaseArea() != None and p.ReleaseArea() != os.environ["LHCBRELEASES"]:
            e = doc.createElement("releaseArea")
            ptext = doc.createTextNode(p.ReleaseArea())
            e.appendChild(ptext)
            pack.appendChild(e)

        # DistLocation
        if p.DistLocation() != None and p.DistLocation() != os.environ["LHCBTAR"]:
            e = doc.createElement("distLocation")
            ptext = doc.createTextNode(p.DistLocation())
            e.appendChild(ptext)
            pack.appendChild(e)

        # Tar ball name
        if p._tarballname != None:
            e = doc.createElement("tarballName")
            ptext = doc.createTextNode(p._tarballname)
            e.appendChild(ptext)
            pack.appendChild(e)

        # deref tar param (false by default so we insert the element when it is true)
        if p._dereference_tar == True:
            e = doc.createElement("dereferenceTar")
            ptext = doc.createTextNode("true")
            e.appendChild(ptext)
            pack.appendChild(e)

    return doc


# Utility method to serialize a list of packages
# Used to generate first version of XML file
def serializeProjects(projectList):
    """ Method to serialize a list of projects to the appropriate format """
    doc = Document()
    # Create the base element
    packageConfiguration = doc.createElement("projectConfiguration")
    doc.appendChild(packageConfiguration)

    # Iterate on the list of packages
    for p in projectList:
        # Project element
        pack = doc.createElement("project")
        packageConfiguration.appendChild(pack)
        pack.setAttribute("name", p.Name())
        try:
            short_name = p.AFSVolumeShortName()
            if short_name != None:
                pack.setAttribute("shortName", short_name)
        except:
            None
            # Just ignore short name in this case

        # Release area
        if p.ReleaseArea() != None and p.ReleaseArea() != os.environ["LHCBRELEASES"]:
            e = doc.createElement("releaseArea")
            ptext = doc.createTextNode(p.ReleaseArea())
            e.appendChild(ptext)
            pack.appendChild(e)

        # DistLocation
        if p.DistLocation() != None and p.DistLocation() != os.environ["LHCBTAR"]:
            e = doc.createElement("distLocation")
            ptext = doc.createTextNode(p.DistLocation())
            e.appendChild(ptext)
            pack.appendChild(e)

        # BaseName
        if p.BaseName() != None and p.BaseName() != "":
            e = doc.createElement("baseName")
            ptext = doc.createTextNode(p.BaseName())
            e.appendChild(ptext)
            pack.appendChild(e)

        # LCGTarballName
        if p._lcgtarballname != None:
            e = doc.createElement("LCGTarballName")
            ptext = doc.createTextNode(p._lcgtarballname)
            e.appendChild(ptext)
            pack.appendChild(e)

        # AFS Volume Name
        if p._afsvolumename != p.NAME():
            e = doc.createElement("afsVolumeName")
            ptext = doc.createTextNode(p._afsvolumename)
            e.appendChild(ptext)
            pack.appendChild(e)

        # AFS Volume Root
        if p._afsvolumeroot != "lhcb":
            e = doc.createElement("afsVolumeRoot")
            ptext = doc.createTextNode(p._afsvolumeroot)
            e.appendChild(ptext)
            pack.appendChild(e)

        # AFS Librarian Group
        if p._afslibgroup != "z5":
            e = doc.createElement("afsLibrarianGroup")
            ptext = doc.createTextNode(p._afslibgroup)
            e.appendChild(ptext)
            pack.appendChild(e)

        # Has binary
        if p._hasbinary != True:
            e = doc.createElement("hasBinary")
            ptext = doc.createTextNode("false")
            e.appendChild(ptext)
            pack.appendChild(e)

        # setenvAlias
        if p._setenvalias != True:
            e = doc.createElement("setenvAlias")
            ptext = doc.createTextNode("false")
            e.appendChild(ptext)
            pack.appendChild(e)

        # setenvAlias
        if p._setupalias != True:
            e = doc.createElement("setupAlias")
            ptext = doc.createTextNode("false")
            e.appendChild(ptext)
            pack.appendChild(e)


        # steeringPackage
        if p._steeringpackage != p._name + "Sys":
            e = doc.createElement("steeringPackage")
            ptext = doc.createTextNode(p._steeringpackage)
            e.appendChild(ptext)
            pack.appendChild(e)

        # steeringPackage
        if p._fullsize != 5000000:
            e = doc.createElement("fullSize")
            ptext = doc.createTextNode(str(p._fullsize))
            e.appendChild(ptext)
            pack.appendChild(e)

        # steeringPackage
        if p._extraexe != {}:
            for it in sorted(p._extraexe.iteritems()):
                e = doc.createElement("extraexe")
                e.setAttribute("name", it[0])
                ptext = doc.createTextNode(it[1][0])
                e.appendChild(ptext)
                pack.appendChild(e)

    return doc


# Main method
if __name__ == '__main__':
    log.setLevel(logging.DEBUG)

    #mc = MainConfigLoader()
    #result = mc.load()
    #pp = pprint.PrettyPrinter(indent=4)
    #pp.pprint(vars(result))

    #mc = ProjectLoader()
    #result = mc.load()
    result = loadProjects()
    pp = pprint.PrettyPrinter(indent=4)
    for p in result:
        pp.pprint(vars(p))


    #packxml = serializePackages(package_list)
    #print packxml.toprettyxml(indent=" ")

    #projxml = serializeProjects(project_list)
    #print projxml.toprettyxml(indent=" ")

    #newconfig = loadMainConfig("../../tests/ExampleLHCbMainConfig.xml")
    #pp = pprint.PrettyPrinter(indent=4)
    #pp.pprint(vars(newconfig))

    #projects = loadProjects("../../tests/ExampleProjectConfig.xml")
    #for p in projects:
    #    print p

    #packages = loadPackages("../../tests/ExamplePackageConfig.xml")
    #for p in packages:
    #    print p.Name()


