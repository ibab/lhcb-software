"""
 script to configure HLT2 trigger lines

 @author P. Koppenburg
 @date 2009-07-08
"""
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.7 $"
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
       , "HltType"                    : 'Hlt1+Hlt2' # only care about Hlt2
       , "Hlt2Requires"               : 'L0+Hlt1'  # require L0 and Hlt1 pass before running Hlt2
       , "ActiveHlt2Lines"            : [] # list of lines to be added
       , "HistogrammingLevel"         : 'None' # or 'Line'
       , "ThresholdSettings"          : {} # ThresholdSettings predefined by Configuration
       , "Hlt2Tracks"                 : "Hlt/Track/Long"
         }

###################################################################################
#
# Get Hlt2 lines Configurables
#
    def getHlt2Configurables(self):
        """
        Return a dictionary of configurables
        """
        #
        # 1) convert hlt type to trigger categories
        #
        hlttype           = self.getProp("HltType")
        # Dictionary of Hlt2 type
        trans = { 'Hlt2' : 'TOPO+LEPT+PHI+EXCL'  # @todo need express as well
                }
        for short,full in trans.iteritems() : hlttype = hlttype.replace(short,full)
        #
        # 2) define what categories stand for
        # There are the strings used in HltThresholdSettings
        #
        type2conf = { 'TOPO' : [ Hlt2TopologicalLinesConf
                               , Hlt2B2DXLinesConf ]
                    , 'LEPT' : [ Hlt2InclusiveDiMuonLinesConf
                               , Hlt2InclusiveMuonLinesConf ]
                    , 'PHI'  : [ Hlt2InclusivePhiLinesConf ]
                    , 'EXCL' : [ Hlt2Bs2JpsiPhiPrescaledAndDetachedLinesConf
                               , Hlt2B2JpsiXLinesConf
                               , Hlt2XGammaLinesConf
                               , Hlt2B2HHLinesConf ]
                      }

        return type2conf
        
###################################################################################
#
# Threshold settings
#
# This is copied and edited from Hlt1.confType().
# One could outsource that to some function, but who cares?
#
    def hlt2Lines(self,Hlt2):
        """
        The actual lines
        """
        Hlt2.Members += [ Sequence('Hlt2Lines',ModeOR=True,ShortCircuit=False) ]
        ThresholdSettings = self.getProp("ThresholdSettings")
        print "# Hlt2 thresholds:", ThresholdSettings
        #
        type2conf = self.getHlt2Configurables()
        #
        # Loop over thresholds
        #
        for i in type2conf :
            for confs in type2conf[i] :
                if confs not in self.__used_configurables__ : raise AttributeError, "configurable for '%s' not in list of used configurables"%i
                log.info( '# requested ' + i + ', importing ' + str(type2conf[i])  )
                # FIXME: warning: the next is 'brittle': if someone outside 
                #        does eg. HltMuonLinesConf(), it will get activated
                #        regardless of whether we do it over here...
                #        So anyone configuring some part explictly will _always_ get
                #        that part of the Hlt run, even if it does not appear in HltType...
                conf = confs()
                if ThresholdSettings and conf in ThresholdSettings : 
                    for (k,v) in ThresholdSettings[conf].iteritems() :
                        # configurables have an exception for list and dict: 
                        #   even if not explicitly set, if you ask for them, you get one...
                        #   this is done to make foo().somelist += ... work.
                        # hence we _assume_ that, even if we have an attr, but it matches the
                        # default, it wasn't set explicitly, and we overrule it...
                        if hasattr(conf,k) and conf.getProp(k) != conf.getDefaultProperty(k) :
                            log.warning('# WARNING: %s.%s has explictly been set, NOT using requested predefined threshold %s, but keeping explicit value: %s '%(conf.name(),k,str(v),getattr(conf,k)))
                        else :
                            if ( type(v) == type({})): # special case for dictionaries (needed in topo)
                                val = conf.getProp(k)
                                val.update(v)                                
                                setattr(conf,k,val)
                            else :
                                setattr(conf,k,v)

        #
        # more
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2Lines.py" ) # I AM OBSOLETE : KILL ME!!!
###################################################################################
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

###################################################################################
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
        
###################################################################################
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

        
###################################################################################
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
        HltMuonIDAlg = cm.configureMuonIDAlg("HltMuonIDAlg")
        HltMuonIDAlg.TrackLocation = "Hlt/Track/Long"
        HltMuonIDAlg.MuonIDLocation = "Hlt/Muon/MuonPID"
        HltMuonIDAlg.MuonTrackLocation = "Hlt/Track/Muon"
        HltMuonIDSeq = GaudiSequencer("HltMuonIDSeq")
        HltMuonIDSeq.Members += ["MuonRec", HltMuonIDAlg]

        return HltMuonIDSeq

###################################################################################
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

###################################################################################
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
       
###################################################################################
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
        if reqs.upper() != "NONE" :
            from Configurables import LoKi__HDRFilter   as HltFilter
            from Configurables import LoKi__L0Filter    as L0Filter
            hlt2requires = { 'L0'   : L0Filter( 'L0Pass',          Code = "L0_DECISION" )
                             , 'Hlt1' : HltFilter('Hlt1GlobalPass' , Code = "HLT_PASS('Hlt1Global')" )
                             }
            for i in reqs.split('+') :
                if i : Sequence("Hlt2Requirements").Members.append( hlt2requires[i] )

###################################################################################
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
        importOptions( "$HLTCONFROOT/options/HltTrackAssociator.py" )
        # reco
        self.hlt2Particles(Hlt2)
        # lines
        self.hlt2Lines(Hlt2)
        
