'''
Util classes that parse/import the files in ${LHCBTAR}/html
in order to check the dependencies between all the released
versions of LHCb software.

To use:

    distdir = os.path.join(os.environ["LHCBTAR"])
    p = ProjectFile.loadProject(distdir, "Gaudi", "v22r5")

or:
    file = os.path.join(htmldir,"GAUDI_GAUDI_v22r5_x86_64-slc5-gcc43-opt.html")
    p = ProjectFile.loadFromFile(file)
    print p

Created on Mar 5, 2012
@author: Ben Couturier
'''

import datetime
import logging
import os
import pickle as p
import re

from LbConfiguration import Project

CONFIG_NOARCH = "noarch"
LHCb_PNAME_WITHCONFIG = "^([a-zA-Z0-9]+)_([a-zA-Z0-9]+)_([a-zA-Z0-9]+)_(.*)"
LHCb_PNAME_NOCONFIG ="^([a-zA-Z0-9]+)_([a-zA-Z0-9]+)_([a-zA-Z0-9]+)"


# Util function that extracts name, version and config from the
# archive name as stored in the html file
def parseLHCbArchiveName(name):
    """ Extract project and version name from strings like
    GANGA_GANGA_v506r3_x86_64-slc5-gcc43-opt """
    project = None
    version = None
    config = None
    log = logging.getLogger()

    # First checking the case where there is a CMTCONFIG
    m = re.match(LHCb_PNAME_WITHCONFIG, name)
    if m != None:
        if m.group(1) != m.group(2) and m.group(1) != "TOOLS":
            log.debug("<%s> <%s>" % (m.group(1), m.group(2)))
            raise Exception("Format error: Project name normally repeated in version in %s" % name)
        project = m.group(1)
        version = m.group(3)
        config = m.group(4)
    else:
        # Now checking if it's a file without CMTCONFIG
        m = re.match(LHCb_PNAME_NOCONFIG, name)
        if m != None:
            if m.group(1) != m.group(2) and m.group(1) != "TOOLS":
                log.debug("<%s> <%s>" % (m.group(1), m.group(2)))
                raise Exception("Format error: Project name normally repeated in version in %s" % name)
            project = m.group(1)
            version = m.group(3)
            config = CONFIG_NOARCH
        else:
            raise Exception("Format error: LHCb Archive name: %s does not match expected format" % name)


    if config == None or config == "":
        config = CONFIG_NOARCH
    return (project, version, config)

# Util function that extracts name, version and config from the
# archive name as stored in the html file
def parseExternalArchiveName(name):
    """ Extract project and version name from strings like
    LCGGrid_v4r3_x86_64-slc5-gcc43-opt """
    m = re.match("([a-zA-Z0-9]+)_([a-zA-Z0-9-]+)_(.*)", name)
    if m != None:
        project = m.group(1)
        version = m.group(2)
        config = m.group(3)
        return (project, version, config)
    else:
        m = re.match("osc_vis_([0-9\.]+)_([a-zA-Z0-9]+)_(.*)", name)
        if m != None:
            return ("osc_vis", m.group(1), m.group(2))
        else:
            raise Exception("Format error: External Archive name: %s does not match expected format" % name)


# Util function that extracts name, version and config from the
# archive name as stored in the html file
def parseDataPackageArchiveName(name):
    """ Extract project and version name from strings like
    DBASE_Det_SQLDDDB_v5r31 """
    m = re.match("([\w]+)_([vrp0-9]+)", name)
    if m == None:
        raise Exception("Format error: Data package Archive name: %s does not match expected format" % name)
    project = m.group(1)
    version = m.group(2)
    config = CONFIG_NOARCH
    return (project, version, config)

#
# Utility classes to represent the dependencies
###############################################################################
class Dependency(object):
    """ Class Representing a software dependency """
    def __str__(self):
        return "%s (%s, %s, %s) %s" %(self.type, self.project, self.version, self.config, self.filename)

class LHCbDependency(Dependency):
    """ Class representing dependency to a LHCb project """
    def __init__(self, archive, filename):
        self.archive = archive
        self.filename = filename
        self.type = "LHCb"
        (self.project, self.version, self.config) = parseLHCbArchiveName(archive)

class ExternalDependency(Dependency):
    """ Class representing dependency to external software  """
    def __init__(self, archive, filename):
        self.archive = archive
        self.filename = filename
        self.type = "EXT"
        (self.project, self.version, self.config) = parseExternalArchiveName(archive)

