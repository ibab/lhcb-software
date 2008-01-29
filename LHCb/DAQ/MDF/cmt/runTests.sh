#!/bin/bash
export EXE=`cmt show macro_value GAUDI_install_scripts`/../$CMTCONFIG/bin/Gaudi.exe
cat << /EOF >/tmp/runtest.py
import os, sys, string

def nextTest(msg):
  print msg
  val = ''
  try:
    val=str(input('Continue [Y=CR/N]'))
  except NameError, X:
    val = 'N'
  except SyntaxError, X:
    return 1
  except Exception, X:
    print X.__class__, str(X)
    val = 'N'
  if len(val) > 0:
    sys.exit(0)
    return 0
  return 1

def execute(cmd):
  print '-->',cmd
  os.system("$EXE "+cmd)

tests = []
tests.append("../options/CreateMDF.opts")
tests.append("../options/ReadMDF.opts")
tests.append("../options/CreateMIF.opts")
tests.append("../options/ReadMIF.opts")
tests.append("../options/CreatePOOL.opts")
tests.append("../options/ReadPOOL.opts")
tests.append("../options/CreateTAE.opts")
tests.append("../options/ReadTAE.opts")
tests.append("../options/ReadTAEPOOL.opts")
for i in tests:
  if nextTest(i)>0:
    execute(i)
/EOF
python /tmp/runtest.py
