
#!/usr/bin/env python
# =============================================================================

from StrippingUtils import LineBuilderTests

config_params = {
    'DiMuonAndGammaPrescale' :   1.00 ,
    'DoubleDiMuonPrescale'   :   1.00 , 
    'PhotonCuts'             : ' PT > 4.0 * GeV  '  
    }


from StrippingSelections.StrippingCharmAssociative import StrippingCharmAssociativeConf as builder

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    
    test_line_builder()
