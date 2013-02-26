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

##################################################################################
class HltConf(LHCbConfigurableUser):
    """
    Hlt configuration
    """
    __used_configurables__ = [ Hlt1Conf
                             , Hlt2Conf ]
    __slots__ = { "L0TCK"                          : ''
                , 'ForceSingleL0Configuration'     : True
                , 'SkipDisabledL0Channels'         : False
                , "DataType"                       : '2010'
                , "Verbose"                        : False      # print the generated Hlt sequence
                , "HistogrammingLevel"             : 'None'     # or 'Line'
                , "ThresholdSettings"              : ''         #  select a predefined set of settings, eg. 'Effective_Nominal'
                , "EnableMonitoring"               : False      # enable embedded monitoring in FilterDesktop/CombineParticles
                , "EnableHltGlobalMonitor"         : True
                , "EnableHltL0GlobalMonitor"       : True
                , "EnableBeetleSyncMonitor"        : False
                , "EnableHltDecReports"            : True
                , "EnableHltSelReports"            : True
                , "EnableHltVtxReports"            : True
                , "EnableHltRoutingBits"           : True
                , "EnableLumiEventWriting"         : False
                , "EnableAcceptIfSlow"             : False      # Switch on AcceptIfSlow switch of HltLine
                , "SlowHlt1Threshold"              : 500000     # microseconds
                , "SlowHlt2Threshold"              : 5000000     # microseconds
                , 'RequireL0ForEndSequence'        : True
                , 'RequireRoutingBits'             : [] # to require not lumi exclusive, set to [ 0x0, 0x4, 0x0 ]
                , 'VetoRoutingBits'                : []
                , 'SkipHltRawBankOnRejectedEvents' : False
                , 'LumiBankKillerPredicate'        : "(HLT_PASS_SUBSTR('Hlt1Lumi') & ~HLT_PASS_RE('Hlt1(?!Lumi).*Decision'))|(HLT_PASS_SUBSTR('Hlt2Lumi') & ~HLT_PASS_RE('Hlt2(?!Lumi).*Decision'))"  
                , 'BeetleSyncMonitorRate'          : 5000
                , "LumiBankKillerAcceptFraction"   : 0.9999     # fraction of lumi-only events where raw event is stripped down
                                                                # (only matters if EnablelumiEventWriting = True)
                , "AdditionalHlt1Lines"            : []         # must be configured
                , "AdditionalHlt2Lines"            : []         # must be configured
                , "ExpressStreamRateLimit"         : 50         # Hz
                , "NanoBanks"                      : ['ODIN','HltLumiSummary','HltRoutingBits','DAQ']
                  }

    __settings__ = None 
    #import logging
    #_log = logging.getLogger( 'HltConf.Configuration')
    #handler = logging.StreamHandler()
    #formatter = logging.Formatter("# %(name)s - %(levelname)s : %(message)s")
    #handler.setFormatter( formatter )
    #_log.addHandler( handler )
    #_log.setLevel(logging.DEBUG)

##################################################################################
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

##################################################################################
    def settings(self) :
        """
        Get the class that contains the thresholds, etc
        """
        thresName  = self.getProp('ThresholdSettings')   # the name
        if not thresName : thresName = self.settingsForDataType( self.getProp('DataType') )
        if not thresName : return None
        from HltConf.ThresholdUtils import Name2Threshold
        return Name2Threshold(thresName)

##################################################################################
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
        if self.getProp('L0TCK') :
            if L0TCK != self.getProp('L0TCK') :
                log.warning( '####################################################################################' )
                log.warning( '## WARNING You are configuring the HLT to run on top of an L0 configuration       ##' )
                log.warning( '## WARNING which is different then the one it was generated for                   ##' )
                log.warning( '## WARNING This may not work, but if it does, it may result in undefined behavior ##' )
                log.warning( '## WARNING Please make sure you know what you are doing!!                         ##' )
                log.warning( '####################################################################################' )
            L0TCK = self.getProp('L0TCK')
        self.defineL0Channels( L0TCK )
        #
        #  main HLT sequencer
        # 
        Hlt = Sequence('Hlt', ModeOR= True, ShortCircuit = False
                      , Members = [ Sequence('HltDecisionSequence', Members = [ Sequence('Hlt1') 
                                                                              , Sequence('Hlt2') 
                                                                              ] )
                                  , Sequence('HltEndSequence') 
                                  ] 
                      )
        if self.getProp('RequireRoutingBits') or self.getProp('VetoRoutingBits') :
            from Configurables import HltRoutingBitsFilter
            filter = HltRoutingBitsFilter( "PhysFilter" )
            if self.getProp('RequireRoutingBits') : filter.RequireMask = self.getProp('RequireRoutingBits')
            if self.getProp('VetoRoutingBits')    : filter.VetoMask    = self.getProp('VetoRoutingBits') 
            Sequence("HltDecisionSequence").Members.insert(0,filter)
            Sequence("HltEndSequence").Members.insert(0,filter)
        #
        # dispatch Hlt1 configuration
        #
        Hlt1Conf()
        Hlt1Conf().ThresholdSettings = ThresholdSettings
        #
        # dispatch Hlt2 configuration
        #
        Hlt2Conf()
        self.setOtherProps(Hlt2Conf(),[ "DataType" ])
        Hlt2Conf().ThresholdSettings = ThresholdSettings
        if thresClass and hasattr( thresClass, 'Hlt2DefaultVoidFilter' ) :
            Hlt2Conf().DefaultVoidFilter = getattr( thresClass, 'Hlt2DefaultVoidFilter' )


