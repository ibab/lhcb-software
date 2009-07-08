"""
 script to configure HLT2 trigger lines

 @author P. Koppenburg
 @date 2009-07-08
"""
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.2 $"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer as Sequence
from Hlt2Lines.Hlt2B2DXLines            import Hlt2B2DXLinesConf
from Hlt2Lines.Hlt2B2JpsiXLines         import Hlt2B2JpsiXLinesConf
from Hlt2Lines.Hlt2B2JpsiXLines         import Hlt2Bs2JpsiPhiPrescaledAndDetachedLinesConf 
from Hlt2Lines.Hlt2InclusiveDiMuonLines import Hlt2InclusiveDiMuonLinesConf
from Hlt2Lines.Hlt2InclusiveMuonLines   import Hlt2InclusiveMuonLinesConf
from Hlt2Lines.Hlt2InclusivePhiLines    import Hlt2InclusivePhiLinesConf
from Hlt2Lines.Hlt2TopologicalLines     import Hlt2TopologicalLinesConf
from Hlt2Lines.Hlt2XGammaLines          import Hlt2XGammaLinesConf
from Hlt2Lines.Hlt2B2HHLines            import Hlt2B2HHLinesConf
from Configurables import RichRecSysConf

class Hlt2Conf(LHCbConfigurableUser):
    __used_configurables__ = [ Hlt2B2DXLinesConf
                             , Hlt2B2JpsiXLinesConf
          		     , Hlt2Bs2JpsiPhiPrescaledAndDetachedLinesConf
	                     , Hlt2InclusiveDiMuonLinesConf
                             , Hlt2InclusiveMuonLinesConf
                             , Hlt2InclusivePhiLinesConf
                             , Hlt2TopologicalLinesConf
                             , RichRecSysConf
                             , Hlt2XGammaLinesConf
                             , Hlt2B2HHLinesConf ]
    __slots__ = {
         "DataType"                   : '2009'    # datatype is one of 2009, MC09, DC06...
       , "Hlt2Requires"               : 'L0+Hlt1'  # require L0 and Hlt1 pass before running Hlt2
       , "ActiveHlt2Lines"            : [] # list of lines to be added
       , "HistogrammingLevel"         : 'None' # or 'Line'
       , "ThresholdSettings"          : '' #  select a predefined set of settings, eg. 'Miriam_20090430' or 'FEST'
       , "Hlt2Tracks"                 : "Hlt/Track/Long"
               }

#
# Lines
#
    def hlt2Lines(self,Hlt2):
        """
        The actual lines
        """
        Hlt2.Members += [ Sequence('Hlt2Lines',ModeOR=True,ShortCircuit=False) ] 
        Hlt2B2DXLinesConf()
        Hlt2Bs2JpsiPhiPrescaledAndDetachedLinesConf()
        Hlt2B2JpsiXLinesConf()
        Hlt2InclusiveDiMuonLinesConf()
        Hlt2InclusiveMuonLinesConf()
        Hlt2InclusivePhiLinesConf()
        Hlt2TopologicalLinesConf()
        Hlt2XGammaLinesConf()
        Hlt2B2HHLinesConf()
        #
        # @todo need to be translated
        #
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2Lines.py" )
#
# Reco
#
    def hlt2Reconstruction(self,Hlt2):
        """
        Reconstruction
        """
        #  Full reconstruction of all tracks 
        from HltConf.HltReco import HltRecoSequence
        Hlt2.Members += [ HltRecoSequence ]
#
# Particle making
#
    def hlt2Particles(self,Hlt2):
        """
        Hlt2 sequencing (was options/Hlt2Particles.py)
        """
        SeqHlt2Particles = Sequence('SeqHlt2Particles'
                                          , MeasureTime = True 
                                          , IgnoreFilterPassed = True) # do all 
        Hlt2.Members += [ SeqHlt2Particles ]
        SeqHlt2Particles.Members += [ self.hlt2Charged() ] # charged
        SeqHlt2Particles.Members += [ self.hlt2Calo() ] # calo
        SeqHlt2Particles.Members += [ self.hlt2Muon() ] # muon
        self.hlt2Protos(SeqHlt2Particles)          # protos
        
