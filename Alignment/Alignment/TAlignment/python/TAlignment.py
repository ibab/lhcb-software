# Python script for T alignment
# @author Jan Amoraal
# @date 02-05-2007

# import gaudi python module
import gaudimodule
from gaudimodule import gbl
from ROOT import *
from array import array

# instantiate application manager
appMgr = gaudimodule.AppMgr(outputlevel=5, joboptions='$TALIGNMENTROOT/options/v200601_nikhef.opts')

# get pointer to histogram persistency svc
histoPerSvc = appMgr.service('HistogramPersistencySvc')
# set histogram filename
#histoPerSvc.OutputFile = "$TALIGNMENTROOT/python/ZeroMisalignment.root"
#histoPerSvc.OutputFile = "$TALIGNMENTROOT/python/FourHundredMicronDeltaXT3.root"
#histoPerSvc.OutputFile = "$TALIGNMENTROOT/python/FiftyMicronDeltaXT3DDDB.root"
#filename = 'FiftyMicronDeltaXT3ConDB'
#filename = 'FiftyMicronDeltaXT3DDDB'
#filename = 'FiftyMicronDeltaXRotZT3ConDB'
#filename = 'OneMicronDeltaXRotZT3ConDB'
#filename = 'OneMicronDeltaXRotZT3DDDB'
#filename = 'ZeroMisalignment'
#filename = 'ZeroMisalignmentChi2ProbMinCut1PercentSVD'
#filename = 'AlignOnlyT3ZeroMisalignment20000Events'
filename = 'test'
histoPerSvc.OutputFile = '$TALIGNMENTROOT/'+filename+'.root'

print "==> Running job " + filename

# get pointer to event selector
sel = appMgr.evtsel()
# open files
#-- For Event Type = 13144002 / Data type = DST 1
#--     Configuration = DC06 - phys-v2-lumi2
#--     DST 1 datasets produced by Brunel - v30r17
#--     From DIGI 1 datasets produced by Boole - v12r10
#--     From SIM 1 datasets produced by Gauss - v25r10
#--     Database version = v30r14
#--     Cards content = physical-physical
#--
#-- Datasets replicated at CERN
#-- 200 dataset(s) - NbEvents = 99368
sel.open(['PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000482_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000483_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000484_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000485_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000486_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000487_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000488_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000489_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000490_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000491_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000492_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000493_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000494_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000495_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000496_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000497_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000498_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000499_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000500_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000501_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000502_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000503_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000504_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000505_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000506_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000507_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000508_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000509_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000510_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000511_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000512_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000513_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000514_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000515_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000516_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000517_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000518_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000519_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000520_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000521_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000522_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000523_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000524_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000525_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000526_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000527_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000528_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000529_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000530_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000531_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000532_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000533_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000534_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000535_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000536_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000537_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000538_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000539_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000540_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000541_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000542_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000543_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000544_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000545_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000546_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000547_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000548_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000549_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000550_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000551_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000552_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000553_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000554_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000555_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000556_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000557_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000558_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000559_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000560_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000561_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000562_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000563_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000564_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000565_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000566_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000567_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000568_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000569_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000570_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000571_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000572_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000573_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000574_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000575_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000576_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000577_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000578_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000579_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000580_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000581_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000582_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000583_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000584_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000585_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000586_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000587_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000588_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000589_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000590_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000591_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000592_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000593_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000594_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000595_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000596_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000597_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000598_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000599_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000600_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000601_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000602_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000603_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000604_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000605_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000606_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000607_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000608_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000609_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000610_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000611_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000612_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000613_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000614_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000615_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000616_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000617_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000618_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000619_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000620_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000621_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000622_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000623_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000624_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000625_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000626_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000627_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000628_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000629_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000630_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000631_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000632_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000633_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000634_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000635_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000636_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000637_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000638_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000639_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000640_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000641_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000642_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000643_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000644_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000645_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000646_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000647_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000648_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000649_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000650_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000651_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000652_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000653_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000654_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000655_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000656_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000657_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000658_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000659_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000660_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000661_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000662_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000663_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000664_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000665_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000666_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000667_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000668_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000669_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000670_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000671_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000672_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000673_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000674_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000675_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000676_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000677_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000678_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000679_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000680_5.dst' ,
          'PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001868/DST/0000/00001868_00000681_5.dst' ])

# set print frequency
sel.PrintFreq = 1000

# set number of events
nEvents =  10

# set number of iterations
nIter = 10

# get pointer to main Escher sequencer
# escherMainSeq = appMgr.algorithm('EscherMainSeq')

# add alignment sequencer
#escherMainSeq.Members += ['GaudiSequencer/AlignmentSeq']
# get pointer to alignment sequencer
#alignSequencer = appMgr.algorithm('AlignmentSeq')
# add alignment algorithm to alignment sequencer
#alignSequencer.Members += ['AlignAlgorithm/AlignOTStations']

