#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =========================================================================================
# $Id:$ 
# =========================================================================================
## @file
#  Fit models for seach fro Y+J/psi associative production 
#  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
#
#                    $Revision:$
#  Last modification $Date:$
#                 by $Author:$
# =========================================================================================
""" Fit models for seach fro Y+J/psi associative production 
"""
# =========================================================================================
import ROOT
from   Ostap.PyRoUts         import * 
from   AnalysisPython.Logger import getLogger
# ==========================================================================================
if '__main__' == __name__ : logger = getLogger('OstapTutor/yfits')
else                      : logger = getLogger( __name__          )
# ==========================================================================================


from    OstapTutor.TestVars1   import m_psi,m_ups
import  Ostap.FitModels        as     Models

signal_psi = Models.CB2_pdf('Jpsi' ,
                            mass  = m_psi ,
                            sigma = 0.013 ,
                            mean  = 3.096 )
signal_psi.aL.fix(1.8)
signal_psi.aR.fix(1.8)
signal_psi.nL.fix(1.8)
signal_psi.nR.fix(1.8)

## define model for J/psi
model_psi = Models.Fit1D(  signal      = signal_psi ,
                           background  = Models.Bkg_pdf  ( 'BJpsi'  , mass = m_psi ) )

##  define the model for Y 
model_Y   = Models.Manca2_pdf ( m_ups , power = 0    , 
                                m1s    = 9.4539e+00  ,
                                sigma  = 4.03195e-02 )

model_Ypsi = Models.MancaX_pdf ( model_Y      ,
                                 signal_psi   ,
                                 suffix  = '' ) 

  
# ==========================================================================================
# The END 
# ==========================================================================================
