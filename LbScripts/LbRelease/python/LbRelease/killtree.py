#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys, re
import signal
try:
    import subprocess
    def getcmdoutput(cmd, params):
        return subprocess.Popen([cmd, params], stdout=subprocess.PIPE).communicate()[0]
except ImportError:
    print "subprocess: no module."
    raise SystemExit
    #import commands
    #def getcmdoutput(cmd, params):
    #    commands.getoutput('%s %s' % (cmd, params)).split('\n')

#   CODE   NORMAL   HEADER
#---------------------------
#   %C     pcpu     %CPU
#   %G     group    GROUP
#   %P     ppid     PPID
#   %U     user     USER
#   %a     args     COMMAND  (only as the last argument)
#   %c     comm     COMMAND
#   %g     rgroup   RGROUP
#   %n     nice     NI
#   %p     pid      PID
#   %r     pgid     PGID
#   %t     etime    ELAPSED
#   %u     ruser    RUSER
#   %x     time     TIME
#   %y     tty      TTY
#   %z     vsz      VSZ

import string
# map(lambda x: x.strip, ...)

class FormatedPs(object):
    __codes = ['C', 'G', 'P', 'U', 'a', 'c', 'g', 'n', 'p', 'r', 't', 'u', 'x', 'y', 'z']
    def _test_format(self, format):
        result = [ p for p in format if p not in self.__codes ]
        if 'a' in format and format[-1] != 'a': result.append('a')
        return result
    def __init__(self, format):
        if not format or self._test_format(format):
            print "Invalid format."
            return
        format = [ '%'+p for p in format ]
        _processes = getcmdoutput('ps', '-eo ' + ' '.join(format) + '').split('\n')
        self.fields = _processes[0].split()
        self.fieldsCount = len(format)
        self.processes = [ map(string.strip, row.split(None, self.fieldsCount - 1)) for row in _processes[1:] if len(row) > 0 ]
    def Print(self, no):
        for x in range(min(no, len(self.processes))):
            print self.processes[x]

def searchForChildren(pid, processes=None):
    if processes == None: processes = map(lambda x: [int(x[0]), int(x[1]), x[2]], FormatedPs(['p', 'P', 'a']).processes)
    ch = [ x[0] for x in processes if x[1] == pid ]
    if len(ch) == 0: return {} # if process has no children
    else:
        d = {}
        for x in ch: d[x] = searchForChildren(x, processes)
        return d

def printTree(pstree, level, processNames={}, maxWidth=None):
    for x in pstree:
        if level: line = '    '*level + ' \_ '
        else: line = '    '
        line += '%s' % x # ├─
        if processNames and x in processNames: line += ' --> %s' % processNames[x]
        if not maxWidth: print line
        else: print line[:maxWidth]
        printTree(pstree[x], level+1, processNames, maxWidth)


def killTree(pid):
    def killBranch(tree):
        for x in tree:
            try:
                os.kill(x, signal.SIGSTOP)
                killBranch(tree[x])
                os.kill(x, signal.SIGKILL)
            except OSError: pass
        return
    pstree = {pid: searchForChildren(pid)}
    if not pstree : 
        print "%d: No such process" % pid
        return
    killBranch(pstree)

def killNightlies(day, slot=None):
    if slot is None: slot = '.*'
    processes = map(lambda x: [int(x[0]), int(x[1]), x[2]], FormatedPs(['p', 'P', 'a']).processes)
    ngt_pids = [x[1] for x in processes if re.match('tee.*/afs/cern.ch/lhcb/software/nightlies/www/logs/'+slot+'\.'+day+'_mainlog_.*\.txt', x[2]) is not None]
    for x in ngt_pids:
        killTree(x)


if len(sys.argv) > 2 and sys.argv[1] == 'kill':         # killtree.py kill 1234
    for x in sys.argv[2:]:
        killTree(int(x))
    raise SystemExit
elif len(sys.argv) == 3 and sys.argv[1] == 'nightlies': # killtree.py nightlies DAY
    killNightlies(sys.argv[2])
    raise SystemExit
elif len(sys.argv) == 4 and sys.argv[1] == 'nightlies': # killtree.py nightlies DAY SLOT
    killNightlies(sys.argv[2], sys.argv[3])
    raise SystemExit



