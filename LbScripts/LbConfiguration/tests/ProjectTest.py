from LbConfiguration.Project import getTarBallName, ProjectConfException
from LbConfiguration.Project import dumpXMLConf
from LbConfiguration.Project import _getSVNProject
from LbConfiguration.Project import getProject
from LbUtils.afs.volume import BadVolumeName

import unittest
import logging

class ProjectTestCase(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def testGetTarBallName(self):
        self.assertEqual(getTarBallName("Gaudi", "v21r0"), "GAUDI_GAUDI_v21r0.tar.gz")
        self.assertEqual(getTarBallName("Gaudi", "v21r0", "slc4_ia32_gcc34"),
                                        "GAUDI_GAUDI_v21r0_slc4_ia32_gcc34.tar.gz")
        self.assertEqual(getTarBallName("Gaudi", "v21r0", "slc4_ia32_gcc28"),
                                        "GAUDI_GAUDI_v21r0_slc4_ia32_gcc28.tar.gz")
        self.assertRaises(ProjectConfException, getTarBallName, "bLah", "v21r0", "slc4_ia32_gcc34")

    def testDumpXMLConf(self):
#        print dumpXMLConf()
        pass

    def testSVNProject(self):
        p = _getSVNProject("Gaudi")
        self.assertEqual("Gaudi", p.Name())


    def testMooreOnline(self):
        p = getProject("MooreOnline")
        print p
        self.assertEqual("MooreOnline", p.Name())

    def testAFSReleaseVolumeName(self):
        p = getProject("Integration")
        self.assertEqual(p.AFSVolumeShortName(), "IN")
        self.assertEqual(p.AFSVolumeName(), "INTEG")
        self.assertEqual(p.AFSReleaseVolumeName("v1r0"), "q.lhcb.INTEG_v1r0")
        ver = "v1000r1234"
        self.assertEqual(p.AFSReleaseVolumeName(ver), "q.lhcb.IN_%s" % ver)
        ver = "v1000000000r123456789"
        self.assertRaises(BadVolumeName, p.AFSReleaseVolumeName, ver)


if __name__ == '__main__':
    log = logging.getLogger()
    log.setLevel(logging.CRITICAL)
    console = logging.StreamHandler()
    console.setFormatter(logging.Formatter("%(levelname)-8s: %(message)s"))
    log.addHandler(console)
    unittest.main()