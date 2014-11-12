##############################################################################
# Option file to run hadronic cross section checks with multiple targets.
# For moreusage informations: https://twiki.cern.ch/twiki/bin/view/LHCb/TargetStudy
# 
# Last modified: Luca Pescatore, 8/02/2014
##############################################################################



import sys
import os
import shutil
import fileinput
import string
import subprocess
from TargetSummary import *



#STARTING PROGRAM DO NOT EDIT AFTER THIS POINT


def createTemplateOptionFile(path,models,particlesTodo,energies,materialsTodo,thicks) :

    opt_file = open(path+"/options.txt","w")

    opt_file.write('\n\n#Set path where data tuples are stored')
    opt_file.write('\n\ninputPath: ' + path)
    opt_file.write('\n\n#Set path where you want your output')
    opt_file.write('\n\noutputPath: ./')
    opt_file.write('\n\n#Select with plot you want to produce: TOTAL (total cross section vs energy), INEL (inelastic cross section vs energy), MULTI (multiplicity vs energy)')
    opt_file.write('\n\nfinalPlot: TOTAL')
    opt_file.write('\n\n#Set "plotdata" to 1 if you want to plot on your graphs COMPASS data, where available (p, pbar in Al and Be total cross section), in caso of ratios this will plot the PDG ratios ')
    opt_file.write('\n\nplotData: 0')
    opt_file.write('\n\n#Set "Ratios" to 1 if you want the ratio between two particle guns, useful to make ratios particle/antiparticle\n#NB: ATTENTION!! If doRatio = 1 in order for this to work you have to list the pguns in sequence "particle, antiparticle, particle, antiparticle, ecc"\n#NB: Ratio will be done only for first material and thickness')
    opt_file.write('\n\ndoRatios: 0')
    opt_file.write('\n\n#Set "print" to 1 if you want to print histograms with PDG ID od daughters, 0 if not (all histograms are saved in rootfiles anyway)')
    opt_file.write('\n\nprint: 0')

    opt_file.write("\n\n#############################################################      GENERATED OPTIONS      ##########################################")	
    opt_file.write('\n\n#Set the models to use\n#N.B.:No more than 2 models per analysis if you put more the rest will be ignored')
    opt_file.write('\n\nModels: ')
    for model in models:
       opt_file.write( model + " ")

    opt_file.write('\n\n#Set the particle guns you want to use: "Piplus", "Piminus", "Kplus", "Kminus", "p", "pbar" are available')
    opt_file.write('\n\nParticleGuns: ')
    for particle in particlesTodo:
       opt_file.write( particle + " ")

    opt_file.write('\n\n#Set the energies of your particles in GeV: any energy available, write just number without units')
    opt_file.write('\n\nEnergies: ')
    for energy in energies:
       opt_file.write( str(energy) + " ")

    opt_file.write('\n\n#Set the materials in which you want to fire your particles: "Al", "Be", "Cu", "Si", "Pb" are available')
    opt_file.write('\n\nMaterials: ')
    for material in materialsTodo:
       opt_file.write( material + " ")
   
    opt_file.write('\n\n#Set the thicknesses of this material in mm, write just number without units')
    opt_file.write('\n\nThicknesses: ')
    for thick in thicks:
       opt_file.write( str(thick) + " ")

    opt_file.close()

    


particles = {'Piminus': '-211', 'Piplus': '211', 'Kminus': '-321', 'Kplus': '321', 'p': '2212', 'pbar': '-2212'}
Zplane = {'Al': {1: '200*mm', 5: '400*mm', 10: '600*mm'}, 'Be': {1: '800*mm', 5: '1000*mm', 10: '1200*mm'}, 'Si': {1: '1400*mm', 5: '1600*mm', '10': '1800*mm'}}
Zorig = {'Al': {1: '100*mm', 5: '300*mm', 10: '500*mm'}, 'Be': {1: '700*mm', 5: '900*mm', 10: '1100*mm'}, 'Si': {1: '1300*mm', 5: '1500*mm', '10': '1700*mm'}}


