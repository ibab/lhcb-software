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
  import subprocess
  return [line for line in subprocess.Popen([cmd]+list(args), stdout=subprocess.PIPE).communicate()[0].split('\n') if line]

gridCastorDir = 'castor:/castor/cern.ch/grid/'
homeCastorDir = os.path.expandvars("castor:$CASTOR_HOME")

CellID = FakeCells.cellID
betas = [8.3, 8.8, 9.5]

if __name__ == '__main__':
  parser = OptionParser()
  parser.add_option("-t", "--cellType",
                    dest="cellType", action="store", type="string", default="SameCell",
                    help="use CELLTYPE cell grouping", metavar="CELLTYPE")
  (options, args) = parser.parse_args()
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
  # Cut cells
  if not 'CaloCalib' in runCommand('nsls', '$CASTOR_HOME'):
    runCommand('nsmkdir', '$CASTOR_HOME/CaloCalib')
  for arg in args:
    # Prepare output dir in Castor
    calibName = os.path.splitext(os.path.basename(arg))[0].lstrip('tup')
    if calibName in runCommand('nsls', '$CASTOR_HOME/CaloCalib'):
      print "Calibration directory $CASTOR_HOME/CaloCalib/%s already exists. I will exit." % calibName
      sys.exit(1)
    dirName = os.path.expandvars('$CASTOR_HOME/CaloCalib/%s' % calibName)
    runCommand('nsmkdir', dirName)
    execfile(arg)
    ntuples = [gridCastorDir+t for t in tup]
    if 0 == len(ntuples):
      print "No ntuples to run on -> %s" % calibName
      continue
    cutter = CellCutter("max(prs1,prs2)<20")
    for ntuple in ntuples:
      cutter.addFile(ntuple)
    # Add cells and opposite cells
    for mCell, cellList in cellsToCalibrate:
      mCellName = str(mCell.all())
      for cell in cellList:
        oppositeCell = Cells.oppositeCell(cell)
        cutter.addCell(cell.index(), mCellName+".root")
        cutter.addCell(oppositeCell.index(), mCellName+".root")
    cutter.cut()
    
    
# EOF
