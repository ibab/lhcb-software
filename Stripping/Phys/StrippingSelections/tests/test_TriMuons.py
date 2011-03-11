#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

config_params = {
    'GoodMuons'    : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 5  ) & ( BPVIPCHI2 () >  6 ) " ,
    'GoodMuonsForBc' : " ( BPVIPCHI2 () >  9 ) " ,
    'TightMuons'   : " ( PT > 1.5 * GeV ) & ( BPVIPCHI2 () > 16 ) " ,
    'HLT'          : " HLT_PASS_RE('(Hlt1|Hlt2).*Mu.*Decision') " , 
    '3mu-Prescale' : 1.00 , 
    'Bc-Prescale'  : 1.00 , 
    'Tau-Prescale' : 1.00 , 
    'Preambulo'    : [
    'chi2vx = VFASPF(VCHI2) '                               ,
    "from GaudiKernel.PhysicalConstants import c_light"     ,
    "ctau    =                    BPVLTIME ( ) * c_light "  ,
    "ctauBc  = PDGM('B_c+') / M * BPVLTIME ( ) * c_light "  
    ]
    }


from StrippingSelections.StrippingTriMuons import StrippingTriMuonsConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
