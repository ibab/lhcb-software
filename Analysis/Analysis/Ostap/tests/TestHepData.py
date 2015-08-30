#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$ 
# =============================================================================
## @file TestHepData.py
#
#  tests conversion to HepDATA format
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-05-11
# 
#                    $Revision: 193299 $
#  Last modification $Date: 2015-08-06 14:06:19 +0200 (Thu, 06 Aug 2015) $
#                 by $Author: ibelyaev $
# =============================================================================
""" Tests conversion to HepDATA format
"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-05-10"
__all__     = ()  ## nothing to be imported 
# =============================================================================
import ROOT
from   Ostap.PyRoUts import VE 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger
if '__main__' == __name__  or '__builtin__'  == __name__ : 
    logger = getLogger ( 'Ostap.TestHepData' )
else : 
    logger = getLogger ( __name__ )
# =============================================================================
logger.info ( 'Test converisons to HepDATA format')
# =============================================================================
from Ostap.HepDATA import HepDataFile,HepData



## create the histogra
h1 = ROOT.TH1D('h1','',5,0,10)
h1[1] = VE( 100 , 15**2 )
h1[2] = VE(  50 , 10**2 )
h1[3] = VE(  10 ,  2**2 )
h1[4] = VE(   1 ,  1**2 )
h1[5] = VE(   0 ,  1**2 )

meta = {
    'location'  : 'Figure 11 (a), red filled circles' ,
    'dscomment' : r"""Normalized differential cross-section
    $d\ln\sigma(pp\rightarrowJ/\psi D^0 X)/dp_T(J/\psi)$
    for $2<y(J/\psi)<4$, $p_T(J/\psi)<12$ GeV/$c$,
    $2<y(D^0)<4$, $3<p_T(D^0)<12$ GeV/$c$ region""", 
    'obskey'    : 'DLN(SIG)/DPT',
    'qual'      : [ 'D0 PT IN GEV : 3.0 TO 12.0',
                    'D0 YRAP : 2.0 TO 4.0',
                    'J/PSI PT IN GEV : 2.0 TO 12.0',
                    'J/PSI YRAP : 2.0 TO 4.0', 
                    'RE : P P --> J/PSI D0',
                    'SQRT(S) IN GEV : 7000.0' ] ,
    'yheader'   : 'DLN(SIG)/DPT IN 1/500 MEV',
    'xheader'   : 'PT IN GEV',
    }
ds = HepData (
    h1 ,
    syst1 = '0.01:detector' ,
    syst2 = lambda i : "%s:ququ" % i ,
    syst3 = lambda i : "%s:last" % h1[i+1].error() , 
    **meta )

gr = h1.toGraph2(0.1)
ds2 = HepData (
    gr ,
    syst1 = '0.01:detector' ,
    syst2 = lambda i : "%s:ququ" % i ,
    syst3 = lambda i : "%s:last" % h1[i+1].error() , 
    **meta )    

meta = { 'author' : 'AAIJ' , 'detector' : 'LHCb' } 
with HepDataFile( **meta ) as hf :
    hf << ds 
    hf << ds2


logger.info( 'Check the file HepDATA.txt' )
with open('HepDATA.txt','r') as f :
    for line in f.readlines() : print line[:-1]

        
# =============================================================================
# The END 
# =============================================================================