def RunTargetJobs(path, models, particlesTodo, energies, materialsTodo, thicks) :

	#Creating option file for analysis with all the options available

	if not os.path.exists(path):
		os.makedirs(path)

	createTemplateOptionFile(path,models,particlesTodo,energies,materialsTodo,thicks)

	#Starting jobs submission

	log = open('logfile','w')

	for model in models:

		if not os.path.exists(path+"/"+model):
			os.makedirs(path+"/"+model)
		
		log.write('  ----------------------  Using ' + model+'\n')

		for energy in energies:
      
			if not os.path.exists(path+"/"+model+"/E"+str(energy)+"GeV"):
				os.makedirs(path+"/"+model+"/E"+str(energy)+"GeV")

			log.write('  -----------------  ' + str(energy) +'GeV\n')

			for thick in thicks:
				
				if not os.path.exists(path+"/"+model+"/E"+str(energy)+"GeV/T"+str(thick)+"mm"):
					os.makedirs(path+"/"+model+"/E"+str(energy)+"GeV/T"+str(thick)+"mm")
         
			 	log.write('  ------------  ' + str(thick)+'mm\n')

				for material in materialsTodo:
				
					log.write('  -------  ' + material+'\n')
					
					for particle in particlesTodo:
					
						log.write('  --  ' + particle+'\n')
						log.flush()
					
						inputfile = "TargetMaterialGunMultiTargetLocalTemporary.py"
						command = 's/PdgCode.*/PdgCode = ' + particles[particle] + '/'
						sub = subprocess.call(['sed', '-i', command, inputfile])
						command = 's|MaterialEval.ModP = .*|MaterialEval.ModP = ' + str(energy) + ' * GeV|'
						sub = subprocess.call(['sed', '-i', command, inputfile])
						command = 's|ParticleGun.MaterialEval.ZPlane =.*|ParticleGun.MaterialEval.ZPlane = ' + Zplane[material][thick] + '|'
						sub = subprocess.call(['sed', '-i', command, inputfile])
						command = 's|ParticleGun.MaterialEval.Zorig =.*|ParticleGun.MaterialEval.Zorig = ' + Zorig[material][thick] + '|'
						sub = subprocess.call(['sed', '-i', command, inputfile])
					
						inputfile = "Gauss-Job-MultiTarget-MultiTemporayOptionsFile.py"
						command = 's/Multi.*.root/Multi_' + particle + '_in' + material + '.root/'
						sub = subprocess.call(['sed', '-i', command, inputfile])
						command = 's|physList = .*|physList = \''+model+'\'|'
						sub = subprocess.call(['sed', '-i', command, inputfile])
						command = 's|targetThick = .*|targetThick = '+str(thick)+'|'
						sub = subprocess.call(['sed', '-i', command, inputfile])
						command = 's|targetMat = .*|targetMat = \''+material+'\'|'
						sub = subprocess.call(['sed', '-i', command, inputfile])
						command = 's|projID = .*|projID = '+particles[particle]+'|'
						sub = subprocess.call(['sed', '-i', command, inputfile])
						command = 's|projEng = .*|projEng = '+str(energy)+'|'
						sub = subprocess.call(['sed', '-i', command, inputfile])
					
						#sub = subprocess.call(['gaudirun.py', 'Gauss-Job-MultiTarget-MultiTemporayOptionsFile.py' ])
					
			dest = path+"/"+model+"/E"+str(energy)+"GeV/T"+str(thick)+"mm/"
			sourcedir = os.environ['PWD']
			source = os.listdir(sourcedir)
			
			for file in source:
				if file.endswith(".root"):
					if os.path.exists(dest+'/'+file):
						os.remove(dest+'/'+file)
						shutil.move(file,dest)

	output = path+"/TargetOutput.root"
	command = 'find '+path+'/*/*/* -name *.root | xargs hadd -f ' + output
	#os.system(command)

	p = Plotter(path+"/", path+"/", models, particlesTodo, energies, materialsTodo, thicks)

	log.write('----------------   END  ------------------')
	log.close()






