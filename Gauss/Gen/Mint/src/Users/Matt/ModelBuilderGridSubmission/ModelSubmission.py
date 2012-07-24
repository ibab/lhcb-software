
import os
import time

from ModelCreator import *

class Models:
    inputfile = ''
    def __init__(self,file='FileOfModels.txt'):
        self.inputfile = file
    def GetModels(self):
        f = open(self.inputfile,'r')
        models = f.readlines()
        return models
    
#Create Ganga Job
class ModelFinderJob:
    basedir=""
    userarea = ""
    ParticleProp = ""
    name = ""
    inputs = []
   
    j = Job()
    def __init__(self,name,Basedir):
        self.name = name
        self.basedir = Basedir
        self.ParticleProp = Basedir + "Mojito/ParticleProperties/src/"
    def userarea(self, Userarea):
        self.userarea = self.basedir+Userarea
        
    def makeSubjobs(self):
        inputargs = []
        MyModels = Models('InputModels.txt')
        models = MyModels.GetModels()
        for model in models:
            print model
            model = model.replace('\n','')
            self.inputs.append(model)
            modelName = model.split('/')
            inputargs.append([str(1),modelName[1]])
            print modelName[1]
        return inputargs
    
    def InputLFNs(self,inputLFNs):
        self.j.backend.inputSandboxLFNs.extend(inputLFNs)
    
    def SetupJob(self):
        app = GaudiPython(project='DaVinci', version='v29r2') #v25r0  
        self.j.application = app
        script=self.userarea+"Grid/GangaSubmit.py"
        exe=self.userarea+"ampFit"
        input_mass=self.ParticleProp+"mass_width.cvs"
        input_parts=self.ParticleProp+"MintDalitzSpecialParticles.cvs"
        self.j.application.script = [script]
        inputargs = self.makeSubjobs()
        print inputargs
        s = ArgSplitter(args=inputargs)
        bckend = Dirac()
        bckend.settings['CPUTime']=60*60*18
        print script, exe
        self.j.name= self.name
        self.j.inputsandbox = [exe, input_mass, input_parts]
        self.j.outputsandbox = ['*.eps','*.pdf','*.png','*.txt']
        self.j.backend = bckend
        self.j.splitter = s
        
    def SubmitJob(self):
#        self.j.backend.inputSandboxLFNs = ['LFN:/lhcb/user/m/mcoombes/FourPiModelFit/BinningFile_D0.root',
#                                           'LFN:/lhcb/user/m/mcoombes/FourPiModelFit/NocutsMC.root']
        self.inputs.append('BG4piModel.txt')
        print self.inputs
        self.j.inputsandbox.extend(self.inputs)
        for entry in self.j.inputsandbox:
            print entry
    #    j.do_auto_resubmit = True
        self.j.submit()
        
    def JobID(self):
        return self.j.id
    def Job(self):
        return self.j
    
    def CreateModelandChi2File(self,outfileName):
        j = self.Job()
        f = open(outfileName,'w')
        for sj in j.subjobs:
            outputdir = sj.outputdir
            readChi2 = ReadChi2(outputdir+'Chi2.txt')
            chi2 = readChi2.Chi2()
            f.writeline(outputdir+'Model'+str(sj.id())+'.txt '+str(chi2)+'\n')
        f.close()
    
    def WaitTillfinished(self):
         jobrunning = True
         while jobrunning:
             if (self.j.status == "completed" or self.j.status == "failed"):
                 jobrunning = False
             time.sleep(3) #Check every 30 seconds to see if job is complete
            
# 
##from ModelFinder import *
##gangaArea = "/afs/cern.ch/work/m/mcoombes/Ganga/ganga_workspace1/workspace/mcoombes/LocalXML"
#modelTextFile = "Models.txt" # output models to this text file
##Chi2TextFile = "Chi2.txt"
#
#MintDir = "/afs/cern.ch/work/m/mcoombes/cmtuser/Gen/Mint/src/"
#UserArea = "Users/Matt/FourPiModelBuilder/" #AmpFit Users Area
#
#inputLFNs = ['LFN:/lhcb/user/m/mcoombes/FourPiModelFit/BinningFile_D0.root',
#             'LFN:/lhcb/user/m/mcoombes/FourPiModelFit/NocutsMC.root']
#
#ModelBuilderExe = MintDir+"Users/Matt/ModelCreator/ModelBuilder"



def RunModelFinder(MintDir,UserArea,inputLFNs,modelTextFile,ModelBuilderExe,max_iterations):
    print "GERE"
    for i in range (0,max_iterations):
        print i 
        if (i == 0):
            print "Run jacks code first iteration"
            modelsForIt = open(modelTextFile,"w")
            decs = ModelCreator()
            optionalDecays = decs.optionalDecays
            for j in range(0,len(optionalDecays)):
                myModel = ModelCreator()
                myModel.modelsUsed.append(optionalDecays[j])
                myModel.MakeInputFile("D_4pi_Base.txt", "FourPiModel_"+str(j)+".txt")
                modelsForIt.writelines("FourPiModel_"+str(j)+".txt\n")
            modelsForIt.close()
#            os.system('./'+ModelBuilderExe+' > modelbuilder.txt') # Ask Jack about iteration 0
        else:
            print "Run iteration ", i, " of Jacks code"
            print "Input ", modelTextFile
            os.system('./'+ModelBuilderExe+' '+str(modelTextFile)+' > modelbuilder.txt')
    
        """Get output from Jacks code"""
        directory = "it"+str(i)
        if ( not( os.path.exists(directory) ) ) :
             os.system("mkdir "+directory)
        else:
            os.system('rm -r '+directory)
            os .system("mkdir "+directory)
        
        os.system("mv FourPiModel*.txt "+directory+"/.") 
        os.system("ls "+directory+"/FourPiModel*.txt > InputModels.txt") 
    
        
        """Create AmpFit Job"""
    
        #Create Job object. Give it a name and the location of Mint /src/
        job = ModelFinderJob("Iteration"+str(i),MintDir)
        job.userarea(UserArea)#Tell job where your working area is for the ampFit you wish to submit.
        job.SetupJob() #Setup the job
        job.InputLFNs(inputLFNs) #Where is the data.
        job.SubmitJob()
        jid = job.JobID()
        print jid
        j = job.Job()# Get the ganga job.
        
        job.WaitTillfinished() #Wait till the job is finished
    
        job.CreateModelandChi2File(modelTextFile) #Create File of models For Jacks Code.
        os.system("cp "+modelTextFile+" "+directory+"/.")  # Copy output file to sub directory.
    
    


RunModelFinder(MintDir,UserArea,inputLFNs,modelTextFile,ModelBuilderExe,max_iterations)
