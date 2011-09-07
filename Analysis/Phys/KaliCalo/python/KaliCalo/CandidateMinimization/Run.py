#!/usr/bin/env python
# =============================================================================
# @file Run.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @date 2011-08-18
# =============================================================================
"""
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"

# General python imports
import os
import sys
import glob
from optparse import OptionParser
import tempfile
import subprocess
import shutil
import time

kaliDir = os.path.expandvars("$KALICALOROOT")
# ROOT
from ROOT import *
try:
  sc = gROOT.LoadMacro(os.path.join(kaliDir, "root", "CellCutter.cxx+"))
  from ROOT import CellCutter
except:
  print "Error importing CellCutter!"
  sys.exit(1)
# Kali
import KaliCalo.CandidateMinimization.Commands as Commands
import KaliCalo.Kali.LambdaMap as LambdaMap
import KaliCalo.FakeCells      as FakeCells
import KaliCalo.Cells          as Cells
from   KaliCalo.Det            import getCalo

def getEcalCells():
  ecal = getCalo()
  cp = ecal.cellParams()
  cells = []
  for p in cp :
    cell = p.cellID()
    if ecal.valid ( cell ) and not cell.isPin() :
      cells.append ( cell )
  cells.sort()
  return cells

gridCastorDir = 'castor:/castor/cern.ch/grid/'
homeCastorDir = os.path.expandvars("castor:$CASTOR_HOME")

CellID = FakeCells.CellID
betas = [8.3, 8.8, 9.5]

if __name__ == '__main__':
  parser = OptionParser()
  parser.add_option("-t", "--cellType",
                    dest="cellType", action="store", type="string", default="SameCell",
                    help="use CELLTYPE cell grouping", metavar="CELLTYPE")
  parser.add_option("-n", "--nSteps",
                    dest="nSteps", action="store", type="int", default=10,
                    help="number of calibration STEPS to perform", metavar="STEPS")
  (options, files) = parser.parse_args()
  if not files:
    options, files = parser.parse_args(sys.stdin.readline().split())
  # Load the cell massager
  CellMassager = getattr(FakeCells, options.cellType, None)
  if not CellMassager:
    print "Wrong Cell Massager!"
    sys.exit(1)
  CellMassager = CellMassager()
  # Determine cell groups only with top cells (the others are opposite and considered by Calibrate.py)
  cellsToCalibrate = {}
  for cell in getEcalCells():
    if cell.row() > 31:
      mCell = CellMassager(cell)
      if not mCell in cellsToCalibrate:
        cellsToCalibrate[mCell] = []
      cellsToCalibrate[mCell].append(cell)
  print "We have %s vars to calibrate" %(len(cellsToCalibrate)*2)
  #print "Cells to calibrate (true cellID) -> %s" % (', '.join([str(cellsToCalibrate[c][0].all()) for c in cellsToCalibrate]))
  #print "Cells to calibrate (fake cell)   -> %s" % (', '.join([str(c) for c in cellsToCalibrate]))
  #print "Cells to calibrate (file name)   -> %s" % (', '.join([str(c.all())+'.root' for c in cellsToCalibrate]))
  # Cut cells
  if not 'CaloCalib' in Commands.nsls(''):
    Commands.runCommand('nsmkdir', '$CASTOR_HOME/CaloCalib')
  for arg in files:
    # Prepare output dir in Castor
    calibName = os.path.splitext(os.path.basename(arg))[0]
    if calibName.startswith('tup'):
      calibName = calibName[3:]
    print "Calib name: %s" % calibName
    print "Creating dir structure"
    #print "Cutting %s" % calibName
    dirName = os.path.expandvars('$CASTOR_HOME/CaloCalib/%s' % calibName)
    if calibName in Commands.nsls('CaloCalib'):
      print "Calibration directory $CASTOR_HOME/CaloCalib/%s already exists." % calibName
    else:
      Commands.runCommand('nsmkdir', dirName)
    # Prepare dir structure
    outputDir = os.path.expandvars("$HOME/CaloCalib/%s/%s" % (options.cellType, calibName))
    # castorDir = 'castor:' + os.path.expandvars("$CASTOR_HOME/CaloCalib/%s" % calibName)
    castorDir = os.path.expandvars("$CASTOR_HOME/CaloCalib/%s" % calibName)
    if not os.path.exists(outputDir):
      os.makedirs(outputDir)
    masterDir = os.path.join(outputDir, 'master')
    if not os.path.exists(masterDir):
      os.makedirs(masterDir)
    for i in range(options.nSteps + 1):
      dirName = os.path.join(outputDir, str(i))
      if not os.path.exists(dirName):
        os.makedirs(dirName)
        for subDir in ['input', 'output', 'lambda']:
          subDirName = os.path.join(dirName, subDir)
          if not os.path.exists(subDirName):
            os.makedirs(subDirName)
    # Create inputs
    print "Now submit jobs"
    prevJob = 0
    fmt = "%30s %s\n"
    for i in range(options.nSteps):
      step = i+1
      if step == 1:
        print "First step, global correction"
        if not os.path.exists(os.path.join(outputDir, '0', 'lambda', 'lambdas.gz')):
          args = ['-t', 'area',
                  '-o', os.path.join(outputDir, '0', 'lambda'),
                  arg]
          jobOptions = ' '.join(args)
          with open(os.path.join(outputDir, '0', 'input', 'input.lambdas'), 'w') as f:
            f.write(jobOptions)
          script = """#!/bin/tcsh
