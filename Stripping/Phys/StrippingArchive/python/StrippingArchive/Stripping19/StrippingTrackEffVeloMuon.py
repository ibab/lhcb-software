## #####################################################################
# A stripping selection for VeloMuon J/Psi->mu+mu- decays
# To be used for tracking studies
#
# @authors G. Krocker, P. Seyfert, S. Wandernoth
# @date 2010-Aug-17
#
# @authors P. Seyfert, A. Jaeger
# @date 2011-Mar-17
# 
#######################################################################


from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import CombineParticles	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from Configurables import ChargedProtoParticleMaker, NoPIDsParticleMaker, DataOnDemandSvc, DelegatingTrackSelector, TrackSelector, CombinedParticleMaker, BestPIDParticleMaker
from Configurables import FastVeloTracking
  
from StrippingConf.StrippingLine import StrippingLine
from Configurables import TrackStateInitAlg, TrackEventFitter, TrackPrepareVelo,TrackContainerCopy, Tf__PatVeloSpaceTool, StandaloneMuonRec
from Configurables import TrackCloneFinder
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseMuons

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, TisTosParticleTagger

from Configurables import GaudiSequencer
from Configurables import TrackSys
from PhysSelPython.Wrappers import AutomaticData
# Get the fitters
from TrackFitter.ConfiguredFitters import ConfiguredFit

from Configurables import TrackEventCloneKiller,VeloMuonBuilder
from Configurables import TrackEventFitter, TrackMasterFitter
from Configurables import TrackKalmanFilter, TrackMasterExtrapolator
#from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
#from Configurables import TrackCloneFinder
from Configurables import DecodeVeloRawBuffer

from SelPy.utils import ( UniquelyNamedObject,
                          ClonableObject,
                          SelectionBase )
