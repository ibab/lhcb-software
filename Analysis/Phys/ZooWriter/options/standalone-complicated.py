# stanalone-complicated.py
#
# this is an older DaVinci configuration to write out Kaons and Lambdas
# due to the quickly changing nature of DaVinci, parts of the code will
# no longer run unchanged, but the documentation of the meaning of
# ZooWriter properties and the overall explanations are quite useful...
#
# a more recent, simpler file can be found in standalone.py
#
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, DaVinci, ZooWriter
from Configurables import CombineParticles

importOptions("$STDOPTS/PreloadUnits.opts")

#MyOptions = [ "MC" ] 
MyOptions = [ "Data"]

################################################################################
# add here your selection

preselSeq = GaudiSequencer("MyZooSeq")
myResoSeq = GaudiSequencer("MyResoSeq")
importOptions( "$ZOOWRITERROOT/options/MyResos.py" )
preselSeq.Members += [ myResoSeq ]
myResoSeq.Members += [ CombineParticles("MyKaons"), CombineParticles("MyLambdas")]

# want the Sequence to succeed if one of the selection inside succeeds
myResoSeq.ModeOR = True
# want all selections to be executed, not only until the first one succeeds
myResoSeq.ShortCircuit = False

################################################################################
# tell the job about the Zoo libaries

ApplicationMgr().Dlls += [ "Zoo" ]

GaudiSequencer("MyZooSeq").Members += [ ZooWriter("ZooWriter") ]

################################################################################
# now we tell the ZooWriter what to do
#
# The ZooWriter can be configured via a couple of options that basically fall in
# two categories:
# (i) Basic, i.e. essential job options
# (ii) Expert options you should only touch if zou know what you do!

zoowriter = ZooWriter("ZooWriter")
################################################################################
# Basic ZooWriter options 

if "MC" in MyOptions :
	zoowriter.WriteMC = True
	zoowriter.WriteMCGenEventInfo = True
	# for which particles (including their daughters) do we like to write
	# out MC truth if the list is empty, and WriteMC = true, it is written
	# out for all particles, which blows up the ntuple enourmously
	zoowriter.MCList = [310, 3122]
	# for which particle do we want to check the background status
	zoowriter.BackgroundList = [310, 3122]

# for which particle do we want to check for TisTosTob
zoowriter.TriggerList = [310, 3122]
# Define the selections for which TisTosTob is checked. Selection names must
# correspond to the ones used by DaVinci
zoowriter.L0SelectionName += [ "Hlt1L0CALO" ]
zoowriter.HLT1SelectionName += []
zoowriter.HLT2SelectionName += []

# which particle types do we like to tag - default none
#zoowriter.TaggingList = []

# write out DLL info for stable particles - default false
#zoowriter.WriteDLL = True 



# Write out some basic track info of stable particles, like fit chi^2, number of
# hits, etc. - default:false
zoowriter.WriteTrackInfo = True
# What extra information about the track should be saved? Insert list of
# integers as defined in the AdditionalInfo enum of the LHCb::Track class
zoowriter.ExtraInfoList = []
# Write out list of LHCbIDs on the track - default:false
zoowriter.WriteLHCbIDs = True
# Write out bit sets corresponding to a hit pattern for the track -
# default:false
zoowriter.WriteHitPattern = True
# Write out bit sets corresponding to a expected hit pattern for the track -
# default:false
zoowriter.WriteExpectedHitPattern = True
# Write out bit sets corresponding to a hit pattern for the track where hits in
# a wider search window around the track are collected. Search windows can be
# adjusted in the corresponding tool - default:false
zoowriter.WriteCollectedHitPattern = True

# For which particles do we want to write out state information of the track.
# Insert particle ID to list or choose -1 for all
zoowriter.PackedStatesList = [ -1 ]
# For which particles do we want to write out the full covariance of the states.
# Insert particle ID to list or choose -1 for all
zoowriter.CovarianceList = [ -1 ]
# Do we want to write out a state at the point of closest approach to the z
# axis? - default:false
zoowriter.PackedStateAtPocaToZAxis = True
# List of z positions for which we want to write out states on the track
zoowriter.PackedStatesZList = [ 770., 990., 2165., 2700., 7500., 8520., 9410., 11900. ]

# Do we want to write out detector Occupancies in the event? - default:false
zoowriter.WriteOccupancies = True

# Which collections of particles should be written to the Zootuple
zoowriter.InputCollections = [
		"Phys/MyKaons/Particles",
		"Phys/MyLambdas/Particles",
		]

# How are they called in Zoo - this name is used in your analysis code to
# identify the particles
zoowriter.DecayName = [
		"MyKaons",
		"MyLambdas",
		]

# Output filename
zoowriter.Filename = "MC.root"
#zoowriter.OutputLevel=2
##################################################################################
# Expert ZooWriter options 

# Link stable particles of different collections using lhcb id content. Each
# particle collection can only linked to one other (single entries in
# LinkFromList). In your analysis code it is then possible to directly get the
# link to the corresponding particle.

# zoowriter.LinkFromList = {"Phys/StdLoosePions/Particles","Phys/Hlt2Pions/Particles", 
#		          "Phys/CombinedKaons/Particles", "Phys/CombinedPions/Particles" };
# zoowriter.LinkToList =   {"Phys/Hlt2Pions/Particles", "Phys/StdLoosePions/Particles", 
#		          "Phys/Hlt2Pions/Particles", "Phys/Hlt2Pions/Particles"};

