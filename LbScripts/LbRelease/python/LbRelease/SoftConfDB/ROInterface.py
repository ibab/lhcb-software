'''

RO methods interfacing to the LHCb Software Configuration DB in Neo4j.

Created on Oct 7, 2014
@author: Ben Couturier
'''

import inspect
import sys
from SoftConfDB import SoftConfDB

def initModule(moduleName, objclass, obj, methodList):
    """ Add methods to  module """
    # Locate module
    module = sys.modules[moduleName]
    
    # Now iterate over the methods and add them to the module
    allmethods =  list(inspect.getmembers(objclass, predicate=inspect.ismethod))
    for method in allmethods:
        methodName, methodObj = method

        # Check if it's one of those we want to publish
        if methodName in methodList:
            func = getattr(obj, methodName)
            setattr(module, methodName, func)

ROMethods =['listProjects',
            'listDatapkgs',
            'listApplications',
            'listReleaseReqs',
            'listCMakeBuiltProjects',
            'listReleaseStackFromPV',
            'listReleaseStackTops',
            'listReleaseStacks',
            'listVersions',
            'listDatapkgVersions',
            'listStackPlatforms',
            'listStackPlatformsToRelease',
            'listPlatforms',
            'listDependencies',
            'listReferences',
            'listActiveReferences',
            'show',
            'listActive',
            'listActiveApplications',
            'listUsed',
            'checkUnused' ]


confDB = SoftConfDB(dbConnectStr="http://localhost:7474/db/data")
initModule(__name__, SoftConfDB, confDB, ROMethods)