#########################################################################################
# Utility function for setting thresholds both in Hlt1 and 2
#
    def settingsForDataType( self, x ) :
        """
        Defaults per datatype
        """
        _dataType2Settings = { 'DC06' : None ,  # development is default
                               'MC09' : None }  # development is default
        return _dataType2Settings[x] if x in _dataType2Settings else None
    
##################################################################################
    def configureRoutingBits(self) :
        """
        Routing bits
        """
        ## set triggerbits
        #  0-7 : reserved for ODIN  // need to add ODIN support to routing bit writer
        #  8-31: reserved for L0    // need to add L0DU support to routing bit writer
        # 32-63: reserved for Hlt1
        # 64-91: reserved for Hlt2

        ### NOTE: any change in the _meaning_ of any of the following needs to be 
        ###       communicated (at least!) with online, to insure the events are 
        ###       still properly routed!!! (this goes esp. for [32,36]!!!)
        ### NOTE: current usage:
        ###       bit 46 -> 'physics triggers'
        ###       bit 37 -> subscrided to be Hlt monitoring
        ###       bit 36 -> express stream
        ###       bit 35 -> subscribed to by Velo closing monitoring
        ###       bit 34 -> count number of 'non-lumi-exlusive events'
        ###       bit 33 -> lumi stream
        ###       bit 32 -> full stream (actually, not used for that, but it could be ;-)
        from Hlt1Lines.HltL0Candidates import L0Channels

        from Configurables import HltRoutingBitsWriter
        routingBits = {  0 : '( ODIN_BXTYP == LHCb.ODIN.Beam1 ) | ( ODIN_BXTYP == LHCb.ODIN.BeamCrossing )'
                      ,  1 : '( ODIN_BXTYP == LHCb.ODIN.Beam2 ) | ( ODIN_BXTYP == LHCb.ODIN.BeamCrossing )'
                      ,  3 : 'ODIN_TRUE' 
                      ,  4 : 'ODIN_TRGTYP == LHCb.ODIN.LumiTrigger'
                      ,  8 : 'L0_DECISION_PHYSICS'
                      ,  9 : "L0_CHANNEL_RE('B?gas')"
                      , 10 : "|".join( [ "L0_CHANNEL('%s')" % chan for chan in [ 'CALO','MUON,minbias' ] if chan in L0Channels() ] )
                      , 11 : "|".join( [ "L0_CHANNEL('%s')" % chan for chan in [ 'Electron','Photon','Hadron','Muon','DiMuon','Muon,lowMult','DiMuon,lowMult','Electron,lowMult','Photon,lowMult','DiEM,lowMult','DiHadron,lowMult'] if chan in L0Channels() ] )
                      , 12 : "L0_CHANNEL('CALO')" if 'CALO' in L0Channels() else "" # note: need to take into account prescale in L0...
                      , 13 : "L0_CHANNEL( 'Hadron' )" if 'Hadron' in L0Channels() else ""
                      , 14 : "L0_CHANNEL_RE('Electron|Photon')" 
                      , 15 : "L0_CHANNEL_RE('Muon|DiMuon')"
                      , 16 : "L0_CHANNEL_RE('.*NoSPD')" 
                      , 17 : "L0_CHANNEL_RE('.*,lowMult')"
                      , 18 : "L0_CHANNEL('DiMuon')" if 'DiMuon' in L0Channels() else ""
                      , 32 : "HLT_PASS('Hlt1Global')"
                      , 33 : "HLT_PASS_RE('^Hlt[12]Lumi.*Decision$')"  # lumi stream
                      , 34 : " ~ ( %s ) " % self.getProp('LumiBankKillerPredicate') #  this must be the opposite of the LumiStripper, i.e. if 34 is set, the event should NEVER be a nanoevent...
                      , 35 : "HLT_PASS_SUBSTR('Hlt1BeamGas')" # beamgas stream
                      , 36 : "scale(%s,RATE(%s))" % ( "HLT_PASS_RE('Hlt2Express.*Decision')", self.getProp('ExpressStreamRateLimit') )  # express stream
                      , 37 : "HLT_PASS_RE('Hlt1(?!BeamGas).*Decision')"  # note: we need the 'Decision' at the end to _exclude_ Hlt1Global # full stream
                      , 38 : "HLT_PASS('Hlt1ODINTechnicalDecision')"
                      , 39 : "HLT_PASS_SUBSTR('Hlt1L0')"
                      , 40 : "HLT_PASS_RE('Hlt1(Velo|MB|BeamGas).*Decision')"  # bit (to be) used by the Velo (closing) monitoring
                      , 41 : "HLT_PASS_RE('Hlt1(Single|Track)Muon.*Decision')"
                      , 42 : "HLT_PASS_RE('Hlt1.*DiMuon.*Decision')"
                      , 44 : "HLT_PASS_RE('Hlt1.*Electron.*Decision')"
                      , 46 : "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"    # exclude 'non-physics' lines
                      , 47 : "HLT_PASS_RE('Hlt1MBMicroBias.*Decision')"
                      , 48 : "HLT_PASS('Hlt1MBNoBiasDecision')"
                      , 49 : "HLT_PASS_SUBSTR('Hlt1BeamGas')"
                      , 50 : "HLT_PASS('Hlt1LumiLowBeamCrossingDecision')"
                      , 51 : "HLT_PASS('Hlt1LumiMidBeamCrossingDecision')"
                      , 54 : "HLT_PASS_RE('Hlt1.*Muon.*Decision')"
                      , 55 : "HLT_PASS_RE('Hlt1Track.*Decision')"  
                      , 56 : "HLT_PASS_RE('Hlt1TrackAllL0.*Decision')"
                      , 57 : "HLT_PASS_RE('Hlt1TrackMuon.*Decision')"
                      , 58 : "HLT_PASS_RE('Hlt1TrackPhoton.*Decision')"
                      , 59 : "HLT_PASS_RE('Hlt1.*DisplVertex.*Decision')"
                      # 64--96: Hlt2
                      , 64 : "HLT_PASS('Hlt2Global')"
                      , 65 : "HLT_PASS('Hlt2DebugEventDecision')"
                      , 66 : "HLT_PASS_RE('Hlt2(?!Transparent).*Decision')"
                      , 67 : "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision')"
                      , 68 : "HLT_PASS_RE('Hlt2.*DiMuon.*Decision')"
                      , 69 : "HLT_PASS_RE('Hlt2.*DY.*Decision')"
                      , 70 : "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                      , 71 : "HLT_PASS_RE('Hlt2.*Charm.*Decision')"
                      , 72 : "HLT_PASS_RE('Hlt2.*IncPhi.*Decision')"
                      , 73 : "HLT_PASS_RE('Hlt2.*B.*Gamma.*Decision')"
                      , 74 : "HLT_PASS_RE('Hlt2.*TriMuon.*Decision')"
                      , 76 : "HLT_PASS_RE('Hlt2.*(Bu2|Bs2|Bd2|Bc2|B2HH|Dst2|DisplVertices).*Decision')" 
                      , 77 : "HLT_PASS_RE('Hlt2(?!Forward)(?!DebugEvent)(?!Express)(?!Lumi)(?!Transparent)(?!PassThrough).*Decision')"
                      , 78 : "HLT_PASS_RE('Hlt2.*Muon.*Decision')"
                      , 79 : "HLT_PASS_RE('Hlt2.*(Topo|Charm|IncPhi).*Decision')"
                      , 80 : "HLT_PASS_RE('Hlt2.*Electron.*Decision')"
                      , 81 : "HLT_PASS_RE('Hlt2Topo.*2Body.*Decision')"   
                      , 82 : "HLT_PASS_RE('Hlt2Topo.*3Body.*Decision')"   
                      , 83 : "HLT_PASS_RE('Hlt2Topo.*4Body.*Decision')"   
                      , 84 : "HLT_PASS_RE('Hlt2TopoMu[234]Body.*Decision')"   
                      , 85 : "HLT_PASS_RE('Hlt2TopoE[234]Body.*Decision')"   
                      , 86 : "HLT_PASS_RE('Hlt2Topo[234]Body.*Decision')"
                      , 87 : "HLT_PASS_RE('Hlt2DisplVertices.*Decision')"
                      , 88 : "HLT_PASS_RE('Hlt2HighPtJets.*Decision')"
		              , 89 : "HLT_PASS_RE('Hlt2PassThrough.*Decision')"
                      , 90 : "HLT_PASS_RE('Hlt2.*Charm.*hhX.*Decision')"  
                      , 91 : "HLT_PASS_RE('Hlt2.*Charm.*_hhX.*Decision')"
                      , 92 : "HLT_PASS_RE('Hlt2.*Charm.*_Baryon.*hhX.*Decision')"
                      , 93 : "HLT_PASS_RE('Hlt2.*Charm.*_Lepton.*hhX.*Decision')"    
                         }
        HltRoutingBitsWriter().RoutingBits = routingBits

        ## and record the settings in the ANN service
        from Configurables       import HltANNSvc 
        HltANNSvc().RoutingBits = dict( [ (v,k) for k,v in routingBits.iteritems() ] )
        # LoKi::Hybrid::HltFactory is what RoutingBitsWriter uses as predicate factory..
        # make sure 'strings' is known... 
        # make sure 'RATE,SCALE and SKIP' are known...
        from Configurables import LoKi__Hybrid__HltFactory as HltFactory
        HltFactory('ToolSvc.LoKi::Hybrid::HltFactory').Modules += [ 'LoKiCore.functions', 'LoKiNumbers.sources' ]
        #  forward compatibility: HltFactory will become private for HltRoutingBitsWriter...
        HltFactory('HltRoutingBitsWriter.LoKi::Hybrid::HltFactory').Modules += [ 'LoKiCore.functions', 'LoKiNumbers.sources' ]
        # and, last but not least, tell the writer what it should write..

