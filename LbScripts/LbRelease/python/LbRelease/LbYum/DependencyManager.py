'''

DependencyManager

Parsing of YUM metadata files to retrieve the available packages
and their dependencies.

for example to list all versions of BRUNEL in the repository:

  client = LbYumClient(os.environ['MYSITEROOT'])
  for p in client.listRPMPackages("BRUNEL.*"):
        print "%s - %s" % (p.name, p.version)

Created on Jun 29, 2012
@author: Ben Couturier
'''

import xml.dom.minidom
import logging
import gzip
import inspect
import os
import re
import urllib

# Constants for directory structure
SVAR = "var"
SCACHE = "cache"
SLBYUM = "lbyum"
SETC = "etc"
SLBYUMCONF = "lbyum.conf"

# List of packages to ignore for our case
IGNORED_PACKAGES = ["rpmlib(CompressedFileNames)", "/bin/sh", "rpmlib(PayloadFilesHavePrefix)",
                    "rpmlib(PartialHardlinkSets)"]

__RCSID__ = "$Id$"

# Setting up the logger
log = logging.getLogger()

# Classes representing requirements and functionality provided by RPMs
###############################################################################
class VersionedObject(object):
    """ Ancestor for classes representing all versioned objects (Provides, Requires...)"""

    def __init__(self, name, version, release, epoch, flags):
        """ Constructor for the class """
        self.name = name
        self.version = version
        self.release = release
        self.epoch = epoch
        self.flags = flags
        self.standardVersion = VersionedObject.getStandardVersion(version)

    # version comparison methods used for dynamic lookup
    ###############################################################################
    def eq(self, x): # IGNORE:C0103
        """ Custom operator """
        return self == x
    def lt(self, x): # IGNORE:C0103
        """ Custom operator """
        return x < self
    def le(self, x): # IGNORE:C0103
        """ Custom operator """
        return x <= self
    def gt(self, x): # IGNORE:C0103
        """ Custom operator """
        return x > self
    def ge(self, x): # IGNORE:C0103
        """ Custom operator """
        return x >= self

    @classmethod
    def getStandardVersion(cls, version):
        """ parse the version and return the list of major, minor,etc version numbers """

        # First check parameters
        if version == None:
            return None

        # Old algo which leads to issues:
        # If the version is of the form: x.y.z
        # return an array with version numbers,
        # e.g. [ 1, 23, 45 ]
        # None otherwise.
        # If it is defined it should be used for comparing versions,
        # otherwise the version strings will be compared
        #if re.match('\d+(\.\d+)*$', version):
        #    standardVersion =  [ int(n) for n in version.split(".") ]

        # We just split with the points and do numerical comparison when we can...
        standardVersion =  [ n for n in version.split(".") ]
        return standardVersion

    @classmethod
    def cmpStandardVersion(cls, v1, v2):
        """ Common method for comparing standard versions as arrays of numbers """
        # zip actually shotens to the length of the shorter list
        # This is ok in our case
        zippedVers = zip(v1, v2)
        cmplist = [  VersionedObject._cmpUtil(a, b) for (a, b) in zippedVers
                   if VersionedObject._cmpUtil(a, b) != 0]
        if len(cmplist) == 0:
            return 0
        else:
            return cmplist[0]

    @classmethod
    def _cmpUtil(cls, a, b): #IGNORE:C0103
        """ Utility function that compares strings
        in the following manner:
        - if both string represent numbers, perform an integer comparison
        - otherwise consider both as strings..."""
        if a.isdigit() and b.isdigit():
            return int(a) - int(b)
        else:
            return cmp(a, b)

    def provideMatches(self, provide):
        """ returns true if the provide passed in parameter matches the requirement """
        # Checking the name of course....
        if provide.name != self.name:
            return False

        if self.version == None or (self.version != None and len(self.version) == 0):
            return True

        allmethods = inspect.getmembers(self, predicate=inspect.ismethod)
        foundMethod = None
        methodname = self.flags.lower()
        for m in allmethods: #IGNORE:C0103
            if m[0].lower() == methodname:
                foundMethod = m[1]

        return(foundMethod(provide))

    def __cmp__(self, other):
        """ Comparison method for dependencies """
        if other == None:
            return -1

        # ordering by name if they are different
        if self.name != other.name:
            return cmp(self.name, other.name)
        else:
            # At this point we can compare the versions
            if self.standardVersion == None or other.standardVersion == None:
                # We couldn't parse the version as a standard x.y.z for both
                # In this case we do string comparison
                return cmp(self.version, other.version)
            else:
                # In this case we can compare the version lists
                cmpVers =  VersionedObject.cmpStandardVersion(self.standardVersion, other.standardVersion)
                if cmpVers != 0:
                    # Versions are different we do not need to compare release numbers
                    return cmpVers
                else:
                    # Comparing down to the release numbers
                    r1 = self.release
                    r2 = other.release

                        # If one is missing a release number they potentially match
                    if not r1 or not r2:
                        return 0

                    if r1.isdigit() and r2.isdigit():
                        return cmp(int(r1), int(r2))
                    else:
                        return cmp(r2, r2)

    # Pretty printing
    ###############################################################################
    def __str__(self):
        """ Display function for the package instance """
        return "%s(%s.%s-%s)" % (self.flags, self.name, self.version, self.release)

    def __repr__(self):
        return self.__str__()

