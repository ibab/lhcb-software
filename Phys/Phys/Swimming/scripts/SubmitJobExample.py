import sys,os

#Set the options
thisproject = "Moore"
thisversion = "v10r2p1"
scriptdir = "/afs/cern.ch/user/g/gligorov/w0/cmtuser/Erasmus_v3r3/Phys/Swimming/python/Swimming"
jobsdir = "/afs/cern.ch/user/g/gligorov/cmtuser/forgrid/ladyganga/MyJobs/SwimmingJobs/Untagged/Trigger/lfnlists_KK_WM"
thistime = 25000
maxevents = '1100'
thesecands = "/Event/Strip02/SeqD2HHStep2/Phys/SelD2HHStep2"
swimStripping = '0'
thesehlt1trigs = "Hlt1TrackAllL0Decision"
thesehlt2trigs = "Hlt2CharmOSTF2BodyWideMassDecision"
if int(swimStripping) :
    thesehlt1trigs = "StreamCharmMicroDST"
    thesehlt2trigs = "/Event/Phys/D02hhPIDLine/Particles"
    thisproject = "DaVinci"
    thisversion = "v26r3p2"

files = os.listdir(jobsdir)

for file in files :
    thistck = ("0x"+file.strip(".py").split('x')[1].split('_')[0]).lower()
    readlfns = open(jobsdir+"/"+file)
    thislfnlist = []
    for line in readlfns :
        if line.find("dst") > -1: 
            lfnname = "/lhcb"+line.split("lhcb")[-1].split('dst')[0]+"dst" 
            thislfnlist.append(LogicalFile(name=lfnname)) 

    gp         = GaudiPython()
    gp.script  = scriptdir+"/RunSwimming.py"
    gp.args    = ["--TCK",thistck,"-n",maxevents]
    gp.project = thisproject
    gp.version = thisversion

    j                             = Job(backend=Dirac())
    j.application                 = gp
    j.name                        = thisproject+"_UntaggedKKWM_"+file.strip(".py")
    j.inputsandbox                = []
    #
    #Add all files in script dir
    helperscripts = os.listdir(scriptdir)
    for helperscript in helperscripts :
        if helperscript.split('.')[1] == "py" and \
           helperscript <> "RunSwimming.py" :
            j.inputsandbox.append(scriptdir+"/"+helperscript)
    #
    j.inputdata                   = thislfnlist
    j.outputsandbox               = ["*.root"]
    j.backend.settings['CPUTime'] = thistime 

    j.splitter = DiracSplitter(filesPerJob=1, ignoremissing=True, maxFiles=-1)
    #j.submit()

