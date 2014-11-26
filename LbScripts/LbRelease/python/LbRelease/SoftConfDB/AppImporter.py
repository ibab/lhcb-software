#!/usr/bin/env python
"""
A script to add a project to the Software Configuration DB

"""
import logging
import os
import re

from LbConfiguration.Repository import getRepositories
from LbRelease import rcs
from LbRelease.SvnTools.Project import translateProject, getProjectCmt
from LbUtils.Processes import callCommand
from LbRelease.SoftConfDB.SoftConfDB import SoftConfDB


# SVN Repositories definition
url = str(getRepositories(protocol='anonymous')["lbsvn"])
lbsvn = rcs.connect(url)

gaudiurl = str(getRepositories(protocol='anonymous')["gaudi"])
gaudisvn = rcs.connect(gaudiurl)

diracurl = str(getRepositories(protocol='anonymous')["dirac"])
diracsvn = rcs.connect(diracurl)

def importerTranslateProject(p, v):
    ''' Function needed to prevent LCGCMT to be passed to translateProject
    as that does not work '''
    if p.lower() == "lcgcmt":
        return (p.upper(), v)
    else:
        return translateProject(p, v)

class AppImporter:
    """ Main scripts class for looking up dependencies.
    It inherits from """

    def __init__(self, autorelease = True):
        # Creating the SoftConfDB Object
        self.mConfDB = SoftConfDB()
        self.log = logging.getLogger()
        self.installArea = None
        self.mAutorelease = autorelease

    def processProjectVersion(self, p, v, alreadyDone = [], recreate=False):
        """ Get the dependencies for a single project """
        # Cleanup the project name and version and get the SVN URL
        (proj,ver)=importerTranslateProject(p,v)
        tagpath = ""

        # Getting the project.cmt file with dependencies
        if proj.upper() == "GANGA":
            projcmt = self.getGangaProjectCMT(ver)
        else:
            if proj.upper() == "GAUDI":
                tagpath = gaudisvn.url(proj,ver, isProject=True)
            elif proj.upper() == "LHCBDIRAC" or proj.upper() == "DIRAC":
                tagpath = diracsvn.url(proj,ver, isProject=True)
            else:
                tagpath=lbsvn.url(proj,ver, isProject=True)
            self.log.debug("SVN PATH:" + tagpath)
            # Get the project.cmt file and parse it to retrieve the dependencies
            #if not tagpath.endswith("cmt"):
            #    tagpath += "/cmt"
            projcmt=getProjectCmt(tagpath).strip()
            self.log.debug("Project tag SVN path:" + tagpath)
        deps = []

        # Formatting the project name/version
        corver = ver
        if proj in ver:
            corver = ver.replace(proj + "_", "")
        proj = proj.upper()

        # Looking for the project version in the DB
        tmp = self.mConfDB.findVersion(proj, ver)
        rev = getPathLastRev(tagpath)
        createNode = False
        node_parent = None

        # First checking if the node is there with the correct revision
        if len(tmp) != 0:
            node = tmp[0][0]
            node_parent = node
            noderev = node["Rev"]
            if noderev != None:
                self.log.warning("Project %s %s already exists with revision %s (path rev: %s)" % (proj,
                                                                                                   ver,
                                                                                                   noderev,
                                                                                                   rev))
                if rev != noderev and recreate:
                    self.log.warning("Mismatch in revisions for %s %s, recreating" % (proj, ver))
                    self.log.warning("WARNING was a retag of  %s %s done? you may need to call "
                                     " lb-sdb-deletepv <proj> <ver> and reimport if "
                                     " it is the case " % (proj, ver))
                    
                    try:
                        self.mConfDB.deletePV(proj, corver)
                    except Exception, e:
                        self.log.error(e)
                    createNode = True
                                 
                                     
            else:
                self.log.warning("Project %s %s already exists without revision" % (proj,ver))

        #If the node does not exist just create it...
        else:
            createNode = True 

        if createNode:
            self.log.warning("Creating project %s %s revision in SVN: %s" % (proj, ver, rev))
            node_parent = self.mConfDB.getOrCreatePV(proj, corver)
            node_parent["Rev"] = rev
            # If releasing is needed!
            if self.mAutorelease:
                self.log.warning("Requesting release of %s %s" % (proj, corver))
                self.mConfDB.setReleaseFlag(proj, corver)
           
                
        self.log.warning("Now checking dependencies for %s %s" % (p, v))
        for l in projcmt.splitlines():
            m = re.match("\s*use\s+(\w+)\s+([\w\*]+)", l)
            if m != None:
                dp = m.group(1)
                dv = m.group(2)
                deps.append((dp, dv))

        if len(deps) == 0:
            self.log.warning("No dependencies found for %s %s" % (p, v))
            return node_parent

        # Now creating the dependencies
        for (dp, dv) in deps:
            if dp in dv:
                dv = dv.replace(dp + "_", "")
            dp = dp.upper()
            self.log.warning("Find project %s %s" % (dp, dv))
            node_child = self.processProjectVersion(dp, dv, alreadyDone, recreate)

            # Now checking if the links exist                
            if self.mConfDB.nodesHaveRelationship(node_parent, node_child, "REQUIRES"):
                self.log.warning("Pre-existing dependency (%s, %s)-[:REQUIRES]->(%s, %s)" % (proj, ver, dp, dv))
            else:
                self.log.warning("Adding dependency (%s, %s)-[:REQUIRES]->(%s, %s)" % (proj, ver, dp, dv))
                self.mConfDB.addRequires(node_parent, node_child)
                
        return node_parent

    def getGangaProjectCMT(self, gangaVersion):
        """ Reads the Project.cmt file for Ganga """
        if self.installArea == None:
            raise Exception("Can only get Ganga dependencies from an install area! - Set it on the AppImporter first...")

        pname = "GANGA"
        f = open(os.path.join(self.installArea, pname, pname + "_" + gangaVersion, "cmt", "project.cmt"), "r")
        txt = f.read()
        f.close()
        return txt


def getProjectLastRev(project, version):
    ''' Get the latest revision of a project in SVN
    Allows to check for retagging '''
    (proj,ver)=importerTranslateProject(project, version)
    tagpath = ""

    # Getting the project.cmt file with dependencies
    if proj.upper() == "GANGA":
        projcmt = self.getGangaProjectCMT(ver)
    else:
        if proj.upper() == "GAUDI":
            tagpath = gaudisvn.url(proj,ver, isProject=True)
        elif proj.upper() == "LHCBDIRAC" or proj.upper() == "DIRAC":
            tagpath = diracsvn.url(proj,ver, isProject=True)
        else:
            tagpath=lbsvn.url(proj,ver, isProject=True)
    return getPathLastRev(tagpath)
    

def getPathLastRev(path):
    ''' Check the SVN revision of the given SVN path '''
    rev = None
    infostr = callCommand('svn','info',path)[0]
    for l in infostr.splitlines():
        m = re.match("Last Changed Rev: (\d+)", l)
        if m != None:
            rev = m.group(1)
            break
    return rev


