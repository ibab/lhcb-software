#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import re
import atexit
import cPickle
from subprocess import Popen, PIPE
from collections import defaultdict

COVERAGE_FILE = '.cmake_coverage'

# coverage reports
coverage = defaultdict(set)

def update_coverage():
    #print 'Updating CMake coverage reports'
    if os.path.exists(COVERAGE_FILE):
        data = cPickle.load(open(COVERAGE_FILE))
        if 'lines' not in data:
            data['lines'] = {}
    else:
        data = {'lines': {}}
    lines = data['lines']
    for filename, linenumbers in coverage.iteritems():
        lines[filename] = sorted(linenumbers.union(lines.get(filename, [])))
    with open(COVERAGE_FILE, 'w') as report:
        cPickle.dump(data, report)
atexit.register(update_coverage)

def cmake_script(name, cwd=None):
    proc = Popen(['cmake', '--trace', '-P', name],
                 stdout=PIPE, stderr=PIPE, cwd=cwd)
    out, err = proc.communicate()
    trace_line = re.compile(r'^(/.*)\(([0-9]+)\): ')
    new_err = []
    for line in err.splitlines():
        match = trace_line.match(line)
        if match:
            coverage[match.group(1)].add(int(match.group(2)))
        else:
            new_err.append(line)
    return out, '\n'.join(new_err), proc.returncode
    #return out, err, proc.returncode

def get_ranges(numbers):
    '''
    >>> list(get_ranges([1,2,3,4,10,11,12,15]))
    [(1, 4), (10, 12), (15, 15)]
    '''
    from itertools import groupby
    from operator import itemgetter
    for _key, group in groupby(enumerate(numbers),
                               lambda (index, number): number - index):
        group = map(itemgetter(1), group)
        yield group[0], group[-1]

def get_active_lines(filename):
    for i, l in enumerate(open(filename)):
        l = l.strip()
        if l and not l.startswith('#') and not l.startswith('end'):
            yield i+1

if __name__ == '__main__':
    data = cPickle.load(open(COVERAGE_FILE))
    lines = data['lines']
    for filename in sorted(lines):
        if not os.path.exists(filename):
            print 'Unknown file', filename
            continue
        print filename
        active_lines = set(get_active_lines(filename))
        touched_lines = set(lines[filename])
        missed_lines = active_lines.difference(touched_lines)
        ranges = [str(a) if a == b else '%d-%d' % (a,b)
                  for a, b in get_ranges(sorted(missed_lines))]
        touched_count = len(touched_lines)
        active_count = len(active_lines)
        if touched_count == active_count:
            print '   coverage 100%'
        else:
            print ('   coverage %3d%%, missed: %s' %
                   (float(touched_count) / active_count * 100,
                    ', '.join(ranges)))
