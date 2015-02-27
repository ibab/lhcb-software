###############################################################################
# (c) Copyright 2013 CERN                                                     #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
__author__ = 'Marco Clemencic <marco.clemencic@cern.ch>'

import os
import sys
from tempfile import mkdtemp
import shutil

latest_gaudi = 'v25r1'

def prepare_tree(base, tree):
    from os import makedirs
    from os.path import dirname, join, exists
    if hasattr(tree, 'items'):
        tree = tree.items()
    for k, v in tree:
        k = join(base, k)
        if v:
            d = dirname(k)
            if not exists(d):
                makedirs(d)
            f = open(k, 'w')
            f.write(v)
            f.close()
        else:
            if not exists(k):
                makedirs(k)

class TempDir(object):
    def __init__(self, tree=None):
        self.tmpdir = None
        self.tree = tree
    def __enter__(self):
        self.tmpdir = mkdtemp()
        if self.tree:
            prepare_tree(self.tmpdir, self.tree)
        return self.tmpdir
    def __exit__(self, type_, value, traceback):
        if self.tmpdir:
            shutil.rmtree(self.tmpdir)


def test_import():
    import LbConfiguration.SP2
    assert LbConfiguration.SP2.path

def test_version():
    from LbConfiguration.SP2.version import expandVersionAlias, isValidVersion

    # this is a dummy test, waiting for a real implementation of version aliases
    assert expandVersionAlias('Gaudi', 'latest') == 'latest'

    assert isValidVersion('Gaudi', 'latest')
    assert isValidVersion('Gaudi', 'HEAD')
    assert isValidVersion('Gaudi', latest_gaudi)
    assert isValidVersion('LHCb', 'v32r5p1')
    assert isValidVersion('LHCb', 'v32r4g1')
    assert isValidVersion('LCGCMT', '64d')
    assert not isValidVersion('Gaudi', 'a random string')

def test_compatibility():
    from LbConfiguration.SP2.compatibility import getOldEnvironment
    env_bk = dict(os.environ)

    try:
        env = getOldEnvironment(['Gaudi', latest_gaudi])
        assert 'GAUDISYSROOT' in env
        assert latest_gaudi in env['GAUDISYSROOT']
    finally:
        os.environ.clear()
        os.environ.update(env_bk)

    try:
        env = getOldEnvironment(['NotAProject', 'v1r0'])
        assert False, 'exception expected'
    except SystemExit, x:
        assert x.code == 1
    finally:
        os.environ.clear()
        os.environ.update(env_bk)



def parse_args(func, args):
    from optparse import OptionParser
    return func(OptionParser(prog='dummy_program')).parse_args(args)

def test_options_addOutputLevel():
    from LbConfiguration.SP2.options import addOutputLevel
    import logging

    opts, _ = parse_args(addOutputLevel, [])
    assert opts.log_level == logging.WARNING

    opts, _ = parse_args(addOutputLevel, ['--debug'])
    assert opts.log_level == logging.DEBUG

    opts, _ = parse_args(addOutputLevel, ['--verbose'])
    assert opts.log_level == logging.INFO

    opts, _ = parse_args(addOutputLevel, ['--quiet'])
    assert opts.log_level == logging.WARNING


def test_options_addPlatform():
    from LbConfiguration.SP2.options import addPlatform

    opts, _ = parse_args(addPlatform, ['-c', 'platform1'])
    assert opts.platform == 'platform1'

    opts, _ = parse_args(addPlatform, ['--platform', 'platform2'])
    assert opts.platform == 'platform2'

    if 'CMTCONFIG' not in os.environ:
        os.environ['CMTCONFIG'] = 'default'
    if 'BINARY_TAG' in os.environ:
        del os.environ['BINARY_TAG']
    opts, _ = parse_args(addPlatform, [])
    assert opts.platform == os.environ['CMTCONFIG']

    os.environ['BINARY_TAG'] = 'another'
    opts, _ = parse_args(addPlatform, [])
    assert opts.platform == os.environ['BINARY_TAG']
    del os.environ['BINARY_TAG']

    del os.environ['CMTCONFIG']
    opts, _ = parse_args(addPlatform, [])
    assert opts.platform

    import LbConfiguration.Platform
    bk = LbConfiguration.Platform.NativeMachine
    class dummy():
        def CMTSupportedConfig(self):
            return None
    LbConfiguration.Platform.NativeMachine = dummy
    try:
        opts, _ = parse_args(addPlatform, [])
        assert False, 'exception expected'
    except SystemExit:
        pass
    LbConfiguration.Platform.NativeMachine = bk


