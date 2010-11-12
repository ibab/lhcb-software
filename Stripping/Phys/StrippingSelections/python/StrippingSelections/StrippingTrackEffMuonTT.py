
'''
Module for construction of Jpsi -> mu mu /  Upsilon -> mu mu /  Z -> mu mu to be used to measure tracking efficiency.  
Provides functions to build a Jpsi / Upsilon / Z selection, with a long track and a muon(tt) track.
Provides class StrippingTrackEffMuonTTConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - chargeFilter
   - tisTosFilter1
   - tisTosFilter2
   - makeResonanceMuMuTrackEff
   - StrippingTrackEffMuonTTConf
   
   
'''

__author__ = ['Michel De Cian']
__date__ = '02/10/2010'
__version__ = '$Revision: 1.1 $'


__all__ = ('StrippingTrackEffMuonTTConf', 
           'chargeFilter',
           'tisTosFilter1',
           'tisTosFilter2',
           'makeResonanceMuMuTrackEff')

from Gaudi.Configuration import *
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils import checkConfig


from Configurables import (MuonCombRec, 
                           MuonTTTrack, 
                           PatAddTTCoord, 
                           TrackMasterFitter, 
                           TrackMasterExtrapolator, 
                           TrackSelector, 
                           ChargedProtoParticleMaker, 
                           NoPIDsParticleMaker,
                           ProtoParticleMUONFilter,
                           FilterDesktop, 
                           CombineParticles, 
                           TisTosParticleTagger
                           )

