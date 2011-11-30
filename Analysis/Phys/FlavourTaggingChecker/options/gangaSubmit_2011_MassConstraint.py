def datasetFromCard(filename):
    files = []
    f = file(filename)
    for line in f.xreadlines():
        l = line.lstrip()
        if l[0:2]=="//" : continue
        #if l.rfind("DATAFILE")==-1: continue
        if l.rfind("LFN")==-1: continue
        wds = l.split("'")
        #print " file ====== ",wds[1]
        files.append(wds[1])
        
    return LHCbDataset(files)
                           

#gridProxy.destroy()
#gridProxy.create()



#Real Data:

print "Create job"

job=Job(application=DaVinci(version='v29r1'), name ='Down_Strip17') #,backend=Dirac(CPUTime=30000))

print "Get LHCb data set"

ds1 = datasetFromCard("/afs/cern.ch/user/s/svecchi/cmtuser/DaVinci_v29r1/Phys/DaVinci/job/Strip17_MagUp.py")
ds2 = datasetFromCard("/afs/cern.ch/user/s/svecchi/cmtuser/DaVinci_v29r1/Phys/DaVinci/job/Strip17_MagDown.py")
#ds2 = DaVinci().readInputData("/afs/cern.ch/user/s/svecchi/cmtuser/DaVinci_v29r1/Phys/DaVinci/job/Strip17_MagDown_slim.py")



job.application.args=["--option='from GaudiConf import IOHelper; IOHelper("+'"ROOT", "ROOT"'+ ").postConfigServices()'"]

#job.inputdata = [file1 for file1 in ds1.files]
#job.inputdata = [file1 for file1 in ds1.files if file1 not in ds1_old.files]
job.inputdata = ds2



print "Specify option files"

job.application.optsfile = ['/afs/cern.ch/user/s/svecchi/cmtuser/DaVinci_v29r1/Phys/DaVinci/job/B2JpsiX_StickToYourLine_Tagging.py']
#job.application.optsfile = ['/afs/cern.ch/user/s/svecchi/cmtuser/DaVinci_v29r1/Phys/DaVinci/job/B2JpsiX_StickToYourLine_Emilie.py']

job.outputsandbox = ['*.root']
print "Specify input data : super seeds what is in option files"


print "Calls Dirac splitter"
job.backend = Dirac()
#job.backend = Local()
job.splitter = DiracSplitter(filesPerJob = 8, maxFiles = -1, ignoremissing=True)
#job.splitter = DiracSplitter(filesPerJob = 10, maxFiles = 40, ignoremissing=True)
# for LSF :
#job.splitter = SplitByFiles(filesPerJob=20, maxFiles = 100) # try this
#job.backend.settings['BannedSites'] = ['LCG.RAL.uk']
#job.backend.settings['BannedSites'] = ['LCG.GRIDKA.de', 'LCG.PIC.es', 'LCG.NIKHEF.nl']
#job.backend.settings['BannedSites'] = ['CREAM.NIKHEF.nl']
print "Finally submit the job"
print job.application.optsfile
job.submit()


## j = jobs(job.id)

## fi = file('list_'+job.name+'.py','w')
## for js in job.subjobs:
##         fi.write(str(js.backend.id) +' ,\n')
## fi.close()
        
