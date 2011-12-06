from LbConfiguration.ConfigurationFactory import loadProjects, loadMainConfig
from LbConfiguration.ConfigurationFactory import loadPackages
from LbConfiguration.ConfigurationFactory import serializeProjects
from LbConfiguration.ConfigurationFactory import serializePackages


import logging
import unittest
#
# Unitest for the configuration factory
# loads examples config and checks the parameters
#
class ConfigurationFactoryTestCase(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def testSimpleProjectLoad(self):
        projects = loadProjects("TestProjectConfig.xml")
        self.assertEquals(len(projects), 2)
        gaudi = projects[0]
        self.assertEquals(gaudi.Name(), "Gaudi")
        self.assertEquals(gaudi.ReleaseArea(), "/afs/cern.ch/lhcb/software/releases")
        self.assertEquals(gaudi.ApplicationPackage(), "GaudiExamples")
        self.assertEquals(gaudi.FullSize(), "3000000")#TODO this is not good, needs cast
        gaudi = projects[1]
        self.assertEquals(gaudi.Name(), "LHCb")
        self.assertEquals(gaudi.ReleaseArea(), "/afs/cern.ch/lhcb/software/releases")
        self.assertEquals(gaudi.ApplicationPackage(), "Ex")
        self.assertEquals(gaudi.FullSize(), "6000000")#TODO this is not good, needs cast


    def testLoadAllProjects(self):
        # Loading all projects and serializing them as XML
        projects = loadProjects("ExampleProjectConfig.xml")
        projxml = serializeProjects(projects)
        newxml = projxml.toprettyxml(indent=" ")
        newxml = newxml.encode("ascii")
        newxml = newxml.replace("\r", "")
        newxml = newxml.replace("\n", "")

        # Loading initial fine and sanitizing (serialized version does not have xsd ref
        f = open("ExampleProjectConfig.xml", "r")
        oldxml = f.read()
        oldxml = oldxml.replace("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"", "")
        oldxml = oldxml.replace("xsi:noNamespaceSchemaLocation=\"../data/LHCbProjectConfig.xsd\"", "")
        oldxml = oldxml.replace("\r", "")
        oldxml = oldxml.replace("\n", "")
        oldxml = oldxml.replace(" >", ">")
        oldxml = oldxml.replace("encoding=\"UTF-8\"", "")


        #for i in range(1, len(newxml)):
        #    if newxml[i] != oldxml[i]:
        #        print "ProjErr:" + newxml[i] + " - " + oldxml[i]
        #print newxml
        #print oldxml

        self.assertEquals(newxml,  oldxml)


    def testLoadAllPackages(self):
        # Loading all projects and serializing them as XML
        packages = loadPackages("ExamplePackageConfig.xml")
        pdom = serializePackages(packages)
        newxml = pdom.toprettyxml(indent=" ")
        newxml = newxml.encode("ascii")
        newxml = newxml.replace("\r", "")
        newxml = newxml.replace("\n", "")

        # Loading initial fine and sanitizing (serialized version does not have xsd ref
        f = open("ExamplePackageConfig.xml", "r")
        oldxml = f.read()
        oldxml = oldxml.replace("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"", "")
        oldxml = oldxml.replace("xsi:noNamespaceSchemaLocation=\"../data/LHCbPackageConfig.xsd\"", "")
        oldxml = oldxml.replace("\r", "")
        oldxml = oldxml.replace("\n", "")
        oldxml = oldxml.replace("<packageConfiguration  >", "<packageConfiguration>")
        oldxml = oldxml.replace("encoding=\"UTF-8\"", "")

        #print newxml
        #print oldxml

        #for i in range(1, len(newxml)):
        #    if newxml[i] != oldxml[i]:
        #        print "Err:" + newxml[i] + " - " + oldxml[i]

        self.assertEquals(newxml,  oldxml)


    def testMainConfigLoad(self):
        config = loadMainConfig("ExampleLHCbMainConfig.xml")
        self.assertEqual(config.distribution_url, u"http://cern.ch/lhcbproject/dist")
        self.assertEqual(config.Python_version, u"2.5")
        self.assertEqual(config.CMT_version, u"v1r20p20090520")
        self.assertEqual(config.tbroadcast_version, u"v2.0.5")
        self.assertEqual(config.doxygen_version, u"1.7.2")
        self.assertEqual(len(config.external_projects), 3)
        self.assertEqual(len(config.lcg_projects), 8)


if __name__ == '__main__':
    logging.basicConfig()
    log = logging.getLogger()
    log.setLevel(logging.CRITICAL)
    console = logging.StreamHandler()
    console.setFormatter(logging.Formatter("%(levelname)-8s: %(message)s"))
    log.addHandler(console)
    unittest.main()