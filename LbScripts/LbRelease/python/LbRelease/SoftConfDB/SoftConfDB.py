'''

Interface to LHCb Software Configuration DB in Neo4j.

This interface connects directly to the Neo4J DB via HTTP.

Created on May 2, 2013
@author: Ben Couturier
'''

import logging
from py2neo import neo4j, cypher
from LbConfiguration.Version import sortVersions, LCGVersion


class SoftConfDB(object):
    '''
    Main class interfacing to the LHCb Configuration Database
    '''
    def __init__(self, dbConnectStr="http://lxbuild159:8080/db/data/"):
        '''
        Initialize the class, setting the address of the Database
        '''
        self.log = logging.getLogger()

        # Initialize with the server address
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

    def listVersions(self, project):
        ''' List the number of versions known for a given project '''

        query = 'start n=node:Project(Project="%s") match n-[:PROJECT]-m  return distinct m.project, m.version order by m.version' \
        % project
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

    def listActive(self):
        ''' List the active project/versions '''
        query = 'start n=node:Lbadmin(Type="STATUS") match n-[:ACTIVE]->m return distinct m.project, m.version'
        pvs = []
        self.runCypher(query, lambda x: pvs.append((x[0], x[1])))
        return pvs

    def listUsed(self):
        ''' List the used project/versions '''
        query = 'start n=node:Lbadmin(Type="USED") match n-[:USED]->m return distinct m.project, m.version'
        pvs = []
        self.runCypher(query, lambda x: pvs.append((x[0], x[1])))
        return pvs

    def checkUnused(self):
        ''' List the used project/versions '''
        queryactive = 'start n=node:Lbadmin(Type="STATUS") match n-[:ACTIVE]->m return distinct m'
        queryused = 'start n=node:Lbadmin(Type="USED") match n-[:USED|REQUIRES*]->(p) return distinct p'
        active = []
        used = []
        self.runCypher(queryused, lambda x: used.append(x[0]))
        self.runCypher(queryactive, lambda x: active.append(x[0]))

        activec = []
        for n in active:
            props = n.get_properties()
            activec.append((props["project"], props["version"]))

        usedc = []
        for n in used:
            props = n.get_properties()
            usedc.append((props["project"], props["version"]))

        return [ n for n in activec if n not in usedc ]


    # Methods to add/update nodes
    ###########################################################################
    def setupDB(self):
        ''' Create or lookup the main entry point nodes and indices '''
        if not self.setupDone:
            self.projectVersionIndex = self.mNeoDB.get_or_create_index(neo4j.Node, "ProjectVersion")
            self.projectIndex = self.mNeoDB.get_or_create_index(neo4j.Node, "Project")
            self.lbadminIndex = self.mNeoDB.get_or_create_index(neo4j.Node, "Lbadmin")
            self.node_projectparent =  self.mNeoDB.get_or_create_indexed_node("Lbadmin",
                                                               "Type",
                                                               "PROJECT_PARENT",
                                                               {"type": "PROJECT_PARENT"})
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

            self.setupDone = True


    def getPlatformParent(self):
        self.setupDB()
        return self.node_platformparent

    def getProjectParent(self):
        self.setupDB()
        return self.node_projectparent


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





