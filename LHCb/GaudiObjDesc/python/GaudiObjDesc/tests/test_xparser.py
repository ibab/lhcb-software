"""
Test xml parsing.
"""
from ..godII import xparser # IGNORE:F0401

import os
import shutil

from tempfile import mkdtemp
from pprint import pprint

### Helpers

# the DTD file is ../../../xml_files/gdd.dtd
dtdPath = os.path.normpath(os.path.join(os.path.dirname(__file__),
                                        os.pardir, os.pardir, os.pardir,
                                        'xml_files', 'gdd.dtd'))

def buildDir(files, rootdir=os.curdir):
    '''
    Create a directory structure from the content of files.

    @param files: a dictionary or list of pairs mapping a filename to the content
                  if the content is a dictionary, recurse
    @param rootdir: base directory
    '''
    if type(files) is dict:
        files = files.items()

    # ensure that the root exists (to allow empty directories)
    if not os.path.exists(rootdir):
        os.makedirs(rootdir)

    # create all the entries
    for filename, data in files:
        filename = os.path.join(rootdir, filename)
        if type(data) is dict:
            buildDir(data, filename)
        else:
            d = os.path.dirname(filename)
            if not os.path.exists(d):
                os.makedirs(d)
            f = open(filename, "w")
            if data:
                f.write(data)
            f.close()

class TempDir(object):
    '''
    Class for easy creation, use and removal of temporary directory structures.
    '''
    def __init__(self, files=None):
        self.tmpdir = mkdtemp()
        if files is None:
            files = {}
        buildDir(files, self.tmpdir)

    def __del__(self):
        shutil.rmtree(self.tmpdir, ignore_errors=False)

    def __call__(self, f):
        '''
        Return the absolute path to a file in the temporary directory.
        '''
        return os.path.join(self.tmpdir, f)

### Tests

def test_dtd_file():
    assert os.path.exists(dtdPath)

def test_instantiate():
    parser = xparser.xparser(dtdPath)
    assert parser

# FIXME: The parsing of the database cannot work!!!
def _test_parsedb_1():
    tmp = TempDir({'test.xml':
'''<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE gdd SYSTEM "gdd.dtd" >
<gdd>
  <class name="Class1" file="some/file.h"/>
</gdd>
''',
                   })
    dbfile = tmp('test.xml')

    parser = xparser.xparser(dtdPath)

    try:
        data = parser.parseDB(dbfile)
    except SystemExit:
        assert False, 'sys.exit() called'

    assert data == {'Class1': 'some/file.h'}


def test_parse_minimal():
    tmp = TempDir({'test.xml':
'''<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE gdd SYSTEM "gdd.dtd" >
<gdd>
  <package name="SomePackage">
    <class name="Class1" author="Somebody" desc="Test class."></class>
  </package>
</gdd>
'''})

    dbfile = tmp('test.xml')

    parser = xparser.xparser(dtdPath)

    try:
        data = parser.parseSource(dbfile)
    except SystemExit:
        assert False, 'sys.exit() called'

    expected = {'attrs': {'version': '1.0'},
                'package': [{'attrs': {'name': 'SomePackage'},
                            'class': [{'attrs': {'allocator': 'DEFAULT',
                                                 'author': 'Somebody',
                                                 'contObjectTypeDef': 'FALSE',
                                                 'defaultconstructor': 'TRUE',
                                                 'defaultdestructor': 'TRUE',
                                                 'desc': 'Test class.',
                                                 'keyedContTypeDef': 'FALSE',
                                                 'name': 'Class1',
                                                 'serializers': 'TRUE',
                                                 'stdVectorTypeDef': 'FALSE',
                                                 'virtual': 'TRUE'}}]}]}

    pprint(data)
    assert data == expected

def test_parse_comment():
    tmp = TempDir({'test.xml':
'''<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE gdd SYSTEM "gdd.dtd" >
<gdd>
  <!-- this is a comment -->
  <package name="SomePackage">
    <class name="Class1" author="Somebody" desc="Test class."></class>
  </package>
</gdd>
'''})

    dbfile = tmp('test.xml')

    parser = xparser.xparser(dtdPath)

    try:
        data = parser.parseSource(dbfile)
    except SystemExit:
        assert False, 'sys.exit() called'

    expected = {'attrs': {'version': '1.0'},
                'package': [{'attrs': {'name': 'SomePackage'},
                            'class': [{'attrs': {'allocator': 'DEFAULT',
                                                 'author': 'Somebody',
                                                 'contObjectTypeDef': 'FALSE',
                                                 'defaultconstructor': 'TRUE',
                                                 'defaultdestructor': 'TRUE',
                                                 'desc': 'Test class.',
                                                 'keyedContTypeDef': 'FALSE',
                                                 'name': 'Class1',
                                                 'serializers': 'TRUE',
                                                 'stdVectorTypeDef': 'FALSE',
                                                 'virtual': 'TRUE'}}]}]}

    pprint(data)
    assert data == expected


