#!/usr/bin/env python
#
#
import unittest
import sys

class BasicsTestCase(unittest.TestCase):
    def setUp(self):
        import PyLCGDict
        PyLCGDict.loadDict('TestModelDict')
        PyLCGDict.loadDict('SealCLHEPDict')
        self.gbl    = PyLCGDict.makeNamespace('')
    def tearDown(self):
        pass

    def testAttributes(self):
        p = self.gbl.TestParticle()
        p.setParticleID(99)
        self.failUnlessEqual( p.particleID(), 99)
        v = self.gbl.TestVertex()


    def testRelations(self):
        p = self.gbl.TestParticle()
        m = self.gbl.TestParticle()
        self.failUnlessEqual( p.originVertex(), None )
        v = self.gbl.TestVertex()
        v.setMother(m)
        self.failUnlessEqual( v.mother()._theObject,  m._theObject)
        p.setOriginVertex(v)
        self.failUnlessEqual( p.originVertex()._theObject,  v._theObject)
        self.failUnlessEqual( p.mother()._theObject,  m._theObject)


suite = unittest.makeSuite(BasicsTestCase,'test')
if __name__ == '__main__':
    ret = unittest.TextTestRunner( sys.stdout, verbosity = 2 ).run(suite)
    raise SystemExit, not ret.wasSuccessful()


    #unittest.main()
