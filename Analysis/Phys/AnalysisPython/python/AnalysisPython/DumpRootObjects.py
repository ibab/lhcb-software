#!/usr/bin/env python 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file DumpRootObjects.py
#
#  Dump important ROOT objects to ROOT file
#  (the file is needed to read data form "old" ROOT versions with
#  different versions of ROOT streamers)
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @date   2010-09-12
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""
Dump important ROOT objects to ROOT file
(the file is needed to read data form ``old'' ROOT versions with
different versions of ROOT streamers)

This file is a part of BENDER project:
   ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
   ``C++ ToolKit for Smart and Friendly Physics Analysis''
   
By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
   ``No Vanya's lines are allowed in LHCb/Gaudi software.''

                    $Revision$
  Last modification $Date$
                 by $Author$  
                 """
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = "2012-03-16"
__version__ = '$Revision$'
__all__     = (
    'dumpToFile' ,
    'readFile'   , 
    )
# =============================================================================
import ROOT, os
from   AnalysisPython.Logger import getLogger
if __name__ in ( '__main__' , '__builtin__' ) : logger = getLogger ('DumpRootObjects')
else : logger = getLogger ( __name__ )
# ============================================================================

def dumpToFile ( objects , fbase  = 'ROOT_Objects_%d_%s.root' ) :
    
    fname = fbase % ( ROOT.gROOT.GetVersionInt() ,
                      os.environ['CMTCONFIG']    )
    
    f = ROOT.TFile( fname , 'RECREATE' )
    
    for o in objects : o.Write()

    f.ls    ()
    f.Close ()
    
    return fname


v1   = ROOT.RooRealVar( 'v1' , '',1)
v2   = ROOT.RooRealVar( 'v2' , '',1)
vars = ROOT.RooArgSet (  v1  ,    v2 )

objects = [
    
    ROOT.TH1F ( 'h1f' , '' , 10 , 0 , 1 ) , 
    ROOT.TH1D ( 'h1d' , '' , 10 , 0 , 1 ) , 
    ROOT.TH2F ( 'h2f' , '' ,  3 , 0 , 1 , 3 , 0 , 1 ) , 
    ROOT.TH2D ( 'h2d' , '' ,  3 , 0 , 1 , 3 , 0 , 1 ) , 
    ROOT.TH3F ( 'h3f' , '' ,  2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 ) , 
    ROOT.TH3D ( 'h3d' , '' ,  2 , 0 , 1 , 2 , 0 , 1 , 2 , 0 , 1 ) ,
    ROOT.TProfile('prof1','',10,0,1) , 
    ROOT.TF1  ( 'f1' , 'sin(x)/x' ) , 
    ROOT.TF2  ( 'f2' , 'sin(x)*sin(y)/x/y') ,    
    ROOT.TGraph            ( 5 ) ,
    ROOT.TGraphErrors      ( 5 ) ,
    ROOT.TGraphAsymmErrors ( 5 ) ,
    ROOT.TTree  ('tt','') ,
    ROOT.TBox   ( 0 , 0 , 1 , 1 ) ,
    ROOT.TLine  ( 0 , 0 , 1 , 1 ) ,
    ROOT.TAxis  ( 2 , 0 , 1 ) ,
    ## ROOT.TFitResultPtr () ,  
    ROOT.TFitResult    () , 
    ROOT.TCut   ('cut') ,
    ##
    ROOT.RooRealVar    () ,
    ROOT.RooArgSet     () ,
    ROOT.RooArgList    () ,
    v1 , v2 , vars ,
    ROOT.RooDataSet    ( 'ds', '', vars )
    ]

def readFile ( fname ) :

    f = ROOT.TFile( fname , 'READ' )

    keys = f.GetListOfKeys()

    for k in keys :

        key = "%s;%d" % ( k.GetName() , k.GetCycle() )

        obj = f.Get( key )
        
        print obj

# =============================================================================
if __name__ == '__main__' :
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120
    
    f = dumpToFile ( objects ) 
    readFile ( f )
    
# =============================================================================
# The END
# =============================================================================
