#!/usr/bin/env python

from StrippingUtils import LineBuilderTests



from StrippingSelections.StrippingHeavyBaryons import HeavyBaryonsConf

# Yasmine Amhis
configHeavyBaryons     = { 'TRCHI2DOF'             :       4.,
                          'PionPIDK'              :       5.,
                          'KaonPIDK'              :      -5.,
                          'DLSForLongLived'       :       5.,
                          'XiMassWindow'          :      30.,
                          'XistarMassWindow'      :      150.,
                          'OmegaMassWindow'       :      30.,
                          "XibminusMassWindow"    :      300.,
                          "XibzeroMassWindow"     :      500.,
                          "OmegabminusMassWindow" :      500.
                          }


def test_line_builder() :
    LineBuilderTests.test_line_builder(HeavyBaryonsConf, configHeavyBaryons)
    b = HeavyBaryonsConf( "HeavyBaryons", configHeavyBaryons )
    for line in b.lines():
        print line.outputLocation()
   
   
	''' 
    LineBuilderTests.test_line_builder(StrippingBs2KstKstConf, configBs2KstKst )
    b = StrippingBs2KstKstConf( "Bs2KstKst", configBs2KstKst )
    for line in b.lines():
        print line.outputLocation()
   	'''


if __name__ == '__main__' :
    test_line_builder()