#
# ProtoParticles
#
    def hlt2Protos(self,SeqHlt2Particles):
        """
        protoparticles 
        """
        from Configurables import ChargedProtoPAlg, ChargedProtoCombineDLLsAlg, NeutralProtoPAlg, TrackSelector
        Hlt2ChargedProtoPAlg = ChargedProtoPAlg('Hlt2ChargedProtoPAlg')
        Hlt2ChargedProtoPAlg.InputTrackLocation = self.getProp("Hlt2Tracks")
        Hlt2ChargedProtoPAlg.OutputProtoParticleLocation = "Hlt/ProtoP/Charged"
        Hlt2ChargedProtoPAlg.addTool( TrackSelector, name="TrackSelector")
        Hlt2ChargedProtoPAlg.TrackSelector.AcceptClones = False
        Hlt2ChargedProtoPAlg.InputMuonPIDLocation = "Hlt/Muon/MuonPID"
        ## Calo PID
        Hlt2ChargedProtoPAlg.UseCaloSpdPID = True 
        Hlt2ChargedProtoPAlg.UseCaloPrsPID = True 
        Hlt2ChargedProtoPAlg.UseCaloEcalPID = True 
        Hlt2ChargedProtoPAlg.UseCaloHcalPID = True 
        Hlt2ChargedProtoPAlg.UseCaloBremPID = True 
        ##Hlt2ChargedProtoPAlg.UseRichPID = False  // Protos will NOT have any RICH information - HltRichPIDsKaons will not work
        Hlt2ChargedProtoPAlg.UseRichPID = True     ## Use this to add RICH info to the HLT protos, needed for HltRichPIDsKaons
        Hlt2ChargedProtoPAlg.UseMuonPID = True 
        Hlt2ChargedProtoPAlg.UseVeloPID = False 
        SeqHlt2Particles.Members += [ Hlt2ChargedProtoPAlg ]

        Hlt2ChargedProtoCombDLL = ChargedProtoCombineDLLsAlg('Hlt2ChargedProtoCombDLL')
        Hlt2ChargedProtoCombDLL.ProtoParticleLocation = "Hlt/ProtoP/Charged" 
        SeqHlt2Particles.Members += [ Hlt2ChargedProtoCombDLL ]
        
        SeqHlt2Particles.Members += [ NeutralProtoPAlg('HltNeutralProtoPAlg') ]

        
#
# MuonID
#
    def hlt2Muon(self):
        """
        Muon ID options
        """
        from MuonID import ConfiguredMuonIDs
        from Configurables import MuonRec, MuonIDAlg
        cm = ConfiguredMuonIDs.ConfiguredMuonIDs(data=self.getProp("DataType"))
#        MuonIDSeq = cm.getMuonIDSeq()
        HltMuonID = MuonIDAlg().clone('HltMuonID')
        HltMuonID.TrackLocation = self.getProp("Hlt2Tracks")
        HltMuonID.MuonIDLocation = "Hlt/Muon/MuonPID" 
        HltMuonID.MuonTrackLocation = "Hlt/Track/Muon"
        
        HltMuonIDSeq = Sequence('HltMuonIDSeq')
        HltMuonIDSeq.Members += [ "MuonRec", HltMuonID ]
        return HltMuonIDSeq
