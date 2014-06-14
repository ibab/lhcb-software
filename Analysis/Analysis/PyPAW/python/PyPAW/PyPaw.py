#!/usr/bin/env ipython 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file PyPaw.py
#
#  Simple PyPaw-analysis environment to provide access to zillions
#  useful decorators for ROOT (and not only ROOT) objects&classes  
# 
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  <b>``C++ ToolKit for Smart and Friendly Physics Analysis''</b>
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software''
#
#  @date   2012-02-15
#  @author Vanya BELYAEV Ivan.Belyaevitep.ru
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""

ooooooooo.               ooooooooo.                              
`888   `Y88.             `888   `Y88.                            
 888   .d88' oooo    ooo  888   .d88'  .oooo.   oooo oooo    ooo 
 888ooo88P'   `88.  .8'   888ooo88P'  `P  )88b   `88. `88.  .8'  
 888           `88..8'    888          .oP\"888    `88..]88..8'   
 888            `888'     888         d8(  888     `888'`888'    
o888o            .8'     o888o        `Y888\"\"8o     `8'  `8'     
             .o..P'                                              
             `Y8P'

Simple PyPaw-analysis environment to provide access to zillions
   useful decorators for ROOT (and not only ROOT) objects&classes  

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = "2012-09-10"
__version__ = '$Revision$'
# =============================================================================
import ROOT, os, sys
# =============================================================================
# local name 
# =============================================================================
_my_name_ = __name__
if '__main__' == _my_name_ : _my_name_ = 'PyPAW'
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
logger = getLogger( _my_name_ )
logger.info ( "Welcome to PyPAW\n" + __doc__ )
# =============================================================================
## 1) load LHCb-style file
# =============================================================================
import PyPAW.LHCbStyle 
# =============================================================================
if ROOT.gROOT.IsBatch() :
    ROOT.gROOT.SetBatch ( False )
    logger.info ( "Set 'IsBatch' to be %s " % ROOT.gROOT.IsBatch () )
# =============================================================================
# The Heart 
# =============================================================================
logger.info ( "Create the default canvas" )
cWidth  = 1000
cHeight =  800 
canvas  = ROOT.TCanvas ( 'glCanvas', _my_name_ , cWidth , cHeight )
if not ROOT.gROOT.IsBatch() :
    ## adjust canvas
    ## @see http://root.cern.ch/root/html/TCanvas.html#TCanvas:TCanvas@4
    dw = cWidth  - canvas.GetWw()
    dh = cHeight - canvas.GetWh()
    canvas.SetWindowSize ( cWidth + dw , cHeight + dh )
    
## load zillions of decorations for ROOT-objects 
import PyPAW.PyRoUts   as PyRoUts     ## NB: the most important line!
import PyPAW.ZipShelve as ZipShelve 
# =============================================================================
if ROOT.gROOT.IsBatch() :
    ROOT.gROOT.SetBatch ( False )
    logger.info("Set 'IsBatch' to be %s " % ROOT.gROOT.IsBatch () )

# =============================================================================
## minor decoration for shelve module 
from PyPAW.shelve_ext import shelve 

# =============================================================================
## import useful context managers
from PyPAW.Utils import *

# ============================================================================= 
## prepend the path 
if '.' not in sys.path :
    logger.info('Prepend sys.path with $PWD')
    sys.path = ['.'] + sys.path 

# =============================================================================
cpp = PyRoUts.cpp
VE  = PyRoUts.VE
# =============================================================================
if '__main__' == __name__ :
    
    print 120*'*'
    print ' Author  : ', __author__ 
    print ' Version : ', __version__ 
    print ' Date    : ', __date__ 
    print 120*'*'


# =============================================================================
# The END 
# =============================================================================

