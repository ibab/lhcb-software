#!/usr/bin/env python
# -*- coding: utf-8 -*-

from cmake_coverage import cmake_script
from os.path import join, curdir

class CMakeTestScripts(object):
    '''
    Helper class to run CMake test scripts.

    @param base_dir: where the tests should be run from
    @param scripts_dir: directory containing the tests scripts (test_<name>.cmake)
    @param tests: list of names of test scripts to run
    '''

    base_dir = curdir
    scripts_dir = curdir
    tests = []

    def run(self, name):
        script_name = join(self.scripts_dir, 'test_%s.cmake' % name)
        out, err, returncode = cmake_script(script_name, cwd=self.base_dir)
        print "---------- stdout ----------"
        print out
        print "---------- stderr ----------"
        print err
        assert returncode == 0

    def test_scripts(self):
        for test in self.tests:
            yield self.run, test