def test_parse_many_classes():
    tmp = TempDir({'test.xml':
'''<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE gdd SYSTEM "gdd.dtd" >
<gdd>
  <package name="SomePackage">
    <class name="Class1" author="Somebody" desc="Test class."></class>
    <class name="Class2" author="Somebody" desc="Test class."></class>
  </package>
</gdd>
'''})

    dbfile = tmp('test.xml')

    parser = xparser.xparser(dtdPath)

    try:
        data = parser.parseSource(dbfile)
    except SystemExit:
        assert False, 'sys.exit() called'

    expected = {'attrs': {'version': '1.0'},
                'package': [{'attrs': {'name': 'SomePackage'},
                            'class': [{'attrs': {'allocator': 'DEFAULT',
                                                 'author': 'Somebody',
                                                 'contObjectTypeDef': 'FALSE',
                                                 'defaultconstructor': 'TRUE',
                                                 'defaultdestructor': 'TRUE',
                                                 'desc': 'Test class.',
                                                 'keyedContTypeDef': 'FALSE',
                                                 'name': 'Class1',
                                                 'serializers': 'TRUE',
                                                 'stdVectorTypeDef': 'FALSE',
                                                 'virtual': 'TRUE'}},
                                      {'attrs': {'allocator': 'DEFAULT',
                                                 'author': 'Somebody',
                                                 'contObjectTypeDef': 'FALSE',
                                                 'defaultconstructor': 'TRUE',
                                                 'defaultdestructor': 'TRUE',
                                                 'desc': 'Test class.',
                                                 'keyedContTypeDef': 'FALSE',
                                                 'name': 'Class2',
                                                 'serializers': 'TRUE',
                                                 'stdVectorTypeDef': 'FALSE',
                                                 'virtual': 'TRUE'}}]}]}

    print "Expected:"
    pprint(expected)
    print "\nData:"
    pprint(data)

    assert data == expected

def test_parse_method():
    tmp = TempDir({'test.xml':
'''<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE gdd SYSTEM "gdd.dtd" >
<gdd>
  <package name="SomePackage">
    <class name="Class1" author="Somebody" desc="Test class.">
      <method name="doSomething" desc="a method">
        <code>
        // here goes the code
        </code>
      </method>
    </class>
  </package>
</gdd>
'''})

    dbfile = tmp('test.xml')

    parser = xparser.xparser(dtdPath)

    try:
        data = parser.parseSource(dbfile)
    except SystemExit:
        assert False, 'sys.exit() called'

    expected = {'attrs': {'version': '1.0'},
                'package': [{'attrs': {'name': 'SomePackage'},
                            'class': [{'attrs': {'allocator': 'DEFAULT',
                                                 'author': 'Somebody',
                                                 'contObjectTypeDef': 'FALSE',
                                                 'defaultconstructor': 'TRUE',
                                                 'defaultdestructor': 'TRUE',
                                                 'desc': 'Test class.',
                                                 'keyedContTypeDef': 'FALSE',
                                                 'name': 'Class1',
                                                 'serializers': 'TRUE',
                                                 'stdVectorTypeDef': 'FALSE',
                                                 'virtual': 'TRUE'},
                                       'method': [{'attrs': {u'access': u'PUBLIC',
                                                             u'const': u'FALSE',
                                                             u'desc': u'a method',
                                                             u'friend': u'FALSE',
                                                             u'inline': u'FALSE',
                                                             u'name': u'doSomething',
                                                             u'static': u'FALSE',
                                                             u'type': u'void',
                                                             u'virtual': u'FALSE'},
                                                   'code': [{'attrs': {'xml:space': 'preserve'},
                                                             'cont': '\n        // here goes the code\n        '}]}]}]}]}

    print "Expected:"
    pprint(expected)
    print "\nData:"
    pprint(data)

    assert data == expected

