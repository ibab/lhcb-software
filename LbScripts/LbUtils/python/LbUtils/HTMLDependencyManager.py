'''
Util classes that parse/import the files in ${LHCBTAR}/html
in order to check the dependencies between all the released
versions of LHCb software.

To use:

    htmldir = os.path.join(os.environ["LHCBTAR"], "html")
    p = ProjectFile.loadProject(htmldir, "Gaudi", "v22r5")

or:
    file = os.path.join(htmldir,"GAUDI_GAUDI_v22r5_x86_64-slc5-gcc43-opt.html")
    p = ProjectFile.loadFromFile(file)
    print p

Created on Mar 5, 2012
@author: Ben Couturier
'''
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

    # First checking the case where there is a CMTCONFIG
    m = re.match(LHCb_PNAME_WITHCONFIG, name)
    if m != None:
        if m.group(1) != m.group(2) and m.group(1) != "TOOLS":
            print "<%s> <%s>" % (m.group(1), m.group(2))
            raise Exception("Format error: Project name normally repeated in version in %s" % name)
        project = m.group(1)
        version = m.group(3)
        config = m.group(4)
    else:
        # Now checking if it's a file without CMTCONFIG
        m = re.match(LHCb_PNAME_NOCONFIG, name)
        if m != None:
            if m.group(1) != m.group(2) and m.group(1) != "TOOLS":
                print "<%s> <%s>" % (m.group(1), m.group(2))
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
        return "%s (%s, %s, %s) %s" %(self.type, self.project, self.version, self.config, self.fileName)

class LHCbDependency(Dependency):
    """ Class representing dependency to a LHCb project """
    def __init__(self, archive, fileName):
        self.archive = archive
        self.fileName = fileName
        self.type = "LHCb"
        (self.project, self.version, self.config) = parseLHCbArchiveName(archive)

class ExternalDependency(Dependency):
    """ Class representing dependency to external software  """
    def __init__(self, archive, fileName):
        self.archive = archive
        self.fileName = fileName
        self.type = "EXT"
        (self.project, self.version, self.config) = parseExternalArchiveName(archive)

class DataPackageDependency(Dependency):
    """ Class representing dependency to external software  """
    def __init__(self, archive, fileName):
        self.archive = archive
        self.fileName = fileName
        self.type = "DAT"
        (self.project, self.version, self.config) = parseDataPackageArchiveName(archive)

class SystemDependency(Dependency):
    """ Class representing dependency to external software  """
    def __init__(self, archive, fileName):
        self.archive = archive
        self.fileName = fileName
        self.type = "SYS"
        (self.project, self.version, self.config) = (archive, archive, archive)

#
# Class representing a specific HTML file
###############################################################################
class ProjectFile(object):
    """ Class representing a LHCb project file for a given config,
    the associated file and dependencies as represented in the html dependency files """

    @classmethod
    def loadFromFile(cls, fileName):
        """ Factory method that creates the ProjectFile object
        from a specific file """
        # Loads the file content
        obj = cls()
        obj.htmlfileName = fileName
        f = open(fileName, "r")
        fileContent = map (lambda s: s.strip(), f.readlines())
        f.close()
        fileContent = [line for line in fileContent if line != None and line != ""]
        #for l in fileContent:
        #    print l
        obj.parseDepFile(fileContent)
        return obj

    @classmethod
    def loadProject(cls, htmldir, projectName, version, config=None):
        """ Loads the ProjectFile object from the triplet
        project, version, config """
        p = Project.getProject(projectName, svn_fallback=True, raise_exception=False)
        tar_file = p.tarBallName(version, config)
        tar_file_html = tar_file + ".html"
        full_tar_file_html = htmldir + os.sep + tar_file_html
        if not os.path.exists(full_tar_file_html):
            raise Exception(full_tar_file_html + " is missing")
        return cls.loadFromFile(full_tar_file_html)

#    def __init__(self, fileName):
#        """ Constructor that loads the dependencies from a html file
#        in install_project format """

        # Loads the file content
