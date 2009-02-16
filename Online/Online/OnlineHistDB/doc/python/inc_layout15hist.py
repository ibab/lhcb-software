from Gaudi.Configuration import *
from Configurables import STCreateHistDBPages
createHistAlg = STCreateHistDBPages("createHistAlg")

createHistAlg.PageLayout = []
createHistAlg.PageLayout.append([0.01, 0.76, 0.24, 0.99])
createHistAlg.PageLayout.append([0.26, 0.76, 0.49, 0.99])
createHistAlg.PageLayout.append([0.51, 0.76, 0.74, 0.99])
createHistAlg.PageLayout.append([0.76, 0.76, 0.99, 0.99])
createHistAlg.PageLayout.append([0.01, 0.51, 0.24, 0.74])
createHistAlg.PageLayout.append([0.26, 0.51, 0.49, 0.74])
createHistAlg.PageLayout.append([0.51, 0.51, 0.74, 0.74])
createHistAlg.PageLayout.append([0.76, 0.51, 0.99, 0.74])
createHistAlg.PageLayout.append([0.01, 0.26, 0.24, 0.49])
createHistAlg.PageLayout.append([0.26, 0.26, 0.49, 0.49])
createHistAlg.PageLayout.append([0.51, 0.26, 0.74, 0.49])
createHistAlg.PageLayout.append([0.76, 0.26, 0.99, 0.49])
createHistAlg.PageLayout.append([0.01, 0.01, 0.24, 0.24])
createHistAlg.PageLayout.append([0.26, 0.01, 0.49, 0.24])
createHistAlg.PageLayout.append([0.51, 0.01, 0.74, 0.24])

