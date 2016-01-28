"""
High level configuration tools for HltConf, to be invoked by Moore and DaVinci
"""
__version__ = "$Id: Configuration.py,v 1.208 2010-09-02 14:05:36 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from Gaudi.Configuration import *
from LHCbKernel.Configuration import LHCbConfigurableUser
from Configurables       import GaudiSequencer as Sequence
from Hlt1                import Hlt1Conf
from Hlt2                import Hlt2Conf
from HltAfterburner      import HltAfterburnerConf
from HltPersistReco      import HltPersistRecoConf
from HltMonitoring       import HltMonitoringConf

def __forAll__( c, prop_value_dict, types=['FilterDesktop','CombineParticles',"DVAlgorithm", "DaVinciAlgorithm", "DaVinciHistoAlgorithm", "DaVinciTupleAlgorithm", "*" ] ) :
    """ Find all configurable algorithms and set certain properties
    """
    if type(prop_value_dict) is not dict:
        raise TypeError("Hey, you need to give me a dictionary, you passed me a, "+str(type(prop_value_dict)))

    if "*" in types or ".*" in types or c.getType() in types:
        for prop in prop_value_dict:
            if hasattr(c,prop) or (hasattr(c,"properties") and prop in c.properties()):
                c.setProp(prop,prop_value_dict[prop])

    #iterate/recurse over all subparts
    for p in [ 'Members','Filter0','Filter1' ] :
        if not hasattr(c,p) : continue
        seq = getattr(c,p)
        if type(seq) is not list: seq = [ seq ]
        for i in seq : __forAll__(i,prop_value_dict,types)

def __updateDict__( d, id, entries ) :
    # pick up first unused entry after 'id'
    for i in entries :
        while id in d.values() : id = id + 1
        d.update({ i : id } )

### TODO: move this into HltPVs...
def onlinePV():
    """Use the tracking cofigurables to define the onlinePV location
    """
    import HltTracking
    from HltTracking.HltVertexNames import Hlt3DPrimaryVerticesName as PV3DSelection
    from HltTracking.HltVertexNames import ( _vertexLocation,
                                             HltSharedVerticesPrefix,
                                             HltGlobalVertexLocation )
    pv3d  = _vertexLocation( HltSharedVerticesPrefix, HltGlobalVertexLocation, PV3DSelection )
    return {"PV3D" : pv3d}

