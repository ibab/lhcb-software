##############################################################################
# Option file to run hadronic cross section checks with multiple targets.
# For moreusage informations: https://twiki.cern.ch/twiki/bin/view/LHCb/TargetStudy
# 
# Last modified: Luca Pescatore, 24/11/2014
##############################################################################



import sys
import os
import shutil
import subprocess
import time
import glob
from Target.TargetSummary import *
from Target.TargetSetup import *


template = '''from Target.TargetSetup import *

setup_Targets_pguns("{particle}", {energy}, "{material}", {thick})
setup_Target_GaussJob("{model}",{thick},"{material}",{energy},"{particle}",{nevts:.0f})
'''

def RunTargetJobs(path, models, particlesTodo, energies, materialsTodo, thicks, nevts = 1e4) :

	#Creating option file for analysis with all the options available

	if not os.path.exists(path):
		os.makedirs(path)

	#Starting jobs submission

	log = open('progress.log','w')

	for model in models:

		if not os.path.exists(os.path.join(path,model)):
			os.makedirs(os.path.join(path,model))
		
		log.write('  ----------------------  Using ' + model+'\n')

		for energy in energies:
      
			if not os.path.exists(os.path.join(path,model,"E%iGeV" %energy)):
				os.makedirs(os.path.join(path,model,"E%iGeV" %energy))

			log.write('  -----------------  ' + str(energy) +'GeV\n')

			for thick in thicks:
				
				sourcedir = os.environ['PWD']
				dest = os.path.join(path,model,"E%iGeV" %energy,"T%imm" %thick)
				if not os.path.exists(dest):
					os.makedirs(dest)
         
				log.write('  ------------  ' + str(thick)+'mm\n')

				for material in materialsTodo:
				
					log.write('  -------  ' + material+'\n')
					
					for particle in particlesTodo:
					
						log.write('  --  ' + particle+'\n')
						log.flush()
				
						time.sleep(2)
			
						with open("targets_tmpfile.py","w") as tmp:
							tmp.write(template.format(
										model=model,
										particle=particle,
										material=material,
										energy=energy,
										thick=thick,
										nevts=nevts 
										))
							tmp.flush()    	
							subprocess.call(['gaudirun.py', tmp.name])
							
							os.remove("targets_tmpfile.py")
				
							time.sleep(2)
							for file in glob.glob(r'Multi*.root'):
								if os.path.exists(os.path.join(dest,file)):
									os.remove(os.path.join(dest,file))
								shutil.move(file,dest)


	output = path+"/TargetOutput.root"
	command = 'find {path}/*/*/* -name *.root | xargs hadd -f {output}'.format(path=path,output=output)
	os.system(command)

	p = Plotter(path+"/", path+"/", models, particlesTodo, energies, materialsTodo, thicks)

	log.write('----------------   END  ------------------')
	log.close()