class StrippingTrackEffMuonTTConf(object) :
    """
    Builder of Jpsi->mu mu, Upsilon-> mu mu and Z-> mu mu stripping Selection and StrippingLine for measuring the tracking efficiency.
    Constructs the Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> trackEffConf = StrippingTrackEffMuonTTConf('Test',config)
    >>> trackEffLines = trackEffConf.lines
    >>> for line in trackEffLines :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Lines of interest:

    TrackEffMuonTTJpsiLine1       : Jpsi-> mu mu, long: minus, muonTT: plus
    TrackEffMuonTTJpsiLine2       : Jpsi-> mu mu, long: plus, muonTT: minus

    TrackEffMuonTTUpsilonLine1    : Upsilon-> mu mu, long: minus, muonTT: plus
    TrackEffMuonTTUpsilonLine2    : Upsilon-> mu mu, long: plus, muonTT: minus
    
    TrackEffMuonTTZLine1          : Z-> mu mu, long: minus, muonTT: plus
    TrackEffMuonTTZLine2          : Z-> mu mu, long: plus, muonTT: minus

    TrackEffMuonZLine1            : Z-> mu mu, long: minus, muon: plus
    TrackEffMuonZLine2            : Z-> mu mu, long: plus, muon: minus

    lines                         : List of all lines

    Exports as class data member:
    StrippingTrackEffMuonTTConf.__configuration_keys__ : List of required configuration parameters.
    """

    ################################################################################################

    __configuration_keys__ = ('JpsiMassWin',
                              'UpsilonMassWin',
                              'ZMassWin',
                              'JpsiMuonTTPT',
                              'UpsilonMuonTTPT',
                              'ZMuonTTPT',
                              'JpsiLongPT',
                              'UpsilonLongPT',
                              'ZLongPT',
                              'JpsiPT',
                              'UpsilonPT',
                              'ZPT',
                              'JpsiLongMuonMinIPCHI2',
                              'UpsilonLongMuonMinIPCHI2',
                              'ZLongMuonMinIPCHI2',
                              'JpsiLongMuonTrackCHI2',
                              'UpsilonLongMuonTrackCHI2',
                              'ZLongMuonTrackCHI2',
                              'VertexChi2',
                              'LongMuonPID',
                              'JpsiPrescale',
                              'UpsilonPrescale',
                              'ZPrescale',
                              'Postscale'
                              )

    def __init__(self, name = 'MuonTTTrackEff', config = None):
            
        checkConfig(StrippingTrackEffMuonTTConf.__configuration_keys__,config)
        
        self.TisTosFilter1Jpsi = tisTosFilter1( name = 'TisTosFilter1Jpsi', flag = 'Jpsi', trigger =  { "Hlt1TrackMuonDecision%TOS" : 0 } )
        self.TisTosFilter1Z = tisTosFilter1( name = 'TisTosFilter1Z',  flag = 'Z', trigger =  { "Hlt1SingleMuonNoIPL0HighPTDecision%TOS" : 0 } )

        self.TisTosFilter2Jpsi = tisTosFilter2( name = 'TisTosFilter2Jpsi', trigger = { "Hlt2SingleMuonDecision%TOS" : 1 }, firstFilter = self.TisTosFilter1Jpsi)
        self.TisTosFilter2Z = tisTosFilter2( name = 'TisTosFilter2Z', trigger = { "Hlt2SingleHighPTMuonDecision%TOS" : 2 }, firstFilter = self.TisTosFilter1Z)

        self.muonTTPlus = chargeFilter( name = 'muonTTPlus', trackAlgo = 'MuonTT',  tisTosAlgo = self.TisTosFilter2Jpsi,  charge = 1)
        self.muonTTMinus = chargeFilter( name = 'muonTTMinus', trackAlgo =  'MuonTT',  tisTosAlgo = self.TisTosFilter2Jpsi, charge = -1)
        self.longPlus = chargeFilter( name = 'longPlus', trackAlgo =  'Long',  tisTosAlgo = self.TisTosFilter2Jpsi, charge = 1)
        self.longMinus = chargeFilter( name = 'longMinus', trackAlgo = 'Long',  tisTosAlgo = self.TisTosFilter2Jpsi, charge = -1)
        
        self.muonTTPlusZ = chargeFilter( name = 'muonTTPlusZ', trackAlgo = 'MuonTT',  tisTosAlgo = self.TisTosFilter2Z,  charge = 1)
        self.muonTTMinusZ = chargeFilter( name = 'muonTTMinusZ', trackAlgo =  'MuonTT',  tisTosAlgo = self.TisTosFilter2Z, charge = -1)
        self.longPlusZ = chargeFilter( name = 'longPlusZ', trackAlgo =  'Long',  tisTosAlgo = self.TisTosFilter2Z, charge = 1)
        self.longMinusZ = chargeFilter( name = 'longMinusZ', trackAlgo = 'Long',  tisTosAlgo = self.TisTosFilter2Z, charge = -1)

        
                
        # def makeResonanceMuMuTrackEff(name, resonanceName, decayDescriptor, plusCharge, minusCharge, 
        # mode, massWin, vertexChi2, resonancePT, muonTTPT, longPT, longMuonPID, longMuonMinIPCHI2, longMuonTrackCHI2):   
        self.makeJpsiMuMuTrackEff1 = makeResonanceMuMuTrackEff('MakeJpsiMuMuTrackEff1', 
                                                               resonanceName = 'J/psi(1S)',
                                                               decayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                                               plusCharge = self.muonTTPlus, 
                                                               minusCharge = self.longMinus,
                                                               mode = 1,
                                                               massWin = config['JpsiMassWin'], 
                                                               vertexChi2 = config['VertexChi2'],
                                                               resonancePT = config['JpsiPT'],
                                                               muonTTPT = config['JpsiMuonTTPT'], 
                                                               longPT = config['JpsiLongPT'], 
                                                               longMuonPID = config['LongMuonPID'],
                                                               longMuonMinIPCHI2 = config['JpsiLongMuonMinIPCHI2'],
                                                               longMuonTrackCHI2 = config['JpsiLongMuonTrackCHI2'])
        
        self.makeJpsiMuMuTrackEff2 = makeResonanceMuMuTrackEff('MakeJpsiMuMuTrackEff2', 
                                                               resonanceName = 'J/psi(1S)',
                                                               decayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                                               plusCharge = self.longPlus, 
                                                               minusCharge = self.muonTTMinus,
                                                               mode = 2,
                                                               massWin = config['JpsiMassWin'], 
                                                               vertexChi2 = config['VertexChi2'],
                                                               resonancePT = config['JpsiPT'],
                                                               muonTTPT = config['JpsiMuonTTPT'], 
                                                               longPT = config['JpsiLongPT'], 
                                                               longMuonPID = config['LongMuonPID'],
                                                               longMuonMinIPCHI2 = config['JpsiLongMuonMinIPCHI2'],
                                                               longMuonTrackCHI2 = config['JpsiLongMuonTrackCHI2'])

        self.makeUpsilonMuMuTrackEff1 = makeResonanceMuMuTrackEff('MakeUpsilonMuMuTrackEff1', 
                                                                  resonanceName = 'Upsilon(1S)',
                                                                  decayDescriptor = 'Upsilon(1S) -> mu+ mu-',
                                                                  plusCharge = self.muonTTPlus, 
                                                                  minusCharge = self.longMinus,
                                                                  mode = 1,
                                                                  massWin = config['UpsilonMassWin'], 
                                                                  vertexChi2 = config['VertexChi2'],
                                                                  resonancePT = config['UpsilonPT'],
                                                                  muonTTPT = config['UpsilonMuonTTPT'], 
                                                                  longPT = config['UpsilonLongPT'], 
                                                                  longMuonPID = config['LongMuonPID'],
                                                                  longMuonMinIPCHI2 = config['UpsilonLongMuonMinIPCHI2'],
                                                                  longMuonTrackCHI2 = config['UpsilonLongMuonTrackCHI2'])

        self.makeUpsilonMuMuTrackEff2 = makeResonanceMuMuTrackEff('MakeUpsilonMuMuTrackEff2', 
                                                                  resonanceName = 'Upsilon(1S)',
                                                                  decayDescriptor = 'Upsilon(1S) -> mu+ mu-',
                                                                  plusCharge = self.longPlus, 
                                                                  minusCharge = self.muonTTMinus,
                                                                  mode = 2,
                                                                  massWin = config['UpsilonMassWin'], 
                                                                  vertexChi2 = config['VertexChi2'],
                                                                  resonancePT = config['UpsilonPT'],
                                                                  muonTTPT = config['UpsilonMuonTTPT'], 
                                                                  longPT = config['UpsilonLongPT'], 
                                                                  longMuonPID = config['LongMuonPID'],
                                                                  longMuonMinIPCHI2 = config['UpsilonLongMuonMinIPCHI2'],
                                                                  longMuonTrackCHI2 = config['UpsilonLongMuonTrackCHI2'])




        self.makeZMuMuTrackEff1 = makeResonanceMuMuTrackEff('MakeZMuMuTrackEff1', 
                                                            resonanceName = 'Z0',
                                                            decayDescriptor = 'Z0 -> mu+ mu-',
                                                            plusCharge = self.muonTTPlusZ, 
                                                            minusCharge = self.longMinusZ,
                                                            mode = 1,
                                                            massWin = config['ZMassWin'], 
                                                            vertexChi2 = config['VertexChi2'],
                                                            resonancePT = config['ZPT'],
                                                            muonTTPT = config['ZMuonTTPT'], 
                                                            longPT = config['ZLongPT'], 
                                                            longMuonPID = config['LongMuonPID'],
                                                            longMuonMinIPCHI2 = config['ZLongMuonMinIPCHI2'],
                                                            longMuonTrackCHI2 = config['ZLongMuonTrackCHI2'])

        self.makeZMuMuTrackEff2 = makeResonanceMuMuTrackEff('MakeZMuMuTrackEff2', 
                                                            resonanceName = 'Z0',
                                                            decayDescriptor = 'Z0 -> mu+ mu-',
                                                            plusCharge = self.longPlusZ, 
                                                            minusCharge = self.muonTTMinusZ,
                                                            mode = 2,
                                                            massWin = config['ZMassWin'], 
                                                            vertexChi2 = config['VertexChi2'],
                                                            resonancePT = config['ZPT'],
                                                            muonTTPT = config['ZMuonTTPT'], 
                                                            longPT = config['ZLongPT'], 
                                                            longMuonPID = config['LongMuonPID'],
                                                            longMuonMinIPCHI2 = config['ZLongMuonMinIPCHI2'],
                                                            longMuonTrackCHI2 = config['ZLongMuonTrackCHI2'])
        


        ################################################
        # Jpsi -> mu mu stripping lines (long + muonTT)
        ################################################
        self.TrackEffMuonTTJpsi_line1 = StrippingLine('TrackEffMuonTTJpsiLine1',
                                                       #HLT = "HLT_PASS('Hlt1SingleMuonNoIPL0Decision')",
                                                       prescale = config['JpsiPrescale'],
                                                       postscale = config['Postscale'],
                                                       algos = [LongParts, FilterLongParts,tisTosPreFilterHlt1Jpsi, tisTosPreFilterHlt2Jpsi, 
                                                                MakeMuonTT, MuonTTPParts, MuonTTParts, self.makeJpsiMuMuTrackEff1]
                                                       )

        self.TrackEffMuonTTJpsi_line2 = StrippingLine('TrackEffMuonTTJpsiLine2',
                                                       #HLT = "HLT_PASS('Hlt1SingleMuonNoIPL0Decision')",
                                                       prescale = config['JpsiPrescale'],
                                                       postscale = config['Postscale'],
                                                       algos = [LongParts, FilterLongParts, tisTosPreFilterHlt1Jpsi,tisTosPreFilterHlt2Jpsi,
                                                                MakeMuonTT, MuonTTPParts, MuonTTParts, self.makeJpsiMuMuTrackEff2]
                                                       )
    
        ################################################
        # Upsilon -> mu mu stripping lines (long + muonTT)
        ################################################
        self.TrackEffMuonTTUpsilon_line1 = StrippingLine('TrackEffMuonTTUpsilonLine1',
                                                         #HLT = "HLT_PASS('Hlt1SingleMuonNoIPL0Decision')",
                                                         prescale = config['UpsilonPrescale'],
                                                         postscale = config['Postscale'],
                                                         algos = [LongParts, FilterLongParts, tisTosPreFilterHlt1Jpsi,tisTosPreFilterHlt2Jpsi, 
                                                                  MakeMuonTT, MuonTTPParts, MuonTTParts, self.makeUpsilonMuMuTrackEff1]
                                                         )

        self.TrackEffMuonTTUpsilon_line2 = StrippingLine('TrackEffMuonTTUpsilonLine2',
                                                         #HLT = "HLT_PASS('Hlt1SingleMuonNoIPL0Decision')",
                                                         prescale = config['UpsilonPrescale'],
                                                         postscale = config['Postscale'],
                                                         algos = [LongParts, FilterLongParts, tisTosPreFilterHlt1Jpsi,tisTosPreFilterHlt2Jpsi, 
                                                                  MakeMuonTT, MuonTTPParts, MuonTTParts, self.makeUpsilonMuMuTrackEff2]
                                                         )
        
        ################################################
        # Z -> mu mu stripping lines (long + muonTT)
        ################################################
        self.TrackEffMuonTTZ_line1 = StrippingLine('TrackEffMuonTTZLine1',
                                                   #HLT = "HLT_PASS('Hlt1SingleMuonNoIPL0Decision')",
                                                   prescale = config['ZPrescale'],
                                                   postscale = config['Postscale'],
                                                   algos = [LongParts, FilterLongParts, tisTosPreFilterHlt1Z,tisTosPreFilterHlt2Z, 
                                                            MakeMuonTT, MuonTTPParts, MuonTTParts, self.makeZMuMuTrackEff1]
                                                   )


        self.TrackEffMuonTTZ_line2 = StrippingLine('TrackEffMuonTTZLine2',
                                                   #HLT = "HLT_PASS('Hlt1SingleMuonNoIPL0Decision')",
                                                   prescale = config['ZPrescale'],
                                                   postscale = config['Postscale'],
                                                   algos = [LongParts, FilterLongParts, tisTosPreFilterHlt1Z, tisTosPreFilterHlt2Z, 
                                                            MakeMuonTT, MuonTTPParts, MuonTTParts, self.makeZMuMuTrackEff2]
                                                   )

        ################################################
        # Z -> mu mu stripping lines (long + muon)
        ################################################
        self.TrackEffMuonZ_line1 = StrippingLine('TrackEffMuonZLine1',
                                                 #HLT = "HLT_PASS('Hlt1SingleMuonNoIPL0Decision')",
                                                 prescale = config['ZPrescale'],
                                                 postscale = config['Postscale'],
                                                 algos = [LongParts, FilterLongParts, tisTosPreFilterHlt1Z,tisTosPreFilterHlt2Z , 
                                                          MakeMuon, MuonTTPParts, MuonTTParts, self.makeZMuMuTrackEff1]
                                                 )


        self.TrackEffMuonZ_line2 = StrippingLine('TrackEffMuonZLine2',
                                                 #HLT = "HLT_PASS('Hlt1SingleMuonNoIPL0Decision')",
                                                 prescale = config['ZPrescale'],
                                                 postscale = config['Postscale'],
                                                 algos = [LongParts, FilterLongParts, tisTosPreFilterHlt1Z,tisTosPreFilterHlt2Z, 
                                                          MakeMuon, MuonTTPParts, MuonTTParts, self.makeZMuMuTrackEff2])
        
        
        self.lines = [self.TrackEffMuonTTJpsi_line1,self.TrackEffMuonTTJpsi_line2, self.TrackEffMuonTTUpsilon_line1, self.TrackEffMuonTTUpsilon_line2, 
                      self.TrackEffMuonTTZ_line1, self.TrackEffMuonTTZ_line2, self.TrackEffMuonZ_line1, self.TrackEffMuonZ_line2   ]

        ################################################
        # return the lines
        ################################################
        def lines():
            return self.lines


