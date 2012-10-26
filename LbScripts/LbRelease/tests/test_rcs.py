#!/usr/bin/env python2
'''
Tests for the module LbRelease rcs.
'''
import os
import sys
import logging
from tempfile import mkdtemp
from shutil import rmtree
from subprocess import call
from os.path import join, exists

from LbRelease.rcs import connect

root_dir = os.path.dirname(__file__)

svn_repo = None
repo_url = None

# Uncomment the following line to create a dump of the initial repository
#dump_file = join(root_dir, 'data', 'rcs.dump')

# Uncomment the following line to create a dump of the final repository
#dump_file_final = join(root_dir, 'data', 'rcs_final.dump')

logging.basicConfig(level=logging.DEBUG)

def setup():
    '''
    Prepare the fake repository for testing.
    '''
    global svn_repo, repo_url
    if svn_repo is None:
        svn_repo = mkdtemp("_rcstest")
    repo_url = 'file://%s' % svn_repo

    print 'Creating repository at', svn_repo

    call(['svnadmin', 'create', svn_repo])

    call(['svn', 'import', '-m', 'initial',
          join(root_dir, 'data', 'rcs', os.path.curdir),
          repo_url])

    checkout = mkdtemp()
    call(['svn', 'checkout', '-N', repo_url, checkout])
    for p in ['packages', 'projects', 'version']:
        call(['svn', 'propset', '-F', join(root_dir, 'data', 'rcs.' + p), p, checkout])
    call(['svn', 'commit', '-m', 'properties', checkout])
    print 'Removing', checkout
    rmtree(checkout, ignore_errors=True)

    if 'dump_file' in globals():
        print 'Dumping the repository in', dump_file
        call(['svnadmin', 'dump', svn_repo], stdout=open(dump_file, 'w'))


def teardown():
    '''
    Clean up after the tests.
    '''
    global svn_repo
    if svn_repo:
        if 'dump_file_final' in globals():
            print 'Dumping the repository in', dump_file_final
            call(['svnadmin', 'dump', svn_repo], stdout=open(dump_file_final, 'w'))
        print 'Removing', svn_repo
        rmtree(svn_repo, ignore_errors=True)
        svn_repo = None

################################################################################
# tests
################################################################################

def test_properties():
    rep = connect(repo_url)
    assert rep.repositoryVersion == ('2', '0'), rep.repositoryVersion
    assert rep.packages == ['ProjPkgA', 'ProjPkgB', 'ProjSys', 'SomePkg'], rep.packages
    assert sorted(rep.projects) == ['BestProj', 'Proj', 'ProjB', 'obs', 'pkgs'], sorted(rep.projects)

def test_tag_package():
    rep = connect(repo_url)

    retcode = rep.tag('ProjPkgA', 'v1r0')
    assert retcode == 0, 'Tagging ProjPkgA v1r0 -> %d' % retcode
    rep = connect(repo_url) # refresh infos
    assert rep._exists('Proj/tags/ProjPkgA/v1r0/cmt/requirements')

    retcode = rep.branch('ProjPkgA', 'v1b')
    assert retcode == 0, 'Branching ProjPkgA v1b -> %d' % retcode
    rep = connect(repo_url) # refresh infos
    assert rep._exists('Proj/branches/ProjPkgA/v1b/cmt/requirements')

    retcode = rep.branch('ProjPkgA', 'v1r0b', from_tag='v1r0')
    assert retcode == 0, 'Branching ProjPkgA v1r0b (from v1r0) -> %d' % retcode
    rep = connect(repo_url) # refresh infos
    assert rep._exists('Proj/branches/ProjPkgA/v1r0b/cmt/requirements')

    retcode = rep.tag('ProjPkgA', 'v1r1', from_branch='v1b')
    assert retcode == 0, 'Tagging ProjPkgA v1r1 (from v1b) -> %d' % retcode
    rep = connect(repo_url) # refresh infos
    assert rep._exists('Proj/tags/ProjPkgA/v1r1/cmt/requirements')

def test_checkout_package():
    rep = connect(repo_url)
    olddir = os.getcwd()
    workdir = mkdtemp()
    def check(d):
        return exists(join(workdir, d))
    try:
        # plain
        os.chdir(workdir)
        rep.checkout('ProjPkgA')
        os.chdir(olddir)

        assert all(map(check,['ProjPkgA/cmt/requirements', 'ProjPkgA/cmt/version.cmt']))
        assert open(join(workdir, 'ProjPkgA/cmt/version.cmt')).read().strip() == 'v1r0'

        rmtree(join(workdir, 'ProjPkgA'), ignore_errors=True)

        # with dest
        rep.checkout('ProjPkgA', dest=workdir)

        assert all(map(check,['ProjPkgA/cmt/requirements', 'ProjPkgA/cmt/version.cmt']))
        assert open(join(workdir, 'ProjPkgA/cmt/version.cmt')).read().strip() == 'v1r0'

        # trunk
        rep.checkout('ProjPkgA', 'trunk', dest=workdir)

        assert all(map(check,['ProjPkgA/cmt/requirements', 'ProjPkgA/cmt/version.cmt']))
        assert open(join(workdir, 'ProjPkgA/cmt/version.cmt')).read().strip() == 'v1r0'

        # tag
        rep.checkout('ProjPkgA', 'v1r0', dest=workdir)

        assert all(map(check,['ProjPkgA/cmt/requirements', 'ProjPkgA/cmt/version.cmt']))
        assert open(join(workdir, 'ProjPkgA/cmt/version.cmt')).read().strip() == 'v1r0'

        # branch
        rep = connect(repo_url) # refresh infos
        rep.useBranches = True # allow for branches lookup
        rep.checkout('ProjPkgA', 'v1r0b', dest=workdir)

        assert all(map(check,['ProjPkgA/cmt/requirements', 'ProjPkgA/cmt/version.cmt']))
        assert open(join(workdir, 'ProjPkgA/cmt/version.cmt')).read().strip() == 'v1r0'

        # tag
        rep.checkout('ProjPkgA', 'v1r1', dest=workdir)

        assert all(map(check,['ProjPkgA/cmt/requirements', 'ProjPkgA/cmt/version.cmt']))
        assert open(join(workdir, 'ProjPkgA/cmt/version.cmt')).read().strip() == 'v1r0'

        # tag with version directory
        rep.checkout('ProjPkgA', 'v1r1', dest=workdir, vers_dir=True)

        assert all(map(check,['ProjPkgA/v1r1/cmt/requirements']))
        assert not exists(join(workdir, 'ProjPkgA/v1r1/cmt/version.cmt'))


    finally:
        os.chdir(olddir)
        rmtree(workdir, ignore_errors=True)


