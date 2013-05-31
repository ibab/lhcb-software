##############################################################################
# File for running Brunel testing the trigger
#
# Syntax is:
#   gaudirun.py SftWithData.py
##############################################################################
from Gaudi.Configuration import importOptions
from Gaudi.Configuration import EventSelector, appendPostConfigAction
from Configurables import Brunel, InputCopyStream, CondDB
from FstTools.Configuration import FstConf

importOptions('$FSTTOOLSROOT/options/Sft.py')


# Configure trigger emulation
FstConf().TStationType = "IT+OT"
FstConf().VeloType = "Velo"
Brunel().EvtMax = 1000

# Output DST
output_fname = "/tmp/tim.dst"
InputCopyStream('DstWriter2').Output = "DATAFILE='PFN:%s'"%(output_fname)

# No bias data taken in early 2012, format is MDF, not digi
#EventSelector().Input =["DATAFILE='PFN:/afs/cern.ch/user/t/thead/w/private/HLT-emulation-data.raw' SVC='LHCb::MDFSelector' OPT='READ'"]
EventSelector().Input =["DATAFILE='PFN:/afs/cern.ch/work/h/hschindl/public/Samples/MinimumBias/DIGI/Velo_Nu7.6_30000000_1_1000ev-Extended.digi' OPT='READ'"]
Brunel().InputType = "DIGI"


DDDBtag = "dddb-20130408"
CondDBtag = "simcond-20121001-vc-md100"
Brunel().DDDBtag = DDDBtag
Brunel().CondDBtag = CondDBtag
CondDB().Upgrade = True
Brunel().DataType = "2012"
Brunel().Simulation = True

Brunel().MCLinksSequence = []
Brunel().MCCheckSequence = []

# Does this remove non L0 events?
Brunel().RecL0Only = True


def setup_mc_truth_matching():
   from Configurables import GaudiSequencer, PrTrackAssociator, PrChecker
   GaudiSequencer("CaloBanksHandler").Members = []
   GaudiSequencer("DecodeTriggerSeq").Members = []
   GaudiSequencer("MCLinksTrSeq").Members = [ "PrLHCbID2MCParticle", "PrTrackAssociator"]
   PrTrackAssociator().RootOfContainers = "/Event/Fst/Track"
   GaudiSequencer("CheckPatSeq" ).Members = [ "PrChecker" ]
   PrChecker().VeloTracks = "/Event/Fst/Track/Velo"
   PrChecker().ForwardTracks = "/Event/Fst/Track/Forward"
appendPostConfigAction(setup_mc_truth_matching)

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
