#!/usr/bin/env python
from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci, DecayTreeTuple
from StrippingConf.Configuration import StrippingConf
from StrippingSelections import StrippingBeauty2Charm as B2C #ADDED
from StrippingConf.StrippingStream import StrippingStream
from Configurables import  ProcStatusCheck
from Configurables import CondDB
from CommonParticles.Utils import DefaultTrackingCuts
from Configurables import EventTuple, TupleToolSelResults, TupleToolStripping
from Configurables import LoKi__HDRFilter
from Configurables import AuditorSvc, ChronoAuditor
from Configurables import StrippingReport

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
CondDB().IgnoreHeartBeat = True

B2C = B2C.Beauty2CharmConf("StrippingBeauty2Charm",B2C.config)

stream = StrippingStream("Test")
stream.appendLines(B2C.lines())
filterBadEvents =  ProcStatusCheck()

sc = StrippingConf(Streams=[stream],MaxCandidates=2000,AcceptBadEvents=False,
                   BadEventSelection=filterBadEvents)
sc.OutputType = "ETC" # Can be either "ETC" or "DST"

# Mimic the new default tracking cuts
DefaultTrackingCuts().Cuts = {"Chi2Cut": [0,5]}
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)"\
         "(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
filterHLT = LoKi__HDRFilter("FilterHLT",Code=regexp)

seq = GaudiSequencer("TagSeq")
#seq.Members = [tag]

AuditorSvc().Auditors.append(ChronoAuditor("Chrono"))
sr = StrippingReport(Selections=sc.selections())

DaVinci().PrintFreq = 1000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
#DaVinci().ETCFile = "etc.root"
DaVinci().EvtMax = 5
DaVinci().EventPreFilters = [filterHLT]
DaVinci().appendToMainSequence([sc.sequence()])
DaVinci().appendToMainSequence([sr])
#DaVinci().appendToMainSequence([ac])
DaVinci().MoniSequence += [seq] 
DaVinci().DataType = "2010"
DaVinci().InputType = 'SDST'

EventSelector().Input = [
    "DATAFILE='PFN:/vols/lhcbdisk03/mwillia1/00008375_00044647_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'"
    ]
FileCatalog().Catalogs = ["xmlcatalog_file:pool_xml_catalog.xml"]