#        self.htmlfileName = fileName
#        f = open(fileName, "r")
#        fileContent = map (lambda s: s.strip(), f.readlines())
#        f.close()

#        fileContent = [line for line in fileContent if line != None and line != ""]
#        self.parseDepFile(fileContent)


    def parseDepFile(self, fileContent):
        """ Parse the content of the dependency file """
        # First checking project definition
        if not re.match("^\s*<H3>", fileContent[0]):
            raise Exception("Format error: Dep file should start with H3: %s" % fileContent[0])

        # parsing secong which contains the project name and fileName
        m = re.match("<A NAME=([\w/-]+) HREF=(.*)>([\w-]+)</A>", fileContent[1])
        if m == None:
            raise Exception("Format error: Second line should match <A NAME=([\w/-]+) HREF=(.*)>([\w-]+)</A>")
        #if m.group(1) != m.group(3):
        #    raise Exception("Format error: %s should match %s" % (m.group(1), m.group(3)))
        self.archiveName = m.group(3)
        self.fileName = m.group(2)

        if self.archiveName.startswith("DBASE") or self.archiveName.startswith("PARAM"):
            (self.project, self.version, self.config) = parseDataPackageArchiveName(self.archiveName)
        elif "LCGCMT" in self.archiveName or "LCGGanga" in self.archiveName or "LCGGrid" in self.archiveName:
            (self.project, self.version, self.config) = parseExternalArchiveName(self.archiveName)
        else:
            (self.project, self.version, self.config) = parseLHCbArchiveName(self.archiveName)

        #print "Processing Project file%s/%s/%s %s" % (project, version, config, self.fileName)

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
        s = "ProjectFile (%s,%s,%s) %s\n" % (self.project, self.version, self.config, self.fileName)
        for c in self.lhcbDeps, self.externalDeps, self.dbaseDeps, self.paramDeps, self.systemDeps:
            for d in c:
                s+= "    " + str(d) + "\n"
        return s

    def getAllDependenciesFileNames(self):
        """Returns the fileNames of ALL dependencies for this project file"""
        depsf = []
        for c in self.lhcbDeps, self.externalDeps, self.dbaseDeps, self.paramDeps, self.systemDeps:
            for d in c:
                depsf.append(d.fileName)
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
    def __init__(self, tardir):
        """ Default constructor """
        self.projectFiles = []
        self.tardir = tardir

    def parseHTMLDir(self, htmlDir):
        """ Method that parses ALL the HTML files in a directory
        to extract the dependencies """
        allfiles = [f for f in os.listdir(htmlDir) if f.endswith("html")]
        for i, f in enumerate(allfiles):
            print "%d ------------- Processing %s" % (i,f)
            pf = ProjectFile(htmlDir + os.sep + f)
            self.projectFiles.append(pf)
            print pf

    def saveToDisk(self, fileName):
        """ Pickle the dependency DB to disk """
        p.dump(self.projectFiles, open(fileName, "wb"))

    def loadFromDisk(self, fileName):
        """ Reload the pickled dependency list fro disk """
        self.projectFiles  = p.load(open( fileName, "rb" ))

    def missingReport(self, fileName):
        f = open(fileName, "w")
        f.write("Referencing HTML\tMissing TAR\n")
        for pf in self.projectFiles:
            if not os.path.exists(self.tardir + os.sep + pf.fileName):
                f.write(pf.htmlfileName.replace("/afs/cern.ch/lhcb/distribution/", ""))
                f.write("\t")
                f.write( pf.fileName)
                f.write("\n")
        f.close()

    def missingDependenciesReport(self, fileName):
        f = open(fileName, "w")
        f.write("Referencing HTML\tMissing Dependency\n")
        for pf in self.projectFiles:
            for dep in pf.getAllDependencies():
                if not os.path.exists(self.tardir + os.sep + dep.fileName):
                    f.write(pf.htmlfileName.replace("/afs/cern.ch/lhcb/distribution/", ""))
                    f.write("\t")
                    f.write( dep.fileName)
                    f.write("\n")
        f.close()