##################################################################################
    def configureVertexPersistence(self,lines) :
        """
        persistify vertices
        """
        ## and persist some vertices...
        from Configurables import HltVertexReportsMaker
        vertices =[ 'PV3D', 'ProtoPV3D'  ] 
        selections = []
        from HltLine.HltLine     import hlt1Lines
        for i in hlt1Lines() :
             if i.name() in lines : selections.extend( [ j for j in i.outputSelections() if j not in selections ] )
        vertices = [ i for i in vertices if i in selections ]
        HltVertexReportsMaker().VertexSelections = vertices
        #Can't do this any longer, need replacing with a smart way to get the vertex locations
        #HltVertexReportsMaker().Context = "HLT"
        from HltTracking.HltVertexNames import Hlt3DPrimaryVerticesName as PV3DSelection
        from HltTracking.HltVertexNames import ( _vertexLocation,
                                                 HltSharedVerticesPrefix,
                                                 HltGlobalVertexLocation )                  
        from HltTracking.HltPVs import ProtoPV3DSelection
        pv3d  = _vertexLocation( HltSharedVerticesPrefix, HltGlobalVertexLocation, PV3DSelection )
        HltVertexReportsMaker().PVLocation=pv3d
        ## do not write out the candidates for the vertices we store 
        from Configurables import HltSelReportsMaker
        HltSelReportsMaker().SelectionMaxCandidates.update( dict( [ (i,0) for i in vertices if i.endswith('Decision') ] ) )
        # We are in a post-config action so Hlt2 has already been called and has set the properties
        # of the unfitted tracking. Therefore it is safe to use it this way
        # What we are doing is to let the SelReportsMaker know where the "trackified" muonID objects
        # live directly from the tracking in question and the "rule" for computing their location
        # TODO: fix locations from tracking!!!!!
        # Note : if no Hlt2 lines are run, this is meaningless since the Hlt1 muons are
        #        handled differently, so we pass some empty "default" location instead
             
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        Hlt2BiKalmanFittedForwardTracking = Hlt2BiKalmanFittedForwardTracking()
        # We need to get the "extra" piece of the Muon stubs location compared to the track location
        tracking = 0
        try :
            tracking = Hlt2BiKalmanFittedForwardTracking.hlt2PrepareTracks()
        except :
            # Nobody configured the tracking so no meaningful Hlt2 was run
            # so just pass some default value
            HltSelReportsMaker().MuonIDSuffix = "/PID/MuonSegments"
        if tracking :
            trackLoc    = tracking.outputSelection()
            muonStubLoc	= Hlt2BiKalmanFittedForwardTracking._trackifiedMuonIDLocation() 	 
            HltSelReportsMaker().MuonIDSuffix = "/" + muonStubLoc.strip(trackLoc)

        veto = [ 'TES:Trig/L0/FullCalo' ,   'TES:Trig/L0/MuonCtrl'
               , 'TES:Hlt/Vertex/ASidePV3D','TES:Hlt/Vertex/CSidePV3D' , 'TES:Hlt2/Track/Forward',
                 'TES:Hlt/Track/RZVelo',    'TES:Hlt2/Track/Velo'
               , 'TES:Hlt/Vertex/PV3D' 
               , 'TES:Hlt/Track/MuonSegmentForL0Single'
               , 'RZVeloBW'
               ]
        HltSelReportsMaker().SelectionMaxCandidatesDebug = dict( [ (i,0) for i in veto ] )

