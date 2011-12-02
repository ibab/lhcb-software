""" Module to load Packages and Projects from XML configuration"""

import logging
import inspect
import pprint
import sys, os

from xml import sax
from xml.dom.minidom import Document

from Project import ProjectConf
from Project import project_list

from Package import PackageConf
from Package import package_list

log = logging.getLogger()
logging.basicConfig()
log.setLevel(logging.CRITICAL)


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

    def getProjectList(self):
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

    def getPackageList(self):
        """ Returns the lit of projects after parsing """
        return self.packageList

    # creates a ProjectConf instance based on the name and short names retrieved
    def createPackage(self, name, hat, projectName):
        """ Create the ProjectConf """
        # TODO: Set short name, ignored for the moment
        # Not set method on the ProjectConf at the moment
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
                boolvalue = False

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
def findMethod(object, prefix, partialMethodName):
    """ Find the method named prefix + partialMethodName in the object instance """
    allmethods = inspect.getmembers(object, predicate=inspect.ismethod)
    foundMethod = None
    methodPrefixes = ["set"]
    for m in allmethods:
            if m[0].lower() == (prefix + partialMethodName.lower()):
                log.debug("## Found Method " + m[0])
                foundMethod = m[1]
    return foundMethod



# Main method for loading projects based on XML file
def loadProjects(filename):
    """ Load projects from the specified file """
    parser = sax.make_parser()
    handler = SAXProjectHandler()
    parser.setContentHandler(handler)
    parser.parse(filename)
    return handler.getProjectList()


# Main method for loading packages based on XML file
def loadPackages(filename):
    """ Load packages from the specified file """
    parser = sax.make_parser()
    handler = SAXPackageHandler()
    parser.setContentHandler(handler)
    parser.parse(filename)
    return handler.getPackageList()


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
# TODO: To be replaced by a test
if __name__ == '__main__':
    log = logging.getLogger()
    logging.basicConfig()
    log.setLevel(logging.DEBUG)

    #packxml = serializePackages(package_list)
    #print packxml.toprettyxml(indent=" ")

    #projxml = serializeProjects(project_list)
    #print projxml.toprettyxml(indent=" ")


    projects = loadProjects("../../tests/ExampleProjectConfig.xml")
    for p in projects:
        print p

    #packages = loadPackages("../../tests/ExamplePackageConfig.xml")
    #for p in packages:
    #    print p.Name()


