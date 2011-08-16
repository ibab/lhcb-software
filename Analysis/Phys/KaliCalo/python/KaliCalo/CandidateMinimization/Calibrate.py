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


castorDir = 'castor:/castor/cern.ch/grid/'

CellID = FakeCells.cellID
betas = [8.3, 8.8, 9.5]

if __name__ == '__main__':
  parser = OptionParser()
  parser.add_option("-l", "--lambdasDir",
                    dest="lambdasDir", action="store", type="string", default=""
                    help="DIR where lambda files are stored", metavar="DIR")
  parser.add_option("-c", "--cellID",
                    dest="cellID", action="store", type="int", default=0,
                    help="CELLID to calibrate", metavar="CELLID")
  parser.add_option("-t", "--cellType",
                    dest="cellType", action="store", type="string", default="SameCell",
                    help="use CELLTYPE cell grouping", metavar="CELLTYPE")
  parser.add_option("-n", "--name",
                    dest="name", action="store", type="string", default="",
                    help="CELLNAME to assign to the group of cells", metavar="CELLNAME")
  (options, args) = parser.parse_args()
  # Check the lambdas dir and join lambdas (if any)
  if not os.path.exists(os.path.abspath(parser.lambdasDir)):
    os.makedirs(os.path.abspath(parser.lambdasDir))    
  lambdas = LambdaMap()
  for lambdaFile in glob.glob(os.path.join(parser.lambdasDir, "*")):
    newLambda = LambdaMap()
    newLambda.read(lambdaFile)
    lambdas = lambdas * LambdaMap()
  # Load the cell massager
  CellMassager = getattr(FakeCells, parser.cellType, None)()
  if not CellMassager:
    print "Wrong Cell Massager!"
    sys.exit(1)
  # Get a list of cells to add
  if 0 == parser.cellID:
    print "No cell ID specified!"
    sys.exit(1)
  myCellID = CellMassager(CellID(parser.cellID))
  cellList = []
  for i in range(Cells.EcalZone.index()):
    cellID = CellID(i)
    if CellMassager(cellID) == myCellID:
      cellList.append(cellID)
  if 0 == len(cellList):
    print "No valid cells found!"
    sys.exit(1)
  # Get ntuples
  ntuples = []
  for arg in args:
    if os.path.exists(arg):
      execfile(arg)
      ntuples.extend([os.path.join(castorDir, t) for t in tup])
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
  minuit.configure(1)
  varName = parser.name if parser.name else str(myCellID.index())
  for cell in cellList:
    minuit.addCell(cell.index(), varName, lambdas[cell])
  
# EOF