# Classes actually representing the Require and Provide of the RPM Specs
###############################################################################
class Provides(VersionedObject):
    """ Class representing a functionality provided by a package """
    def __init__(self, name, version, release, epoch=None, flags="EQ", package=None):
        super( Provides, self ).__init__(name, version, release, epoch, flags)
        # Provides can actually know which package they provide for
        # This is useful for looking for packages in the repository
        self.package = package

class Requires(VersionedObject):

    """ Class representing a functionality required by a package """
    def __init__(self, name, version, release, epoch=None, flags="EQ", pre=None):
        super( Requires, self ).__init__(name, version, release, epoch, flags)
        self.pre = pre

# Package: Class representing a package available for installation
###############################################################################
class Package(VersionedObject): #IGNORE:R0902
    """ Class representing an RPM package in the repository """
    #
    # Constructor and public method
    ###########################################################################
    def __init__(self, name=None, version=None, release=None, epoch=None, flags=None, #IGNORE:R0913
                 group=None, arch=None, location=None, provides=None, requires=None):
        """ Default constructor """
        super( Package, self ).__init__(name, version, release, epoch, flags)
        self.group = group
        self.arch = arch
        self.location = location
        if requires != None:
            self.requires = requires
        else:
            self.requires = []
        if provides != None:
            self.provides = provides
        else:
            self.provides = []
        self.repository = None

    def setRepository(self, repo):
        """ Sets the instance repository """
        self.repository = repo

    def rpmName(self):
        """ Formats the name of the RPM package"""
        return "%s-%s-%s" % ( self.name, self.version, self.release)

    def rpmFileName(self):
        """ Formats the name of the RPM package"""
        return "%s-%s-%s.rpm" % ( self.name, self.version, self.release)

    def url(self):
        """ Returns the URL to download the file """
        return self.repository.repourl + "/" + self.location

    def __eq__(self, other):
        if other == None:
            return False
        return self.name == other.name \
               and self.version == other.version
    
    def __hash__(self):
        return hash((self.name, self.version, self.group, self.arch))

    #
    # Methods for pretty display
    ###########################################################################
    def __repr__(self):
        return self.__str__()

    def __str__(self):
        """ Display function for the package instance """
        tmpstr = "Package: %s-%s-%s\t%s" % ( self.name, self.version, self.release, self.group)
        if len(self.provides) > 0:
            tmpstr += "\nProvides:\n"
            for pr in self.provides:
                tmpstr += "\t%s-%s-%s\n" % (pr.name, pr.version, pr.release)
        if len(self.requires) > 0:
            tmpstr += "\nRequires:\n"
            for req in self.requires:
                tmpstr += "\t%s-%s-%s\t%s\n" % (req.name, req.version, req.release, req.flags)
        return tmpstr