def test_tag_project():
    rep = connect(repo_url)

    # simple project
    assert rep.tag('Proj', 'v1r0', isProject=True) == 0
    rep = connect(repo_url) # refresh infos
    assert rep._exists('Proj/tags/PROJ/PROJ_v1r0/cmt/project.cmt')

    # simple project (branch)
    assert rep.branch('Proj', 'v1b', isProject=True) == 0
    rep = connect(repo_url) # refresh infos
    assert rep._exists('Proj/branches/PROJ/PROJ_v1b/cmt/project.cmt')

    # files expected in the CMake project tag/branch
    expected_files = ['cmt/project.cmt', 'CMakeLists.txt',
                      'cmake/SomeCMakeModule.cmake', 'toolchain.cmake',
                      'configure']

    # CMake project
    assert rep.tag('ProjB', 'v1r0', isProject=True) == 0
    rep = connect(repo_url) # refresh infos
    exists = lambda x: rep._exists('ProjB/tags/PROJB/PROJB_v1r0/' + x)
    assert all(map(exists, expected_files)), rep._ls('ProjB/tags/PROJB/PROJB_v1r0/')

    # CMake project (branch)
    assert rep.branch('ProjB', 'v1b', isProject=True) == 0
    rep = connect(repo_url) # refresh infos
    exists = lambda x: rep._exists('ProjB/branches/PROJB/PROJB_v1b/' + x)
    assert all(map(exists, expected_files)), rep._ls('ProjB/branches/PROJB/PROJB_v1b/')


def test_checkout_project():
    rep = connect(repo_url)
    olddir = os.getcwd()
    workdir = mkdtemp()
    def check(d):
        return exists(join(workdir, d))
    try:
        # plain
        os.chdir(workdir)
        rep.checkout('Proj', project=True, vers_dir=True)
        os.chdir(olddir)

        assert all(map(check,['PROJ/PROJ_head/cmt/project.cmt']))

        rmtree(join(workdir, 'PROJ'), ignore_errors=True)

        # with dest
        rep.checkout('Proj', project=True, dest=workdir, vers_dir=True)

        assert all(map(check,['PROJ/PROJ_head/cmt/project.cmt']))

        # trunk
        rep.checkout('Proj', 'trunk', project=True, dest=workdir, vers_dir=True)

        assert all(map(check,['PROJ/PROJ_trunk/cmt/project.cmt']))

        # tag
        rep.checkout('Proj', 'v1r0', project=True, dest=workdir, vers_dir=True)

        assert all(map(check,['PROJ/PROJ_v1r0/cmt/project.cmt']))


    finally:
        os.chdir(olddir)
        rmtree(workdir, ignore_errors=True)

def test_checkout_cmake_project():
    rep = connect(repo_url)
    olddir = os.getcwd()
    workdir = mkdtemp()
    class check(object):
        def __init__(self, prefix):
            self.prefix = prefix
        def __call__(self, d):
            return exists(join(workdir, self.prefix, d))

    expected_files = ['cmt/project.cmt', 'CMakeLists.txt',
                      'cmake/SomeCMakeModule.cmake', 'toolchain.cmake',
                      'configure']

    try:
        # plain
        os.chdir(workdir)
        rep.checkout('ProjB', project=True, vers_dir=True)
        os.chdir(olddir)

        assert all(map(check('PROJB/PROJB_head'), expected_files))

        rmtree(join(workdir, 'PROJB'), ignore_errors=True)

        # with dest
        rep.checkout('ProjB', project=True, dest=workdir, vers_dir=True)

        assert all(map(check('PROJB/PROJB_head'), expected_files))

        # trunk
        rep.checkout('ProjB', 'trunk', project=True, dest=workdir, vers_dir=True)

        assert all(map(check('PROJB/PROJB_trunk'), expected_files))

        # tag
        rep.checkout('ProjB', 'v1r0', project=True, dest=workdir, vers_dir=True)

        assert all(map(check('PROJB/PROJB_v1r0'), expected_files))


    finally:
        os.chdir(olddir)
        rmtree(workdir, ignore_errors=True)

if __name__ == '__main__':
    from nose.core import main
    main(defaultTest=os.path.dirname(__file__),
         argv=['nosetests', '-v', '--with-coverage'])