class DataPackageDependency(Dependency):
    """ Class representing dependency to external software  """
    def __init__(self, archive, filename):
        self.archive = archive
        self.filename = filename
        self.type = "DAT"
        (self.project, self.version, self.config) = parseDataPackageArchiveName(archive)

class SystemDependency(Dependency):
    """ Class representing dependency to external software  """
    def __init__(self, archive, filename):
        self.archive = archive
        self.filename = filename
        self.type = "SYS"
        (self.project, self.version, self.config) = (archive, archive, archive)

#
# Class representing a specific HTML file
###############################################################################
class ProjectFile(object):
    """ Class representing a LHCb project file for a given config,
    the associated file and dependencies as represented in the html dependency files """

    @classmethod
    def loadFromFile(cls, distdir, filename):
        """ Factory method that creates the ProjectFile object
        from a specific file """
        # Loads the file content
        obj = cls()
        obj.distdir = distdir
        obj.htmldir = os.path.join(obj.distdir, "html")
        obj.htmlfilename = filename
        obj.htmlbasename = os.path.basename(obj.htmlfilename)
        f = open(filename, "r")
        fileContent = map (lambda s: s.strip(), f.readlines())
        f.close()
        fileContent = [line for line in fileContent if line != None and line != ""]
        #for l in fileContent:
        #    print l
        try:
            obj.parseDepFile(fileContent)
        except:
            print "ERROR parsing: %s - ignoring" % filename
            return None

        return obj

    @classmethod
    def loadProject(cls, distdir, projectName, version, config=None):
        """ Loads the ProjectFile object from the triplet
        project, version, config """
        p = Project.getProject(projectName, svn_fallback=True, raise_exception=False)
        tar_file = p.tarBallName(version, config)
        tar_file_html = tar_file + ".html"
        full_tar_file_html = distdir + os.sep+ "html" + os.sep + tar_file_html
        if not os.path.exists(full_tar_file_html):
            raise Exception(full_tar_file_html + " is missing")
        return cls.loadFromFile(distdir, tar_file_html)

#    def __init__(self, filename):
#        """ Constructor that loads the dependencies from a html file
#        in install_project format """

        # Loads the file content
#        self.htmlfilename = filename
#        f = open(filename, "r")
#        fileContent = map (lambda s: s.strip(), f.readlines())
#        f.close()

