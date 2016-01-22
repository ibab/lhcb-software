import os, sys, string

mydataset="Reco15a_Run164668.py"
mydatasetxml="Reco15a_Run164668.xml"

mystripping="stripping24"

wg = ''
job_name = ''
if len(sys.argv) > 1 :
  wg = str(sys.argv[1])
  job_name = 'Str24_'+str(wg)

print "TEST FOR "+wg+" WORKING GROUP"

dv = DaVinci(version='v38r1',
            setupProjectOptions = '--nightly lhcb-prerelease')

#dv = DaVinci(version='v38r0')
ds = dv.readInputData(mydataset)
ds.XMLCatalogueSlice = mydatasetxml
#ds = dv.readInputData('data.py')

f = open('TestFromSettings_all.py','r')
opts = f.read()
opts = opts.replace('___wg___',wg)
opts = opts.replace('__mystripping__',mystripping)
f.close()
dv.extraopts = opts


#f =open('DV-ReRunPID.py')
#opts = f.read()
#dv.extraopts += opts

j = Job(name = job_name,
        application = dv,
        inputdata = ds,
        inputsandbox = [mydatasetxml],
        outputfiles = [LocalFile(namePattern='*.root'),
                                              MassStorageFile(namePattern='*.dst'),
                                              MassStorageFile(namePattern='*.mdst')],
#                                              DiracFile(namePattern='*.xml')],
        splitter = SplitByFiles(filesPerJob = 2, maxFiles = 50, ignoremissing = True),
        backend = LSF(queue = '1nd')
#        backend = Local()
)

#j.outputfiles = ['000000.BhadronCompleteEvent.dst'
                #,"000000.Bhadron.mdst"
                #,'000000.Charm.mdst'
                #,'000000.CharmCompleteEvent.dst'
                #,'000000.Dimuon.dst'
                #,'000000.EW.dst'
                #,'000000.Leptonic.mdst'
                #,'000000.MDST.dst'
                #,'000000.PID.mdst'
                #,'000000.Semileptonic.mdst']
#j.inputsandbox=['CaloReconstruction-2010-2013-v4.db']

#j.backend    = Dirac()
#j.backend.settings['CPUTime'] = 8*3600
#j.backend.diracOpts = 'j.setDestination("LCG.CERN.ch")'

j.application.args = ["-T"]

j.submit()

