#!/usr/bin/env python
# =============================================================================
# @file Commands.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @date 2011-08-24
# =============================================================================
"""Shortcuts for frequently used commands."""

__author__ = "Albert Puig (albert.puig@cern.ch)"

import subprocess
import os

def runCommand(cmd, *args):
  """Run given command with args on the command line.

  @param cmd: command to execute
  @type  cmd: string
  @param args: arguments of the command
  @type  args: list

  @return: list of lines of the output

  """
  return [line for line in subprocess.Popen([cmd]+list(args), stdout=subprocess.PIPE).communicate()[0].split('\n') if line]

def bsub(*args):
  args = ['-q8nh'] + list(args)
  out = runCommand('bsub', *args)
  if len(out) > 1:
    print out
    return 0
  out = out[0]
  return int(out.split()[1].strip('<').strip('>'))
    
def nsls(dirName):
  dirName = os.path.join(os.path.expandvars('$CASTOR_HOME'), dirName)
  return runCommand('nsls', dirName)
    
# EOF