#        fileContent = [line for line in fileContent if line != None and line != ""]
#        self.parseDepFile(fileContent)


    def parseDepFile(self, fileContent):
        """ Parse the content of the dependency file """
        # First checking project definition
        if not re.match("^\s*<H3>", fileContent[0]):
            raise Exception("Format error: Dep file should start with H3: %s" % fileContent[0])

        # parsing secong which contains the project name and filename
        m = re.match("<A NAME=([\w/-]+) HREF=(.*)>([\w-]+)</A>", fileContent[1])
        if m == None:
            raise Exception("Format error: Second line should match <A NAME=([\w/-]+) HREF=(.*)>([\w-]+)</A>")
        #if m.group(1) != m.group(3):
        #    raise Exception("Format error: %s should match %s" % (m.group(1), m.group(3)))
        self.archiveName = m.group(3)
        self.filename = m.group(2)

        if self.archiveName.startswith("DBASE") or self.archiveName.startswith("PARAM"):
            (self.project, self.version, self.config) = parseDataPackageArchiveName(self.archiveName)
        elif "LCGCMT" in self.archiveName or "LCGGanga" in self.archiveName or "LCGGrid" in self.archiveName:
            (self.project, self.version, self.config) = parseExternalArchiveName(self.archiveName)
        else:
            (self.project, self.version, self.config) = parseLHCbArchiveName(self.archiveName)

        #print "Processing Project file%s/%s/%s %s" % (project, version, config, self.filename)

        # 3rd line should be <MENU> for projects if there are dependencies to be shown...
        #if len(fileContent) > 3 and not fileContent[2].startswith("<MENU><LI>"):
        #    raise Exception("Format error: 3rd line should be <MENU><LI> instead of %s"  % fileContent[2])


        self.lhcbDeps = []
        self.externalDeps = []
        self.dbaseDeps = []
        self.paramDeps = []
        self.systemDeps = []

        # Now checking the dependencies
        for line in fileContent[2:]:
            if "HREF" not in line:
                continue
                # We just ignore this case, we process HREFs in the bit of code as dependencies are always
                # specified this way
            if line.startswith("[<A HREF=source"):
                # Dependency of externals
                # Processed separately as naming conventions are different...
                # [<A HREF=source/LCGGrid_v4r3_x86_64-slc5-gcc43-opt.tar.gz>LCGGrid_v4r3_x86_64-slc5-gcc43-opt</A>]
                m = re.match("\[<A HREF=(.+)>([\w\.-]+)</A>\].*", line)
                if m == None:
                    raise Exception("Format error: problem parsing external dependency %s" % line)
                self.externalDeps.append(ExternalDependency(m.group(2),m.group(1)))
            elif line.startswith("[<A HREF=system"):
                # Dependency of externals
                # Processed separately as naming conventions are different...
                # [<A HREF=system/slc4_amd64_gcc34.tar.gz>slc4_amd64_gcc34</A>]
                m = re.match("\[<A HREF=(.+)>([\w-]+)</A>\].*", line)
                if m == None:
                    raise Exception("Format error: problem parsing system dependency %s" % line)
                self.externalDeps.append(SystemDependency(m.group(2),m.group(1)))
            elif line.startswith("</MENU>"):
                break # Normally last line in the file...
            elif line.startswith("[<A HREF=DBASE"):
                # Dependency of externals
                # Processed separately as naming conventions are different...
                # [<A HREF=source/LCGGrid_v4r3_x86_64-slc5-gcc43-opt.tar.gz>LCGGrid_v4r3_x86_64-slc5-gcc43-opt</A>]
                m = re.match("\[<A HREF=(.+)>([\w-]+)</A>\].*", line)
                if m == None:
                    raise Exception("Format error: problem parsing DBASE dependency %s" % line)
                self.dbaseDeps.append(DataPackageDependency(m.group(2),m.group(1)))
            elif line.startswith("[<A HREF=PARAM"):
                # Dependency of externals
                # Processed separately as naming conventions are different...
                # [<A HREF=source/LCGGrid_v4r3_x86_64-slc5-gcc43-opt.tar.gz>LCGGrid_v4r3_x86_64-slc5-gcc43-opt</A>]
                m = re.match("\[<A HREF=(.+)>([\w-]+)</A>\].*", line)
                if m == None:
                    raise Exception("Format error: problem parsing PARAM dependency %s" % line)
                self.paramDeps.append(DataPackageDependency(m.group(2),m.group(1)))
            elif line.startswith("[<A HREF=/__slc3_ia32_gcc323.tar.gz>__slc3_ia32_gcc323</A>]"):
                # Ignore this case, this is a problem in file:
                # PANORAMIX_PANORAMIX_v13r5b_slc3_ia32_gcc323.html
                pass
            else:
                # Dependency on normal LHCb Project
                # Processed separately as naming conventions are different...
                # [<A HREF=DIRAC/DIRAC_DIRAC_v5r13p11_x86_64-slc5-gcc43-opt.tar.gz>DIRAC_DIRAC_v5r13p11_x86_64-slc5-gcc43-opt</A>]
                m = re.match("\[<A HREF=(.+)>([\w-]+)</A>\].*", line)
                if m == None:
                    raise Exception("Format error: problem parsing LHCb dependency %s" % line)
                self.lhcbDeps.append(LHCbDependency(m.group(2),m.group(1)))


    def __str__(self):
        s = "ProjectFile (%s,%s,%s) %s\n" % (self.project, self.version, self.config, self.filename)
        for c in self.lhcbDeps, self.externalDeps, self.dbaseDeps, self.paramDeps, self.systemDeps:
            for d in c:
                s+= "    " + str(d) + "\n"
        return s

    def getAllDependenciesFileNames(self):
        """Returns the filenames of ALL dependencies for this project file"""
        depsf = []
        for c in self.lhcbDeps, self.externalDeps, self.dbaseDeps, self.paramDeps, self.systemDeps:
            for d in c:
                depsf.append(d.filename)
        return depsf

    def getAllDependencies(self):
        """ Return all the dependency objects """
        return self.lhcbDeps +self.externalDeps+ self.dbaseDeps+ self.paramDeps+ self.systemDeps

