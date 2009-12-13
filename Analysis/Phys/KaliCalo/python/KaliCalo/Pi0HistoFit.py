#!/usr/bin/env python
# =============================================================================
"""
A module for fitting the histograms with pi0-mass
"""
# =============================================================================
from ROOT import TH1F, TF1
from math import sqrt, pi
from array import array

def HiFit(FilledHistos, alam):
    """
    Module for running over all the histograms and calculation of the
    improved correction coefficients. Returnes a dictionary of the
    coefficients.
    """
    # loop over all the cells
    for i in alam.keys():
        # fitting and calculating the new correction coefficient
        alam[i]=alam[i]*0.135/FitProcess(FilledHistos[i])
    return alam

def FitProcess(histo):
    """
    A module for performing a fit of the histogram and return the pi0 mass
    Fit function is a sum of the Gauss and the second order polinom. Gauss mean
    value is returned as the "pi0 mass".
    """
    #== extracting the histogram parameters
    left=histo.GetXaxis().GetXmin()        # left limit
    right=histo.GetXaxis().GetXmax()       # right limit
    binwidth = histo.GetBinWidth(1)        # width of the bin

    #== true pi0 peak parameters
    trueMass = 0.135                       # initial gauss mean
    trueSigm = 0.0094                      # initial gauss sigma
    norm     = 1.0/(sqrt(2.0*pi)*trueSigm) # gauss constant

    #== check if there are enough events in the histogram
    if(histo.GetEntries()<=(right-left)/binwidth):
        print "Bad fit, returned coefficient = 1"
        return trueMass

    #== array of the fit parameters
    npar     = 12                          # number of fit parameters
    par      = array("d",npar*[0.0])       # par[0-5] are fit parameters
                                           # par[6-11] are the errors of fit parameters

    #== fit parameters bounds
    massmin = 0.120                        # min mean value
    massmax = 0.150                        # max mean value
    sigmmin = 0.005                        # min sigma value
    sigmmax = 0.015                        # max sigma value

    #== gauss fit limits
    gaussleft  = trueMass-2*trueSigm       # (mean+/-2sigma)
    gaussright = trueMass+2*trueSigm 

    #== the functions
    polinom = TF1("polinom","1.0+[1]*(x-[0])+[2]*(x-[0])*(x-[0])") # 2nd order polinom
    gauss = TF1("gauss","[0]*exp(-0.5*((x-[1])**2)/([2]**2))")     # gauss function
    fitfunc = TF1("fitfunc","gauss+polinom")                       # sum of the gauss and the polinom
    gauss.SetParameter(1,trueMass)
    gauss.SetParameter(2,trueSigm)
    
    #== fitting
    histo.Fit("gauss","Q","",gaussleft,gaussright)
    parGaus=gauss.GetParameters()

    histo.Fit("polinom","Q","",left,right)
    parPol=polinom.GetParameters()

    #== setting the parameters of the total fit function
    par[0],par[1],par[2]=parGaus[0],parGaus[1],parGaus[2]
    par[3],par[4],par[5]=parPol[0],parPol[1],parPol[2]

    fitfunc.SetParameters(par)
    fitfunc.SetParLimits(1,massmin,massmax)
    fitfunc.SetParLimits(2,sigmmin,sigmmax)
    
    #== fitting
    histo.Fit("fitfunc","Q")

    #== getting the fit parameters and their errors
    for i in range(npar/2):
        par[i]=fitfunc.GetParameter(i)   # parameters
        par[i+6]=fitfunc.GetParError(i)  # errors

    #== return the gauss mean value and it's error
    return par[1]
