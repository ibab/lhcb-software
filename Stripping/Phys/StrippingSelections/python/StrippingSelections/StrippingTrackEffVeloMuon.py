## #####################################################################
# A stripping selection for VeloMuon J/Psi->mu+mu- decays
# To be used for tracking studies
#
# @authors G. Krocker, P. Seyfert, S. Wandernoth
# @date 2010-Aug-17
# 
#######################################################################

#FIXME Write includes in a cleaner way

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter,ProtoParticleMUONFilter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from Configurables import ChargedProtoParticleMaker, NoPIDsParticleMaker, DataOnDemandSvc, UnpackTrack, DelegatingTrackSelector, TrackSelector, CombinedParticleMaker, BestPIDParticleMaker
from Configurables import Tf__PatVeloRTracking, Tf__PatVeloSpaceTracking, Tf__PatVeloGeneralTracking, DecodeVeloRawBuffer
  
from StrippingConf.StrippingLine import StrippingLine
from Configurables import TrackStateInitAlg, TrackEventFitter, TrackPrepareVelo,TrackContainerCopy,TrackMatchVeloSeed, Tf__PatVeloSpaceTool, HltMuonRec
from Configurables import TrackCloneFinder
from Configurables import ChargedProtoParticleAddMuonInfo, MuonIDAlg, ChargedProtoCombineDLLsAlg
from MuonID import ConfiguredMuonIDs