#
# Repository: Facade in front of classes managing the various DB types
#
###############################################################################
class Repository(object):
    """ Class representing a yum repository with all associated metadata"""

    def __init__(self, name, url, repocachedir, backendList, setupBackend = True, checkForUpdates = True):
        # These are hardwired dependencies in RPM.
        # we do not need to care about them...
        log.info("Initializing repository: %s / %s" % (name, url))

        # URL of the Yum repository and associated files
        self.name = name
        self.repourl = url
        self.repomdurl = self.repourl + "/repodata/repomd.xml"
        self.localRepomdXml = os.path.join(repocachedir, "repomd.xml")

        # Cache directory and file names
        self.cachedir = repocachedir
        if not os.path.exists(self.cachedir):
            os.makedirs(self.cachedir)

        # Now initializing the backend
        self.availableBackends = backendList
        self.backend = None
        self.findLatestMatchingName = None
        self.findLatestMatchingRequire = None
        self.getAllPackages = None

        # Loading the appropriate backend
        # if requested
        if setupBackend:
            if checkForUpdates:
                self.setupBackendFromRemote()
            else:
                self.setupBackendFromLocal()
        else:
            log.warning("Backend NOT setup as requested")

    def setupBackendFromRemote(self):
        """ Checks which back-end should be used, and update DB files. """

        # first get repository metadata with the list of available files
        (data, remotemd) = self._getRemoteMetadata()

        localmd  = self._getLocalMetadata()
        backend = None
        for ba in self.availableBackends:
            log.info("Checking availability of interface: %s" % ba.__name__)
            try:
                try:
                    (checksum, timestamp, filename) = remotemd[ba.yumDataType()] #@UnusedVariable IGNORE:W0612
                except KeyError:
                    log.warning("Remote repository does not provide %s DB" % ba.__name__)
                    continue

                # A priori we have a match (KeyError otherwise)
                backend = ba(self)
                self.backend = backend
                ltimestamp = None
                try:
                    (lchecksum, ltimestamp, lfilename) =  localmd[ba.yumDataType()] #@UnusedVariable IGNORE:W0612
                except: #IGNORE:W0702
                    pass
                    # Doesn't matter, we download DB in this case

                if not self.backend.hasDB() or ltimestamp == None or timestamp > ltimestamp:
                    # We need to update the DB in this case
                    furl = self.repourl + "/" + filename
                    log.info("Updating the RPM database for % s" % ba.__name__)
                    self.backend.getLatestDB(furl)
                    # Now saving the metadata to the repomd file
                    ftmp = open(self.localRepomdXml, 'w')
                    ftmp.write(data)
                    ftmp.close()

                # Loading data necessary for the backend
                self.backend.load()

            except: #IGNORE:W0702
                backend = None
                log.warning("Error initializing %s backend - Trying next" % ba.__name__)
            # Stop at first one found
            if backend != None:
                break

        # Now loading the data
        if backend == None:
            raise Exception("No valid backend found")
        else:
            log.info("Repository %s - Chosen backend: %s" % (self.name, backend.mName))
            # Now initializing the methods delegated to the backend
            self.setBackend()


    def setupBackendFromLocal(self):
        """ Checks which back-end should be used, using local cache. """

        localmd  = self._getLocalMetadata()
        backend = None
        for ba in self.availableBackends:
            log.info("Checking availability of interface: %s" % ba.__name__)
            try:
                try:
                    (checksum, timestamp, filename) = localmd[ba.yumDataType()] #@UnusedVariable IGNORE:W0612
                except KeyError:
                    log.warning("Repository %s - Local repository does not provide %s DB - Trying next"
                                % (self.name, ba.__name__))
                    continue

                # A priori we have a match (KeyError otherwise)
                backend = ba(self)
                self.backend = backend
                # Loading data necessary for the backend
                self.backend.load()

            except Exception, e: # IGNORE:W0703
                backend = None
                log.warning("Repository %s - Error: %s" % (self.name, str(e)))
                log.warning("Repository %s - Error initializing %s backend - Trying next" % (self.name, ba.__name__))
            # Stop at first one found
            if backend != None:
                break

        # Now loading the data
        if backend == None:
            raise Exception("No valid backend found")
        else:
            log.info("Repository %s - Chosen backend: %s" % (self.name, backend.mName))
            # Now initializing the methods delegated to the backend
            self.setBackend()


    def setBackend(self, backend = None):
        """ Sets the backend and makes sure the methods are properly delegated """
        # Checing if teh backend needs setting
        if backend != None:
            self.backend = backend

        # Setting up delegation
        self.findLatestMatchingName = self.backend.findLatestMatchingName
        self.findLatestMatchingRequire = self.backend.findLatestMatchingRequire
        self.getAllPackages = self.backend.getAllPackages


    # Tools to get the metadata and check whether it is up to date
    ###########################################################################
    def _getRemoteMetadata(self):
        """ Gets the remote repomd file """
        log.info("NET - Getting remote metadata for %s" % self.name)
        import urllib2
        ret = None
        response = urllib2.urlopen(self.repomdurl)
        data = response.read()
        if data != None:
            ret = self._checkRepoMD(data)
        response.close()
        return (data, ret)

    def _getLocalMetadata(self):
        """ Gets the remote repomd file """
        log.debug("LOC - Getting local metadata for %s" % self.name)
        ret = None
        if os.path.exists(self.localRepomdXml):
            ftmp = open(self.localRepomdXml, 'r')
            data = ftmp.read()
            if data != None:
                ret = self._checkRepoMD(data)
            ftmp.close()
        return ret

    @classmethod
    def _checkRepoMD(cls, repomdxml):
        """ Method to parse the Repository metadata XML file """
        dom = xml.dom.minidom.parseString(repomdxml)
        dbTimestamps = {}
        for nd in dom.documentElement.childNodes:
            if nd.nodeType == xml.dom.Node.ELEMENT_NODE and nd.tagName == "data":
                checksum = None
                timestamp = None
                location = None
                fileType = nd.getAttribute("type")
                for nc in nd.childNodes:
                    if nc.nodeType == xml.dom.Node.ELEMENT_NODE and nc.tagName == "checksum":
                        checksum = RepositoryXMLBackend.getNodeText(nc)
                    if nc.nodeType == xml.dom.Node.ELEMENT_NODE and nc.tagName == "timestamp":
                        timestamp = RepositoryXMLBackend.getNodeText(nc)
                    if nc.nodeType == xml.dom.Node.ELEMENT_NODE and nc.tagName == "location":
                        location = nc.getAttribute("href")
                dbTimestamps[fileType] = (checksum, timestamp, location)
        return dbTimestamps

