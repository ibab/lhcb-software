"""
Test command line parsing.
"""
from ..godII import godII

import os

from nose import SkipTest

def setup():
    import GaudiObjDesc
    if 'GAUDIOBJDESCROOT' not in os.environ:
        os.environ['GAUDIOBJDESCROOT'] = os.path.normpath(os.path.join(GaudiObjDesc.__file__, os.pardir, os.pardir, os.pardir))

class CmdLineTestBase(object):
    prog = 'godII.py'
    args = []

    @classmethod
    def setup_all(cls):
        """
        Create the instance.
        """
        cls.g = cls.exit = None
        try:
            cls.g = godII([cls.prog] + cls.args)
        except SystemExit, x:
            cls.exit = x

class OptionsTestBase(CmdLineTestBase):
    prog = 'godII.py'
    args = []

    _defaults = {'fullCommand': lambda self, value: value == ' '.join([self.prog] + self.args),
                 'version': 'v8r0',
                 'xmlSources': [],
                 'xmlDBFile': lambda self, value: value == os.path.join(os.environ.get("GAUDIOBJDESCROOT"), 'xml_files', 'GODsClassDB.xml'),
                 'xmlDBFileExtra': [],
                 'srcOutput': os.curdir,
                 'dictOutput': os.curdir,
                 'argv0': lambda self, value: value == self.prog,
                 'godRoot': lambda self, value: os.path.normpath(value) == os.path.normpath(os.environ.get("GAUDIOBJDESCROOT")),
                 'gClasses': True,
                 'gClassDicts': True,
                 'gNamespaces': True,
                 'gAssocDicts': True,
                 'allocatorType': 'FROMXML',
                 'default_namespace': 'LHCb',
                 'dtdPath': lambda self, value: value == os.environ.get("GODDTDPATH")
                }

    expected = {}

    def get(self, key):
        if self.g is None:
            raise SkipTest()
        return getattr(self.g, key)

    def getExpected(self, key):
        if key in self.expected:
            return self.expected[key]
        else:
            return self._defaults[key]
    def check(self, key):
        value = self.get(key)
        expected = self.getExpected(key)
        if type(expected) is bool:
            assert bool(value) is expected
        elif hasattr(expected, '__call__'):
            assert expected(self, value), value
        else:
            assert value == expected, "found %r, expected %r" % (value, expected)

    def test_exit_code(self):
        assert self.exit is None, "execution interrupted (exit code %s)" % self.exit

    def test_options(self):
        keys = set(self.expected)
        keys.update(self._defaults)
        for k in keys:
            yield self.check, k

class ExitCodeTestBase(CmdLineTestBase):
    expected_code = 0
    def test_exit_code(self):
        assert self.exit is not None and self.exit.code == self.expected_code, "wrong exit code %s, expected %s" % (self.exit, self.expected_code)

class TestNoOpts(OptionsTestBase):
    pass

class Test_gen_src(OptionsTestBase):
    args = ['-g', 'src']
    expected = {'gClassDicts': False, 'gAssocDicts': False}

class Test_gen_dict(OptionsTestBase):
    args = ['-g', 'dct']
    expected = {'gClasses': False, 'gNamespaces': False}

class Test_gen_stuff(ExitCodeTestBase):
    args = ['-g', 'stuff']
    expected_code = 1

class Test_version(ExitCodeTestBase):
    args = ['-v']

class Test_help(ExitCodeTestBase):
    args = ['-h']

class TestBadOpt(ExitCodeTestBase):
    args = ['--bad-option']
    expected_code = 1

class Test_DBFileExtra(OptionsTestBase):
    args = ['-i', 'file1', '-i', 'file2']
    expected = {'xmlDBFileExtra': ['file1', 'file2']}

class Test_srcOutput_1(OptionsTestBase):
    args = ['-s', 'some_destination']
    expected = {'srcOutput': 'some_destination'}

class Test_srcOutput_env(OptionsTestBase):
    args = ['-s', 'env']
    expected = {'srcOutput': 'some_destination'}
    @classmethod
    def setup_all(cls):
        os.environ['GODDOTHOUT'] = 'some_destination'
        super(Test_srcOutput_env, cls).setup_all()