def test_options_addSearchPath():
    from LbConfiguration.SP2.options import addSearchPath

    if 'LHCBDEV' not in os.environ:
        os.environ['LHCBDEV'] = '/afs/cern.ch/lhcb/software/DEV'
    opts, _ = parse_args(addSearchPath, ['--dev'])
    assert os.environ['LHCBDEV'] in map(str, opts.dev_dirs)

    del os.environ['LHCBDEV']
    try:
        opts, _ = parse_args(addSearchPath, ['--dev'])
        assert False, 'exception expected'
    except SystemExit:
        pass

    opts, _ = parse_args(addSearchPath, ['--dev-dir', '/some/path'])
    assert '/some/path' in map(str, opts.dev_dirs)

    if 'User_release_area' in os.environ:
        del os.environ['User_release_area']
    opts, _ = parse_args(addSearchPath, [])
    assert opts.user_area is None

    os.environ['User_release_area'] = '/home/myself/cmtuser'
    opts, _ = parse_args(addSearchPath, [])
    assert opts.user_area == os.environ['User_release_area']
    assert not opts.no_user_area

    opts, _ = parse_args(addSearchPath, ['--user-area', '/tmp/myself'])
    assert opts.user_area == '/tmp/myself'

    opts, _ = parse_args(addSearchPath, ['--no-user-area'])
    assert opts.no_user_area

    try:
        opts, _ = parse_args(addSearchPath, ['--nightly'])
        assert False, 'exception expected'
    except SystemExit:
        pass

    import datetime
    days = ('Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun')
    weekday = datetime.date.today().weekday()
    today = 'Today' # days[weekday]
    yesterday = days[(weekday - 1) % 7]

    with TempDir({'lhcb-nightly-slot/%s' % today: None,
                  'lhcb-nightly-slot/%s' % yesterday: None}) as tmp:
        os.environ['LHCBNIGHTLIES'] = tmp
        opts, _ = parse_args(addSearchPath, ['--nightly', 'lhcb-nightly-slot'])
        assert os.path.join(tmp, 'lhcb-nightly-slot', today) in map(str, opts.dev_dirs)
        assert opts.nightly == ('lhcb-nightly-slot', today)
        opts, _ = parse_args(addSearchPath, ['--nightly', 'lhcb-nightly-slot', yesterday])
        assert os.path.join(tmp, 'lhcb-nightly-slot', yesterday) in map(str, opts.dev_dirs)
        assert opts.nightly == ('lhcb-nightly-slot', yesterday)

        try:
            opts, _ = parse_args(addSearchPath, ['--nightly', 'dummy'])
            assert False, 'exception expected'
        except SystemExit:
            pass

        try:
            opts, _ = parse_args(addSearchPath, ['--nightly', 'lhcb-nightly-slot',
                                                 days[(weekday + 1) % 7]])
            assert False, 'exception expected'
        except SystemExit:
            pass

    with TempDir({'lhcb-nightly-slot/%s/confSummary.py' % today: '''cmtProjectPathList = ['/extra/dir']\n'''}) as tmp:
        os.environ['LHCBNIGHTLIES'] = tmp
        opts, _ = parse_args(addSearchPath, ['--nightly', 'lhcb-nightly-slot'])
        assert '/extra/dir' in map(str, opts.dev_dirs)

    with TempDir({'lhcb-nightly-slot/%s/configuration.xml' % today: '''<configuration><slot name="lhcb-nightly-slot">
<cmtprojectpath><path value="/extra/xml/dir"/></cmtprojectpath>
</slot></configuration>'''}) as tmp:
        os.environ['LHCBNIGHTLIES'] = tmp
        opts, _ = parse_args(addSearchPath, ['--nightly', 'lhcb-nightly-slot'])
        assert '/extra/xml/dir' in map(str, opts.dev_dirs)



