#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file TestWeighted.py
#
#  tests for weighted fits and model with convolution 
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2014-05-11
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
import ROOT
from   Ostap.PyRoUts         import *
from   Ostap.Utils           import rooSilent 
# =============================================================================
from AnalysisPython.Logger   import getLogger 
if '__main__' == __name__ : logger = getLogger( 'Ostap.TestWeighted' )
else                      : logger = getLogger( __name__             )
# =============================================================================
logger.info ( 'Test for weighted fits and model with convolution')
# ============================================================================
RAD = ROOT.RooAbsData
if RAD.Tree != RAD.getDefaultStorageType() :
    logger.info ( 'DEFINE default storage type to be TTree! ') 
    RAD.setDefaultStorageType ( RAD.Tree )

# ============================================================================
# The variables  
# ============================================================================
m_b    = ROOT.RooRealVar ( 'mB'   , 'm(B)'   ,  5.2 , 5.4   )
m_phi  = ROOT.RooRealVar ( 'mPhi' , 'm(Phi)' ,  1.0 , 1.050 )
h_phi  = m_phi.histo     ( 50 )
# ============================================================================

import Ostap.FitModels       as     Models
sigB   = Models.Gauss_pdf ( 'Bh'             ,
                            m_b.getMin ()    ,
                            m_b.getMax ()    , 
                            mass     = m_b   ,
                            mean     = 5.278 , 
                            sigma    = 0.015 )

bw     = cpp.Gaudi.Math.Phi0 ( 1.0195 ,
                               0.0043 ,
                               0.4937 )

sigPhi0 = Models.BreitWigner_pdf ( 'Phi0'               ,
                                   bw                   , 
                                   mean        = 1.0195 ,
                                   gamma       = 0.0043 ,
                                   mass        = m_phi  ,
                                   convolution = 0.0005 )

sigPhi = sigPhi0

ps2    = cpp.Gaudi.Math.PhaseSpaceNL ( 2*0.4937 , 5.278-3.096 , 2 , 3 ) 
model0 = Models.Fit2D (
    signal_1 = sigB    ,
    signal_2 = sigPhi0 ,
    ##
    bkg2     = Models.PSPol_pdf  ( 'PSP0' , m_phi , ps2 , power = 1 ) , 
    bkgB     = Models.PSPol_pdf  ( 'PSB0' , m_phi , ps2 , power = 1 ) ,
    suffix   = '_GEN'
    )
model0.bkg1.tau.fix(0)
model0.bkgA.tau.fix(0)

model0.ss.fix ( 1000 )
model0.sb.fix (  300 )
model0.bs.fix (  300 )
model0.bb.fix (  300 )

model1 = Models.Fit2D (
    signal_1 = sigB   ,
    signal_2 = sigPhi ,
    ##
    bkg2     = Models.PSPol_pdf    ( 'PSP' , m_phi , ps2 , power = 1 ) , 
    bkgB     = Models.PSPol_pdf    ( 'PSB' , m_phi , ps2 , power = 1 ) ,
    )

model_phi = Models.Fit1D(
    signal     = sigPhi ,
    background = Models.PSPol_pdf  ( 'PSQ' , m_phi , ps2 , power = 1 ) ,
    suffix     = '_P'      
    )

model_B   = Models.Fit1D(
    signal     = sigB ,
    background = Models.Bkg_pdf    ( 'BB' , m_b    , power = 1  ) ,
    suffix     = '_B'      
    )

varset  = ROOT.RooArgSet ( m_b , m_phi )
dataset = model0.pdf.generate( varset , 1900 )  

## 2D fits 
with rooSilent() :  
    r,u = model1.fitTo( dataset )
    r,u = model1.fitTo( dataset )

logger.info('Results of 2D-fit \n%s' % r )

## B-mass fit
with rooSilent() :  
    rB,u = model_B.fitTo ( dataset )
    rB,u = model_B.fitTo ( dataset )
    model_B.sPlot ( dataset )
    
logger.info('Results of B-mass fit \n%s' % rB )

#
dataset.project ( h_phi , 'mPhi' , 'S_B_sw' )
dataW = dataset.makeWeighted ( 'S_B_sw' )

with rooSilent() :
    
    rH,u  = model_phi.fitHisto ( h_phi , False )
    rW,u  = model_phi.fitTo ( dataW , False , 100 , False ) 
    rWt,u = model_phi.fitTo ( dataW , False , 100 , False , ROOT.RooFit.SumW2Error ( True  ) )
    rWf,u = model_phi.fitTo ( dataW , False , 100 , False , ROOT.RooFit.SumW2Error ( False ) )
    
logger.info ( 'Results of (binned)                         phi fit \n%s' % rH  )
logger.info ( 'Results of (unbinned weighted) default      phi fit \n%s' % rW  )
logger.info ( 'Results of (unbinned weighted) sumw2(True)  phi fit \n%s' % rWt )
logger.info ( 'Results of (unbinned weighted) sumw2(False) phi fit \n%s' % rWf )

logger.info ( 'HISTO           %s ' % rH  ('S_P') [0] ) 
logger.info ( 'WEIGHTED        %s ' % rW  ('S_P') [0] ) 
logger.info ( 'WEIGHTED(True)  %s ' % rWt ('S_P') [0] ) 
logger.info ( 'WEIGHTED(False) %s ' % rWf ('S_P') [0] ) 

# =============================================================================
# The END 
# =============================================================================

