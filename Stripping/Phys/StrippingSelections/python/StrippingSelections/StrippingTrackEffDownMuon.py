## #####################################################################
# A stripping selection for Downstream J/psi->mu+mu- decays
# To be used for tracking studies
#
# @authors G. Krocker, P. Seyfert, S. Wandernoth
# @date 2010-Aug-17
# 
#######################################################################

#FIXME Write includes in a cleaner way
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer 
from Configurables import UnpackTrack, ChargedProtoParticleMaker, DelegatingTrackSelector, TrackSelector, BestPIDParticleMaker
from TrackFitter.ConfiguredFitters import ConfiguredFit
from Configurables import TrackStateInitAlg
from PatAlgorithms import PatAlgConf
from Configurables import TrackToDST
from StrippingConf.StrippingLine import StrippingLine
from Configurables import ChargedProtoParticleAddMuonInfo, MuonIDAlg
from MuonID import ConfiguredMuonIDs
from Configurables import ChargedProtoCombineDLLsAlg, ProtoParticleMUONFilter 

from Configurables import TrackSys

class StrippingTrackEffDownMuonConf(LHCbConfigurableUser):
    """
    Definition of Downstream J/Psi stripping.
    """
    __slots__ = {
			"MuMom":		1000.	# MeV
		,	"MuTMom":		100.	# MeV
		,	"TrChi2":		10.	# MeV
		,	"MassPreComb":		2000.	# MeV
		,	"MassPostComb":		200.	# MeV
		,	"Doca":			5.	# mm
		,	"VertChi2":		25.	# adimensional
                ,       "DataType":             '2010'        
		}

    def nominal_line( self ):
	PatAlgConf.DownstreamConf().configureAlg()
	

	#FIXME: Only necessary if Downstream tracking is not defined in a Configurable
	
	#GaudiSequencer("TrackDownstreamFitSeq").Members += [TrackStateInitAlg("InitDownstreamFit")]
        #TrackStateInitAlg("InitDownstreamFit").TrackLocation = "Rec/Track/Downstream"
        # Downstream fit

        #fitter = ConfiguredFit("FitDownstream","Rec/Track/Downstream")
        #GaudiSequencer("TrackDownstreamFitSeq").Members += [fitter]

	TrackToDST("DownTrackToDST").TracksInContainer = "Rec/Track/Downstream"
	
	TrackSys().setProp('SpecialData', ['earlyData'])
	
        from Configurables import LoKi__VoidFilter
        Jpsi_already_there = LoKi__VoidFilter("Jpsi_already_there")
        Jpsi_already_there.Code = "1 <= CONTAINS('Rec/Track/Downstream')"
        Jpsi_not_yet_there = LoKi__VoidFilter("Jpsi_not_yet_there")
        Jpsi_not_yet_there.Code = "1 > CONTAINS('Rec/Track/Downstream')"
        GaudiSequencer("Jpsidotracking").Members = [GaudiSequencer("TrackSeedPatSeq"),
                        GaudiSequencer("TrackSeedFitSeq"),
                        GaudiSequencer("TrackDownstreamPatSeq"),
                        GaudiSequencer("TrackDownstreamFitSeq"),
                        TrackToDST("DownTrackToDST")]
        GaudiSequencer("Jpsitracks").Members = [Jpsi_already_there, GaudiSequencer("Jpsidotracking")]
        GaudiSequencer("Jpsitracks").ModeOR = True
        GaudiSequencer("Jpsitracks").ShortCircuit = True
        GaudiSequencer("Jpsimainseq").Members = [GaudiSequencer("Jpsitracks"), self.DownJPsi()]
        algos =[tisTosPreFilterHlt1Jpsi, tisTosPreFilterHlt2Jpsi, GaudiSequencer("Jpsimainseq")]

	return StrippingLine('TrackEffDownMuonJpsiLine', prescale = 1., algos=algos)

    def trackEffValid_line( self ):
        algos =[tisTosPreFilterHlt1Jpsi, tisTosPreFilterHlt2Jpsi]

	return StrippingLine('TrackEffDownValidLine', prescale = 0.5, algos=algos)

    def makeMyMuons(self, name, trackcont):
	unpacker = UnpackTrack(trackcont+"UnpackTrack")
	unpacker.InputName="pRec/Track/"+trackcont
	unpacker.OutputName="Rec/Track/"+trackcont

	idalg = MuonIDAlg(name+"IDalg")
	cm=ConfiguredMuonIDs.ConfiguredMuonIDs( self.getProp("DataType") ) #data=DaVinci().getProp("DataType"))
	cm.configureMuonIDAlg(idalg)
	idalg.TrackLocation = "Rec/Track/"+trackcont
	idalg.MuonIDLocation = "Rec/Muon/MuonPID/"+trackcont
	idalg.MuonTrackLocation = "Rec/Track/MuonFor" +trackcont

	downprotoseq = GaudiSequencer(name+"ProtoPSeq")
	downprotos = ChargedProtoParticleMaker(name+"ProtoPMaker")
	downprotos.InputTrackLocation = ["Rec/Track/"+trackcont]
	downprotos.OutputProtoParticleLocation = "Rec/ProtoP/"+name+"ProtoPMaker"
	downprotos.addTool( DelegatingTrackSelector, name="TrackSelector" )
	tracktypes = [ "Long","Upstream","Downstream","Ttrack","Velo","VeloR" ]
	if (trackcont == "Best") :
		tracktypes = [ "Long" ]
	downprotos.TrackSelector.TrackTypes = tracktypes
	selector = downprotos.TrackSelector
	for tsname in tracktypes:
		selector.addTool(TrackSelector,name=tsname)
		ts = getattr(selector,tsname)
		# Set Cuts
		ts.TrackTypes = [tsname]

	addmuonpid = ChargedProtoParticleAddMuonInfo(name+"addmuoninfo")
	addmuonpid.InputMuonPIDLocation = "Rec/Muon/MuonPID/"+trackcont
	addmuonpid.ProtoParticleLocation = "Rec/ProtoP/"+name+"ProtoPMaker"
        combinedll = ChargedProtoCombineDLLsAlg(name+"CombineDLL")
	combinedll.ProtoParticleLocation = "Rec/ProtoP/"+name+"ProtoPMaker"
        
	particleMaker =  BestPIDParticleMaker(name , Particle = "muon")
	particleMaker.addTool(ProtoParticleMUONFilter(Selection = ["RequiresDet='MUON' IsMuonLoose=True"]),name="muon")
	particleMaker.Particles = [ "muon" ]
        particleMaker.Input = "Rec/ProtoP/"+name+"ProtoPMaker"

	# DST post treatment
	TrackToDST(name+"TrackToDST").TracksInContainer = "Rec/Track/"+trackcont
	downprotoseq.Members += [ TrackToDST(trackcont+"TrackToDST"), downprotos, addmuonpid, combinedll ]
	DataOnDemandSvc().AlgMap.update( {
		"/Event/Rec/Track/"+trackcont : unpacker.getFullName(),
		"/Event/Rec/Muon/MuonPID/"+trackcont : idalg.getFullName(),
		"/Event/Rec/ProtoP/"+name+"ProtoPMaker" : downprotoseq.getFullName(),
        	"/Event/Phys/" + particleMaker.name() + '/Particles' : particleMaker.getFullName(),
               	"/Event/Phys/" + particleMaker.name() + '/Vertices'  : particleMaker.getFullName() 
	} )


    def DownJPsi( self ):
	self.makeMyMuons("DownMuonsForTrackEff", "Downstream")
	
	
	from Configurables import CombineParticles, FilterDesktop

	MyDownJpsis = CombineParticles("DownJpsisForTrackEff")
	MyDownJpsis.InputLocations = [ "DownMuonsForTrackEff" ]
	MyDownJpsis.DecayDescriptor = "J/psi(1S) -> mu+ mu-" 
	MuCuts =  "((TRCHI2DOF < %(TrChi2)s) & (PT > %(MuTMom)s*MeV) & (P > %(MuMom)s*MeV) )" % self.getProps()
	MyDownJpsis.DaughtersCuts = {
        	        "mu+": MuCuts,
                	"mu-": MuCuts
                	}
	MyDownJpsis.CombinationCut = "((ADAMASS('J/psi(1S)') < %(MassPreComb)s * MeV) & (AMAXDOCA('') < %(Doca)s*mm) )" % self.getProps()
	MyDownJpsis.MotherCut = "((ADMASS('J/psi(1S)') < %(MassPostComb)s * MeV) & (VFASPF(VCHI2/VDOF) < %(VertChi2)s))" % self.getProps()

	selseq = GaudiSequencer("DownJpsiSelSequence")
	selseq.Members += [ MyDownJpsis ]
	selseq.ModeOR = True
	selseq.ShortCircuit = False
	selseq.IgnoreFilterPassed = False
	return selseq	
    