def test_lookup():
    from LbConfiguration.SP2 import path
    bk_path = list(path)

    from LbConfiguration.SP2.lookup import (findProject, MissingProjectError,
                                            findDataPackage, MissingDataPackageError,
                                            getEnvXmlPath, MissingManifestError)

    if 'BINARY_TAG' in os.environ:
        del os.environ['BINARY_TAG']
    os.environ['CMTCONFIG'] = 'x86_64-slc6-gcc46-opt'

    data = {'MYPROJECT/MYPROJECT_v1r0/InstallArea/x86_64-slc6-gcc46-opt/manifest.xml':
                '<?xml version="1.0" encoding="UTF-8"?><manifest></manifest>',
            'DERIVEDPROJECT/DERIVEDPROJECT_v1r3/InstallArea/x86_64-slc6-gcc46-opt/manifest.xml':
                '<?xml version="1.0" encoding="UTF-8"?><manifest>'
                '<used_projects>'
                '<project name="MyProject" version="v1r0" />'
                '</used_projects>'
                '<used_data_pkgs>'
                '<package name="MyPackage" version="*"/>'
                '</used_data_pkgs>'
                '</manifest>',
            'OLDPROJECT/OLDPROJECT_v9r0/InstallArea/x86_64-slc6-gcc46-opt': None,
            'DBASE/MyPackage/v2r0': None,
            'DBASE/MyPackage/v2r1': None,
            'DBASE/MyPackage/v10r0': None,
            'User/MyProjectDev_v1r0/InstallArea/x86_64-slc6-gcc46-opt/manifest.xml':
                '<?xml version="1.0" encoding="UTF-8"?><manifest>'
                '<used_projects><project name="MyProject" version="v1r0" /></used_projects>'
                '</manifest>',
            'User/MyAnalysis/InstallArea/x86_64-slc6-gcc46-opt/manifest.xml':
                '<?xml version="1.0" encoding="UTF-8"?><manifest>'
                '<used_projects><project name="MyProject" version="v1r0" /></used_projects>'
                '</manifest>'}
    with TempDir(data) as tmp:
        path[:] = [tmp] + bk_path

        proj = findProject('MyProject', 'v1r0', os.environ['CMTCONFIG'])
        assert proj == os.path.join(tmp, 'MYPROJECT/MYPROJECT_v1r0/InstallArea/x86_64-slc6-gcc46-opt')

        try:
            findProject('AnotherProject', 'v1r0', os.environ['CMTCONFIG'])
            assert False, 'exception expected'
        except MissingProjectError, x:
            assert str(x)

        # test with user area
        path.insert(0, os.path.join(tmp, 'User'))
        proj = findProject('MyProjectDev', 'v1r0', os.environ['CMTCONFIG'])
        assert proj == os.path.join(tmp, 'User/MyProjectDev_v1r0/InstallArea/x86_64-slc6-gcc46-opt')

        proj = findProject('MyAnalysis', 'latest', os.environ['CMTCONFIG'])
        assert proj == os.path.join(tmp, 'User/MyAnalysis/InstallArea/x86_64-slc6-gcc46-opt')

        try:
            findProject('MyAnalysis', 'v1r0', os.environ['CMTCONFIG'])
            assert False, 'exception expected'
        except MissingProjectError, x:
            assert str(x)

        pkg = findDataPackage('MyPackage', 'v2r0')
        assert pkg == os.path.join(tmp, 'DBASE/MyPackage/v2r0')

        pkg = findDataPackage('MyPackage', 'v2r*')
        assert pkg == os.path.join(tmp, 'DBASE/MyPackage/v2r1')

        pkg = findDataPackage('MyPackage', 'v*')
        assert pkg == os.path.join(tmp, 'DBASE/MyPackage/v10r0')

        pkg = findDataPackage('MyPackage', '*')
        assert pkg == os.path.join(tmp, 'DBASE/MyPackage/v10r0')

        try:
            findDataPackage('NoPackage', '*')
            assert False, 'exception expected'
        except MissingDataPackageError, x:
            assert str(x)

        xml_path = getEnvXmlPath('DerivedProject', 'v1r3', os.environ['CMTCONFIG'])
        assert os.path.join(tmp, 'MYPROJECT/MYPROJECT_v1r0/InstallArea/x86_64-slc6-gcc46-opt') in xml_path
        assert os.path.join(tmp, 'DERIVEDPROJECT/DERIVEDPROJECT_v1r3/InstallArea/x86_64-slc6-gcc46-opt') in xml_path

        try:
            xml_path = getEnvXmlPath('OldProject', 'v9r0', os.environ['CMTCONFIG'])
            assert False, 'exception expected'
        except MissingManifestError, x:
            assert str(x)

    path[:] = bk_path

