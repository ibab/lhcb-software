##############################################################################
# File for running Brunel testing the trigger
#
# Syntax is:
#   gaudirun.py SftWithData.py
##############################################################################

from Configurables import GaudiSequencer, RecMoniConf, TrackSys, PrDebugTrackingLosses, FastVeloTracking
from Configurables import PrForwardTracking, PrForwardTool, PrForwardFromPointTool, ToolSvc, PrFTHitManager
from Configurables import OutputStream
from Configurables import MessageSvc
from FstTools.Configuration import FstConf

from Configurables import Brunel
import Gaudi.Configuration as GC

Brunel().DataType = "2012"
Brunel().Simulation = False
Brunel().EvtMax = 2000

# Does this remove non L0 events?
Brunel().RecL0Only = True

Brunel().DDDBtag = "head-20120126"
Brunel().CondDBtag = "head-20120607"

Brunel().RecoSequence = ["L0", "HLT", "Writer"]
RecMoniConf().MoniSequence = []
Brunel().MCLinksSequence = []
Brunel().MCCheckSequence = []
Brunel().OutputType = "NONE"

MessageSvc().Format = '% F%50W%S%7W%R%T %0W%M'
#MessageSvc().setVerbose += ['FastFit', 'DstWriter2', 'STOnlinePosition']
#MessageSvc().setDebug += ['SimplifiedMaterialLocator']
#MessageSvc().setVerbose = ['FstForward']
#MessageSvc().setVerbose += ['ToolSvc.PatTStationHitManager']
#MessageSvc().setDebug = ['FstVeloTracking']
#MessageSvc().setDebug = ['HltPVsPV3D']#.PVOfflineTool']
#MessageSvc().setVerbose = ['SelectFwd1']

#FstConf().TCK = '0x0094003d'
FstConf().TStationType = "IT+OT"
FstConf().VeloType = "Velo"
FstConf().TTType = "none"
FstConf().TrackFit = "HltFit"
FstConf().MaxIP = 9999
FstConf().MinPt = 1700. #MeV
# Apply after the track fit
FstConf().MinIPChi2 = 16.1
FstConf().MaxChi2Ndf = 1.5
FstConf().FastDecoding = False
FstConf().Compare = False

from Gaudi.Configuration import *

#EventSelector().Input =["DATAFILE='PFN:/afs/cern.ch/user/t/thead/w/private/HLT-emulation-data.raw' SVC='LHCb::MDFSelector' OPT='READ'"]
EventSelector().Input =["DATAFILE='PFN:/dev/shm/HLT-emulation-data.raw' SVC='LHCb::MDFSelector' OPT='READ'"]
Brunel().InputType = "MDF"

writer = OutputStream('DstWriter2')
writer.Output = "DATAFILE='PFN:/tmp/tim.dst'"
writer.OptItemList += ["Fst#999"]
GaudiSequencer("RecoWriterSeq").Members += [writer]

transform = {'.*STOnlinePosition.*': {'OutputLevel': {'.*': '1'}},
             '.*ITLiteCluster.*': {'OutputLevel': {'.*': '1'}},}

def doMyChanges():
   from Configurables import GaudiSequencer, PrTrackAssociator, PrChecker
   GaudiSequencer("CaloBanksHandler").Members = []
   GaudiSequencer("DecodeTriggerSeq").Members = []
   GaudiSequencer("MCLinksTrSeq").Members = [ "PrLHCbID2MCParticle", "PrTrackAssociator"]
   PrTrackAssociator().RootOfContainers = "/Event/Fst/Track"
   GaudiSequencer("CheckPatSeq" ).Members = [ "PrChecker" ]
   PrChecker().VeloTracks = "/Event/Fst/Track/Velo"
   PrChecker().ForwardTracks = "/Event/Fst/Track/Forward"
appendPostConfigAction(doMyChanges)

# The magic that is the HLT transform
import re
import GaudiKernel.ConfigurableDb as ConfigDb
from Gaudi.Configuration import appendPostConfigAction
import Gaudi.Configuration as GC

def transform_configurable(configurable, transform):
    full_name = configurable.getFullName()
    for prop_pat, trans in transform.iteritems():
        reg2 = re.compile(prop_pat)
        for prop in configurable.properties().iterkeys():
            if reg2.match(prop):
                old_val = configurable.getProp(prop)

                val = None # to make val's scope bigger
                # For dict based transformations
                # we need to make sure the value
                # is a string. For things which are actually
                # lists or other complicated python objects
                # chances are the conversion to a string and
                # back will not work, for those just use a
                # python function to transform them
                if isinstance(trans, dict):
                    val = str(old_val)
                    val_type = type(old_val)
                    
                    for val_pat, new_val in trans.items():
                        val = re.sub(val_pat, new_val, val)
                    val = val_type(val)

                elif callable(trans):
                    val = trans(old_val)

                if val is None:
                    raise RuntimeError("%s property %s was not transformed!"%(full_name, prop))

                print full_name, prop, "changed from", old_val, "to", val
                configurable.setProp(prop, val)

def hlt_transform(transformations):
    jo = GC.JobOptionsSvc()

    # configurables to skip in the second iteration
    skip = []
    duplicates = []
    for pattern, transform in transformations.iteritems():
        reg = re.compile(pattern)
        for config_name in ConfigDb.cfgDb.keys():
            configurable = ConfigDb.getConfigurable(config_name)
            # Some configurables do not seem to "exist"? Skip them
            if configurable is None:
                continue
            else:
                configurable = configurable()
                
            full_name = configurable.getFullName()
            if reg.match(full_name):
                transform_configurable(configurable, transform)
                skip.append(full_name)

        for configurable in jo.allConfigurables.values():
            full_name = configurable.getFullName()
            if full_name in skip:
                duplicates.append(full_name)
            if (full_name not in skip) and reg.match(full_name):
                transform_configurable(configurable, transform)

    print "Duplicates:", duplicates
                
#appendPostConfigAction(lambda: hlt_transform(transform))