#
# Calo
#
    def hlt2Calo(self):
        """
        Calorimeter options
        """
        from Configurables import InSpdAcceptanceAlg, InPrsAcceptanceAlg, InEcalAcceptanceAlg, InHcalAcceptanceAlg, InBremAcceptanceAlg
        from Configurables import ElectronMatchAlg, BremMatchAlg, PhotonMatchAlg
        from Configurables import Track2SpdEAlg, Track2PrsEAlg, Track2EcalEAlg, Track2HcalEAlg
        from Configurables import EcalChi22ID, BremChi22ID, ClusChi22ID

        caloSeq = Sequence('HltRecoCALOSeq', Context = 'HLT', IgnoreFilterPassed = True ) 
        ## Options for Calo reconstruction
        ## @todo Translate from .opts
        importOptions("$CALORECOROOT/options/HltCaloSeq.opts")
        from HltConf.HltDecodeRaw import DecodeECAL, DecodeSPD, DecodePRS, DecodeHCAL
        _m = caloSeq.Members
        Sequence('HltRecoCALOSeq').Members = DecodeECAL.members() + DecodeSPD.members() + DecodePRS.members() + DecodeHCAL.members() + _m 

        HltTracks = [ self.getProp("Hlt2Tracks") ] # takes a list
        #
        # @todo That's ugly
        #
        InSpdAcceptanceAlg('HltInSPD').Inputs   =     HltTracks
        InPrsAcceptanceAlg('HltInPRS').Inputs   =     HltTracks
        InEcalAcceptanceAlg('HltInECAL').Inputs =     HltTracks
        InHcalAcceptanceAlg('HltInHCAL').Inputs =     HltTracks
        InBremAcceptanceAlg('HltInBREM').Inputs =     HltTracks
        ElectronMatchAlg('HltElectronMatch').Tracks = HltTracks
        BremMatchAlg('HltBremMatch').Tracks =         HltTracks
        PhotonMatchAlg('HltClusterMatch').Tracks =    HltTracks
        Track2SpdEAlg('HltSpdE').Inputs =             HltTracks
        Track2PrsEAlg('HltPrsE').Inputs =             HltTracks
        Track2EcalEAlg('HltEcalE').Inputs =           HltTracks
        Track2HcalEAlg('HltHcalE').Inputs =           HltTracks
        EcalChi22ID('HltEcalChi22ID').Tracks =        HltTracks
        BremChi22ID('HltBremChi22ID').Tracks =        HltTracks
        ClusChi22ID('HltClusChi22ID').Tracks =        HltTracks
        # return
        return caloSeq 
#
# Charged Particle making
#
    def hlt2Charged(self):
        """
        Charged particles
        """
        from Configurables import NumberOfTracksFilter
        from Configurables import HltInsertTrackErrParam
        SeqHlt2Charged = Sequence('SeqHlt2Charged'
                                        , MeasureTime = True
                                        , IgnoreFilterPassed = False)        
        #
        # @todo TEMPORARY kill huge events
        #
        NumberOfTracksFilter =  NumberOfTracksFilter()
        NumberOfTracksFilter.TrackLocations = [ self.getProp("Hlt2Tracks") ]
        NumberOfTracksFilter.MaxTracks = 1000 
        SeqHlt2Charged.Members += [ NumberOfTracksFilter ]
        #
        # MC truth associated tracks
        #
        SeqHlt2Charged.Members += [ Sequence('SeqTrueSignalTracks') ] #  debug
        #
        # Hacking of errors
        #
        HltInsertTrackErrParam = HltInsertTrackErrParam()
        HltInsertTrackErrParam.InputLocation = "Hlt/Track/Long" 
        SeqHlt2Charged.Members += [ HltInsertTrackErrParam ]
        return SeqHlt2Charged
       
#
# Requirements
#
    def hlt2Requirements(self,Hlt2):
        """
        Requirements
        """
        Hlt2.Members += [ Sequence("Hlt2Requirements") ]
        Sequence("Hlt2Requirements").Members = [ ]
        reqs = self.getProp('Hlt2Requires')
        if reqs.upper != "NONE" :
            from Configurables import LoKi__HDRFilter   as HltFilter
            from Configurables import LoKi__L0Filter    as L0Filter
            hlt2requires = { 'L0'   : L0Filter( 'L0Pass',          Code = "L0_DECISION" )
                             , 'Hlt1' : HltFilter('Hlt1GlobalPass' , Code = "HLT_PASS('Hlt1Global')" )
                             }
            for i in reqs.split('+') :
                if i : Sequence("Hlt2Requirements").Members.append( hlt2requires[i] )
#
# Main configuration
#
    def __apply_configuration__(self):
        """
        Hlt2 configuration
        """
        Hlt2 = Sequence("Hlt2", Context = 'HLT' )
        if Hlt2 not in Sequence("Hlt").Members : Sequence("Hlt").Members += [ Hlt2 ]
        # requirements
        self.hlt2Requirements(Hlt2)        
        # reco
        self.hlt2Reconstruction(Hlt2)
        # reco
        self.hlt2Particles(Hlt2)
        # lines
        self.hlt2Lines(Hlt2)
        
