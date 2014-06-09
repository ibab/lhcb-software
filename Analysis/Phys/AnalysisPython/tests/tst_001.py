#!/bin/env python
# ====================================================================================================================
import ROOT
from   LoKiCore.basic import cpp 

mass = ROOT.RooRealVar('mass','mass',0 ,1)
tau  = ROOT.RooRealVar('tau' ,'tau' ,-1,1)
phi  = ROOT.RooRealVar('phi' ,'phi' ,-4,4)
phis = ROOT.RooArgList( phi ) 
# ===================================================================================================================
## THIS FAILS FOR ROOT6
# ===================================================================================================================

pdf = cpp.Analysis.Models.ExpoPositive ( 'Model' ,'model' , mass , tau , phis , mass.getMin() , mass.getMax() )

print 'PDF:', pdf

