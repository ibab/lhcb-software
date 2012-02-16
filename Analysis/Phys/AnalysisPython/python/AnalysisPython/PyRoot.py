#!/usr/bin/env ipython 
# =============================================================================
# $Id:$ 
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
#                    $Revision: 135413 $
#  Last modification $Date: 2012-02-15 21:32:19 +0100 (Wed, 15 Feb 2012) $
#                 by $Author: ibelyaev $
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
__version__ = '$Revision: 135413 $'
# =============================================================================
import ROOT, os, sys, logging
# =============================================================================
# local name 
# =============================================================================
_my_name_ = __name__
if '__main__' == _my_name_ : _my_name_ = 'PyRoot'
# =============================================================================
# logging 
# =============================================================================
logger = logging.getLogger(_my_name_)
if not logger.handlers : logging.basicConfig()
logger.setLevel(logging.DEBUG)
# =============================================================================
## 1) load LHCb-style file
# =============================================================================
import AnalysisPython.LHCbStyle 
# =============================================================================
if ROOT.gROOT.IsBatch() :
    ROOT.gROOT.SetBatch ( False )
    logger.info("Set 'IsBatch' to be %s " % ROOT.gROOT.SetBatch () )
# =============================================================================
# The Heart 
# =============================================================================
canvas  = ROOT.TCanvas( 'Canvas', _my_name_ , 1000 , 800 )
## load zillions of decorations for ROOT-objects 
import AnalysisPython.PyRoUts   as PyRoUts     ## NB: the most important line!
import AnalysisPython.ZipShelve as ZipShelve 
# =============================================================================
if ROOT.gROOT.IsBatch() :
    ROOT.gROOT.SetBatch ( False )
    logger.info("Set 'IsBatch' to be %s " % ROOT.gROOT.SetBatch () )
# =============================================================================
# tiny decoration for shelve module
# =============================================================================
import shelve
_old_shelve_open_ = shelve.open
## A bit extended version of shelve.open
def _new_shelve_open_ ( filename , *kargs , **kwargs ) :
    """
    A bit extended version of shelve.open
    """
    import os
    filename = os.path.expandvars ( filename )
    filename = os.path.expandvars ( filename )
    filename = os.path.expandvars ( filename )
    filename = os.path.abspath    ( filename )
    #
    return _old_shelve_open_ ( filename , *kargs , **kwargs ) 

_new_shelve_open_ .__doc__ += '\n' + _old_shelve_open_ .__doc__ 
shelve.open = _new_shelve_open_ 
## List DB-keys 
def _ls_ ( self )  :
    """
    List DB-keys 
    """
    keys = self.keys()
    keys.sort()
    for k in keys : print k

shelve.Shelf.ls = _ls_
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

