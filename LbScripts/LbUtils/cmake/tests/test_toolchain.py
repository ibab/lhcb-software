# -*- coding: utf-8 -*-

import os
from os.path import join, dirname

from cmake_test_utils import CMakeTestScripts

class Tests(CMakeTestScripts):
    base_dir = dirname(__file__)
    scripts_dir = join(base_dir, 'cmake_scripts')

    def setup(self):
        os.environ['CMAKE_PREFIX_PATH'] = join(self.base_dir, 'data', 'toolchain')
        os.environ['CMTCONFIG'] = 'x86_64-slc6-gcc48-opt'

    tests = ['toolchain_extensions_min',
             'toolchain_extensions',
             'toolchain_extensions_multi']
