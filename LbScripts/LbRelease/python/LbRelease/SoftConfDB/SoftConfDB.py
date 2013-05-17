'''

Interface to LHCb Software Configuration DB in Neo4j.

This interface connects directly to the Neo4J DB via HTTP.

Created on May 2, 2013
@author: Ben Couturier
'''

from py2neo import neo4j, cypher

class SoftConfDB(object):
    '''
    Main class interfacing to the LHCb Configuration Database
    '''
    def __init__(self, dbConnectStr="http://lxbuild159:8080/db/data/"):
        '''
        Initialize the class, setting the address of the Database
        '''
        # Initialize with the server address
        self.mDBURL = dbConnectStr

        # Initializing the DB itself
        self.mNeoDB = neo4j.GraphDatabaseService(dbConnectStr)

        # Only lookup the main nodes/indices if needed
        self.setupDone = False

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

    def listStackPlatforms(self, project, version):
        query = 'start n=node:ProjectVersion(ProjectVersion="%s_%s") match p=n-[:REQUIRES*]->m-[:PLATFORM]->q  return q.platform, m.project, m.version, length(p)' \
        % (project, version)

        plist = []
        self.runCypher(query, lambda x: plist.append((x[0], x[1], x[2], x[3])))
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


    def getOrCreatePV(self, project, version):
        ''' Create a project version node, with appropriate links '''
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

        return node_pv

    def addRequires(self, parentNode, childNode):
        ''' Add a dependency between two projects, which need to have been   '''
        self.mNeoDB.get_or_create_relationships((parentNode,"REQUIRES", childNode))





