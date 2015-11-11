##################################################################
# Data structure to hold a particles selection for the alignment.
# This still needs to be moved to a better location.
##################################################################

class ParticleSelection:
    #__slots__ = {
    #    "_name" : ""
    #    ,"_location" : "" # particle location in the TES
    #    ,"_algorithm" : None
    #    }
    def __init__( self, Name, Location, Algorithm = None ) :
        self._name = Name
        self._location = Location
        self._algorithm = Algorithm

    def algorithm(self) : return self._algorithm
    def location(self) : return self._location
    def name(self) : return self._name

##################################################################
# Creates a selection object from a location in the TES ona dst,
# making sure to refit the particles. It would make sense to add some
# cuts here as well.
##################################################################
def configuredParticleListFromDST( ParticleLocation, FilterCode = "" ) :
    from Configurables import GaudiSequencer
    from Configurables import TrackParticleRefitter, TrackMasterFitter
    from TrackFitter.ConfiguredFitters  import ConfiguredMasterFitter
    # create a sequence to refit and monitor
    name = ParticleLocation
    name = name.replace("/Event","")
    name = name.replace("/Phys","")
    name = name.replace("/Particles","")
    name = name.replace("/","")
    fitter = TrackParticleRefitter( name + "Refitter",
                                    TrackFitter = TrackMasterFitter(),
                                    ParticleLocation = ParticleLocation )
    ConfiguredMasterFitter( fitter.TrackFitter, SimplifiedGeometry = True )
    seq = GaudiSequencer(name + "Seq",IgnoreFilterPassed = True)
    seq.Members.append( fitter )
    if FilterCode != "":
        from Configurables import FilterDesktop
        newLocation = ParticleLocation.replace("/Particles","") + "Filtered"
        newfilter = FilterDesktop( #name + "Filter",#Output =
                                   newLocation,
                                   Inputs = [ParticleLocation],
                                   CloneFilteredParticles = False,
                                   Code = FilterCode )
        seq.Members.append( newfilter )
        ParticleLocation = newLocation + "/Particles"
    sel = ParticleSelection( Name = name,
                             Location = ParticleLocation,
                             Algorithm = seq)
    return sel


def protoParticlesFromHLTSelSequence( Name, HltDecision,
                                      ApplyRichID = False,
                                      ApplyMuonID = False):
    # create a sequence that
    # * revives tracks from an HLT sequence
    # * fits them
    # * runs rich or muon ID on demand
    # tracks will be stored at location 'Rec/Track/' + Name
    # protoparticles will be stored at location ????
    trackListName =  'Rec/Track/' + Name
    from Configurables import GaudiSequencer
    seq = GaudiSequencer(Name + "Seq")
    # revive only particles used for trigger
    from Configurables import HltTrackConverter
    hltTrackConv = HltTrackConverter(Name + "HltTrackConv")
    hltTrackConv.ReadHltLinesFrom1stEvent = HltDecision
    hltTrackConv.TrackDestination = 'Rec/Track/' + Name + 'All'
    if Escher().DataType in ['2015']: hltTrackConv.SelReportsLocation = 'Hlt1/SelReports'

    ## TODO: Watch out for the HLT1/2 Split here!!
    ## probably the decoder DB will take care of it for you...
    from DAQSys.Decoders import DecoderDB
    hltdecs = []
    if Escher().DataType in ['2015']:
        decDec = DecoderDB["HltDecReportsDecoder/Hlt1DecReportsDecoder"]
        selDec = DecoderDB["HltSelReportsDecoder/Hlt1SelReportsDecoder"]
        hltdecs += [decDec, selDec]
    else:
        from DAQSys.DecoderClass import decodersForBank
        hltdecs=decodersForBank(DecoderDB,"HltSelReports")
    trackseq.Members += [ d.setup() for d in hltdecs ]
    trackseq.Members += [ hltTrackConv ]

    # now fit those tracks and apply a selection
    from TAlignment.Utils import configuredFitAndHitAdderSequence
    fitseq = configuredFitAndHitAdderSequence( Name + 'Fit',
                                               trackListName + 'All',
                                               trackListName )
    seq.Members += [ fitseq ]
    # next step, prepare the RICH sequence, if we need it.
    #if ApplyRichID:
    return seq

