#!/usr/bin/env python
# =============================================================================
# @file GetGlobalConstant.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @date 2011-08-25
# =============================================================================
"""
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"

from optparse import OptionParser
import os
import sys

from KaliCalo.Kali.LambdaMap                  import LambdaMap
from KaliCalo.CandidateMinimization.FillTask  import fillDatabase
from KaliCalo.Det                             import getCalo
import KaliCalo.FitUtils                      as FitUtils
import KaliCalo.CandidateMinimization.FitTask as Task
import KaliCalo.FakeCells                     as FakeCells
import KaliCalo.Cells                         as Cells

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

castorDir = '/castor/cern.ch/grid/'

if __name__ == '__main__':
  parser = OptionParser()
  parser.add_option("-t", "--cellType",
                    dest="cellType", action="store", type="string", default="GlobalCalo",
                    help="use CELLTYPE cell grouping", metavar="CELLTYPE")  
  parser.add_option("-o", "--outputDir",
                    dest="outputDir", action="store", type="string", default="",
                    help="DIR where to store output constants", metavar="DIR")
  (options, args) = parser.parse_args()
  if not args:
    options, args = parser.parse_args(sys.stdin.readline().split())
  ntuples = []
  for arg in args:
    execfile(arg)
    ntuples.extend(['castor:' + castorDir + t for t in tup])
  # Load the cell massager
  CellMassager = getattr(FakeCells, options.cellType, None)
  if not CellMassager:
    print "Wrong Cell Massager!"
    sys.exit(1)
  CellMassager = CellMassager()
  lambdas  = LambdaMap()
  #
  from GaudiMP.Parallel import WorkManager
  manager = WorkManager(ppservers=(), ncpus=8)
  histos, badfiles = fillDatabase(lambdas, ntuples, manager=None, cellFunc=CellMassager)
  histomap = Task.fitHistos(histos, manager=None, nHistos=100)
  cells = {}
  for cell in getEcalCells():
    mCell = CellMassager(cell)
    if not mCell in cells:
      cells[mCell] = []
    cells[mCell] = cell
  for mCell, cellList in cells.items():
    hs = histos[mCell]
    mean  = FitUtils.getPi0Params ( hs[0] )[1]
    for cell in cellList:
      lams = lambdas[cell]
      lams.append(mean)
  lambdas.save(os.path.join(options.outputDir, 'lambdas.gz'))
  
  
# EOF