#########################################################################################
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
	
"""
Define TisTos Prefilters

"""

from Configurables import TisTosParticleTagger
# ################################################################
# Hlt1 PreFilter for Jpsi and Upsilons 
tisTosPreFilterHlt1Jpsi = TisTosParticleTagger("tisTosPreFilterHlt1JpsiForDownTrackEffLine")
tisTosPreFilterHlt1Jpsi.InputLocations = [ "Phys/StdLooseMuons" ]
tisTosPreFilterHlt1Jpsi.TisTosSpecs = { "Hlt1TrackMuonDecision%TOS" : 0}
tisTosPreFilterHlt1Jpsi.ProjectTracksToCalo = False
tisTosPreFilterHlt1Jpsi.CaloClustForCharged = False
tisTosPreFilterHlt1Jpsi.CaloClustForNeutral = False
tisTosPreFilterHlt1Jpsi.TOSFrac = { 4:0.0, 5:0.0 }
tisTosPreFilterHlt1Jpsi.NoRegex = True
# ################################################################
# Hlt2 PreFilter for Jpsis and Upsilons (IP cut)
tisTosPreFilterHlt2Jpsi = TisTosParticleTagger("tisTosPreFilterHlt2JpsiForDownTrackEffLine")
tisTosPreFilterHlt2Jpsi.InputLocations = [ "Phys/tisTosPreFilterHlt1JpsiForDownTrackEffLine" ]
tisTosPreFilterHlt2Jpsi.TisTosSpecs = { "Hlt2SingleMuonDecision%TOS" : 0}
tisTosPreFilterHlt2Jpsi.ProjectTracksToCalo = False
tisTosPreFilterHlt2Jpsi.CaloClustForCharged = False
tisTosPreFilterHlt2Jpsi.CaloClustForNeutral = False
tisTosPreFilterHlt2Jpsi.TOSFrac = { 4:0.0, 5:0.0 }
tisTosPreFilterHlt2Jpsi.NoRegex = True