kinit -R
aklog
SetupProject DaVinci v28r5
python %s < %s
exit $?
""" % ( os.path.expandvars("$KALICALOROOT/python/KaliCalo/CandidateMinimization/GetGlobalConstant.py"),
        os.path.join(outputDir, '0', 'input', 'input.lambdas') )
          scriptPath = os.path.join(outputDir, '0', 'input', 'cut.csh')
          with open(scriptPath, 'w') as f:
            f.write(script)
          os.chmod(scriptPath, int('777', 8))
          globalArgs = ['-Q', 'all ~0',
                        '-J', 'Calib%sGlobalConstant' % calibName,
                        #'-o/dev/null',
                        '-o', os.path.join(outputDir, '0', 'output', 'GlobalConstants'),
                        scriptPath]
          print globalArgs
          prevJob = Commands.bsub(*globalArgs)
          with open(os.path.join(masterDir, 'jobIDs'), 'a') as f:
            f.write(fmt % ('GlobalConstant', prevJob))
      if os.path.exists(os.path.join(outputDir, str(step), 'lambda', 'lambdas.gz')): # The step has finalized
        print "Step %s of %s is done, skipping..." %(step, calibName)
        continue
      print "Step %s" % step
      inputDir = os.path.join(outputDir, str(step), 'input')
      inputNumber = 0
      # Divide the cells to calibrate into 50 evenly distributed jobs, so we have ~510 jobs in the end
      for cellsToCalib in (cellsToCalibrate.keys()[i::50] for i in range(50)):
        args = ['-l%s' % os.path.join(outputDir, str(step-1), 'lambda', 'lambdas.gz'),
                '-c%s' % arg,
                '-t%s' % options.cellType,
                '-o%s' % os.path.join(outputDir, str(step), 'lambda'),
               ]
        args.extend([os.path.join(castorDir, str(mCellName.all())+'.root') for mCellName in cellsToCalib])
        jobOptions = ' '.join(args)
        with open(os.path.join(inputDir, 'input.%s' % inputNumber), 'w') as f:
          f.write(jobOptions)
        inputNumber += 1
      calibArgs = ['-Q', "all ~0",
                   '-J', 'Calib%sStep%s[1-%s]' % (calibName, step, len(cellsToCalib)),
                #   "-i%s" % os.path.join(inputDir, 'input.%I'),
                   "-oo%s" % os.path.join(outputDir, str(step), 'output', 'output.%I') ]
      if prevJob:
        calibArgs.extend(['-w', 'done(%s)' %prevJob])
      script = """#!/bin/tcsh
kinit -R
aklog
SetupProject DaVinci v28r5
echo "My index is "$LSB_JOBINDEX
python %s < %s
exit $?
""" % ( os.path.expandvars("$KALICALOROOT/python/KaliCalo/CandidateMinimization/Calibrate.py"),
        os.path.join(inputDir, 'input.$LSB_JOBINDEX' ))
      with open(os.path.join(inputDir, 'calibrate.csh' ), 'w') as f:
        f.write(script)
      os.chmod(os.path.join(inputDir, 'calibrate.csh' ), int('777', 8))
      calibArgs.append(os.path.join(inputDir, 'calibrate.csh' ))
      print calibArgs
      prevJob = Commands.bsub(*calibArgs)
      if 0 == prevJob:
        print "Error submitting"
        sys.exit(1)
      with open(os.path.join(masterDir, 'jobIDs'), 'a') as f:
        f.write(fmt % ('Step%s' % step, prevJob))
      with open(os.path.join(inputDir, 'input.lambdas'), 'w') as f:
        f.write(' '.join(['-o%s' % os.path.join(outputDir, str(step), 'lambda', 'lambdas.gz'), os.path.join(outputDir, str(step), 'lambda','*.gz')]))
      #time.sleep(10)
      joinArgs = ['-Q', "all ~0",
                  #"-i%s" % os.path.join(inputDir, 'input.lambdas'),
                  "-o%s" % os.path.join(outputDir, str(step), 'output', 'output.lambdas'), 
                  '-w', "done(%s)" % prevJob,
                 ]#os.path.expandvars("$KALICALOROOT/python/KaliCalo/CandidateMinimization/CombineLambdaMaps.py")]
      script = """#!/bin/tcsh
kinit -R
aklog
SetupProject DaVinci v28r5
python %s < %s
exit $?
""" % ( os.path.expandvars("$KALICALOROOT/python/KaliCalo/CandidateMinimization/CombineLambdaMaps.py"),
        os.path.join(inputDir, 'input.lambdas'))
      with open(os.path.join(inputDir, 'combine.csh'), 'w') as f:
        f.write(script)
      os.chmod(os.path.join(inputDir, 'combine.csh'), int('777', 8))
      joinArgs.append(os.path.join(inputDir, 'combine.csh'))
      print joinArgs
      prevJob = Commands.bsub(*joinArgs)
      if 0 == prevJob:
        print "Error submitting"
        sys.exit(1)
      with open(os.path.join(masterDir, 'jobIDs'), 'a') as f:
        f.write(fmt % ('Combine%s' % step, prevJob))
  sys.exit(0)

# EOF