class HltConf(LHCbConfigurableUser):
    """
    Hlt configuration
    """
    ## Streams we know about: {name, (rb, enabled-by-default)}
    __streams__ = {"LUMI"        : (33, True),
                   "BEAMGAS"     : (35, True),
                   "VELOCLOSING" : (40, True),
                   "FULL"        : (87, True),
                   "TURBO"       : (88, True),
                   "PARKED"      : (89, False),
                   "TURCAL"      : (90, True),
                   "NOBIAS"      : (91, False),
                   "SMOGPHYS"    : (93, False)}

    __used_configurables__ = [ Hlt1Conf
                             , Hlt2Conf
                             , HltMonitoringConf
                             , HltAfterburnerConf
                             , HltPersistRecoConf ]
    __slots__ = { "L0TCK"                          : None
                , 'ForceSingleL0Configuration'     : True
                , 'SkipDisabledL0Channels'         : False
                , "DataType"                       : '2012'
                , "Verbose"                        : False      # print the generated Hlt sequence
                , "ThresholdSettings"              : ''         #  select a predefined set of settings, eg. 'Effective_Nominal'
                , "Split"                          : ''         # Hlt1 or Hlt2 (will also be set by Moore)
                , "EnableMonitoring"               : True       # Insert the monitoring sequence
                , "EnableHltDecReports"            : True
                , "EnableHltSelReports"            : True
                , "EnableHltVtxReports"            : True
                , "EnableHltTrkReports"            : False
                , "EnableHltRoutingBits"           : True
                , "EnableHltAfterburner"           : True
                , "EnableLumiEventWriting"         : True
                , "EnableAcceptIfSlow"             : False      # Switch on AcceptIfSlow switch of HltLine
                , "SlowHlt1Threshold"              : 500000     # microseconds
                , "SlowHlt2Threshold"              : 5000000     # microseconds
                , 'RequireL0ForEndSequence'        : False
                , 'RequireRoutingBits'             : [] # to require not lumi exclusive, set to [ 0x0, 0x4, 0x0 ]
                , 'VetoRoutingBits'                : []
                , 'SkipHltRawBankOnRejectedEvents' : True
                , 'LumiBankKillerPredicate'        : "(HLT_PASS_SUBSTR('Hlt1Lumi') & ~HLT_PASS_RE('Hlt1(?!Lumi).*Decision'))"
                , "LumiBankKillerAcceptFraction"   : 0.9999     # fraction of lumi-only events where raw event is stripped down
                                                                # (only matters if EnablelumiEventWriting = True)
                  , "AdditionalHlt1Lines"            : []         # must be configured
                , "AdditionalHlt2Lines"            : []         # must be configured
               , "RemoveHlt1Lines"            : []         # must be configured
                , "RemoveHlt2Lines"            : []         # must be configured
                , "Hlt2LinesForDQ"                 : ["PIDD02KPiTagTurboCalib", "PIDLambda2PPiLLTurboCalib", "PIDDetJPsiMuMuPosTaggedTurboCalib",
                                                      "PIDDetJPsiMuMuNegTaggedTurboCalib", "PIDLambda2PPiLLhighPTTurboCalib", "PIDLambda2PPiLLveryhighPTTurboCalib",
                                                      "DiMuonDetachedJPsi"]
                , "NanoBanks"                      : ['ODIN','HltLumiSummary','HltRoutingBits','DAQ']
                , "PruneHltANNSvc"                 : True
                , "PersistReco"                    : False
                , "CustomPersistRecoFilter"        : ""
                , "PruneHltANNSvc"                 : True
                , "EnabledStreams"                 : {"LUMI" : None, "BEAMGAS" : None, "FULL" : None, "TURBO" : None, "TURCAL" : None, "VELOCLOSING" : None}
                }

    __settings__ = None
    #import logging
    #_log = logging.getLogger( 'HltConf.Configuration')
    #handler = logging.StreamHandler()
    #formatter = logging.Formatter("# %(name)s - %(levelname)s : %(message)s")
    #handler.setFormatter( formatter )
    #_log.addHandler( handler )
    #_log.setLevel(logging.DEBUG)

    def knownStreams(self):
        return self.__streams__
    
    def defineL0Channels(self, L0TCK = None) :
        """
        Define L0 channels
        """
        if L0TCK :
            log.warning( '###############################################################')
            log.warning( '## WARNING HLT will assume input data contains L0 TCK %s ##' % L0TCK )
            log.warning( '###############################################################')
            from Hlt1Lines.HltL0Candidates import decodeL0Channels
            decodeL0Channels( L0TCK
                            , skipDisabled               = self.getProp('SkipDisabledL0Channels')
                            , forceSingleL0Configuration = self.getProp('ForceSingleL0Configuration')
                            )
        else :
            log.warning( '##################################################################################################')
            log.warning( '## WARNING You did not inform the HLT configuration what L0 Configuration is used for the input ##')
            log.warning( '## WARNING This may not work, and if it does, it will result in undefined behavior              ##' )
            log.warning( '## WARNING Please make sure you know what you are doing!!                                       ##' )
            log.warning( '##################################################################################################')
        from Hlt1Lines.HltL0Candidates import setupL0Channels
        setupL0Channels()

    def settings(self) :
        """
        Get the class that contains the thresholds, etc
        """
        name  = self.getProp('ThresholdSettings')   # the name
        if not name : return None
        from HltConf.ThresholdUtils import Name2Threshold
        return Name2Threshold(name)

    def configureFactories(self):
        ## Add correct modules to factories.
        from itertools import product
        import HltTracking.Hlt1Tracking
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        from Configurables import LoKi__Hybrid__Tool as HybridFactory
        from Configurables import LoKi__Hybrid__HltFactory as HltFactory
        modules = ["LoKiCore.decorators",
                   "LoKiTrigger.decorators",
                   "LoKiNumbers.decorators",
                   'LoKiNumbers.sources',
                   "LoKiCore.functions",
                   "LoKiCore.math",
                   "LoKiHlt.algorithms"]
        extra_modules = ["LoKiArrayFunctors.decorators",
                         "LoKiPhys.decorators",
                         "LoKiTracks.decorators"]

        for factories, mods in {(HltFactory(),
                                 HltFactory("HltFactory"),
                                 CoreFactory("Hlt1CoreFactory"),
                                 CoreFactory("Hlt2CoreFactory"),
                                 HltFactory("Hlt1HltFactory"),
                                 HltFactory("Hlt2HltFactory")) :
                                modules,
                                (CoreFactory("Hlt1Factory"),
                                 HybridFactory("Hlt1HybridFactory"),
                                 HybridFactory("Hlt2HybridFactory")) :
                                modules + extra_modules}.iteritems():
             for factory, module in product(factories, mods):
                 if not module in factory.Modules:
                     factory.Modules.append(module)
                     factory.Lines += ["from GaudiKernel.SystemOfUnits import GeV, MeV, mm",
                                       "import HltTracking.Hlt1StreamerConf"]

    def confType(self) :
        """
        Decoding of configuration. This is where Hlt1 and 2 configurations are called.
        """
        #
        # set thresholds if you can
        #
        ThresholdSettings = {}
        thresClass = self.settings()
        if ( thresClass != None ):
            ThresholdSettings = thresClass.Thresholds()
        else :
            log.warning( '##################################################################' )
            log.warning( '## WARNING You are running the HLT with no defined thresholds   ##' )
            log.warning( '## WARNING You will get the default cuts and all lines          ##' )
            log.warning( '## WARNING Set a ThresholdSetting to get something well defined ##' )
            log.warning( '## ###############################################################' )


        ## what L0 configuration are we running on top of?
        L0TCK = None
        if thresClass : L0TCK = thresClass.L0TCK()
        if self.isPropertySet('L0TCK') and self.getProp('L0TCK') :
            if L0TCK != self.getProp('L0TCK') :
                log.warning( '####################################################################################' )
                log.warning( '## WARNING You are configuring the HLT to run on top of an L0 configuration       ##' )
                log.warning( '## WARNING which is different then the one it was generated for                   ##' )
                log.warning( '## WARNING This may not work, but if it does, it may result in undefined behavior ##' )
                log.warning( '## WARNING Please make sure you know what you are doing!!                         ##' )
                log.warning( '####################################################################################' )
            L0TCK = self.getProp('L0TCK')
        if not self.isPropertySet('L0TCK') or not self.getProp('L0TCK') and L0TCK != None:
            self.setProp('L0TCK', L0TCK)

        self.defineL0Channels( L0TCK )

        # obtain list of lines,
        activehlt1lines,activehlt2lines=self._runHltLines()
        # and push into the Hlt1Lines and Hlt2Lines code...
        #  (for backwards compatible behaviour, just skip the next three lines... )
        from  HltLine.HltLine import setRequestedHlt1Lines, setRequestedHlt2Lines
        setRequestedHlt1Lines( activehlt1lines )
        setRequestedHlt2Lines( activehlt2lines )

        Dec = Sequence('HltDecisionSequence', Members = [])

        # Decoding at end of Hlt1 and beginning of HLT2. Make sure the decoders do not run, if location already exists.
        from DAQSys.Decoders import DecoderDB
        for n in [ "HltDecReportsDecoder/Hlt1DecReportsDecoder",
                   "HltSelReportsDecoder/Hlt1SelReportsDecoder",
                   "HltTrackReportsDecoder"] :
            decoder = DecoderDB[n]
            decoder.setup().VetoObjects = [ loc for loc in decoder.listOutputs() ]

        #
        # dispatch Hlt1 configuration, don't do this if there are no HLT1 lines
        #
        if activehlt1lines:
            Dec.Members.append(Sequence("Hlt1"))
            Dec.Members.append(Sequence("Hlt1Postamble"))
            Hlt1Conf()
            Hlt1Conf().ThresholdSettings = ThresholdSettings

        #
        # dispatch Hlt2 configuration
        #
        # don't do this if there are no HLT2 lines
        if activehlt2lines:
            Dec.Members.append(Sequence("Hlt2"))
            if self.getProp("EnableHltAfterburner"):
                seq = Sequence("HltAfterburner", IgnoreFilterPassed = True)
                Dec.Members.append(seq)
                HltAfterburnerConf().Sequence = seq
                # Have we been asked to persist the Hlt reco
                if self.getProp("PersistReco"):
                    PRseq = Sequence("HltPersistReco", IgnoreFilterPassed = True)
                    HltPersistRecoConf().Sequence = PRseq
                    if self.getProp("CustomPersistRecoFilter"):
                        HltPersistRecoConf().PreFilter=self.getProp("CustomPersistRecoFilter")
            Dec.Members.append(Sequence("Hlt2Postamble"))
            Hlt2Conf()
            self.setOtherProps(Hlt2Conf(),[ "DataType" ])
            Hlt2Conf().ThresholdSettings = ThresholdSettings
            if thresClass and hasattr( thresClass, 'Hlt2DefaultVoidFilter' ) :
                Hlt2Conf().DefaultVoidFilter = getattr( thresClass, 'Hlt2DefaultVoidFilter' )
            # Never try to decode if we want to rerun the HLT1 tracking in HLT2.
            if Hlt2Conf().getProp('Hlt1TrackOption') == "Rerun":
                decoder = DecoderDB["HltTrackReportsDecoder"]
                decoder.setup().Enable = False

        Hlt = Sequence('Hlt', ModeOR = True, ShortCircuit = False
                       , Members = [ Sequence('HltDecisionSequence')
                                   , Sequence('HltEndSequence')
                                   ]
                       )

        if self.getProp('EnableMonitoring'):
            Hlt.Members.insert(1, Sequence('HltMonitorSequence'))

        if self.getProp('RequireRoutingBits') or self.getProp('VetoRoutingBits') :
            from Configurables import HltRoutingBitsFilter
            filter = HltRoutingBitsFilter( "PhysFilter" )
            if self.getProp('RequireRoutingBits') : filter.RequireMask = self.getProp('RequireRoutingBits')
            if self.getProp('VetoRoutingBits')    : filter.VetoMask    = self.getProp('VetoRoutingBits')
            Sequence("HltDecisionSequence").Members.insert(0, filter)
            Sequence("HltEndSequence").Members.insert(0, filter)
            Sequence('HltMonitorSequence').Members.insert(0, filter)

        #fix input locations, for the moment do with a post-config action,
        #TODO: in the future set in Hlt1 and Hlt2 separately

        #get the vertex locations required
        loc= onlinePV()["PV3D"]
        def __setOnlinePV__(hlt=Hlt, loc=loc):
            __forAll__(hlt, {"InputPrimaryVertices":loc})
        appendPostConfigAction(__setOnlinePV__)


    def configureRoutingBits(self) :
        """
        Routing bits
        """
        ## set triggerbits
        #  0-7 : reserved for ODIN  // need to add ODIN support to routing bit writer
        #  8-31: reserved for L0    // need to add L0DU support to routing bit writer
        # 32-63: reserved for Hlt1
        # 64-91: reserved for Hlt2

        ## NOTE: any change in the _meaning_ of any of the following needs to be
        ##       communicated (at least!) with online, to insure the events are
        ##       still properly routed!!! (this goes esp. for [32,36]!!!)
        ## NOTE: Usage of bits set in HLT1:
        ##       bit 46 -> 'physics triggers'
        ##       bit 37 -> subscribed to be Hlt monitoring
        ##       bit 36 -> express stream
        ##       bit 35 -> subscribed to by Velo closing monitoring
        ##       bit 34 -> count number of 'non-lumi-exlusive events'
        ##       bit 33 -> lumi stream
        ##       bit 32 -> full stream (actually, not used for that, but it could be ;-)
        ## NOTE: Routing bits for streams set in HLT2
        ##       bit 87 for the full (non-turbo(calib)) stream
        ##       bit 88 for Turbo stream, includes lumi events.
        ##       bit 89 for parked stream; reserved but not set for now
        ##       bit 90 for TurboCalib stream, includes lumi events.
        ##       bit 91 for NOBIAS stream; reserved
        ##       bit 92 for online DQ on HLT2 output
        ##       bit 93 for SMOG physics; reserved

        from Hlt1Lines.HltL0Candidates import L0Channels

        from Configurables import HltRoutingBitsWriter
        routingBits = {  0 : '( ODIN_BXTYP == LHCb.ODIN.Beam1 ) | ( ODIN_BXTYP == LHCb.ODIN.BeamCrossing )'
                      ,  1 : '( ODIN_BXTYP == LHCb.ODIN.Beam2 ) | ( ODIN_BXTYP == LHCb.ODIN.BeamCrossing )'
                      ,  3 : 'ODIN_TRUE'
                      ,  4 : 'ODIN_TRGTYP == LHCb.ODIN.LumiTrigger'
                      ,  5 : 'jbit( ODIN_EVTTYP, 2)' # ODIN NOBIAS
                      ,  6 : 'jbit( ODIN_EVTTYP, 14)' # ODIN LEADING BUNCH
                      ,  8 : 'L0_DECISION_PHYSICS'
                      ,  9 : "L0_CHANNEL_RE('B?gas')"
                      , 10 : "|".join( [ "L0_CHANNEL('%s')" % chan for chan in [ 'CALO','MUON,minbias' ] if chan in L0Channels() ] )
                      , 11 : "|".join( [ "L0_CHANNEL('%s')" % chan for chan in [ 'Electron','Photon','Hadron','Muon','DiMuon',
                                                                                 'Muon,lowMult','DiMuon,lowMult','Electron,lowMult',
                                                                                 'Photon,lowMult','DiEM,lowMult','DiHadron,lowMult'] if chan in L0Channels() ] )
                      , 12 : "L0_CHANNEL('CALO')" if 'CALO' in L0Channels() else "" # note: need to take into account prescale in L0...
                      , 13 : "L0_CHANNEL( 'Hadron' )" if 'Hadron' in L0Channels() else ""
                      , 14 : "L0_CHANNEL_RE('Electron|Photon')"
                      , 15 : "L0_CHANNEL_RE('Muon|DiMuon')"
                      , 16 : "L0_CHANNEL_RE('.*NoSPD')"
                      , 17 : "L0_CHANNEL_RE('.*,lowMult')"
                      , 18 : "L0_CHANNEL('DiMuon')" if 'DiMuon' in L0Channels() else ""
                      , 19 : "L0_CHANNEL( 'Hadron|SumEt' )" if ('Hadron' in L0Channels() and 'SumEt' in L0Channels()) else ""
                      , 32 : "HLT_PASS('Hlt1Global')"
                      , 33 : "HLT_PASS_RE('^Hlt1Lumi.*Decision$')"  # lumi stream
                      , 34 : " ~ ( %s ) " % self.getProp('LumiBankKillerPredicate') #  this must be the opposite of the LumiStripper, i.e. if 34 is set, the event should NEVER be a nanoevent...
                      , 35 : "HLT_PASS_SUBSTR('Hlt1BeamGas')" # beamgas stream
                      , 37 : "HLT_PASS_RE('Hlt1(?!BeamGas).*Decision')"  # note: we need the 'Decision' at the end to _exclude_ Hlt1Global # full stream
                      , 38 : "HLT_PASS('Hlt1ODINTechnicalDecision')"
                      , 39 : "HLT_PASS_SUBSTR('Hlt1L0')"
                      , 40 : "HLT_PASS_RE('Hlt1(Velo|BeamGas).*Decision')"  # bit (to be) used by the Velo (closing) monitoring
                      , 41 : "HLT_PASS_RE('Hlt1(Single|Track)Muon.*Decision')"
                      , 42 : "HLT_PASS_RE('Hlt1.*DiMuon.*Decision')"
                      , 43 : "HLT_PASS_RE('Hlt1.*MuonNoL0.*Decision')"
                      , 44 : "HLT_PASS_RE('Hlt1.*Electron.*Decision')"
                      , 45 : "HLT_PASS_RE('Hlt1.*Gamma.*Decision')"
                      , 46 : "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"    # exclude 'non-physics' lines
                      , 47 : "HLT_PASS_RE('Hlt1MBMicroBias.*Decision')"
                      , 48 : "HLT_PASS('Hlt1MBNoBiasDecision')"
                      , 49 : "HLT_PASS_RE('Hlt1.*MVA.*Decision')"
                      , 50 : "HLT_PASS('Hlt1LumiLowBeamCrossingDecision')"
                      , 51 : "HLT_PASS('Hlt1LumiMidBeamCrossingDecision')"
                      , 53 : "HLT_PASS_RE('Hlt1Calib(TrackingKPiDetached|HighPTLowMultTrks)Decision')"
                      , 54 : "HLT_PASS_RE('Hlt1CalibRICH.*Decision')"
                      , 55 : "HLT_PASS('Hlt1MBNoBiasRateLimitedDecision')"
                      , 56 : "HLT_PASS('Hlt1CalibMuonAlignJpsiDecision')"
                      , 60 : "HLT_PASS('Hlt1TrackAllL0Decision')"

                      # 64--96: Hlt2
                      , 64 : "HLT_PASS('Hlt2Global')"
                      , 65 : "HLT_PASS('Hlt2DebugEventDecision')"
                      , 66 : "HLT_PASS_RE('Hlt2(?!Transparent).*Decision')"
                      , 67 : "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision')"
                      , 68 : "HLT_PASS_RE('Hlt2.*DiMuon.*Decision')"
                      , 69 : "HLT_PASS_RE('Hlt2.*DY.*Decision')"
                      , 70 : "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                      , 71 : "HLT_PASS_RE('Hlt2.*CharmHad.*Decision')"
                      , 72 : "HLT_PASS_RE('Hlt2.*IncPhi.*Decision')"
                      , 73 : "HLT_PASS_RE('Hlt2.*Gamma.*Decision')"
                      , 74 : "HLT_PASS_RE('Hlt2.*TriMuon.*Decision')"
                      , 75 : "HLT_PASS_RE('Hlt2.*RareCharm.*Decision')"
                      , 76 : "HLT_PASS_RE('Hlt2.*DisplVertices.*Decision')"
                      , 77 : "HLT_PASS_RE('Hlt2(?!Forward)(?!DebugEvent)(?!Lumi)(?!Transparent)(?!PassThrough).*Decision')"
                      , 78 : "HLT_PASS_RE('Hlt2.*Muon.*Decision')"
                      , 79 : "HLT_PASS_RE('Hlt2.*Incl.*HHX.*Decision')"
                      , 80 : "HLT_PASS_RE('Hlt2.*Electron.*Decision')"
                      , 81 : "HLT_PASS_RE('Hlt2Topo.*2Body.*Decision')"
                      , 82 : "HLT_PASS_RE('Hlt2Topo.*3Body.*Decision')"
                      , 83 : "HLT_PASS_RE('Hlt2Topo.*4Body.*Decision')"
                      , 84 : "HLT_PASS_RE('Hlt2TopoMu[234]Body.*Decision')"
                      , 85 : "HLT_PASS_RE('Hlt2TopoE[234]Body.*Decision')"
                      , 86 : "HLT_PASS_RE('Hlt2Topo[234]Body.*Decision')"
                      # RB 87 for the full (non-turbo(calib)) stream
                      , 87 : "HLT_NONTURBOPASS_RE('Hlt2.*Decision') | HLT_PASS('Hlt2LumiDecision')"
                      # RB 88 for Turbo stream, includes lumi events.
                      # this now excludes turbocalib events which have their own stream/routing bit
                      , 88 : "HLT_TURBOPASS_RE('^Hlt2.*(?!TurboCalib)Decision$') | HLT_PASS('Hlt2LumiDecision')"
                      # RB 89 for the parked stream; reserved but not set for now
                      # RB 90 for TurboCalib stream, includes lumi events.
                      , 90 : "HLT_TURBOPASS_RE('^Hlt2.*TurboCalibDecision$') | HLT_PASS('Hlt2LumiDecision')"
                      # RB 91 for the NOBIAS stream; reserved
                      # RB 92 for online DQ on HLT2 output
                      , 92 : "HLT_PASS_RE('^Hlt2(%s)Decision$')" % '|'.join(self.getProp("Hlt2LinesForDQ"))
                      # RB 93 for SMOG physics; reserved
        }

        ## Stream configuration
        knownStreams = self.knownStreams()
        defaultEnabled = dict(filter(lambda e: e[1][1], knownStreams.iteritems()))

        ## Update routing bits according to enabled streams
        sets = self.settings()
        ep = 'EnabledStreams'
        if self.isPropertySet(ep) and self.getProp(ep) != self.getDefaultProperty(ep):
            log.warning( '##########################################################')
            log.warning( 'Non-default stream configuration specified from EnabledStreams property')
        elif sets and hasattr(sets, 'Streams') and set(sets.Streams()) != set(self.getDefaultProperty(ep)):
            log.warning( '##########################################################')
            log.warning( 'Non-default stream configuration specified in Settings')
            self._safeSet('EnabledStreams', sets.Streams())

        streams = {s : (bit, None, False) for (s, (bit, on)) in knownStreams.iteritems()}
        for stream, expr in self.getProp('EnabledStreams').iteritems():
            if stream not in streams:
                log.fatal("Attempt to enable a non-existent stream %s." % stream)
                raise RuntimeError
            if stream not in defaultEnabled:
                log.warning("Enabling non-default enabled stream %s." % stream)
            bit = streams[stream][0]
            if expr == None:
                if bit in routingBits:
                    expr = routingBits[bit]
                else:
                    log.fatal("Attempt to enable %s stream with the default expression, which is not defined." % stream)
                    raise RuntimeError
            streams[stream] = (bit, expr, True)

        ## Check if default enabled streams are on
        disabled = [s[0] for s in streams.iteritems() if not s[1][2] and s[0] in defaultEnabled]
        if disabled:
            log.warning( '##########################################################')
            log.warning( 'Normally enabled streams are disabled: %s.' % ', '.join(disabled) )
            log.warning( '##########################################################')

        if {k : (bit, on) for (k, (bit, expr, on)) in streams.iteritems()} != knownStreams:
            log.warning( '##########################################################' )
            log.warning( 'Non-standard stream configuration' )
            log.warning( 'Enabled:  %s' % ', '.join(s[0] for s in streams.iteritems() if s[1][2]) )
            log.warning( 'Disabled: %s' % ', '.join(s[0] for s in streams.iteritems() if not s[1][2]) )
            log.warning( '##########################################################' )

        ## Do the actual disabling of streams
        for stream, (bit, expr, on) in streams.iteritems():
            if not on and bit in routingBits:
                routingBits.pop(bit)
            if on and routingBits.get(bit, None) != expr:
                log.warning( 'Setting non standard routing bit expression for %s stream' % stream)
                log.warning( 'Now:     %s' % expr)
                log.warning( 'Default: %s' % routingBits.get(bit, "Disabled"))
                log.warning( '##########################################################' )
                routingBits[bit] = expr

        ## Record the settings in the ANN service
        from Configurables       import HltANNSvc
        HltANNSvc().RoutingBits = dict( [ (v,k) for k,v in routingBits.iteritems() ] )
        # LoKi::Hybrid::HltFactory is what RoutingBitsWriter uses as predicate factory..
        # make sure 'strings' is known...
        # make sure 'RATE,SCALE and SKIP' are known...
        from Configurables import LoKi__Hybrid__HltFactory as HltFactory
        HltFactory('ToolSvc.LoKi::Hybrid::HltFactory').Modules += [ 'LoKiCore.functions', 'LoKiNumbers.sources' ]
        #  forward compatibility: HltFactory will become private for HltRoutingBitsWriter...
        for stage in ('Hlt1', 'Hlt2'):
            HltFactory(stage + 'RoutingBitsWriter.LoKi::Hybrid::HltFactory').Modules += [ 'LoKiCore.functions', 'LoKiNumbers.sources' ]

        # and, last but not least, tell the writers what they should write..
        HltRoutingBitsWriter('Hlt1RoutingBitsWriter').RoutingBits = {k : routingBits[k] for k in routingBits.iterkeys() if k < 64}
        HltRoutingBitsWriter('Hlt2RoutingBitsWriter').RoutingBits = routingBits

    def configurePersistence(self, hlt1Lines, lines, stage) :
        """
        persistify vertices
        """
        ## and persist some vertices...
        from Configurables import HltVertexReportsMaker
        vertices =[ 'PV3D' ]
        selections = set()
        for i in hlt1Lines:
            selections = selections.union(set([j for j in i.outputSelections()]))
        vertices = [ i for i in vertices if i in selections ]

        vtxMaker = HltVertexReportsMaker(stage + "VtxReportsMaker")
        vtxMaker.VertexSelections = vertices
        #Can't do this any longer, need replacing with a smart way to get the vertex locations
        #HltVertexReportsMaker().Context = "HLT"
        vtxMaker.PVLocation= onlinePV()["PV3D"]

        ## do not write out the candidates for the vertices we store
        from Configurables import Hlt1SelReportsMaker, Hlt2SelReportsMaker
        configs = {'Hlt1' : Hlt1SelReportsMaker, 'Hlt2' : Hlt2SelReportsMaker}
        selMaker = configs[stage](stage + "SelReportsMaker")
        selMaker.SelectionMaxCandidates.update( dict( [ (i, 0) for i in vertices if i.endswith('Decision') ] ) )

        ## Related info locations need to be passed the SelReportsMaker and the
        ## HltANNSvc needs to be updated
        from Configurables import HltANNSvc
        locIDs = HltANNSvc().RelInfoLocations
        infoLocs = []
        for line in lines:
            locs = line.relatedInfoLocations()
            infoLocs.extend(locs)
            if locs:
                selMaker.RelatedInfoLocations[line.decision()] = locs
        __updateDict__(locIDs, 50, sorted(list(set(infoLocs))))
        HltANNSvc().RelInfoLocations = locIDs

        # Veto some selections
        veto = [ 'TES:Trig/L0/FullCalo', 'TES:Trig/L0/MuonCtrl', 'TES:Hlt/Vertex/ASidePV3D',
                 'TES:Hlt/Vertex/CSidePV3D', 'TES:Hlt2/Track/Forward', 'RZVeloBW',
                 'TES:Hlt/Track/RZVelo', 'TES:Hlt2/Track/Velo',
                 'TES:Hlt/Vertex/PV3D', 'TES:Hlt/Track/MuonSegmentForL0Single' ]
        selMaker.SelectionMaxCandidatesDebug = dict( [ (i,0) for i in veto ] )

    def configureANNSelections(self) :
        """
        Assigned numbers configuration
        """

        ### TODO: use the computed indices available from the lines...
        ### TODO: what about shared selections??? (which appear with multiple indices!)
        ###       but which have names not prefixed by the line name
        ### Make sure that the ANN Svc has everything it will need
        from HltLine.HltLine     import hlt1Selections, hlt1Decisions
        from HltLine.HltLine     import hlt2Selections, hlt2Decisions
        from Configurables       import HltANNSvc
        hltSelections = {'Hlt1' : (hlt1Selections(), hlt1Decisions(), 1000, 11000),
                         'Hlt2' : (hlt2Selections(), hlt2Decisions(), 5000, 50000)}
        for stage, (selections, decisions, decStart, selStart) in hltSelections.iteritems():
            ids = getattr(HltANNSvc(), '%sSelectionID' % stage)
            missing  = [ i for i in sorted(set(selections['Output']) - set(ids.keys())) if not i.startswith('TES:') ]
            missing += [ i for i in sorted(set(decisions) - set(ids.keys())) ]
            missing = sorted(list(set(missing)))
            missingDecisions  = [ i for i in missing if i.endswith('Decision') ]
            __updateDict__( ids, decStart, missingDecisions )
            missingSelections = [ i for i in missing if not i.endswith('Decision') ]
            __updateDict__( ids, selStart, missingSelections )
            log.warning( '# added %d %s selections to HltANNSvc' %  (len( missingSelections ), stage ) )
            log.warning( '# added %d %s decisions to HltANNSvc' % ( len( missingDecisions ), stage ) )

        if self.getProp('PruneHltANNSvc') :
            # prune all Decisions which are not members of Htl1 or Hlt2...
            def genName( c ) :
                if type(c) != str : c = c.name()
                return '%sDecision'%c
            for stage in ('Hlt1', 'Hlt2'):
                decnames = set([ genName(i) for i in Sequence(stage).Members ])
                # remove 'stale' entries
                ids = getattr(HltANNSvc(), '%sSelectionID' % stage)
                extradecnames = [ i for i in ids.keys() if i.endswith('Decision') and i not in decnames ]
                #print 'stale %s entries : %s ' % (stage, extradecnames)
                for i in extradecnames : del ids[i]
                setattr(HltANNSvc(), '%sSelectionID' % stage, ids)

        # given that both Hlt1 and Hlt2 end up in the same rawbank, and thus
        # effectively 'share a namespace' we MUST make sure that there is no overlap
        # between Hlt1SelectionID and Hlt2SelectionID -- because if there is, on
        # decoding, the Hlt1SelectionID _will_ be used... (as all the decoding knows
        # is the number, and it first checks Hlt1SelectionID).
        overlap =  set( HltANNSvc().Hlt1SelectionID.values() ).intersection( HltANNSvc().Hlt2SelectionID.values() )
        if overlap :
            raise RuntimeError, ' # Hlt1 and Hlt2 have overlapping ID values: %s -- this will cause problems when decoding the raw bank' % overlap

    def _runHltLines(self):
        from HltLine.HltLine     import hlt2Lines

        activeHlt1Lines = []
        activeHlt2Lines = []
        sets = self.settings()
        if ( sets ):
            activeHlt1Lines = sets.ActiveHlt1Lines()
            activeHlt2Lines = sets.ActiveHlt2Lines()
        else :
            activeHlt1Lines = [ i.name() for i in hlt1Lines() ]
            activeHlt2Lines = [ i.name() for i in hlt2Lines() ]

        activeHlt1Lines.extend( self.getProp('AdditionalHlt1Lines')  )
        activeHlt2Lines.extend( self.getProp('AdditionalHlt2Lines')  )

        for l in self.getProp('RemoveHlt1Lines'):
            if l in activeHlt1Lines:
                activeHlt1Lines.remove(l)
        for l in self.getProp('RemoveHlt2Lines'):
            if l in activeHlt2Lines:
                activeHlt2Lines.remove(l)

        
        # make sure Hlt.Global is included as soon as there is at least one Hlt. line...
        if activeHlt1Lines : activeHlt1Lines += [ 'Hlt1Global' ]
        if activeHlt2Lines : activeHlt2Lines += [ 'Hlt2Global' ]

        # Brute force uniquifier, which conserves order
        def unique( s ) :
            u = []
            for x in s:
               if x not in u:
                    u.append(x)
               else :
                    log.warning('Duplicate entry in requested list of lines: %s; please fix ' % x  )
            return u

        activeHlt1Lines = unique( activeHlt1Lines )
        activeHlt2Lines = unique( activeHlt2Lines )

        if self.getProp('Split') :
            if self.getProp('Split') == 'Hlt1' : activeHlt2Lines = []
            if self.getProp('Split') == 'Hlt2' : activeHlt1Lines = []
            if self.getProp('Split') not in [ 'Hlt1','Hlt2','' ] :
                raise KeyError("invalid value for property 'Split': %s -- must be either 'Hlt1', or 'Hlt2', or ''." % self.getProp('Split'))

        return activeHlt1Lines, activeHlt2Lines

    def postConfigAction(self) :
        """
        Add the configured lines into the Hlt1 and Hlt2 sequencers,
        provided there is no reason not to do so...

        @todo remove this method
        """
        from HltLine.HltLine     import hlt1Lines
        from HltLine.HltLine     import hlt2Lines
        activeHlt1Lines, activeHlt2Lines = self._runHltLines()

        print '# List of requested Hlt1Lines : %s ' % activeHlt1Lines
        # print '# List of available Hlt1Lines : %s ' % [ i.name() for i in hlt1Lines() ]
        awol1 = set( activeHlt1Lines ) - set( [ i.name() for i in hlt1Lines() ] )
        if awol1 : log.fatal(' # some requested Hlt1 lines are absent : %s ' % awol1 )

        print '# List of requested Hlt2Lines : %s ' % activeHlt2Lines
        # print '# List of available Hlt2Lines : %s ' % [ i.name() for i in hlt2Lines() ]
        awol2 = set( activeHlt2Lines ) - set( [ i.name() for i in hlt2Lines() ] )
        if awol2 : log.fatal(' # some requested Hlt2 lines are absent : %s ' % awol2 )

        if awol1 or awol2 :
            raise RuntimeError, ' # some requested lines are absent;\n Hlt1: %s\n Hlt2: %s' % ( awol1 , awol2 )


        lines1 = [ i for i in hlt1Lines() if  i.name() in activeHlt1Lines ]
        log.info( '# List of configured Hlt1Lines : ' + str(hlt1Lines()) )
        log.info( '# List of Hlt1Lines added to Hlt1 : ' + str(lines1) )
        log.info( '# List of configured Hlt1Lines not added to Hlt1 : ' + str(set(hlt1Lines())-set(lines1)) )
        Sequence('Hlt1').Members = [ i.configurable() for i in lines1 ]

        # for i in hlt2Lines() : print '# active line :', i.name(), ' found :', i.name() in activeHlt2Lines

        lines2 = [ i for i in hlt2Lines() if i.name() in activeHlt2Lines  ]
        log.info( '# List of configured Hlt2Lines : ' + str(hlt2Lines())  )
        log.info( '# List of Hlt2Lines added to Hlt2 : ' + str( lines2 )  )
        log.info( '# List of configured Hlt2Lines not added to Hlt2 : ' + str(set(hlt2Lines())-set(lines2)) )
        Sequence('Hlt2').Members += [ i.configurable() for i in lines2 ]

        # switch on timing limit / accept if slow
        for i in lines1 :
                i.configurable().AcceptIfSlow = self.getProp('EnableAcceptIfSlow')
                i.configurable().FlagAsSlowThreshold = self.getProp('SlowHlt1Threshold')
        for i in lines2 :
                i.configurable().AcceptIfSlow =  self.getProp('EnableAcceptIfSlow')
                i.configurable().FlagAsSlowThreshold = self.getProp('SlowHlt2Threshold')

        for stage, lines in zip(('Hlt1', 'Hlt2'), (lines1, lines2)):
            self.configurePersistence( lines1, lines, stage )
        self.configureANNSelections()

        from HltConf.HltMonitoring import HltMonitoringConf
        HltMonitoringConf().configureHltMonitoring(lines1, lines2)

        if self.getProp("Verbose") : print Sequence('Hlt')

    def _safeSet(self, option, newVal):
        """
        Set properties safely, only if not already set by someone.. I'm pretty sure there is a way to do this in gudi with owned configurables ...
        """
        if option not in self.__slots__:
            raise AttributeError("No such option, " + str(option))
        if self.isPropertySet(option):
            log.warning('#Asked to reset already set property, '+str(option)+', ignoring request')
            return False
        self.setProp(option, newVal)
        return True

    def endSequence(self):
        """
        define end sequence (persistence + monitoring)
        """
        from Configurables        import GaudiSequencer as Sequence
        from Configurables        import bankKiller
        from Configurables        import LoKi__HDRFilter   as HltFilter
        from Configurables        import Hlt1SelReportsMaker, Hlt2SelReportsMaker, HltSelReportsWriter
        from Configurables        import HltDecReportsWriter
        from Configurables        import HltVertexReportsMaker, HltVertexReportsWriter
        from Configurables        import HltTrackReportsWriter
        from Configurables        import HltRoutingBitsWriter
        from Configurables        import HltLumiWriter
        from Configurables        import DeterministicPrescaler as Prescale
        from HltLine.HltDecodeRaw import DecodeIT, DecodeVELO, DecodeL0DU
        from Configurables        import LoKi__VoidFilter as Filter

        sets = self.settings()
        if sets and hasattr(sets, 'StripEndSequence') and getattr(sets,'StripEndSequence'):
            log.warning('### Setting requests stripped down HltEndSequence ###')
            strip = getattr(sets,'StripEndSequence')
            for option in ['EnableHltSelReports','EnableHltVtxReports', 'EnableHltTrkReports','EnableLumiEventWriting']:
                self._safeSet(option, (option in strip))


        activehlt1lines, activehlt2lines = self._runHltLines()

        # make sure we encode from the locations the decoders will use...
        from DAQSys.Decoders import DecoderDB
        hlt1_decrep_loc = DecoderDB["HltDecReportsDecoder/Hlt1DecReportsDecoder"].listOutputs()[0]
        hlt2_decrep_loc = DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"].listOutputs()[0]
        hlt1_selrep_loc = DecoderDB["HltSelReportsDecoder/Hlt1SelReportsDecoder"].listOutputs()[0]
        hlt2_selrep_loc = DecoderDB["HltSelReportsDecoder/Hlt2SelReportsDecoder"].listOutputs()[0]
        hlt1_vtxrep_loc = DecoderDB["HltVertexReportsDecoder/Hlt1VertexReportsDecoder"].listOutputs()[0]
        hlt2_vtxrep_loc = DecoderDB["HltVertexReportsDecoder/Hlt2VertexReportsDecoder"].listOutputs()[0]

        ## L0 decoder
        from DAQSys.DecoderClass import decodersForBank
        l0decoder = decodersForBank( DecoderDB, 'L0DU' )
        assert len(l0decoder)
        l0decoder = l0decoder[0].setup()

        ### store the BDT response (and a bit more) through ExtraInfo on particles:
        sel_rep_opts =  dict( InfoLevelDecision = 3, InfoLevelTrack = 3, InfoLevelRecVertex = 3,
                              InfoLevelCaloCluster = 3, InfoLevelParticle = 3,
                              )

        ### HLT2 rec summary location, empty if HltAfterburner is disabled
        if self.getProp("EnableHltAfterburner"):
            recSumLoc = HltAfterburnerConf().getProp("RecSummaryLocation")
        else:
            recSumLoc = ""

        ### FIXME/TODO: having the routing bits writer(s) in the postamble implies they do NOT run for rejected events.
        ###             Is that really appropriate? Maybe we don't care, as those events (in the pit!) are never seen
        ###             downstream, but eg. for MC they may be kept...
        ### So maybe the routingbits should be written in the end sequence instead, but then the code must take into
        ### account that Hlt2 missing on Hlt1 rejected events is not an error...
        _hlt1postamble = ( ( "EnableHltRoutingBits" ,  type(l0decoder), l0decoder.getName(), {})
                         , ( "EnableHltRoutingBits" ,  HltRoutingBitsWriter,   'Hlt1RoutingBitsWriter', {'Hlt1DecReportsLocation': hlt1_decrep_loc,
                                                                                                         'Hlt2DecReportsLocation' : '',  } )
                         , ( "EnableHltDecReports"  ,  HltDecReportsWriter,    'Hlt1DecReportsWriter',  {'SourceID' : 1,
                                                                                                         'InputHltDecReportsLocation' : hlt1_decrep_loc } )
                         , ( "EnableHltSelReports"  ,  Hlt1SelReportsMaker,    'Hlt1SelReportsMaker',   dict( InputHltDecReportsLocation = hlt1_decrep_loc
                                                                                                            , OutputHltSelReportsLocation = hlt1_selrep_loc
                                                                                                            , **sel_rep_opts )  )
                         , ( "EnableHltSelReports"  ,  HltSelReportsWriter,    'Hlt1SelReportsWriter',  {'SourceID' : 1,
                                                                                                         'InputHltSelReportsLocation': hlt1_selrep_loc } )
                         , ( "EnableHltTrkReports"  ,  HltTrackReportsWriter,  'Hlt1TrkReportsWriter',  {})
                         , ( "EnableHltVtxReports"  ,  HltVertexReportsMaker,  'Hlt1VtxReportsMaker',   {'OutputHltVertexReportsLocation' : hlt1_vtxrep_loc } )
                         , ( "EnableHltVtxReports"  ,  HltVertexReportsWriter, 'Hlt1VtxReportWriter',  { 'SourceID' : 1,
                                                                                                         'InputHltVertexReportsLocation': hlt1_vtxrep_loc } )
                         )
        _hlt2postamble = ( ( "EnableHltRoutingBits" ,  type(l0decoder), l0decoder.getName(), {})
                         , ( "EnableHltRoutingBits" ,  HltRoutingBitsWriter,   'Hlt2RoutingBitsWriter', { 'Hlt1DecReportsLocation' : hlt1_decrep_loc,
                                                                                                          'Hlt2DecReportsLocation' : hlt2_decrep_loc,
                                                                                                          'UpdateExistingRawBank'  : True} )
                         , ( "EnableHltDecReports"  ,  HltDecReportsWriter,    'Hlt2DecReportsWriter',  { 'SourceID': 2,
                                                                                                          'InputHltDecReportsLocation' : hlt2_decrep_loc } )
                         , ( "EnableHltSelReports"  ,  Hlt2SelReportsMaker,    'Hlt2SelReportsMaker',   dict( InputHltDecReportsLocation = hlt2_decrep_loc,
                                                                                                              OutputHltSelReportsLocation = hlt2_selrep_loc,
                                                                                                              RecSummaryLocation = recSumLoc,
                                                                                                              **sel_rep_opts ) )
                         , ( "EnableHltSelReports"  ,  HltSelReportsWriter,    'Hlt2SelReportsWriter',  { 'SourceID' : 2,
                                                                                                          'InputHltSelReportsLocation': hlt2_selrep_loc } )
                         , ( "EnableHltVtxReports"  ,  HltVertexReportsMaker,  'Hlt2VtxReportsMaker',   { 'OutputHltVertexReportsLocation' : hlt2_vtxrep_loc } )
                         , ( "EnableHltVtxReports"  ,  HltVertexReportsWriter, 'Hlt2VtxReportWriter',   { 'SourceID' : 2,
                                                                                                          'InputHltVertexReportsLocation': hlt2_vtxrep_loc } ) )

        # Don't try to decode L0 for the routing bits writer if no L0TCK has
        # been set. This allows forward compatibility.
        if self.getProp('L0TCK') is None:
            _hlt1postamble = _hlt1postamble[1 :]
            _hlt2postamble = _hlt2postamble[1 :]

        # make sure we only instantiate members which are used...
        instantiate = lambda name, cfg : Sequence( name,
                                                   IgnoreFilterPassed = True,
                                                   Members = [ tp( nm, **props ) for (gate,tp,nm,props) in cfg if self.getProp(gate) ]  )
        Monitoring    = Sequence("HltMonitorSequence", IgnoreFilterPassed = True)
        End           = Sequence( 'HltEndSequence', [] )
        Hlt1PostAmble = instantiate( 'Hlt1Postamble',_hlt1postamble )
        Hlt2PostAmble = instantiate( 'Hlt2Postamble',_hlt2postamble )

        # Configure monitoring
        from HltMonitoring import HltMonitoringConf
        HltMonitoringConf().MonitorSequence = Monitoring
        # Disable L0 monitoring if no L0TCK has been set
        if self.getProp('L0TCK') is None:
            HltMonitoringConf().EnableL0Monitor = False

        if (self.getProp("EnableLumiEventWriting")) :
            if sets and hasattr(sets, 'NanoBanks') :
                if not self.isPropertySet('NanoBanks') :
                    self.setProp('NanoBanks',sets.NanoBanks)
                else :
                    log.warning('Setting %s requested NanoBanks = %s, but also explicitly set; using %s.' % (sets.HltType(), sets.NanoBanks, self.getProp('NanoBanks')))
            if self.isPropertySet('NanoBanks') :
                    log.warning('Using non-default NanoBanks = %s.' % (self.getProp('NanoBanks')))

            from DAQSys.Decoders import DecoderDB
            decoder = DecoderDB["HltDecReportsDecoder/Hlt1DecReportsDecoder"]
            End.Members += [ HltLumiWriter()
                           , Sequence( 'LumiStripper' , Members =
                                       [ decoder.setup()
                                       , HltFilter('LumiStripperHlt1Filter', Code = self.getProp('LumiBankKillerPredicate')
                                                  , Location = decoder.listOutputs()[0])
                                       , Prescale('LumiStripperPrescaler', AcceptFraction=self.getProp('LumiBankKillerAcceptFraction'))
                                       , bankKiller('LumiStripperBankKiller', BankTypes=self.getProp('NanoBanks'),  DefaultIsKill=True )
                                       ] )
                           ]

        if self.getProp('RequireL0ForEndSequence') :
            from Configurables import LoKi__L0Filter as L0Filter
            from HltLine.HltDecodeRaw import DecodeL0DU
            L0accept = Sequence(name='HltEndSequenceFilter', Members = DecodeL0DU.members() + [ L0Filter( 'L0Pass', Code = "L0_DECISION_PHYSICS" )])
            EndMembers.insert(1,  L0accept )  # after routing bits

    def __apply_configuration__(self):
        """
        Apply Hlt configuration
        """
        log.debug("Hlt configuration")
        import GaudiKernel.ProcessJobOptions
        GaudiKernel.ProcessJobOptions.PrintOff()
        import HltConf.HltInit  # make sure ANN numbers are assigned...

        self.configureFactories()
        self.confType()
        self.endSequence()
        self.configureRoutingBits()

        #appendPostConfigAction( self.postConfigAction )
        GaudiKernel.Configurable.postConfigActions.insert( 0,  self.postConfigAction )
