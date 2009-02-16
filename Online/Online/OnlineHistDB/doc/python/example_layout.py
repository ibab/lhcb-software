# this file defines the layout for a page
# (this example is the same as layout4histRows.py)

# needed in all option files
from Gaudi.Configuration import *
from Configurables import STCreateHistDBPages
createHistAlg = STCreateHistDBPages("createHistAlg")

# the actual layout definition
# PageLayout is std::vector<std::vector<int> >
# with structure:
# PageLayout => { 
#     histo1 => { Xmin, Ymin, Xmax, Ymax }
#     histo2 => { Xmin, Ymin, Xmax, Ymax }
# }
createHistAlg.PageLayout = []
createHistAlg.PageLayout.append([0.01, 0.51, 0.49, 0.99])
createHistAlg.PageLayout.append([0.51, 0.51, 0.99, 0.99])
createHistAlg.PageLayout.append([0.01, 0.01, 0.49, 0.49])
createHistAlg.PageLayout.append([0.51, 0.01, 0.99, 0.49])

