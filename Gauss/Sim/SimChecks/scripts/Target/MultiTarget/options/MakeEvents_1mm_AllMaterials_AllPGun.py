### This option generates all pguns but only in blocka of 1mm (all materials)
### Takes approximately 3.5 h 

import sys
import os

sys.path.append(os.environ["PWD"].replace("/options",""))
from Target_CreateEvents import RunTargetJobs

path=os.environ['PWD']+'/TargetOutput' # where you want your output (absolute or relative path)

models=['QGSP_BERT','FTFP_BERT']#any present in the version of Gauss you are using es. 'FTFP_BERT','LHEP'
energies=[1,2,5,10,50,100]#any
materialsTodo=['Al','Be','Si'] # 'Al' 'Be' 'Si'
thicks=[1]  #in mm 1, 5, 10 (only)
particlesTodo=['p','pbar','Piminus', 'Piplus','Kminus', 'Kplus'] # Available: 'Piminus' 'Piplus' 'Kminus' 'Kplus' 'p' 'pbar'

RunTargetJobs(path, models, particlesTodo, energies, materialsTodo, thicks)

