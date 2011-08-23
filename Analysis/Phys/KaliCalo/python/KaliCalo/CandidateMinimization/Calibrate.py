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

def decodeCellID( cellinfo ):
  """
  Decodes the CellID.index() information:
    * 6 bits: col.
    * 6 bits: row.
    * 2 bits: area.
    * 2 bits: calo.
  """
  def intToBinary( num , digits = None ):
    out = ''
    while num > 0:
      j = num & 1
      out = str(j) + out
      num >>= 1
    if digits and len(out) > digits:
      print "Warning: Output is longer than 'digits'."
      return out
    if digits:
      while True:
        if len(out) < digits:
          out = '0%s' %out
        else:
          break
    return out

  if not isinstance(cellinfo, int):
    cellinfo = int(cellinfo)
  bin = intToBinary( cellinfo , 16 )
  #calo = int( bin[0:2] , 2 )
  calo = 2
  area = int( bin[2:4] , 2 )
  row  = int( bin[4:10] , 2 )
  col  = int( bin[10:16] , 2 )
  return Cells.CellID( calo , area , row , col )

def decodeCellIDs( cellList ):
  output = []
  ecal = getCalo()
  for cell in cellList:
    cellID = decodeCellID(cell)
    if ecal.valid(cellID) and not cellID.isPin():
      output.append(cellID)
    else:
      print "Non-valid cell -> %s" % cellID
  return output

castorDir = 'castor:/castor/cern.ch/grid/'

CellID = FakeCells.CellID
betas = [8.3, 8.8, 9.5]
sigmas = [15.0, 15.0, 15.0]
ecalCells = getEcalCells()

if __name__ == '__main__':
  parser = OptionParser()
  parser.add_option("-l", "--lambdaFile",
                    dest="lambdaFile", action="store", type="string", default="",
                    help="FILE where input constants are stored", metavar="FILE")
  parser.add_option("-c", "--cellIDs",
                    dest="cellIDs", action="store", type="string", default="",
                    help="comma-separated list of CELLIDs to calibrate", metavar="CELLIDs")
  parser.add_option("-t", "--cellType",
                    dest="cellType", action="store", type="string", default="SameCell",
                    help="use CELLTYPE cell grouping", metavar="CELLTYPE")
  parser.add_option("-o", "--outputDir",
                    dest="outputDir", action="store", type="string", default="",
                    help="DIR where to store output constants", metavar="DIR")
  (options, args) = parser.parse_args()
  if not args:
    options, args = parser.parse_args(sys.stdin.readline().split())
  # Check the lambdas dir and join lambdas (if any)
  lambdas = LambdaMap.LambdaMap()
  lambdaFile = options.lambdaFile
  if os.path.exists(lambdaFile) and os.path.isfile(lambdaFile):
    print "Found DB -> %s" % lambdaFile
    lambdas.read(lambdaFile)
  # Load the cell massager
  CellMassager = getattr(FakeCells, options.cellType, None)
  if not CellMassager:
    print "Wrong Cell Massager!"
    sys.exit(1)
  CellMassager = CellMassager()
  # Get a list of cells to add
  if "" == options.cellIDs:
    print "No cell ID specified!"
    sys.exit(1)
  cellsToCalibrate = {}
  oppositeCells = {}
  cellRel = {}
  for cellID in decodeCellIDs(options.cellIDs.split(',')): # CellIDs are comma separated
    cellsToCalibrate[CellMassager(cellID)] = []
    #oppositeCells[CellMassager(Cells.oppositeCell(CellMassager(cellID)))] = []
  print "Cells to calibrate: %s" % ' '.join(map(str, cellsToCalibrate.keys()))
  for cellID in ecalCells:
    mCellID = CellMassager(cellID)
    oppCellID = Cells.oppositeCell(cellID)
    oppMCellID = CellMassager(oppCellID)
    if mCellID in cellsToCalibrate.keys():
      if not mCellID in cellRel:
        cellRel[mCellID] = []
      cellsToCalibrate[mCellID].append(cellID)
      if not oppMCellID in oppositeCells:
        oppositeCells[oppMCellID] = []
        cellRel[mCellID].append(oppMCellID)
      oppositeCells[oppMCellID].append(oppCellID)
  print cellRel
  # Get ntuples
  ntuples = []
  for arg in args:
    #if os.path.exists(arg):
      ntuples.append(arg)
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
  for area, sigma in enumerate(sigmas):
    minuit.setSigma(area, sigma)
  #minuit.setDebug()
  minuit.loadFiles(fileList)
  minuit.configure(2*len(cellsToCalibrate)+1)
  # Add cells to calibrate
  print "Adding cells to calibrate"
  for var, cellList in cellsToCalibrate.items():
    varName = str(var.index())
    for cell in cellList:
      minuit.addCell(cell.all(), varName, lambdas[cell][-1])
    for oppMCell in cellRel[var]:
      oppVarName = str(oppMCell.index())
      for cell in oppositeCells[oppMCell]:
        minuit.addCell(cell.all(), oppVarName, lambdas[cell][-1])
  # Add other cells with a configuration
  print "Adding fixed cells"
  for cell in lambdas:
    if not minuit.hasCell(cell.all()):
      minuit.addCell(cell.all(), "fixed", lambdas[cell][-1])
  # Minimize all vars
  newLambdas = LambdaMap.LambdaMap()
  for var in cellsToCalibrate:
    oppVars = cellRel[var]
    print "Calibrating -> %s and %s" % (var, ' and '.join(map(str,oppVars)))
    varName = str(var.index())
    for varToFix in cellsToCalibrate:
      oppVarsToFix = cellRel[varToFix]
      value    = lambdas[cellsToCalibrate[varToFix][0]][-1]
      varToFix    = str(varToFix.index())
      if varToFix != varName:
        minuit.setFixedVar(varToFix, value)
        for oppVarToFix in oppVarsToFix:
          oppValue = lambdas[oppositeCells[oppVarToFix][0]][-1]
          oppVarName = str(oppVarToFix.index())
          minuit.setFixedVar(oppVarName, oppValue)
      else:
        minuit.setLimitedVar(varToFix, value, value*0.95, value*1.05)
        for oppVarToFix in oppVarsToFix:
          oppValue = lambdas[oppositeCells[oppVarToFix][0]][-1]
          oppVarName = str(oppVarToFix.index())
          minuit.setLimitedVar(oppVarName, oppValue, oppValue*0.95, oppValue*1.05)
    sc = minuit.minimize()
    # TODO: error handling
    calib = minuit.getVarCalib(varName)
    for cell in cellsToCalibrate[var]:
      l = newLambdas[cell]
      l[-1] = calib
    for oppVar in cellRel[var]:
      oppVarName = str(oppVar.index())
      calib = minuit.getVarCalib(oppVarName)
      for cell in oppositeCells[oppVar]:
        l = newLambdas[cell]
        l[-1] = calib
  print "We have %s new calibrations" % len(newLambdas.lambdas())
  fileName = "%s.gz" % '-'.join([str(c.all()) for c in cellsToCalibrate.keys()])
  outputDir = os.path.abspath(options.outputDir)
  if not os.path.exists(outputDir):
    os.makedirs(outputDir)
  fileName = os.path.join(outputDir, fileName)
  newLambdas.save(fileName)
  lambdasFile = os.path.join(outputDir,"lambdas.input")
  if not os.path.exists(lambdasFile):
    with open(lambdasFile, 'w') as f:
      f.write("")
  with open(lambdasFile, 'a') as f:
    f.write(" %s " % fileName)
  print "Done!"
  sys.exit(0)

# EOF
