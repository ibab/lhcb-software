'''

Interface to LHCb Software Configuration DB in Neo4j.

This interface connects to the HTTP XMLRPC bridge to Neo4j.

Created on Nov 24, 2014
@author: Ben Couturier
'''

import logging
import xmlrpclib
from LbConfiguration.Version import sortVersions, LCGVersion

class SoftConfDBRO(object):
    '''
    Main class interfacing to the LHCb Configuration Database in RO,
    using the XMLRPC bridge
    Prod Bridge URL: http://ariadne-lhcb.cern.ch/xmlrpc/
    '''
    def __init__(self, dbConnectStr="http://ariadne-lhcb.cern.ch/xmlrpc/"):
        '''
        Initialize the class, setting the address of the Database
        '''
        self.log = logging.getLogger()

        # Choosing the db string
        import os
        envdburl = os.environ.get('SDBBURL')
        if envdburl != None:
            if envdburl == "DEV":
                dbConnectStr = "http://ariadne-lhcb.cern.ch/xmlrpc/"
                self.log.info("SDBBURL set to DEV: Using DB URL %s" % dbConnectStr)
            else:
                dbConnectStr = envdburl
                self.log.info("SDBBURL set to %s" % dbConnectStr)

        # Initialize with the server address
        self.log.debug("Connecting to Neo4j Bridge: %s" % dbConnectStr)
        self.mDBURL = dbConnectStr

        # Initializing the DB itself
        self.mProxy = xmlrpclib.ServerProxy(self.mDBURL, allow_none = True)
        self.mSupportedMethods = ['listProjects',
                                  'listDatapkgs',
                                  'listApplications',
                                  'listReleaseReqs',
                                  'listCMakeBuiltProjects',
                                  'listVersions',
                                  'listDatapkgVersions',
                                  'listStackPlatforms',
                                  'listPlatforms',
                                  'listDependencies',
                                  'listReferences',
                                  'listActiveReferences',
                                  'listActive',
                                  'listActiveApplications',
                                  'listUsed',
                                  'checkUnused' ]


    # Methods to query the database
    ###########################################################################
    def __getattr__(self, name):
        if name in self.mSupportedMethods:
            def wrapper(*args, **kwargs):
                m =  getattr(self.mProxy, name)
                return m(*args, **kwargs)
            return wrapper

    def findVersion(self, project, version):
        return  [ True ]

if __name__ == '__main__':
    pass
    sdb = SoftConfDBRO()
    print sdb.listPlatforms("DAVINCI", "v36r2")
    

