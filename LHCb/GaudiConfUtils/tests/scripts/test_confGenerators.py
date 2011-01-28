'''
Created on Jan 28, 2011

@author: marcocle
'''
import unittest

class Test(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        from GaudiKernel.Configurable import purge
        purge()

    def test_000_import(self):
        import GaudiConfUtils.ConfigurableGenerators
        from GaudiConfUtils.ConfigurableGenerators import ApplicationMgr

    def test_010_generate(self):
        from GaudiConfUtils.ConfigurableGenerators import MessageSvc
        from Gaudi.Configuration import allConfigurables

        self.assert_("MessageSvc" not in allConfigurables)

        gen = MessageSvc(OutputLevel = 1)
        ms = gen.configurable("MessageSvc")

        self.assert_("MessageSvc" in allConfigurables)
        self.assertEquals(ms.OutputLevel, gen.OutputLevel)

    def test_020_properties(self):
        from GaudiConfUtils.ConfigurableGenerators import MessageSvc
        from Gaudi.Configuration import allConfigurables

        self.assert_("MessageSvc" not in allConfigurables)

        try:
            MessageSvc.NoPropName = "test"
            self.fail("No exception raised assigning to class")
        except AttributeError:
            pass
        except Exception, x:
            self.fail("wrong exception raised assigning to class: %s" % x)

        try:
            MessageSvc.OutputLevel = 3
            self.fail("No exception raised assigning to class")
        except AttributeError:
            pass
        except Exception, x:
            self.fail("wrong exception raised assigning to class: %s" % x)

        gen = MessageSvc()

        try:
            gen.NoPropName = "test"
            self.fail("No exception raised assigning to wrong property")
        except AttributeError:
            pass
        except Exception, x:
            self.fail("wrong exception raised assigning to wrong property: %s" % x)

        gen.OutputLevel = 5

        ms = gen("MessageSvc")

        self.assert_("MessageSvc" in allConfigurables)
        self.assertEquals(ms.OutputLevel, gen.OutputLevel)

    def test_020_repr(self):
        from GaudiConfUtils.ConfigurableGenerators import MessageSvc
        gen = MessageSvc(OutputLevel = 7)
        self.assertEquals(gen, eval(repr(gen)))

if __name__ == "__main__":
    import sys
    unittest.main(testRunner = unittest.TextTestRunner(stream = sys.stdout,
                                                       verbosity = 2))