####################################################
# Data on Demand Selections Framework Wrappers
####################################################
getMuonTTParticles = DataOnDemand(Location = 'Phys/MuonTTParts')
getJpsiLongParticles = DataOnDemand(Location = 'Phys/tisTosPreFilterHlt2Jpsi')
getZLongParticles = DataOnDemand(Location = 'Phys/tisTosPreFilterHlt2Z')

                                                 
def chargeFilter(name, trackAlgo, tisTosAlgo, charge):
    """
    Select plus or minus charge for muonTT or long track
    """
    Filter = FilterDesktop('_' + name)
        
    if(charge == -1):
        Filter.Code = "(Q < 0)"
    if(charge == 1):
        Filter.Code = "(Q > 0)"    
        
    if(trackAlgo == 'MuonTT'):
        return Selection( name, Algorithm = Filter, RequiredSelections = [ getMuonTTParticles ] )
    if(trackAlgo == 'Long'):
        return Selection( name, Algorithm = Filter, RequiredSelections = [ tisTosAlgo ] )
                                                    

def tisTosFilter1(name, flag, trigger):
    """
    Filter to require the candidate is TOS (Hlt1) for the long track muon!
    """
    
    tisTosFilter1 = TisTosParticleTagger('_' + name)

    tisTosFilter1.TisTosSpecs = trigger
    tisTosFilter1.NoRegex = True

    # Do TisTosTobbing only with Trackers & Muon (no Calo)
    tisTosFilter1.ProjectTracksToCalo = False
    tisTosFilter1.CaloClustForCharged = False
    tisTosFilter1.CaloClustForNeutral = False
    tisTosFilter1.TOSFrac = { 4:0.0, 5:0.0 }

    if(flag == 'Jpsi'):
        return Selection(name, Algorithm = tisTosFilter1, RequiredSelections = [ getJpsiLongParticles ] )
    if(flag == 'Z'):
        return Selection(name, Algorithm = tisTosFilter1, RequiredSelections = [ getZLongParticles ] )


