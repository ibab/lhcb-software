### This option generates all pguns but only in blocks of Al (all thicknesses)
### Takes approximately 3.5 h 

import sys
import os

sys.path.append(os.environ["PWD"].replace("/options",""))
from Target_CreateEvents import RunTargetJobs

path=os.environ['PWD']+'/TargetOutput' # where you want your output (absolute or relative path)

models=['FTFP_BERT']#any present in the version of Gauss you are using es. 'FTFP_BERT'
energies=[1,2,5,10,50,100]#any
materials=['Al'] # 'Al' 'Be' 'Si'
thicks=[1,5,10]  #in mm 1, 5, 10 (only)
pguns=['p','pbar','Piminus', 'Piplus','Kminus', 'Kplus'] # Available: 'Piminus' 'Piplus' 'Kminus' 'Kplus' 'p' 'pbar'

RunTargetJobs(path, models, pguns, energies, materials, thicks)

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