##################################################################
# Defines a minimal RICH sequence for running pion and kaon ID
##################################################################
def MinimalRichSequence():
    # Tweak a little bit RICH
    from Configurables import Escher,GaudiSequencer,RichRecSysConf,RecSysConf
    richSeqName         = Escher()._instanceName(RichRecSysConf)
    richSeq             = GaudiSequencer(richSeqName+"Seq")
    richSeq.MeasureTime = True
    richConf            = RichRecSysConf( "RichOfflineRec" )
    richConf.DataType   = Escher().getProp( "DataType" )
#    richConf.Context    = "HLT"
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
    return

##################################################################
# Revive tracks that triggered the event
##################################################################
def ReviveHltTracks(TriggerLines=[]):

    HltLines = TriggerLines
    # Default trigger lines from Escher filter
    from Configurables import Escher
    if len(TriggerLines) == 0:
        lines = Escher().HltFilterCode.split('\'')
        for l in lines:
            if l.find('Hlt') != -1 and l.find('Decision') -1: HltLines.append(l)

    # revive only particles used for trigger
    from Configurables import GaudiSequencer
    trackseq = GaudiSequencer("RecoAlignTrSeq")

    from Configurables import HltTrackConverter
    hltTrackConv = HltTrackConverter("HltTrackConv")
    hltTrackConv.HltLinesToUse = HltLines
    hltTrackConv.TrackDestination = 'Rec/Track/AllBest'
    if Escher().DataType in ['2015']: hltTrackConv.SelReportsLocation = 'Hlt1/SelReports'

    ## TODO: Watch out for the HLT1/2 Split here!!
    ## probably the decoder DB will take care of it for you...
    from DAQSys.Decoders import DecoderDB
    hltdecs = []
    if Escher().DataType in ['2015']:
        decDec = DecoderDB["HltDecReportsDecoder/Hlt1DecReportsDecoder"]
        selDec = DecoderDB["HltSelReportsDecoder/Hlt1SelReportsDecoder"]
        hltdecs += [decDec, selDec]
    else:
        from DAQSys.DecoderClass import decodersForBank
        hltdecs=decodersForBank(DecoderDB,"HltSelReports")
    trackseq.Members += [ d.setup() for d in hltdecs ]
    trackseq.Members += [ hltTrackConv ]

    # create a sequence that fits the tracks and does the hit-adding
    from TAlignment.Utils import configuredFitAndHitAdderSequence
    fitseq =  configuredFitAndHitAdderSequence( Name = 'HltBest',
                                                InputLocation = hltTrackConv.TrackDestination,
                                                OutputLocation = 'Rec/Track/Best' )
    trackseq.Members.append( fitseq )
    # now make sure that there are at least 2 tracks left
    from Configurables import LoKi__VoidFilter as LokiFilter
    trackseq.Members.append( LokiFilter ( 'BestTrackFilter' ,
                                          Code = "1 < CONTAINS ( 'Rec/Track/Best' )" ) )
    return HltLines


