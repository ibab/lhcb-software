##################################################################
# Data structure to hold a particles selection for the alignment.
# This still needs to be moved to a better location.
##################################################################

class ParticleSelection:
    __slots__ = {
        "location" : "" # particle location in the TES
        ,"algorithm" : None
        }

##################################################################
# Helper function to create a sequence to fit the tracks and run the hitadder
##################################################################
def configuredFitAndHitAdderSequence( Sequence, InputLocation, OutputLocation):
    from TrackFitter.ConfiguredFitters import ConfiguredEventFitter
    from Configurables import (TrackHitAdder, TrackContainerCopy, 
                               TrackSelector, GaudiSequencer)
    # create the sequence
    if isinstance(Sequence,GaudiSequencer) :
        seq = Sequence
    else:
        seq = GaudiSequencer(Sequence)
    # I am lazy: use the DOD to get the decoded clusters
    #importOption( "$STDOPTS/DecodeRawEvent.py" )
    # now setup the fitters
    seq.Members += [ 
        ConfiguredEventFitter('FitBeforeHitAdder',TracksInContainer = InputLocation),
        TrackHitAdder( TrackLocation = InputLocation ),
        ConfiguredEventFitter('FitAfterHitAdder',TracksInContainer = InputLocation)]
    tracksel =  TrackContainerCopy('CopyAndSelect',
                                   inputLocation = InputLocation,
                                   outputLocation = OutputLocation,
                                   Selector = TrackSelector())
    # also apply a modest selection
    tracksel.Selector.MaxChi2Cut = 5
    tracksel.Selector.MaxChi2PerDoFMatch = 5
    tracksel.Selector.MaxChi2PerDoFVelo = 5
    tracksel.Selector.MaxChi2PerDoFDownstream = 5
    seq.Members.append( tracksel )