# get pointer to alignment algorithm
alignAlg = appMgr.algorithm('AlignOTStations')

#alignAlg.properties()["NumberOfIterations"].setValue(nIter);
#alignAlg.NumberOfIterations = 10;
#print alignAlg.NumberOfIterations

#pointer to event service
#evt = appMgr.evtsvc()

# pointer to  detector service
det = appMgr.detsvc()
# get OT
ot = det['/dd/Structure/LHCb/AfterMagnetRegion/T/OT']
#ot = det['DeOTDetectorLocation::Default']

def getElements() :
    elements = {1:det[ot.name()+'/T1'], 2:det[ot.name()+'/T2'], 3:det[ot.name()+'/T3']}
    return elements

def getAlignmentConstants(elements) :
    deltas = []
    for i in range(len(elements)) :
        # Txyz and Rxyz are stl vectors
        Txyz = elements[i+1].geometry().alignmentCondition().paramAsDoubleVect('dPosXYZ')
        Rxyz = elements[i+1].geometry().alignmentCondition().paramAsDoubleVect('dRotXYZ')
        for j in range(len(Txyz)) :
            deltas.append(Txyz.at(j))
        for j in range(len(Rxyz)) :
            deltas.append(Rxyz.at(j))        
    return deltas

def update(algorithm) :
    # get pointer to incident service
    incSvc = appMgr.service('IncidentSvc', 'IIncidentSvc')
    # incident to trigger update of constants
    updateConstants = gbl.Incident(algorithm, 'UpdateConstants')
    #updateConstants = gbl.Incident(algorithm, 'TestTrajectories')
    incSvc.fireIncident(updateConstants)

elements = getElements()
# get initial constants
deltaVsIter = [getAlignmentConstants(elements)]

# run over events
print '==> Iterating ' + str(nIter) + ' times over ' + str(nEvents) + ' events'

# i iterations
for i in range(nIter) :
    print "==> Iteration: " + str(i) 
    oldDeltas = getAlignmentConstants(elements)
    appMgr.run(nEvents)
    # trigger update of alignment parameters
    update(alignAlg.name())
    # print old and new deltas
    print "==> Old alignment conditions" 
    print oldDeltas
    print "==> New alignment conditions"
    newDeltas = getAlignmentConstants(elements)
    print newDeltas
    deltaVsIter.append(newDeltas)
    sel.rewind()
    #tc = evt['Rec/Track/Best']
    #ta = evt['Alignment/OTTracks']
    #print '==> tracks in track best container ' + str(tc.size())
    #print '==> tracks in track alignment container ' + str(ta.size())
    
#findHisto = gROOT.FindObjectAny
#histoStore = []
#for i in range(len(elements)*2) :
#    histoStore.append[(
# finalize job
#appMgr.finalize();

print "==> Deltas per iterarion"
print deltaVsIter 

# #make delta plots
deltaPosPlots = []
deltaRotPlots = []
for i in range(len(elements)) :
    dPosXYZHistos = []
    dRotXYZHistos = []
    for i in range(3) :
        dPosXYZHistos.append(TGraph())
        dRotXYZHistos.append(TGraph())
    deltaPosPlots.append(dPosXYZHistos)
    deltaRotPlots.append(dRotXYZHistos)

for i in range(len(elements)) :
    for j in range(len(deltaVsIter)) :
        deltaPosPlots[i][0].SetPoint(j, j+1, deltaVsIter[j][i*6])
        deltaPosPlots[i][1].SetPoint(j, j+1, deltaVsIter[j][i*6+1])
        deltaPosPlots[i][2].SetPoint(j, j+1, deltaVsIter[j][i*6+2])
        deltaRotPlots[i][0].SetPoint(j, j+1, deltaVsIter[j][i*6+3])
        deltaRotPlots[i][1].SetPoint(j, j+1, deltaVsIter[j][i*6+4])
        deltaRotPlots[i][2].SetPoint(j, j+1, deltaVsIter[j][i*6+5])

# write plots
file = TFile('${TALIGNMENTROOT}/deltas'+filename+'.root', 'recreate')
if (file) :
    print 'Writing deltas to file'
    for i in range(len(deltaPosPlots)) :
        for j in range(len(deltaPosPlots[i])) :
            name = "Element"+str(i)+"Parameter"+str(10+j)
            deltaPosPlots[i][j].SetName(name)
            deltaPosPlots[i][j].Write()
    for i in range(len(deltaRotPlots)) :
        for j in range(len(deltaRotPlots[i])) :
            name = "Element"+str(i)+"Parameter"+str(20+j)
            deltaRotPlots[i][j].SetName(name)
            deltaRotPlots[i][j].Write()
    file.Close()