##################################################################
# Create a selection based on HLT D0->Kpi
##################################################################
def defaultHLTD0Selection():

    # this still needs to be worked out
    from Configurables import Escher
    Escher().RecoSequence = ["Hlt","Decoding","AlignTr",'VELO', 'TrHLT1',"Vertex","RICH" ]
    if Escher().DataType in ['2011','2012']:
        Escher().RecoSequence = ["Hlt","Decoding","AlignTr","Vertex","RICH" ]
    Escher().MoniSequence = ["Tr","OT"]

    # if the Escher hlt filter is not set, set it here
    if not hasattr(Escher(),"HltFilterCode") or not Escher().HltFilterCode :
        Escher().HltFilterCode = "HLT_PASS_RE( 'Hlt2ExpressD02KPiDecision' )"

    # retrieve the trigger lines from the hlt filter code
    print 'Hlt lines to be used: '
    print ReviveHltTracks()

    # Tweak a little bit RICH
    MinimalRichSequence()

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

    # add tight PID cuts basically toensure that we don't swap the
    # kaon and pion.
    AlignD02KPiWideCode = "(ADMASS('D0') < 50.*MeV) & (VFASPF(VCHI2) < 9.)"
    if Escher().DataType in ['2011','2012']:
        AlignD02KPiWideCode = AlignD02KPiWideCode + \
            " & (MINTREE('K+'==ABSID, PIDK) > 0)" \
            " & (MINTREE('pi+'==ABSID, PIDK) < 0)"
    AlignD02KPiWide = FilterDesktop("AlignD02KPiWide",
                                    Inputs = ["Phys/StdLooseD02KPi"],
                                    Code = AlignD02KPiWideCode )

    # tighten the mass window for candidates used in alignment.
    AlignD02KPi = FilterDesktop("AlignD02KPi",
                                Inputs = ["Phys/AlignD02KPiWide"],
                                Code = "(ADMASS('D0') < 20.*MeV)" )

    # create the sequence that we pass to the alignment
    from Configurables import TrackParticleMonitor, GaudiSequencer
    recoD0Seq= GaudiSequencer("RecoD0Seq")
    recoD0Seq.Members = [
        ChargedProtoParticleMaker('ChargedProtoPMaker'),
        ChargedProtoParticleAddRichInfo('ChargedProtoPAddRich'),
        ChargedProtoCombineDLLsAlg('ChargedProtoPCombDLLs'),
        TrackParticleMonitor('StdLooseD02KPiMonitor',
                              InputLocation = '/Event/Phys/StdLooseD02KPi/Particles',
                              MinMass = 1810, MaxMass = 1930),
        AlignD02KPiWide,
        TrackParticleMonitor('AlignD02KPiWideMonitor',
                             InputLocation = '/Event/Phys/AlignD02KPiWide/Particles',
                             MinMass = 1810, MaxMass = 1930),
        AlignD02KPi,
        TrackParticleMonitor('AlignD02KPiMonitor',
                             InputLocation = '/Event/Phys/AlignD02KPi/Particles',
                             MinMass = 1810, MaxMass = 1930)
                        ]

    sel = ParticleSelection( Name = 'D02KPi',
                             Location = '/Event/Phys/AlignD02KPi/Particles',
                             Algorithm = recoD0Seq )
    return sel