#
# Utility class to load ALL html files and stored the dependency graph
###############################################################################
class LHCbSoftwareDeps(object):
    """ Class containing parsed information about dependencies
    between LHCb projects """

    #
    # Static constructors for the Dependency object
    ###########################################################################

    @classmethod
    def loadDistInfo(cls, distdir):
        """ Static method to load all dependency and file information from AFS """
        deps = cls(distdir)
        deps.parseHTMLDir()
        deps.loadDistFiles()
        return deps

    @classmethod
    def loadFromDB(cls, dbfile):
        """ Static method to load all dependency and file information from AFS """
        deps = cls(None)
        deps.loadFromDisk(dbfile)
        return deps

    #
    # Utilities to build/load the instance
    ###########################################################################

    def __init__(self, distdir = None):
        """ Default constructor, should not be used directly """
        self.projectFiles = []
        self.archiveList = {}
        if distdir != None:
            self.distdir = distdir
            self.htmldir = os.path.join(self.distdir, "html")
        self.log = logging.getLogger()

    def parseHTMLDir(self, htmlDir=None):
        """ Method that parses ALL the HTML files in a directory
        to extract the dependencies """

        if htmlDir == None:
            htmlDir = self.htmldir

        self.log.info("Processing directory: %s" % htmlDir)
        allfiles = [f for f in os.listdir(htmlDir) if f.endswith("html")]
        for i, f in enumerate(allfiles):
            self.log.debug("%d - Processing %s" % (i,f))
            pf = ProjectFile.loadFromFile(self.distdir, htmlDir + os.sep + f)
            if pf != None:
                self.projectFiles.append(pf)
            else:
                self.log.warning("%d - Ignoring %s" % (i,f))
        self.log.info ("Processed %s files" % len(allfiles))

    def loadDistFiles(self):
        """ Add the list of tar files actually on disk """
        # Listing all files on disk
        self.log.info("Loading archives from directory: %s" % self.distdir)
        fileStatuses = {}
        fileSizes = {}
        exludeddirs = ['eclipse', 'Dirac_project', 'LHCbDirac_project', 'html', 'TOOLS']
        for (root, dirs, files) in os.walk(self.distdir):
            dirs[:] = [d for d in dirs if d not in exludeddirs]
            prefix = root.replace(self.distdir + os.sep, "")
            print "In dir %s" % root
            for f in files:
                if f.endswith(".tar.gz"):
                    archivename = os.path.join(prefix, f)
                    fp = os.path.join(self.distdir, archivename)
                    if not os.path.islink(fp):
                        fileStatuses[archivename] = "unused"
                        fileSizes[archivename] = os.path.getsize(fp)
        self.log.info("Archives found: %d" % len(fileStatuses.keys()))
        self.archiveList = fileStatuses
        self.archiveSizes = fileSizes

    def saveToDisk(self, filename):
        """ Pickle the dependency DB to disk """
        db = open(filename, "wb")
        p.dump(self.distdir, db)
        p.dump(self.projectFiles, db)
        p.dump(self.archiveList, db)
        p.dump(self.archiveSizes, db)
        p.dump(datetime.datetime.now(), db)
        db.close()

    def loadFromDisk(self, filename):
        """ Reload the pickled dependency list fro disk """
        db = open( filename, "rb" )
        self.distdir = p.load(db)
        self.htmldir = os.path.join(self.distdir, "html")
        self.projectFiles  = p.load(db)
        self.archiveList  = p.load(db)
        self.archiveSizes  = p.load(db)
        self.dbDate = p.load(db)
        self.log.info("Database saved on: " + str(self.dbDate))

    #
    # Methods to access/list project files
    ###########################################################################
    def findProjectFiles(self, projectNameRe, versionRe, cmtconfigRe):
        """ Finds projects matching the name regexp """
        return [pf for pf in self.projectFiles if re.match(projectNameRe, pf.project) != None\
                and re.match(versionRe, pf.version) != None \
                and re.match(cmtconfigRe, pf.config) != None ]
    #
    # Various reports
    ###########################################################################

    def missingTarReport(self, filename):
        """ Creates a report with all the missing tars """
        f = open(filename, "w")
        f.write("Referencing HTML\tMissing TAR\n")
        for pf in self.projectFiles:
            if not os.path.exists(self.distdir + os.sep + pf.filename):
                f.write(pf.htmlfilename.replace("/afs/cern.ch/lhcb/distribution/", ""))
                f.write("\t")
                f.write( pf.filename)
                f.write("\n")
        f.close()

    def missingDependenciesReport(self, filename):
        """ Check if there are dependencies missing """
        f = open(filename, "w")
        f.write("Referencing HTML\tMissing Dependency\n")
        for pf in self.projectFiles:
            for dep in pf.getAllDependencies():
                if not os.path.exists(self.distdir + os.sep + dep.filename):
                    f.write(pf.htmlfilename.replace("/afs/cern.ch/lhcb/distribution/", ""))
                    f.write("\t")
                    f.write( dep.filename)
                    f.write("\n")
        f.close()
