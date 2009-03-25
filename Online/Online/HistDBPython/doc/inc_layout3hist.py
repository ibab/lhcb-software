from Gaudi.Configuration import *
from Configurables import CreateHistDBPages
createHistAlg = CreateHistDBPages("createHistAlg")

createHistAlg.PageLayout = []
createHistAlg.PageLayout.append([0.01, 0.51, 0.49, 0.99])
createHistAlg.PageLayout.append([0.51, 0.51, 0.99, 0.99])
createHistAlg.PageLayout.append([0.01, 0.01, 0.49, 0.49])