class StrippingTrackEffVeloMuonConf(LHCbConfigurableUser):
    """
    Definition of tag and probe JPsi stripping.
    """
    __slots__ = {
			"TrChi2Mu":		10.	# adimensional
		,	"JpsiPt":		0.5	# GeV
		,	"TrPt":			100.	# MeV
		,	"VertChi2":		25.	# adimensional
		,	"MassPreComb":		1000.	# MeV
		,	"MassPostComb":		400.	# MeV
		}

    def nominal_line( self ):

	from Configurables import TisTosParticleTagger
        # ################################################################
        # Hlt1 PreFilter for Jpsi and Upsilons 
        tisTosPreFilterHlt1Jpsi = TisTosParticleTagger("tisTosPreFilterHlt1JpsiForVeloMuonTrackEffLine")
        tisTosPreFilterHlt1Jpsi.InputLocations = [ "Phys/StdLooseMuons" ]
        tisTosPreFilterHlt1Jpsi.TisTosSpecs = { "Hlt1TrackMuonDecision%TOS" : 0}
        tisTosPreFilterHlt1Jpsi.ProjectTracksToCalo = False
        tisTosPreFilterHlt1Jpsi.CaloClustForCharged = False
        tisTosPreFilterHlt1Jpsi.CaloClustForNeutral = False
        tisTosPreFilterHlt1Jpsi.TOSFrac = { 4:0.0, 5:0.0 }
        tisTosPreFilterHlt1Jpsi.NoRegex = True
        # ################################################################
        # Hlt2 PreFilter for Jpsis and Upsilons (IP cut)
        tisTosPreFilterHlt2Jpsi = TisTosParticleTagger("tisTosPreFilterHlt2JpsiForVeloMuonTrackEffLine")
        tisTosPreFilterHlt2Jpsi.InputLocations = [ "Phys/tisTosPreFilterHlt1JpsiForVeloMuonTrackEffLine" ]
        tisTosPreFilterHlt2Jpsi.TisTosSpecs = { "Hlt2SingleMuonDecision%TOS" : 0}
        tisTosPreFilterHlt2Jpsi.ProjectTracksToCalo = False
        tisTosPreFilterHlt2Jpsi.CaloClustForCharged = False
        tisTosPreFilterHlt2Jpsi.CaloClustForNeutral = False
        tisTosPreFilterHlt2Jpsi.TOSFrac = { 4:0.0, 5:0.0 }
        tisTosPreFilterHlt2Jpsi.NoRegex = True

	from Configurables import TrackEventCloneKiller,VeloMuonBuilder#, VMinMC, VeloMuonBuilder
	VeloMuonBuilder1 = VeloMuonBuilder("VeloMuonBuilder")
	VeloMuonBuilder1.OutputLevel = 6
	VeloMuonBuilder1.MuonLocation = "Hlt1/Track/MuonSeg"
	VeloMuonBuilder1.VeloLocation = "Rec/Track/FittedVelo"
	VeloMuonBuilder1.lhcbids = 4
	
	preve = TrackPrepareVelo("preve")
	preve.inputLocation = "Rec/Track/Velo"
	preve.outputLocation = "Rec/Track/UnfittedPreparedVelo"
	preve.bestLocation = ""
	from Configurables import TrackEventFitter, TrackMasterFitter
	from Configurables import TrackKalmanFilter, TrackMasterExtrapolator
	vefit = TrackEventFitter('vefit')
	vefit.TracksInContainer = "Rec/Track/UnfittedPreparedVelo"
	vefit.TracksOutContainer = "Rec/Track/FittedVelo"
	vefit.addTool(TrackMasterFitter, name = 'Fitter')
	from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
	ConfiguredFastFitter( getattr(vefit,'Fitter'))
	
        #define a TrackCloneFinder
	from Configurables import TrackCloneFinder
        low = TrackCloneFinder("TrackCloneFinder/low")
        low.MatchingFraction = 0.6

	
	Tf__PatVeloSpaceTracking("PatVeloSpaceTracking").addTool( Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool" )
	Tf__PatVeloSpaceTracking("PatVeloSpaceTracking").PatVeloSpaceTool.MarkClustersUsed = True;
	
	algos = [tisTosPreFilterHlt1Jpsi, tisTosPreFilterHlt2Jpsi, Tf__PatVeloRTracking(), Tf__PatVeloSpaceTracking(),Tf__PatVeloGeneralTracking(), preve,vefit, HltMuonRec(), VeloMuonBuilder1]
	
	algos.append(self.VeloCaloMuons())
	
	return StrippingLine('TrackEffVeloMuonJpsiLine', prescale = 1.,  algos=algos)
     	
    def makeMyParticles( self, name, trackcont, particle, type, charge, clone):
        unpacker = UnpackTrack(name+"UnpackTrack")
        unpacker.InputName="pRec/Track/"+trackcont
        unpacker.OutputName="Rec/Track/"+trackcont
        downprotos = ChargedProtoParticleMaker(name+"ProtoPMaker")
        downprotos.InputTrackLocation = ["Rec/Track/"+trackcont]
        downprotos.OutputProtoParticleLocation = "Rec/ProtoP/"+name+"ProtoPMaker"
        downprotos.addTool( DelegatingTrackSelector, name="TrackSelector" )
        tracktypes = [ type ]
        downprotos.TrackSelector.TrackTypes = tracktypes
        selector = downprotos.TrackSelector
        for tsname in tracktypes:
                selector.addTool(TrackSelector,name=tsname)
                ts = getattr(selector,tsname)
                # Set Cuts
                ts.TrackTypes = [tsname]
        particleMaker =  NoPIDsParticleMaker(name , Particle = particle)
        particleMaker.Input = "Rec/ProtoP/"+name+"ProtoPMaker"
        DataOnDemandSvc().AlgMap.update( {
                "/Event/Rec/ProtoP/"+name+"ProtoPMaker" : downprotos.getFullName(),
                "/Event/Phys/" + particleMaker.name() + '/Particles' : particleMaker.getFullName(),
                "/Event/Phys/" + particleMaker.name() + '/Vertices'  : particleMaker.getFullName() } )


    def makeMyVeloCaloMuons( self ):
	self.makeMyParticles("VeloMuonForTrackEffLine","VeloMuon","Muon","Long",-1,"Rec/Track/Best")
	PlusFilter = FilterDesktop("VeloMuonPlusForTrackEffLine")
	PlusFilter.InputLocations = ["VeloMuonForTrackEffLine"]
	
	MinusFilter = FilterDesktop("VeloMuonMinusForTrackEffLine")
	MinusFilter.InputLocations = ["VeloMuonForTrackEffLine"]
	PlusFilter.Code = "(Q > 0)"
	MinusFilter.Code = "(Q < 0)"
	DataOnDemandSvc().AlgMap.update( {
		"/Event/Phys/VeloMuonPlusForTrackEffLine/Particles" : PlusFilter.getFullName(),
		"/Event/Phys/VeloMuonMinusForTrackEffLine/Particles": MinusFilter.getFullName() })


    def VeloCaloMuons( self ):
	self.makeMyVeloCaloMuons()
	from Configurables import CombineParticles, FilterDesktop
	
	selseq = GaudiSequencer("MuonSelSequence")

	muCut = "((TRCHI2DOF < %(TrChi2Mu)s)) & (PT > %(TrPt)s)" % self.getProps()
	vmCut = "((TRCHI2DOF < 10)) & (PT > %(TrPt)s)" % self.getProps()

	muplus = FilterDesktop("MuPlusForTrackEffLine")
	muplus.InputLocations = [ "StdLooseMuons" ]
	muplus.Code = "(Q > 0)"
	muminus = FilterDesktop("MuMinusForTrackEffLine")
	muminus.InputLocations = [ "StdLooseMuons" ]
	muminus.Code = "(Q < 0)"

	vmtest1 = CombineParticles("vmtest1")
	vmtest1.InputLocations = [ "MuPlusForTrackEffLine", "VeloMuonMinusForTrackEffLine" ]
	vmtest1.DecayDescriptor = "[J/psi(1S) -> mu+ mu-]cc"
	vmtest1.DaughtersCuts = { "mu+" : muCut, "mu-" : vmCut }
	vmtest1.CombinationCut = "ADAMASS('J/psi(1S)')<%(MassPreComb)s*MeV"% self.getProps()
	vmtest1.MotherCut = "(VFASPF(VCHI2/VDOF)< %(VertChi2)s) & (ADMASS('J/psi(1S)')<%(MassPostComb)s*MeV) & (PT > %(JpsiPt)s*GeV)"% self.getProps()
	vmtest2 = CombineParticles("vmtest2")
	vmtest2.InputLocations = [ "MuMinusForTrackEffLine", "VeloMuonPlusForTrackEffLine" ]
	vmtest2.DecayDescriptor = "[J/psi(1S) -> mu+ mu-]cc"
	vmtest2.DaughtersCuts = { "mu+" : vmCut, "mu-" : muCut }
	vmtest2.CombinationCut = "ADAMASS('J/psi(1S)')<%(MassPreComb)s*MeV"% self.getProps()
	vmtest2.MotherCut = "(VFASPF(VCHI2/VDOF)< %(VertChi2)s) & (ADMASS('J/psi(1S)')<%(MassPostComb)s*MeV) & (PT > %(JpsiPt)s*GeV)"% self.getProps()

	selseq.Members += [ vmtest1,vmtest2 ]


	combine = FilterDesktop("CombineJPsi")
	combine.InputLocations = ["vmtest1", "vmtest2"]
	combine.Code = "(ALL)"
	
	selseq.Members += [ combine ]

        selseq.ModeOR = True
        selseq.ShortCircuit = False
        selseq.IgnoreFilterPassed = False
        

        preseq = GaudiSequencer("MuonPreSelSequene")
        preseq.Members += [ muplus,muminus ]
        preseq.ModeOR = True
        preseq.ShortCircuit = False

        combseq = GaudiSequencer("MuonCombinedSequence")
        combseq.Members += [ preseq, selseq ]
        combseq.ModeOR = False
        combseq.ShortCircuit = False


	return combseq	

	
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

