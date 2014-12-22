#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =========================================================================================
# $Id:$ 
# =========================================================================================
## @file
#  Define some cuts for search of signal for associative production of Y+J/psi  
#  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
#
#                    $Revision:$
#  Last modification $Date:$
#                 by $Author:$
# =========================================================================================
""" Define some cuts for search of signal for associative production of Y+J/psi  
"""
# =========================================================================================
import ROOT
from   Ostap.PyRoUts import *
from   AnalysisPython.Logger import getLogger
# =========================================================================================
if '__main__' == __name__ : logger = getLogger('OstapTutor/ycuts')
else                      : logger = getLogger( __name__         )
# ==========================================================================================

from OstapTutor.TestVars1  import m_psi,m_ups 


cuts_psi1 = ROOT.TCut ( "%s<=mass1 && mass1<=%s" % m_psi.minmax() ) & "c2dtf1<5" & "0<=c2dtf1" 
cuts_psi2 = ROOT.TCut ( "%s<=mass2 && mass2<=%s" % m_psi.minmax() ) & "c2dtf2<5" & "0<=c2dtf2" 
cuts_ups1 = ROOT.TCut ( "%s<=mass1 && mass1<=%s" % m_ups.minmax() ) & "c2dtf1<5" & "0<=c2dtf1" 
cuts_ups2 = ROOT.TCut ( "%s<=mass2 && mass2<=%s" % m_ups.minmax() ) & "c2dtf2<5" & "0<=c2dtf2" 

cuts_psi  = cuts_psi1 | cuts_psi2
cuts_ups  = cuts_ups1 | cuts_ups2

cuts_1    = cuts_psi1 & cuts_ups2
cuts_2    = cuts_ups1 & cuts_psi2

pair      = cuts_1 | cuts_2
prompt    = ROOT.TCut ( "0<= c2dtf"      ) & "c2dtf<5"
mupid     = ROOT.TCut ( "0.5<=minann_mu" ) & "maxTrGh_track<0.3" 

cuts = prompt & mupid & pair 


print 'PAIR   cuts: ', pair
print 'PROMPT cuts: ', prompt
print 'PID    cuts: ', mupid  

# ==========================================================================================
## The END 
# ==========================================================================================
