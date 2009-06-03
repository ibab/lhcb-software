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

# defining the pages that should be created 
#   PageNames is a list of strings that contains one entry for each page to be 
#   created. 
#   The page names will be prefixed with PageBase
createHistAlg.PageNames   = []

# define the documentation string for each page
#   PageDoc is a list of strings with the same ordering as PageNames
#   depending on the size of the list, the follwing actions will be taken:
#   - if the list is empty, no documentation will be added. 
#   - if the list has one entry for each page, these will 
#     be used.
#   - otherwise the first documentation will be used for 
#     every page.
createHistAlg.PageDoc   = []

# define the histograms that should be shown on each page 
#   HistoNames is a list of lists. Each inner list is a list of strings 
#   containing the histogram names for one page.
# the structure is [pageNumber][histogram]
#   e.g.: HistoNames => {
#            page1 => { histo1A, histo1B }
#            page2 => { histo2A, histo2B }
#         }
# Conditions:
# - each page vector needs to have the same number of entries as the layout 
#   definition.
# - the page vectors need to be sorted according to PageNames above.
createHistAlg.HistoNames = []

# define the pages histograms should be linked with
#   HistoLinks is a dictionary with structure { string : string }
#   *) the first string is the name of a histogram in HistoNames.
#   *) the second string is the full path of the page that should be displayed 
#      when one clicks on the histogram.
#   The page to display is a property of the histogram, so only one page can 
#   be defined per histogram.
createHistAlg.HistoLinks = {}

# loop filling the variables defined above
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
importOptions("exampl_genCreate.py")
