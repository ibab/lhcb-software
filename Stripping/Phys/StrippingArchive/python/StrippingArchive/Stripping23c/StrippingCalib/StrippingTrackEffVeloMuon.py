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
# @author M. Kolpin
# @date 2015-Mar-23
# 
#######################################################################

__author__ = ['Michael Kolpin']
__date__ = '23/03/2015'
__version__ = '$Revision: 2.0 $'

__all__ = ('StrippingTrackEffVeloMuonConf',
           'default_config',
           'chargeFilter',
           'longtrackFilter',
           'selMuonPParts',
           'makeMyMuons',
           'makeResonanceVeloMuTrackEff',
           'selHlt1Jpsi',
           'selHlt2Jpsi',
           'trackingPreFilter',
           )



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

from SelPy.utils import ( UniquelyNamedObject,
                          ClonableObject,
                          SelectionBase )

default_config = {
    'NAME'        : 'TrackEffVeloMuon',
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'StrippingTrackEffVeloMuonConf',
    'CONFIG'      : {
			"TrChi2VeMu":		5.	# adimensional
		,	"TrChi2LongMu":		3.	# adimensional
		,	"JpsiPt":		0.5	# GeV
		,	"ZPt":			0.5	# GeV
		,	"UpsilonPt":		0.5	# GeV
		,	"TrPt":			100.	# MeV
		,	"TrP":			5.	# GeV
		,	"ZTrPt":		20000.	# MeV
		,	"ZTrP":			0.	# MeV
		,	"UpsilonTrPt":		500.	# MeV
		,	"UpsilonTrP":		0.	# MeV
		,	"LongP":		7.	# GeV
		,	"ZTrMinEta":		2.0	# adimensional
		,	"ZTrMaxEta":		4.5	# adimensional
		,	"MuDLL":		1.	# adimensional
		,	"VertChi2":		2.	# adimensional
		,	"ZVertChi2":		10000.	# adimensional, dummy
		,	"UpsilonVertChi2":	10000.	# adimensional, dummy
		,	"MassPreComb":		1000.	# MeV
		,	"ZMassPreComb":		100000. # MeV
		,	"UpsilonMassPreComb":	100000. # MeV
		,	"MassPostComb":		400.	# MeV
		,	"ZMassPostComb":	40000.	# MeV
		,	"UpsilonMassPostComb":	1500.	# MeV
		,	"Prescale":		1.	# adimensional
		,	"ZPrescale":		1.	# adimensional
		,	"UpsilonPrescale":	1.	# adimensional
		,	"Postscale":		1.	# adimensional
		,	"ZPostscale":		1.	# adimensional
		,	"UpsilonPostscale":	1.	# adimensional
        ,   'JpsiHlt1Filter' : 'Hlt1.*Decision'
        ,   'JpsiHlt2Filter' : 'Hlt2.*Decision'
		,	'HLT1TisTosSpecs'	: { "Hlt1TrackMuonDecision%TOS" : 0, "Hlt1SingleMuonNoIPDecision%TOS" : 0 } #no reg. expression allowed(see selHlt1Jpsi )
		,	'ZHLT1TisTosSpecs'	: { "Hlt1SingleMuonHighPTDecision%TOS" : 0 } #no reg. expression allowed(see selHlt1Jpsi )
		,	'UpsilonHLT1TisTosSpecs': { "Hlt1SingleMuonHighPTDecision%TOS" : 0 } #no reg. expression allowed(see selHlt1Jpsi )
		,	'HLT1PassOnAll'		: True
		,	'HLT2TisTosSpecs'	: { "Hlt2SingleMuon.*Decision%TOS" : 0, "Hlt2TrackEffDiMuonVeloMuon.*Decision%TOS" : 0 } #reg. expression allowed
		,	'ZHLT2TisTosSpecs'	: { "Hlt2SingleMuonHighPTDecision%TOS" : 0} #reg. expression allowed
		,	'UpsilonHLT2TisTosSpecs': { "Hlt2SingleMuonLowPTDecision%TOS" : 0} #reg. expression allowed
		,	'HLT2PassOnAll'		: False
         },
    'STREAMS'     : { 'Calibration' : ['StrippingTrackEffVeloMuonLine1',
                                       'StrippingTrackEffVeloMuonLine2',
                                       'StrippingTrackEffVeloMuonZLine1',
                                       'StrippingTrackEffVeloMuonZLine2',
                                       'StrippingTrackEffVeloMuonUpsilonLine1',
                                       'StrippingTrackEffVeloMuonUpsilonLine2']}
    }



