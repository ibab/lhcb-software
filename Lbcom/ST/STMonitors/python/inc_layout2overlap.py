from Gaudi.Configuration import *
from Configurables import CreateHistDBPages
createHistAlg = CreateHistDBPages("createHistAlg")

createHistAlg.PageLayout = []
createHistAlg.PageLayout.append([0.01, 0.01, 0.99, 0.99])
createHistAlg.PageLayout.append([-1.0, 0.0, 0.0, 0.0])

