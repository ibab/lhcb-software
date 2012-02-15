#!/usr/bin/env python 
# =============================================================================
# $Id$ 
# =============================================================================
## @file Bender/PyRoot.py
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
import ROOT


# =============================================================================
import ROOT
# =============================================================================
## load LHCb-style file 
def LHCbStyle ( stylefile = '$BENDERROOT/scripts/lhcbStyle.C' ) :
    """
    Load LHCb-Style file for plots 
    """

    if ROOT.gROOT.IsBatch() :
        ROOT.gROOT.SetBatch ( False )
        
    if hasattr ( ROOT , 'LHCb' ) and hasattr ( ROOT.LHCb , 'setLHCbStyle' ) :
        print 'Style-file is already loaded '
        return ROOT.gROOT.IsBatch() 
        
    import os
    if not os.path.exists ( stylefile ) :
        stylefile = os.path.expandvars ( stylefile ) 
    if not os.path.exists ( stylefile ) : 
        stylefile = os.path.expanduser ( stylefile ) 
    if not os.path.exists ( stylefile ) : 
        stylefile = '/afs/cern.ch/user/i/ibelyaev/public/lhcbStyle.C'
        
    if os.path.exists ( stylefile ) : 
        ROOT.gROOT.LoadMacro( stylefile )
        ROOT.LHCb.setLHCbStyle()
    else :
        print "WARNING: can't find/load LHCb-style file"

    ## put some minor polishing atop of the style
    #
    ROOT.gStyle.SetOptStat (0)
    ROOT.gStyle.SetOptFit  (0)
    # ROOT.gStyle.SetOptFit  (1111)
    # ROOT.gStyle.SetOptFit  (0)
    
    if ROOT.gROOT.IsBatch() :
        ROOT.gROOT.SetBatch ( False )
        
    return ROOT.gROOT.IsBatch() 

# =============================================================================
# The Heart 
# =============================================================================
LHCbStyle()
import AnalysisPython.PyRoUts   as PyRoUts     ## NB: the most important line!
import AnalysisPython.ZipShelve as ZipShelve 
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
bw  = cpp.Bender.Welcome.instance()
bw.welcome()
# =============================================================================
# 
if '__main__' == __name__ :
    
    print 120*'*'
    print ' Author  : ', __author__ 
    print ' Version : ', __version__ 
    print ' Date    : ', __date__ 
    print 120*'*'


# =============================================================================
# The END 
# =============================================================================

