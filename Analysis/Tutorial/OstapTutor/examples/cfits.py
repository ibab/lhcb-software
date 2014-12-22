#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =========================================================================================
import ROOT
from   Ostap.PyRoUts         import * 
# ==========================================================================================
from   Ostap.Utils           import RooSilent, NoContext
from AnalysisPython.Logger   import getLogger 
if '__main__' == __name__ : logger = getLogger( 'work_CY/fits' )
else                      : logger = getLogger( __name__       )
# ==========================================================================================
logger.info('Define fit models and PDFs for Y+charm analysis')
# ==========================================================================================
from OstapTutor.TestVars1 import m_D0, m_Dp, m_Ds, m_Lc, m_ups

import Ostap.FitModels  as Models 
signal_D0 = Models.D0_pdf (
    mass     = m_D0       ,
    name     = 'D0'       ,
    mean     = 1.8647e+00 , 
    sigma    = 7.2037e-03 )

signal_Dp = Models.Dp_pdf (
    mass     = m_Dp       ,
    name     = 'Dp'       ,
    mean     = 1.8711e+00 , 
    sigma    = 7.1183e-03 ) 

signal_Ds = Models.Ds_pdf (
    mass     = m_Ds       ,
    name     = 'Ds'       , 
    mean     = 1.9699e+00 ,
    sigma    = 6.0178e-03 )  

signal_Lc = Models.Lc_pdf (
    mass     = m_Lc       , 
    name     = 'Lc'       ,
    mean     = 2.2884e+00 ,
    sigma    = 5.1856e-03 ) 


model_D0 = Models.Fit1D ( signal = signal_D0 , bpower = 1 )
model_Dp = Models.Fit1D ( signal = signal_Dp , bpower = 1 )
model_Ds = Models.Fit1D ( signal = signal_Ds , bpower = 1 )
model_Lc = Models.Fit1D ( signal = signal_Lc , bpower = 1 )


model_Y0 = Models.Manca_pdf  ( m_ups , power = 1       , name = 'Y0'  )


model_Y1 = Models.Manca2_pdf ( m_ups , power = 2       , name = 'Y1' , 
                               m1s    = 9.4539e+00   ,
                               sigma  = 4.03195e-02  )

model_Y2 = Models.Manca2_pdf ( m_ups , power = 1       , name = 'Y2' ,
                               m1s    = model_Y1.m1s ,
                               sigma  = model_Y1.s1s ,
                               alphaL = model_Y1.aL  , 
                               alphaR = model_Y1.aR  , 
                               nL     = model_Y1.nL  , 
                               nR     = model_Y1.nR  ) 


signal_D0.mean.fix ( 1.8652e+00 ) 
signal_Dp.mean.fix ( 1.8694e+00 )

model2_D0 = Models.MancaX_pdf ( model_Y1 , signal_D0 , suffix = '_2D0') 
model2_Dp = Models.MancaX_pdf ( model_Y1 , signal_Dp , suffix = '_2Dp') 
model2_Ds = Models.MancaX_pdf ( model_Y1 , signal_Ds , suffix = '_2Ds') 
model2_Lc = Models.MancaX_pdf ( model_Y1 , signal_Lc , suffix = '_2Lc') 

# ==========================================================================================
# The END 
# ==========================================================================================
