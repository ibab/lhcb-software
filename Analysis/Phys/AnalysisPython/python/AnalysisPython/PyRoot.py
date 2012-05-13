#!/usr/bin/env ipython 
# =============================================================================
# $Id$ 
# =============================================================================
## @file AnalysisPython/PyRoot.py
#
#  Simple PyRoot-analysis environment pt provide access to zillions
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

Simple PyRoot-analysis environment pt provide access to zillions
   useful decorators for ROOT (and not only ROOT) objects&classes  

This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''

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
if '__main__' == _my_name_ : _my_name_ = 'PyRoot'
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
logger = getLogger( _my_name_ )
logger.info ( "Welcome to PyRoot" )
# =============================================================================
## 1) load LHCb-style file
# =============================================================================
import AnalysisPython.LHCbStyle 
# =============================================================================
if ROOT.gROOT.IsBatch() :
    ROOT.gROOT.SetBatch ( False )
    logger.info ( "Set 'IsBatch' to be %s " % ROOT.gROOT.SetBatch () )
# =============================================================================
# The Heart 
# =============================================================================
logger.info ( "Create the default canvas" )
canvas  = ROOT.TCanvas ( 'Canvas', _my_name_ , 1000 , 800 )
## load zillions of decorations for ROOT-objects 
import AnalysisPython.PyRoUts   as PyRoUts     ## NB: the most important line!
import AnalysisPython.ZipShelve as ZipShelve 
# =============================================================================
if ROOT.gROOT.IsBatch() :
    ROOT.gROOT.SetBatch ( False )
    logger.info("Set 'IsBatch' to be %s " % ROOT.gROOT.SetBatch () )

# ============================================================================
## minor decoration for shelve module 
from AnalysisPython.shelve_ext import shelve 

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

