from Configurables import Moore

###
# Configure Moore to run from TCK.
# MAKE SURE THE TCK FITS THE MOORE VERSION YOU USE.
#
Moore().UseTCK = True
Moore().InitialTCK = '0x0023002b'

Moore().L0 = True
Moore().ReplaceL0BanksWithEmulated = True
Moore().UseDBSnapshot = False
Moore().EnableRunChangeHandler = False
Moore().CheckOdin = False
Moore().Simulation = True
Moore().DataType = '2010'

Moore().EvtMax = 100

###
# what to write and where? everything?
#
Moore().WriterRequires = []
#Moore().WriterRequires = ['Hlt1MuTrackPostScaler']
Moore().outputFile = 'tmp.dst'


###
# this shrinks the FOI of the L0 muon emulation
# use it only if you know what you are doing !!
#
## from Configurables import L0Conf
## L0Conf().IgnoreL0MuonCondDB = True
## from L0DU.L0Algs import emulateL0Muon
## l0muon = emulateL0Muon()
## l0muon.IgnoreCondDB = True
## l0muon.FoiXSize =[6,5,0,4,8]
## l0muon.FoiYSize =[0,0,0,1,1]
## l0muon.OutputLevel=2




###
# DB tags. MAKE SURE YOUR TAGS ARE RIGHT
#
#this should work (2010-10-05) on all MC2010
Moore().DDDBtag = 'head-20100624'
Moore().CondDBtag = 'sim-20100831-vc-md100'

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True
#layer needed to run on simulation
CondDB().addLayer(dbFile = "/afs/cern.ch/user/g/graven/public/simcond-RunParameters-layer.db", dbName = "SIMCOND")

###
# some input data. this is Bs2mm signal MC
#
Moore().inputFiles = ['/castor/cern.ch/grid/lhcb/MC/2010/DST/00007617/0000/00007617_00000029_1.dst']


