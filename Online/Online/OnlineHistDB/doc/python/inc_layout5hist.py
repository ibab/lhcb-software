from Gaudi.Configuration import *
from Configurables import STCreateHistDBPages
createHistAlg = STCreateHistDBPages("createHistAlg")

createHistAlg.PageLayout = []
createHistAlg.PageLayout.append([0.01, 0.676667, 0.323333, 0.99])
createHistAlg.PageLayout.append([0.343333, 0.676667, 0.656667, 0.99])
createHistAlg.PageLayout.append([0.676667, 0.676667, 0.99, 0.99])
createHistAlg.PageLayout.append([0.01, 0.343333, 0.323333, 0.656667])
createHistAlg.PageLayout.append([0.343333, 0.343333, 0.656667, 0.656667])