##################################################################
# Create a selection based on HLT D*+->D0pi, D0->Kpi
##################################################################
def defaultHLTDstarSelection():

    # this still needs to be worked out
    from Configurables import Escher
    Escher().RecoSequence = ["Hlt","Decoding","AlignTr","Vertex","RICH" ]
    Escher().MoniSequence = ["Tr","OT"]

    # if the Escher hlt filter is not set, set it here
    if not hasattr(Escher(),"HltFilterCode") or not Escher().HltFilterCode :
        Escher().HltFilterCode = "HLT_PASS_RE( 'Hlt2ExpressDStar2D0PiDecision' )"

    # retrieve the trigger lines from the hlt filter code
    print 'Hlt lines to be used: '
    print ReviveHltTracks()

    # Tweak a little bit RICH
    MinimalRichSequence()

    # now create the D0->K-Pi+ candidates
    from Configurables import FilterDesktop
    from Configurables import ChargedProtoParticleMaker, ChargedProtoParticleAddRichInfo, ChargedProtoCombineDLLsAlg

    # take as much as possible from CommonParticles
    from CommonParticles.StdAllLooseKaons import StdAllLooseKaons
    from CommonParticles.StdAllLoosePions import StdAllLoosePions
    from CommonParticles.StdLooseKaons import StdLooseKaons
    from CommonParticles.StdLoosePions import StdLoosePions
    from CommonParticles.StdLooseD02HH import StdLooseD02KPi
    from CommonParticles.StdLooseDstarWithD2HH import StdLooseDstarWithD02KPi
    # remove cuts that require a PV
    StdLooseD02KPi.MotherCut = "((VFASPF(VCHI2)<10) & (ADMASS('D0')<100*MeV))"
    StdLooseKaons.Code = "ALL"
    StdLoosePions.Code = "ALL"

    # add tight PID cuts basically toensure that we don't swap the
    # kaon and pion.
    AlignD02KPiWide = FilterDesktop("AlignD02KPiWide",
                                    Inputs = ["Phys/StdLooseD02KPi"],
                                    Code = "(ADMASS('D0') < 50.*MeV) & (VFASPF(VCHI2) < 9.)" )

    # tighten the mass window for candidates used in alignment.
    AlignD02KPi = FilterDesktop("AlignD02KPi",
                                Inputs = ["Phys/AlignD02KPiWide"],
                                Code = "(ADMASS('D0') < 20.*MeV)" )

    # create the sequence that we pass to the alignment
    from Configurables import TrackParticleMonitor, GaudiSequencer
    recoDstarWithD0Seq= GaudiSequencer("RecoDstarWithD0Seq")
    recoDstarWithD0Seq.Members = [
        ChargedProtoParticleMaker('ChargedProtoPMaker'),
        ChargedProtoParticleAddRichInfo('ChargedProtoPAddRich'),
        ChargedProtoCombineDLLsAlg('ChargedProtoPCombDLLs'),
        TrackParticleMonitor('StdLooseD02KPiMonitor',
                              InputLocation = '/Event/Phys/StdLooseD02KPi/Particles',
                              MinMass = 1810, MaxMass = 1930),
        StdLooseDstarWithD02KPi, #apply Dstar cut
        AlignD02KPiWide,
        TrackParticleMonitor('AlignD02KPiWideMonitor',
                             InputLocation = '/Event/Phys/AlignD02KPiWide/Particles',
                             MinMass = 1810, MaxMass = 1930),
        AlignD02KPi,
        TrackParticleMonitor('AlignD02KPiMonitor',
                             InputLocation = '/Event/Phys/AlignD02KPi/Particles',
                             MinMass = 1810, MaxMass = 1930)
                        ]

    sel = ParticleSelection( Name = 'DstarWithD02KPi',
                             Location = '/Event/Phys/AlignD02KPi/Particles',
                             Algorithm = recoDstarWithD0Seq )
    return sel


