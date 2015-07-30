## #####################################################################
# A stripping selection for Downstream J/psi->mu+mu- decays
# To be used for tracking studies
#
# @authors G. Krocker, P. Seyfert, S. Wandernoth
# @date 2010-Aug-17
#
# @authors P. Seyfert, A. Jaeger
# @date 2011-Mar-17
# 
# @author M. Kolpin
# @date 2015-Mar-23
# 
#######################################################################

__author__ = [ 'Michael Kolpin']
__date__ = '23/03/2015'
__version__ = '$Revision: 2.0 $'

__all__ = ('StrippingTrackEffDownMuonConf',
           'default_config',
           'TrackEffDownMuon',
	   'selMuonPParts',
           'makeMyMuons',
           'DownJPsi',
           'DownZ',
           'DownUpsilon',
           'selHlt1Jpsi',
           'selHlt2Jpsi',
           'trackingDownPreFilter'
           )

#FIXME Write includes in a cleaner way
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from DAQSys.DecoderClass import Decoder
from Configurables import GaudiSequencer 
from Configurables import UnpackTrack, ChargedProtoParticleMaker, DelegatingTrackSelector, TrackSelector, BestPIDParticleMaker
from TrackFitter.ConfiguredFitters import ConfiguredFit
from Configurables import TrackStateInitAlg
from StrippingConf.StrippingLine import StrippingLine
from Configurables import ChargedProtoParticleAddMuonInfo, MuonIDAlgLite
from MuonID import ConfiguredMuonIDs
from Configurables import ChargedProtoCombineDLLsAlg, ProtoParticleMUONFilter 
from PhysSelPython.Wrappers import Selection, DataOnDemand
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles

from StrippingUtils.Utils import LineBuilder
from Configurables import LoKi__VoidFilter
from os import environ
from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from GaudiKernel.SystemOfUnits import mm

from GaudiConfUtils.ConfigurableGenerators import TisTosParticleTagger
from StandardParticles import StdAllLooseMuons
from Configurables import GaudiSequencer
from Configurables import TrackToDST
from Configurables import TrackSys
from Configurables import PatSeeding, PatSeedingTool, PatDownstream
from PhysSelPython.Wrappers import AutomaticData
# Get the fitters
from TrackFitter.ConfiguredFitters import ConfiguredFit, ConfiguredFitSeed, ConfiguredFitDownstream
from PatAlgorithms import PatAlgConf

from SelPy.utils import ( UniquelyNamedObject,
                          ClonableObject,
                          SelectionBase )