def tisTosFilter2(name, trigger, firstFilter):
    """
    Filter to require the candidate is TOS (Hlt2) for the long track muon!
    """
    
    tisTosFilter2 = TisTosParticleTagger('_' + name)

    tisTosFilter2.TisTosSpecs = trigger
    tisTosFilter2.NoRegex = True

    # Do TisTosTobbing only with Trackers & Muon (no Calo)
    tisTosFilter2.ProjectTracksToCalo = False
    tisTosFilter2.CaloClustForCharged = False
    tisTosFilter2.CaloClustForNeutral = False
    tisTosFilter2.TOSFrac = { 4:0.0, 5:0.0 }

    return Selection(name, Algorithm = tisTosFilter2, RequiredSelections = [ firstFilter ] )



def makeResonanceMuMuTrackEff(name, resonanceName, decayDescriptor, plusCharge, minusCharge, 
                              mode, massWin, vertexChi2, resonancePT, muonTTPT, longPT, longMuonPID, longMuonMinIPCHI2, longMuonTrackCHI2):    
    """
    Create and return a Jpsi -> mu mu Selection object, with one track a long track
    and the other a MuonTT track.
    Arguments:
    name                 : name of the selection
    resonanceName        : name of the resonance
    decayDescriptor      : decayDescriptor of the decay
    plusCharge           : algorithm for selection positvely charged tracks
    minusCharge          : algorithm for selection negatively charged tracks
    mode                 : Tag(-)-and-Probe(+) (1) or  Tag(+)-and-Probe(-) (2)
    massWin              : mass window around J/psi mass
    VertexChi2           : vertex chi2 / ndof of mu mu vertex
    resonancePT          : Pt of the resonance particle (f.ex. J/psi)
    muonTTPT             : Pt of MuonTT-track muon
    longPT               : Pt of Long-track muon
    longMuonPID          : CombDLL(mu-pi) of long-track muon
    NOT IMPLEMENTED YET:
    longMuonMinIPCHI2    : MinIPCHI2 of long-track muon
    longMuonTrackCHI2    : Track-Chi2 of long-track muon
    """
    
    massWinCombCut = 2 * massWin
    
    MuonTTResonance = CombineParticles('_'+name)
    MuonTTResonance.DecayDescriptor = decayDescriptor

    MuonTTResonance.OutputLevel = 4 

   

    if(mode == 1):
        MuonTTResonance.DaughtersCuts = {"mu+"	:	"PT > %(muonTTPT)s"  % locals(),
                                         "mu-"	:	"(PT > %(longPT)s) & (PIDmu > %(longMuonPID)s)"  % locals() }
        
    if(mode == 2):
        MuonTTResonance.DaughtersCuts = {"mu-"	:	"PT > %(muonTTPT)s"  % locals(),
                                         "mu+"	:	"(PT > %(longPT)s) & (PIDmu > %(longMuonPID)s)"  % locals() }
     
    
    MuonTTResonance.CombinationCut = "ADAMASS('%(resonanceName)s') < %(massWinCombCut)s" % locals()
    MuonTTResonance.MotherCut = "(ADMASS('%(resonanceName)s') < %(massWin)s) & (VFASPF(VCHI2/VDOF) < %(vertexChi2)s)" % locals()


    return Selection( name, Algorithm = MuonTTResonance, RequiredSelections = [minusCharge, plusCharge] )




