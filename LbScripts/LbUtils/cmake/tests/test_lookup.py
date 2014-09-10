# -*- coding: utf-8 -*-

import os
from os.path import join, dirname

from cmake_coverage import cmake_script

base_dir = dirname(__file__)

def setup():
    # There is no 'projects_cmt' directory, but it helps testing the priorities
    # of CMAKE_PREFIX_PATH and CMTPROJECTPATH
    os.environ['CMTPROJECTPATH'] = join(base_dir, 'data', 'projects_cmt')
    os.environ['CMAKE_PREFIX_PATH'] = join(base_dir, 'data', 'projects')
    #os.environ['CMTPROJECTPATH'] = ''
    #os.environ['CMAKE_PREFIX_PATH'] = join(base_dir, 'data', 'projects')
    os.environ['CMTCONFIG'] = 'x86_64-slc6-gcc48-opt'

def cmake_script_runner(name):
    script_name = join(base_dir, 'cmake_scripts', 'test_%s.cmake' % name)
    out, err, returncode = cmake_script(script_name, cwd=base_dir)
    print "---------- stdout ----------"
    print out
    print "---------- stderr ----------"
    print err
    assert returncode == 0

def test_script():
    for test in ['no_use', 'simple_use', 'chain', 'diamond',
                 'with_tools', 'with_chained_tools',
                 'version_selection',
                 'atlas_convention', 'special_conventions',
                 'guess_toolchain']:
        yield cmake_script_runner, test
