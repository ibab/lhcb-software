#!/usr/bin/env python
'''
Simple script to extract metadata (dependencies, labels) from QMTest tests (.qmt
files) and suites (.qms files), and report them as declaration of CTest test
properties.
'''
__author__ = 'Marco Clemencic <marco.clemencic@cern.ch>'

import os
import xml.etree.ElementTree as ET
import collections
import re

def qmt_filename_to_name(path):
    '''
    convert the relative path to a .qmt/.qms file to the canonical QMTest test
    name.

    For example:

    >>> qmt_filename_to_name('some_suite.qms/sub.qms/mytest.qmt')
    'some_suite.sub.mytest'
    '''
    return '.'.join(re.sub(r'\.qm[st]$', '', p)
                    for p in path.split(os.path.sep))

def fix_test_name(name, pkg):
    '''
    Convert the QMTest test name to the name used in CTest.

    >>> fix_test_name('package.bug.123', 'Package')
    'Package.bug.123'

    >>> fix_test_name('simple', 'Package')
    'Package.simple'
    '''
    return re.sub(r'^(%s\.)?' % pkg.lower(), '%s.' % pkg, name)

def find_files(rootdir, ext):
    '''
    Find recursively all the files in a directory with a given extension.
    '''
    for dirpath, _dirnames, filenames in os.walk(rootdir):
        for filename in filenames:
            if os.path.splitext(filename)[1] == ext:
                yield os.path.join(dirpath, filename)


def analyze_deps(pkg, rootdir):
    '''
    Collect dependencies from the QMTest tests in a directory and report them
    to stdout as CMake commands.

    @param pkg: name of the package (used to fix the name of the tests to match
                the CMake ones
    @param rootdir: directory containing the QMTest tests (usually tests/qmtest)
    '''
    prereq_xpath = 'argument[@name="prerequisites"]/set/tuple/text'
    for path in find_files(rootdir, '.qmt'):
        name = qmt_filename_to_name(os.path.relpath(path, rootdir))
        name = fix_test_name(name, pkg)

        tree = ET.parse(path)
        prereqs = [fix_test_name(el.text, pkg)
                   for el in tree.findall(prereq_xpath)]
        if prereqs:
            print ('set_property(TEST {0} APPEND PROPERTY DEPENDS {1})'
                   .format(name, ' '.join(prereqs)))

def analyze_suites(pkg, rootdir):
    '''
    Find all the suites (.qms files) defined in a directory and use it as a
    label for the tests in it.
    '''
    labels = collections.defaultdict(list)

    tests_xpath = 'argument[@name="test_ids"]/set/text'
    suites_xpath = 'argument[@name="suite_ids"]/set/text'
    for path in find_files(rootdir, '.qms'):
        name = qmt_filename_to_name(os.path.relpath(path, rootdir))
        name = fix_test_name(name, pkg)

        tree = ET.parse(path)

        labels[name].extend(fix_test_name(el.text, pkg)
                            for el in tree.findall(tests_xpath))

        if tree.findall(suites_xpath):
            sys.stderr.write(('WARNING: %s: suites of suites are '
                              'not supported yet\n') % path)
            sys.stderr.flush()

    # transpose the dictionary of lists
    test_labels = collections.defaultdict(set)
    for label, tests in labels.iteritems():
        for test in tests:
            test_labels[test].add(label)

    for test, labels in test_labels.iteritems():
        print ('set_property(TEST {0} APPEND PROPERTY LABELS {1})'
               .format(test, ' '.join(labels)))

def analyze_disabling(pkg, rootdir):
    '''
    Set the label 'disabled' for tests that are not supported on a platform.
    '''
    platform_id = (os.environ.get('BINARY_TAG') or
                   os.environ.get('CMTCONFIG') or
                   platform.platform())

    unsupp_xpath = 'argument[@name="unsupported_platforms"]/set/text'
    for path in find_files(rootdir, '.qmt'):
        name = qmt_filename_to_name(os.path.relpath(path, rootdir))
        name = fix_test_name(name, pkg)

        tree = ET.parse(path)
        # If at least one regex matches the test is disabled.
        skip_test = [None
                     for el in tree.findall(unsupp_xpath)
                     if re.search(el.text, platform_id)]
        if skip_test:
            print ('set_property(TEST {0} APPEND PROPERTY LABELS disabled)'
                   .format(name))


if __name__ == '__main__':
    import sys
    analyze_deps(*sys.argv[1:])
    analyze_suites(*sys.argv[1:])
    analyze_disabling(*sys.argv[1:])
