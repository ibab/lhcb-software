from LbLegacy.install_project import getProjectVersions, listVersions, getVersionList
from pprint import pprint
import unittest


class InstallProjectTestCase(unittest.TestCase):
    
    def setUp(self):
        unittest.TestCase.setUp(self)
    
    def tearDown(self):
        unittest.TestCase.tearDown(self)
        
    def testVersionsList(self):
        pprint(getVersionList("DecFiles"))
        
if __name__ == '__main__':
    unittest.main()
