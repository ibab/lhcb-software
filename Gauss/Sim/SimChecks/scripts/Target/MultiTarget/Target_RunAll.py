import sys
import os
import shutil
import fileinput
import string
import subprocess



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

    opt_file.write('\n\n#Set the models to use\nN.B.:No more than 2 models per analysis if you put more the rest will be ignored')
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
       opt_file.write( energy + " ")

    opt_file.write('\n\n#Set the materials in which you want to fire your particles: "Al", "Be", "Cu", "Si", "Pb" are available')
    opt_file.write('\n\nMaterials: ')
    for material in materialsTodo:
       opt_file.write( material + " ")
   
    opt_file.write('\n\n#Set the thicknesses of this material in mm, write just number without units')
    opt_file.write('\n\nThicknesses: ')
    for thick in thicks:
       opt_file.write( thick + " ")

    opt_file.close()
    
    
#if __name__=='__main__':
#    path=os.environ['PWD']+'/test'
#    models=['QGSP_BERT']#,'LHEP','FTFP_BERT'] #any present in the version of Gauas you are using
#    energies=['1']#, '5', '10', '50', '100'] #any
#    materialsTodo=['Al']#,'Be','Si'] # 'Al' 'Be' 'Si'
#    thicks=['1']#,'5']  #in mm 1, 5, 10
#    particlesTodo=['Piminus', 'Piplus']#, 'Kminus', 'Kplus'] # Available: 'Piminus' 'Piplus' 'Kminus' 'Kplus' 'p' 'pbar'
#    createTemplateOptionFile(path,models,particlesTodo,energies,materialsTodo,thicks)


    

path=os.environ['PWD']+'/TargetOutput' # where you want your output (absolute or relative path)
workpath = os.environ['PWD']#+'../options/General/target'  # where is youroptions file


useganga = False  #Do you plan to run this on ganga or locally?
myversion = 'v46r5' #needed only if you use ganga, running locally you define the verson when you setup gauss

models=['QGSP_BERT']#,'FTFP_BERT','LHEP'] #any present in the version of Gauas you are using
energies=['1','5']#, '5', '10', '50', '100'] #any
materialsTodo=['Al'] # 'Al' 'Be' 'Si'
thicks=['1']  #in mm 1, 5, 10
#particlesTodo=['p', 'pbar', 'Piminus', 'Piplus', 'Kminus', 'Kplus'] # Available: 'Piminus' 'Piplus' 'Kminus' 'Kplus' 'p' 'pbar'
particlesTodo=['Piminus']#, 'Kminus', 'p', 'pbar']


#STARTING PROGRAM DO NOT EDIT AFTER THIS POINT


particles = {'Piminus': '-211', 'Piplus': '211', 'Kminus': '-321', 'Kplus': '321', 'p': '2212', 'pbar': '-2212'}
Zplane = {'Al': {'1': '200*mm', '5': '400*mm', '10': '600*mm'}, 'Be': {'1': '800*mm', '5': '1000*mm', '10': '1200*mm'}, 'Si': {'1': '1400*mm', '5': '1600*mm', '10': '1800*mm'}}
Zorig = {'Al': {'1': '100*mm', '5': '300*mm', '10': '500*mm'}, 'Be': {'1': '700*mm', '5': '900*mm', '10': '1100*mm'}, 'Si': {'1': '1300*mm', '5': '1500*mm', '10': '1700*mm'}}


#Creating option file for analysis with all the options available

if not os.path.exists(path):
   os.makedirs(path)

createTemplateOptionFile(path,models,particlesTodo,energies,materialsTodo,thicks)


#Defining function to run on ganga

