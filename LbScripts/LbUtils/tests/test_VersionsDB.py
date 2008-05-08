# pylint: disable-msg=W0212
import unittest
from LbUtils import VersionsDB
__author__ = "Marco Clemencic <Marco.Clemencic@cern.ch>"
__version__ = "$Id: test_VersionsDB.py,v 1.8 2008-05-08 21:22:36 marcocle Exp $"

class VersionsDBTest(unittest.TestCase):
    def _prepareXML(self, data):
        """Prepare the XML to be tested.
        'data' must have this format:
        [("ReleaseName","BaseRelease",[("proj1","v1"),("proj2","v2r0"),...]),...]"""
        xml = """<?xml version="1.0" encoding="ISO-8859-1"?><releases_db>"""
        for name,base,projects in data:
            xml += '<release name="' + name
            if base: xml += '" base="' + base
            xml += '">'
            for p_v in projects:
                xml += '<project name="%s" version="%s"/>'%p_v
            xml += '</release>'
        xml += """</releases_db>"""
        return xml
    def _fillDB(self,data):
        VersionsDB.loadString(self._prepareXML(data))
    
    def setUp(self):
        VersionsDB.clean()
    def tearDown(self):
        VersionsDB.clean()
    
    def test_001_version_class(self):
        from LbUtils.VersionsDB import Version
        # parse string, evaluate representation and convert to string
        # (all tests in one line...)
        v = "v1r2p3"
        self.assertEquals(str(eval(repr(Version(v)))),v)
        v = "v1r2"
        self.assertEquals(str(eval(repr(Version(v)))),v)
        v = "54"
        self.assertEquals(str(eval(repr(Version(v)))),v)
        v = "54e"
        self.assertEquals(str(eval(repr(Version(v)))),v)
        v = "" # unversioned
        self.assertEquals(str(eval(repr(Version(v)))),v)
        # copy constructor
        v = Version("v1r2p3")
        self.assertEquals(Version(v),v)
        v = Version("54e")
        self.assertEquals(Version(v),v)
        v = Version("")
        self.assertEquals(Version(v),v)
        # errors
        self.assertRaises(ValueError,Version,"test")
        self.assertRaises(TypeError,Version,)
        self.assertRaises(TypeError,Version,1,2,3,4)
        # sorting
        sl = ["v0r0","v0r0p1","v0r1","v1r0","v1r0p0","v1r1p0","v2r0","v10r5"]
        l = [ Version(s) for s in sl ]
        l.sort()
        res = [ str(v) for v in l ]
        self.assertEquals(res,sl)
        sl = ["51","52","52a","52b","54","54a","54e"]
        l = [ Version(s) for s in sl ]
        l.sort()
        res = [ str(v) for v in l ]
        self.assertEquals(res,sl)
        
    def test_010_create_release(self):
        ra = VersionsDB.Release("ReleaseA")
        rb = VersionsDB.Release("ReleaseB",base="ReleaseA")
        self.assertEqual(VersionsDB.getRelease("ReleaseA"),ra)
        self.assertEqual(VersionsDB.getRelease("ReleaseB"),rb)
        self.assertEquals(rb.base, "ReleaseA")
    def test_020_duplicate_release(self):
        VersionsDB.Release("ARelease")
        try:
            VersionsDB.Release("ARelease")
            self.fail("DuplicatedReleaseError exception expected")
        except VersionsDB.DuplicatedReleaseError, x:
            self.assertEquals(x.release,"ARelease")
    def test_030_add_project(self):
        r = VersionsDB.Release("ReleaseA")
        r["Project1"] = "v1r0"
        r["Project2"] = "v2r0"
        r = VersionsDB.Release("ReleaseB",base="ReleaseA")
        r["Project2"] = "v2r1"
        
        r = VersionsDB.getRelease("ReleaseA")
        self.assertEquals(r["Project1"].version,"v1r0")
        self.assertEquals(r["Project2"].version,"v2r0")
        r = VersionsDB.getRelease("ReleaseB")
        self.assertEquals(r["Project1"].version,"v1r0")
        self.assertEquals(r["Project2"].version,"v2r1")
    def test_040_missing_project(self):
        r = VersionsDB.Release("ReleaseA")
        r["Project1"] = "v1r0"
        r["Project2"] = "v2r0"
        self.assert_("Project3" not in r)
        self.assert_(not ("Project3" in r))
        
    def test_110_basic_parse(self):
        """Basic parsing"""
        data = [("ReleaseName",None,[("Proj%d"%i,"v%dr0"%i) for i in range(3)])]
        VersionsDB.loadString(self._prepareXML(data))

    def test_120_parse_errors(self):
        """XML errors"""
        data = [("ReleaseName",None,[]),("ReleaseName",None,[])]
        self.assertRaises(VersionsDB.DuplicatedReleaseError,
                          VersionsDB.loadString,
                          self._prepareXML(data))

        data = [("ReleaseName",None,[("P","v1r0"),("P","v1r0")])]
        self.assertRaises(VersionsDB.DuplicatedProjectError,
                          VersionsDB.loadString,
                          self._prepareXML(data))
        
    def test_200_findReleases(self):
        data = [("R1",None,[("P1","v1r0"),("P2","v1r0"),("P3","v1r0")]),
                ("R2",None,[("P1","v1r0"),("P2","v2r0"),("P3","v1r0")]),
                ("R3","R2",[("P3","v2r0")]),
                ]
        self._fillDB(data)
        self.assertEquals(VersionsDB._findReleases("P1","v1r0"),["R1","R2","R3"])
        self.assertEquals(VersionsDB._findReleases("P2","v1r0"),["R1"])
        self.assertEquals(VersionsDB._findReleases("P2","v2r0"),["R2","R3"])
        self.assertEquals(VersionsDB._findReleases("P3","v1r0"),["R1","R2"])
        self.assertEquals(VersionsDB._findReleases("P3","v2r0"),["R3"])
        # all releases
        self.assertEquals(VersionsDB._findAllReleases("P2",["v1r0","v2r0"]),
                          ["R1","R2","R3"])
        # non-existing versions
        self.assertEquals(VersionsDB._findReleases("P1","v2r0"),[])
        self.assertEquals(VersionsDB._findReleases("P4","v1r0"),[])
        # get versions
        self.assertEquals(VersionsDB._getVersions("P1",["R1"]),["v1r0"])
        self.assertEquals(VersionsDB._getVersions("P1",["R1","R2","R3"]),["v1r0"])
        self.assertEquals(VersionsDB._getVersions("P2",["R1","R2","R3"]),["v1r0","v2r0"])
        self.assertEquals(VersionsDB._getVersions("P3",["R1","R2","R3"]),["v1r0","v2r0"])
        self.assertEquals(VersionsDB._getVersions("P5",["R1","R2","R3"]),[])

    def test_300_getCompatibleVersions(self):
        data = [("R1",None,[("P1","v1r0"),("P2","v1r0"),("P3","v1r0"),("P4","v1r0")]),
                ("R2",None,[("P1","v2r0"),("P2","v2r0"),("P3","v1r0"),("P4","v1r0")]),
                ("R3",None,[("P1","v3r0"),("P2","v2r0"),("P3","v2r0"),("P4","v1r0")]),
                ("R4",None,[("P1","v4r0"),("P2","v2r0"),("P3","v2r0"),("P4","v2r0")]),
                ("R5","R4",[                        ("P3","v3r0")]),
                ("R6",None,[("P1","v5r0"),("P2","v2r0"),("P3","v4r0"),("P4","v3r0")]),
                ]
        self._fillDB(data)
        
        self.assertEquals(VersionsDB.getCompatibleVersions("P3","v2r0",
                                                           "P4",
                                                           "P1"),["v1r0","v2r0"])
        
        self.assertEquals(VersionsDB.getCompatibleVersions("P3","v3r0",
                                                           "P4",
                                                           "P1"),["v2r0"])
        
        self.assertEquals(VersionsDB.getCompatibleVersions("P4","v1r0",
                                                           "P3",
                                                           "P1"),["v1r0","v2r0"])
                
        self.assertEquals(VersionsDB.getCompatibleVersions("P2","v2r0",
                                                           "P3",
                                                           "P1"),["v1r0","v2r0","v3r0","v4r0"])
        
        self.assertEquals(VersionsDB.getCompatibleVersions("P3","v3r0",
                                                           "P4",
                                                           "P2"),["v1r0","v2r0","v3r0"])

        self.assertEquals(VersionsDB.getCompatibleVersions("P3","v5r0",
                                                           "P4",
                                                           "P1"),[])
        self.assertEquals(VersionsDB.getCompatibleVersions("P3","v3r0",
                                                           "P5",
                                                           "P1"),[])
        self.assertEquals(VersionsDB.getCompatibleVersions("P3","v3r0",
                                                           "P4",
                                                           "P5"),[])
        
    def _prepareDBWithDeps(self):
        r = VersionsDB.Release("r1")
        for i in range(1,8):
            r["p%d"%i] = "v1r0"
        r["p2"].addBuildTimeDep("p1")
        r["p3"].addBuildTimeDep("p1")
        r["p3"].addRunTimeDep("p2")
        r["p4"].addBuildTimeDep("p1")
        r["p5"].addRunTimeDep("p4")
        r["p6"].addBuildTimeDep("p3")
        r["p6"].addRunTimeDep("p5")
        r["p7"].addBuildTimeDep("p1")
        r["p7"].addBuildTimeDep("p2")
        r = VersionsDB.Release("r2",base="r1")
        r["z10"] = "v1r0"
        r["p5"] = "v1r1"
        r["p5"].addRunTimeDep("p4")
        r["p5"].addRunTimeDep("z10")
        
    def test_400_dependencies(self):
        self._prepareDBWithDeps()
        r = VersionsDB.getRelease("r1")
        
        deps = r.expandBuildTimeDeps("p6")
        deps.sort()
        self.assertEquals(deps,["p1","p3"])
        
        deps = r.expandRunTimeDeps("p6")
        deps.sort()
        self.assertEquals(deps,["p2","p4","p5"])
    def test_410_sort_projects(self):
        self._prepareDBWithDeps()
        
        r = VersionsDB.getRelease("r1")
        projs = [ p.name for p in r.allProjects() ]
        self.assertEquals(projs,['p1', 'p2', 'p4', 'p3', 'p5', 'p7', 'p6'])
        
        r = VersionsDB.getRelease("r2")
        projs = [ p.name for p in r.allProjects() ]
        self.assertEquals(projs,['p1', 'z10', 'p2', 'p4', 'p3', 'p5', 'p7', 'p6'])
        
        projs = [ p.name for p in r.values() ]
        self.assertEquals(projs,['z10', 'p5'])
    def test_420_dependencies_loop(self):
        r = VersionsDB.Release("r1")
        r["a"] = "v1r0"
        r["b"] = "v1r0"
        r["c"] = "v1r0"
        
        r["b"].addBuildTimeDep("a")
        r["c"].addBuildTimeDep("b")
        r["a"].addBuildTimeDep("c")
        
        self.assertRaises(VersionsDB.DependencyLoopError,VersionsDB._sortProjects,[r['a'],r['b'],r['c']])
    def test_500_unversioned_projects(self):
        r = VersionsDB.Release("r1")
        r["a"] = "v1r0"
        r["b"] = "v1r0"
        r["c"] = "v1r0"
        VersionsDB.addUnversionedProject("u1")
        VersionsDB.addUnversionedProject("u2").addRunTimeDep("u1")
        
        r["b"].addRunTimeDep("u2")
        r["c"].addRunTimeDep("b")
        
        rtd = r.expandRunTimeDeps("c")
        rtd.sort()
        self.assertEquals(rtd,["b","u1","u2"])
        
        expected = [ ("b","v1r0"), ("u1",""), ("u2","") ]
        rtv = VersionsDB.getRuntimeVersions("c","v1r0","a")
        rtv.sort()
        self.assertEquals(rtv,expected)
        
        self.assertEquals([p.name for p in r.allProjects()],["a","u1","u2","b","c"])
    def test_600_release_date(self):
        def test(self):
            # check content of db
            r = VersionsDB.getRelease("R1")
            self.assertEquals(r.date, VersionsDB.DEFAULT_RELEASEDATE)
            r = VersionsDB.getRelease("R2")
            self.assertEquals(r.date, (2008,5,7))
        # prepare db
        VersionsDB.Release("R1")
        VersionsDB.Release("R2", date = (2008,5,7))
        
        test(self)
        
        # check XML generation 
        xml = VersionsDB.generateXML()
        VersionsDB.loadString(xml)
        
        test(self)

    def test_610_project_version_tag(self):
        def test(self):
            # check content of db
            r = VersionsDB.getRelease("R1")
            self.assertEquals(r["p1"].tag, None)
            self.assertEquals(r["p2"].tag, "DC06")
        # prepare db
        r = VersionsDB.Release("R1")
        r.add(VersionsDB.Project("p1", "v1r0"))
        r.add(VersionsDB.Project("p2", "v1r0", tag = "DC06"))
        
        test(self)
        
        # check XML generation 
        xml = VersionsDB.generateXML()
        VersionsDB.loadString(xml)
        
        test(self)
        
    def test_900_generateXML(self):
        data = [("R1",None,[("P1","v1r0"),("P2","v1r0"),("P3","v1r0"),("P4","v1r0")]),
                ("R2",None,[("P1","v2r0"),("P2","v2r0"),("P3","v1r0"),("P4","v1r0")]),
                ("R3",None,[("P1","v3r0"),("P2","v2r0"),("P3","v2r0"),("P4","v1r0")]),
                ("R4",None,[("P1","v4r0"),("P2","v2r0"),("P3","v2r0"),("P4","v2r0")]),
                ("R5","R4",[                        ("P3","v3r0")]),
                ("R6",None,[("P1","v5r0"),("P2","v2r0"),("P3","v4r0"),("P4","v3r0")]),
                ]
        self._fillDB(data)
        VersionsDB.addUnversionedProject("U1")
        VersionsDB.addUnversionedProject("U2").addRunTimeDep("U1")
        xml = VersionsDB.generateXML()
        #print repr(xml)
        VersionsDB.loadString(xml)
        self.assertEquals(xml,VersionsDB.generateXML())

    def test_910_generateXML_deps(self):
        self._prepareDBWithDeps()
        
        xml = VersionsDB.generateXML()
        #print repr(xml)
        VersionsDB.loadString(xml)
        self.assertEquals(xml,VersionsDB.generateXML())
        
if __name__ == "__main__":
    unittest.main()