class StrippingTrackEffVeloMuonConf(LineBuilder):
    """
    Definition of tag and probe JPsi stripping.
    """
    __configuration_keys__ = (
				'TrChi2VeMu',
                'TrChi2LongMu',
				'JpsiPt',
				'ZPt',
				'UpsilonPt',
				'TrPt',
				'TrP',
				'ZTrPt',
				'ZTrP',
				'UpsilonTrPt',
				'UpsilonTrP',
				'LongP',
				'ZTrMinEta',
				'ZTrMaxEta',
				'MuDLL',
				'VertChi2',
				'ZVertChi2',
				'UpsilonVertChi2',
				'MassPreComb',
				'ZMassPreComb',
				'UpsilonMassPreComb',
				'MassPostComb',
				'ZMassPostComb',
				'UpsilonMassPostComb',
                'Prescale',
                'ZPrescale',
                'UpsilonPrescale',
                'Postscale',
                'ZPostscale',
                'UpsilonPostscale',
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
	
	# CHECK TRIGGER
        self.TisTosPreFilter1Jpsi = selHlt1Jpsi('TisTosFilter1Jpsifor'+name, HLT1TisTosSpecs = config['HLT1TisTosSpecs'], HLT1PassOnAll = config['HLT1PassOnAll'])
        self.TisTosPreFilter2Jpsi = selHlt2Jpsi('TisTosFilter2Jpsifor'+name, hlt1Filter = self.TisTosPreFilter1Jpsi, HLT2TisTosSpecs = config['HLT2TisTosSpecs'], HLT2PassOnAll = config['HLT2PassOnAll'])
        
        self.TisTosPreFilter1Z = selHlt1Jpsi('TisTosFilter1Zfor'+name, HLT1TisTosSpecs = config['ZHLT1TisTosSpecs'], HLT1PassOnAll = config['HLT1PassOnAll'])
        self.TisTosPreFilter2Z = selHlt2Jpsi('TisTosFilter2Zfor'+name, hlt1Filter = self.TisTosPreFilter1Z, HLT2TisTosSpecs = config['ZHLT2TisTosSpecs'], HLT2PassOnAll = config['HLT2PassOnAll'])
        
        self.TisTosPreFilter1Upsilon = selHlt1Jpsi('TisTosFilter1Upsilonfor'+name, HLT1TisTosSpecs = config['UpsilonHLT1TisTosSpecs'], HLT1PassOnAll = config['HLT1PassOnAll'])
        self.TisTosPreFilter2Upsilon = selHlt2Jpsi('TisTosFilter2Upsilonfor'+name, hlt1Filter = self.TisTosPreFilter1Upsilon, HLT2TisTosSpecs = config['UpsilonHLT2TisTosSpecs'], HLT2PassOnAll = config['HLT2PassOnAll'])
        
        # CHECK FOR TAG-TRACKS
        muCut = "((TRCHI2DOF < %(TrChi2LongMu)s)) & (PT > %(TrPt)s) & (P > %(LongP)s) & (PIDmu > %(MuDLL)s)" % config
        vmCut = "((TRCHI2DOF < %(TrChi2VeMu)s)) & (PT > %(TrPt)s) & (P > %(TrP)s)" % config
        ZmuCut = "((PT > %(ZTrPt)s) & (ETA > %(ZTrMinEta)s) & (ETA < %(ZTrMaxEta)s) & (P > %(ZTrP)s) )" % config
        UpsilonmuCut = "((PT > %(UpsilonTrPt)s) & (P > %(UpsilonTrP)s) )" % config
        
        self.longbothJpsi = longtrackFilter( name+'LongJpsiBoth', trackAlgo = 'LongMu', partSource = StdLooseMuons, muCut = muCut)
        self.longMinusJpsi = chargeFilter( name+'LongJpsiMinus', trackAlgo = 'LongMu',   partSource = self.longbothJpsi, charge = -1, vmCut = vmCut, muCut = muCut)
        self.longPlusJpsi = chargeFilter( name+'LongJpsiPlus', trackAlgo =  'LongMu',   partSource = self.longbothJpsi, charge = 1, vmCut = vmCut, muCut = muCut)
        
        self.longbothZ = longtrackFilter( name+'LongZBoth', trackAlgo = 'LongMu', partSource = StdLooseMuons, muCut = ZmuCut)
        self.longMinusZ = chargeFilter( name+'LongZMinus', trackAlgo = 'LongMu',   partSource = self.longbothZ, charge = -1, vmCut = ZmuCut, muCut = ZmuCut)
        self.longPlusZ = chargeFilter( name+'LongZPlus', trackAlgo =  'LongMu',   partSource = self.longbothZ, charge = 1, vmCut = ZmuCut, muCut = ZmuCut)
        
        self.longbothUpsilon = longtrackFilter( name+'LongUpsilonBoth', trackAlgo = 'LongMu', partSource = StdLooseMuons, muCut = UpsilonmuCut)
        self.longMinusUpsilon = chargeFilter( name+'LongUpsilonMinus', trackAlgo = 'LongMu',   partSource = self.longbothUpsilon, charge = -1, vmCut = UpsilonmuCut, muCut = UpsilonmuCut)
        self.longPlusUpsilon = chargeFilter( name+'LongUpsilonPlus', trackAlgo =  'LongMu',   partSource = self.longbothUpsilon, charge = 1, vmCut = UpsilonmuCut, muCut = UpsilonmuCut)
        
        # RECONSTRUCT PROBE-TRACKS
        self.TrackingPreFilter = trackingPreFilter(name+"_Jpsi", [self.TisTosPreFilter2Jpsi,self.longbothJpsi]) 
        self.VeloMuProtoPFilter = selMuonPParts(name+"_Jpsi", self.TrackingPreFilter)
        self.VeloMuPFilter = makeMyMuons(name+"_Jpsi", self.VeloMuProtoPFilter)
        
        self.ZTrackingPreFilter = trackingPreFilter(name+"_Z", [self.TisTosPreFilter2Z,self.longbothZ]) 
        self.ZVeloMuProtoPFilter = selMuonPParts(name+"_Z", self.ZTrackingPreFilter)
        self.ZVeloMuPFilter = makeMyMuons(name+"_Z", self.ZVeloMuProtoPFilter)
           
        self.UpsilonTrackingPreFilter = trackingPreFilter(name+"_Upsilon", [self.TisTosPreFilter2Upsilon,self.longbothUpsilon]) 
        self.UpsilonVeloMuProtoPFilter = selMuonPParts(name+"_Upsilon", self.UpsilonTrackingPreFilter)
        self.UpsilonVeloMuPFilter = makeMyMuons(name+"_Upsilon", self.UpsilonVeloMuProtoPFilter)
           
        self.veloMuonMinusJpsi = chargeFilter(name+'MuonVeloJpsiMinus', trackAlgo =  'VeloMuon',   partSource =  self.VeloMuPFilter , charge = -1, vmCut = vmCut, muCut = muCut)
        self.veloMuonPlusJpsi = chargeFilter(name+'MuonVeloJpsiPlus', trackAlgo = 'VeloMuon',  partSource = self.VeloMuPFilter,  charge = 1, vmCut = vmCut, muCut = muCut)
        
        self.veloMuonMinusZ = chargeFilter(name+'MuonVeloZMinus', trackAlgo =  'VeloMuon',   partSource =  self.ZVeloMuPFilter , charge = -1, vmCut = ZmuCut, muCut = ZmuCut)
        self.veloMuonPlusZ = chargeFilter(name+'MuonVeloZPlus', trackAlgo = 'VeloMuon',  partSource = self.ZVeloMuPFilter,  charge = 1, vmCut = ZmuCut, muCut = ZmuCut)
        
        self.veloMuonMinusUpsilon = chargeFilter(name+'MuonVeloUpsilonMinus', trackAlgo =  'VeloMuon',   partSource =  self.UpsilonVeloMuPFilter , charge = -1, vmCut = UpsilonmuCut, muCut = UpsilonmuCut)
        self.veloMuonPlusUpsilon = chargeFilter(name+'MuonVeloUpsilonPlus', trackAlgo = 'VeloMuon',  partSource = self.UpsilonVeloMuPFilter,  charge = 1, vmCut = UpsilonmuCut, muCut = UpsilonmuCut)
        
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
        
        self.ZMuMuTrackEff1 = makeResonanceVeloMuTrackEff(name + "VeloMuZSel1", 
        						  resonanceName = 'Z0', 
        						  decayDescriptor = 'Z0 -> mu+ mu-',
        						  plusCharge = self.veloMuonPlusZ, 
        						  minusCharge = self.longMinusZ,
        						  mode = 1,
        						  TrPt = config['ZTrPt'], 
        						  TrP = config['ZTrP'],
        						  MuDLL = config['MuDLL'],
        						  MassPreComb = config['ZMassPreComb'], 
        						  VertChi2 = config['ZVertChi2'], 
        						  MassPostComb = config['ZMassPostComb'], 
        						  JpsiPt = config['ZPt'])   
        
        self.ZMuMuTrackEff2 = makeResonanceVeloMuTrackEff(name + "VeloMuZSel2", 
        						  resonanceName = 'Z0', 
        						  decayDescriptor = 'Z0 -> mu+ mu-',
        						  plusCharge = self.longPlusZ, 
        						  minusCharge = self.veloMuonMinusZ,
        						  mode = 2,
        						  TrPt = config['ZTrPt'], 
        						  TrP = config['ZTrP'],
        						  MuDLL = config['MuDLL'],
        						  MassPreComb = config['ZMassPreComb'], 
        						  VertChi2 = config['ZVertChi2'], 
        						  MassPostComb = config['ZMassPostComb'], 
        						  JpsiPt = config['ZPt'])   
        
        self.UpsilonMuMuTrackEff1 = makeResonanceVeloMuTrackEff(name + "VeloMuUpsilonSel1", 
        						  resonanceName = 'Upsilon(1S)', 
        						  decayDescriptor = 'Upsilon(1S) -> mu+ mu-',
        						  plusCharge = self.veloMuonPlusUpsilon, 
        						  minusCharge = self.longMinusUpsilon,
        						  mode = 1,
        						  TrPt = config['UpsilonTrPt'], 
        						  TrP = config['UpsilonTrP'],
        						  MuDLL = config['MuDLL'],
        						  MassPreComb = config['UpsilonMassPreComb'], 
        						  VertChi2 = config['UpsilonVertChi2'], 
        						  MassPostComb = config['UpsilonMassPostComb'], 
        						  JpsiPt = config['UpsilonPt'])   
        
        self.UpsilonMuMuTrackEff2 = makeResonanceVeloMuTrackEff(name + "VeloMuUpsilonSel2", 
        						  resonanceName = 'Upsilon(1S)', 
        						  decayDescriptor = 'Upsilon(1S) -> mu+ mu-',
        						  plusCharge = self.longPlusUpsilon, 
        						  minusCharge = self.veloMuonMinusUpsilon,
        						  mode = 2,
        						  TrPt = config['UpsilonTrPt'], 
        						  TrP = config['UpsilonTrP'],
        						  MuDLL = config['MuDLL'],
        						  MassPreComb = config['UpsilonMassPreComb'], 
        						  VertChi2 = config['UpsilonVertChi2'], 
        						  MassPostComb = config['UpsilonMassPostComb'], 
        						  JpsiPt = config['UpsilonPt'])   
        
        self.nominal_line1 =  StrippingLine(name + 'Line1'
                                            , prescale = config['Prescale']
                                            , postscale = config['Postscale']
                                            , algos=[self.JpsiMuMuTrackEff1]
                                            , RequiredRawEvents = ["Trigger","Muon","Velo","Tracker"]
                                            , HLT1 = "HLT_PASS_RE('%(JpsiHlt1Filter)s')" % config
                                            , HLT2 = "HLT_PASS_RE('%(JpsiHlt2Filter)s')" % config
                                            , MDSTFlag = True)

        self.nominal_line2 =  StrippingLine(name + 'Line2'
                                            , prescale = config['Prescale']
                                            , postscale = config['Postscale']
                                            , algos=[self.JpsiMuMuTrackEff2]
                                            , RequiredRawEvents = ["Trigger","Muon","Velo","Tracker"]
                                            , HLT1 = "HLT_PASS_RE('%(JpsiHlt1Filter)s')" % config
                                            , HLT2 = "HLT_PASS_RE('%(JpsiHlt2Filter)s')" % config
                                            , MDSTFlag = True)
        
        self.Z_line1 =  StrippingLine(name + 'ZLine1',  prescale = config['ZPrescale'], postscale = config['ZPostscale'], algos=[self.ZMuMuTrackEff1], RequiredRawEvents = ["Trigger","Muon","Velo","Tracker"], MDSTFlag = True)
        self.Z_line2 =  StrippingLine(name + 'ZLine2',  prescale = config['ZPrescale'], postscale = config['ZPostscale'], algos=[self.ZMuMuTrackEff2], RequiredRawEvents = ["Trigger","Muon","Velo","Tracker"], MDSTFlag = True)
        
        self.Upsilon_line1 =  StrippingLine(name + 'UpsilonLine1',  prescale = config['UpsilonPrescale'], postscale = config['UpsilonPostscale'], algos=[self.UpsilonMuMuTrackEff1], RequiredRawEvents = ["Trigger","Muon","Velo","Tracker"], MDSTFlag = True)
        self.Upsilon_line2 =  StrippingLine(name + 'UpsilonLine2',  prescale = config['UpsilonPrescale'], postscale = config['UpsilonPostscale'], algos=[self.UpsilonMuMuTrackEff2], RequiredRawEvents = ["Trigger","Muon","Velo","Tracker"], MDSTFlag = True)
        
        self.registerLine(self.nominal_line1)
        self.registerLine(self.nominal_line2)
        
        self.registerLine(self.Z_line1)
        self.registerLine(self.Z_line2)
        
        self.registerLine(self.Upsilon_line1)
        self.registerLine(self.Upsilon_line2)
	
# ########################################################################################
# Make the protoparticles
# ########################################################################################
def selMuonPParts(name, trackingSeq):
   """
       Make ProtoParticles out of VeloMuon tracks
   """
   veloprotos = ChargedProtoParticleMaker(name+"ProtoPMaker")
   veloprotos.Inputs = ["Rec/VeloMuon/"+name+"Tracks"]
   veloprotos.Output = "Rec/ProtoP/ProtoPMaker/"+name+"ProtoParticles"
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
                    output='Rec/ProtoP/ProtoPMaker/'+name+'ProtoParticles',
                    requiredSelections = [ trackingSeq])