##################################################################################
    def configureANNSelections(self) :
        """
        Assigned numbers configuration
        """

        def updateDict( d, id, entries ) :
             # pick up first unused entry after 'id'
             for i in entries :
                 while id in d.values() : id = id + 1
                 d.update({ i : id } )

        ### TODO: use the computed indices available from the lines...
        ### TODO: what about shared selections??? (which appear with multiple indices!)
        ###       but which have names not prefixed by the line name
        ### Make sure that the ANN Svc has everything it will need
        from HltLine.HltLine     import hlt1Selections
        from Configurables       import HltANNSvc 
        missing = [ i for i in sorted(set(hlt1Selections()['All']) - set(HltANNSvc().Hlt1SelectionID.keys())) if not i.startswith('TES:') ]
        missingDecisions  = [ i for i in missing if i.endswith('Decision') ]
        updateDict( HltANNSvc().Hlt1SelectionID, 1000, missingDecisions )
        missingSelections = [ i for i in missing if not i.endswith('Decision') ]
        updateDict( HltANNSvc().Hlt1SelectionID, 11000, missingSelections )
        log.warning( '# added %s selections to HltANNSvc' % len(missingSelections) )
        log.warning( '# added %s decisions to HltANNSvc' % len(missingDecisions)  )
        # TODO: add Hlt2 to this list, and remove the hardwired numbers from the Hlt2 lines...


        # prune all Decisions which are not members of Htl1 or Hlt2...
        def genName( c ) :
            if type(c) != str : c = c.name()
            return '%sDecision'%c                  

        hlt1decnames = [  genName(i) for i in Sequence('Hlt1').Members ]
        hlt2decnames = [  genName(i) for i in Sequence('Hlt2').Members ]
        # remove 'stale' entries
        hlt1extradecnames = [ i for i in HltANNSvc().Hlt1SelectionID.keys() if i.endswith('Decision') and i not in hlt1decnames ]
        hlt2extradecnames = [ i for i in HltANNSvc().Hlt2SelectionID.keys() if i.endswith('Decision') and i not in hlt2decnames ]
        #print 'stale Hlt1 entries : %s ' % hlt1extradecnames
        #print 'stale Hlt2 entries : %s ' % hlt2extradecnames
        for i in hlt1extradecnames : del HltANNSvc().Hlt1SelectionID[i]
        for i in hlt2extradecnames : del HltANNSvc().Hlt2SelectionID[i]

        # given that both Hlt1 and Hlt2 end up in the same rawbank, and thus
        # effectively 'share a namespace' we MUST make sure that there is no overlap
        # between Hlt1SelectionID and Hlt2SelectionID -- because if there is, on
        # decoding, the Hlt1SelectionID _will_ be used... (as all the decoding knows
        # is the number, and it first checks Hlt1SelectionID).
        overlap =  set( HltANNSvc().Hlt1SelectionID.values() ).intersection( HltANNSvc().Hlt2SelectionID.values() )
        if overlap :
            raise RuntimeError, ' # Hlt1 and Hlt2 have overlapping ID values: %s -- this will cause problems when decoding the raw bank' % overlap 

        if False :
            from HltLine.HltLine     import hlt1Lines
            for i in hlt1Lines() :
                print ( "checking " + i.name() )
                decisionName = i.name() + 'Decision'
                if decisionName in HltANNSvc().Hlt1SelectionID :
                    id = HltANNSvc().Hlt1SelectionID[ decisionName ] 
                else :
                    id = None

                if id :
                    print ( i.index() )
                    for (key,value ) in zip(i.index(),range(0,len(i.index()))) :
                        from HltLine.HltLine     import hlt1Selections
                        if key in hlt1Selections()['All'] :
                            # TODO: see if the selection is unique to this line...
                            unique = key.startswith(i.name())
                            cur = HltANNSvc().Hlt1SelectionID[ key ] if key in HltANNSvc().Hlt1SelectionID else  None
                            print( ' selection %s in line %s (unique:%s) should have ID %d:%d -- has %d' % ( key,  i.name(), unique, id, value, cur)  )
                        #else :
                        #    print ' line %s, algo %s does not have a selection? ' % (i.name(),key)
                else :
                    log.warning( 'Hlt1Line %s not known to ANNSvc??' % i.name() )

