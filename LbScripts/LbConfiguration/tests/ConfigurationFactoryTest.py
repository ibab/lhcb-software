from LbConfiguration.ConfigurationFactory import loadProjects, loadMainConfig, loadPackages
from LbConfiguration.ConfigurationFactory import Factory
from LbConfiguration.ConfigurationFactory import serializeProjects, serializePackages

import logging
import unittest
import os

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
        projects = loadProjects(os.path.join(os.path.dirname(__file__), "TestProjectConfig.xml"))
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

    def loadAllProjects(self, url):
        # Loading all projects and serializing them as XML
        projects = loadProjects(url)
        projxml = serializeProjects(projects)
        newxml = projxml.toprettyxml(indent=" ")
        # Loading initial fine and sanitizing (serialized version does not have xsd ref
        f = open(os.path.join(os.path.dirname(__file__), "conf", "ProjectConfig.xml"), "r")
        oldxml = f.read()
        self.compareProjectsXML(oldxml, newxml)


    def compareProjectsXML(self, oldxml, newxml):
        newxml = newxml.encode("ascii")
        newxml = newxml.replace("\r", "")
        newxml = newxml.replace("\n", "")

        oldxml = oldxml.replace("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"", "")
        oldxml = oldxml.replace("xsi:noNamespaceSchemaLocation=\"../../data/LHCbProjectConfig.xsd\"", "")
        oldxml = oldxml.replace("\r", "")
        oldxml = oldxml.replace("\n", "")
        oldxml = oldxml.replace(" >", ">")
        oldxml = oldxml.replace("encoding=\"UTF-8\"", "")

        #print newxml
        #print oldxml

        #for i in range(1, len(newxml)):
        #    if newxml[i] != oldxml[i]:
        #        print "Err:" + newxml[i] + " - " + oldxml[i]
        self.assertEquals(newxml,  oldxml)


    def testLoadAllProjectsFromFile(self):
        self.loadAllProjects(os.path.join(os.path.dirname(__file__), "conf", "ProjectConfig.xml"))


    def testProjectFactory(self):
        factory = Factory()
        factory.setConfigDir(os.path.join(os.path.dirname(__file__), "conf"))
        projects = factory.getProjects()
        projxml = serializeProjects(projects)
        newxml = projxml.toprettyxml(indent=" ")
        f = open(os.path.join(os.path.dirname(__file__), "conf", "ProjectConfig.xml"), "r")
        oldxml = f.read()
        self.compareProjectsXML(oldxml, newxml)

    def testSingleProjectFactory(self):
        factory = Factory()
        factory.setConfigDir(os.path.join(os.path.dirname(__file__), "conf"))
        gaudi = factory.getProject("Gaudi")
        self.assertEquals(gaudi.Name(), "Gaudi")

    def loadAllPackages(self, url):
        packages = loadPackages(url)
        pdom = serializePackages(packages)
        newxml = pdom.toprettyxml(indent=" ")
        f = open(os.path.join(os.path.dirname(__file__), "conf", "PackageConfig.xml"), "r")
        oldxml = f.read()
        self.comparePackagesXML(oldxml, newxml)


    def comparePackagesXML(self, oldxml, newxml):
        newxml = newxml.encode("ascii")
        newxml = newxml.replace("\r", "")
        newxml = newxml.replace("\n", "")

        oldxml = oldxml.replace("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"", "")
        oldxml = oldxml.replace("xsi:noNamespaceSchemaLocation=\"../../data/LHCbPackageConfig.xsd\"", "")
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

    def testLoadAllPackagesFromFile(self):
        self.loadAllPackages(os.path.join(os.path.dirname(__file__), "conf", "PackageConfig.xml"))

    def testPackageFactory(self):
        factory = Factory()
        factory.setConfigDir(os.path.join(os.path.dirname(__file__), "conf"))
        projects = factory.getPackages()
        projxml = serializePackages(projects)
        newxml = projxml.toprettyxml(indent=" ")
        f = open(os.path.join(os.path.dirname(__file__), "conf", "PackageConfig.xml"), "r")
        oldxml = f.read()
        self.comparePackagesXML(oldxml, newxml)


    def testMainConfigLoad(self):
        config = loadMainConfig(os.path.join(os.path.dirname(__file__), "conf", "MainConfig.xml"))
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
    log.setLevel(logging.INFO)
    console = logging.StreamHandler()
    console.setFormatter(logging.Formatter("%(levelname)-8s: %(message)s"))
    log.addHandler(console)
    unittest.main()