#   return Selection(name+"_SelPParts", Algorithm = veloprotos, OutputBranch="Rec/ProtoP", Extension="ProtoParticles",RequiredSelections=[trackingSeq], InputDataSetter=None)

def makeMyMuons(name, protoParticlesMaker):
   """
     Make Particles out of the muon ProtoParticles
   """
   particleMaker =  NoPIDsParticleMaker(name+"ParticleMaker" , Particle = "Muon")
   particleMaker.Input = "Rec/ProtoP/ProtoPMaker/"+name+"ProtoParticles"
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

       MuonVeloResonance.CombinationCut = "ADAMASS('%(resonanceName)s')<%(MassPreComb)s*MeV"% locals()
       MuonVeloResonance.MotherCut = "(VFASPF(VCHI2/VDOF)< %(VertChi2)s) & (ADMASS('%(resonanceName)s')<%(MassPostComb)s*MeV) & (PT > %(JpsiPt)s*GeV)"% locals()
        
       return Selection( name, Algorithm = MuonVeloResonance, RequiredSelections = [minusCharge, plusCharge] )
     
   if(mode == 2):
       #MuonVeloResonance.DaughtersCuts = {"mu-": muCut  % locals(),
       #                                   "mu+": vmCut  % locals() }

       MuonVeloResonance.CombinationCut = "ADAMASS('%(resonanceName)s')<%(MassPreComb)s*MeV"% locals()
       MuonVeloResonance.MotherCut = "(VFASPF(VCHI2/VDOF)< %(VertChi2)s) & (ADMASS('%(resonanceName)s')<%(MassPostComb)s*MeV) & (PT > %(JpsiPt)s*GeV)"% locals()
            
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

   VeloMuonBuilder1 = VeloMuonBuilder("VeloMuonBuilder"+name)
   VeloMuonBuilder1.OutputLevel = 6
   #VeloMuonBuilder1.MuonLocation = "Rec/Track/"+name+"MuonStandalone"
   #VeloMuonBuilder1.MuonLocation = "Hlt1/Track/MuonSeg"
   VeloMuonBuilder1.MuonLocation = "Rec/Track/"+name+"MuonStandalone"
   VeloMuonBuilder1.VeloLocation = "Rec/Track/"+name+"UnFittedVelo"
   VeloMuonBuilder1.lhcbids = 4
   VeloMuonBuilder1.OutputLocation = "Rec/VeloMuon/"+name+"Tracks"

   preve = TrackPrepareVelo(name+"preve")
   preve.inputLocation = "Rec/"+name+"_Track/Velo"
   preve.outputLocation = "Rec/Track/"+name+"UnFittedVelo"
   preve.bestLocation = ""

   #TODO: apparently FastVelo is now (april 2012) run with fixes in the production which don't neccessarily apply to the stripping...
   alg = GaudiSequencer("VeloMuonTrackingFor"+name,
                         Members = [ FastVeloTracking(name+"FastVelo",OutputTracksName="Rec/"+name+"_Track/Velo"),
				 preve, 
				 #StandaloneMuonRec(name+"MuonStandalone", OutputMuonTracksName = "Rec/Track/"+name+"MuonStandalone"), VeloMuonBuilder1])
				 StandaloneMuonRec(name+"MuonStandalone",OutputMuonTracksName="Rec/Track/"+name+"MuonStandalone"), VeloMuonBuilder1])

   return GSWrapper(name="WrappedVeloMuonTracking"+name,
                     sequencer=alg,
                     output='Rec/VeloMuon/'+name+'Tracks',
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