##################################################################################
    def groupLines( self, lines , mapping ) :
        import re
        groups = {}
        taken = []
        for pos in range(len(mapping)):
          (name, pattern) = mapping[pos]
          expr = re.compile(pattern)
          groups[ name ] = [ i for i in lines if expr.match(i) and i not in taken ]
          taken += groups[ name ]
        return groups

##################################################################################
    def configureHltMonitoring(self, lines1, lines2) :
        """
        Hlt Monitoring
        """
        ## and tell the monitoring what it should expect..
        # the keys are the Labels for the Histograms in the GUI
        # the values are the Pattern Rules to for the Decisions contributing 
        from Configurables import HltGlobalMonitor
        HltGlobalMonitor().DecToGroupHlt1  = self.groupLines( [ i.decision() for i in lines1 ],
                                [ ("L0"         , "Hlt1L0.*Decision"),
                                  ("LumiBeamGas", "Hlt1(Lumi|BeamGas).*Decision"),
                                  ("SingleMuon" , "Hlt1(Single|Track)Muon.*Decision"),
                                  ("DiMuon"     , "Hlt1DiMuon.*Decision"),
                                  ("TrackAllL0" , "Hlt1TrackAllL0.*Decision"),
                                  ("ECAL"       , "Hlt1.*(Electron|Photon).*Decision"),
                                  ("Commissioning" , "Hlt1(ODIN.*|Tell1Error|Incident)Decision"),
                                  ("MinBias"    , "Hlt1MB.*Decision"),
                                  ("Global"     , ".*Global.*"),
                                  ("Other"      , ".*") # add a 'catch all' term to pick up all remaining decisions...
                                ]
                                )
        HltGlobalMonitor().DecToGroupHlt2 = self.groupLines( [ i.decision() for i in lines2 ],
                                 [ ("Topo"           , "Hlt2Topo.*Decision"),
                                   ("IncPhi"         , "Hlt2IncPhi.*Decision"),
                                   ("SingleMuon"     , "Hlt2Single.*Muon.*Decision"),
                                   ("DiMuon"         , "Hlt2.*DiMuon.*Decision"),
                                   ("B2DX"           , "Hlt2B2D2.*Decision"),
                                   ("B2XGamma"       , "Hlt2.*Gamma.*Decision"),
                                   ("B2HH"           , "Hlt2B2HH.*Decision"),
                                   ("Express"        , "Hlt2Express.*Decision"),
                                   ("Commissioning"  , "Hlt2(PassThrough|Transparent|Forward|DebugEvent).*Decision"),
                                   ("DisplVertices"  , "Hlt2DisplVertices.*Decision"),
                                   ("HighPtJets"     , "Hlt2HighPtJets.*Decision"),
                                   ("Charm"          , "Hlt2Charm.*Decision"),
                                   ("Global"         , ".*Global.*"),
                                   ("Other"          , ".*") # add a 'catch all' term to pick up all remaining decisions...
                                 ]
                                 )

        # Configure vertex monitoring
        from HltTracking.HltVertexNames import Hlt3DPrimaryVerticesName as PV3DSelection
        from HltTracking.HltVertexNames import ( _vertexLocation,
                                                 HltSharedVerticesPrefix,
                                                 HltGlobalVertexLocation )                  
        from HltTracking.HltPVs import ProtoPV3DSelection
        pv3d  = _vertexLocation( HltSharedVerticesPrefix, HltGlobalVertexLocation, PV3DSelection )
        proto = _vertexLocation( HltSharedVerticesPrefix, HltGlobalVertexLocation, ProtoPV3DSelection )
        HltGlobalMonitor().VertexLocations = { 'PV3D'  : pv3d,
                                               'Proto' : proto }

        def _recurse(c,fun) :
            fun(c)
            for p in [ 'Members','Filter0','Filter1' ] :
                if not hasattr(c,p) : continue
                x = getattr(c,p)
                if list is not type(x) : x = [ x ]
                for i in x : _recurse(i,fun)

        def _enableMonitoring(c) :
            if c.getType() in ['FilterDesktop','CombineParticles' ] : 
                c.Monitor = True
                c.HistoProduce = True

        def _disableHistograms(c,filter = lambda x : True) :
            if 'HistoProduce' in c.getDefaultProperties() and filter(c):
                c.HistoProduce = False
            for p in [ 'Members','Filter0','Filter1' ] :
                if not hasattr(c,p) : continue
                x = getattr(c,p)
                if list is not type(x) : x = [ x ]
                for i in x : _disableHistograms(i,filter) 
        from HltLine.HltLine     import hlt1Lines, hlt2Lines
        if   self.getProp('HistogrammingLevel') == 'None' : 
            for i in hlt1Lines()+hlt2Lines() : _disableHistograms( i.configurable() )
        elif self.getProp('HistogrammingLevel') == 'Line' : 
            for i in hlt1Lines()+hlt2Lines() : _disableHistograms( i.configurable(), lambda x: x.getType()!='Hlt::Line' ) 
        elif self.getProp('HistogrammingLevel') == 'NotLine' : 
            for i in hlt1Lines()+hlt2Lines() : _disableHistograms( i.configurable(), lambda x: x.getType()=='Hlt::Line' )
        if self.getProp('EnableMonitoring') :
            for i in hlt1Lines()+hlt2Lines() : _recurse( i.configurable(),_enableMonitoring )


