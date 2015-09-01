"""
 script to configure HLT Afterburner

 @author R. Aaij
 @date 13-05-2015
"""
# =============================================================================
__author__  = "R. Aaij roel.aaij@cern.ch"
# =============================================================================
import types
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer as Sequence

class HltAfterburnerConf(LHCbConfigurableUser):
    __used_configurables__ = []

    __slots__ = {"Sequence"                : None,
                 "EnableHltRecSummary"     : True,
                 "RecSummaryLocation"      : "Hlt2/RecSummary",
                 "AddAdditionalTrackInfos" : True,
                 "AddPIDToDownstream"      : True,
                 "Hlt2DownstreamFilter"    : "HLT_TURBOPASS_RE('Hlt2.*DDTurbo.*Decision') | HLT_TURBOPASS_RE('Hlt2.*Spec.*LambdaTurbo.*Decision') | HLT_TURBOPASS_RE('Hlt2.*Lcp.*Lam.*Turbo.*Decision')",
                 "Hlt2Filter"              : "HLT_PASS_RE('Hlt2(?!Forward)(?!DebugEvent)(?!Lumi)(?!Transparent)(?!PassThrough).*Decision')"
                }

###################################################################################
#
# Main configuration
#
    def __apply_configuration__(self):
        """
        HLT Afterburner configuration
        """

        Afterburner = self.getProp("Sequence") if self.isPropertySet("Sequence") else None
        if not Afterburner:
            return
        if self.getProp("Hlt2Filter"):
            from DAQSys.Decoders import DecoderDB
            decoder = DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"]
            from Configurables import LoKi__HDRFilter   as HDRFilter
            hlt2Filter = HDRFilter('HltAfterburnerHlt2Filter', Code = self.getProp("Hlt2Filter"),
                                   Location = decoder.listOutputs()[0])
            Afterburner.Members += [hlt2Filter]

        if self.getProp("EnableHltRecSummary"):
            from Configurables import RecSummaryAlg
            seq = Sequence("RecSummarySequence")

            from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
            from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking
            tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()
            tracksDown = Hlt2BiKalmanFittedDownstreamTracking().hlt2PrepareTracks()
            muonID = Hlt2BiKalmanFittedForwardTracking().hlt2MuonID()

            from HltLine.HltDecodeRaw import DecodeVELO, DecodeIT, DecodeTT, DecodeSPD, DecodeMUON
            decoders = {"Velo"   : (DecodeVELO, "VeloLiteClustersLocation"),
                        "TT"     : (DecodeTT,   "clusterLocation"),
                        "IT"     : (DecodeIT,   "clusterLocation"),
                        "SPD"    : (DecodeSPD,  "DigitsContainer"),
                        'Muon'   : (DecodeMUON, "OutputLocation"),
                        'MuonTr' : (muonID,     "MuonTrackLocation")}
            decoders = {k : (bm.members(), bm.members()[-1].getProp(loc)) for (k, (bm, loc)) in decoders.iteritems()}

            from HltTracking.HltPVs import PV3D
            PVs = PV3D("Hlt2")
            from HltTracking.HltTrackNames import Hlt2TrackLoc
            recSeq = Sequence("RecSummaryRecoSequence", IgnoreFilterPassed = True)
            from itertools import chain
            from Hlt2Lines.Utilities.Utilities import uniqueEverseen
            recSeq.Members = (list(chain.from_iterable([dec[0] for dec in decoders.itervalues()]))
                              + list(uniqueEverseen(chain.from_iterable([tracks, tracksDown, muonID, PVs]))))
            summary = RecSummaryAlg('Hlt2RecSummary', SummaryLocation = self.getProp("RecSummaryLocation"),
                                    HltSplitTracks = True,
                                    SplitLongTracksLocation = tracks.outputSelection(),
                                    SplitDownTracksLocation = tracksDown.outputSelection(),
                                    PVsLocation = PVs.output,
                                    VeloClustersLocation = decoders['Velo'][1],
                                    ITClustersLocation = decoders['IT'][1],
                                    TTClustersLocation = decoders['TT'][1],
                                    SpdDigitsLocation  = decoders['SPD'][1],
                                    MuonCoordsLocation = decoders['Muon'][1],
                                    MuonTracksLocation = decoders['MuonTr'][1])
            seq.Members = [recSeq, summary]
            Afterburner.Members += [seq]

        if self.getProp("AddAdditionalTrackInfos"):
            from GaudiKernel.SystemOfUnits import mm
            from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
            from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking
            from Configurables import LoKi__VoidFilter as Filter
            trackLocations = {"Long" : Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks().outputSelection(),
                              "Downstream" : Hlt2BiKalmanFittedDownstreamTracking().hlt2PrepareTracks().outputSelection()}
            infoSeq = Sequence("TrackInfoSequence", IgnoreFilterPassed = True)
            # I don't want to pull in reconstruction if not run before, then there should be also no candidates needing this information
            #infoSeq.Members +=  Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks().members() + Hlt2BiKalmanFittedDownstreamTracking().hlt2PrepareTracks().members()
            for name, location in trackLocations.iteritems():
                from Configurables import TrackBuildCloneTable, TrackCloneCleaner
                prefix = name
                trackClones = Sequence(prefix + "TrackClonesSeq")
                checkTracks =  Filter(prefix+"CheckTrackLoc",Code = "EXISTS('%(trackLoc)s')"% {"trackLoc" : location})
                trackClones.Members += [checkTracks]
                cloneTable = TrackBuildCloneTable(prefix + "FindTrackClones")
                cloneTable.maxDz   = 500*mm
                cloneTable.zStates = [ 0*mm, 990*mm, 9450*mm ]
                cloneTable.klCut   = 5e3
                cloneTable.inputLocation = location
                cloneTable.outputLocation = location + "Clones"
                cloneCleaner = TrackCloneCleaner(prefix + "FlagTrackClones")
                cloneCleaner.CloneCut = 5e3
                cloneCleaner.inputLocation = location
                cloneCleaner.linkerLocation = cloneTable.outputLocation
                trackClones.Members += [ cloneTable, cloneCleaner ]

                ## Add the likelihood information
                from Configurables import TrackAddLikelihood, TrackLikelihood
                trackAddLikelihood = TrackAddLikelihood(prefix + "TrackAddLikelihood" )
                trackAddLikelihood.addTool( TrackLikelihood, name = "TrackMatching_likTool" )
                trackAddLikelihood.TrackMatching_likTool.otEff = 0.9
                trackAddLikelihood.inputLocation = location
                trackClones.Members += [ trackAddLikelihood ]
                infoSeq.Members += [trackClones]

            Afterburner.Members += [infoSeq]

            # Add VeloCharge to protoparticles for dedx
            veloChargeSeq = Sequence("VeloChargeSequence")
            from Configurables import ChargedProtoParticleAddVeloInfo
            protoLocation = Hlt2BiKalmanFittedForwardTracking().hlt2ChargedAllPIDsProtos().outputSelection()
            checkProto =  Filter("CheckProtoParticles",Code = "EXISTS('%(protoLoc)s')"% {"protoLoc" : protoLocation})
            addVeloCharge = ChargedProtoParticleAddVeloInfo("Hlt2AddVeloCharge")
            addVeloCharge.ProtoParticleLocation = protoLocation
            decodeVeloFullClusters = DecoderDB["DecodeVeloRawBuffer/createVeloClusters"].setup()
            veloChargeSeq.Members +=  [checkProto, decodeVeloFullClusters, addVeloCharge]
            Afterburner.Members += [veloChargeSeq]


        if self.getProp("AddPIDToDownstream"):
            from Configurables import LoKi__HDRFilter   as HDRFilter
            hlt2DownstreamFilter = HDRFilter('DownstreamHlt2Filter', Code = self.getProp("Hlt2DownstreamFilter"),
                                   Location = decoder.listOutputs()[0])
            downstreamPIDSequence = Sequence( "Hlt2AfterburnerDownstreamPIDSeq")
            downstreamPIDSequence.Members += [ hlt2DownstreamFilter ]
            from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking
            downstreamTracking = Hlt2BiKalmanFittedDownstreamTracking()
            tracksDown = downstreamTracking.hlt2PrepareTracks()
            chargedProtosOutputLocation =  downstreamTracking.hlt2ChargedNoPIDsProtos().outputSelection()
            from DAQSys.Decoders import DecoderDB
            decoder = DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"]
            richPid = downstreamTracking.hlt2RICHID()

            downstreamPIDSequence.Members += list(uniqueEverseen(chain.from_iterable([ tracksDown, richPid ])))
            from Configurables import ChargedProtoParticleAddRichInfo,ChargedProtoParticleAddMuonInfo
            downstreamRichDLL_name            = "Hlt2AfterburnerDownstreamProtoPAddRich"
            downstreamRichDLL                 = ChargedProtoParticleAddRichInfo(downstreamRichDLL_name)
            downstreamRichDLL.InputRichPIDLocation    = richPid.outputSelection()
            downstreamRichDLL.ProtoParticleLocation   = chargedProtosOutputLocation
            # Add the muon info to the DLL
            downstreamMuon_name                   = "Hlt2AfterburnerDownstreamProtoPAddMuon"
            downstreamMuon                        = ChargedProtoParticleAddMuonInfo(downstreamMuon_name)
            downstreamMuon.ProtoParticleLocation  = chargedProtosOutputLocation
            downstreamMuon.InputMuonPIDLocation   = downstreamTracking.hlt2MuonID().outputSelection()
            # Add the Calo info to the DLL
            #
            from Configurables import ( ChargedProtoParticleAddEcalInfo,
                                        ChargedProtoParticleAddBremInfo,
                                        ChargedProtoParticleAddHcalInfo,
                                        ChargedProtoParticleAddPrsInfo,
                                        ChargedProtoParticleAddSpdInfo
                                        )
            caloPidLocation = downstreamTracking.hlt2CALOID().outputSelection()
            downstreamEcal = ChargedProtoParticleAddEcalInfo("HltAfterburnerDownstreamProtoPAddEcal")
            downstreamBrem = ChargedProtoParticleAddBremInfo("HltAfterburnerDownstreamProtoPAddBrem")
            downstreamHcal = ChargedProtoParticleAddHcalInfo("HltAfterburnerDownstreamProtoPAddHcal")
            downstreamPrs  = ChargedProtoParticleAddPrsInfo ("HltAfterburnerDownstreamProtoPAddPrs")
            downstreamSpd  = ChargedProtoParticleAddSpdInfo ("HltAfterburnerDownstreamProtoPAddSpd")
            for alg in (downstreamEcal,downstreamBrem,downstreamHcal,downstreamPrs,downstreamSpd):
                alg.setProp("ProtoParticleLocation",chargedProtosOutputLocation)
                alg.setProp("Context",caloPidLocation)
                downstreamPIDSequence.Members += [ alg  ]


            from Configurables import ChargedProtoCombineDLLsAlg
            downstreamCombine_name                    = "Hlt2AfterburnerDownstreamRichCombDLLs"
            downstreamCombine                         = ChargedProtoCombineDLLsAlg(downstreamCombine_name)
            downstreamCombine.ProtoParticleLocation   = downstreamTracking.hlt2ChargedNoPIDsProtos().outputSelection()
            from Hlt2Lines.Utilities.Utilities import uniqueEverseen
            downstreamPIDSequence.Members += [ downstreamMuon,downstreamRichDLL,downstreamCombine ]
            Afterburner.Members += [ downstreamPIDSequence ]
