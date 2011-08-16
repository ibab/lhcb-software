#!/usr/bin/env python
# =============================================================================
# @file Calibrate.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @date 2011-08-16
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
  sc = gROOT.LoadMacro(os.path.join(kaliDir, "root", "EcalMinimizer.cxx+"))
  from ROOT import EcalMinimizer
except:
  print "Error importing EcalMinimizer!"
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

def indexToCellID(cellList):
  output = []
  ecal = getCalo()
  for cell in cellList:
    cellID = ecal.cellIdByIndex(int(cell))
    if ecal.valid(cellID) and not cellID.isPin():
      output.append(cellID)
  return output

castorDir = 'castor:/castor/cern.ch/grid/'

CellID = FakeCells.CellID
betas = [8.3, 8.8, 9.5]
ecalCells = getEcalCells()

if __name__ == '__main__':
  parser = OptionParser()
  parser.add_option("-l", "--lambdaFile",
                    dest="lambdaFile", action="store", type="string", default="",
                    help="FILE where lambda constants are stored", metavar="FILE")
  parser.add_option("-c", "--cellIDs",
                    dest="cellIDs", action="store", type="string", default="",
                    help="comma-separated list of CELLIDs to calibrate", metavar="CELLIDs")
  parser.add_option("-t", "--cellType",
                    dest="cellType", action="store", type="string", default="SameCell",
                    help="use CELLTYPE cell grouping", metavar="CELLTYPE")
  (options, args) = parser.parse_args()
  # Check the lambdas dir and join lambdas (if any)
  lambdas = LambdaMap.LambdaMap()
  lambdaFile = options.lambdaFile
  if os.path.exists(lambdaFile) and os.path.isfile(lambdaFile):
    print "Found DB -> %s" % lambdaFile
    lambdas.read(lambdaFile)
  # Load the cell massager
  CellMassager = getattr(FakeCells, options.cellType, None)()
  if not CellMassager:
    print "Wrong Cell Massager!"
    sys.exit(1)
  # Get a list of cells to add
  if "" == options.cellIDs:
    print "No cell ID specified!"
    sys.exit(1)
  cellsToCalibrate = {}
  for cellID in indexToCellID(options.cellIDs.split(',')): # CellIDs are comma separated
    cellsToCalibrate[CellMassager(cellID)] = []
  print "Cells to calibrate: %s" % ' '.join(map(str, cellsToCalibrate.keys()))
  for cellID in ecalCells:
    mCellID = CellMassager(cellID)
    if mCellID in cellsToCalibrate.keys():
      cellsToCalibrate[mCellID].append(cellID)
  # Get ntuples
  ntuples = []
  for arg in args:
    if os.path.exists(arg):
      execfile(arg)
      ntuples.extend([castorDir+t for t in tup])
  if 0 == len(ntuples):
    print "No valid ntuples found"
    sys.exit(1)
  fileList = std.vector('TString')()
  for ntuple in ntuples:
    fileList.push_back(ntuple)
  # Configure Minuit
  minuit = EcalMinimizer()
  for area, beta in enumerate(betas):
    minuit.setBeta(area, beta)
  minuit.loadFiles(fileList)
  minuit.configure(len(cellsToCalibrate)+1)
  # Add cells to calibrate
  for var, cellList in cellsToCalibrate.items():
    varName = str(var.all())
    for cell in cellList:
      minuit.addCell(cell.index(), varName, lambdas[cell][-1])
  # Add other cells with a configuration
  for cell in lambdas:
    if not minuit.hasCell(cell.index()):
      minuit.addCell(cell.index(), "fixed", lambdas[cell][-1])
  # Minimize all vars
  newLambdas = LambdaMap.LambdaMap()
  for var in cellsToCalibrate:
    varName = str(var.all())
    for varToFix in cellsToCalibrate:
      varToFix = str(varToFix.all())
      value = lambdas[cellsToCalibrate[var][0]][-1]
      if varToFix != varName:
        minuit.setFixedVar(varToFix, value)
      else:
        minuit.setLimitedVar(varToFix, value, value*0.95, value*1.05)
    sc = minuit.minimize()
    # TODO: error handling
    calib = minuit.getVarCalib(varName)
    for cell in cellsToCalibrate[var]:
      l = newLambdas[cell]
      l[-1] = calib
  
# EOF
