#!/usr/bin/env python
# =============================================================================
"""
A script for iterative (pi0->gamma gamma) ECAL calibration (primary iterations)

Run:
> python Pi0.py

niter is the desired number of iterations (10 by default)
alam   is the dictionary of the calibration coefficients, cell IDs are used as
       the dictionary keys
pi0mas is the dictionary of the pi0 mass histograms, cell IDs are used as the
       dictionary keys
"""
# =============================================================================
from ROOT import TFile, TH1F

# ========================== some nessesary variables =========================
niter=10                               # number of iterations
alam = {}                              # dictionary of the calibration coefficients
pi0mass={}                             # dictionary of the pi0 mass histograms
Nbad=0                                 # number of cells with too few statistics

from Pi0FillHistos import MyPySelector
from Pi0HistoFit import HiFit

# =============================== opening a file ==============================
#== opening a file
fil=TFile("KaliPi0_Tuples.root")

#== extracting a tree
Bamboo=fil.Get("KaliPi0/Pi0-Tuple")

# =========================== starting the iterations =========================
for i in range(niter):
    print '='*20, "iteration number", i, '='*20

    # sending the correction coefficients to the selector
    MyPySelector().CoefGet(alam, pi0mass)

    # processing the tree to fill the histograms
    Bamboo.Process("TPySelector","Pi0FillHistos")

    # getting the mass histograms
    pi0mass,alam=MyPySelector().HiReturn()

    # fitting the histograms and getting a dictionary of the fit coefficients
    alam=HiFit(pi0mass,alam)

f = open('Coefficients.txt','w')
print >> f, alam

for j in alam.keys():
    if(alam[j]==1.0): Nbad+=1

print "Cells calibrated", len(alam) - Nbad
print Nbad, "cells have too few events for calibration"
print "The others have no events at all"

# =============================================================================
# The END 
# =============================================================================