# ##########################################################################################
# As we have different inputs (tracks, protoparticles, nothing...)
# I do not know how to wrap that in the Selections framework... So we just use 
# plain Gaudi Algorithms and put them in the algos list at the end
# ##########################################################################################
muonComb = MuonCombRec(OutputLevel = INFO)  
muonComb.MeasureTime = True
muonComb.CloneKiller = True
muonComb.SkipStation = -1 # -1=no skip, 0=M1, 1=M2, 2=M3, 3=M4, 4=M5
muonComb.DecodingTool = "MuonHitDecode"
muonComb.PadRecTool = "MuonPadRec"
muonComb.ClusterTool = "MuonClusterRec" # to enable: "MuonClusterRec"
muonComb.PhysicsTiming = True
muonComb.AssumeCosmics = False
muonComb.AssumePhysics = True
muonComb.AddXTalk = True # has no effect if "MuonClusterRec" is selected
muonComb.SeedStation = 4 # default seet station is M5
# muonComb.TracksOutputLocation = "Rec/Track/MuonStandalone"
# muonComb.TracksOutputLocation = "Rec/Track/MuonTTTracks"
muonComb.OutputLevel = 3
# ################################################################
MakeMuonTT = MuonTTTrack("MakeMuonTT")
MakeMuonTT.ToolName = "MuonCombRec"
MakeMuonTT.addTool( muonComb )
MakeMuonTT.OutputLevel = 4
MakeMuonTT.AddTTHits = True
MakeMuonTT.MC = False
MakeMuonTT.addTool( PatAddTTCoord )
MakeMuonTT.PatAddTTCoord.YTolSlope = 400000.0
MakeMuonTT.PatAddTTCoord.XTol = 12.0
MakeMuonTT.PatAddTTCoord.XTolSlope = 400000.0
MakeMuonTT.PatAddTTCoord.MinAxProj = 2.5
MakeMuonTT.PatAddTTCoord.MajAxProj = 22.0
MakeMuonTT.addTool( TrackMasterFitter )
MakeMuonTT.TrackMasterFitter.OutputLevel = 4
MakeMuonTT.TrackMasterFitter.MaterialLocator = "SimplifiedMaterialLocator"
MakeMuonTT.addTool( TrackMasterExtrapolator ) 
MakeMuonTT.TrackMasterExtrapolator.MaterialLocator = "SimplifiedMaterialLocator"
MakeMuonTT.OutputLocation = "Rec/Track/MuonTTTracks"
# ################################################################
# To be used if only muon tracks and not MuonTT tracks should be reconstructed
MakeMuon = MakeMuonTT.clone()
MakeMuon.AddTTHits = False
# ################################################################
MuonTTPParts = ChargedProtoParticleMaker("MuonTTPParts")
MuonTTPParts.addTool( TrackSelector )
MuonTTPParts.TrackSelector.TrackTypes = [ "Long" ]
MuonTTPParts.InputTrackLocation = ["Rec/Track/MuonTTTracks"]
MuonTTPParts.OutputProtoParticleLocation = "Rec/ProtoP/MuonTTProtoP"
MuonTTPParts.OutputLevel = 4
# ################################################################
MuonTTParts = NoPIDsParticleMaker("MuonTTParts")
MuonTTParts.Particle = 'muon'
MuonTTParts.addTool( TrackSelector )
MuonTTParts.TrackSelector.TrackTypes = [ "Long" ]
MuonTTParts.Input =  "Rec/ProtoP/MuonTTProtoP"
MuonTTParts.OutputLevel = 4
# ################################################################
LongParts = NoPIDsParticleMaker("LongParts")
LongParts.addTool(ProtoParticleMUONFilter(Selection = ["RequiresDet='MUON' IsMuon=True"]),name="muon")
LongParts.Particle = 'muon'
LongParts.addTool( TrackSelector )
LongParts.TrackSelector.TrackTypes = [ "Long" ]
LongParts.addTool( TrackSelector )
LongParts.TrackSelector.MinPtCut = 1300 # hardcoded PT cut (as in HLT1)!
LongParts.OutputLevel = 4
# ################################################################
# HARDCODED CUTS to speed up everything! Should be looser than cuts which can be configured
FilterLongParts = FilterDesktop("FilterLongParts")
FilterLongParts.Code = "(PIDmu > 0.0) & (TRCHI2DOF < 5.0)"
FilterLongParts.InputLocations = [ "LongParts" ]
# ################################################################
# Hlt1 PreFilter for Jpsi and Upsilons 
tisTosPreFilterHlt1Jpsi = TisTosParticleTagger("tisTosPreFilterHlt1Jpsi")
tisTosPreFilterHlt1Jpsi.InputLocations = [ "Phys/FilterLongParts" ]
tisTosPreFilterHlt1Jpsi.TisTosSpecs = { "Hlt1TrackMuonDecision%TOS" : 0}
tisTosPreFilterHlt1Jpsi.ProjectTracksToCalo = False
tisTosPreFilterHlt1Jpsi.CaloClustForCharged = False
tisTosPreFilterHlt1Jpsi.CaloClustForNeutral = False
tisTosPreFilterHlt1Jpsi.TOSFrac = { 4:0.0, 5:0.0 }
tisTosPreFilterHlt1Jpsi.NoRegex = True
# ################################################################
# Hlt1 PreFilter for Zs (PT cut)
tisTosPreFilterHlt1Z = TisTosParticleTagger("tisTosPreFilterHlt1Z")
tisTosPreFilterHlt1Z.InputLocations = [ "Phys/FilterLongParts" ]
tisTosPreFilterHlt1Z.TisTosSpecs = { "Hlt1SingleMuonNoIPL0HighPTDecision%TOS" : 0}
tisTosPreFilterHlt1Z.ProjectTracksToCalo = False
tisTosPreFilterHlt1Z.CaloClustForCharged = False
tisTosPreFilterHlt1Z.CaloClustForNeutral = False
tisTosPreFilterHlt1Z.TOSFrac = { 4:0.0, 5:0.0 }
tisTosPreFilterHlt1Z.NoRegex = True
# ################################################################
# Hlt2 PreFilter for Jpsis and Upsilons (IP cut)
tisTosPreFilterHlt2Jpsi = TisTosParticleTagger("tisTosPreFilterHlt2Jpsi")
tisTosPreFilterHlt2Jpsi.InputLocations = [ "Phys/tisTosPreFilterHlt1Jpsi" ]
tisTosPreFilterHlt2Jpsi.TisTosSpecs = { "Hlt2SingleMuonDecision%TOS" : 0}
tisTosPreFilterHlt2Jpsi.ProjectTracksToCalo = False
tisTosPreFilterHlt2Jpsi.CaloClustForCharged = False
tisTosPreFilterHlt2Jpsi.CaloClustForNeutral = False
tisTosPreFilterHlt2Jpsi.TOSFrac = { 4:0.0, 5:0.0 }
tisTosPreFilterHlt2Jpsi.NoRegex = True
# ################################################################
# Hlt2 PreFilter for Zs (PT cut)
tisTosPreFilterHlt2Z = TisTosParticleTagger("tisTosPreFilterHlt2Z")
tisTosPreFilterHlt2Z.InputLocations = [ "Phys/tisTosPreFilterHlt1Z" ]
tisTosPreFilterHlt2Z.TisTosSpecs = { "Hlt2SingleHighPTMuonDecision%TOS" : 0}
tisTosPreFilterHlt2Z.ProjectTracksToCalo = False
tisTosPreFilterHlt2Z.CaloClustForCharged = False
tisTosPreFilterHlt2Z.CaloClustForNeutral = False
tisTosPreFilterHlt2Z.TOSFrac = { 4:0.0, 5:0.0 }
tisTosPreFilterHlt2Z.NoRegex = True
# ##########################################################################################
# The end...
# ##########################################################################################

