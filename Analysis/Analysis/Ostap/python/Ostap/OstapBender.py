#!/usr/bin/env ipython 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$ 
# =============================================================================
## @file OstapBender.py
#  
#     .oooooo.                .                        
#    d8P'  `Y8b             .o8                        
#   888      888  .oooo.o .o888oo  .oooo.   oo.ooooo.  
#   888      888 d88(  "8   888   `P  )88b   888' `88b 
#   888      888 `"Y88b.    888    .oP"888   888   888 
#   `88b    d88' o.  )88b   888 . d8(  888   888   888 
#    `Y8bood8P'  8""888P'   "888" `Y888""8o  888bod8P' 
#                                            888       
#                                           o888o      
#                                                    
#  Simple interactive analysis environment to provide access to zillions
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
#                    $Revision: 173871 $
#  Last modification $Date: 2014-06-14 14:12:37 +0200 (Sat, 14 Jun 2014) $
#                 by $Author: ibelyaev $
# =============================================================================
""" Simple intractive PyRoot-based analysis environment to provide access
    to zillions useful decorators for ROOT (and not only ROOT) objects&classes
    
    This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign 
of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = "2012-09-10"
__version__ = '$Revision: 173871 $'
# =============================================================================
import ROOT, os, sys
# =============================================================================
# local name 
# =============================================================================
_my_name_ = __name__
if '__main__' == _my_name_ : _my_name_ = 'Ostap Bender'
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
logger = getLogger( _my_name_ )
logger.info ( "Welcome to Ostap\n" + __doc__ )
# =============================================================================
## 1) load LHCb-style file
# =============================================================================
import Ostap.LHCbStyle 
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
import Ostap.PyRoUts   as PyRoUts     ## NB: the most important line!
import Ostap.ZipShelve as ZipShelve 
# =============================================================================
if ROOT.gROOT.IsBatch() :
    ROOT.gROOT.SetBatch ( False )
    logger.info("Set 'IsBatch' to be %s " % ROOT.gROOT.IsBatch () )

# =============================================================================
## minor decoration for shelve module 
from Ostap.shelve_ext import shelve 

# =============================================================================
## import useful context managers
from Ostap.Utils import *

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

