import sys
import os
from ROOT import *

sys.path.append(os.environ["PWD"].replace("/options",""))
from TargetPlots import Plot

plots = [ "RATIO_TOTAL", "RATIO_INEL", "TOTAL", "INEL", "EL", "MULTI", "MULTI_NCH", "MULTI_GAMMA" ]

models = ["FTFP_BERT"]  # any you generated, by default "FTFP_BERT"
thicks = [1] #1,5,10
materials = ["Al"] ### Al,Si,Be
energies = [1,5,10,100] # any you generated, by default 1,5,10,100
pguns = ["p","pbar","Kplus","Kminus","Piplus","Piminus"] # "p","pbar","Kplus","Kminus","Piplus","Piminus"

path=os.environ['PWD']+'/TargetOutput'

file = TFile(path+"/TargetsPlots.root")
dataTree = file.Get("summaryTree")
	
os.system("mkdir -p "+path+"/Kaons")
os.system("mkdir -p "+path+"/Protons")
os.system("mkdir -p "+path+"/Pions")

for p in plots :
	Plot( dataTree, "energy", p, path, models , pguns , materials , 2 , 1, True )
	Plot( dataTree, "energy", p, path+"/Protons", models , ["p","pbar"] , materials , 2 , 1, True )
	Plot( dataTree, "energy", p, path+"/Kaons", models , ["Kplus","Kminus"] , materials , 2 , 1, True )
	Plot( dataTree, "energy", p, path+"/Pions", models , ["Piplus","Piminus"] , materials , 2 , 1, True )