##################################################################################
    def postConfigAction(self) : 
        """
        Add the configured lines into the Hlt1 and Hlt2 sequencers,
        provided there is no reason not to do so...

        @todo remove this method
        """
        from HltLine.HltLine     import hlt1Lines
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

        ###
        # append additional lines # WARNING do that in DaVinci, not Moore!
        # @todo : Need to protect against that when making a TCK. Gerhard know how
        activeHlt1Lines.extend( self.getProp('AdditionalHlt1Lines')  )
        activeHlt2Lines.extend( self.getProp('AdditionalHlt2Lines')  )

        ### @FIXME / @TODO
        #  brute force addition of Hlt2PileUp -- needs to be done in an elegant fashion...
        #  problem is that it is the setting which drives this, but Hlt2.py which generates
        #  the Hlt2Line('PileUp',... ) -- so for now we rely on the fact that we happen
        #  to know what Hlt2.py does...
        if activeHlt2Lines and 'Hlt2PileUp' in [ i.name() for i in hlt2Lines() ] and 'Hlt2PileUp' not in activeHlt2Lines :
                activeHlt2Lines += [ 'Hlt2PileUp' ]

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


        print '# List of requested Hlt1Lines : %s ' % activeHlt1Lines 
        # print '# List of available Hlt1Lines : %s ' % [ i.name() for i in hlt1Lines() ] 
        awol1 = set( activeHlt1Lines ) - set( [ i.name() for i in hlt1Lines() ] )
        if awol1 : 
            log.fatal(' # some requested Hlt1 lines are absent : %s ' % awol1 )

        print '# List of requested Hlt2Lines : %s ' % activeHlt2Lines 
        # print '# List of available Hlt2Lines : %s ' % [ i.name() for i in hlt2Lines() ] 
        awol2 = set( activeHlt2Lines ) - set( [ i.name() for i in hlt2Lines() ] )
        if awol2 : 
            log.fatal(' # some requested Hlt2 lines are absent : %s ' % awol2 )

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
        for i in hlt1Lines() :
                i.configurable().AcceptIfSlow = self.getProp('EnableAcceptIfSlow') 
                i.configurable().FlagAsSlowThreshold = self.getProp('SlowHlt1Threshold')
        for i in hlt2Lines() :
                i.configurable().AcceptIfSlow =  self.getProp('EnableAcceptIfSlow')
                i.configurable().FlagAsSlowThreshold = self.getProp('SlowHlt2Threshold')
        
        self.configureHltMonitoring(lines1, lines2)
        self.configureVertexPersistence( [ i.name() for i in lines1 ] ) # TODO: add lines2...
        self.configureANNSelections()

        if self.getProp("Verbose") : print Sequence('Hlt') 
         

