from LbLegacy.install_project import getProjectVersions, listVersions, getVersionList
from LbLegacy.install_project import getFile, script_version
from pprint import pprint
import unittest
import os

url_dist = 'http://lhcbproject.web.cern.ch/lhcbproject/dist/'

class InstallProjectTestCase(unittest.TestCase):
    
    def setUp(self):
        unittest.TestCase.setUp(self)
    
    def tearDown(self):
        if os.path.exists("latest_install_project.py") :
            os.remove("latest_install_project.py")
        unittest.TestCase.tearDown(self)
        
    def testVersionsList(self):
        pprint(getVersionList("DecFiles"))
        
    def testSelfVersion(self):
        new_install = os.path.join(os.environ["LBLEGACYROOT"], 
                                   "python", "LbLegacy", "install_project.py")
        latest_version = os.popen("python %s --version" % new_install).read()[:-1]
        self.assertEqual(script_version, latest_version)

if __name__ == '__main__':
    unittest.main()
