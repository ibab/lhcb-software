### This option generates all pguns in blocks of all available materials and thicknesses
### Takes approximately 12 h 

import sys
import os

sys.path.append(os.environ["PWD"].replace("/options",""))
from Target_CreateEvents import RunTargetJobs
from optparse import OptionParser

parser = OptionParser()
parser.add_option("--parallel", default=False, dest="parallel", action='store_true',help="If you want to parallelise the particle guns jobs")
parser.add_option("--physList", default=None, dest="physList", help="Specific single Physics List to use by default does both FTFP_BERT and QGSP_BERT" )
(opts, args) = parser.parse_args()

path=os.environ['PWD']+'/TargetOutput' # where you want your output (absolute or relative path)

models=['FTFP_BERT','QGSP_BERT']#any present in the version of Gauss you are using e.g. 'FTFP_BERT'
if opts.physList :
	models = [ opts.physList ]

energies=[1,2,5,10,50,100]#any
materials=['Al','Be','Si'] # 'Al' 'Be' 'Si'
thicks=[1,5,10]  #in mm 1, 5, 10 (only)
pguns=["p","pbar","Kplus","Kminus","Piplus","Piminus"] # Available: 'Piminus' 'Piplus' 'Kminus' 'Kplus' 'p' 'pbar'

RunTargetJobs(path, models, pguns, energies, materials, thicks, parallel=opts.parallel)

from ROOT import *
from TargetPlots import Plot

plots = [ "RATIO_TOTAL", "RATIO_INEL", "TOTAL", "INEL", "EL", "MULTI", "MULTI_NCH", "MULTI_GAMMA" ] 

file = TFile(path+"/TargetsPlots.root")
dataTree = file.Get("summaryTree")
	
os.system("mkdir -p "+path+"/Kaons")
os.system("mkdir -p "+path+"/Protons")
os.system("mkdir -p "+path+"/Pions")

for t in thicks :

	for p in plots :
	
		Plot( dataTree, "energy", p, path, models , pguns , materials , 2 , t, True )
		Plot( dataTree, "energy", p, path+"/Protons", models , ["p","pbar"] , materials , 2 , t, True )
		Plot( dataTree, "energy", p, path+"/Kaons", models , ["Kplus","Kminus"] , materials , 2 , t, True )
		Plot( dataTree, "energy", p, path+"/Pions", models , ["Piplus","Piminus"] , materials , 2 , t, True )



