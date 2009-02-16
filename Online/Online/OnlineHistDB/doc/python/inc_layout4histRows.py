from Gaudi.Configuration import *
from Configurables import STCreateHistDBPages
createHistAlg = STCreateHistDBPages("createHistAlg")

createHistAlg.PageLayout = []
createHistAlg.PageLayout.append([0.01, 0.51, 0.49, 0.99])
createHistAlg.PageLayout.append([0.51, 0.51, 0.99, 0.99])
createHistAlg.PageLayout.append([0.01, 0.01, 0.49, 0.49])
createHistAlg.PageLayout.append([0.51, 0.01, 0.99, 0.49])

