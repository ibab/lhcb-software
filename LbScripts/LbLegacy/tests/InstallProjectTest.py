from LbLegacy.install_project import getProjectVersions, listVersions, getVersionList
from LbLegacy.install_project import getFile, script_version, getLatestVersion

from LbUtils.Temporary import TempDir


from pprint import pprint

import shutil
import unittest
import os

url_dist = 'http://lhcbproject.web.cern.ch/lhcbproject/dist/'


#============================================================================
# bootstrapping the location of the file
try:
    _this_file = __file__
    _lblg_tests_dir = os.path.dirname(_this_file)
    _lblg_dir = os.path.dirname(_lblg_tests_dir)
    _lblg_py_dir = os.path.join(_lblg_dir, "python")
    _lblg_ip = os.path.join(_lblg_py_dir, "LbLegacy", "install_project.py")
except NameError :
    # special procedure to handle the situation when __file__ is not defined.
    # It happens typically when trying to use pdb.
    from imp import find_module, load_module
    _ff, _filename, _desc = find_module("LbLegacy")
    try :
        lbconf_package = load_module('LbLegacy', _ff, _filename, _desc)
        _ff, _filename, _desc = find_module('install_project', lbconf_package.__path__)
        _lblg_ip = _filename
        _lblg_py_dir = os.path.dirname(os.path.dirname(_lblg_ip))
        _lblg_dir = os.path.dirname(_lblg_py_dir)
        _lblg_tests_dir = os.path.join(_lblg_dir, "tests")
        _this_file = os.path.join(_lblg_tests_dir, "InstallProjectTest.py")
    finally :
        _ff.close()


class InstallProjectTestCase(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

    def tearDown(self):
        if os.path.exists("latest_install_project.py") :
            os.remove("latest_install_project.py")
        unittest.TestCase.tearDown(self)

    def testVersionsList(self):
        self.assertEqual(getVersionList("DecFiles", "v20r0")[0],"DBASE_Gen_DecFiles_v20r0.tar.gz")


    def testSelfVersion(self):
        new_install = _lblg_ip
        latest_version = os.popen("python %s --version" % _lblg_ip).read()[:-1]
        self.assertEqual(script_version, latest_version)

class InstallProjectDeploymentTestCase(unittest.TestCase):
    def setUp(self):
        unittest.TestCase.setUp(self)
        self.tmp = TempDir("testInstallProject")
        self.tmpdir = self.tmp.getName()
        self.distdir = os.path.join(self.tmpdir, "dist")
        os.mkdir(self.distdir)
        self.vodir = os.path.join(self.tmpdir, "vo")
        os.mkdir(self.vodir)
        self.mysiteroot = os.path.join(self.vodir, "lib")
        os.mkdir(self.mysiteroot)
        self.ip = os.path.join(self.mysiteroot, "install_project.py")
        shutil.copy(_lblg_ip, self.ip)

    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def testSetup(self):
        self.assertTrue(os.path.exists(self.distdir))
        self.assertTrue(os.path.exists(self.vodir))
        self.assertTrue(os.path.exists(self.mysiteroot))
        self.assertTrue(os.path.isfile(self.ip))

    def testSelfVersion(self):
        latest_version = os.popen("python %s --version" % self.ip).read()[:-1]
        self.assertEqual(script_version, latest_version)


if __name__ == '__main__':
    unittest.main()