#
# Class handling the YUM primary.xml..gz files
#
###########################################################################
class RepositoryXMLBackend(object):
    """ Class interfacing with the XML interface provided by Yum repositories """

    def __init__(self, repository):
        self.mName = "RepositoryXMLBackend"
        self.mPackages = {}
        self.mProvides = {}
        self.mPackageCount = 0
        self.mDBName = "primary.xml.gz"
        self.mPrimary = os.path.join(repository.cachedir, self.mDBName)
        self.mRepository = repository

    #
    # Public Interface for the XML Backend
    ###########################################################################
    def getLatestDB(self, url):
        """ Dowload the DB from the server """
        log.info("NET - Downloading latest version of XML DB")
        urllib.urlretrieve (url, self.mPrimary)

    def hasDB(self):
        """ Check whether the DB is there """
        return os.path.exists(self.mPrimary)

    def load(self):
        """ Actually load the data """
        self._loadYumMetadataFile(self.mPrimary)

    @classmethod
    def yumDataType(cls):
        """ Returns the ID for the data type as used in the repomd.xml file """
        return "primary"

    def findLatestMatchingName(self, name, version, release=None):
        """ Utility function to locate a package by name, returns the latest available version """
        package = None
        try:
            availableVersions = self.mPackages[name]
            if availableVersions != None:
                if (version == None or len(version)==0) and len(availableVersions) > 0:
                    # returning latest
                    package = sorted(availableVersions)[-1]
                else:
                    # Trying to match the requirements and what is available
                    req = Requires(name, version, release)
                    matching = [ p for p in availableVersions if req.provideMatches(p) ]
                    if len(matching) > 0:
                        package = sorted(matching)[-1]

        except KeyError:
            log.debug("Could not find package %s" % (name))

        # Checking whether we actually found something
        if package == None:
            log.debug("Could not find package %s.%s-%s" % (name, version, release))

        return package

    def findLatestMatchingRequire(self, requirement):
        """ Utility function to locate a package providing a given functionality """

        log.debug("Looking for match for %s" % requirement)
        if requirement == None:
            raise Exception("_findPackageMatchingRequire passed Null requirement")

        package = None
        try:
            availableVersions = self.mProvides[requirement.name]
            if availableVersions != None:
                if (requirement.version == None or len(requirement.version)==0) and len(availableVersions) > 0:
                    availableVersions.sort()
                    # If no version is specified we just return the latest one
                    log.debug("Found %d versions - returning latest: %s"
                              % (len(availableVersions), availableVersions[-1] ))
                    package = availableVersions[-1].package
                else:
                    # Trying to match the requirements and what is available
                    matching = [ p for p in availableVersions if requirement.provideMatches(p) ]
                    if len(matching) > 0:
                        matching.sort()
                        log.debug("Found %d version matching - returning latest: %s"
                                  % (len(matching),matching[-1] ))
                        package = matching[-1].package
        except KeyError:
            log.debug("Could not find package providing %s-%s" % (requirement.name, requirement.version))

        if package == None:
            log.debug("Could not find package providing %s-%s" % (requirement.name, requirement.version))
        return package

    def getAllPackages(self, nameMatch=None):
        """ Yields the list of all packages known by the repository """
        for pak_list_k in self.mPackages.keys():
            if nameMatch == None or (nameMatch != None and re.match(nameMatch, pak_list_k) != None):
                for pa in self.mPackages[pak_list_k]:
                    yield pa

    #
    # Methods for XML parsing and data loading
    ###########################################################################
    @classmethod
    def _fromYumXML(cls, packageNode): #IGNORE:R0912
        """ Method that instantiates a correct package instance, based on
        the YUM Metadata XML structure"""

        # First checking the node passed, just in case
        if packageNode.nodeType == xml.dom.Node.ELEMENT_NODE and packageNode.tagName != "package":
            raise Exception("Trying to create Package from wrong node" + str(packageNode))

        lp = Package()
        for cn in packageNode.childNodes:
            if cn.nodeType != xml.dom.Node.ELEMENT_NODE:
                continue
            if cn.tagName == "name":
                lp.name = RepositoryXMLBackend.getNodeText(cn)
            elif cn.tagName == "arch":
                lp.arch = RepositoryXMLBackend.getNodeText(cn)
            elif cn.tagName == "version":
                lp.version = cn.getAttribute("ver")
                lp.release = cn.getAttribute("rel")
                lp.epoch = cn.getAttribute("epoch")
            elif cn.tagName == "location":
                lp.location = cn.getAttribute("href")
            elif cn.tagName == "format":
                for fnode in cn.childNodes:
                    if fnode.nodeType != xml.dom.Node.ELEMENT_NODE:
                        continue
                    if fnode.tagName == "rpm:group":
                        lp.group =  RepositoryXMLBackend.getNodeText(fnode)
                    if fnode.tagName == "rpm:provides":
                        for dep in fnode.childNodes:
                            if dep.nodeType != xml.dom.Node.ELEMENT_NODE:
                                continue
                            depname = dep.getAttribute("name")
                            depver = dep.getAttribute("ver")
                            deprel = dep.getAttribute("rel")
                            depepoch = dep.getAttribute("epoch")
                            depflags = dep.getAttribute("flags")
                            lp.provides.append(Provides(depname, depver, deprel,
                                                       depepoch, depflags, lp))
                    if fnode.tagName == "rpm:requires":
                        for dep in fnode.childNodes:
                            if dep.nodeType != xml.dom.Node.ELEMENT_NODE:
                                continue
                            depname = dep.getAttribute("name")
                            depver = dep.getAttribute("ver")
                            deprel = dep.getAttribute("rel")
                            depepoch = dep.getAttribute("epoch")
                            depflags = dep.getAttribute("flags")
                            deppre = dep.getAttribute("pre")
                            lp.requires.append(Requires(depname, depver, deprel,
                                                       depepoch, depflags, deppre))

        # Set the "standard version field, used for comparison
        lp.standardVersion = VersionedObject.getStandardVersion(lp.version)
        # Now return the object back...
        return lp

    @classmethod
    def getNodeText(cls, node):
        """ Gets the value of the first child text node """
        for tn in node.childNodes:
            if tn.nodeType == xml.dom.Node.TEXT_NODE:
                return tn.data

    def _loadYumMetadataFile(self, filename):
        """ Loads the yum XML package list """
        fi = gzip.open(filename, 'rb')
        try:
            log.debug("Starting the parsing of the Metadata XML file")
            dom = xml.dom.minidom.parse(fi)
            log.debug("Parsing of the Metadata XML file done")
            self._loadYumMetadataDOM(dom)
        except Exception, e:
            log.error("Error while parsing file %s: %s" % (filename, str(e)))
            raise e
        fi.close()

    def _loadYumMetadataDOM(self, dom):
        """ Loads the yum XML package list """
        # Finding all packages and adding then to the repository
        log.debug("Starting to iterate though Metadata DOM")
        for nd in dom.documentElement.childNodes:
            if nd.nodeType == xml.dom.Node.ELEMENT_NODE:
                # Generating the package object from the XML
                pa = RepositoryXMLBackend._fromYumXML(nd)
                pa.setRepository(self.mRepository)
                # Adding the package to the repository
                self._addPackage(pa)
                self._addAllProvides(pa)
                self.mPackageCount += 1
                log.debug("Added %s package <%s><%s><%s>" % (pa.group, pa.name, pa.version, pa.release))

                # Checking the Package type...
                if nd.getAttribute("type") != "rpm":
                    log.warning("Package type for %s is %s not RPM" % (pa.name, nd.getAttribute("type")))

        log.debug("Finished to iterate though Metadata DOM")

    def _addPackage(self, package):
        """ Adds a package to the repository global list """
        try:
            allversions = self.mPackages[package.name]
        except KeyError:
            allversions = []
        allversions.append(package)
        self.mPackages[package.name] = allversions

    def _addAllProvides(self, package):
        """ Adds a package to the map with the list of provides """
        for prov in package.provides:
            if prov.name not in IGNORED_PACKAGES:
                try:
                    allprovides = self.mProvides[prov.name]
                except KeyError:
                    allprovides = []
                allprovides.append(prov)
                self.mProvides[prov.name] = allprovides