##################################################################
# Create a selection based on HLT D0->Kpi
##################################################################
def defaultHLTD0Selection():

    #-- A HLT report decoder is needed
    from Configurables import LoKi__HDRFilter as HDRFilter
    from Configurables import LoKi__VoidFilter as LokiFilter 
    from Configurables import HltDecReportsDecoder, HltSelReportsDecoder, HltCompositionMonitor, AddToProcStatus
    from Configurables import GaudiSequencer

    from Configurables import RecSysConf
    RecSysConf().RecoSequence = ["Hlt","Decoding","AlignTr","Vertex","RICH" ]

    hltfilterSeq = GaudiSequencer( "RecoHltSeq" )
    # identifies events that are not of type Hlt1ErrorEvent or Hlt2ErrorEvent
    # taken from Rec/Brunel/python/Brunel/Configuration.py
    filterCode = "HLT_PASS_RE('Hlt1(?!ErrorEvent).*Decision') & HLT_PASS_RE('Hlt2(?!ErrorEvent).*Decision')"  # from Gerhard
    hltErrorFilter = HDRFilter('HltErrorFilter', Code = filterCode )   # the filter
    hltfilterSeq.Members += [HltDecReportsDecoder(),             # decode DecReports
                             HltCompositionMonitor(),
                             hltErrorFilter  ]                   # apply filter

    #-- Physics Filter
    from Configurables import LoKi__HDRFilter as HDRFilter 
    hltfilter = HDRFilter ( 'HLTFilter' ,
                           #Code = "HLT_PASS_RE( 'Hlt2CharmHadD02HH_D02KPiDecision' )"
                            Code = "HLT_PASS_RE( 'Hlt2ExpressDStar2D0PiDecision' )"# | HLT_PASS_RE( 'Hlt2ExpressBeamHalo' )"
                            )
    hltfilter.Preambulo += [ "from LoKiCore.functions import *" ]
    hltfilterSeq.Members += [ hltfilter ]

    # revive only particles used for trigger
    from Configurables import HltTrackConverter
    hltTrackConv = HltTrackConverter("HltTrackConv")
    hltTrackConv.ReadHltLinesFrom1stEvent = 'Hlt2ExpressDStar2D0PiDecision'
    hltTrackConv.TrackDestignation = 'Rec/Track/AllBest'
    hltfilterSeq.Members += [ HltSelReportsDecoder(), hltTrackConv ]#, trkContCopy]

    # create a sequence that fits the tracks and does the hit-adding
    trackseq = GaudiSequencer("RecoAlignTrSeq")
    configuredFitAndHitAdderSequence( Sequence = trackseq, 
                                      InputLocation = hltTrackConv.TrackDestignation,
                                      OutputLocation = 'Rec/Track/Best' )
    # now make sure that there are at least 2 tracks left
    trackseq.Members.append( LokiFilter ( 'BestTrackFilter' ,
                                          Code = "0 < CONTAINS ( 'Rec/Track/Best' )" ) )
 

    # Tweak a little bit RICH
    from Configurables import Escher,RichRecSysConf,RecSysConf
    richSeqName         = Escher()._instanceName(RichRecSysConf)
    richSeq             = GaudiSequencer(richSeqName+"Seq")
    richSeq.MeasureTime = True
    richConf            = RichRecSysConf(RecSysConf().richRecConfName)
    richConf.DataType   = Escher().getProp( "DataType" )
    richConf.Context    = "HLT"
    #enable 'fast' PID settings
    richConf.PidConfig          = "FastGlobal"
    # only consider the Pion and Kaon hypos (instead of the full list. This means you will
    # only have DLL(K-pi) available, but if that is all you need its OK).
    richConf.Particles    = [ "pion","kaon" ]
    #turn off trackless ring finding
    richConf.TracklessRingAlgs  = []
    #richConf.trackConfig().InputTracksLocation = '/Rec/Track/PidTracks'
    # Set the sequence to run the RICH PID in
    richConf.setProp("RecoSequencer",richSeq)

    # now create the D0->K-Pi+ candidates
    from Configurables import FilterDesktop
    from Configurables import ChargedProtoParticleMaker, ChargedProtoParticleAddRichInfo, ChargedProtoCombineDLLsAlg

    # take as much as possible from CommonParticles
    from CommonParticles.StdAllLooseKaons import StdAllLooseKaons
    from CommonParticles.StdAllLoosePions import StdAllLoosePions
    from CommonParticles.StdLooseKaons import StdLooseKaons
    from CommonParticles.StdLoosePions import StdLoosePions
    from CommonParticles.StdLooseD02HH import StdLooseD02KPi
    # remove cuts that require a PV
    StdLooseD02KPi.MotherCut = "((VFASPF(VCHI2)<10) & (ADMASS('D0')<100*MeV))"
    StdLooseKaons.Code = "ALL"
    StdLoosePions.Code = "ALL"

    # tighten the mass window for candidates used in alignment. also
    # add tight PID cuts basically toensure that we don't swap the
    # kaon and pion.
    AlignD02KPi = FilterDesktop("AlignD02KPi",
                                Inputs = ["Phys/StdLooseD02KPi"], 
                                Code = "(ADMASS('D0') < 20.*MeV) & (VFASPF(VCHI2) < 9.)"\
                                    " & (MINTREE('K+'==ABSID, PIDK) > 0)"\
                                    " & (MINTREE('pi+'==ABSID, PIDK) < 0)" )
    # create the sequence that we pass to the alignment 
    recoD0Seq= GaudiSequencer("RecoD0Seq")
    recoD0Seq.Members = [ 
        ChargedProtoParticleMaker('ChargedProtoPMaker'),
        ChargedProtoParticleAddRichInfo('ChargedProtoPAddRich'),
        ChargedProtoCombineDLLsAlg('ChargedProtoPCombDLLs'),
        #StdAllLoosePions, StdAllLooseKaons,
        #StdLoosePions, StdLooseKaons,
        #StdLooseD02KPi,
        AlignD02KPi]

    # Add monitors
    from Configurables import TrackParticleMonitor
    recoD0Seq.Members += [ 
        TrackParticleMonitor('StdLooseD02KPiMonitor', 
                             InputLocation = '/Event/Phys/StdLooseD02KPi/Particles',
                             MinMass = 1810, MaxMass = 1930),
        TrackParticleMonitor('AlignD02KPiMonitor', 
                             InputLocation = '/Event/Phys/AlignD02KPi/Particles',
                             MinMass = 1810, MaxMass = 1930)
        ]

    d0selection = ParticleSelection()
    d0selection.algorithm = recoD0Seq
    d0selection.location  = '/Event/Phys/AlignD02KPi/Particles'
    return d0selection

