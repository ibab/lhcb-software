"""
Configurable to configure HLT Output

This configurable configures how events are output from Moore. Events are classified
into logical streams using regexes that operate on HLT selection decisions. Examples are
the Full, Turbo and TurCal streams.

The final output happens in per groups of streams, which consist of one or more logical streams,
where the same set of raw banks is kept for the entire group.

Lumi events in a group of streams are nanofied only if none of component streams selected the event.

@author R. Aaij
@date 16-03-2015
"""
# =============================================================================
__author__  = "R. Aaij roel.aaij@cern.ch"
# =============================================================================
from itertools import chain
from copy import copy, deepcopy
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer as Sequence
from GaudiConf import IOExtension, IOHelper

class HltOutputConf(LHCbConfigurableUser):
    __used_configurables__ = []
    __queried_configurables__ = []

    ## This is the per-stream bit that indicates not-lumi-exclusive
    __not_exclusive_lumi___ = 95
    ## This is the name of the filter that applies to all output streams
    __writer_filter_name__ = 'WriterFilter'

    ## Streams we know about: {name : rb}
    __streams__ = {"VELOCLOSING" : 40,
                   "TURBOFULL"   : 85,
                   "BEAMGAS"     : 86,
                   "FULL"        : 87,
                   "TURBO"       : 88,
                   "PARKED"      : 89,
                   "TURCAL"      : 90,
                   "NOBIAS"      : 91,
                   "SMOGPHYS"    : 93,
                   "LUMI"        : 94}

    __slots__ = {'OutputFile'             : '',
                 'Split'                  : '',
                 'UseTCK'                 : False,
                 'RunOnline'              : False,
                 'GenerateTCK'            : False,
                 'Simulation'             : False,
                 'WriterRequires'         : ['HltDecisionSequence'],
                 'WriteFSR'               : True,
                 'EnableOutputStreaming'  : False,
                 'StreamsWithBanks'       : [(["FULL"],      []),
                                             (["BEAMGAS"],   []),
                                             (["TURCAL"],    []),
                                             (["PARKED"],    []),
                                             (["NOBIAS"],    []),
                                             (["SMOGPHYS"],  []),
                                             (["TURBOFULL"], []),
                                             (["LUMI"], ['ODIN', 'HltLumiSummary', 'HltRoutingBits', 'DAQ']),
                                             (["TURBO"],['ODIN', 'HltLumiSummary', 'DAQ',
                                                         'HltRoutingBits', 'HltDecReports', 'HltSelReports', 'HltVertexReports', 'HltTrackReports',
                                                         'L0DU', 'L0Calo', 'L0PU', 'L0PUFull', 'L0Muon', 'L0CaloFull',
                                                         'L0MuonCtrlAll', 'L0MuonProcCand', 'L0MuonProcData', 'L0MuonCtrlAll'])],
                 'EnabledStreams'         : {"LUMI" : None,
                                             "BEAMGAS" : None,
                                             "FULL" : None,
                                             "TURBO" : None,
                                             "TURBOFULL" : None,
                                             "TURCAL" : None,
                                             "VELOCLOSING" : None},
                 'StreamsWithLumi'        : ['FULL', 'TURCAL', 'TURBO', 'TURBOFULL', 'NOBIAS', 'SMOGPHYS', 'PARKED'],
                 'OutputAlgProperties'    : None,
                 'HltEndSequence'         : None,
                 'LumiPredicate'          : "HLT_PASS('Hlt2LumiDecision')",
                 'EnableLumiEventWriting' : True,
                 'NotLumiPredicate'       : "HLT_PASS_RE('Hlt2(?!Lumi).*Decision')",
                 'NanoBanks'              : ['ODIN', 'HltLumiSummary', 'HltRoutingBits', 'DAQ'],
                 'LumiBankKillerAcceptFraction' : 0.9999, # fraction of lumi-only events where raw event is stripped down
                                                          # (only matters if EnablelumiEventWriting = True)
                 'Hlt2LinesForDQ'         : ['PIDD02KPiTagTurboCalib', 'PIDLambda2PPiLLTurboCalib', 'PIDDetJPsiMuMuPosTaggedTurboCalib',
                                             'PIDDetJPsiMuMuNegTaggedTurboCalib', 'PIDLambda2PPiLLhighPTTurboCalib', 'PIDLambda2PPiLLveryhighPTTurboCalib',
                                             'DiMuonDetachedJPsi', 'Hlt2PIDDs2PiPhiKKNegTaggedTurboCalib', 'Hlt2PIDDs2PiPhiKKPosTaggedTurboCalib',
                                             'Hlt2PIDDs2PiPhiKKUnbiasedTurboCalib'],
                 '_streams'               : None # Cache property
                 }

    _propertyDocDct={'EnableOutputStreaming' : "Treat raw event differently for different sets of streams"}

    def knownStreams(self):
        return HltOutputConf.__streams__

    def __rawLocationPattern(self):
        return 'DAQ/%sRawEvent'

    def __streamFilterName(self, stream):
        return stream + 'Hlt2Filter'

    def __writerFilter(self, requirement):
        from Configurables import LoKi__VoidFilter as VoidFilter
        if requirement:
            code = ' & '.join(["ALG_EXECUTED('%s') & ALG_PASSED('%s')" % (i, i) for i in requirement])
        else:
            code = "LoKi.Constant('void', 'bool')(True)"
        return VoidFilter(HltOutputConf.__writer_filter_name__, Code = code,
                          Preambulo = ['from LoKiHlt.algorithms import ALG_EXECUTED, ALG_PASSED'])

    def __checkRoutingBits(self, routingBits):
        def __check(expr, stage):
            stages = ['ODIN', 'L0', 'Hlt1', 'Hlt2']
            stages.remove(stage)
            checks = ["'" + s for s in stages]
            for s in checks:
                if s in expr:
                    return False
            return True

        for bit, expression in routingBits.iteritems():
            for m, s in ((8, 'ODIN'), (32, 'L0'), (64, 'Hlt1'), (96, 'Hlt2')):
                if bit < m:
                    if not __check(expression, s):
                        raise ValueError("Routing bit %d: %s contains an expression that it shouldn't,"
                                         " please check!" % (bit, expression))
                    break

    def __addLumi(self, streams, expr):
        if any(map(lambda s: s.upper() in self.getProp('StreamsWithLumi'), streams)):
            return '( %s ) | ( %s )' % (expr, self.getProp('LumiPredicate'))
        else:
            return expr

    def __addLumiToStreamBits(self, routingBits, streamBits):
        # Add lumi to those streams that require it
        for sub, (bit, expr) in streamBits.iteritems():
            routingBits[bit] = self.__addLumi([sub], expr)

    def __outputFilename(self, stream):
        filename = self.getProp('OutputFile')
        if not filename:
            raise ValueError('An output filename has been requested, so OutputFile must be set!')

        if not stream:
            return filename
        else:
            base, ext = os.path.splitext(filename)
            return base + '_' + stream + ext

    def __MDFWriterProperties(self):
        filename = self.getProp('OutputFile')
        if not filename:
            raise ValueError("No filename set, but still trying to build an output writer!")
        gProps = {'Connection'   : "file://{0}%s{1}".format(*os.path.splitext(filename))}
        sProps = {'BankLocation' : self.__rawLocationPattern()}
        return  'LHCb::MDFWriter', {'Compress' : str(0)}, gProps, sProps

    def __lumiPredicate(self, exclude, exclusive):
        # Given a predicate for exclusion, return a predicate that indicates whether an event is:
        # either:
        # -  exclusively a lumi event, i.e. only lumi triggers fired on this event
        # or:
        # - not exclusively a lumi event, i.e. something else triggered.
        # The expressions used are each other's inverse, because the precondition that at least
        # a trigger fired holds
        if exclusive:
            return None, "( %s ) & ~( %s )" % (self.getProp('LumiPredicate'), exclude)
        else:
            return HltOutputConf.__not_exclusive_lumi___, exclude

    def __nanoBanks(self, stream):
        nanoBanks = self.getProp('NanoBanks')
        if isinstance(nanoBanks, dict):
            # If dict, either nanobanks for all streams must be given or a default
            if stream.upper() in nanoBanks:
                nanoBanks = nanoBanks[stream.upper()]
            else:
                nanoBanks = nanoBanks['DEFAULT']
        return nanoBanks

    def __lumiAlgs(self, stream, exclude, rawLocation = None):
        # Return the list of algorithms that are required to treat lumi events

        # Don't nanofy after HLT1, of if there is nothing base nanofication on
        if self.getProp('Split') == 'Hlt1' or not exclude:
            return []

        from DAQSys.Decoders import DecoderDB
        from Configurables   import bankKiller
        from Configurables   import DeterministicPrescaler as Prescale
        from Configurables   import LoKi__HDRFilter   as HltFilter

        # Strip exclusive lumi events
        decoder = DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"]
        prefix = stream + 'Lumi' if stream != 'Lumi' else stream + 'Stream'
        stripper = Sequence(prefix + 'Stripper')

        if exclude != self.getProp('LumiPredicate'):
            # Filter on which events to nanofy for all streams, except Lumi,
            # where everything is nanofied
            stripper.Members += [HltFilter(prefix + 'Hlt2StripperFilter',
                                            Code = self.__lumiPredicate(exclude, exclusive = True)[1],
                                            Location = decoder.listOutputs()[0])]
        else:
            # Keep a small fraction of non-nanofied events in the Lumi stream.
            stripper.Members += [Prescale('LumiStripperPrescaler', AcceptFraction = self.getProp('LumiBankKillerAcceptFraction'))]

        # The actual nanofier
        killer = bankKiller(prefix + 'StripperBankKiller', BankTypes = self.__nanoBanks(stream), DefaultIsKill = True)
        if rawLocation:
            killer.RawEventLocations = [rawLocation]
        stripper.Members += [killer]

        return [stripper]

    def __hlt2RoutingBitsWriter(self, stream, routingBits = None, rawLocation = None):
        # Create RoutingBitsWriter instance for HLT2
        from Configurables import HltRoutingBitsWriter
        from DAQSys.Decoders import DecoderDB
        hlt1_decrep_loc = DecoderDB["HltDecReportsDecoder/Hlt1DecReportsDecoder"].listOutputs()[0]
        hlt2_decrep_loc = DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"].listOutputs()[0]

        rbWriterName = stream + 'Hlt2RoutingBitsWriter'
        rbWriter = HltRoutingBitsWriter(rbWriterName)
        rbWriter.Hlt1DecReportsLocation = hlt1_decrep_loc
        rbWriter.Hlt2DecReportsLocation = hlt2_decrep_loc
        rbWriter.UpdateExistingRawBank = True

        # Configure private HltFactory of RoutingBitWriter
        from Configurables import LoKi__Hybrid__HltFactory as HltFactory
        HltFactory(rbWriterName + '.LoKi::Hybrid::HltFactory').Modules += [ 'LoKiCore.functions', 'LoKiNumbers.sources' ]

        if routingBits == None:
            return rbWriter

        self.__checkRoutingBits(routingBits)
        rbWriter.RoutingBits = routingBits

        # If the rawLocation is not set, the default value is used
        if rawLocation:
            rbWriter.RawEventLocation = rawLocation

        return rbWriter

    def __rawEventAlgs(self, stream, banks, lumiExclude, routingBits = None, rawLocation = None):
        # Return the list of algorithms that are required to correctly output an
        # event to a stream

        from Configurables import LHCb__RawEventCopy as RawEventCopy
        from Configurables import LoKi__VoidFilter as VoidFilter
        from Configurables import HltRoutingBitsWriter
        from Configurables import bankKiller

        rawAlgs = []

        # Copy RawEvent if needed
        if rawLocation:
            copier = RawEventCopy(stream + "RawEventCopy", Destination = '/Event/' + rawLocation,
                                  DeepCopy = True)
            rawAlgs += [copier]

        # Write RoutingBits for this stream
        if routingBits:
            rawAlgs += [self.__hlt2RoutingBitsWriter(stream, routingBits, rawLocation)]

        # Strip banks if needed
        if banks and banks != self.__nanoBanks(stream):
            nbs = set(self.__nanoBanks(stream))
            if (stream.upper() in self.getProp('StreamsWithLumi') and self.getProp('EnableLumiEventWriting')
                and nbs.intersection(set(banks)) != nbs):
                raise ValueError("Lumi banks are being stripped by the raw event stripper, while this stream"
                                 " needs lumi events")
            stripper = bankKiller(stream + 'Stripper', BankTypes = banks, DefaultIsKill = True)
            if rawLocation:
                stripper.RawEventLocations = [rawLocation]
            rawAlgs += [stripper]

        # Lumi writing
        if self.getProp("EnableLumiEventWriting"):
            rawAlgs += self.__lumiAlgs(stream, lumiExclude, rawLocation)

        return rawAlgs

    def __configureRoutingBits(self, streams):
        """
        Configuration of HLT Routing bits, there are 96 bits in total.
        These bits are used in the DAQ to route events.
         0- 7 : reserved for ODIN
         8-31: reserved for L0
        32-63: reserved for Hlt1
        64-96: reserved for Hlt2

        NOTE: All bits are defined in an EDMS document:
              https://edms.cern.ch/document/1146861/1.3
        NOTE: any change in the _meaning_ of any of the routing bits needs to be
              communicated with online, alignment and offline, to ensure the events
              are still properly routed!!!
        """

        from Hlt1Lines.HltL0Candidates import L0Channels

        from Configurables import HltRoutingBitsWriter
        # note: 'Decision' at the end is needed to _exclude_Hlt1Global

                      # RB 33 Lumi after HLT1
        routingBits = { 33 : "HLT_PASS_RE('^Hlt1Lumi.*Decision$')"
                      # RB 35 Beam-Gas for Velo alignment
                      , 35 : "HLT_PASS_RE('Hlt1BeamGas.*Decision')"
                      # RB 40 Velo (closing) monitoring
                      , 40 : "HLT_PASS_RE('Hlt1(Velo|BeamGas).*Decision')"
                      # RB 46 HLT1 physics for monitoring and alignment
                      , 46 : "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
                      # RB 48 NoBias, prescaled
                      , 48 : "HLT_PASS('Hlt1MBNoBiasDecision')"
                      # RB 53 Tracker alignment
                      , 53 : "HLT_PASS_RE('Hlt1Calib(TrackingKPiDetached|HighPTLowMultTrks)Decision')"
                      # RB 54 RICH mirror alignment
                      , 54 : "HLT_PASS_RE('Hlt1CalibRICH.*Decision')"
                      # RB 55 NoBias, rate-limited
                      , 55 : "HLT_PASS('Hlt1MBNoBiasRateLimitedDecision')"
                      # RB 56 Muon alignment
                      , 56 : "HLT_PASS('Hlt1CalibMuonAlignJpsiDecision')"
                      # RB 57 Tell1 Error events
                      , 57 : "HLT_PASS('Hlt1Tell1ErrorDecision')"
                      # 64--96: Hlt2
                      # RB 85 Turbo stream with full raw event
                      , 85 : "HLT_TURBOPASS_RE('^Hlt2.*(?!TurboCalib)Decision$')"
                      # RB 86 BEAMGAS stream
                      , 86 : "HLT_PASS_SUBSTR('Hlt2BeamGas')"
                      # RB 87 for the full (non-turbo(calib)) stream
                      , 87 : "HLT_NONTURBOPASS_RE('Hlt2(?!BeamGas)(?!Lumi).*Decision')"
                      # RB 88 for Turbo stream, includes lumi events.
                      # this now excludes turbocalib events which have their own stream/routing bit
                      , 88 : "HLT_TURBOPASS_RE('^Hlt2.*(?!TurboCalib)Decision$')"
                      # RB 89 for the parked stream; reserved but not set for now
                      # RB 90 for TurboCalib stream, includes lumi events.
                      , 90 : "HLT_TURBOPASS_RE('^Hlt2.*TurboCalibDecision$')"
                      # RB 91 for the NOBIAS stream
                      , 91 : "HLT_PASS('Hlt2MBNoBiasDecision')"
                      # RB 92 for online DQ on HLT2 output
                      , 92 : "HLT_PASS('Hlt2(%s)Decision')" % '|'.join(self.getProp("Hlt2LinesForDQ"))
                      # RB 93 for SMOG physics; reserved
                      # RB 94 LUMI stream
                      , 94 : self.getProp('LumiPredicate')
                      # RB 95 Reserved for not-lumi-exclusive, set stream-by-stream
                      }

        ## Update routing bits in case of non-standard expressions
        for stream, (bit, expr, on) in streams.items():
            if expr == None and on:
                if bit not in routingBits:
                    log.fatal("Attempt to enable %s stream with the default expression, which is not defined." % stream)
                    raise RuntimeError
            elif on:
                log.warning( 'Setting non standard routing bit expression for %s stream' % stream)
                log.warning( 'Now:     %s' % expr)
                log.warning( 'Default: %s' % routingBits.get(bit, "Disabled"))
                log.warning( '##########################################################' )
                routingBits[bit] = expr
            elif bit in routingBits:
                # Stream is disabled, remove the preset bit
                routingBits.pop(bit)

        ## Record the settings in the ANN service
        from Configurables       import HltANNSvc
        if self.getProp("Split") == "Hlt1":
            HltANNSvc().RoutingBits = {v : k for k, v in routingBits.iteritems() if k < 64}
        else :
            HltANNSvc().RoutingBits = {v : k for k,v in routingBits.iteritems()}

        # LoKi::Hybrid::HltFactory is what RoutingBitsWriter uses as predicate factory..
        # make sure 'strings' is known...
        # make sure 'RATE,SCALE and SKIP' are known...
        from Configurables import LoKi__Hybrid__HltFactory as HltFactory
        HltFactory('ToolSvc.LoKi::Hybrid::HltFactory').Modules += [ 'LoKiCore.functions', 'LoKiNumbers.sources' ]
        #  forward compatibility: HltFactory will become private for HltRoutingBitsWriter...

        # and, last but not least, tell the writers what they should write..
        HltRoutingBitsWriter('Hlt1RoutingBitsWriter').RoutingBits = {k : v for k, v in routingBits.iteritems() if k < 64}
        HltFactory('Hlt1RoutingBitsWriter.LoKi::Hybrid::HltFactory').Modules += [ 'LoKiCore.functions', 'LoKiNumbers.sources' ]

        return routingBits

    def __outputStreams(self, streams):
        outputStreams = {}

        # Build output streams
        for substreams, banks in self.getProp('StreamsWithBanks'):
            # If no substreamBits are left, this stream is disabled altogether
            subs = sorted(filter(lambda e: streams[e.capitalize()][2], substreams))
            if not subs:
                continue
            outputStreams[subs[0].capitalize()] = ([s.capitalize() for s in subs], banks)
        return outputStreams

    def __configureStreams(self):
        if self.isPropertySet('_streams'):
            return self.getProp('_streams')

        ## Stream configuration
        knownStreams = self.knownStreams()

        ## Figure out which streams we really want to enable
        from Configurables import HltConf
        sets = HltConf().settings()
        ep = 'EnabledStreams'
        enabledStreams = self.getProp(ep)
        defaultEnabled = self.getDefaultProperty(ep)
        if self.isPropertySet(ep) and enabledStreams != defaultEnabled:
            log.warning( '##########################################################')
            log.warning( 'Non-default stream configuration specified from EnabledStreams property')
            log.warning( 'This will overwrite any other setting of the streams, for example from the settings.')
        elif sets and hasattr(sets, 'Streams') and set(sets.Streams()) != set(self.getDefaultProperty(ep)):
            log.warning( '##########################################################')
            log.warning( 'Non-default stream configuration specified in Settings')
            enabledStreams = sets.Streams()

        streams = {s : (bit, None, False) for s, bit in knownStreams.iteritems()}
        for stream, expr in enabledStreams.iteritems():
            if stream not in streams:
                log.fatal("Attempt to enable a non-existent stream %s." % stream)
                raise RuntimeError
            if stream not in defaultEnabled:
                log.warning("Enabling non-default enabled stream %s." % stream)
            bit = streams[stream][0]
            streams[stream] = (bit, expr, True)

        ## Check if default enabled streams are on
        disabled = [s[0] for s in streams.iteritems() if not s[1][2] and s[0] in defaultEnabled]
        if disabled:
            log.warning( '##########################################################')
            log.warning( 'Normally enabled streams are disabled: %s.' % ', '.join(disabled) )
            log.warning( '##########################################################')

        enabled = set([k for (k, (_, _, on)) in streams.iteritems() if on])
        if  enabled != set(defaultEnabled.keys()):
            log.warning( '##########################################################' )
            log.warning( 'Non-standard stream configuration' )
            log.warning( 'Enabled:  %s' % ', '.join(s[0] for s in streams.iteritems() if s[1][2]) )
            log.warning( 'Disabled: %s' % ', '.join(s[0] for s in streams.iteritems() if not s[1][2]) )
            log.warning( '##########################################################' )

        streams = {k.capitalize() : v for k, v in streams.iteritems()}
        self.setProp('_streams', streams)
        return streams

    def __addNotExclusiveLumiBit(self, routingBits, predicate):
        bit, pred = self.__lumiPredicate(predicate, exclusive = False)
        routingBits[bit] = pred

    def __configureEndSequence(self, streams, routingBits):
        from Configurables import HltRoutingBitsWriter
        from Configurables import LoKi__HDRFilter as HltFilter

        filename = self.getProp('OutputFile')
        # If there is no output file and we're not running online, or generating a TCK:
        # don't do anything
        if not (filename or self.getProp('RunOnline') or self.getProp('GenerateTCK')):
            return

        # This is where we plug everything in
        EndSequence = self.getProp("HltEndSequence")

        # Filter on writerRequires, if it's empty, the filter will be there, but
        # pass all
        writerRequires = self.getProp('WriterRequires')
        from Configurables import LoKi__VoidFilter as VoidFilter
        EndSequence.Members += [self.__writerFilter(writerRequires)]

        # If split is both or HLT1, add the lumi writer to the end sequence.
        if self.getProp("EnableLumiEventWriting") and self.getProp('Split') != 'Hlt2':
            from Configurables   import HltLumiWriter
            EndSequence.Members += [HltLumiWriter()]

        # Routing bits per substream
        bitsByStream = {sub : (bit, routingBits[bit]) for sub, (bit, _, on) in streams.iteritems() if on}

        # Configure the output streams
        if self.getProp('EnableOutputStreaming') and self.getProp('Split') != 'Hlt1':
            # Case of separate streams
            # Create a "top-level" stream in OR mode without shortcircuiting to
            # ensure all streams always run
            streamsSequence = Sequence("HltOutputStreamsSequence", ModeOR = True, ShortCircuit = False)
            EndSequence.Members += [streamsSequence]

            # Location of HLT2 DecReports
            from DAQSys.Decoders import DecoderDB
            decoder = DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"]

            # Build output streams
            for streamName, (subs, banks) in self.__outputStreams(streams).iteritems():
                # Update with stream specific routing bits
                substreamBits = {sub : bitsByStream[sub] for sub in subs}

                # Filter to select events for this stream
                expressions = sorted([v[1] for v in substreamBits.values()])
                if len(expressions) > 1:
                    streamPredicate = " | ".join(['( %s )' % s for s in expressions])
                else:
                    streamPredicate = expressions[0]

                # The output stream sequence, with it's HltFilter
                streamSequence = Sequence(streamName + "OutputSequence")
                streamFilter = HltFilter(self.__streamFilterName(streamName),
                                         Code = self.__addLumi(subs, streamPredicate),
                                         Location = decoder.listOutputs()[0])
                streamSequence.Members = [streamFilter]

                # This is the location for the raw event copy of this stream
                rawLocation = self.__rawLocationPattern() % streamName
                # Sequence for the output algorithms
                prepSequence = Sequence(streamName + "RawEventSequence", ModeOR = True, ShortCircuit = False)
                streamSequence.Members += [prepSequence]

                # Always pass the HLT1 routing bits
                rbs = {k : routingBits[k] for k in routingBits.iterkeys() if k < 64}
                self.__addLumiToStreamBits(rbs, substreamBits)

                # Make the expression for not-lumi-exclusive and add it to the routing bits writer
                self.__addNotExclusiveLumiBit(rbs, streamPredicate)
                prepSequence.Members = self.__rawEventAlgs(streamName, banks, streamPredicate, rbs, rawLocation)

                # Add the output sequence to the streams sequence
                streamsSequence.Members += [streamSequence]
            # We add the routing bits writer for the original raw event so they
            # are written there too. This is needed for running in flagging mode
            # with a single output stream.
            self.__addLumiToStreamBits(routingBits, bitsByStream)
            self.__addNotExclusiveLumiBit(routingBits, self.getProp('NotLumiPredicate'))
            streamsSequence.Members += self.__rawEventAlgs('', [], '', routingBits)
        else:
            # Case of single output stream
            rbs = None
            if self.getProp("Split") != "Hlt1":
                # Hlt2 routing bits make no sense when running only Hlt1
                rbs = deepcopy(routingBits)
                # not-exclusive lumi bit
                self.__addNotExclusiveLumiBit(rbs, self.getProp('NotLumiPredicate'))
                # Update routing bits that require lumi
                self.__addLumiToStreamBits(rbs, bitsByStream)
            # Plug the single global instance directly into the end sequence
            rawAlgs = self.__rawEventAlgs('', [], self.getProp('NotLumiPredicate'), rbs)
            prepSequence = Sequence("RawEventSequence", ModeOR = True, ShortCircuit = False,
                                    Members = rawAlgs)
            EndSequence.Members += [prepSequence]

    def __routingBitsForFlaggingMode(self):
        # The EndSequence always runs and the HLT2 routing bits writer needs the
        # HLT2 DecReports, so we need to check that HLT2 at least ran.
        from Configurables import LoKi__VoidFilter as VoidFilter
        hlt2Filter = VoidFilter("RoutingBitsHlt2Filter",
                                 Preambulo = ['from LoKiHlt.algorithms import ALG_EXECUTED'],
                                 Code = "ALG_EXECUTED('Hlt2')")
        rbWriter = self.__hlt2RoutingBitsWriter('')
        return GaudiSequencer("Hlt2RoutingBitsSequence", Members = [hlt2Filter, rbWriter])

    def __configureInputCopy(self, filename, writerRequires):
        if self.getProp('EnableOutputStreaming'):
            raise AttributeError("OutputStreaming doesn't work with a DST file as output, only with MDF.")
        from Configurables import InputCopyStream
        writer = InputCopyStream("Writer", RequireAlgs = writerRequires)
        IOHelper(None, None).outStream(filename, writer, writeFSR = self.getProp('WriteFSR'))

        if not bool(writerRequires):
            # If Moore is running in flagging mode, the RoutingBitsWriter
            # should be added here to write routing bits for events that
            # didn't pass.
            ApplicationMgr().OutStream = [self.__routingBitsForFlaggingMode()]
        ApplicationMgr().OutStream += [writer]

    def __configureHltOutputSequence(self, filename, writerRequires):
        from Configurables import HltOutputSvc, HltOutputSequence
        outputSvc = HltOutputSvc()
        ApplicationMgr().ExtSvc += [outputSvc.getFullName()]

        # Configure HltOutputSvc
        if not self.getProp('UseTCK'):
            streamGroups = {}
            filters = {}
            writerFilter = HltOutputConf.__writer_filter_name__
            if self.getProp('Split') == 'Hlt1' or not self.getProp('EnableOutputStreaming'):
                # Configure HltOutputSvc with single stream
                streamGroups[''] = ['']
                filters[''] = [writerFilter]
            elif self.getProp('EnableOutputStreaming'):
                # Multiple stream case, not HLT1, not using TCK and using output
                # streams. This is used when generating a TCK for HLT2, so the
                # HltOutputSvc should be configured so it can go into the TCK
                allStreams = self.__configureStreams()
                streamGroups = {k : v[0] for k, v in self.__outputStreams(allStreams).iteritems()}
                for substream in chain.from_iterable(streamGroups.values()):
                    filters[substream] = [writerFilter, self.__streamFilterName(substream)]

            outputSvc.StreamGroups = streamGroups
            outputSvc.Filters = filters

        if not bool(writerRequires):
            # If Moore is running in flagging mode, the RoutingBitsWriter
            # should be added here to write routing bits for events that
            # didn't pass.
            ApplicationMgr().OutStream = [self.__routingBitsForFlaggingMode()]

        if filename or self.getProp('RunOnline'):
            # Configure HltOutputStream
            outStream = HltOutputSequence()
            writerType, iProps, gProps, sProps = self.getProp("OutputAlgProperties")()
            outStream.OutputPerGroup = self.getProp('RunOnline')
            outStream.WriterType = writerType
            outStream.WriterProperties = iProps
            outStream.GroupProperties = gProps
            outStream.StreamProperties = sProps
            outStream.IgnoreFilters = (not bool(self.getProp('WriterRequires')))
            if self.getProp('Simulation') and not self.getProp('EnableOutputStreaming'):
                outStream.ForceStreams = {'' : [self.__writerFilterName()]}
            ApplicationMgr().OutStream += [outStream]


    def __configureOutStream(self):
        # Without filename, or running online, there is nothing to do
        filename = self.getProp('OutputFile')
        if not (filename or self.getProp('RunOnline') or self.getProp('GenerateTCK')):
            return

        writerRequires = self.getProp('WriterRequires')

        # Check the file type and use if it's InputCopyStream, configure a single InputCopyStream
        iox = IOExtension()
        if filename and iox.detectFileType(filename) != 'MDF':
            self.__configureInputCopy(filename, writerRequires)
        else:
            self.__configureHltOutputSequence(filename, writerRequires)

    def __propFromSettings(self, settings, prop):
        if settings and hasattr(settings, prop):
            if not self.isPropertySet(prop):
                self.setProp(prop, getattr(settings, prop)())
            else :
                args = (settings.HltType(), prop, getattr(settings, prop)(), self.getProp(prop))
                log.warning('Setting {0} requested {1} = {2}, but also explicitly set; using {3}.'.format(*args))
        if self.isPropertySet(prop):
            log.warning('Using non-default {0} = {1}.'.format(self.getProp(prop)))

    def __apply_configuration__(self):
        if not self.isPropertySet("OutputAlgProperties"):
            self.OutputAlgProperties = self.__MDFWriterProperties

        # Configure ApplicationMgr().Outstream first, because the rest depends
        # on ThresholdSettings
        self.__configureOutStream()

        # When running online from TCK, this is all that is needed
        if self.getProp('UseTCK'):
            return

        # Do we want to write lumi events and strip them?
        from Configurables import HltConf
        sets = HltConf().settings()
        if self.getProp("EnableLumiEventWriting"):
            self.__propFromSettings(sets, 'NanoBanks')

        # Check assignment of streams to output streams from Settings
        self.__propFromSettings(sets, 'StreamsWithBanks')

        streams = [c for c in chain.from_iterable([e[0] for e in self.getProp('StreamsWithBanks')])]
        if len(set(streams)) != len(streams):
            raise ValueError('Streams can be part of only one group in StreamsWithBanks')

        streams = self.__configureStreams()
        # Configure the routing bits and update the streams
        routingBits = self.__configureRoutingBits(streams)
        self.__configureEndSequence(streams, routingBits)