##################################################################
# Create a selection based on HLT J/psi->mumu
##################################################################
def defaultHLTJPsiSelection(lines = []):

    # this still needs to be worked out
    from Configurables import Escher
    Escher().RecoSequence = ["Hlt","Decoding","AlignTr","Vertex","RICH","CALO","MUON","PROTO" ]
    Escher().MoniSequence = ["Tr","OT"]

    # if the Escher hlt filter is not set, set it here
    if not hasattr(Escher(),"HltFilterCode") or not Escher().HltFilterCode :
        Escher().HltFilterCode = "HLT_PASS_RE( 'Hlt2.*JPsi.*Decision' )"

    # revive only particles used for trigger
    print 'Hlt lines to be used: '
    print ReviveHltTracks(lines if len(lines) else ['Hlt2ExpressJPsiDecision',
                           'Hlt2DiMuonDetachedJPsiDecision',
                           'Hlt2DiMuonJPsiDecision',
                           'Hlt2DiMuonJPsiHighPTDecision'])

    # Now create the J/psi candidates
    from Configurables import CombineParticles, FilterDesktop
    from CommonParticles.StdAllLooseMuons import StdAllLooseMuons
    from CommonParticles.StdLooseJpsi2MuMu import StdLooseJpsi2MuMu
    StdLooseJpsi2MuMu.DaughtersCuts = { "mu-": "(PIDmu-PIDK>5.0) & (PIDmu-PIDe>8.0)" }

    ## tighten the mass window for candidates used in alignment
    AlignJpsi2MuMu = FilterDesktop("AlignJpsi2MuMu",
                                   Inputs = ["Phys/StdLooseJpsi2MuMu"],
                                   Code = "(ADMASS('J/psi(1S)') < 35.*MeV) & (VFASPF(VCHI2) < 7.)") # Tight requirements on vchi2 - test 18/5/2012

    from Configurables import ChargedProtoParticleMaker, ChargedProtoParticleAddMuonInfo, ChargedProtoCombineDLLsAlg
    from Configurables import TrackParticleMonitor, GaudiSequencer
    recoJpsiSeq= GaudiSequencer("RecoJpsiSeq")
    recoJpsiSeq.Members = [
        ChargedProtoParticleMaker('ChargedProtoPMaker'),
        ChargedProtoParticleAddMuonInfo('ChargedProtoPAddMuon'),
        ChargedProtoCombineDLLsAlg('ChargedProtoPCombDLLs'),
        StdAllLooseMuons,  # we could also get this from the DoD
        StdLooseJpsi2MuMu, # we could also get this from the DoD
        TrackParticleMonitor('StdLooseJpsi2MuMuMonitor',
                             InputLocation = '/Event/Phys/StdLooseJpsi2MuMu/Particles',
                             MinMass = 3000, MaxMass = 3190),
        AlignJpsi2MuMu,
        TrackParticleMonitor('AlignJpsi2MuMuMonitor',
                             InputLocation = '/Event/Phys/AlignJpsi2MuMu/Particles',
                             MinMass = 3000, MaxMass = 3190),
        ]

    sel = ParticleSelection( Name = 'Jpsi2MuMu',
                             Location = '/Event/Phys/AlignJpsi2MuMu/Particles',
                             Algorithm = recoJpsiSeq )
    return sel



##################################################################
# Create a selection based on D0->Kpi for Minimum Bias events
##################################################################
def defaultMinBiasD0Selection():

    # this still needs to be worked out
    from Configurables import Escher
    Escher().RecoSequence = ["Decoding","VELO","Tr","Vertex","RICH" ]
    Escher().MoniSequence = ["Tr","OT"]

    # Tweak a little bit RICH
    MinimalRichSequence()

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
    StdLooseD02KPi.MotherCut = "((VFASPF(VCHI2)<10) & (ADMASS('D0')<100*MeV) & (MIPCHI2DV(PRIMARY)<100.0))"
    StdLooseKaons.Code = "((PT>800.*MeV) & (MIPCHI2DV(PRIMARY)>5.0))"
    StdLoosePions.Code = "((PT>800.*MeV) & (MIPCHI2DV(PRIMARY)>5.0))"

    # add tight PID cuts basically to ensure that we don't swap the
    # kaon and pion.
    AlignD02KPiWide = FilterDesktop("AlignD02KPiWide",
                                    Inputs = ["Phys/StdLooseD02KPi"],
                                    Code = "(ADMASS('D0') < 50.*MeV) & (VFASPF(VCHI2) < 9.)" \
    #                                    " & (MINTREE((ABSID=='D0'), VFASPF(VZ))-VFASPF(VZ) > 0.09*mm )" \
                                    " & (MINTREE('K+'==ABSID, PIDK) > 0)" \
                                    " & (MINTREE('pi+'==ABSID, PIDK) < 0)" )

    # tighten the mass window for candidates used in alignment.
    AlignD02KPi = FilterDesktop("AlignD02KPi",
                                Inputs = ["Phys/AlignD02KPiWide"],
                                Code = "(ADMASS('D0') < 20.*MeV)" )

    # create the sequence that we pass to the alignment
    from Configurables import TrackParticleMonitor, GaudiSequencer
    recoD0Seq= GaudiSequencer("RecoD0Seq")
    recoD0Seq.Members = [
        ChargedProtoParticleMaker('ChargedProtoPMaker'),
        ChargedProtoParticleAddRichInfo('ChargedProtoPAddRich'),
        ChargedProtoCombineDLLsAlg('ChargedProtoPCombDLLs'),
        TrackParticleMonitor('StdLooseD02KPiMonitor',
                              InputLocation = '/Event/Phys/StdLooseD02KPi/Particles',
                              MinMass = 1810, MaxMass = 1930),
        AlignD02KPiWide,
        TrackParticleMonitor('AlignD02KPiWideMonitor',
                             InputLocation = '/Event/Phys/AlignD02KPiWide/Particles',
                             MinMass = 1810, MaxMass = 1930),
        AlignD02KPi,
        TrackParticleMonitor('AlignD02KPiMonitor',
                             InputLocation = '/Event/Phys/AlignD02KPi/Particles',
                             MinMass = 1810, MaxMass = 1930)
                        ]

    sel = ParticleSelection( Name = 'D02KPi',
                             Location = '/Event/Phys/AlignD02KPi/Particles',
                             Algorithm = recoD0Seq )
    return sel