##################################################################################
#
# end sequence
#
    def endSequence(self):
        """
        define end sequence (mostly for persistence + monitoring)
        """
        from Configurables        import GaudiSequencer as Sequence
        from Configurables        import HltGlobalMonitor, HltL0GlobalMonitor
        from Configurables        import HltBeetleSyncMonitor
        from Configurables        import bankKiller
        from Configurables        import LoKi__HDRFilter   as HltFilter
        from Configurables        import HltSelReportsMaker, HltSelReportsWriter
        from Configurables        import HltDecReportsWriter
        from Configurables        import HltVertexReportsMaker, HltVertexReportsWriter
        from Configurables        import HltRoutingBitsWriter
        from Configurables        import HltLumiWriter
        from Configurables        import DeterministicPrescaler as Prescale
        from HltLine.HltDecodeRaw import DecodeIT, DecodeVELO, DecodeL0DU
        from Configurables        import LoKi__VoidFilter as Filter

        sets = self.settings()
        if sets and hasattr(sets, 'StripEndSequence') and getattr(sets,'StripEndSequence'):
            log.warning('### Setting requests stripped down HltEndSequence ###')
            strip = getattr(sets,'StripEndSequence')
            #  TODO: check not explicitly set if so, provide warning....
            self.EnableHltGlobalMonitor   = ( 'EnableHltGlobalMonitor'   in strip )
            self.EnableHltL0GlobalMonitor = ( 'EnableHltL0GlobalMonitor' in strip )
            self.EnableBeetleSyncMonitor  = ( 'EnableBeetleSyncMonitor'  in strip )
            self.EnableHltSelReports      = ( 'EnableHltSelReports'      in strip )
            self.EnableHltVtxReports      = ( 'EnableHltVtxReports'      in strip )
            self.EnableLumiEventWriting   = ( 'EnableLumiEventWriting'   in strip )

        # Setup the beetle sync sequence
        BeetleMonitorAccept = Sequence( 'BeetleSyncMonitorAcceptSequence' )
        BeetleMonitorAccept.ModeOR = True
        BeetleMonitor = Sequence( 'BeetleSyncMonitorSequence' )
        BeetleMonitor.Members = [ Filter( 'BeetleSyncScaler',
                                          Code = 'RATE(%d)' % self.getProp('BeetleSyncMonitorRate') ) ]
        BeetleMonitor.Members += DecodeL0DU.members() + DecodeVELO.members() \
                                 + DecodeIT.members() + [ HltBeetleSyncMonitor() ]
        BeetleMonitorAccept.Members = [ BeetleMonitor, Filter( 'ForceAccept', Code = 'FALL' ) ]
        # note: the following is a list and not a dict, as we depend on the
        # order of iterating through it!!!
        from Configurables import Hlt__Line as Line
        _list = ( ( "EnableHltRoutingBits",     [ HltRoutingBitsWriter ] )
                , ( "EnableHltGlobalMonitor",   [ HltGlobalMonitor ] )
                , ( "EnableHltL0GlobalMonitor", [ HltL0GlobalMonitor ] )
                , ( "EnableBeetleSyncMonitor",  [ lambda : BeetleMonitorAccept ] )
                , ( "SkipHltRawBankOnRejectedEvents", [ lambda : Line('Hlt1Global') ] )
                # , ( "SkipHltRawBankOnRejectedEvents", [ lambda : 'Hlt1Global' ] ) # TODO: fwd Moore.WriterRequires (which is a list...)
                , ( "EnableHltDecReports"    , [ HltDecReportsWriter ] )
                , ( "EnableHltSelReports"    , [ HltSelReportsMaker, HltSelReportsWriter ] )
                , ( "EnableHltVtxReports"    , [ HltVertexReportsMaker, HltVertexReportsWriter ] )
                )
        
        End = Sequence( 'HltEndSequence' )
        EndMembers = End.Members
        # make sure we only instantiate if we actually use it...
        for i in [ v for (k,v) in _list if self.getProp(k) ] :
            EndMembers += [ c() for c in i ]
        if (self.getProp("EnableLumiEventWriting")) :
            if sets and hasattr(sets, 'NanoBanks') :
                if not self.isPropertySet('NanoBanks') :
                    self.setProp('NanoBanks',sets.NanoBanks)
                else :
                    log.warning('Setting %s requested NanoBanks = %s, but also explicitly set; using %s.' % (sets.HltType(), sets.NanoBanks, self.getProp('NanoBanks')))
            if self.isPropertySet('NanoBanks') :
                    log.warning('Using non-default NanoBanks = %s.' % (self.getProp('NanoBanks')))
                
            EndMembers += [ HltLumiWriter()
                          , Sequence( 'LumiStripper' , Members = 
                                      [ HltFilter('LumiStripperFilter' , Code = self.getProp('LumiBankKillerPredicate')  ) 
                                      , Prescale('LumiStripperPrescaler',AcceptFraction=self.getProp('LumiBankKillerAcceptFraction')) 
                                      , bankKiller( BankTypes=self.getProp('NanoBanks'),  DefaultIsKill=True )
                                      ])
                          ] 
        if self.getProp( 'RequireL0ForEndSequence') :
            from Configurables import LoKi__L0Filter    as L0Filter
            from HltLine.HltDecodeRaw import DecodeL0DU
            L0accept = Sequence(name='HltEndSequenceFilter', Members = DecodeL0DU.members() + [ L0Filter( 'L0Pass', Code = "L0_DECISION_PHYSICS" )])
            EndMembers.insert(1,  L0accept )  # after routing bits

        ### store the BDT response (and a bit more) through ExtraInfo on particles:
        HltSelReportsMaker().InfoLevelDecision = 3
        HltSelReportsMaker().InfoLevelTrack = 3
        HltSelReportsMaker().InfoLevelRecVertex = 3
        HltSelReportsMaker().InfoLevelCaloCluster = 3
        HltSelReportsMaker().InfoLevelParticle = 3

##################################################################################
    def __apply_configuration__(self):
        """
        Apply Hlt configuration
        """
        log.debug("Hlt configuration")
        import GaudiKernel.ProcessJobOptions
        GaudiKernel.ProcessJobOptions.PrintOff()
        import HltConf.HltInit  # make sure ANN numbers are assigned...
        
        self.confType()
        self.endSequence()
        self.configureRoutingBits()

        #appendPostConfigAction( self.postConfigAction )
        GaudiKernel.Configurable.postConfigActions.insert( 0,  self.postConfigAction )
