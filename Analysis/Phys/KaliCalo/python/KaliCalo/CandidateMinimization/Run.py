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
import KaliCalo.Kali.LambdaMap as LambdaMap
import KaliCalo.FakeCells      as FakeCells
import KaliCalo.Cells          as Cells
from KaliCalo.Det              import getCalo

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
  args = ['-q1nh'] + list(args)
  out = runCommand('bsub', *args)
  if len(out) > 1:
    print out
    return 0
  out = out[0]
  return int(out.split()[1].strip('<').strip('>'))

def chunks(l, n):
  return (l[i:i+n] for i in range(0, len(l), n))

gridCastorDir = 'castor:/castor/cern.ch/grid/'
homeCastorDir = os.path.expandvars("castor:$CASTOR_HOME")

CellID = FakeCells.CellID
betas = [8.3, 8.8, 9.5]

if __name__ == '__main__':
  parser = OptionParser()
  parser.add_option("-t", "--cellType",
                    dest="cellType", action="store", type="string", default="SameCell",
                    help="use CELLTYPE cell grouping", metavar="CELLTYPE")
  (options, args) = parser.parse_args()
  if not args:
    options, args = parser.parse_args(sys.stdin.readline().split())
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
  print "Cells to calibrate (true cellID) -> %s" % (', '.join([str(cellsToCalibrate[c][0].all()) for c in cellsToCalibrate]))
  print "Cells to calibrate (fake cell)   -> %s" % (', '.join([str(c) for c in cellsToCalibrate]))
  print "Cells to calibrate (file name)   -> %s" % (', '.join([str(c.all())+'.root' for c in cellsToCalibrate]))
  # Cut cells
  if not 'CaloCalib' in runCommand('nsls', os.path.expandvars('$CASTOR_HOME')):
    runCommand('nsmkdir', '$CASTOR_HOME/CaloCalib')
  for arg in args:
    # Prepare output dir in Castor
    calibName = os.path.splitext(os.path.basename(arg))[0].lstrip('tup')
    print "Cutting %s" % calibName
    dirName = os.path.expandvars('$CASTOR_HOME/CaloCalib/%s' % calibName)
    if calibName in runCommand('nsls', os.path.expandvars('$CASTOR_HOME/CaloCalib')):
      print "Calibration directory $CASTOR_HOME/CaloCalib/%s already exists." % calibName
    else:
      runCommand('nsmkdir', dirName)
    if 0 == len(runCommand('nsls', dirName)):
      execfile(arg)
      ntuples = [gridCastorDir+t for t in tup]
      if 0 == len(ntuples):
        print "No ntuples to run on -> %s" % calibName
        continue
      # Now create the dir
      tmpDir = tempfile.mkdtemp(calibName)
      print "Using tempDir ->", tmpDir
      try:
        cutter = CellCutter("",100)
        for ntuple in ntuples:
          cutter.addFile(ntuple)
        # Add cells and opposite cells
        filesToMove = set()
        for mCell, cellList in cellsToCalibrate.items():
          mCellName = str(mCell.all())
          for cell in cellList:
            oppositeCell = Cells.oppositeCell(cell)
            fName = os.path.join(tmpDir, mCellName+".root")
            cutter.addCell(cell.index(), fName)
            cutter.addCell(oppositeCell.index(), fName)
            filesToMove.add(fName)
        cutter.cut(20.0)
        delete = True
        for fileToMove in filesToMove:
          try:
            retcode = subprocess.call("rfcp %s %s" % (fileToMove, dirName), shell=True)
          except OSError, e:
            delete = False
            print "Error moving %s" %fileToMove
        if delete:
          shutil.rmtree(tmpDir)
      except Exception, e:
        print "Exception cutting! ->", e
        runCommand('nsrmdir', dirName)
    else:
      print "There are some files in the tuples directory, not cutting..."
    # Prepare dir structure
    outputDir = os.path.expandvars("$HOME/CaloCalib/%s/%s" % (options.cellType, calibName))
    castorDir = 'castor:' + os.path.expandvars("$CASTOR_HOME/CaloCalib/%s" % calibName)
    if not os.path.exists(outputDir):
      os.makedirs(outputDir)
    masterDir = os.path.join(outputDir, 'master')
    if not os.path.exists(masterDir):
      os.makedirs(masterDir)
    for i in range(11):
      dirName = os.path.join(outputDir, str(i))
      if not os.path.exists(dirName):
        os.makedirs(dirName)
        for subDir in ['input', 'output', 'lambda']:
          subDirName = os.path.join(dirName, subDir)
          if not os.path.exists(subDirName):
            os.makedirs(subDirName)
    # Create inputs
    prevJob = 0
    for i in range(10):
      step = i+1
      if os.path.exists(os.path.join(outputDir, str(step), 'lambda', 'lambdas.gz')): # The step has finalized
        print "Step %s of %s is done, skipping..." %(step, calibName)
        continue
      inputDir = os.path.join(outputDir, str(step), 'input')
      jobIDs = []
      for chunkNum, cellsToCalib in enumerate(chunks(cellsToCalibrate.keys(), 500)):
        inputNumber = 1
        for mCell in cellsToCalib:
          cellList = cellsToCalibrate[mCell]
          mCellName = str(mCell.all())
          cellToCalib = str(cellList[0].all())
          args = ['-l%s' % os.path.join(outputDir, str(step-1), 'lambda', 'lambdas.gz'),
                  '-c%s' % cellToCalib,
                  '-t%s' % options.cellType,
                  '-o%s' % os.path.join(outputDir, str(step), 'lambda'),
                  os.path.join(castorDir, mCellName+'.root')
                  ]
          jobOptions = ' '.join(args)
          with open(os.path.join(inputDir, 'input.%s.%s' % (chunkNum, inputNumber)), 'w') as f:
            f.write(jobOptions)
          inputNumber += 1
        calibArgs = ['-Q', "all ~0",
                     '-J', 'Calib%sStep%s[1-%s]' % (calibName, step, len(cellsToCalib)),
                  #   "-i%s" % os.path.join(inputDir, 'input.%I'),
                   #  "-oo%s" % os.path.join(outputDir, str(step), 'output', 'output.%s.%%I' % chunkNum) ]
                     "-o/dev/null" ] # output to /dev/null to avoid quota problems
        if prevJob:
          calibArgs.extend(['-w', 'done(%s)' %prevJob])
        script = """#!/bin/tcsh
SetupProject DaVinci v28r5
echo "My index is "$LSB_JOBINDEX
python %s < %s
exit $?
""" % ( os.path.expandvars("$KALICALOROOT/python/KaliCalo/CandidateMinimization/Calibrate.py"),
        os.path.join(inputDir, 'input.%s.$LSB_JOBINDEX' % chunkNum))
        with open(os.path.join(inputDir, 'calibrate.%s.csh' % chunkNum), 'w') as f:
          f.write(script)
        os.chmod(os.path.join(inputDir, 'calibrate.%s.csh' % chunkNum), int('777', 8))
        calibArgs.append(os.path.join(inputDir, 'calibrate.%s.csh' % chunkNum))
        print calibArgs
        jobIDs.append(bsub(*calibArgs))
      if 0 == len(jobIDs):
        print "Error submitting"
        sys.exit(1)
      with open(os.path.join(inputDir, 'input.lambdas'), 'w') as f:
        f.write(' '.join(['-o%s' % os.path.join(outputDir, str(step), 'lambda', 'lambdas.gz'), os.path.join(outputDir, str(step), 'lambda','*.gz')]))
      #time.sleep(10)
      joinArgs = ['-Q', "all ~0",
                  #"-i%s" % os.path.join(inputDir, 'input.lambdas'),
                  "-o%s" % os.path.join(outputDir, str(step), 'output', 'output.lambdas'), 
                  '-w', " && ".join(["done(%s)" % jobID for jobID in jobIDs])
                 ]#os.path.expandvars("$KALICALOROOT/python/KaliCalo/CandidateMinimization/CombineLambdaMaps.py")]
      script = """#!/bin/tcsh
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
      prevJob = bsub(*joinArgs)
      if 0 == prevJob:
        print "Error submitting"
        sys.exit(1)
  sys.exit(0)

# EOF