default_config = {
    'NAME'        : 'TrackEffDownMuon',
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'StrippingTrackEffDownMuonConf',
    'CONFIG'      : { 
				'MuMom':		2000.	# MeV
			,	'MuTMom':		200.	# MeV
			,	'ZMuMom':		0.	# MeV
			,	'ZMuTMom':		20000.	# MeV
			,	'UpsilonMuMom':		0.	# MeV
			,	'UpsilonMuTMom':	500.	# MeV
			,	'ZMuMinEta':		2.	# adimensional
			,	'ZMuMaxEta':		4.5	# adimensional
			,	'TrChi2':		10.	# MeV
			,	'MassPreComb':		2000.	# MeV
			,	'MassPostComb':		200.	# MeV
			,	'ZMassPreComb':		100000.	# MeV
			,	'ZMassPostComb':	1500.	# MeV
			,	'UpsilonMassPreComb':	100000.	# MeV
			,	'UpsilonMassPostComb':	00.	# MeV
			,	'Doca':			5.	# mm
			,	'VertChi2':		25.	# adimensional
			,	'SeedingMinP':		1500.	# MeV
                	,       'DataType':             '2011'  # for ConfiguredMuonIDs configuration
			,	'NominalLinePrescale':  1. # proposal: 0.2 to stay below 0.15% retention rate 
			,	'NominalLinePostscale': 1.
			,	'ValidationLinePrescale':0.0015 #0.5 in stripping15: 0.1 gives 1.42% retention rate , ValidationLine further prescaled
			,	'ValidationLinePostscale': 1.
			,	'ZLinePrescale':  1. # proposal: 0.2 to stay below 0.15% retention rate 
			,	'ZLinePostscale':  1.  
			,	'UpsilonLinePrescale':  1. # proposal: 0.2 to stay below 0.15% retention rate 
			,	'UpsilonLinePostscale':  1. 
            ,   'JpsiHlt1Filter' : 'Hlt1.*Decision'
            ,   'JpsiHlt2Filter' : 'Hlt2.*Decision'
			,	'HLT1TisTosSpecs': { "Hlt1TrackMuonDecision%TOS" : 0, "Hlt1SingleMuonNoIPL0Decision%TOS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
			,	'ZHLT1TisTosSpecs': { "Hlt1SingleMuonHighPTDecision%TOS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
			,	'UpsilonHLT1TisTosSpecs': { "Hlt1SingleMuonHighPTDecision%TOS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
			,	'HLT1PassOnAll': True
			,	'HLT2TisTosSpecs': { "Hlt2SingleMuon.*Decision%TOS" : 0, "Hlt2TrackEffDiMuonDownstream.*Decision%TOS" : 0 } #reg. expression allowed
			,	'ZHLT2TisTosSpecs': { "Hlt2SingleMuonHighPTDecision%TOS" : 0 } #reg. expression allowed
			,	'UpsilonHLT2TisTosSpecs': { "Hlt2SingleMuonLowPTDecision%TOS" : 0} #reg. expression allowed
			,	'HLT2PassOnAll': False
                    },
    'STREAMS'     : { 'Calibration' : ['StrippingTrackEffDownMuonNominalLine'
	                                   ,'StrippingTrackEffDownMuonValidationLine'
                                       ,'StrippingTrackEffDownMuonZLine'
                                       ,'StrippingTrackEffDownMuonUpsilonLine']
                                       }
    }


class StrippingTrackEffDownMuonConf(LineBuilder):
    """
    Definition of Downstream J/Psi stripping.
    """
    
    __configuration_keys__ = (
		        'MuMom',
				'MuTMom',
		    	'ZMuMom',
		    	'ZMuTMom',
		    	'UpsilonMuMom',
		    	'UpsilonMuTMom',
		    	'ZMuMinEta',
		    	'ZMuMaxEta',
				'TrChi2',
				'MassPreComb',
				'MassPostComb',
				'ZMassPreComb',
				'ZMassPostComb',
				'UpsilonMassPreComb',
				'UpsilonMassPostComb',
				'Doca',
				'VertChi2',
				'SeedingMinP',
				'DataType',
                'NominalLinePrescale',
                'NominalLinePostscale',
                'ValidationLinePrescale',
                'ValidationLinePostscale',
                'ZLinePrescale',
                'ZLinePostscale',
                'UpsilonLinePrescale',
                'UpsilonLinePostscale',
                'JpsiHlt1Filter',
                'JpsiHlt2Filter',
				'HLT1TisTosSpecs',
				'ZHLT1TisTosSpecs',
				'UpsilonHLT1TisTosSpecs',
		   	    'HLT1PassOnAll',
				'HLT2TisTosSpecs',
				'ZHLT2TisTosSpecs',
				'UpsilonHLT2TisTosSpecs',
				'HLT2PassOnAll'
                              )
				

    def __init__(self, name, config) :

          LineBuilder.__init__(self, name, config)
          
          nominal_name = name + 'Nominal'
          valid_name = name + 'Validation'
          Z_name = name + 'Z'
          Upsilon_name = name + 'Upsilon'
          
          self.TisTosPreFilter1Jpsi = selHlt1Jpsi('TisTosFilter1Jpsi'+name, HLT1TisTosSpecs = config['HLT1TisTosSpecs'], HLT1PassOnAll = config['HLT1PassOnAll'])
          self.TisTosPreFilter2Jpsi = selHlt2Jpsi('TisTosFilter2Jpsi'+name, hlt1Filter = self.TisTosPreFilter1Jpsi, HLT2TisTosSpecs = config['HLT2TisTosSpecs'], HLT2PassOnAll = config['HLT2PassOnAll'])
          
          self.TisTosPreFilter1Z = selHlt1Jpsi('TisTosFilter1Z'+name, HLT1TisTosSpecs = config['ZHLT1TisTosSpecs'], HLT1PassOnAll = config['HLT1PassOnAll'])
          self.TisTosPreFilter2Z = selHlt2Jpsi('TisTosFilter2Z'+name, hlt1Filter = self.TisTosPreFilter1Z, HLT2TisTosSpecs = config['ZHLT2TisTosSpecs'], HLT2PassOnAll = config['HLT2PassOnAll'])
          
          self.TisTosPreFilter1Upsilon = selHlt1Jpsi('TisTosFilter1Upsilon'+name, HLT1TisTosSpecs = config['UpsilonHLT1TisTosSpecs'], HLT1PassOnAll = config['HLT1PassOnAll'])
          self.TisTosPreFilter2Upsilon = selHlt2Jpsi('TisTosFilter2Upsilon'+name, hlt1Filter = self.TisTosPreFilter1Upsilon, HLT2TisTosSpecs = config['UpsilonHLT2TisTosSpecs'], HLT2PassOnAll = config['HLT2PassOnAll'])
          
          self.TrackingPreFilter = trackingDownPreFilter(nominal_name, self.TisTosPreFilter2Jpsi, config['SeedingMinP'])
          self.DownMuProtoPFilter = selMuonPParts(nominal_name, config['DataType'], self.TrackingPreFilter)
          self.DownMuPFilter = makeMyMuons(nominal_name, self.DownMuProtoPFilter)
          self.DownJpsiFilter = DownJPsi( 'DownJpsiSel'+nominal_name, self.DownMuPFilter, config['TrChi2'], 
          		config['MuTMom'], config['MuMom'], config['MassPreComb'], config['Doca'], 
          		config['MassPostComb'], config['VertChi2'] )
          
          self.ZTrackingPreFilter = trackingDownPreFilter( Z_name, self.TisTosPreFilter2Z, config['SeedingMinP'])
          self.ZDownMuProtoPFilter = selMuonPParts(Z_name, config['DataType'], self.ZTrackingPreFilter)
          self.ZDownMuPFilter = makeMyMuons(Z_name, self.ZDownMuProtoPFilter)
          self.DownZFilter = DownZ( 'DownSel'+Z_name, self.ZDownMuPFilter,  
          		config['ZMuMom'], config['ZMuTMom'], config['ZMuMinEta'], config['ZMuMaxEta'], 
          		config['ZMassPreComb'], config['ZMassPostComb'], )
          
          self.UpsilonTrackingPreFilter = trackingDownPreFilter( Upsilon_name, self.TisTosPreFilter2Upsilon, config['SeedingMinP'])
          self.UpsilonDownMuProtoPFilter = selMuonPParts(Upsilon_name, config['DataType'], self.UpsilonTrackingPreFilter)
          self.UpsilonDownMuPFilter = makeMyMuons(Upsilon_name, self.UpsilonDownMuProtoPFilter)
          self.DownUpsilonFilter = DownUpsilon( 'DownSel'+Upsilon_name, self.UpsilonDownMuPFilter,  
          		config['UpsilonMuMom'], config['UpsilonMuTMom'], 
          		config['UpsilonMassPreComb'], config['UpsilonMassPostComb'], )
          
          self.nominal_line =  StrippingLine(nominal_name + 'Line'
                                             , prescale = config['NominalLinePrescale']
                                             , postscale = config['NominalLinePostscale']
                                             , algos=[self.DownJpsiFilter]
                                             , HLT1 = "HLT_PASS_RE('%(JpsiHlt1Filter)s')" % config
                                             , HLT2 = "HLT_PASS_RE('%(JpsiHlt2Filter)s')" % config
                                             )
          
          self.valid_line = StrippingLine(valid_name + 'Line', prescale = config['ValidationLinePrescale'], postscale = config['ValidationLinePostscale'], algos=[self.TisTosPreFilter2Jpsi])
          
          self.Z_line =  StrippingLine(Z_name + 'Line',  prescale = config['ZLinePrescale'], postscale = config['ZLinePostscale'], algos=[self.DownZFilter])
          
          self.Upsilon_line =  StrippingLine(Upsilon_name + 'Line',  prescale = config['UpsilonLinePrescale'], postscale = config['UpsilonLinePostscale'], algos=[self.DownUpsilonFilter])
          
          self.registerLine(self.nominal_line)
          self.registerLine(self.valid_line)
          self.registerLine(self.Z_line)
          self.registerLine(self.Upsilon_line)

# ########################################################################################
# Make the protoparticles
# ########################################################################################
def selMuonPParts(name, DataType, downstreamSeq):
   """
       Make ProtoParticles out of Downstream tracks
   """
   unpacker = UnpackTrack(name+"UnpackTrack")  # do we need this or is it here for historical reason ?
   unpacker.InputName="pRec/"+name+"_Downstream/FittedTracks"
   unpacker.OutputName="Rec/"+name+"_Downstream/FittedTracks"

   cm=ConfiguredMuonIDs.ConfiguredMuonIDs( DataType ) #data=DaVinci().getProp("DataType"))
   idalg = cm.configureMuonIDAlgLite(name+"IDalg")
   idalg.TracksLocations = ["Rec/"+name+"_Downstream/FittedTracks"]
   idalg.MuonIDLocation = "Rec/"+name+"_Muon/DownstreamMuonPID"
   idalg.MuonTrackLocation = "Rec/"+name+"_Track/MuonForDownstream" # I would call it FromDownstream

   downprotoseq = GaudiSequencer(name+"ProtoPSeq")
   downprotos = ChargedProtoParticleMaker(name+"ProtoPMaker")
   downprotos.Inputs = ["Rec/"+name+"_Downstream/FittedTracks"]
   downprotos.Output = "Rec/ProtoP/"+name+"_ProtoPMaker/ProtoParticles"
   downprotos.addTool( DelegatingTrackSelector, name="TrackSelector" )
   #tracktypes = [ "Long","Upstream","Downstream","Ttrack","Velo","VeloR" ] # only downstream needed 
   tracktypes = ["Downstream"]
   #if (trackcont == "Best") :
   #	tracktypes = [ "Long" ]
   downprotos.TrackSelector.TrackTypes = tracktypes
   selector = downprotos.TrackSelector
   for tsname in tracktypes:
   	selector.addTool(TrackSelector,name=tsname)
   	ts = getattr(selector,tsname)
   	# Set Cuts
   	ts.TrackTypes = [tsname]
#	ts.MinNDoF = 1 
   	ts.MaxChi2Cut = 10

   addmuonpid = ChargedProtoParticleAddMuonInfo(name+"addmuoninfo")
   addmuonpid.InputMuonPIDLocation = "Rec/"+name+"_Muon/DownstreamMuonPID"
   addmuonpid.ProtoParticleLocation = "Rec/ProtoP/"+name+"_ProtoPMaker/ProtoParticles"
   #addmuonpid.OutputLevel = 0
   combinedll = ChargedProtoCombineDLLsAlg(name+"CombineDLL")
   combinedll.ProtoParticleLocation = "Rec/ProtoP/"+name+"_ProtoPMaker/ProtoParticles"
   #combinedll.OutputLevel = 0
   # DST post treatment
   #TrackToDST(name+"TrackToDST").TracksInContainer = "Rec/Downstream/Tracks"
   #downprotoseq.Members += [ TrackToDST(name+"TrackToDST"), downprotos, addmuonpid, combinedll ]
   downprotoseq.Members += [ downprotos, addmuonpid, combinedll ]
#        
   DataOnDemandSvc().AlgMap.update( {
                "/Event/Rec/"+name+"_Downstream/Tracks" : unpacker.getFullName(),
                "/Event/Rec/"+name+"_Muon/DownstreamMuonPID" : idalg.getFullName(),
#                "/Event/Rec/ProtoP/"+name+"ProtoPMaker" : downprotoseq.getFullName()
		} )

   return GSWrapper(name="WrappedDownMuonProtoPSeqFor"+name,
                    sequencer=downprotoseq,
                    output='Rec/ProtoP/' + name +'_ProtoPMaker/ProtoParticles',
                    requiredSelections = [ downstreamSeq])
   #     return Selection(name+"_SelPParts", Algorithm = MuonTTPParts, OutputBranch="Rec/ProtoP", Extension="ProtoParticles",RequiredSelections=[downstreamSeq], InputDataSetter=None)

def makeMyMuons(name, protoParticlesMaker):
   """
     Make Particles out of the muon ProtoParticles
   """
   particleMaker =  BestPIDParticleMaker(name+"ParticleMaker" , Particle = "muon")
   particleMaker.addTool(ProtoParticleMUONFilter,name="muon")
   particleMaker.muon.Selection = ["RequiresDet='MUON' IsMuonLoose=True"]
   particleMaker.Particles = [ "muon" ]
   particleMaker.Input = "Rec/ProtoP/"+name+"_ProtoPMaker/ProtoParticles"
   #particleMaker.OutputLevel = 0

   DataOnDemandSvc().AlgMap.update( {
           "/Event/Phys/" + particleMaker.name() + '/Particles' : particleMaker.getFullName(),
           "/Event/Phys/" + particleMaker.name() + '/Vertices'  : particleMaker.getFullName() 
   } )


   return Selection(name+"SelDownMuonParts", Algorithm = particleMaker, RequiredSelections = [protoParticlesMaker], InputDataSetter=None)

#
def DownJPsi( name,
		protoPartSel,
		TrChi2,
		MuTMom,
		MuMom,
		MassPreComb,
		Doca,
		MassPostComb,
		VertChi2 ) :
   #self.makeMyMuons("DownMuonsForTrackEff", "Downstream")
   
   _MuCuts =  "((TRCHI2DOF < %(TrChi2)s) & (PT > %(MuTMom)s*MeV) & (P > %(MuMom)s*MeV) )" % locals()
   _CombinationCuts = "((ADAMASS('J/psi(1S)') < %(MassPreComb)s * MeV) & (AMAXDOCA('') < %(Doca)s*mm) )" % locals()
   _MotherCuts = "((ADMASS('J/psi(1S)') < %(MassPostComb)s * MeV) & (VFASPF(VCHI2/VDOF) < %(VertChi2)s))" % locals()
   
   _MyDownJpsis = CombineParticles( DecayDescriptor = "J/psi(1S) -> mu+ mu-" ,
   DaughtersCuts = { "mu+": _MuCuts,  "mu-": _MuCuts }, CombinationCut = _CombinationCuts, MotherCut = _MotherCuts)

   return Selection ( name,
                      Algorithm = _MyDownJpsis,
                      RequiredSelections = [protoPartSel])
 
#########################################################################################
def DownZ( name,
		protoPartSel,
		MuMom,
		MuTMom,
		MuMinEta,
		MuMaxEta,
		MassPreComb,
		MassPostComb) :
   #self.makeMyMuons("DownMuonsForTrackEff", "Downstream")
   
   _MuCuts =  "((ETA < %(MuMaxEta)s*MeV) & (ETA > %(MuMinEta)s*MeV) & (P > %(MuMom)s*MeV) & (PT > %(MuTMom)s*MeV) )" % locals()
   _CombinationCuts = "((ADAMASS('Z0') < %(MassPreComb)s * MeV))" % locals()
   _MotherCuts = "((ADMASS('Z0') < %(MassPostComb)s * MeV))" % locals()
   
   _MyDownZ = CombineParticles( DecayDescriptor = "Z0 -> mu+ mu-" ,
   DaughtersCuts = { "mu+": _MuCuts,  "mu-": _MuCuts }, CombinationCut = _CombinationCuts, MotherCut = _MotherCuts)

   return Selection ( name,
                      Algorithm = _MyDownZ,
                      RequiredSelections = [protoPartSel])
 
#########################################################################################
def DownUpsilon( name,
		protoPartSel,
		MuMom,
		MuTMom,
		MassPreComb,
		MassPostComb) :
   #self.makeMyMuons("DownMuonsForTrackEff", "Downstream")
   
   _MuCuts =  "( (P > %(MuMom)s*MeV) & (PT > %(MuTMom)s*MeV) )" % locals()
   _CombinationCuts = "((ADAMASS('Z0') < %(MassPreComb)s * MeV))" % locals()
   _MotherCuts = "((ADMASS('Upsilon(1S)') < %(MassPostComb)s * MeV))" % locals()
   
   _MyDownZ = CombineParticles( DecayDescriptor = "Upsilon(1S) -> mu+ mu-" ,
   DaughtersCuts = { "mu+": _MuCuts,  "mu-": _MuCuts }, CombinationCut = _CombinationCuts, MotherCut = _MotherCuts)

   return Selection ( name,
                      Algorithm = _MyDownZ,
                      RequiredSelections = [protoPartSel])
 
#########################################################################################
"""
Define TisTos Prefilters

"""


def selHlt1Jpsi(name, HLT1TisTosSpecs, HLT1PassOnAll):
   """
   Filter the long track muon to be TOS on a HLT1 single muon trigger,
   for J/psi selection
   """
   Hlt1Jpsi = TisTosParticleTagger(
   TisTosSpecs = HLT1TisTosSpecs 
   ,ProjectTracksToCalo = False
   ,CaloClustForCharged = False
   ,CaloClustForNeutral = False
   ,TOSFrac = { 4:0.0, 5:0.0 }
   ,NoRegex = True
   )
   Hlt1Jpsi.PassOnAll = HLT1PassOnAll

   return Selection(name+"_SelHlt1Jpsi", Algorithm = Hlt1Jpsi, RequiredSelections = [ StdAllLooseMuons ])

#########################################################
def selHlt2Jpsi(name, hlt1Filter, HLT2TisTosSpecs, HLT2PassOnAll):
   """
   Filter the long track muon to be TOS on a HLT2 single muon trigger,
   for J/psi selection
   """
   Hlt2Jpsi = TisTosParticleTagger(
   TisTosSpecs =HLT2TisTosSpecs 
   ,ProjectTracksToCalo = False
   ,CaloClustForCharged = False
   ,CaloClustForNeutral = False
   ,TOSFrac = { 4:0.0, 5:0.0 }
   ,NoRegex = False
   )
   Hlt2Jpsi.PassOnAll = HLT2PassOnAll
   
   return Selection(name+"_SelHlt2Jpsi", Algorithm = Hlt2Jpsi, RequiredSelections = [ hlt1Filter ])
##########################################################
        

def trackingDownPreFilter(name, prefilter, seedcut):
    #Test code for debugging
    #Jpsi_already_there = LoKi__VoidFilter("Jpsi_already_there")
    #Jpsi_already_there.Code = "1 <= CONTAINS('Rec/Track/Downstream')"

    #Jpsi_not_yet_there = LoKi__VoidFilter("Jpsi_not_yet_there")
    #Jpsi_not_yet_there.Code = "1 > CONTAINS('Rec/Track/Downstream')"

    TrackToDST(name+"_DownTrackToDST").TracksInContainer = "Rec/"+name+"_Downstream/FittedTracks"

    jpsidotracking=GaudiSequencer("DownTrackingFor" + name)
    
    #Add seed tracking
    DownSeeding = PatSeeding(name+"_DownSeeding")
    DownSeeding.OutputTracksName = "Rec/"+name+"_DownSeeding/Tracks" 
    PatAlgConf.SeedingConf().configureAlg()
    DownSeeding.addTool(PatSeedingTool, name=name+"_PatSeedingTool")
    #DownSeeding.addTool(PatSeedingTool)
    PatSeedingTool(name+"_PatSeedingTool").MinMomentum = seedcut

    jpsidotracking.Members += [DownSeeding]
    #Add Seed Fit
    jpsidotracking.Members += [GaudiSequencer(name+"_TrackSeedFitSeq")]
    #AddPatDownstream
    downstreamTracking = PatDownstream(name+"_PatDownstream")
    downstreamTracking.InputLocation = DownSeeding.OutputTracksName
    downstreamTracking.OutputLocation = 'Rec/'+name+'_Downstream/Tracks'
    jpsidotracking.Members += [ downstreamTracking ];
    #AddDownstreamFitSeq
    jpsidotracking.Members += [TrackStateInitAlg(name+"_InitSeedDownstream")]
    TrackStateInitAlg(name+"_InitSeedDownstream").TrackLocation = "Rec/"+name+"_Downstream/Tracks"
    downstreamFit = ConfiguredFitDownstream(name+"_FitDownstream")
    downstreamFit.TracksInContainer = 'Rec/'+name+'_Downstream/Tracks'
    downstreamFit.TracksOutContainer = 'Rec/'+name+'_Downstream/FittedTracks'
    jpsidotracking.Members += [downstreamFit]
    jpsidotracking.Members += [TrackToDST(name+"_DownTrackToDST")]


    return GSWrapper(name="WrappedDownstreamTracking"+name,
                     sequencer=jpsidotracking,
                     output='Rec/'+name+'_Downstream/FittedTracks',
                     requiredSelections = [ prefilter])



class GSWrapper(UniquelyNamedObject,
                ClonableObject,
                SelectionBase) :
    
    def __init__(self, name, sequencer, output, requiredSelections) :
        UniquelyNamedObject.__init__(self, name)
        ClonableObject.__init__(self, locals())
        SelectionBase.__init__(self,
                               algorithm = sequencer,
                               outputLocation = output,
                               requiredSelections = requiredSelections )



