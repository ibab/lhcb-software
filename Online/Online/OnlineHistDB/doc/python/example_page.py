from Gaudi.Configuration import *
from Configurables import STCreateHistDBPages
createHistAlg = STCreateHistDBPages("createHistAlg")

# ***********************************************
# defining helper variables to create the page 
# and histogram names

# page info
folderBase = "IT/Cosmics/"
folders    = [ "Central/" ]
pageName   = "Last4Cosmics"

# histogram info
histoSet   = "IT-Cluster_Map_Live/" 
histoNames = [ "0", "1", "2", "3"  ] 


# ***********************************************
# setting the options for the algorithm

# PageName is a std::vector<std::string> that contains one 
# entry for each page to be created. 
# The page pathes will be prefixed with BasePath plus a '/'
# (defined in genOpts.py)
createHistAlg.PageNames   = []

# HistoNames is a vector of vectors 
#   std::vector<std::vector<std::string> >
# the structure is [pageNumber][histogram]
# e.g.: HistoNames => {
#            page1 => { histo1A, histo1B }
#            page2 => { histo2A, histo2B }
#       }
# Conditions:
# - each page vector needs to have the same number of
#   entries as the layout definition.
# - the page vectors need to be sorted according to 
#   PageNames above.
createHistAlg.HistoNames = []

for folder in folders:
	createHistAlg.PageNames.append(folderBase+folder+pageName);
	
        folderHistos = []
	for name in histoNames:
                #histogram name: endOfAlgorithmName + histogramSet + variableFolderPart + variableNamePart
		folderHistos.append(folder+histoSet+folder+name)
	createHistAlg.HistoNames.append(folderHistos)


# ***********************************************
# importing common configuration files

# layout definition
importOptions("example_layout.py")

# general options 
importOptions("example_genOpts.py")