from Gaudi.Configuration import *
from Configurables import DaVinci
from Configurables import GaudiSequencer

simulation=False

stream='Bhadron'

line='Bc2hhh_Kpipi_exclLine'

my_wg='BnoC'

from StrippingSelections import buildersConf
confs = buildersConf()
from StrippingSelections.Utils import lineBuilder, buildStreams
streams = buildStreams( confs, WGs=my_wg )

DecisionList=[]

for s in streams:
    if s.name() is stream:
        for l in s.lines:
            Decision='{linename}Decision'.format(linename=l.name())
            #print Decision
            DecisionList.append(Decision)
            
print DecisionList

from Configurables import DecayTreeTuple
from DecayTreeTuple.Configuration import *
tuple=DecayTreeTuple()
tuple.Decay="[B_c+ -> K+ pi- pi+]CC"
tuple.Inputs=['Phys/{0}/Particles'.format(line)]
tuple.ToolList += [
    "TupleToolDira"
    ]

from Configurables import TupleToolStripping

tts=tuple.addTupleTool("TupleToolStripping")
tts.StrippingList = DecisionList


DaVinci().InputType='MDST'
DaVinci().RootInTES='/Event/{0}'.format(stream)
DaVinci().UserAlgorithms+=[tuple]
DaVinci().TupleFile="Output.root"
DaVinci().HistogramFile="histos.root"
DaVinci().DataType='2015'
DaVinci().EvtMax=-1
DaVinci().Lumi=True
DaVinci().PrintFreq=1000
DaVinci().MoniSequence=[tuple]
DaVinci().Simulation=False



#from GaudiConf import IOHelper
 #Use the local input data
#IOHelper().inputFiles([
 # './000000.Bhadron.mdst'
  #], clear=True)





