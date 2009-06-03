from Gaudi.Configuration import *
from Configurables import CreateHistDBPages
createHistAlg = CreateHistDBPages("createHistAlg")

# define the layout for this page type
#   this is a list containing one entry for each histogram. each entry is itself
#   a list with four double values giving the position of the histogram
# the structure is:
#   PageLayout => { 
#       histo1 => { Xmin, Ymin, Xmax, Ymax }
#       histo2 => { Xmin, Ymin, Xmax, Ymax }
#   }
# if Xmin < 0 the histogram will be overlayed on the previous histogram.
createHistAlg.PageLayout = []
createHistAlg.PageLayout.append([0.01, 0.51, 0.49, 0.99])
createHistAlg.PageLayout.append([0.51, 0.51, 0.99, 0.99])
createHistAlg.PageLayout.append([0.01, 0.01, 0.49, 0.49])
createHistAlg.PageLayout.append([0.51, 0.01, 0.99, 0.49])

