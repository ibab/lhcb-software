from LbUtils.File import genTemplateFile
from LbUtils.Temporary import TempFile

import unittest

import os

class FileTest(unittest.TestCase):
    
    def setUp(self):
        unittest.TestCase.setUp(self)
    
    def tearDown(self):
        unittest.TestCase.tearDown(self)
        
    def testTemplate(self):
        t = TempFile()
        fdt = open(t.name, "w")
        fdt.write("wdht ddd %(tobereplaced)s\n")
        fdt.close()
        tmpl_dict = {}
        tmpl_dict["tobereplaced"] = "blah"
        genTemplateFile(t.name, tmpl_dict, "myfile.txt")
        self.assertEqual(open("myfile.txt").read(),"wdht ddd blah\n" )
        os.remove("myfile.txt")
    def testTemplate2(self):
        t = TempFile()
        fdt = open(t.name, "w")
        fdt.write("""wdht ddd %(tobereplaced)s\n""")
        fdt.close()
        tmpl_dict = {}
        tmpl_dict["tobereplaced"] = "blah"
        genTemplateFile(t.name, tmpl_dict, "myfile.txt")
        self.assertEqual(open("myfile.txt").read(),"wdht ddd blah\n" )
        os.remove("myfile.txt")
    def testTemplate3(self):
        t = TempFile()
        fdt = open(t.name, "w")
        fdt.write("""wdht ddd "%(tobereplaced)s"\n""")
        fdt.close()
        tmpl_dict = {}
        tmpl_dict["tobereplaced"] = "blah"
        genTemplateFile(t.name, tmpl_dict, "myfile.txt")
        self.assertEqual(open("myfile.txt").read(),"""wdht ddd "blah"\n""" )
        os.remove("myfile.txt")
        
if __name__ == '__main__':
    unittest.main()