confdict={
			"TrChi2VeMu":		5.	# adimensional
		,	"TrChi2LongMu":		3.	# adimensional
		,	"JpsiPt":		0.5	# GeV
		,	"TrPt":			100.	# MeV
		,	"TrP":			5.	# GeV
		,	"LongP":		7.	# GeV
		,	"MuDLL":		1.	# adimensional
		,	"VertChi2":		2.	# adimensional
		,	"MassPreComb":		1000.	# MeV
		,	"MassPostComb":		400.	# MeV
		,	"Prescale":		0.22	# adimensional
		,	"Postscale":		1.	# adimensional
		,	'HLT1TisTosSpecs': { "Hlt1TrackMuonDecision%TOS" : 0, "Hlt1SingleMuonNoIPDecision%TOS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
		,	'HLT1PassOnAll': True
		,	'HLT2TisTosSpecs': { "Hlt2SingleMuon.*Decision%TOS" : 0} #reg. expression allowed
		,	'HLT2PassOnAll': False
         }

class StrippingTrackEffVeloMuonConf(LineBuilder):
    """
    Definition of tag and probe JPsi stripping.
    """
    __configuration_keys__ = (
				'TrChi2VeMu',
                                'TrChi2LongMu',
				'JpsiPt',
				'TrPt',
				'TrP',
				'LongP',
				'MuDLL',
				'VertChi2',
				'MassPreComb',
				'MassPostComb',
                              	'Prescale',
                              	'Postscale',
				'HLT1TisTosSpecs',
		   	        'HLT1PassOnAll',
				'HLT2TisTosSpecs',
				'HLT2PassOnAll'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
	
	# CHECK TRIGGER
	self.TisTosPreFilter1Jpsi = selHlt1Jpsi('TisTosFilter1Jpsifor'+name, HLT1TisTosSpecs = config['HLT1TisTosSpecs'], HLT1PassOnAll = config['HLT1PassOnAll'])
	self.TisTosPreFilter2Jpsi = selHlt2Jpsi('TisTosFilter2Jpsifor'+name, hlt1Filter = self.TisTosPreFilter1Jpsi, HLT2TisTosSpecs = config['HLT2TisTosSpecs'], HLT2PassOnAll = config['HLT2PassOnAll'])

	# CHECK FOR TAG-TRACKS
	muCut = "((TRCHI2DOF < %(TrChi2LongMu)s)) & (PT > %(TrPt)s) & (P > %(LongP)s) & (PIDmu > %(MuDLL)s)" % config
	vmCut = "((TRCHI2DOF < %(TrChi2VeMu)s)) & (PT > %(TrPt)s) & (P > %(TrP)s)" % config
        self.longbothJpsi = longtrackFilter( name+'LongJpsiBoth', trackAlgo = 'LongMu', partSource = StdLooseMuons, muCut = muCut)
	self.longMinusJpsi = chargeFilter( name+'LongJpsiMinus', trackAlgo = 'LongMu',   partSource = self.longbothJpsi, charge = -1, vmCut = vmCut, muCut = muCut)
	self.longPlusJpsi = chargeFilter( name+'LongJpsiPlus', trackAlgo =  'LongMu',   partSource = self.longbothJpsi, charge = 1, vmCut = vmCut, muCut = muCut)
	
	# RECONSTRUCT PROBE-TRACKS
	self.TrackingPreFilter = trackingPreFilter('TrackingPreFilter'+name, [self.TisTosPreFilter2Jpsi,self.longbothJpsi]) 
	self.VeloMuProtoPFilter = selMuonPParts('VeloMuon'+name, self.TrackingPreFilter)
	self.VeloMuPFilter = makeMyMuons('VeloMuon'+name, self.VeloMuProtoPFilter)
	   
	self.veloMuonMinusJpsi = chargeFilter(name+'MuonVeloJpsiMinus', trackAlgo =  'VeloMuon',   partSource =  self.VeloMuPFilter , charge = -1, vmCut = vmCut, muCut = muCut)
        self.veloMuonPlusJpsi = chargeFilter(name+'MuonVeloJpsiPlus', trackAlgo = 'VeloMuon',  partSource = self.VeloMuPFilter,  charge = 1, vmCut = vmCut, muCut = muCut)

	# TAG-AND-PROBE
	self.JpsiMuMuTrackEff1 = makeResonanceVeloMuTrackEff(name + "VeloMuJpsiSel1", 
							   resonanceName = 'J/psi(1S)', 
							   decayDescriptor = 'J/psi(1S) -> mu+ mu-',
							   plusCharge = self.veloMuonPlusJpsi, 
							   minusCharge = self.longMinusJpsi,
							   mode = 1,
							   TrPt = config['TrPt'], 
							   TrP = config['TrP'],
							   MuDLL = config['MuDLL'],
							   MassPreComb = config['MassPreComb'], 
							   VertChi2 = config['VertChi2'], 
							   MassPostComb = config['MassPostComb'], 
							   JpsiPt = config['JpsiPt'])   

	self.JpsiMuMuTrackEff2 = makeResonanceVeloMuTrackEff(name + "VeloMuJpsiSel2", 
							   resonanceName = 'J/psi(1S)', 
							   decayDescriptor = 'J/psi(1S) -> mu+ mu-',
							   plusCharge = self.longPlusJpsi, 
							   minusCharge = self.veloMuonMinusJpsi,
							   mode = 2,
							   TrPt = config['TrPt'],  
							   TrP = config['TrP'],
							   MuDLL = config['MuDLL'],
							   MassPreComb = config['MassPreComb'], 
							   VertChi2 = config['VertChi2'], 
							   MassPostComb = config['MassPostComb'], 
							   JpsiPt = config['JpsiPt'])    
	
	self.nominal_line1 =  StrippingLine(name + 'Line1',  prescale = config['Prescale'], postscale = config['Postscale'], algos=[self.JpsiMuMuTrackEff1])
	self.nominal_line2 =  StrippingLine(name + 'Line2',  prescale = config['Prescale'], postscale = config['Postscale'], algos=[self.JpsiMuMuTrackEff2])

	self.registerLine(self.nominal_line1)
	self.registerLine(self.nominal_line2)
	
# ########################################################################################
# Make the protoparticles
# ########################################################################################
def selMuonPParts(name, trackingSeq):
   """
       Make ProtoParticles out of VeloMuon tracks
   """
   veloprotos = ChargedProtoParticleMaker(name+"ProtoPMaker")
   veloprotos.Inputs = ["Rec/VeloMuon/Tracks"]
   veloprotos.Output = "Rec/ProtoP/"+name+"ProtoPMaker/ProtoParticles"
   veloprotos.addTool( DelegatingTrackSelector, name="TrackSelector" )
   tracktypes = [ "Long" ]
   veloprotos.TrackSelector.TrackTypes = tracktypes
   selector = veloprotos.TrackSelector
   for tsname in tracktypes:
   	selector.addTool(TrackSelector,name=tsname)
   	ts = getattr(selector,tsname)
   	# Set Cuts
   	ts.TrackTypes = [tsname]

   veloprotoseq = GaudiSequencer(name+"ProtoPSeq")
   veloprotoseq.Members += [ veloprotos ]

   return GSWrapper(name="WrappedVeloMuonProtoPSeqFor" + name,
                    sequencer=veloprotoseq,
                    output='Rec/ProtoP/' + name +'ProtoPMaker/ProtoParticles',
                    requiredSelections = [ trackingSeq])
#   return Selection(name+"_SelPParts", Algorithm = veloprotos, OutputBranch="Rec/ProtoP", Extension="ProtoParticles",RequiredSelections=[trackingSeq], InputDataSetter=None)

def makeMyMuons(name, protoParticlesMaker):
   """
     Make Particles out of the muon ProtoParticles
   """
   particleMaker =  NoPIDsParticleMaker(name+"ParticleMaker" , Particle = "Muon")
   particleMaker.Input = "Rec/ProtoP/"+name+"ProtoPMaker/ProtoParticles"
   #particleMaker.OutputLevel = 0

   DataOnDemandSvc().AlgMap.update( {
           "/Event/Phys/" + particleMaker.name() + '/Particles' : particleMaker.getFullName(),
           "/Event/Phys/" + particleMaker.name() + '/Vertices'  : particleMaker.getFullName() 
   } )


   return Selection(name+"SelVeloMuonParts", Algorithm = particleMaker, RequiredSelections = [protoParticlesMaker], InputDataSetter=None)

def makeResonanceVeloMuTrackEff(name, resonanceName, decayDescriptor, plusCharge, minusCharge, 
                              mode, TrPt, TrP, MuDLL, MassPreComb, VertChi2, MassPostComb, JpsiPt):    
   """
   Create and return a Resonance -> mu mu Selection object, with one track a long track
   and the other a MuonVelo track.
   Arguments:
   name                 : name of the selection
   resonanceName        : name of the resonance
   decayDescriptor      : decayDescriptor of the decay
   plusCharge           : algorithm for selection positvely charged tracks
   minusCharge          : algorithm for selection negatively charged tracks
   mode                 : Tag(-)-and-Probe(+) (1) or  Tag(+)-and-Probe(-) (2)
   """
    
    
   MuonVeloResonance = CombineParticles('_'+name)
   MuonVeloResonance.DecayDescriptor = decayDescriptor
   MuonVeloResonance.OutputLevel = 4 


   if(mode == 1):
       #MuonVeloResonance.DaughtersCuts = {"mu+": muCut,
       #                                   "mu-": vmCut}

       MuonVeloResonance.CombinationCut = "ADAMASS('J/psi(1S)')<%(MassPreComb)s*MeV"% locals()
       MuonVeloResonance.MotherCut = "(VFASPF(VCHI2/VDOF)< %(VertChi2)s) & (ADMASS('J/psi(1S)')<%(MassPostComb)s*MeV) & (PT > %(JpsiPt)s*GeV)"% locals()
        
       return Selection( name, Algorithm = MuonVeloResonance, RequiredSelections = [minusCharge, plusCharge] )
     
   if(mode == 2):
       #MuonVeloResonance.DaughtersCuts = {"mu-": muCut  % locals(),
       #                                   "mu+": vmCut  % locals() }

       MuonVeloResonance.CombinationCut = "ADAMASS('J/psi(1S)')<%(MassPreComb)s*MeV"% locals()
       MuonVeloResonance.MotherCut = "(VFASPF(VCHI2/VDOF)< %(VertChi2)s) & (ADMASS('J/psi(1S)')<%(MassPostComb)s*MeV) & (PT > %(JpsiPt)s*GeV)"% locals()
            
       return Selection( name, Algorithm = MuonVeloResonance, RequiredSelections = [plusCharge, minusCharge] )
# ########################################################################################
# Charge filter, that filters, well, the charge and takes the particles from the right source (long or Velomuon)
# TODO: I introduced vmCut and muCut here although muCut is already been done somewhere else ... clean this some day
# ########################################################################################
def chargeFilter(name, trackAlgo,  partSource, charge, vmCut, muCut):
    """
        Select plus or minus charge for Velomuon or long track
    """
    Filter = FilterDesktop() #there is maybe a change needed
    myFilter1 = Filter.configurable("myFilter1")
            
    if(charge == -1):
        myFilter1.Code = "(Q < 0) & "
    if(charge == 1):
        myFilter1.Code = "(Q > 0) & "    
            
    if(trackAlgo == 'VeloMuon'):
        myFilter1.Code += vmCut
    if(trackAlgo == "LongMu"):
        myFilter1.Code += muCut
    if(trackAlgo == 'VeloMuon'):
        return Selection( name+'_chargeFilter'+'VeloMuon', Algorithm = myFilter1, RequiredSelections = [  partSource ] )
    if(trackAlgo == 'LongMu'):
        return Selection( name+'_chargeFilter'+'LongMu', Algorithm = myFilter1, RequiredSelections = [  partSource ] )
# ################################################################

# ##########################
# high quality muons
# ##########################
def longtrackFilter(name, trackAlgo, partSource, muCut):
    """
        Select plus or minus charge for longtrack
    """
    Filter = FilterDesktop() #there is maybe a change needed
    myFilter1 = Filter.configurable("mylongFilter1")
            
    myFilter1.Code = muCut
    return Selection( name+'_longFilter'+'LongMu', Algorithm = myFilter1, RequiredSelections = [  partSource ] )
# ################################################################

"""
Define TisTos Prefilters

"""
#getMuonParticles = DataOnDemand(Location = 'Phys/StdLooseMuons')


#def selHlt1Jpsi(name, longPartsFilter):
def selHlt1Jpsi(name, HLT1TisTosSpecs, HLT1PassOnAll):
   """
   Filter the long track muon to be TOS on a HLT1 single muon trigger,
   for J/psi selection
   """
   #Hlt1Jpsi = TisTosParticleTagger(name+"Hlt1Jpsi")
   Hlt1Jpsi = TisTosParticleTagger(
   TisTosSpecs = HLT1TisTosSpecs #{ "Hlt1TrackMuonDecision%TOS" : 0, "Hlt1SingleMuonNoIPL0Decision%TOS" : 0}
   ,ProjectTracksToCalo = False
   ,CaloClustForCharged = False
   ,CaloClustForNeutral = False
   ,TOSFrac = { 4:0.0, 5:0.0 }
   ,NoRegex = True
   )
   Hlt1Jpsi.PassOnAll = HLT1PassOnAll
   #Hlt1Jpsi.PassOnAll = True # TESTING!
   #
   return Selection(name+"_SelHlt1Jpsi", Algorithm = Hlt1Jpsi, RequiredSelections = [ StdLooseMuons ])

#########################################################
def selHlt2Jpsi(name, hlt1Filter, HLT2TisTosSpecs, HLT2PassOnAll):
   """
   Filter the long track muon to be TOS on a HLT2 single muon trigger,
   for J/psi selection
   """
   #Hlt2Jpsi = TisTosParticleTagger("Hlt2Jpsi")
   Hlt2Jpsi = TisTosParticleTagger(
   TisTosSpecs =HLT2TisTosSpecs #{ "Hlt2SingleMuon.*Decision%TOS" : 0}
   ,ProjectTracksToCalo = False
   ,CaloClustForCharged = False
   ,CaloClustForNeutral = False
   ,TOSFrac = { 4:0.0, 5:0.0 }
   ,NoRegex = False
   )
   Hlt2Jpsi.PassOnAll = HLT2PassOnAll
   #Hlt2Jpsi.PassOnAll = True # TESTING!
   #
   return Selection(name+"_SelHlt2Jpsi", Algorithm = Hlt2Jpsi, RequiredSelections = [ hlt1Filter ])
##########################################################
        

def trackingPreFilter(name, prefilter):

   VeloMuonBuilder1 = VeloMuonBuilder("VeloMuonBuilder")
   VeloMuonBuilder1.OutputLevel = 6
   VeloMuonBuilder1.MuonLocation = "Hlt1/Track/MuonSeg"
   VeloMuonBuilder1.VeloLocation = "Rec/Track/UnFittedVelo"
   VeloMuonBuilder1.lhcbids = 4
   VeloMuonBuilder1.OutputLocation = "Rec/VeloMuon/Tracks"

   preve = TrackPrepareVelo("preve")
   preve.inputLocation = "Rec/Track/Velo"
   preve.outputLocation = "Rec/Track/UnFittedVelo"
   preve.bestLocation = ""

   #TODO: apparently FastVelo is now (april 2012) run with fixes in the production which don't neccessarily apply to the stripping...
   alg = GaudiSequencer("VeloMuonTrackingFor"+name,
                         Members = [ DecodeVeloRawBuffer(name+"VeloDecoding",DecodeToVeloLiteClusters=True,DecodeToVeloClusters=True),
			         FastVeloTracking(name+"FastVelo",OutputTracksName="Rec/Track/Velo"),
				 preve, 
				 StandaloneMuonRec(name+"MuonStandalone"), VeloMuonBuilder1])

   return GSWrapper(name="WrappedVeloMuonTracking",
                     sequencer=alg,
                     output='Rec/VeloMuon/Tracks',
                     requiredSelections =  prefilter)



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




