# -*- coding: utf-8 -*-

import os
from os.path import join, dirname

from cmake_test_utils import CMakeTestScripts

class Tests(CMakeTestScripts):
    base_dir = dirname(__file__)
    scripts_dir = join(base_dir, 'cmake_scripts')

    def setup(self):
        os.environ['CMAKE_PREFIX_PATH'] = join(self.base_dir, 'data', 'heptools')
        os.environ['CMTCONFIG'] = 'x86_64-slc6-gcc48-opt'
        if 'CMTPROJECTPATH' in os.environ:
            del os.environ['CMTPROJECTPATH']

    tests = ['heptools_parsing']
