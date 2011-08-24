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

castorDir = '/castor/cern.ch/grid/'

CellID = FakeCells.CellID
betas = [8.3, 8.8, 9.5]
sigmas = [15.0, 15.0, 15.0]
ecalCells = getEcalCells()

if __name__ == '__main__':
  parser = OptionParser()
  parser.add_option("-l", "--lambdaFile",
                    dest="lambdaFile", action="store", type="string", default="",
                    help="FILE where input constants are stored", metavar="FILE")
  # parser.add_option("-c", "--cellIDs",
  #                   dest="cellIDs", action="store", type="string", default="",
  #                   help="comma-separated list of CELLIDs to calibrate", metavar="CELLIDs")
  parser.add_option("-c", "--cut",
                    dest="cut", action="store", type="string", default="",
                    help="activate cutting of cells from ntuples in FILE", metavar="FILE")
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
  if 0 == len(args):
    print "No cell ID specified!"
    sys.exit(1)
  cellsToCalibrate = []
  for inputNtuple in args: # CellIDs are comma separated
    cellIndex = os.path.splitext(os.path.basename(inputNtuple))[0]
    cellID = decodeCellID(cellIndex)
    if not cellID == CellMassager(cellID):
      print "Input cellID is not a massaged cell!! -> %s vs %s" % (cellID, CellMassager(cellID))
      sys.exit(1)
    cellsToCalibrate.append(cellID)
  print "Cells to calibrate: %s" % ' '.join(map(str, cellsToCalibrate))
  cellMap = {}
  cellRel = {}
  for cellID in ecalCells:
    mCellID = CellMassager(cellID)
    mOppCell = CellMassager(Cells.oppositeCell(cellID))
    if not mCellID in cellMap:
      cellMap[mCellID] = []
    cellMap[mCellID].append(cellID)
    if not mCellID in cellRel:
      cellRel[mCellID] = []
    cellRel[mCellID].append(mOppCell)
  # We need to check all input ntuples
  inputFiles = {'found': [], 'missing': []}
  for arg in args:
    if 0 == len(Commands.nsls(arg)):
      inputFiles['missing'].append(arg)
    else:
      inputFiles['found'].append(arg)
  if 0 != len(inputFiles['missing']): # There are missing input files
    if os.path.exists(options.cut):
      execfile(options.cut)
      ntuples = ['castor:' + os.path.join(castorDir, t) for t in tup]
      outputTuple = 'castor:' + arg
      cutter = CellCutter("", 100)
      for ntuple in ntuples:
        cutter.addFile(ntuple)    
      for arg in inputFiles['missing']:
        cellIndex = os.path.splitext(os.path.basename(arg))[0]
        cellID = decodeCellID(cellIndex)
        fName = 'castor:' + os.path.join(castorDir, arg)
        for cID in (cellID + cellRel[cellID]):
          for cell in cellMap[cID]:
            cutter.addCell(cell.index(), fName)
      cutter.cut(20.0)
    else:
      print "Missing files but no input ntuples specified!"
      sys.exit(1)
  # Get ntuples
  inputFiles = inputFiles['missing'] + inputFiles['found']
  if 0 == len(inputFiles):
    print "No valid ntuples found"
    sys.exit(1)
  # Calibration
  newLambdas = LambdaMap.LambdaMap()
  for inputFile in inputFiles:
    cellIndex = os.path.splitext(os.path.basename(inputFile))[0]
    cellID = decodeCellID(cellIndex)    
    fileList = std.vector('TString')()
    fileList.push_back(inputFile)
    # Configure Minuit
    minuit = EcalMinimizer()
    for area, beta in enumerate(betas):
      minuit.setBeta(area, beta)
    for area, sigma in enumerate(sigmas):
      minuit.setSigma(area, sigma)
    #minuit.setDebug()
    minuit.loadFiles(fileList)    
    myCells = [cellID] + [cell for cell in cellRel[cellID]]
    minuit.configure(len(myCells)+1) # Own cell + opposite cells + fixed cells
    # Add cells to calibrate
    print "Adding cells to calibrate"
    for cellVar in myCells:
      varName = str(cellVar.index())
      for cell in cellMap[cellVar]:
        minuit.addCell(cell.all(), varName, lambdas[cell][-1])
      minuit.setLimitedVar(varName, lambdas[cell][-1], lambdas[cell][-1]*0.95, lambdas[cell][-1]*1.05)
    print "Adding fixed cells"
    for cell in lambdas:
      if not minuit.hasCell(cell.all()):
        minuit.addCell(cell.all(), "fixed", lambdas[cell][-1])
    sc = minuit.minimize()
    # TODO: error handling
    for cellVar in myCells:
      varName = str(cellVar.index())
      calib = minuit.getVarCalib(varName)
      for cell in cellMap[cellVar]:
        l = newLambdas[cell]
        l[-1] = calib
  print "We have %s new calibrated cells" % len(newLambdas.lambdas())
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
