from Gaudi.Configuration import *
from Configurables import CreateHistDBPages
createHistAlg = CreateHistDBPages("createHistAlg")

createHistAlg.PageLayout = [ [0.01, 0.01, 0.99, 0.99] ]
