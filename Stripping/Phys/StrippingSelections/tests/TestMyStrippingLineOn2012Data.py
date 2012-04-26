# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")



from StrippingSelections.StrippingBs2ChicPhi_Chic2KKPiPi import Bs2ChicPhi_Chic2KKPiPiConf as builder
from StrippingSelections.StrippingBs2ChicPhi_Chic2KKPiPi import config_params as config_params0

lb0 = builder('Bs2ChicPhi_Chic2KKPiPi',config_params0)




from StrippingSelections.StrippingB2EtacKst import B2EtacKstConf as builder
from StrippingSelections.StrippingB2EtacKst import config_params as config_params1

lb1 = builder('B2ChicKst',config_params1)



# tutaj wszystko z Chic

from StrippingSelections.StrippingBs2EtacPhi import Bs2EtacPhiConf as builder
from StrippingSelections.StrippingBs2EtacPhi import config_params as config_params01

lb01 = builder('Bs2EtacPhi',config_params01)

from StrippingSelections.StrippingB2EtacKst import B2EtacKstConf as builder
from StrippingSelections.StrippingB2EtacKst import config_params as config_params02

lb02 = builder('B2EtacKst',config_params02)






stream.appendLines( lb0.lines() )
stream.appendLines( lb1.lines() )
stream.appendLines( lb01.lines() )
stream.appendLines( lb02.lines() )









from StrippingSelections.StrippingB2Kphiphi import B2KPhiPhiConf as dummy_builder
from StrippingSelections.StrippingB2Kphiphi import config_params

dlb = dummy_builder('B2KPhiPhi',config_params)
stream.appendLines( dlb.lines() )





from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents,
                    TESPrefix = 'Strip'
                    )


MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

DaVinci().PrintFreq = 2000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = -1
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType  = "2012"
DaVinci().InputType = "DST"

# change the column size of timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

# database
DaVinci().DDDBtag  = "head-20120413"
DaVinci().CondDBtag = "head-20120413"

from GaudiConf import IOHelper
IOHelper().inputFiles([
    'PFN:castor:/castor/cern.ch/user/j/jhe/Stripping18/111473_0000000110-FastVeloFix.dst'])



