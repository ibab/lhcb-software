# -*- coding: utf-8 -*-
'''
Tests for SvnChecker

Created on Nov 24, 2010
'''
from LbRelease.SvnChecker.StdCheckers import MovePackage
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"

import unittest

import LHCbCheckers
from Core import FakeTransaction
from Core import Failure, Success, Not
from StdCheckers import AllowedUsers
from StdCheckers import AllPaths, PackageTag, ProjectTag, TagRemoval
from StdCheckers import TagIntermediateDirs, ValidXml, ValidPythonEncoding

import logging
import sys

class Test(unittest.TestCase):

    package_tags = [(FakeTransaction({'/Project/tags/Package/v1r0': ('A', (1234, '/Project/trunk/Package'), 'dir')},
                                     {'/Project/trunk/Package':'dir'}),
                                     True),
                    (FakeTransaction({'/Project/tags/Hat/Package/v1r0': ('A', (1234, '/Project/trunk/Hat/Package'), 'dir')},
                                     {'/Project/trunk/Hat/Package':'dir'}),
                                     True),
                    (FakeTransaction({'/Project/tags/Package/v1r1': ('A', (1234, '/Project/branches/Package/v1r0b'), 'dir')},
                                     {'/Project/branches/Package/v1r0b':'dir'}),
                                     True),
                    (FakeTransaction({'/Project/tags/Package/v1r1': ('A', (1234, '/Project/tags/Package/v1r0'), 'dir')},
                                     {'/Project/tags/Package/v1r0':'dir'}),
                                     True),
                    (FakeTransaction({'/Project/tags/Package/test': ('A', (1234, '/Project/trunk/Package'), 'dir')},
                                     {'/Project/trunk/Package':'dir'}),
                                     (False, 'Invalid tag copy')),
                    (FakeTransaction({'/Project/tags/Package/v1r1': ('A', (1234, '/Project/branches/Package'), 'dir')},
                                     {'/Project/branches/Package':'dir'}),
                                     (False, 'Invalid tag copy')),
                    (FakeTransaction({'/Project/tags/v1r1': ('A', (1234, '/Project/trunk'), 'dir')},
                                     {'/Project/trunk/Package':'dir'}),
                                     (False, 'Invalid tag copy')),
                    #(FakeTransaction({'/Project/tags/Hat/v1r0': ('A', (1234, '/Project/trunk/Hat'), 'dir')},
                    #                 {'/Project/trunk/Hat/Package/cmt':'dir'}),
                    #                 False),
                    ]
    project_tags = [
                    (FakeTransaction({'/Project/tags/PROJECT/PROJECT_v1r0/cmt': ('A', (999, '/Project/trunk/cmt'), 'dir')},
                                     {'/Project/trunk/cmt': 'dir',
                                      '/Project/trunk/Package': 'dir',
                                      }),
                                      True),
                    (FakeTransaction({'/Project/tags/PROJECT/PROJECT_v1r0/CMakeLists.txt': ('A', (999, '/Project/trunk/CMakeLists.txt'), 'file')},
                                     {'/Project/trunk/CMakeLists.txt': 'file',
                                      '/Project/trunk/Package': 'dir',
                                      }),
                                      True),
                    (FakeTransaction({'/Project/tags/PROJECT/PROJECT_v1r0/toolchain.cmake': ('A', (999, '/Project/trunk/toolchain.cmake'), 'file')},
                                     {'/Project/trunk/toolchain.cmake': 'file',
                                      '/Project/trunk/Package': 'dir',
                                      }),
                                      True),
                    (FakeTransaction({'/Project/tags/PROJECT/PROJECT_v1r0/cmt':
                                         ('A', (999, '/Project/branches/PROJECT/PROJECT_v1b/cmt'), 'dir')},
                                     {'/Project/trunk/cmt': 'dir',
                                      '/Project/trunk/Package': 'dir',
                                      }),
                                      True),
                    (FakeTransaction({'/Project/tags/PROJECT/PROJECT_v1r0':
                                         ('A', (999, '/Project/branches/PROJECT/PROJECT_v1b'), 'dir')},
                                     {'/Project/trunk/cmt': 'dir',
                                      '/Project/trunk/Package': 'dir',
                                      }),
                                      True),
                    (FakeTransaction({'/Project/tags/PROJECT/PROJECT_v1r1/cmt':
                                         ('A', (999, '/Project/tags/PROJECT/PROJECT_v1r0/cmt'), 'dir')},
                                     {'/Project/trunk/cmt': 'dir',
                                      '/Project/trunk/Package': 'dir',
                                      }),
                                      True),
                    (FakeTransaction({'/Project/tags/PROJX/PROJECT_v1r0/cmt': ('A', (999, '/Project/trunk/cmt'), 'dir')},
                                     {'/Project/trunk/cmt': 'dir',
                                      '/Project/trunk/Package': 'dir',
                                      }), False),
                    (FakeTransaction({'/Project/tags/PROJECT/PROJX_v1r0/cmt': ('A', (999, '/Project/trunk/cmt'), 'dir')},
                                     {'/Project/trunk/cmt': 'dir',
                                      '/Project/trunk/Package': 'dir',
                                      }), False),
                    (FakeTransaction({'/Project/tags/PROJECT/PROJECT_v1r0/cmt': ('A', (999, '/ProjX/trunk/cmt'), 'dir')},
                                     {'/Project/trunk/cmt': 'dir',
                                      '/Project/trunk/Package': 'dir',
                                      }), False),
                    ]

    tags_removal = [
                    (FakeTransaction({'/Project/tags/Package/v1r0': ('D', (-1, None), 'dir')},
                                     {'/Project/trunk/cmt': 'dir',
                                      '/Project/trunk/Package': 'dir',
                                      }), True),
                    (FakeTransaction({'/Project/tags/Hat/Package/v1r0': ('D', (-1, None), 'dir')},
                                     {'/Project/trunk/cmt': 'dir',
                                      '/Project/trunk/Hat/Package': 'dir',
                                      }), True),
                    (FakeTransaction({'/Project/tags/PROJECT/PROJECT_v1r0': ('D', (-1, None), 'dir')},
                                     {'/Project/trunk/cmt': 'dir',
                                      '/Project/trunk/Package': 'dir',
                                      }), True),
                    (FakeTransaction({'/Project/tags/Package/v1r0/cmt': ('D', (-1, None), 'dir')},
                                     {'/Project/trunk/cmt': 'dir',
                                      '/Project/trunk/Package': 'dir',
                                      }), False),
                    (FakeTransaction({'/Project/tags/Package/test': ('D', (-1, None), 'dir')},
                                     {'/Project/trunk/cmt': 'dir',
                                      '/Project/trunk/Package': 'dir',
                                      }), False),
                    (FakeTransaction({'/Project/tags/PROJECT/PROJECT_pippo': ('D', (-1, None), 'dir')},
                                     {'/Project/trunk/cmt': 'dir',
                                      '/Project/trunk/Package': 'dir',
                                      }), False),
                    ]

    tags_int_dirs = [
                     # Package tag directory
                     (FakeTransaction({'/Project/tags/Package': ('A', (-1, None), 'dir')},
                                      {'/Project/trunk/Package': 'dir'}), True),
                     # Project tag directories
                     (FakeTransaction({'/Project/tags/PROJECT': ('A', (-1, None), 'dir'),
                                       '/Project/tags/PROJECT/PROJECT_v1r0': ('A', (-1, None), 'dir')},
                                       {'/Project/trunk/Package': 'dir'}), True),
                     (FakeTransaction({'/Project/tags/PROJECT': ('A', (-1, None), 'dir'),
                                       '/Project/tags/PROJECT/PROJECT_hello': ('A', (-1, None), 'dir')},
                                       {'/Project/trunk/Package': 'dir'}), False),
                     (FakeTransaction({'/Project/tags/PROJECT1': ('A', (-1, None), 'dir')},
                                      {'/Project/trunk/Package': 'dir'}), False),
                     ]
    normal_commits = [
                      (FakeTransaction({'/Project/trunk/Package/src/NewClass.cpp': ('A', (-1, None), 'file'),
                                        '/Project/trunk/Package/cmt/requirements': ('M', (-1, None), 'file'),
                                        },
                                       {'/Project/trunk/Package': 'dir'}), True),
                      ]

    xml_files = [
                 (FakeTransaction({"/path/to/xml/data.xml": ('M', (-1, None), 'file')},
                                  files = {"/path/to/xml/data.xml": """<root>
  <group name="1">
    <node>1</node>
  </group>
</root>"""}), True),
                 (FakeTransaction({"/path/to/xml/data.xml": ('M', (-1, None), 'file')},
                                  files = {"/path/to/xml/data.xml": """<root>
  <group name="1">
    <node>1<node>
  </group>
</root>"""}), False),
                 (FakeTransaction({"/path/to/xml/data.txt": ('M', (-1, None), 'file')},
                                  files = {"/path/to/xml/data.txt": "text"}), True)
                 ]

    py_files = [
                 (FakeTransaction({"/path/to/xml/good_ascii.py": ('M', (-1, None), 'file')},
                                  files = {"/path/to/xml/good_ascii.py": """
print 'hello world'
"""}), True),
                (FakeTransaction({"/path/to/xml/good_utf8.py": ('M', (-1, None), 'file')},
                                  files = {"/path/to/xml/good_utf8.py":
"""# -*- coding: utf-8 -*-
print u'\xe4\xbd\xa0\xe5\xa5\xbd\xe4\xb8\x96\xe7\x95\x8c'
"""}), True),
                (FakeTransaction({"/path/to/xml/bad_utf8.py": ('M', (-1, None), 'file')},
                                  files = {"/path/to/xml/bad_utf8.py":
"""
print u'\xe4\xbd\xa0\xe5\xa5\xbd\xe4\xb8\x96\xe7\x95\x8c'
"""}), False),
                 (FakeTransaction({"/path/to/xml/removed.py": ('D', (-1, None), 'file')},
                                  files = {}), True),
                 ]

    move_package = [
                    # good one
                    (FakeTransaction({'/Dest/trunk/Package': ('A', (999, '/Source/trunk/Package'), 'dir'),
                                      '/Dest/tags/Package': ('A', (999, '/Source/tags/Package'), 'dir'),
                                      '/Dest/branches/Package': ('A', (999, '/Source/branches/Package'), 'dir'),
                                      '/Source/trunk/Package': ('D', (-1, None), 'dir'),
                                      '/Source/tags/Package': ('D', (-1, None), 'dir'),
                                      '/Source/branches/Package': ('D', (-1, None), 'dir'),
                                      '/': ('M', (-1, None), 'dir')},
                                      {'/': 'dir'},
                                      node_properties={'/': {'packages':
                                                             "\n".join(["Package Dest"])
                                                             }
                                                       }),
                                      True),
                    # good one with hat
                    (FakeTransaction({'/Dest/trunk/Hat/Package': ('A', (999, '/Source/trunk/Hat/Package'), 'dir'),
                                      '/Dest/tags/Hat/Package': ('A', (999, '/Source/tags/Hat/Package'), 'dir'),
                                      '/Dest/branches/Hat/Package': ('A', (999, '/Source/branches/Hat/Package'), 'dir'),
                                      '/Source/trunk/Hat/Package': ('D', (-1, None), 'dir'),
                                      '/Source/tags/Hat/Package': ('D', (-1, None), 'dir'),
                                      '/Source/branches/Hat/Package': ('D', (-1, None), 'dir'),
                                      '/': ('M', (-1, None), 'dir')},
                                      {'/': 'dir'},
                                      node_properties={'/': {'packages':
                                                             "\n".join(["Hat/Package Dest"])
                                                             }
                                                       }),
                                      True),
                    # good two
                    (FakeTransaction({'/Dest/trunk/Package': ('A', (999, '/Source/trunk/Package'), 'dir'),
                                      '/Dest/tags/Package': ('A', (999, '/Source/tags/Package'), 'dir'),
                                      '/Dest/branches/Package': ('A', (999, '/Source/branches/Package'), 'dir'),
                                      '/Source/trunk/Package': ('D', (-1, None), 'dir'),
                                      '/Source/tags/Package': ('D', (-1, None), 'dir'),
                                      '/Source/branches/Package': ('D', (-1, None), 'dir'),
                                      '/Dest2/trunk/Hat/Package': ('A', (999, '/Source/trunk/Hat/Package'), 'dir'),
                                      '/Dest2/tags/Hat/Package': ('A', (999, '/Source/tags/Hat/Package'), 'dir'),
                                      '/Dest2/branches/Hat/Package': ('A', (999, '/Source/branches/Hat/Package'), 'dir'),
                                      '/Source/trunk/Hat/Package': ('D', (-1, None), 'dir'),
                                      '/Source/tags/Hat/Package': ('D', (-1, None), 'dir'),
                                      '/Source/branches/Hat/Package': ('D', (-1, None), 'dir'),
                                      '/': ('M', (-1, None), 'dir')},
                                      {'/': 'dir'},
                                      node_properties={'/': {'packages':
                                                             "\n".join(["Package Dest",
                                                                        "Hat/Package Dest2"])
                                                             }
                                                       }),
                                      True),
                    # bad property
                    (FakeTransaction({'/Dest/trunk/Package': ('A', (999, '/Source/trunk/Package'), 'dir'),
                                      '/Dest/tags/Package': ('A', (999, '/Source/tags/Package'), 'dir'),
                                      '/Dest/branches/Package': ('A', (999, '/Source/branches/Package'), 'dir'),
                                      '/Source/trunk/Package': ('D', (-1, None), 'dir'),
                                      '/Source/tags/Package': ('D', (-1, None), 'dir'),
                                      '/Source/branches/Package': ('D', (-1, None), 'dir'),
                                      '/': ('M', (-1, None), 'dir')},
                                      {'/': 'dir'},
                                      node_properties={'/': {'packages':
                                                             "\n".join(["Package Source"])
                                                             }
                                                       }),
                                      False),
                    # no change in '/'
                    (FakeTransaction({'/Dest/trunk/Package': ('A', (999, '/Source/trunk/Package'), 'dir'),
                                      '/Dest/tags/Package': ('A', (999, '/Source/tags/Package'), 'dir'),
                                      '/Dest/branches/Package': ('A', (999, '/Source/branches/Package'), 'dir'),
                                      '/Source/trunk/Package': ('D', (-1, None), 'dir'),
                                      '/Source/tags/Package': ('D', (-1, None), 'dir'),
                                      '/Source/branches/Package': ('D', (-1, None), 'dir')},
                                      {'/': 'dir'},
                                      node_properties={'/': {'packages':
                                                             "\n".join(["Package Dest"])
                                                             }
                                                       }),
                                      False),
                    # bad destination in property
                    (FakeTransaction({'/Dest/trunk/Package': ('A', (999, '/Source/trunk/Package'), 'dir'),
                                      '/Dest/tags/Package': ('A', (999, '/Source/tags/Package'), 'dir'),
                                      '/Dest/branches/Package': ('A', (999, '/Source/branches/Package'), 'dir'),
                                      '/Source/trunk/Package': ('D', (-1, None), 'dir'),
                                      '/Source/tags/Package': ('D', (-1, None), 'dir'),
                                      '/Source/branches/Package': ('D', (-1, None), 'dir'),
                                      '/': ('M', (-1, None), 'dir')},
                                      {'/': 'dir'},
                                      node_properties={'/': {'packages':
                                                             "\n".join(["Package Other"])
                                                             }
                                                       }),
                                      False),
                    # bad source of copy
                    (FakeTransaction({'/Dest/trunk/Package': ('A', (999, '/Source/trunk/Package'), 'dir'),
                                      '/Dest/tags/Package': ('A', (999, '/Other/tags/Package'), 'dir'),
                                      '/Dest/branches/Package': ('A', (999, '/Source/branches/Package'), 'dir'),
                                      '/Source/trunk/Package': ('D', (-1, None), 'dir'),
                                      '/Source/tags/Package': ('D', (-1, None), 'dir'),
                                      '/Source/branches/Package': ('D', (-1, None), 'dir'),
                                      '/': ('M', (-1, None), 'dir')},
                                      {'/': 'dir'},
                                      node_properties={'/': {'packages':
                                                             "\n".join(["Package Dest"])
                                                             }
                                                       }),
                                      False),
                    # Extra changes
                    (FakeTransaction({'/Dest/trunk/Package': ('A', (999, '/Source/trunk/Package'), 'dir'),
                                      '/Dest/tags/Package': ('A', (999, '/Source/tags/Package'), 'dir'),
                                      '/Dest/branches/Package': ('A', (999, '/Source/branches/Package'), 'dir'),
                                      '/Source/trunk/Package': ('D', (-1, None), 'dir'),
                                      '/Source/tags/Package': ('D', (-1, None), 'dir'),
                                      '/Source/branches/Package': ('D', (-1, None), 'dir'),
                                      '/': ('M', (-1, None), 'dir'),
                                      "/some/file.txt": ('M', (-1, None), 'file')},
                                      {'/': 'dir'},
                                      node_properties={'/': {'packages':
                                                             "\n".join(["Package Dest"])
                                                             }
                                                       }),
                                      False),
                    ]


    def assertCheckTxn(self, txn, checker, result):
        logging.getLogger("assertCheckTxn").info("--- check ---")
        check = checker(txn)
        msg = "Expected %r but found %r checking\n%s" % (result, check, txn)
        if type(result) is tuple:
            self.assertEquals(check, result, msg = msg)
        else:
            self.assertEquals(check[0], result, msg = msg)

    def test_010_basic_logic(self):
        """basic logic"""
        txn = None

        # shortcuts
        S, F = Success, Failure

        # constants
        self.assertCheckTxn(txn, S("OK"), (True, "OK"))
        self.assertCheckTxn(txn, F("BAD"), (False, "BAD"))

        # not
        self.assertCheckTxn(txn, -S("OK"), (False, "OK"))
        self.assertCheckTxn(txn, -F("BAD"), (True, "BAD"))

        # and
        self.assertCheckTxn(txn, S("First") * S("Second"), (True, "Second"))
        self.assertCheckTxn(txn, S("First") * F("Second"), (False, "Second"))
        self.assertCheckTxn(txn, F("First") * S("Second"), (False, "First"))
        self.assertCheckTxn(txn, F("First") * F("Second"), (False, "First"))

        # or
        self.assertCheckTxn(txn, S("First") + S("Second"), (True, "First"))
        self.assertCheckTxn(txn, S("First") + F("Second"), (True, "First"))
        self.assertCheckTxn(txn, F("First") + S("Second"), (True, "Second"))
        self.assertCheckTxn(txn, F("First") + F("Second"), (False, "Second"))

        # chains
        a, b, c = "abc" # messages
        self.assertCheckTxn(txn, S(a) * S(b) + S(c), (True, b))
        self.assertCheckTxn(txn, S(a) * F(b) + S(c), (True, c))
        self.assertCheckTxn(txn, S(a) + S(b) * F(c), (True, a))
        self.assertCheckTxn(txn, F(a) + S(b) * F(c), (False, c))
        self.assertCheckTxn(txn, S(a) * S(b) * F(c), (False, c))
        self.assertCheckTxn(txn, F(a) + S(b) + F(c), (True, b))

    def test_020_allowed_user(self):
        """allowed user"""
        users = ["a", "b", "c"]
        S, F, AU = Success("success"), Failure("failure"), AllowedUsers(users)

        # not allowed user
        txn = FakeTransaction(properties = {"svn:author": "none"})
        #  - on good operation
        self.assertCheckTxn(txn, AU + S, (True, "success"))
        #  - on bad operation
        self.assertCheckTxn(txn, AU + F, (False, "failure"))
        #  - restrict to user
        self.assertCheckTxn(txn, S * AU, (False, "Value 'none' not allowed for property 'svn:author'"))

        # allowed user
        txn = FakeTransaction(properties = {"svn:author": "a"})
        #  - on good operation
        self.assertCheckTxn(txn, AU + S, (True, "Value 'a' allowed for property 'svn:author'"))
        #  - on bad operation
        self.assertCheckTxn(txn, AU + F, (True, "Value 'a' allowed for property 'svn:author'"))
        #  - restrict to user
        self.assertCheckTxn(txn, S * AU, (True, "Value 'a' allowed for property 'svn:author'"))

    def test_030_package_tag(self):
        """package tag validation"""
        checker = AllPaths(PackageTag(), ".*/tags/.*")
        for txn, result in self.package_tags:
            self.assertCheckTxn(txn, checker, result)

    def test_030_project_tag(self):
        """project tag validation"""
        checker = AllPaths(ProjectTag(), ".*/tags/.*")
        for txn, result in self.project_tags:
            self.assertCheckTxn(txn, checker, result)

    def test_030_tag_removal(self):
        """tag removal"""
        checker = AllPaths(TagRemoval(), ".*/tags/.*")
        for txn, result in self.tags_removal:
            self.assertCheckTxn(txn, checker, result)

    def test_030_tag_int_dirs(self):
        """tag intermediate dirs"""
        checker = AllPaths(TagIntermediateDirs(), ".*/tags/.*")
        for txn, result in self.tags_int_dirs:
            self.assertCheckTxn(txn, checker, result)

    def test_040_valid_xml(self):
        checker = AllPaths(ValidXml(), r".*\.xml$")
        for txn, result in self.xml_files:
            self.assertCheckTxn(txn, checker, result)

    def test_040_valid_py_encoding(self):
        checker = AllPaths(ValidPythonEncoding(), r".*\.py$")
        for txn, result in self.py_files:
            self.assertCheckTxn(txn, checker, result)

    def test_050_duplicated_packages(self):
        checker = LHCbCheckers.uniquePackages
        for txn, result in [(FakeTransaction({"/": ('M', (-1, None), 'file')},
                                             files = {"/": "dir"},
                                             node_properties = {"/" : {"packages": """
# a comment

Hat1/Pack1 Proj1
Hat2/Pack1 Proj2
"""}},), False),
(FakeTransaction({"/": ('M', (-1, None), 'file')},
                                             files = {"/": "dir"},
                                             node_properties = {"/" : {"packages": """
Hat1/Pack1 Proj1
Hat2/Pack2 Proj2
"""}},), True)]:
            self.assertCheckTxn(txn, checker, result)

    def test_060_move_package(self):
        checker = MovePackage()
        for txn, result in self.move_package:
            self.assertCheckTxn(txn, checker, result)

    def test_090_combined(self):
        """combined checks"""
        tests = ( self.package_tags
                + self.project_tags
                + self.tags_removal
                + self.tags_int_dirs
                + self.normal_commits
                + self.move_package
                )

        checker = LHCbCheckers.notHasTags + (MovePackage() + LHCbCheckers.validTag)
        for txn, result in tests:
            self.assertCheckTxn(txn, checker, result)

        checker = LHCbCheckers.librarian + checker
        for txn, result in tests:
            txn.properties["svn:author"] = "liblhcb"
            self.assertCheckTxn(txn, checker, True)



if __name__ == "__main__":
    if '--debug' in sys.argv:
        logging.basicConfig(level=logging.DEBUG)
        sys.argv.remove('--debug')
    #import sys;sys.argv = ['', 'Test.testName']
    unittest.main()
