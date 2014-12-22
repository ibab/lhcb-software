#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =========================================================================================
import ROOT
from   Ostap.PyRoUts         import * 
# ==========================================================================================
from AnalysisPython.Logger   import getLogger 
if '__main__' == __name__ : logger = getLogger( 'work_CY/cuts')
else                      : logger = getLogger( __name__      )
# ==========================================================================================
logger.info('Define some basic cuts for Y+charm analysis')
# ==========================================================================================
from OstapTutor.TestVars1  import m_D0 , m_Dp , m_Ds , m_Lc, m_Y


hD0  = m_D0.histo ( 100 )
hDp  = m_Dp.histo (  90 )
hDs  = m_Ds.histo ( 140 )
hLc  = m_Lc.histo (  90 )
hY   = m_Y .histo ( 400 ) 

pileup = ROOT.RooRealVar ( 'pileup' , 'log10(chi2_DTF)' , -2 , 6 )

hD0Y = ROOT.TH2D( hID () , '' , 20 , 9.0 , 11.0 , 16 , 1.82  , 1.90  ) ; hD0Y.Sumw2() 
hDpY = ROOT.TH2D( hID () , '' , 20 , 9.0 , 11.0 , 16 , 1.82  , 1.90  ) ; hDpY.Sumw2() 
hDsY = ROOT.TH2D( hID () , '' , 20 , 9.0 , 11.0 , 16 , 1.92  , 2.00  ) ; hDsY.Sumw2()
hLcY = ROOT.TH2D( hID () , '' , 20 , 9.0 , 11.0 , 16 , 2.245 , 2.325 ) ; hLcY.Sumw2()

cuts_Y  = ROOT.TCut( ' -0.1<=c2dtf_y &&   c2dtf_y<=5   ' ) 
cuts_Y +=            '  2<=eta_mu[0] && eta_mu[0]<=4.5 '
cuts_Y +=            '  2<=eta_mu[1] && eta_mu[1]<=4.5 '
cuts_Y +=            ' 10<=  p_mu[0] &&   p_mu[0]<=400 '
cuts_Y +=            ' 10<=  p_mu[1] &&   p_mu[1]<=400 '
cuts_Y +=            '  1<= pt_mu[0] &&  pt_mu[0]<=25  '
cuts_Y +=            '  1<= pt_mu[1] &&  pt_mu[1]<=25  '


cuts_C    = ROOT.TCut ( '   -0.1<=c2dtf_c && c2dtf_c<=5  ' ) 
cuts_C   +=                ' 1.0<= pt_C   && pt_C   <20  '
cuts_C   +=                '   2<=  y_C   && y_C    <4.5 '
cuts_C   +=                '   2<=  y_C   && y_C    <4.5 '


cuts_PID  = ROOT.TCut ( '  minann_mu>0.5 && mindll_mu>0 ' )  
cuts_PID += ' maxTrGh_track<0.3 '

cuts      = ROOT.TCut ( '  %s<= m_Y && m_Y <=%s' % ( m_Y.getMin() , m_Y.getMax() ) ) 
cuts     += cuts_Y
cuts     += cuts_C
cuts     += cuts_PID


cuts += ' -0.1<=c2dtf   && c2dtf  <=5 '
logger.info    ( 'Anti-pileup cuts is active' )
##logger.warning ( '*NO* anti-pileup cut!'        ) 

tos  = ROOT.TCut ( '(2==( ups_l0tos&2))' ) & '(2==( ups_l1tos&2))' & '(2==( ups_l2tos&2))'
tos1 = ROOT.TCut ( '(2==(ups1_l0tos&2))' ) & '(2==(ups1_l1tos&2))' & '(2==(ups1_l2tos&2))'

logger.warning('No trigger is applied!')
## cuts += tos
## logger.warning('Use TOS (Y) events only!' )


# ==========================================================================================
# The END 
# ==========================================================================================


