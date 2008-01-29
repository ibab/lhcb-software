#!/bin/bash
. setup.sh
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
tests.append("$MDFROOT/options/CreateMDF.opts")
tests.append("$MDFROOT/options/ReadMDF.opts")
tests.append("$MDFROOT/options/CreateMIF.opts")
tests.append("$MDFROOT/options/ReadMIF.opts")
tests.append("$MDFROOT/options/CreatePOOL.opts")
tests.append("$MDFROOT/options/ReadPOOL.opts")
tests.append("$MDFROOT/options/CreateTAE.opts")
tests.append("$MDFROOT/options/ReadTAE.opts")
tests.append("$MDFROOT/options/ReadTAEPOOL.opts")
for i in tests:
  if nextTest(i)>0:
    execute(i)
/EOF
python /tmp/runtest.py