def test_listVersions():
    from LbConfiguration.SP2 import path
    bk_path = list(path)
    from os.path import join

    from LbConfiguration.SP2.lookup import listVersions

    if 'BINARY_TAG' in os.environ:
        del os.environ['BINARY_TAG']
    os.environ['CMTCONFIG'] = 'x86_64-slc6-gcc48-opt'

    data = {'MYPROJECT/MYPROJECT_v1r0/InstallArea/x86_64-slc6-gcc48-opt/manifest.xml':
                '<?xml version="1.0" encoding="UTF-8"?><manifest></manifest>',
            'MYPROJECT/MYPROJECT_v1r2/InstallArea/x86_64-slc6-gcc48-opt/manifest.xml':
                '<?xml version="1.0" encoding="UTF-8"?><manifest></manifest>'}
    with TempDir(data) as tmp:
        path[:] = ['/no/where', tmp, tmp]

        expected = [('v1r2', join(tmp, 'MYPROJECT', 'MYPROJECT_v1r2')),
                    ('v1r0', join(tmp, 'MYPROJECT', 'MYPROJECT_v1r0'))]
        observed = list(listVersions('MyProject', os.environ['CMTCONFIG']))
        print 'expected', expected
        print 'observed', observed
        assert expected == observed

    path[:] = bk_path


def _test_profiling():
    from LbConfiguration.SP2 import profiling

    from StringIO import StringIO

    try:
        sys.stderr = StringIO()
        profiling.run('sys.exit()')
        assert False, 'exception expected'
    except SystemExit:
        print sys.stderr.getvalue()
    finally:
        sys.stderr = sys.__stderr__

    def dummy():
        raise ImportError()

    import cProfile
    bk1 = cProfile.Profile
    cProfile.Profile = dummy

    try:
        sys.stderr = StringIO()
        profiling.run('sys.exit()')
        assert False, 'exception expected'
    except SystemExit:
        print sys.stderr.getvalue()
    finally:
        sys.stderr = sys.__stderr__

    import profile
    bk2 = profile.Profile
    profile.Profile = dummy
    try:
        sys.stderr = StringIO()
        profiling.run('sys.exit()')
        assert False, 'exception expected'
    except SystemExit:
        print sys.stderr.getvalue()
    finally:
        sys.stderr = sys.__stderr__

    cProfile.Profile = bk1
    profile.Profile = bk2

def test_LBCORE_522():
    '''LBCORE-522: lb-run does not expand some variables when falling back on SetupProject
    https://its.cern.ch/jira/browse/LBCORE-522'''
    from LbConfiguration.SP2.compatibility import expandAllVars

    original = {'MYDATA': '${DATA}/subdir', 'DATA': '/main/path'}
    expected = {'MYDATA': '/main/path/subdir', 'DATA': '/main/path'}
    assert expandAllVars(original) == expected

    try:
        expandAllVars({'A': '${B}/x', 'B': '${A}/y'})
        assert False, 'exception expected'
    except ValueError:
        pass