class Test_srcOutput_no_env(ExitCodeTestBase):
    args = ['-s', 'env']
    expected_code = 1
    @classmethod
    def setup_all(cls):
        try:
            del os.environ['GODDOTHOUT']
        except KeyError:
            pass
        super(Test_srcOutput_no_env, cls).setup_all()

class Test_dictOutput_1(OptionsTestBase):
    args = ['-d', 'some_destination']
    expected = {'dictOutput': 'some_destination'}

class Test_dictOutput_env(OptionsTestBase):
    args = ['-d', 'env']
    expected = {'dictOutput': 'some_destination'}

    @classmethod
    def setup_all(cls):
        os.environ['GODDICTOUT'] = 'some_destination'
        super(Test_dictOutput_env, cls).setup_all()

class Test_dictOutput_no_env(ExitCodeTestBase):
    args = ['-d', 'env']
    expected_code = 1
    @classmethod
    def setup_all(cls):
        try:
            del os.environ['GODDICTOUT']
        except KeyError:
            pass
        super(Test_dictOutput_no_env, cls).setup_all()

class Test_deprecated_srcOutput(OptionsTestBase):
    args = ['-g', 'src', '-o', 'some_destination']
    expected = {'gClassDicts': False, 'gAssocDicts': False,
                'srcOutput': 'some_destination'}

class Test_deprecated_dictOutput(OptionsTestBase):
    args = ['-g', 'dct', '-o', 'some_destination']
    expected = {'gClasses': False, 'gNamespaces': False,
                'dictOutput': 'some_destination'}

class Test_missing_GAUDIOBJDESCROOT(ExitCodeTestBase):
    expected_code = 1
    @classmethod
    def setup_all(cls):
        root = os.environ['GAUDIOBJDESCROOT']
        del os.environ['GAUDIOBJDESCROOT']

        try:
            super(Test_missing_GAUDIOBJDESCROOT, cls).setup_all()
        finally:
            os.environ['GAUDIOBJDESCROOT'] = root

class Test_godRoot(OptionsTestBase):
    args = ['-r', 'some/path/location']
    expected = {'godRoot': lambda self, value: os.path.normpath(value) == os.path.normpath('some/path/location'),
                'xmlDBFile': 'some/path/location/xml_files/GODsClassDB.xml'}

class Test_xmlDBFile(OptionsTestBase):
    args = ['-x', 'file_x.xml']
    expected = {'xmlDBFile': 'file_x.xml'}

class Test_xmlDBFile_env(OptionsTestBase):
    args = ['-x', 'env']
    expected = {'xmlDBFile': 'file_x.xml'}
    @classmethod
    def setup_all(cls):
        os.environ['GODXMLDB'] = 'file_x.xml'
        super(Test_xmlDBFile_env, cls).setup_all()

class Test_xmlDBFile_no_env(ExitCodeTestBase):
    args = ['-x', 'env']
    expected_code = 1
    @classmethod
    def setup_all(cls):
        try:
            del os.environ['GODXMLDB']
        except KeyError:
            pass
        super(Test_xmlDBFile_no_env, cls).setup_all()

class Test_allocator(OptionsTestBase):
    args = ['--allocator', 'BOOST']
    expected = {'allocatorType': 'BOOST'}

class Test_allocator_unknown(ExitCodeTestBase):
    args = ['--allocator', 'unknown']
    expected_code = 1

class Test_allocator_missing(ExitCodeTestBase):
    # FIXME: the code reached by this test is not needed
    args = ['--allocator', '']
    expected_code = 1

class Test_ignored_opt_l(OptionsTestBase):
    args = ['-l', 'value']

class Test_namespace(OptionsTestBase):
    args = ['-n', 'SomeExperiment']
    expected = {'default_namespace': 'SomeExperiment'}

class Test_namespace_missing(ExitCodeTestBase):
    # FIXME: the code reached by this test is not needed
    args = ['-n', '']
    expected_code = 1

class Test_dtdPath(OptionsTestBase):
    args = ['-t', 'path/to/gdd.dtd']
    expected = {'dtdPath': 'path/to/gdd.dtd'}

class Test_dtdPath_missing(ExitCodeTestBase):
    # FIXME: the code reached by this test is not needed
    args = ['-t', '']
    expected_code = 1