#
# Class handling the YUM primary.sqlite.bz2 files
#
###########################################################################
class RepositorySQLiteBackend(object):
    """ Class interfacing with the SQLite interface provided by Yum repositories """

    def __init__(self, repository):
        self.mName = "RepositorySQLiteBackend"
        self.mDBName = "primary.sqlite.bz2"
        self.mDBNameUncompressed = "primary.sqlite"
        self.mPrimary = os.path.join(repository.cachedir, self.mDBName)
        self.mPrimaryUncompressed = os.path.join(repository.cachedir, self.mDBNameUncompressed)
        self.mRepository = repository
        self.mDBConnection = None
    #
    # Public interface
    ###########################################################################
    @classmethod
    def yumDataType(cls):
        """ Returns the ID for the data type as used in the repomd.xml file """
        return "primary_db"

    def hasDB(self):
        """ Check whether the DB is there """
        return os.path.exists(self.mPrimary)

    def getLatestDB(self, url):
        """ Download the latest DB from the server """
        log.debug("NET - Downloading latest version of SQLite DB")
        urllib.urlretrieve (url, self.mPrimary)

        log.debug("Decompressing latest version of SQLite DB")
        if os.path.exists(self.mPrimaryUncompressed):
            os.unlink(self.mPrimaryUncompressed)
        self._decompressDB()

    def load(self):
        """ Actually load the data """
        # Checking if we need to uncompress the DB again
        if not os.path.exists(self.mPrimaryUncompressed):
            self._decompressDB()

        # Import the the package and open DB connection
        #try:
        import sqlite3 as sql #@UnresolvedImport @Reimport #IGNORE:F0401 #@UnusedImport
        #except: #IGNORE:W0702
        #    import sqlite as sql #@UnresolvedImport @Reimport #IGNORE:F0401

        self.mDBConnection = sql.connect(self.mPrimaryUncompressed)

    def findLatestMatchingName(self, name, version, release=None):
        """ Utility function to locate a package by name, returns the latest available version """
        package = None
        found = self._loadPackagesByName(name, version)
        if len(found) > 0:
            req = Requires(name, version, release)
            matching = [p for p in found if req.provideMatches(p)]
            if len(matching) > 0:
                package = matching[-1]
        return package

    def getAllPackages(self, nameMatch=None):
        """ Yields the list of all packages known by the repository """
        cursor = self.mDBConnection.cursor()
        # request to find the entry in the package table
        sq = """select pkgkey, name, version, release, epoch, rpm_group, arch, location_href
             from packages """
        res = cursor.execute(sq)

        # Getting the results
        for (pkgkey, pname, version, release, epoch, rpm_group, arch, location_href) in res:
            # Creating the package object
            if nameMatch == None or (nameMatch != None and re.match(nameMatch, pname) != None):
                pa = Package(pname, version, release, epoch, None, rpm_group, arch, location_href)
                # Now getting the provides and requires
                pa.requires = self._loadRequiresByKey(pkgkey)
                pa.provides = self._loadProvidesByKey(pkgkey, pa)
                pa.setRepository(self.mRepository)
                # Now yield this
                yield(pa)

        # Free resources
        cursor.close()

    def findLatestMatchingRequire(self, requirement):
        """ Utility function to locate a package providing a given functionality """

        log.debug("Looking for match for %s" % requirement)
        if requirement == None:
            raise Exception("_findPackageMatchingRequire passed Null requirement")
        package = None

        # List of all provides with the same name
        #(we do version comparison in python)
        allprovides = self._findProvidesByName(requirement.name)
        matching = [ pr for pr in allprovides if requirement.provideMatches(pr)]

        # Now lookup the matching package
        if len(matching) > 0:
            prov = sorted(matching)[-1]
            allpackages = self._loadPackageProviding(prov)
            package = sorted(allpackages)[-1]
        return package

    #
    # Utility methods
    ###########################################################################
    def _loadProvidesByKey(self, pkgkey, package):
        """ Loads provide list given a package key """
        allprovides = []
        cursorSub = self.mDBConnection.cursor()
        sqprov = "select name, flags, epoch, version, release from provides where pkgkey=?"
        resprov = cursorSub.execute(sqprov, [ pkgkey ])
        for (name, flags, epoch, version, release) in resprov:
            prov = Provides(name, version, release, epoch, flags, package)
            allprovides.append(prov)
        cursorSub.close()
        return allprovides

    def _loadRequiresByKey(self, pkgkey):
        """ Loads requires list given a package key """
        allrequires = []
        cursorSub = self.mDBConnection.cursor()
        sqlreq = "select name, flags, epoch, version, release, pre from requires where pkgkey=?"
        respreq = cursorSub.execute(sqlreq, [ pkgkey ])
        for (name, flags, epoch, version, release, pre) in respreq:
            if flags == None:
                flags = "EQ"
            req = Requires(name, version, release, epoch, flags, pre)
            allrequires.append(req)
        cursorSub.close()
        return allrequires

    def _findProvidesByName(self, name):
        """ Find all provides with a given name """
        allprovides = []
        cursorSub = self.mDBConnection.cursor()
        sqprov = "select pkgkey, name, flags, epoch, version, release from provides where name=?"
        resprov = cursorSub.execute(sqprov, [ name ])
        for (pkgkey, name, flags, epoch, version, release) in resprov: #@UnusedVariable IGNORE:W0612
            prov = Provides(name, version, release, epoch, flags, None)
            allprovides.append(prov)
        cursorSub.close()
        return allprovides

    def _loadPackagesByName(self, name, version=None):
        """ Lookup packages with a given name """
        allpackages = []
        cursor = self.mDBConnection.cursor()
        # request to find the entry in the package table
        sq = """select pkgkey, name, version, release, epoch, rpm_group, arch, location_href
             from packages where name = ? """
        if version != None:
            sq += " and version = ? "
            res = cursor.execute(sq, [ name, version])
        else:
            res = cursor.execute(sq, [ name ])

        # Getting the results
        for (pkgkey, pname, version, release, epoch, rpm_group, arch, location_href) in res:
            # Creating the package object
            pa = Package(pname, version, release, epoch, None, rpm_group, arch, location_href)
            pa.setRepository(self.mRepository)

            # Now getting the provides and requires
            pa.requires = self._loadRequiresByKey(pkgkey)
            pa.provides = self._loadProvidesByKey(pkgkey, pa)
            #and append to thelist
            allpackages.append(pa)
        cursor.close()
        return allpackages

    def _loadPackageProviding(self, provide):
        """ Lookup packages with a given name """
        allpackages = []
        cursor = self.mDBConnection.cursor()
        # request to find the entry in the package table
        sq = """select p.pkgkey, p.name, p.version, p.release, p.epoch, p.rpm_group, p.arch, p.location_href
             from packages p, provides r
             where p.pkgkey = r.pkgkey
             and r.name = ?
             and r.version = ? """

        if provide.release != None:
            sq += " and r.release = ? "
            res = cursor.execute(sq, [ provide.name, provide.version, provide.release])
        else:
            res = cursor.execute(sq, [ provide.name, provide.version])

        # Getting the results
        for (pkgkey, pname, version, release, epoch, rpm_group, arch, location_href) in res:
            # Creating the package object
            pa = Package(pname, version, release, epoch, None, rpm_group, arch, location_href)
            pa.setRepository(self.mRepository)

            # Now getting the provides and requires
            pa.requires = self._loadRequiresByKey(pkgkey)
            pa.provides = self._loadProvidesByKey(pkgkey, pa)
            #and append to the list
            allpackages.append(pa)
        cursor.close()
        return allpackages

    def _decompressDB(self):
        """ Uncompress DB file to be able to open it with SQLLite """
        import bz2, shutil
        primaryBz2 = bz2.BZ2File(self.mPrimary, 'rb')
        primaryUncomp = file(self.mPrimaryUncompressed, 'wb')
        shutil.copyfileobj(primaryBz2, primaryUncomp)
        primaryBz2.close()
        primaryUncomp.close()

