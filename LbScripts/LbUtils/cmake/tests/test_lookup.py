# -*- coding: utf-8 -*-

import os
from os.path import join, dirname

from cmake_test_utils import CMakeTestScripts

class Tests(CMakeTestScripts):
    base_dir = dirname(__file__)
    scripts_dir = join(base_dir, 'cmake_scripts')

    def setup(self):
        # There is no 'projects_cmt' directory, but it helps testing the priorities
        # of CMAKE_PREFIX_PATH and CMTPROJECTPATH
        os.environ['CMTPROJECTPATH'] = join(self.base_dir, 'data', 'projects_cmt')
        os.environ['CMAKE_PREFIX_PATH'] = join(self.base_dir, 'data', 'projects')
        #os.environ['CMTPROJECTPATH'] = ''
        #os.environ['CMAKE_PREFIX_PATH'] = join(self.base_dir, 'data', 'projects')
        os.environ['CMTCONFIG'] = 'x86_64-slc6-gcc48-opt'

    tests = ['no_use', 'simple_use', 'chain', 'diamond',
             'with_tools', 'with_chained_tools',
             'version_selection',
             'atlas_convention', 'special_conventions',
             'guess_toolchain']
