# To run this script you need to set JOBOPTSEARCHPATH
# to the correct path

from Gaudi.Configuration import *
from Configurables import CreateHistDBPages
createHistAlg = CreateHistDBPages("createHistAlg")

tae        = "Central"
folderBase = "Summary"
pageName   = "ClusterSize"

histoName  = "TT-Cluster_Size" 


createHistAlg.PageNames   = []
createHistAlg.HistoNames = []

createHistAlg.PageNames.append( "/" + folderBase + "/" + pageName);
folderHistos = [ tae+"/"+histoName+"/"+tae ]
createHistAlg.HistoNames.append(folderHistos)


importOptions("inc_layout1hist.py")
importOptions("incTT_genOpts.py")
