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
from ROOT import *

from KaliCalo.Kali.LambdaMap                  import LambdaMap
from KaliCalo.CandidateMinimization.FillTask  import fillDatabase
from KaliCalo.Det                             import getCalo
from KaliCalo.Kali                            import pi0Mass
import KaliCalo.FitUtils                      as FitUtils
import KaliCalo.CandidateMinimization.FitFunctions as FitFunctions
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
                    dest="cellType", action="store", type="string", default="global",
                    help="use CELLTYPE cell grouping", metavar="CELLTYPE")  
  parser.add_option("-o", "--outputDir",
                    dest="outputDir", action="store", type="string", default="",
                    help="DIR where to store output constants", metavar="DIR")
  (options, args) = parser.parse_args()
  if not args:
    options, args = parser.parse_args(sys.stdin.readline().split())
  ntuples = []
  chain = TChain("KaliPi0/Pi0-Tuple")
  for arg in args:
    execfile(arg)
    for f in ['castor:' + castorDir + t for t in tup]:
      chain.AddFile(f)
  lambdas  = LambdaMap()
  histos = {}
  if options.cellType == "global": # Only one constant
    globalH = TH1F("globalH", "globalH", 250, 0, 250)
    chain.Project("globalH", "m12", "max(prs1,prs2) < 10")
    st, n = FitFunctions.simpleFit(globalH)
    histos[0] = globalH
    histos[1] = globalH
    histos[2] = globalH
  elif options.cellType == "area": # three constants
    inner  = TH1F("inner", "inner", 250, 0, 250)
    chain.Project("inner", "m12", "max(prs1,prs2)<10 && (ind1>8000 && ind2>8000)")
    middle = TH1F("middle", "middle", 250, 0, 250)
    chain.Project("middle", "m12", "max(prs1,prs2)<10 && (ind1<8000 && ind1>4000 && ind2<8000 && ind2>4000)")
    outer  = TH1F("outer", "outer", 250, 0, 250)
    chain.Project("outer", "m12", "max(prs1,prs2)<10 && (ind1<4000 && ind2<4000)")
    st, n = FitFunctions.simpleFit(outer)
    st, n = FitFunctions.simpleFit(middle)
    st, n = FitFunctions.simpleFit(inner)
    histos[0] = outer
    histos[1] = middle
    histos[2] = inner
  else:
    print "Unknown cell type -> %s" % options.cellType
    sys.exit(1)
  means = {}
  means[2] = FitUtils.getPi0Params(histos[2])[1]
  means[1] = FitUtils.getPi0Params(histos[1])[1]
  means[0] = FitUtils.getPi0Params(histos[0])[1]
  print "Found mean positions"
  print "  Inner : %s" % means[2]
  print "  Middle: %s" % means[1]
  print "  Outer : %s" % means[0]
  # Save results 
  for cell in getEcalCells():
    area = cell.area()
    lams = lambdas[cell]
    lams.append(pi0Mass/means[area].value())
  lambdas.save(os.path.join(options.outputDir, 'lambdas.gz'))
  sys.exit(0)
  
# EOF

