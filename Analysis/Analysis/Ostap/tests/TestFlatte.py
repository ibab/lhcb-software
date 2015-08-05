#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =========================================================================================
# $Id$ 
# =========================================================================================
# @file  TestFlatte
# test for Flatte shape
#
# @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
# @date 2014-05-11
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
Tests for various background fit models  
"""
import ROOT
from   Ostap.PyRoUts         import * 
# ==========================================================================================
from AnalysisPython.Logger   import getLogger 
if '__main__' == __name__ : logger = getLogger( 'Ostap.TestFlatte'  )
else                      : logger = getLogger( __name__  )
# ==========================================================================================
logger.info('Compare Breit and Flatte')
# ==========================================================================================
import Ostap.FitModels as Models
from   Ostap.Utils     import rootWarning
m_X    = 3.87156
g_X    = 0.001 
m_Jpsi = 3.0960
m_pipi = 0.4000

mass   = ROOT.RooRealVar('m' , 'mass(J/psi pipi)' , 3.868 , 3.88 ) 

bw    = cpp.Gaudi.Math.BreitWigner ( m_X , g_X , m_Jpsi , m_pipi , 0 )
breit = Models.BreitWigner_pdf ( 'BW'          ,
                                 bw            ,
                                 mass  = mass  ,
                                 mean  = m_X   ,
                                 gamma = g_X   )


a1 = 0.40
a2 = 0.03

a1 = 0.03
a2 = 0.40

f1 = a1 /( a1+a2)
f2 = a2 /( a1+a2)

fl_      = cpp.Gaudi.Math.Flatte ( m_X           ,
                                   m_X * g_X * f1 ,
                                   f2/f1          ,
                                   m_Jpsi         ,
                                   m_pipi         ,
                                   1.86483        ,
                                   2.00696        )

flatte  = Models.Flatte_pdf ( 'Flatte' , fl_ , 
                              mass   = mass            ,
                              m0_980 = m_X             ,
                              m0g1   = m_X * g_X * f1  ,
                              g2og1  = f2/f1           )

flatte2 = Models.Flatte2_pdf ( 'Flatte' , fl_ , 
                               mass   = mass            ,
                               m0_980 = m_X             ,
                               m0g1   = m_X * g_X * f1  ,
                               g2og1  = f2/f1           )

if not ROOT.gROOT.IsBatch() :
    from Ostap.Canvas import getCanvas
    c = getCanvas()

with rootWarning() : 
    br  = breit  .draw () # nbins = 500 )
    fl  = flatte .draw () # nbins = 500 )

##fl2 = flatte2.draw ( nbins = 1000 )

if not ROOT.gROOT.IsBatch() :
    with rootWarning() : 
        br .Draw()
        fl .Draw('same')
    
## fl2.Draw('same')


# ==========================================================================================
# The END 
# ==========================================================================================
