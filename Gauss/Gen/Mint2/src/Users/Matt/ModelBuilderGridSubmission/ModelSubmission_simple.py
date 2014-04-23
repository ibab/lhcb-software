
import os
import time

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
        script=self.basedir+"Users/Matt/ModelBuilderGridSubmission/GangaSubmit.py"
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
        
    def TestJob(self,jobNum):
        self.j = jobs(jobNum)
        
    def JobID(self):
        return self.j.id
    def Job(self):
        return self.j
    
    def CreateModelandChi2File(self,outfileName):
        j = self.Job()
        f = open(outfileName,'w')
        for sj in j.subjobs:
            outputdir = sj.outputdir
            readChi2 = self.ReadChi2(outputdir+'Chi2.txt')
            chi2 = readChi2
            f.write(outputdir+'Model'+str(sj.id)+'.txt '+str(chi2)+'\n')
        f.close()
    
    def ReadChi2(self,inputFile):
        f = open(inputFile)
        chi2 = f.read()
        f.close()
        return chi2

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



def RunModelFinder(MintDir,UserArea,inputLFNs,modelTextFile,ModelBuilderExe,max_iterations,start_iteration):
    for i in range (start_iteration,max_iterations):
        directory = "it"+str(i)

        print i 
        if (i == 0):
            print "Run jacks code first iteration"
            print "Will create an empty file to feed to Jack for iteration 0"
            modelsForIt = open(modelTextFile,"w")
            modelsForIt.close()
            os.system('./'+ModelBuilderExe+' '+str(modelTextFile)+' > modelbuilder.txt') # Ask Jack about iteration 0
        else:
            print "Run iteration ", i, " of Jacks code"
            print "Input ", modelTextFile
            os.system('./'+ModelBuilderExe+' '+directory+'/'+str(modelTextFile)+' > modelbuilder.txt')
    
        """Get output from Jacks code"""
        if ( not( os.path.exists(directory) ) ) :
             os.system("mkdir "+directory)
        else:
            os.system('rm -r '+directory)
            os .system("mkdir "+directory)
        
        if  ( os.path.exists("modelbuilder.txt") ):
            os.system("mv modelbuilder.txt "+directory+"/.") # move output log to it directory
        os.system("mv FourPiModel*.txt "+directory+"/.") # move output log to it directory

        os.system("ls "+directory+"/FourPiModel*.txt > InputModels.txt") 
    
        """Create AmpFit Job"""
        
        app = GaudiPython(project='DaVinci', version='v29r2') #v25r0  
        j = Job(application = app)
        script=MintDir+"Users/Matt/ModelBuilderGridSubmission/GangaSubmit.py"
        exe=UserArea+"ampFit"
        ParticleProp = MintDir+"Mojito/ParticleProperties/src/"
        input_mass=ParticleProp+"mass_width.cvs"
        input_parts=ParticleProp+"MintDalitzSpecialParticles.cvs"
        j.application.script = [script]
        
        
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
        
        s = ArgSplitter(args=inputargs)
        bckend = Dirac()
        bckend.settings['CPUTime']=60*60*18
        print script, exe
        j.name= "FourPiModelBuilder_It"+str(i)
        j.inputsandbox = [exe, input_mass, input_parts]
        j.outputsandbox = ['*.eps','*.pdf','*.png','*.txt']
        j.backend = bckend
        j.splitter = s
    
        j.backend.inputSandboxLFNs.extend(inputLFNs)
        inputs = []
        inputs.append('BG4piModel.txt')
        print inputs
        j.inputsandbox.extend(inputs)
        for entry in j.inputsandbox:
            print entry
    #    j.do_auto_resubmit = True
        j.submit()

        print j.id()

        jobrunning = True
        while jobrunning:
             if (j.status == "completed" or j.status == "failed"):
                 jobrunning = False
             time.sleep(3) #Check ev
        
        f = open(outfileName,'w')
        for sj in j.subjobs:
            outputdir = sj.outputdir
            f2 = open(outputdir+'Chi2.txt')
            chi2 = f2.read()
            f2.close()
            f.write(outputdir+'Model'+str(sj.id)+'.txt '+str(chi2)+'\n')
        f.close()
    
        os.system("cp "+modelTextFile+" "+directory+"/.")  # Copy output file to sub directory.
    


RunModelFinder(MintDir,UserArea,inputLFNs,modelTextFile,ModelBuilderExe,max_iterations,start_iteration)