# Name of the particle collection for which a tree fit should be applied
# zoowriter.TreefitName = []
# PID of particle to which the mass should be constrained in the tree fit, one
# for each fitted collection
# zoowriter.TreefitMassConstraints = [];
#
# Use another PVReFitter to refit your vertices - default:AdaptivePVReFitter
#zoowriter.PVReFitterName = "AdaptivePVReFitter"

# Use extrapolator to extrapolate tracks to a given z position to get the
# state - default:true
#zoowriter.PackedStatesExtrapolate = True

# Try to make an emergency PV instead of the default one - default:false
#zooWriter.MakeEmergencyPV = False

# Save generator info for the event, such as interaction type, etc -
# default:false
#zoowriter.WriteMCGenEventInfo = False

# Parameters which are used to calculate the generated particle multiplicity
# Eta region in which the partcles must be produced / aka detector acceptence
#zoowriter.MCAcceptanceEtaRegion = [2., 5.]
# Min momentum the particles must have, to supress low energetic background
#zoowriter.MCAcceptanceMinP = 1000.
# count charged particles
#zoowriter.MCAcceptanceCharged = True
# count neutral particles
#zoowriter.MCAcceptanceNeutral = False
# require ct to supress resonances and shortlifed stuff
#zoowriter.MCAcceptanceMinCtau = 50.

# If you want to run the ZooWriter on every event (for example to write out MC
# information) regardless of the outcome of the selection you have to apply the
# patch below
#def patch_sequences():
#    GaudiSequencer("MyZooSeq").IgnoreFilterPassed = True
#    GaudiSequencer("MyZooSeq").Members += [ ZooWriter("ZooWriter") ]
#appendPostConfigAction(patch_sequences)

##########################################################################################
# Configure the Trigger you like to run
#from Configurables import HltConf, L0Conf

#if "MC" in MyOptions :
#	# L0 emulation
#	DaVinci().L0 = True
#	DaVinci().ReplaceL0BanksWithEmulated = True 
#	DaVinci().Hlt = True
#	# HLT setup
#	HltConf().ThresholdSettings = 'Physics_MinBiasL0_PassThroughHlt_Apr10'
#if "DATA" in MyOptions :
#	importOptions('$HLTCONFROOT/options/HltInit.py')
#	from Configurables import HltDecReportsDecoder,HltSelReportsDecoder,HltVertexReportsDecoder,DataOnDemandSvc
#	DataOnDemandSvc().AlgMap["Hlt/DecReports"] = HltDecReportsDecoder(OutputLevel = 4);
#	DataOnDemandSvc().AlgMap["Hlt/SelReports"] = HltSelReportsDecoder(OutputLevel = 4);
#	DataOnDemandSvc().AlgMap["Hlt/VertexReports"] = HltVertexReportsDecoder(OutputLevel = 4);
#	DaVinci().UserAlgorithms = [HltDecReportsDecoder(OutputLevel = 4),HltSelReportsDecoder(OutputLevel = 4)]

#DaVinci().Hlt2Requires = 'L0'               ## change if you want Hlt2 with respective of L0+Hlt1
#DaVinci().HltType = 'Hlt1+Hlt2'                  ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'

##########################################################################################
# Configure basic DaVinci options

#-- Use latest 2009 database tags for real data
#DaVinci().DataType = "2009"
DaVinci().DataType = "2011"
#DaVinci().PrintFreq = 1
#DaVinci().InputType = "DST"

if "Data" in MyOptions:
	# sanity check
	if "MC" in MyOptions:
		print "Can't specify both data and MC in MyOptions!"
		sys.exit(1)
	# setup for data [FIXME: What databases do I have to run in any case?]
	DaVinci().Simulation = False
	# if you know which tag to use
	#DaVinci().DDDBtag = "head-20100119"
	#DaVinci().CondDBtag = "head-20100303"
	# to get the grid's default tag
  #from Configurables import CondDB
  #CondDB().UseLatestTags = ["2011"]
  # NB: 29th Oct 2012: here was an instruction to use Oracle. Marco Cat. announced that this is deprecated from now on.



elif "MC" in MyOptions:
	# setup for MC [FIXME: What databases do I have to run in any case?]
	DaVinci().Simulation = True
	DaVinci().DDDBtag = "head-20100407"
	DaVinci().CondDBtag = "sim-20100412-vc-mu100"

else:
	print "Neither data nor MC specified in MyOptions!"
	sys.exit(1)

#[FIXME: What is this decode clusters tool for?]
#[FIX: Needed when pattern reco is run in DaVinci]
importOptions("$ZOOWRITERROOT/options/DecodeClusters.py")


#importOptions("$ZOOWRITERROOT/options/Collision10-Beam3500-VeloClosed-MagDown-Minbias-RecoStripping4.py")
importOptions("$ZOOWRITERROOT/options/test.py")

#FileCatalog().Catalogs = [
#	"xml_file:$HOME/gkrocker/Datacards/2010_3500GeV_VeloClosed_MagDown_Nu1_Sim03Reco03_minbias.xml"
#	]

##########################################################################################
# set the number of events you like to run on

DaVinci().UserAlgorithms += [ preselSeq ]
DaVinci().EvtMax = 10000
##########################################################################################
# start the job with: gaudirun.py options/standalone.py options/Bs_DsPi.py
