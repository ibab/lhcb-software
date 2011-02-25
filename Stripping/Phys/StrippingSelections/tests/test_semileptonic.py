#!/usr/bin/env python

from StrippingSelections.StrippingB0q2DplusMuX import B0q2DplusMuXAllLinesConf as builder1

from StrippingSelections.StrippingB0q2DplusMuX import confdict as config_params1

from StrippingSelections.StrippingB2XuMuNu import B2XuMuNuBuilder as builder2

from StrippingSelections.StrippingB2XuMuNu import confdict as config_params2

from StrippingSelections.StrippingB2DMuNuX import B2DMuNuXAllLinesConf as builder3

from StrippingSelections.StrippingB2DMuNuX import confdict as config_params3

from StrippingSelections.StrippingBd2DstarMuNu import Bd2DstarMuNuAllLinesConf as builder4

from StrippingSelections.StrippingBd2DstarMuNu import Bd2DstarMuNuAllLinesConf as builder4

from StrippingSelections.StrippingBd2DstarMuNu import confdict as config_params4


###from StrippingSelections.StrippingBd2DstarTauNu import Bd2DstarTauNuAllLinesConf as builder5

###from StrippingSelections.StrippingBd2DstarTauNu import confdict as config_params5

from StrippingSelections.StrippingDForBSemi import DforBSemiLinesConf as builder6

from StrippingSelections.StrippingDForBSemi import confdict as config_params6



from StrippingUtils import LineBuilderTests

def test_line_builder() :
    ## LineBuilderTests.test_line_builder(builder1, config_params1)
    ##     LineBuilderTests.test_line_builder(builder2, config_params2)
    ##     LineBuilderTests.test_line_builder(builder3, config_params3)
    LineBuilderTests.test_line_builder(builder4, config_params4)
    ##LineBuilderTests.test_line_builder(builder5, config_params5)
    LineBuilderTests.test_line_builder(builder6, config_params6)

if __name__ == '__main__' :
    
    test_line_builder()
