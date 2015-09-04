### This option generates all pguns but only in blocka of 1mm (all materials)
### Takes approximately 3.5 h 

import sys
import os
import re

from Target_CreateEvents import RunTargetJobs
from optparse import OptionParser

parser = OptionParser()
parser.add_option("--physList", default=None, dest="physList", help="Specific single Physics List to use by default does both FTFP_BERT and QGSP_BERT" )
(opts, args) = parser.parse_args()

version = re.search("GAUSS_v(.*?)/",os.environ["GAUSSROOT"]).groups(0)[0]
path=os.environ['WORK']+'/TargetOutput_v'+version # where you want your output (absolute or relative path)


models=['FTFP_BERT','QGSP_BERT']#any present in the version of Gauss you are using e.g. 'FTFP_BERT'
if opts.physList :
	models = [ opts.physList ]

energies=[1,2,5,10,50,100]#any
materials=['Al','Be','Si'] # 'Al' 'Be' 'Si'
thicks=[1]  #in mm 1, 5, 10 (only)
pguns=["p","pbar","Kplus","Kminus","Piplus","Piminus"] # Available: 'Piminus' 'Piplus' 'Kminus' 'Kplus' 'p' 'pbar'

RunTargetJobs(path, models, pguns, energies, materials, thicks)

from ROOT import *
from Target.TargetPlots import Plot

plots = [ "RATIO_TOTAL", "RATIO_INEL", "TOTAL", "INEL", "EL", "MULTI", "MULTI_NCH", "MULTI_GAMMA", "ASYM_INEL" ]

file = TFile(path+"/TargetsPlots.root")
dataTree = file.Get("summaryTree")
	
os.system("mkdir -p "+path+"/Kaons")
os.system("mkdir -p "+path+"/Protons")
os.system("mkdir -p "+path+"/Pions")

for p in plots :
	Plot( dataTree, "energy", p, path, models , pguns , materials , 2 , thicks[0], True )
	if "p" in pguns and "pbar" in pguns :
		Plot( dataTree, "energy", p, path+"/Protons", models , ["p","pbar"] , materials , 2 , thicks[0], True )
	if "Kplus" in pguns and "Kminus" in pguns :
		Plot( dataTree, "energy", p, path+"/Kaons", models , ["Kplus","Kminus"] , materials , 2 , thicks[0], True )
	if "Piplus" in pguns and "Piminus" in pguns :
		Plot( dataTree, "energy", p, path+"/Pions", models , ["Piplus","Piminus"] , materials , 2 , thicks[0], True )