##################################################################
# Create a selection based on HLT J/psi->mumu for muon track candidates == SV
##################################################################
def muonTracksFORmuonAlignment():

    # this still needs to be worked out
    from Configurables import Escher
    Escher().RecoSequence = ["Hlt","Decoding","AlignTr","Vertex","RICH","CALO","MUON","PROTO" ] # ????
    Escher().MoniSequence = ["Tr","OT"]

    # if the Escher hlt filter is not set, set it here
    if not hasattr(Escher(),"HltFilterCode") or not Escher().HltFilterCode :
        Escher().HltFilterCode = "HLT_PASS_RE( 'Hlt1DiMuonHighMass*Decision' )" # Hlt2-->Hlt1 requirement

    # revive only particles used for trigger
    print 'Hlt lines to be used: '
    print ReviveHltTracks(['Hlt1DiMuonHighMassDecision'])

    # Now create the J/psi candidates
    from Configurables import CombineParticles, FilterDesktop
    from CommonParticles.StdAllLooseMuons import StdAllLooseMuons    # requires IsMuon==1
    from CommonParticles.StdLooseJpsi2MuMu import StdLooseJpsi2MuMu  # requires (ADAMASS('J/psi')<100.*MeV)&(ADOCACHI2CUT(30,'')) && "(VFASPF(VCHI2) < 25.)"

    StdLooseJpsi2MuMu.DaughtersCuts = { "mu-": "( P> 6000*MeV)" }  # momentum cut

    ## tighten the mass window for candidates used in alignment
    AlignJpsi2MuMu = FilterDesktop("AlignJpsi2MuMu",
                                   Inputs = ["Phys/StdLooseJpsi2MuMu"],
                                   Code = "(ADMASS('J/psi(1S)') < 35.*MeV) & (VFASPF(VCHI2) < 10.)")  # tighter requirements

    from Configurables import ChargedProtoParticleMaker, ChargedProtoParticleAddMuonInfo
    #####, ChargedProtoCombineDLLsAlg
    from Configurables import TrackParticleMonitor, GaudiSequencer
    recoJpsiSeq= GaudiSequencer("RecoJpsiSeq")
    recoJpsiSeq.Members = [
        ChargedProtoParticleMaker('ChargedProtoPMaker'),
        ChargedProtoParticleAddMuonInfo('ChargedProtoPAddMuon'),
        ###ChargedProtoCombineDLLsAlg('ChargedProtoPCombDLLs'),
        StdAllLooseMuons,
        StdLooseJpsi2MuMu,
        TrackParticleMonitor('StdLooseJpsi2MuMuMonitor',
                             InputLocation = '/Event/Phys/StdLooseJpsi2MuMu/Particles',
                             MinMass = 3000, MaxMass = 3190),
        AlignJpsi2MuMu,
        TrackParticleMonitor('AlignJpsi2MuMuMonitor',
                             InputLocation = '/Event/Phys/AlignJpsi2MuMu/Particles',
                             MinMass = 3000, MaxMass = 3190),
        ]

    sel = ParticleSelection( Name = 'MufromJpsiMuMu',
                             Location = '/Event/Phys/AlignJpsi2MuMu/Particles',
                             Algorithm = recoJpsiSeq )
    return sel
