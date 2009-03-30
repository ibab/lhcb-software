from Gaudi.Configuration import *
from Configurables import CreateHistDBPages
createHistAlg = CreateHistDBPages("createHistAlg")

# ***********************************************
# defining helper variables to create the page 
# and histogram names

# page info
folderBase = "Expert/perTell/tell"
page   = "Noise"

# histogram info
histoSet1  = "Raw/raw_$tell"
histoSet2  = "CMS/cms_$tell"
histoNum   = range(1, 49) 


# ***********************************************
# setting the options for the algorithm

# PageName is a std::vector<std::string> that contains one 
# entry for each page to be created. 
# The page pathes will be prefixed with PageBase
# (defined in genCreate.py)
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

# HistoLinks is a map
#   std::map<std::string, std::string>
# *) the first string is the name of a histogram in 
#    HistoNames.
# *) the second string is the full path of the page 
#    that should be displayed when one clicks on 
#    the histogram.
#    (this is not supported in the presenter at 
#    2009-03-30)
# The page to display is a property of the histogram,
# so only one page can be defined per histogram.
createHistAlg.HistoLinks = {}

for num in histoNum:
	id = str(num)
	createHistAlg.PageNames.append("/"+folderBase+id+"/"+page)
	
        folderHistos = []
	folderHistos.append(histoSet1+id)
	folderHistos.append(histoSet2+id)
	createHistAlg.HistoNames.append(folderHistos)

        # this is useless and just for demonstartion
	createHistAlg.HistoLinks[histoSet1+id] = "/TT/Expert/NoiseAllTell1s/"

# ***********************************************
# importing common configuration files

# layout definition
importOptions("inc_layout2overlap.py")

# general options
importOptions("example_genCreate.py")
