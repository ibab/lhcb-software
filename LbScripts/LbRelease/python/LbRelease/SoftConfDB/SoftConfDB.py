'''

Interface to LHCb Software Configuration DB in Neo4j.

This interface connects directly to the Neo4J DB via HTTP.

Created on May 2, 2013
@author: Ben Couturier
'''

import logging
from ariadne.py2neo import neo4j, cypher
from LbConfiguration.Version import sortVersions, LCGVersion

class SoftConfDB(object):
    '''
    Main class interfacing to the LHCb Configuration Database
    Test DB: http://lxbuild159:8080/db/data/
    Prod DB: https://ariadne-lhcb.cern.ch/
    '''
    def __init__(self, dbConnectStr="https://ariadne-lhcb.cern.ch/db/data"):
        '''
        Initialize the class, setting the address of the Database
        '''
        self.log = logging.getLogger()

        # Choosing the db string
        import os
        envdburl = os.environ.get('SDBURL')
        if envdburl != None:
            if envdburl == "DEV":
                dbConnectStr = "http://lxbuild159:8080/db/data/"
                self.log.info("SDBURL set to DEV: Using DB URL %s" % dbConnectStr)
            else:
                dbConnectStr = envdburl
                self.log.info("SDBURL set to %s" % dbConnectStr)

        # Initialize with the server address
        self.log.debug("Connecting to Neo4j DB: %s" % dbConnectStr)
        self.mDBURL = dbConnectStr

        # Initializing the DB itself
        self.mNeoDB = neo4j.GraphDatabaseService(dbConnectStr)

        # Only lookup the main nodes/indices if needed
        self.setupDone = False

    def getWriteBatch(self):
        """ Returns a write batch for this DB """
        return neo4j.WriteBatch(self.mNeoDB)


    def runCypher(self, query, handler):
        ''' Run a cypher query on the Neo4j DB'''
        cypher.execute(self.mNeoDB, query,  None, row_handler=handler)

    # Methods to query the database
    ###########################################################################
    def listProjects(self):
        ''' List the projects known by the SoftConfDB '''

        query = 'start n=node:Lbadmin(Type="PROJECT_PARENT") match n-[:TYPE]-m  return distinct m.project'
        projects = []
        self.runCypher(query, lambda x: projects.append(x[0]))
        return projects

    def listDatapkgs(self):
        ''' List the datapackages known by the SoftConfDB '''

        query = 'start n=node:Lbadmin(Type="DATAPKG_PARENT") match n-[:TYPE]-m  return distinct m.datapkg'
        datapkgs = []
        self.runCypher(query, lambda x: datapkgs.append(x[0]))
        return datapkgs

    def listApplications(self):
        ''' List the applications known by the SoftConfDB '''

        query = 'start n=node:Lbadmin(Type="APPLICATION") match n-[:APPLICATION]-m  return distinct m.project'
        projects = []
        self.runCypher(query, lambda x: projects.append(x[0]))
        return projects

    def listReleaseReqs(self):
        ''' List the applications to be released '''

        query = 'start n=node:Lbadmin(Type="RELEASE") match n-[:RELEASEREQ]-m  return distinct m.project, m.version'
        projects = []
        self.runCypher(query, lambda x: projects.append((x[0], x[1])))
        return projects

    def listCMakeBuiltProjects(self):
        ''' List the applications to be released '''
        
        query = 'start n=node:Lbadmin(Type="CMAKE") match n-[:BUILDTOOL]-m  return distinct m.project, m.version'
        projects = []
        self.runCypher(query, lambda x: projects.append((x[0], x[1])))
        return projects

    def listTag(self, tag):
        ''' List the applications to be released '''
        
        query = 'start n=node:Lbadmin(Type="%s") match n-[:TAG]-m  return distinct m.project, m.version' % tag
        projects = []
        self.runCypher(query, lambda x: projects.append((x[0], x[1], None)))

        query = 'start n=node:Lbadmin(Type="%s") match n-[:PTAG]-m  return distinct m.project, m.version, m.platform' % tag
        self.runCypher(query, lambda x: projects.append((x[0], x[1], x[2])))
        return projects


    def listCMTBuiltProjects(self):
        ''' List the applications to be released '''
        
        query = 'start n=node:Lbadmin(Type="CMT") match n-[:BUILDTOOL]-m  return distinct m.project, m.version'
        projects = []
        self.runCypher(query, lambda x: projects.append((x[0], x[1])))
        return projects

    def listReleaseStackFromPV(self, project, version):
        query = 'start n=node:Lbadmin(Type="RELEASE"), m=node:ProjectVersion(ProjectVersion="%s_%s")  '  % (project, version)
        query += ' match p = n-[:RELEASEREQ]->m-[:REQUIRES*]->o<-[:RELEASEREQ]-n '
        query += ' return extract(x in nodes(p) | x.project + "_" + x.version)'

        paths = []
        self.runCypher(query, lambda x: paths.append(x))
        stack = set()

        # If no deps are found the stack consists of the project only
        if len(paths) == 0:
            stack.add((project, version))
            return stack

        self.log.debug("Iterating on nodes on the project %s %s" % (project, version))
        # Now iterate through all paths returned and add the nodes to the set
        # (the set takes care of deduplicating)
        rpaths = []
        for p in paths:
            for pp in p:
                for n in pp:
                    if n != "NONE_NONE":
                        stack.add(tuple(n.split("_")))
        return stack
                        
    def listReleaseStackTops(self):
        ''' List the applications to be released '''
        query = 'start n=node:Lbadmin(Type="RELEASE") match p = n-[:RELEASEREQ]->m-[r?:REQUIRES*]->o<-[:RELEASEREQ]->n '
        query += ' where not n-[:RELEASEREQ]->()-[:REQUIRES]->m '
        query += ' return distinct m.project, m.version'

        projects = []
        self.runCypher(query, lambda x: projects.append((x[0], x[1])))
        return projects

    def listReleaseStacks(self):
        ''' List the applications to be released '''

        self.log.debug("Starting listReleaseStacks");
        # First, find the top of the various stacks
        stackTops = self.listReleaseStackTops()
        stacks = []
        for (stp, stv) in stackTops:
            self.log.debug("Listing projects in stack for %s %s" % (stp, stv))
            stack = self.listReleaseStackFromPV(stp, stv)
            stacks.append(stack)
            self.log.debug("Listed projects in stack for %s %s" % (stp, stv))

        # Now filter all the stacks
        filteredstacks = []
        import itertools
        for s in stacks:
            ismerged = False
            for i, fs in enumerate(filteredstacks):
                intersec = s & fs
                if len(intersec) > 0:
                    union = s | fs
                    # Make sure we don't have the same project with two different versions
                    # This is not suported by the nightlies
                    import collections
                    duplicateProjs = [x for x, y in collections.Counter([p for (p,v) in union]).items() if y > 1]
                    # If there are no duplicate projects, we can use the union of the stacks
                    if len(duplicateProjs) == 0:
                        filteredstacks[i] = union
                        ismerged = True
                    break
            if not ismerged:
                filteredstacks.append(s)

        return filteredstacks

    def listVersions(self, project):
        ''' List the number of versions known for a given project '''

        query = 'start n=node:Project(Project="%s") match n-[:PROJECT]-m  return distinct m.project, m.version order by m.version' \
        % project
        pvs = []
        self.runCypher(query, lambda x: pvs.append((x[0], x[1])))
        return pvs

    def listDatapkgVersions(self, datapkg):
        ''' List the number of versions known for a given datapkg '''

        query = 'start n=node:Datapkg(Datapkg="%s") match n-[:DATAPKG]-m  return distinct m.datapkg, m.version order by m.version' \
        % datapkg
        pvs = []
        self.runCypher(query, lambda x: pvs.append((x[0], x[1])))
        return pvs


    def findVersion(self, project, version):
        ''' Find whether a specific project version exists in the DB '''

        query = 'start n=node:ProjectVersion(ProjectVersion="%s_%s") return n' \
        % (project, version)
        pvs = []
        self.runCypher(query, lambda x: pvs.append(x))
        return pvs

    def listStackPlatforms(self, project, version):
        #query = 'start n=node:ProjectVersion(ProjectVersion="%s_%s") match p=n-[:REQUIRES*]->m-[:PLATFORM]->q  return distinct q.platform, m.project, m.version, length(p)' \
        #% (project, version)
        query = 'start n=node:ProjectVersion(ProjectVersion="%s_%s") match p=n-[:REQUIRES*]->m-[:PLATFORM]->q  return distinct q.platform, m.project, m.version' \
        % (project, version)
        plist = []
        self.runCypher(query, lambda x: plist.append((x[0], x[1], x[2])))
        return plist

    def listStackPlatformsToRelease(self, project, version):
        # Onlist list the subset that should be released
        query = 'start n=node:ProjectVersion(ProjectVersion="%s_%s") match p=n-[:REQUIRES*]->m-[:PLATFORM]->q  return distinct q.platform, m.project, m.version, length(p)'  \
        % (project, version)
        plist = []
        self.runCypher(query, lambda x: plist.append((x[0], x[1], x[2], x[3])))

        # Now find the shortest path
        # First check we have SOME info
        if len(plist) == 0:
            raise Exception("Cannot find platform information! Cannot list platforms to release")
        minpathlength = min([x[3] for x in plist])

        # Now grouping the platforms by project/version
        platinfo = {}
        for (plat, p, v, l) in plist:
            idx = "%s_%s" % (p, v)
            curplat = platinfo.get(idx)
            if curplat == None:
                curplat = set()
            curplat.add(plat)
            platinfo[idx] = curplat

        # Now taking the intersection
        curplat = None
        for plats in platinfo.values():
            if curplat == None:
                curplat = plats
            else:
                curplat = curplat & plats

        # And return the result
        return curplat


    def listPlatforms(self, project, version):
        ''' List the Platforms released for a Couple project version '''

        # Query that looks specifically for the Project Version node, and returns the associated
        # platforms
        query = 'start n=node:ProjectVersion(ProjectVersion="%s_%s") match p=n-[:PLATFORM]->q  return distinct q.platform' \
        % (project, version)

        platforms = []
        self.runCypher(query, lambda x: platforms.append(x[0]))
        return platforms

    def listDependencies(self, project, version):
        ''' List the project/versions the specified project depends on '''

        query = 'start n=node:ProjectVersion(ProjectVersion="%s_%s") match p=n-[:REQUIRES*]->m  return distinct m.project, m.version' \
        % (project, version)
        pvs = []
        self.runCypher(query, lambda x: pvs.append((x[0], x[1])))
        return pvs

    def listReferences(self, project, version):
        ''' List the project/versions that depend on this project '''

        query = 'start n=node:ProjectVersion(ProjectVersion="%s_%s") match m-[:REQUIRES*]->n  return distinct m.project, m.version' \
        % (project, version)
        pvs = []
        self.runCypher(query, lambda x: pvs.append((x[0], x[1])))
        return pvs

    def listActiveReferences(self, project, version):
        ''' List the project/versions that depend on this project, that are active on disk '''

        query = 'start n=node:ProjectVersion(ProjectVersion="%s_%s") match o-[:ACTIVE]-m-[:REQUIRES*]->n  return distinct m.project, m.version' \
        % (project, version)
        pvs = []
        self.runCypher(query, lambda x: pvs.append((x[0], x[1])))
        return pvs

    def show(self, project, version):
        ''' Show teh various attributes of a project/version '''
        node_pv =  self.mNeoDB.get_or_create_indexed_node("ProjectVersion",
                                                          "ProjectVersion",
                                                          project + "_" + version,
                                                          {"project": project, "version":version})
        
        print "Node %s Properties" % node_pv._id
        print "------------------------------"
        for (p,v) in node_pv.get_properties().items():
            print "%-10s: %s" % (p, v)

        print "\nNode %s relationships" % node_pv._id
        print "------------------------------"
        outrels = node_pv.get_relationships(neo4j.Direction.OUTGOING)
        for r in outrels:
            relprops = r.end_node.get_properties()
            relprops['ID'] = r.end_node._id
            props = ", ".join("{}:{}".format(k, v) for k, v in sorted(relprops.items()))
            tmp = "%s:%-15s -> (%s)" % (r._id, r.type + "(O)", props)
            print tmp

        inrels = node_pv.get_relationships(neo4j.Direction.INCOMING)
        for r in inrels:
            relprops = r.start_node.get_properties()
            relprops['ID'] = r.start_node._id
            props = ", ".join("{}:{}".format(k, v) for k, v in sorted(relprops.items()))
            tmp = "%s:%-15s <- (%s)" % (r._id, r.type + "(I)", props)
            print tmp

    def nodesHaveRelationship(self, parent, child, type):
        """ Check if the nodes have a directed parent to child node of given type"""
        outrels = parent.get_relationships(neo4j.Direction.OUTGOING)
        for r in outrels:
            if r.end_node._id == child._id and r.type == type:
                return True
        return False

    def listActive(self):
        ''' List the active project/versions '''
        query = 'start n=node:Lbadmin(Type="STATUS") match n-[:ACTIVE]->m return distinct m.project, m.version'
        pvs = []
        self.runCypher(query, lambda x: pvs.append((x[0], x[1])))
        return pvs

    def listActiveApplications(self):
        ''' List the active project/versions '''
        query = '''start n=node:Lbadmin(Type="STATUS"), p=node:Lbadmin(Type="APPLICATION")
        match n-[:ACTIVE]->m--q--p return distinct m.project, m.version '''
        pvs = []
        self.runCypher(query, lambda x: pvs.append((x[0], x[1])))
        return pvs

    def listUsed(self):
        ''' List the used project/versions '''
        query = 'start n=node:Lbadmin(Type="USED") match n-[:USED]->m return distinct m.project, m.version'
        pvs = []
        self.runCypher(query, lambda x: pvs.append((x[0], x[1])))
        return pvs

    def checkUnused(self, verbose):
        ''' List the used project/versions '''
        queryactive = 'start n=node:Lbadmin(Type="STATUS") match n-[:ACTIVE]->m return distinct m'
        queryappused = '''start n=node:Lbadmin(Type="USED"), q=node:Lbadmin(Type="STATUS")
                        match n-[:USED]-t-[:ACTIVE]-q
                        return distinct t'''
        queryused = '''start n=node:Lbadmin(Type="USED"), q=node:Lbadmin(Type="STATUS")
                        match n-[:USED]-t-[:ACTIVE]-q
                        with t
                        match t-[:REQUIRES*]->m
                        return distinct m'''
        active = []
        appused = []
        used = []
        self.runCypher(queryused, lambda x: used.append(x[0]))
        self.runCypher(queryactive, lambda x: active.append(x[0]))
        self.runCypher(queryappused, lambda x: appused.append(x[0]))

        activec = []
        for n in active:
            props = n.get_properties()
            activec.append((props["project"], props["version"]))
            if verbose:
                print "Active: %s %s" % (props["project"], props["version"])

        usedc = []
        for n in used:
            props = n.get_properties()
            usedc.append((props["project"], props["version"]))
            if verbose:
                print "Used: %s %s" % (props["project"], props["version"])

        appusedc = []
        for n in appused:
            props = n.get_properties()
            appusedc.append((props["project"], props["version"]))
            if verbose:
                print "AppUsed: %s %s" % (props["project"], props["version"])


        if verbose:
            print "Unused = Active - Used - AppUsed"
        return [ n for n in activec if n not in usedc and n not in appusedc ]

    def getBuildTools(self, project, version):
        """ Check which buildtool should used to build this project """

        # First checking whether we're directly connected to the node
        query = 'start n=node:ProjectVersion(ProjectVersion="%s_%s") match n-[:BUILDTOOL]-q return q.type' \
        % (project, version)
        plist = []
        self.runCypher(query, lambda x: plist.append((x[0])))
        if len(plist) > 0:
            return plist
    
        # Or if one of the ancestors
        query = 'start n=node:ProjectVersion(ProjectVersion="%s_%s") match p=n-[:REQUIRES*]->m-[:BUILDTOOL]-q  return q.type' \
        % (project, version)
        plist = []
        self.runCypher(query, lambda x: plist.append(( x[0] )))
        return plist


    # Methods to add/update nodes
    ###########################################################################
    def setupDB(self):
        ''' Create or lookup the main entry point nodes and indices '''
        if not self.setupDone:
            self.projectVersionIndex = self.mNeoDB.get_or_create_index(neo4j.Node, "ProjectVersion")
            self.projectVersionPlatformIndex = self.mNeoDB.get_or_create_index(neo4j.Node, "ProjectVersionPlatform")
            self.datapkgVersionIndex = self.mNeoDB.get_or_create_index(neo4j.Node, "DatapkgVersion")
            self.projectIndex = self.mNeoDB.get_or_create_index(neo4j.Node, "Project")
            self.projectIndex = self.mNeoDB.get_or_create_index(neo4j.Node, "datapkg")
            self.lbadminIndex = self.mNeoDB.get_or_create_index(neo4j.Node, "Lbadmin")
            self.node_projectparent =  self.mNeoDB.get_or_create_indexed_node("Lbadmin",
                                                               "Type",
                                                               "PROJECT_PARENT",
                                                               {"type": "PROJECT_PARENT"})
            self.node_pvpparent =  self.mNeoDB.get_or_create_indexed_node("Lbadmin",
                                                               "Type",
                                                               "PVP_PARENT",
                                                               {"type": "PVP_PARENT"})
            self.node_datapkgparent =  self.mNeoDB.get_or_create_indexed_node("Lbadmin",
                                                               "Type",
                                                               "DATAPKG_PARENT",
                                                               {"type": "DATAPKG_PARENT"})
            self.node_platformparent =  self.mNeoDB.get_or_create_indexed_node("Lbadmin",
                                                               "Type",
                                                               "PLATFORM",
                                                               {"type": "PLATFORM"})
            self.node_used =  self.mNeoDB.get_or_create_indexed_node("Lbadmin",
                                                               "Type",
                                                               "USED",
                                                               {"type": "USED"})
            self.node_status =  self.mNeoDB.get_or_create_indexed_node("Lbadmin",
                                                               "Type",
                                                               "STATUS",
                                                               {"type": "STATUS"})

            self.node_application =  self.mNeoDB.get_or_create_indexed_node("Lbadmin",
                                                               "Type",
                                                               "APPLICATION",
                                                               {"type": "APPLICATION"})

            self.node_release =  self.mNeoDB.get_or_create_indexed_node("Lbadmin",
                                                                        "Type",
                                                                        "RELEASE",
                                                                        {"type": "RELEASE",
                                                                         "project": "NONE",
                                                                         "version":"NONE"})

            self.node_cmake =  self.mNeoDB.get_or_create_indexed_node("Lbadmin",
                                                                      "Type",
                                                                      "CMAKE",
                                                                      {"type": "CMAKE"})
            self.node_cmt =  self.mNeoDB.get_or_create_indexed_node("Lbadmin",
                                                                    "Type",
                                                                    "CMT",
                                                                    {"type": "CMT"})

            self.setupDone = True


    def getNodeTag(self, tag):
        ''' Creates or gets the node named TAG (upper case) and index it for retrieval '''
        cleanTag = tag.upper()
        node =  self.mNeoDB.get_or_create_indexed_node("Lbadmin",
                                                       "Type",
                                                       cleanTag,
                                                       {"type": cleanTag})
        return node

    def getPlatformParent(self):
        self.setupDB()
        return self.node_platformparent

    def getProjectParent(self):
        self.setupDB()
        return self.node_projectparent

    def getPVPParent(self):
        self.setupDB()
        return self.node_pvpparent

    def getDatapkgParent(self):
        self.setupDB()
        return self.node_datapkgparent

    def addPVPlatform(self, project, version, platform):
        """ Get the dependencies for a single project """

        # First create the indices needed
        node_platform = self.mNeoDB.get_or_create_indexed_node("Platform",
                                                           "Platform",
                                                           platform,
                                                           {"platform": platform})
        node_pv =  self.mNeoDB.get_or_create_indexed_node("ProjectVersion",
                                                           "ProjectVersion",
                                                           project + "_" + version,
                                                           {"project": project, "version": version})

        self.mNeoDB.get_or_create_relationships((node_pv, "PLATFORM", node_platform))
        self.mNeoDB.get_or_create_relationships((self.getPlatformParent(), "TYPE", node_platform))

    def delPVPlatform(self, project, version, platform):
        """ Get the dependencies for a single project """

        # First create the indices needed
        node_platform = self.mNeoDB.get_or_create_indexed_node("Platform",
                                                           "Platform",
                                                           platform,
                                                           {"platform": platform})
        node_pv =  self.mNeoDB.get_or_create_indexed_node("ProjectVersion",
                                                           "ProjectVersion",
                                                           project + "_" + version,
                                                           {"project": project, "version": version})
        rels = node_pv.get_relationships()
        for r in rels:
            if r.type == 'PLATFORM':
                props = r.end_node.get_properties()
                if props["platform"] == platform:
                    self.log.warning("Removing relationship: %s" % str(r))
                    r.delete()
                    
    def versionIsPattern(self, version):
        return '*' in version

    def getOrCreatePV(self, project, version):
        ''' Create a project version node, with appropriate links '''

        # Check whether the version is a pattern
        # If yes, then create the appropriate node
        if self.versionIsPattern(version):
            return self.getOrCreateVersionPattern(project, version)

        node_project = self.mNeoDB.get_or_create_indexed_node("Project",
                                                           "Project",
                                                           project,
                                                           {"project": project})
        node_pv =  self.mNeoDB.get_or_create_indexed_node("ProjectVersion",
                                                           "ProjectVersion",
                                                           project + "_" + version,
                                                           {"project": project, "version":version})
        self.mNeoDB.get_or_create_relationships((node_project, "PROJECT", node_pv))
        self.mNeoDB.get_or_create_relationships((self.getProjectParent(), "TYPE", node_project))

        self._updateVersionPatternLinks(node_project)

        return node_pv
    
    def deletePV(self, project, version):
        ''' Create a project version node, with appropriate links '''

        self.log.warning("Deleting: %s %s" % (project, version))
        node_pv =  self.mNeoDB.get_indexed_node("ProjectVersion",
                                                "ProjectVersion",
                                                project + "_" + version)
        if node_pv is None:
            self.log.warning("Node not found, exiting")
            return

        rels = node_pv.get_relationships()
        for r in rels:
            self.log.warning("Deleting rel: " + str(r))
            r.delete()
        self.log.warning("Deleting node: " + str(node_pv))
        node_pv.delete()

    def getOrCreateVersionPattern(self, project, version):
        ''' Create a project version pattern, with appropriate links '''
        node_project = self.mNeoDB.get_or_create_indexed_node("Project",
                                                           "Project",
                                                           project,
                                                           {"project": project})
        node_pv =  self.mNeoDB.get_or_create_indexed_node("ProjectVersion",
                                                           "ProjectVersion",
                                                           project + "_" + version,
                                                           {"project": project, "version":version})
        self.mNeoDB.get_or_create_relationships((node_project, "PATTERN", node_pv))
        self.mNeoDB.get_or_create_relationships((self.getProjectParent(), "TYPE", node_project))

        # Make sure the links point to the correct versions
        self._updateVersionPatternLinks(node_project)

        return node_pv

    def _updateVersionPatternLinks(self, node_project):
        """ Update all the links between the version patterns and the actual project versions"""

        # Keeping the instances of all the nodes
        versionNodes = {}
        patternNodes = {}

        # Iterating on all the used dependencies
        for r in node_project.get_relationships(neo4j.Direction.OUTGOING):
            if r.is_type("PROJECT"):
                versionNodes[r.end_node.get_properties()["version"]] = r.end_node
            elif r.is_type("PATTERN"):
                patternNodes[r.end_node.get_properties()["version"]] = r.end_node

        for p in patternNodes.keys():

            node_pattern = patternNodes[p]
            # Deleting the existing relationship first
            rels = node_pattern.get_relationships(neo4j.Direction.OUTGOING)
            for r in rels:
                if r.is_type("REQUIRES"):
                    r.delete()

            self.log.warning("Processing pattern: %s %s" % ( node_project.get_properties()["project"], p ))
            matching = self.matchAndSortVersions(p, versionNodes.keys())
            if len(matching) == 0:
                self.log.warning("Pattern %s %s No match" % ( node_project.get_properties()["project"], p))
            else:
                vmatch = matching[-1]
                self.log.warning("Pattern %s %s matching %s" % ( node_project.get_properties()["project"], p,  vmatch))
                node_version = versionNodes[vmatch]
                self.addRequires(node_pattern, node_version)

    def getOrCreatePVP(self, project, version, platform):
        ''' Create a project version platform node, with appropriate links '''

        node_pv =  self.getOrCreatePV(project, version)
        pvp_parent = self.getPVPParent()
        node_pvp =  self.mNeoDB.get_or_create_indexed_node("ProjectVersionPlatform",
                                                           "ProjectVersionPlatform",
                                                           project + "_" + version + "_" + platform,
                                                           {"project": project, "version":version, "platform":platform})
        node_platform = self.mNeoDB.get_or_create_indexed_node("Platform",
                                                               "Platform",
                                                               platform,
                                                               {"platform": platform})
        self.mNeoDB.get_or_create_relationships((node_pvp, "PVPPLATFORM", node_platform))
        self.mNeoDB.get_or_create_relationships((node_pvp, "PVP", node_pv))
        self.mNeoDB.get_or_create_relationships((pvp_parent, "TYPE", node_pvp))
        return node_pvp
    
    def deletePVP(self, project, version, platform):
        ''' Create a project version platform node, with appropriate links '''

        self.log.warning("Deleting: %s %s %s" % (project, version, platform))
        node_pvp =  self.mNeoDB.get_indexed_node("ProjectVersionPlatform",
                                                "ProjectVersionPlatform",
                                                project + "_" + version + "_" + platform)
        if node_pvp is None:
            self.log.warning("Node not found, exiting")
            return

        rels = node_pvp.get_relationships()
        for r in rels:
            self.log.warning("Deleting rel: " + str(r))
            r.delete()
        self.log.warning("Deleting node: " + str(node_pvp))
        node_pvp.delete()

    def deleteActiveLinks(self, batch = None):
        """ delete the active nodes in the graph """
        self.setupDB()
        # Iterating on all the proper version nodes to find all normal version
        # and pattern nodes
        for r in self.node_status.get_relationships(neo4j.Direction.OUTGOING):
            if r.is_type("ACTIVE"):
                if batch:
                    self.log.warning("deleting relationships in batch")
                    batch.delete_relationship(r)
                else:
                    r.delete()

    def deleteUsedLinks(self):
        """ delete the used nodes in the graph """
        self.setupDB()
        # Iterating on all the proper version nodes to find all normal version
        # and pattern nodes
        for r in self.node_used.get_relationships(neo4j.Direction.OUTGOING):
            if r.is_type("USED"):
                r.delete()


    def setAllAppVersionsUsed(self):
        ''' List the number of versions known for a given project '''

        query = 'start n=node:Lbadmin(Type="APPLICATION") match n-[:APPLICATION]-p-[:PROJECT]-m  return distinct m'
        pvs = []
        self.runCypher(query, lambda x: pvs.append((x[0])))
        for pv in pvs:
            if not pv.has_relationship_with(self.node_used):
                self.mNeoDB.get_or_create_relationships((self.node_used, "USED", pv))


    def setPVUsed(self, project, version):
        ''' Set the used link for a project version '''
        self.setupDB()

        node_pv =  self.mNeoDB.get_indexed_node("ProjectVersion",
                                                
                                                           "ProjectVersion",
                                                           project + "_" + version)
        if node_pv is None:
            raise Exception("%s %s not found" % (project, version))

        if not self.node_used.has_relationship_with(node_pv):
            self.mNeoDB.get_or_create_relationships((self.node_used, "USED", node_pv))

    def setApplication(self, project):
        ''' Set the link to indicate that a project is an application '''
        self.setupDB()

        node_project = self.mNeoDB.get_or_create_indexed_node("Project",
                                                           "Project",
                                                           project,
                                                           {"project": project})

        if not self.node_application.has_relationship_with(node_project):
            self.mNeoDB.get_or_create_relationships((self.node_application, "APPLICATION", node_project))

    def unsetApplication(self, project):
        ''' unset the used link for a project version '''
        self.setupDB()

        node_project = self.mNeoDB.get_or_create_indexed_node("Project",
                                                           "Project",
                                                           project,
                                                           {"project": project})

        if self.node_application.has_relationship_with(node_project):
            for r in node_project.get_relationships():
                if r.is_type("APPLICATION"):
                    r.delete()


    def setBuildTool(self, project, version, buildTool):
        ''' Set the link to indicate that a project was built with CMake '''
        self.setupDB()
        node_pv =  self.mNeoDB.get_indexed_node("ProjectVersion",
                                                "ProjectVersion",
                                                project + "_" + version)
        
        self.nodebuildtool =  self.node_cmake
        if buildTool != None and buildTool.upper() == "CMT":
            self.nodebuildtool = self.node_cmt

        if not self.nodebuildtool.has_relationship_with(node_pv):
            rels = self.mNeoDB.get_or_create_relationships((self.nodebuildtool, "BUILDTOOL", node_pv))
            for r in rels:
                props = r.get_properties()
                import datetime
                props["DATE"] = str(datetime.datetime.now())
                r.set_properties(props)
                
    def unsetBuildTool(self, project, version):
        ''' Unset the link to indicate that a project was built with CMake '''
        self.setupDB()
        node_pv =  self.mNeoDB.get_indexed_node("ProjectVersion",
                                                "ProjectVersion",
                                                project + "_" + version)

        if self.node_cmake.has_relationship_with(node_pv):
            for r in node_pv.get_relationships():
                if r.is_type("BUILDTOOL"):
                    r.delete()

        if self.node_cmt.has_relationship_with(node_pv):
            for r in node_pv.get_relationships():
                if r.is_type("BUILDTOOL"):
                    r.delete()


    def setTag(self, project, version, tag):
        ''' Set the link to a node named tag from the given project/version'''
        self.setupDB()
        node_pv =  self.mNeoDB.get_indexed_node("ProjectVersion",
                                                "ProjectVersion",
                                                project + "_" + version)

        node_tag = self.getNodeTag(tag)
        if not node_tag.has_relationship_with(node_pv):
            rels = self.mNeoDB.get_or_create_relationships((node_tag, "TAG", node_pv))
            for r in rels:
                props = r.get_properties()
                import datetime
                props["DATE"] = str(datetime.datetime.now())
                r.set_properties(props)
                
    def unsetTag(self, project, version, tag):
        ''' Unset the link to indicate that a project was built with CMake '''
        self.setupDB()
        node_pv =  self.mNeoDB.get_indexed_node("ProjectVersion",
                                                "ProjectVersion",
                                                project + "_" + version)

        node_tag = self.getNodeTag(tag)
        if node_tag.has_relationship_with(node_pv):
            for r in node_pv.get_relationships():
                if r.is_type("TAG") and (r.start_node == node_tag or r.end_node == node_tag):
                    r.delete()

    def setPlatformTag(self, project, version, platform, tag):
        ''' Set the link to a node named tag from the given project/version'''
        self.setupDB()
        node_pvp =  self.getOrCreatePVP(project, version, platform)
        node_tag = self.getNodeTag(tag)
        if not node_tag.has_relationship_with(node_pvp):
            rels = self.mNeoDB.get_or_create_relationships((node_tag, "PTAG", node_pvp))
            for r in rels:
                props = r.get_properties()
                import datetime
                props["DATE"] = str(datetime.datetime.now())
                r.set_properties(props)
                
    def unsetPlatformTag(self, project, version, platform, tag):
        ''' Unset the link to indicate that a project was built with CMake '''
        self.setupDB()
        node_pvp =  self.getOrCreatePVP(project, version, platform)
        node_tag = self.getNodeTag(tag)
        print "---> nodes:", node_tag, node_pvp
        if node_tag.has_relationship_with(node_pvp):
            print "---> Found relationship"
            for r in node_pvp.get_relationships():
                if r.is_type("PTAG") and (r.start_node == node_tag or r.end_node == node_tag):
                    r.delete()

    def setReleaseFlag(self, project, version):
        ''' Set the link to indicate that a release was requested '''
        self.setupDB()
        node_pv =  self.mNeoDB.get_indexed_node("ProjectVersion",
                                                "ProjectVersion",
                                                project + "_" + version)

        if not self.node_release.has_relationship_with(node_pv):
            rels = self.mNeoDB.get_or_create_relationships((self.node_release, "RELEASEREQ", node_pv))
            for r in rels:
                props = r.get_properties()
                import datetime
                props["REQDATE"] = str(datetime.datetime.now())
                r.set_properties(props)
                
    def unsetReleaseFlag(self, project, version):
        ''' unset the link indicated that a release was requested '''
        self.setupDB()
        node_pv =  self.mNeoDB.get_indexed_node("ProjectVersion",
                                                "ProjectVersion",
                                                project + "_" + version)

        if self.node_release.has_relationship_with(node_pv):
            for r in node_pv.get_relationships():
                if r.is_type("RELEASEREQ"):
                    r.delete()

    def setPVActive(self, project, version, batch=None):
        ''' Set the used link for a project version '''
        self.setupDB()

        node_pv =  self.mNeoDB.get_indexed_node("ProjectVersion", "ProjectVersion", project + "_" + version)

        if node_pv is None:
            raise Exception("%s %s not found" % (project, version))

        if not self.node_status.has_relationship_with(node_pv):
            if batch:
                batch.get_or_create_relationships((self.node_status, "ACTIVE", node_pv))
            else:
                self.mNeoDB.get_or_create_relationships((self.node_status, "ACTIVE", node_pv))


    def matchAndSortVersions(self, pattern, vers):
        """ Filters the version found to those matching the pattern
        and returns them sorted """

        # Patterns are:
        # v* v1r* v1r2p* etc etc
        # We keep everything before the * and only keep the versions that start by that...
        if not '*' in pattern:
            raise Exception("%s is not a valid pattern" % pattern)

        prefix = pattern.split("*")[0]
        if len(vers) > 0 and vers[0].startswith("v"):
            return sortVersions([vi for vi in vers if vi.startswith(prefix)])
        else:
            return sortVersions([vi for vi in vers if vi.startswith(prefix)], versiontype=LCGVersion )


    def addRequires(self, parentNode, childNode):
        ''' Add a dependency between two projects, which need to have been   '''
        self.mNeoDB.get_or_create_relationships((parentNode,"REQUIRES", childNode))


    def getOrCreateDatapkgVersion(self, project, hat, datapkg, version):
        ''' Return or create a datapkg node '''

        # Check whether the version is a pattern
        # XXX
        
        # Create the project node
        project = project.upper()
        datapkgini = datapkg
        datapkg = datapkg.upper()
        if hat == None:
            hat = ""
            
        node_project = self.mNeoDB.get_or_create_indexed_node("Project",
                                                           "Project",
                                                           project,
                                                           {"project": project})
        self.mNeoDB.get_or_create_relationships((self.getProjectParent(), "TYPE", node_project))

        # Create the datapkg node
        node_datapkg = self.mNeoDB.get_or_create_indexed_node("Datapkg",
                                                           "Datapkg",
                                                           datapkg,
                                                           {"datapkg": datapkg, "hat":hat, "realname":datapkgini})
        self.mNeoDB.get_or_create_relationships((self.getDatapkgParent(), "TYPE", node_datapkg))
        self.mNeoDB.get_or_create_relationships((node_datapkg, "PKGPROJECT", node_project))

        # 
        node_dv =  self.mNeoDB.get_or_create_indexed_node("DatapkgVersion",
                                                          "DatapkgVersion",
                                                          datapkg + "_" + version,
                                                          {"datapkg": datapkg, "version":version})
        self.mNeoDB.get_or_create_relationships((node_datapkg, "DATAPKG", node_dv))
        
        return node_dv


if __name__ == '__main__':
    sdb = SoftConfDB()
    sdb.setupDB()
    sdb.getOrCreateDatapkgVersion("DBASE", "", "PRConfig", "v1r8")
    sdb.getOrCreateDatapkgVersion("DBASE", "", "PRConfig", "v1r9")
    print sdb.listDatapkgs()
    print sdb.listDatapkgVersions("PRCONFIG")
    
    #sdb.checkUnused()


