'''
Create two MicroDSTs using FilterDesktop-based Selections. One is with a FilterDsktop, the other with a ConfigurableGenerator.FilterDesktop. They should be identical.
'''

from Gaudi.Configuration import *
from Configurables import DaVinci, MicroDSTWriter

from PhysSelPython.Wrappers import SelectionSequence, Selection
from Configurables import FilterDesktop
from GaudiConfUtils import ConfigurableGenerators

from StandardParticles import StdTightPions

filter = FilterDesktop('_filter', Code = 'ALL')
selection0  = Selection('SelFilter', 
                       Algorithm = filter, 
                       RequiredSelections = [StdTightPions])
selSeq0 = SelectionSequence('Seq'+selection0.name(),
                            TopSelection = selection0)


filter_gen = ConfigurableGenerators.FilterDesktop(Code = 'ALL')
selection1  = Selection('SelFilterGen', 
                        Algorithm = filter_gen, 
                        RequiredSelections = [StdTightPions])
selSeq1 = SelectionSequence('Seq'+selection1.name(),
                            TopSelection = selection1)


conf = MicroDSTWriter("MicroDST0", 
                      OutputFileSuffix = "TestFilterDesktop",
                      SelectionSequences = [selSeq0, selSeq1],
                      CopyBTags = False)

selDST0Seq = conf.sequence()

dv = DaVinci()
dv.DataType = '2010'
dv.InputType = 'SDST'
dv.EvtMax = -1
dv.UserAlgorithms = [selDST0Seq]
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/testFiles/Brunel-v37r1-069857_0000000006-1000ev.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs =['xmlcatalog_file:TestSDSTCatalog.xml']
