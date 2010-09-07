'''
Create a bunch of FilterDesktop Selections and test that they get written to a MicroDST. This requires
that the MicroDSTWriter sets their CloneFilteredParticles properties to True.
'''

from Gaudi.Configuration import *
from Configurables import DaVinci, MicroDSTWriter

from PhysSelPython.Wrappers import AutomaticData, SelectionSequence, Selection
from Configurables import FilterDesktop

sels = [AutomaticData(Location = 'Phys/StdTightPions'),
        AutomaticData(Location = 'Phys/StdTightMuons'),
        AutomaticData(Location = 'Phys/StdTightKaons'),
        AutomaticData(Location = 'Phys/StdLooseJpsi2MuMu') ]

filterSeqs = []
for sel in sels :
    filter = FilterDesktop('_'+sel.name(), Code = 'ALL')
    selection  = Selection('Sel'+sel.name(), 
                           Algorithm = filter, 
                           RequiredSelections = [sel])
    filterSeqs.append(SelectionSequence('Seq'+selection.name(), TopSelection = selection))

conf = MicroDSTWriter("MicroDST0", 
                      OutputFileSuffix = "TestFilterDesktop",
                      SelectionSequences = filterSeqs,
                      CopyBTags = False)

selDST0Seq = conf.sequence()

dv = DaVinci()
dv.DataType = '2010'
dv.InputType = 'SDST'
dv.EvtMax = -1
dv.UserAlgorithms = [selDST0Seq]
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/testFiles/Brunel-v37r1-069857_0000000006-1000ev.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs =['xmlcatalog_file:TestSDSTCatalog.xml']