def RunOnGanga(workpath, model, energy, thick, material, particle) :

    gs=Gauss(version = myversion,
    platform = 'x86_64-slc5-gcc43-opt',
    user_release_area = os.environ['HOME']+'/cmtuser',
    optsfile = [ workpath + '/Gauss-Job-MultiTarget-MultiTemporayOptionsFile.py', workpath + '/TargetMaterialGunMultiTargetLocalTemporary.py' ] )

    jobtreedir = workpath
    jobtree.mkdir(jobtreedir)
    jobtree.cd(jobtreedir)

    d=Dirac()
    i=Interactive()
    
    jobname = model+'-E'+energy+'GeV-T'+thick+'mm-'+material+'-'+particle
    j=Job(name=jobname, application=gs,backend=d)
      
    jobtree.add(j)
    j.submit()
     
    nomefile = 'Multi_' + particle + '_in' + material + '.root'
    curfile = GetJobFolder(j.id) + '/output/' + nomefile
    link_name = path + '/' + model + '/E' + energy + 'GeV/T' + thick + 'mm/' + nomefile

    if os.path.exists(link_name) :
        os.remove(link_name)
        os.symlink(curfile, link_name)

    os.system( 'sleep 5' )




#Starting jobs submission

log = open('logfile','w')

for model in models:

   if not os.path.exists(path+"/"+model):
      os.makedirs(path+"/"+model)
   
   log.write('  ----------------------  Using ' + model+'\n')

   for energy in energies:
      
      if not os.path.exists(path+"/"+model+"/E"+energy+"GeV"):
         os.makedirs(path+"/"+model+"/E"+energy+"GeV")

      log.write('  -----------------  ' + energy +'GeV\n')

      for thick in thicks:
         
         if not os.path.exists(path+"/"+model+"/E"+energy+"GeV/T"+thick+"mm"):
            os.makedirs(path+"/"+model+"/E"+energy+"GeV/T"+thick+"mm")
         
         log.write('  ------------  ' + thick+'mm\n')
 
         for material in materialsTodo:

            log.write('  -------  ' + material+'\n')

            for particle in particlesTodo:

               log.write('  --  ' + particle+'\n')
               log.flush()

               inputfile = "TargetMaterialGunMultiTargetLocalTemporary.py"
               command = 's/PdgCode.*/PdgCode = ' + particles[particle] + '/'
               sub = subprocess.call(['sed', '-i', command, inputfile])
               command = 's|MaterialEval.ModP = .*|MaterialEval.ModP = ' + energy + ' * GeV|'
               sub = subprocess.call(['sed', '-i', command, inputfile])
	       command = 's|ParticleGun.MaterialEval.ZPlane =.*|ParticleGun.MaterialEval.ZPlane = ' + Zplane[material][thick] + '|'
               sub = subprocess.call(['sed', '-i', command, inputfile])
               command = 's|ParticleGun.MaterialEval.Zorig =.*|ParticleGun.MaterialEval.Zorig = ' + Zorig[material][thick] + '|'
               sub = subprocess.call(['sed', '-i', command, inputfile])

	       inputfile = "Gauss-Job-MultiTarget-MultiTemporayOptionsFile.py"
               command = 's/Multi.*.root/Multi_' + particle + '_in' + material + '.root/'
               sub = subprocess.call(['sed', '-i', command, inputfile])
	       command = 's|\"Hadron\":.*, \"Gen|\"Hadron\":'"'"'' + model +''"'"', \"Gen|'
               sub = subprocess.call(['sed', '-i', command, inputfile])
               command = 's|target = \'Target_.*|target = \'Target_'+ thick + 'mm' + material + '\'|'
               sub = subprocess.call(['sed', '-i', command, inputfile])
               
	       #inputfile = "Gauss-Job-MultiTarget-MultiTemporayOptionsFile.py"
               #command = 's|histoFile = \'Multi.*|histoFile = \'Multi_' + particle + '_in' + material + '\'|'
               #sub = subprocess.call(['sed', '-i', command, inputfile])
	       
	       
	       if(useganga) :
                   RunOnGanga(workpath, model, energy, thick, material, particle)
               else :
                   sub = subprocess.call(['gaudirun.py', 'Gauss-Job-MultiTarget-MultiTemporayOptionsFile.py' ])


         if(useganga==False) :
             dest = path+"/"+model+"/E"+energy+"GeV/T"+thick+"mm/"
             sourcedir = os.environ['PWD']
             source = os.listdir(sourcedir)
   
             for file in source:
                if file.endswith(".root"):
                   if os.path.exists(dest+'/'+file):
                      os.remove(dest+'/'+file)
                   shutil.move(file,dest)


log.write('----------------   END  ------------------')
log.close()






