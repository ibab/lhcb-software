#!/usr/bin/env python
# =============================================================================
# $Id: K0Scompare.py,v 1.1 2004-08-05 12:20:34 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# Simple example to compare "standard" Ks2PiPi algorithm with
# K0S_Creator from Ex/LoKiExmaples package 
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2004-07-24
# =============================================================================

# import the Bender itself  
import gaudimodule   as gaudi 
from   bendermodule  import *
import benderconfig  as bender 
import benderPreLoad as preload
import sys

# =============================================================================
# Generic job configuration & input data 
# =============================================================================

bender.config( files   = [ '$BENDEREXAMPLEOPTS/K0S.opts' ] ,
               options = [ 'DDKs2PiPi.HistogramFlag = true ' ,
                           'LLKs2PiPi.HistogramFlag = true ' ,
                           'LDKs2PiPi.HistogramFlag = true ' ,
                           'LVKs2PiPi.HistogramFlag = true ' ,
                           'EventSelector.PrintFreq =  10  ' ] )

# input data 
g.readOptions('/afs/cern.ch/lhcb/project/web/cards/615300.opts')

# g.HistogramPersistency = "HBOOK" ;
g.HistogramPersistency = 'ROOT' ;

# =============================================================================
# specific job configuration 
# =============================================================================

hsvc = g.property( 'HistogramPersistencySvc' )
hsvc.OutputFile = 'K0S.root'

# =============================================================================
# job execution 
# =============================================================================

g.run(10) 

hSvc = g.histSvc()

h1 = hSvc.retrieve1D("LLKs/21")

import AidaProxy
from benderPiHippo import plotter
plotter.plot(h1)

#from benderROOT import plotter, TH1D

#plotter.plot( h1 ) 

# g.exit()

# =============================================================================
# The END 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.7  2004/03/17 10:18:18  ibelyaev
#  add usage of benderPreLoad module
#
# =============================================================================