##################################################################
# Create a selection based on HLT J/psi->mumu
##################################################################
def defaultHLTJPsiSelection():

    #-- A HLT report decoder is needed
    from Configurables import LoKi__HDRFilter as HDRFilter
    from Configurables import LoKi__VoidFilter as LokiFilter 
    from Configurables import HltDecReportsDecoder, HltSelReportsDecoder, HltCompositionMonitor, AddToProcStatus
    from Configurables import GaudiSequencer

    from Configurables import RecSysConf
    RecSysConf().RecoSequence = ["Hlt","Decoding","AlignTr","Vertex","MUON" ]

    # fix problem in rich config
    #from Configurables import RichTrackCreatorConfig
    #RichTrackCreatorConfig().InputTracksLocation = "/Rec/Track/Best"

    hltfilterSeq = GaudiSequencer( "RecoHltSeq" )
# identifies events that are not of type Hlt1ErrorEvent or Hlt2ErrorEvent -- taken from Rec/Brunel/python/Brunel/Configuration.py
    filterCode = "HLT_PASS_RE('Hlt1(?!ErrorEvent).*Decision') & HLT_PASS_RE('Hlt2(?!ErrorEvent).*Decision')"  # from Gerhard
    hltErrorFilter = HDRFilter('HltErrorFilter', Code = filterCode )   # the filter
    hltfilterSeq.Members += [HltDecReportsDecoder(),             # decode DecReports
                             HltCompositionMonitor(),
                             hltErrorFilter  ]                   # apply filter

    #-- Physics Filter
    from Configurables import LoKi__HDRFilter as HDRFilter 
    hltfilter = HDRFilter ( 'HLTFilter' ,
                            #Code = "HLT_PASS_RE( 'Hlt2DiMuonJPsi.*Decision' )"
                            Code = "HLT_PASS_RE( 'Hlt2ExpressJPsi.*Decision' )"
                            )
    hltfilter.Preambulo += [ "from LoKiCore.functions import *" ]
    hltfilterSeq.Members += [ hltfilter ]

    # revive only particles used for trigger
    from Configurables import HltTrackConverter
    hltTrackConv = HltTrackConverter("HltTrackConv")
    hltTrackConv.ReadHltLinesFrom1stEvent = 'Hlt2ExpressJPsiMuMuDecision'
    hltTrackConv.TrackDestignation = 'Rec/Track/AllBest'
    hltfilterSeq.Members += [ HltSelReportsDecoder(), hltTrackConv ]#, trkContCopy]

    # create a sequence that fits the tracks and does the hit-adding
    trackseq = GaudiSequencer("RecoAlignTrSeq")
    configuredFitAndHitAdderSequence( Sequence = trackseq, 
                                      InputLocation = hltTrackConv.TrackDestignation,
                                      OutputLocation = 'Rec/Track/Best' )
    # now make sure that there are at least 2 tracks left
    trackseq.Members.append( LokiFilter ( 'BestTrackFilter' ,
                                          Code = "0 < CONTAINS ( 'Rec/Track/Best' )" ) )
                             
    # Now create the J/psi candidates
    from Configurables import CombineParticles, FilterDesktop
    from CommonParticles.StdAllLooseMuons import StdAllLooseMuons
    from CommonParticles.StdLooseJpsi2MuMu import StdLooseJpsi2MuMu
    
    ## tighten the mass window for candidates used in alignment
    AlignJpsi2MuMu = FilterDesktop("AlignJpsi2MuMu",
                                   Inputs = ["Phys/StdLooseJpsi2MuMu"], 
                                   Code = "(ADMASS('J/psi(1S)') < 25.*MeV) & (VFASPF(VCHI2) < 9.)")
    
    from Configurables import ChargedProtoParticleMaker, ChargedProtoParticleAddMuonInfo, ChargedProtoCombineDLLsAlg
    from Configurables import TrackParticleMonitor
    recoJpsiSeq= GaudiSequencer("RecoJpsiSeq")
    recoJpsiSeq.Members = [ 
        ChargedProtoParticleMaker('ChargedProtoPMaker'),
        ChargedProtoParticleAddMuonInfo('ChargedProtoPAddMuon'),
        ChargedProtoCombineDLLsAlg('ChargedProtoPCombDLLs'),
        StdAllLooseMuons,  # we could also get this from the DoD
        StdLooseJpsi2MuMu, # we could also get this from the DoD
        TrackParticleMonitor('JpsiParticleMonitor', 
                             InputLocation = '/Event/Phys/StdLooseJpsi2MuMu/Particles',
                             MinMass = 3000, MaxMass = 3190),
        AlignJpsi2MuMu]
    
    selection = ParticleSelection()
    selection.algorithm = recoJpsiSeq
    selection.location  = '/Event/Phys/AlignJpsi2MuMu/Particles'
    return selection

