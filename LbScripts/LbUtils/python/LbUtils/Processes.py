from subprocess import Popen, PIPE

import logging
import os, sys
import signal
import commands # subprocess module depends on Python version

class FormatedPs(object):
    __codes = ['C', 'G', 'P', 'U', 'a', 'c', 'g', 'n', 'p', 'r', 't', 'u', 'x', 'y', 'z']
    def _test_format(self, format):
        result = [ p for p in format if p not in self.__codes ]
        if 'a' in format and (format[-1] != 'a' or format.count('a') != 1): result.append('a')
        return result
    def __init__(self, format):
        """ FormatedPs(listOfParameters) -> object

        Example: FormatedPs(['p', 'P', 'a'])

        Available parameters:
           CODE   NORMAL   HEADER
        ---------------------------
            C     pcpu     %CPU
            G     group    GROUP
            P     ppid     PPID
            U     user     USER
            a     args     COMMAND  (only as the last argument)
            c     comm     COMMAND
            g     rgroup   RGROUP
            n     nice     NI
            p     pid      PID
            r     pgid     PGID
            t     etime    ELAPSED
            u     ruser    RUSER
            x     time     TIME
            y     tty      TTY
            z     vsz      VSZ
        For more details check: man ps.
        """
        self.invalidFormat = False
        if not format or self._test_format(format):
            print "Invalid format."
            self.invalidFormat = True
            return
        format = [ '%'+p for p in format ]
        _processes = commands.getoutput('ps -eo "' + ' '.join(format) + '"').split('\n')
        self.fields = _processes[0].split()
        self.fieldsCount = len(format)
        self.processes = [ [x.strip() for x in row.split(None, self.fieldsCount - 1)] for row in _processes[1:] if len(row) > 0 ]
    def values(self):
        """ returns dictionary with the following keys:
            - fields: list of field names
            - processes: list of processes where each process is a list of ps command's fields as strings
        """
        return {'fields': self.fields, 'processes': self.processes }
    def __repr__(self, maxLines=None): #
        value = ""
        if self.invalidFormat: return value
        if maxLines is None: maxLines = len(self.processes)
        fieldsWidths = [len(x) for x in self.fields]
        for x in range(min(maxLines, len(self.processes))):
            fieldsWidths = [max(len(self.processes[x][q]), fieldsWidths[q]) for q in range(len(self.processes[x]))]
        for y in range(len(self.fields)):
            value += "%s" % self.fields[y] + " "*(fieldsWidths[y]-len(self.fields[y])+1)
        value += "\n"
        for x in range(min(maxLines, len(self.processes))):
            for y in range(len(self.processes[x])):
                value += "%s" % self.processes[x][y] + " "*(fieldsWidths[y]-len(self.processes[x][y])+1)
            value += "\n"
        return value[:-2] # without last "\n"

def __searchForChildren(pid, processes=None):
    if processes == None: processes = map(lambda x: [int(x[0]), int(x[1]), x[2]], FormatedPs(['p', 'P', 'a']).processes)
    ch = [ x[0] for x in processes if x[1] == pid ]
    if len(ch) == 0: return {} # if process has no children
    else:
        d = {}
        for x in ch: d[x] = __searchForChildren(x, processes)
        return d

def printTree(topParentId=1, maxWidth=None):
    """ printTree([topParentId[, maxWidth]])

    Function prints a process tree with a topParentId as a root process. Output width can be limited by maxWidth parameter.
    """
    def __printTree(pstree, level=0, processNames={}, maxWidth=None):
        for x in pstree:
            if level: line = '    '*level + ' \_ '
            else: line = '    '
            line += '%s' % x
            if processNames and x in processNames: line += ' %s' % processNames[x]
            if not maxWidth: print line
            else: print line[:maxWidth]
            __printTree(pstree[x], level+1, processNames, maxWidth)
    processes = map(lambda x: [int(x[0]), int(x[1]), x[2]], FormatedPs(['p', 'P', 'a']).processes)
    processNames = {}
    for x in processes: processNames[x[0]] = x[2]
    topParentTree = __searchForChildren(topParentId)
    pstree = {topParentId: topParentTree }
    if topParentTree: __printTree(pstree, 0, processNames, maxWidth)

def killTree(pid):
    """ killTree(pid)

    Function sends SIGSTOP signal to process pid and all its children (up-down), then SIGKILL signal is sent to all processes in down-up direction.
    """
    def killBranch(tree):
        for x in tree:
            try:
                os.kill(x, signal.SIGSTOP)
                killBranch(tree[x])
                os.kill(x, signal.SIGKILL)
            except OSError: pass
        return
    pstree = {pid: __searchForChildren(pid)}
    if not pstree :
        print "%d: No such process" % pid
        return
    killBranch(pstree)


_call_command_log = logging.getLogger("_call_command")
def callCommand(cmd, *args, **kwargs):
    """
    Simple wrapper to execute a command and return standard output, standard error and return code.
    """
    use_shell = sys.platform.startswith("win")
    d = {"stdout": PIPE, "stderr": PIPE, "shell": use_shell}
    d.update(kwargs)
    cmd = [cmd] + list(args)
    _call_command_log.debug("Execute command: %r %r", " ".join(cmd), kwargs)
    proc = apply(Popen, (cmd,), d)
    out, err = proc.communicate()
    return (out, err, proc.returncode)

class RetryCommand(object):
    """Small wrapper to add a 're-try' feature to _call_command."""
    def __init__(self, command, check, retries = 3, sleeptime = 30):
        self._command = command
        self._check = check
        self._retries = retries
        self._sleeptime = sleeptime
    def __call__(self, *args, **kwargs):
        from time import sleep
        retries = self._retries
        retval = apply(self._command, args, kwargs)
        while (not self._check(retval)) and (retries > 0):
            retval = apply(self._command, args, kwargs)
            retries -= 1
            sleep(self._sleeptime)
        return retval
