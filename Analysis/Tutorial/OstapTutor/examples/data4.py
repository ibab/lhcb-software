#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =========================================================================================
import ROOT, math 
from   Ostap.PyRoUts         import * 
from   Ostap.Selectors       import SelectorWithVars 
# ==========================================================================================
from AnalysisPython.Logger   import getLogger 
if '__main__' == __name__ : logger = getLogger( 'OstapTutor/data4')
else                      : logger = getLogger( __name__          )
# ==========================================================================================
logger.info('Build datasets for Y+Charm analysis')
# ==========================================================================================

from OstapTutor.TestVars1  import m_D0 , m_Dp , m_Ds , m_Lc , m_ups
from ccuts   import cuts, pileup
from math    import log10, sin  

def pileup_fun( s ) :
    import math 
    return math.log10( s.c2dtf )

## import trees
from data3 import tD0Y, tDpY, tDsY, tLcY  
variables = [
    ( m_ups     , lambda s : s.mass_y  ) ,
    ( 'c2dtf'   , '#chi^{2}_{DTF}(#Upsilon+C)' , -100 , 1e+8 , lambda s : min(max(s.c2dtf  ,-99.9),1.e+7) ) ,
    ( 'c2dtf_y' , '#chi^{2}_{DTF}(#Upsilon)'   , -100 , 1e+8 , lambda s : min(max(s.c2dtf_y,-99.9),1.e+7) ) ,
    ( 'c2dtf_c' , '#chi^{2}_{DTF}(C)'          , -100 , 1e+8 , lambda s : min(max(s.c2dtf_c,-99.9),1.e+7) ) , 
    ( 'pt_c'    , 'p_{T}(C)'                   ,  1   ,   20 , lambda s : s.pt_C          ) ,
    ( 'y_c'     , 'y(C)'                       ,  2   ,  4.5 , lambda s : s.y_C           ) ,
    ( 'pt_y'    , 'p_{T}(Y)'                   ,  0   ,   20 , lambda s : s.pt_Y          ) ,
    ( 'y_y'     , 'y(Y)'                       ,  2   ,  4.5 , lambda s : s.y_Y           ) ,
    ( 'annMu'   , 'min(PROBNNmu(muons))'       , -1   ,    3 , lambda s : s.minann_mu     ) ,
    ( 'annK'    , 'min(PROBNNk (kaons))'       , -1   ,    3 , lambda s : s.minann_K      ) ,
    ( 'annPi'   , 'min(PROBNNpi(pions))'       , -1   ,    3 , lambda s : s.minann_pi     ) ,
    ( 'year'    , 'year'                       , 2000 , 2020 , lambda s : s.year          ) ,
    ( pileup    ,  pileup_fun ) , 
    ]


sel_D0 = SelectorWithVars (
    variables + [ ( m_D0 , lambda s : s.mass_c  ) ] , 
    selection = cuts + ' %s<=mass_c && mass_c<=%s' % ( m_D0.getMin() , m_D0.getMax() )
    )

sel_Dp = SelectorWithVars (
    variables + [ ( m_Dp , lambda s : s.mass_c  ) ] ,
    selection = cuts + ' %s<=mass_c && mass_c<=%s' % ( m_Dp.getMin() , m_Dp.getMax() )
    )

sel_Ds = SelectorWithVars (
    variables + [ ( m_Ds , lambda s : s.mass_c  ) ] , 
    selection = cuts + ' %s<=mass_c && mass_c<=%s' % ( m_Ds.getMin() , m_Ds.getMax() )
    )

p_pid_vars = [
    ( 'pK_ann'  , 'PROBNNk (p)' , -1 , 3 , lambda s : s.ann_proton_K [0] ) ,
    ( 'pPi_ann' , 'PROBNNpi(p)' , -1 , 3 , lambda s : s.ann_proton_PI[0] ) ,
    ( 'annP'    , 'PROBNNP (p)' , -1 , 3 , lambda s : s.ann_proton   [0] ) 
    ]

sel_Lc = SelectorWithVars (
    variables + [ ( m_Lc , lambda s : s.mass_c  )  ] + p_pid_vars ,
    selection = cuts + ' (ann_proton_K[0]<0.8) && (ann_proton_PI[0]<0.8) && minann_P>0.20 ' + ' %s<=mass_c && mass_c<=%s' % ( m_Lc.getMin() , m_Lc.getMax() ) 
    )


tD0Y.process ( sel_D0 )
ds_D0 = sel_D0.data

tDpY.process ( sel_Dp )
ds_Dp = sel_Dp.data

tDsY.process ( sel_Ds )
ds_Ds = sel_Ds.data

tLcY.process ( sel_Lc )
ds_Lc = sel_Lc.data

# ==========================================================================================
# The END 
# ==========================================================================================
