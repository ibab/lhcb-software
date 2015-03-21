# -*- coding: utf-8 -*-

from cmake_coverage import cmake_script
from os.path import join, dirname

base_dir = dirname(__file__)
scripts_dir = join(base_dir, 'cmake_scripts')

def test_loops():
    script_name = join(scripts_dir, 'test_loops.cmake')
    out, err, returncode = cmake_script(script_name, cwd=base_dir)
    print "---------- stdout ----------"
    print out
    print "---------- stderr ----------"
    print err
    assert returncode != 0
    assert 'Infinite recursion detected at project Loop' in err

def test_missing_base():
    script_name = join(scripts_dir, 'test_missing_base.cmake')
    out, err, returncode = cmake_script(script_name, cwd=base_dir)
    print "---------- stdout ----------"
    print out
    print "---------- stderr ----------"
    print err
    assert returncode != 0
    assert 'Cannot find project IDoNotExist v0r0' in err