#
# LbYumClient: CLass that parses the Yum metadata and manages the repositories
#
###############################################################################
class LbYumClient(object):
    """ Client class to be invoked to to query RPMs """
    def __init__(self, localConfigRoot, checkForUpdates=True, manualConfig = False):
        """ Constructor for the client """
        # Basic initializations
        self.localConfigRoot = localConfigRoot
        self.etcdir = os.path.join(localConfigRoot, SETC)
        self.lbyumcache = os.path.join(localConfigRoot, SVAR, SCACHE, SLBYUM)
        self.yumconf = os.path.join(self.etcdir, "yum.conf")
        self.yumreposdir = os.path.join(self.etcdir, "yum.repos.d")
        self.configured = False
        self.repositories = {}
        self.repourls = {}

        # In this case the repository configuration will be done by hand
        # This is useful for testing...
        if manualConfig:
            return

        # Loading the config and setting URLs accordingly
        repourls = self._loadConfig()

        # At this point we have the repo names and URLs in self.repourls
        # we know connect to them to get the best method to get the appropriate files
        self._initializeRepositories(repourls, checkForUpdates, [ RepositorySQLiteBackend, RepositoryXMLBackend ])

    def addRepository(self, repository):
        """ Adds a repository manually, useful for testing """
        self.repositories[repository.name] = repository
        self.repourls[repository.name] = repository.repourl

    #
    # Public methods
    ###########################################################################
    def findLatestMatchingName(self, name, version=None, release=None):
        """ Main method for locating packages by RPM name"""
        allfound = []
        package = None

        # Looking for matches in all repos
        for rep in self.repositories.values():
            log.debug("Searching RPM %s/%s/%s in %s" % (name, version, release, rep.name))
            res = rep.findLatestMatchingName(name, version, release)
            if res != None:
                allfound.append(res)

        # Sorting to get latest one
        allfound.sort()
        if len(allfound) > 0:
            package = allfound[-1]

        return package

    def listPackages(self, nameRegexp=None, versionRegexp=None, releaseRegexp=None):
        """ List packages available"""
        # Looking for matches in all repos
        for rep in self.repositories.values():
            for pa in rep.getAllPackages():
                namematch = True
                versionmatch = True
                releasematch = True
                if nameRegexp != None and re.match(nameRegexp, pa.name) == None:
                    namematch = False
                if versionRegexp != None and re.match(versionRegexp, pa.name) == None:
                    versionmatch = False
                if releaseRegexp != None and re.match(releaseRegexp, pa.name) == None:
                    releasematch = False
                if namematch and versionmatch and releasematch:
                    yield pa

    def findLatestMatchingRequire(self, requirement):
        """ Main method for locating packages by RPM name"""
        allmatching = []
        matchingPackage = None

        # Looking for matches in all repos
        for rep in self.repositories.values():
            ma = rep.findLatestMatchingRequire(requirement)
            if ma != None:
                allmatching.append(ma)

        # Sorting to get latest one
        allmatching.sort()
        if len(allmatching) > 0:
            matchingPackage = allmatching[-1]

        return matchingPackage

    #
    # Dependency management
    ###########################################################################
    def getAllPackagesRequired(self, package):
        """ Get allpackages needed for installation (including the package itself)"""
        deps = self.getPackageDependencies(package)
        deps.append(package)
        return deps

    def getPackageDependencies(self, package):
        """ Get all dependencies for the package (excluding the package itself)"""
        log.info("Checking dependencies for %s.%s-%s" % (package.name, package.version, package.release))
        return list(self._getpackageDeps(package, set()))

    def _getpackageDeps(self, package, alreadyprocessed):
        """ Internal method to recurse on dependencies """
        # Now iterating on all requires to find the matching requirement
        alreadyprocessed.add(package)
        requiredlist = set()
        for req in package.requires:
            (reqPackage, reqVersion, reqRelease) = (req.name, req.version, req.release)
            if reqPackage not in IGNORED_PACKAGES:
                log.debug("Processing deps %s.%s-%s" % (reqPackage, reqVersion, reqRelease))
                pa = self.findLatestMatchingRequire(req)

                # Check for circular dependencies using the set passed
                if pa in alreadyprocessed:
                    #log.warning("Cyclic dependency in repository with package: %s" % pa.name)
                    continue

                if pa != None:
                    requiredlist.add(pa)
                    # Then adding the children...
                    for subreq in self._getpackageDeps(pa, alreadyprocessed):
                        if subreq not in requiredlist:
                            requiredlist.add(subreq)
                else:
                    if reqPackage not in IGNORED_PACKAGES:
                        log.error("Package %s.%s-%s not found" % (reqPackage, reqVersion, reqRelease))
                        #raise Exception("Package %s.%s-%s not found" % (reqPackage, reqVersion, reqRelease))
        return requiredlist

    #
    # Configuration parsing helpers
    ###########################################################################
    def _loadConfig(self):
        """ Look up the location of the yum repository """
        log.debug("Loading the configs from repository: %s" % self.yumreposdir)

        for fi in os.listdir(self.yumreposdir):
            # Checking that we have a file indeed, and if the name matches, parse the content
            ma = re.match("(.*)\.repo$", fi)
            if ma != None:
                repourls = self._parseRepoConfigFile(os.path.join(self.yumreposdir, fi))
                if len(repourls.keys()) == None:
                    log.warning("Could not read repository URL from %s" % fi)
                else:
                    for (key, val) in repourls.items():
                        self.repourls[key] = val

        self.configured = True
        # Checking whether we found any repositories at all
        if len(self.repourls.keys())==0:
            raise Exception("Could not find repository config in %s" % self.yumreposdir)

        return self.repourls

    @classmethod
    def _parseRepoConfigFile(cls, filename):
        """ Parses the xxxx.repo file and returns a map with reponame/repourl """
        log.debug("Opening config file for repo: %s" % filename)
        repourls = {}
        currentSection = None
        repourl = None

        ycf = open(filename, 'r')
        for li in ycf.readlines():
            # Ignoring comment lines
            if (re.match("\s*#.*", li) != None):
                continue
            # Look for the repository name (syntax: [name])
            secm = re.match("\s*\[(.*)\]", li)
            if secm != None:
                currentSection = secm.group(1)
                log.debug("Found section: %s" % currentSection)
                # Move to next line...
                continue

            # If we are in a section of the conf file,
            # look for the baseurl attribute
            if currentSection != None:
                ma = re.match("\s*baseurl\s*=\s*(.*)\s*$", li)
                if ma != None:
                    repourl = ma.group(1)
                    log.debug("Found repository: %s %s" % (currentSection, repourl))
                    repourls[currentSection] = repourl
                    # Only one base url per section...
                    currentSection = None
        ycf.close()
        return repourls

    def _initializeRepositories(self, repourls, checkForUpdates, knownBackends):
        """ Initilize the repositories given their URLS """

        self.repourls = repourls

        # At this point self.repourls is a map containing the list of repositories and their URLs
        # Iterate on them to setup the repositories
        for repo in self.repourls.keys():
            # Getting the main parameters for the repository:
            # URL and cache directory
            repourl = self.repourls[repo]
            repocachedir = os.path.join(self.lbyumcache, repo)
            if not os.path.exists(repocachedir):
                os.makedirs(repocachedir)

            try:
                rep = Repository(repo, repourl, repocachedir, knownBackends, True, checkForUpdates)
                log.debug("Repository %s created" % repo)
                # Now adding the repository to the map
                self.repositories[repo] = rep
            except Exception, e: # IGNORE:W0703
                log.error("ERROR - Banning repository %s: %s" % (repo, e))

if __name__ == '__main__':
    FORMAT = '%(asctime)-15s %(message)s'
    logging.basicConfig(format=FORMAT)
    log.setLevel(logging.DEBUG)



