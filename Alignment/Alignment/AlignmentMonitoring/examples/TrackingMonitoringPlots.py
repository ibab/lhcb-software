#!/bin/python

from GaudiPython import gbl
AlMon = gbl.Alignment.AlignmentMonitoring

import ROOT as r
import os

def ConfigureMonApp(outputName, pages):
    r.gStyle.SetOptTitle(1)
    monApp = AlMon.MonitoringApplication(outputName)
    for name, hists in pages.iteritems():
        if len(hists) == 1: monApp.addPage( AlMon.MonitoringPage(name, hists[0]) )
        elif len(hists) == 2: monApp.addPage( AlMon.MonitoringPage(name, hists[0], hists[1]) )
        elif len(hists) == 3: monApp.addPage( AlMon.MonitoringPage(name, hists[0], hists[1], hists[2]) )
        elif len(hists) == 4: monApp.addPage( AlMon.MonitoringPage(name, hists[0], hists[1], hists[2], hists[3]) )
        elif len(hists) == 5: monApp.addPage( AlMon.MonitoringPage(name, hists[0], hists[1], hists[2], hists[3], hists[4]) )
        elif len(hists) == 6: monApp.addPage( AlMon.MonitoringPage(name, hists[0], hists[1], hists[2], hists[3], hists[4], hists[5]) )
        elif len(hists) == 7: monApp.addPage( AlMon.MonitoringPage(name, hists[0], hists[1], hists[2], hists[3], hists[4], hists[5], hists[6]) )
        elif len(hists) == 8: monApp.addPage( AlMon.MonitoringPage(name, hists[0], hists[1], hists[2], hists[3], hists[4], hists[5], hists[6], hists[7]) )
        elif len(hists) == 9: monApp.addPage( AlMon.MonitoringPage(name, hists[0], hists[1], hists[2], hists[3], hists[4], hists[5], hists[6], hists[7], hists[8]) )
        else: print 'Too many histograms per page requested!'
    return monApp

Files = {'base': '/afs/cern.ch/work/m/mamartin/public/Tests/TrackingPlots_2015-5000ev-histos.root',
         'new' : '/afs/cern.ch/work/m/mamartin/public/Tests/TrackingPlots_2015_Velov5_Trackerv8-5000ev-histos.root'}
Pages = {"tracks" : ["Track/TrackMonitor/1",
				     "Track/TrackMonitor/2",
				     "Track/TrackMonitor/TrackMultiplicityFine",
				     "Track/TrackMonitor/history"],
        "Long track properties":
				     ["Track/TrackMonitor/Long/2",
				     "Track/TrackMonitor/Long/126",
				     "Track/TrackMonitor/Long/125",
				     "Track/TrackMonitor/Long/6",
				     "Track/TrackMonitor/Long/7",
				     "Track/TrackMonitor/Long/8"],
        "Downstream track properties":
				     ["Track/TrackMonitor/Downstream/2",
				     "Track/TrackMonitor/Downstream/126",
				     "Track/TrackMonitor/Downstream/125",
				     "Track/TrackMonitor/Downstream/6",
				     "Track/TrackMonitor/Downstream/7",
				     "Track/TrackMonitor/Downstream/8"],
        "Long track hit multiplicity":
				     ["Track/TrackMonitor/Long/110",
				     "Track/TrackMonitor/Long/111",
				     "Track/TrackMonitor/Long/112",
				     "Track/TrackMonitor/Long/114",
				     "Track/TrackMonitor/Long/115",
				     "Track/TrackMonitor/Long/101"],
        "Hits per Long track per layer":
				     ["Track/TrackMonitor/Long/HitVeloLayers",
				     "Track/TrackMonitor/Long/HitTTLayers",
				     "Track/TrackMonitor/Long/HitOTLayers",
				     "Track/TrackMonitor/Long/HitITLayers"],
        "Hits per Downstream track (per layer)":
				     ["Track/TrackMonitor/Downstream/110",
				     "Track/TrackMonitor/Downstream/111",
				     "Track/TrackMonitor/Downstream/112",
				     "Track/TrackMonitor/Downstream/HitTTLayers",
				     "Track/TrackMonitor/Downstream/HitOTLayers",
				     "Track/TrackMonitor/Downstream/HitITLayers"],
        "Hits per Ttrack track (per layer)":
				     ["Track/TrackMonitor/Ttrack/111",
				     "Track/TrackMonitor/Ttrack/112",
				     "Track/TrackMonitor/Ttrack/HitOTLayers",
				     "Track/TrackMonitor/Ttrack/HitITLayers"],  
        "long track chisquares":
				     ["Track/TrackMonitor/Long/3",
				     "Track/TrackMonitor/Long/chi2PerDofMatch",
				     "Track/TrackMonitor/Long/chi2PerDofDownstream",
				     "Track/TrackMonitor/Long/chi2PerDofVelo",
				     "Track/TrackMonitor/Long/numiter"],
        "long track chisquare versus momentum, eta, phi":
				     ["Track/TrackMonitor/Long/chi2ProbVsMom",
				     "Track/TrackMonitor/Long/chi2ProbVsEta",
				     "Track/TrackMonitor/Long/chi2ProbVsPhi",
				     "Track/TrackMonitor/Long/chi2ProbMatchVsMom",
				     "Track/TrackMonitor/Long/chi2ProbDownstreamVsMom",
				     "Track/TrackMonitor/Long/chi2ProbVeloVsMom"],
        "long track chisquare for <10GeV (top) and >10GeV (bottom) momentum":
				     ["Track/LowMomMonitor/Long/3",
				     "Track/LowMomMonitor/Long/chi2PerDofMatch",
				     "Track/LowMomMonitor/Long/chi2PerDofDownstream",
				     "Track/HighMomMonitor/Long/3",
				     "Track/HighMomMonitor/Long/chi2PerDofMatch",
				     "Track/HighMomMonitor/Long/chi2PerDofDownstream"],
        "residuals":
				     ["Track/TrackMonitor/Long/OTResidual",
				     "Track/TrackMonitor/Long/ITResidual",
				     "Track/TrackMonitor/Long/TTResidual",
				     "Track/TrackMonitor/Long/VeloPhiResidual",
				     "Track/TrackMonitor/Long/VeloRResidual"],
        "residuals for >10 GeV tracks":
				     ["Track/HighMomMonitor/Long/OTResidual",
				     "Track/HighMomMonitor/Long/ITResidual",
				     "Track/HighMomMonitor/Long/TTResidual",
				     "Track/HighMomMonitor/Long/VeloPhiResidual",
				     "Track/HighMomMonitor/Long/VeloRResidual"],
        "residual pulls":
				     ["Track/TrackMonitor/Long/OTresidualPull",
				     "Track/TrackMonitor/Long/ITresidualPull",
				     "Track/TrackMonitor/Long/TTresidualPull",
				     "Track/TrackMonitor/Long/VeloPhiresidualPull",
				     "Track/TrackMonitor/Long/VeloRresidualPull",
				     "Track/TrackMonitor/Long/outliertype"],
        "TT clusters on tracks":
				      ["Track/TTTrackMonitor/charge",
				      "Track/TTTrackMonitor/cluster charge vs slope",
				      "Track/TTTrackMonitor/size",
				      "Track/TTTrackMonitor/cluster size vs tx"],   
        "IT1 clusters on tracks":
				      ["Track/ITTrackMonitor/IT1/charge",
				      "Track/ITTrackMonitor/IT1/cluster charge vs slope",
				      "Track/ITTrackMonitor/IT1/size",
				      "Track/ITTrackMonitor/IT1/cluster size vs tx"],
        "OT hits on tracks":
				      ["OT/OTTrackMonitor/station1/drifttime",
				      "OT/OTHitEfficiencyMonitor/Module4/effvsdist",
				      "OT/OTTrackMonitor/avtimeres",
				      "OT/OTTrackMonitor/station1/residualgood",
				      "OT/OTTrackMonitor/station1/avtimeresvsy",
				      "OT/OTTrackMonitor/drifttimeuse"],
        "More OT hits on tracks":
				      ["OT/OTTrackMonitor/station1/trkdistgood",
				      "OT/OTTrackMonitor/station1/trkdist",
				      "OT/OTTrackMonitor/driftTimeUseVsTrkDist",
				      "OT/OTTrackMonitor/driftTimeUseVsDriftTime"],
        "PV":
				      ["Track/TrackVertexMonitor/PV x position",
				      "Track/TrackVertexMonitor/PV y position",
				      "Track/TrackVertexMonitor/PV z position",
				      "Track/TrackVertexMonitor/NumTracksPerPV",
				      "Track/TrackVertexMonitor/NumLongTracksPerPV",
				      "Track/TrackVertexMonitor/NumBackTracksPerPV"],
        "PV chi-squares":
				      ["Track/TrackVertexMonitor/NumPrimaryVertices",
				      "Track/TrackVertexMonitor/PV chisquare per dof",
				      "Track/TrackVertexMonitor/PV forward chisquare=SLASH=dof",
				      "Track/TrackVertexMonitor/PV backward chisquare=SLASH=dof",
				      "Track/TrackVertexMonitor/PV long chisquare per dof"],
        "PV left-right":
				      ["Track/TrackVertexMonitor/PV left-right delta x",
				      "Track/TrackVertexMonitor/PV left-right delta y",
				      "Track/TrackVertexMonitor/PV left-right delta z",
				      "Track/TrackVertexMonitor/PV left-right delta x versus z",
#				      //"Track/TrackVertexMonitor/PV left-right delta x pull",
#				      //"Track/TrackVertexMonitor/PV long chisquare per dof",
				      "Track/TrackVertexMonitor/PV left-right delta y versus z",
#				      //"Track/TrackVertexMonitor/PV left-right delta y pull",
                        ],
        "PV forward-backward":
				      ["Track/TrackVertexMonitor/PV forward-backward delta x",
				      "Track/TrackVertexMonitor/PV forward-backward delta y",
				      "Track/TrackVertexMonitor/PV forward-backward delta z",
				      "Track/TrackVertexMonitor/PV forward-backward delta x versus z",
#				      //"Track/TrackVertexMonitor/PV forward-backward delta x pull",
#				      //"Track/TrackVertexMonitor/PV long chisquare per dof",
				      "Track/TrackVertexMonitor/PV forward-backward delta y versus z"
#				      //"Track/TrackVertexMonitor/PV forward-backward delta y pull",
                        ],
        "PV left-right (unbiased)":
				      ["Track/TrackPV2HalfAlignMonitor/Left-Right PV delta x",
				      "Track/TrackPV2HalfAlignMonitor/Left-Right PV delta y",
				      "Track/TrackPV2HalfAlignMonitor/Left-Right PV delta z",
				      "Track/TrackPV2HalfAlignMonitor/PV left-right delta x versus z",
				      "Track/TrackPV2HalfAlignMonitor/PV left-right delta y versus z"],
        "IP in X and Y (biased)":
				      ["Track/TrackVertexMonitor/track IP X",
				      "Track/TrackVertexMonitor/track IP X vs phi",
				      "Track/TrackVertexMonitor/track IP X vs eta",
				      "Track/TrackVertexMonitor/track IP Y",
				      "Track/TrackVertexMonitor/track IP Y vs phi",
				      "Track/TrackVertexMonitor/track IP Y vs eta"],
        "IP of track with highest pT (x/y)":
				      ["Track/TrackVertexMonitor/fast track IP X",
				      "Track/TrackVertexMonitor/fast track IP X vs phi",
				      "Track/TrackVertexMonitor/fast track IP X vs eta",
				      "Track/TrackVertexMonitor/fast track IP Y",
				      "Track/TrackVertexMonitor/fast track IP Y vs phi",
				      "Track/TrackVertexMonitor/fast track IP Y vs eta"],
        "IP of track with highest pT (trans/longitudinal)":
				      ["Track/TrackVertexMonitor/fast track transverse IP",
				      "Track/TrackVertexMonitor/fast track transverse IP vs phi",
				      "Track/TrackVertexMonitor/fast track transverse IP vs eta",
				      "Track/TrackVertexMonitor/fast track longitudinal IP",
				      "Track/TrackVertexMonitor/fast track longitudinal IP vs phi",
				      "Track/TrackVertexMonitor/fast track longitudinal IP vs eta"],
        "Two-prongs":
				      ["Track/TrackVertexMonitor/twoprong mass (GeV)",
#				      //"Track/TrackVertexMonitor/twoprong momentum (GeV)",
				      "Track/TrackVertexMonitor/twoprong doca",
				      "Track/TrackVertexMonitor/twoprong doca pull",
				      "Track/TrackVertexMonitor/twoprong decaylength",
				      "Track/TrackVertexMonitor/twoprong decaylength significance",
				      "Track/TrackVertexMonitor/twoprong IP chi2 per dof"],
        "Two-prong doca":
				      ["Track/TrackVertexMonitor/twoprong doca",
				      "Track/TrackVertexMonitor/twoprong doca pull",
				      "Track/TrackVertexMonitor/twoprong doca vs vs eta",
				      "Track/TrackVertexMonitor/twoprong doca vs phi"],
        "Ks":
				      ["Track/TrackV0Monitor/DownstreamDownstream/pipimass",
				      "Track/TrackV0Monitor/DownstreamDownstream/chi2prob",
				      "Track/TrackV0Monitor/DownstreamDownstream/pipimassVsPhi",
				      "Track/TrackV0Monitor/LongLong/pipimass",
				      "Track/TrackV0Monitor/LongLong/chi2prob",
				      "Track/TrackV0Monitor/DownstreamDownstream/pipimassVsPhi"],
        "D0sWide":
				      ["Track/AlignD02KPiWideMonitor/mass",
				      "Track/AlignD02KPiWideMonitor/chi2prob",
				      "Track/AlignD02KPiWideMonitor/massVersusPhi",
				      "Track/AlignD02KPiWideMonitor/massVersusPhiMatt",
				      "Track/AlignD02KPiWideMonitor/multiplicity",
				      "Track/AlignD02KPiWideMonitor/trackchi2"],
        "D0sAlign":
				      ["Track/AlignD02KPiMonitor/mass",
				      "Track/AlignD02KPiMonitor/chi2prob",
				      "Track/AlignD02KPiWideMonitor/massVersusPhi",
				      "Track/AlignD02KPiMonitor/massVersusPhiMatt",
				      "Track/AlignD02KPiMonitor/multiplicity",
				      "Track/AlignD02KPiMonitor/trackchi2"],
        "Di-muons":
				      ["Track/TrackDiMuonMonitor/mass",
				      "Track/TrackDiMuonMonitor/chi2prob",
				      "Track/TrackDiMuonMonitor/massfine",
				      "Track/TrackDiMuonMonitor/massVersusPhiMatt",
				      "Track/TrackDiMuonMonitor/multiplicity",
				      "Track/TrackDiMuonMonitor/trackchi2"],
        "More dimuons":
				      ["Track/AlignParticleMonitor/mass",
				      "Track/AlignParticleMonitor/massVersusPhiMatt",
				      "Track/AlignParticleMonitor/massVersusPhi",
				      "Track/AlignParticleMonitor/massVersusMom",
				      "Track/AlignParticleMonitor/massVersusMomDif",
				      "Track/AlignParticleMonitor/massVersusMomAsym",
				      "Track/AlignParticleMonitor/massVersusTy"],   
        "Velo(TT)-T match":
				      ["Track/TrackFitMatchMonitor/T-TT/dx for dtx==0",
				      "Track/TrackFitMatchMonitor/T-TT/dy for dty==0",
				      "Track/TrackFitMatchMonitor/T-TT/dx pull vs tx",
				      "Track/TrackFitMatchMonitor/Velo-T/dx for dtx==0",
				      "Track/TrackFitMatchMonitor/Velo-T/dy for dty==0",
				      "Track/TrackFitMatchMonitor/Velo-T/dx pull vs tx"],
        "Velo(TT)-T match: curvature":
				      ["Track/TrackFitMatchMonitor/curvatureRatioTToLong",
				      "Track/TrackFitMatchMonitor/curvatureRatioVeloTTToLong",
				      "Track/TrackFitMatchMonitor/curvatureRatioTToLongVsQoP",
				      "Track/TrackFitMatchMonitor/curvatureRatioVeloTTToLongVsQoP"],
        "Velo-T match: position of kick":
				      ["Track/TrackFitMatchMonitor/kickZ",
				      "Track/TrackFitMatchMonitor/kickZVsX",
				      "Track/TrackFitMatchMonitor/kickDeltay"],
        "M1 track match A-side":
				      ["Track/TrackMuonMatchMonitor/resX_ASide_M1R1",
				      "Track/TrackMuonMatchMonitor/resX_ASide_M1R2",
				      "Track/TrackMuonMatchMonitor/resX_ASide_M1R3",
				      "Track/TrackMuonMatchMonitor/resX_ASide_M1R4",
				      "Track/TrackMuonMatchMonitor/resY_ASide_M1R1",
				      "Track/TrackMuonMatchMonitor/resY_ASide_M1R2",
				      "Track/TrackMuonMatchMonitor/resY_ASide_M1R3",
				      "Track/TrackMuonMatchMonitor/resY_ASide_M1R4"],
        "M1 track match C-side":
				      ["Track/TrackMuonMatchMonitor/resX_CSide_M1R1",
				      "Track/TrackMuonMatchMonitor/resX_CSide_M1R2",
				      "Track/TrackMuonMatchMonitor/resX_CSide_M1R3",
				      "Track/TrackMuonMatchMonitor/resX_CSide_M1R4",
				      "Track/TrackMuonMatchMonitor/resY_CSide_M1R1",
				      "Track/TrackMuonMatchMonitor/resY_CSide_M1R2",
				      "Track/TrackMuonMatchMonitor/resY_CSide_M1R3",
				      "Track/TrackMuonMatchMonitor/resY_CSide_M1R4"],
        "Ecal-track match":
				      ["Track/TrackEcalMatchMonitor/dyVsTy",
				      "Track/TrackEcalMatchMonitor/dxVsTx",
				      "Track/TrackEcalMatchMonitor/dxVsX",
				      "Track/TrackEcalMatchMonitor/E over P (inner)",
				      "Track/TrackEcalMatchMonitor/E over P (middle)",
				      "Track/TrackEcalMatchMonitor/E over P (outer)"],
        "Ecal-track match A side":
				      ["Track/TrackEcalMatchMonitor/xEcal - xTRK (inner A-side)",
				      "Track/TrackEcalMatchMonitor/xEcal - xTRK (middle A-side)",
				      "Track/TrackEcalMatchMonitor/xEcal - xTRK (outer A-side)",
				      "Track/TrackEcalMatchMonitor/yEcal - yTRK (inner A-side)",
				      "Track/TrackEcalMatchMonitor/yEcal - yTRK (middle A-side)",
				      "Track/TrackEcalMatchMonitor/yEcal - yTRK (outer A-side)"],
        "Ecal-track match C side":
				      ["Track/TrackEcalMatchMonitor/xEcal - xTRK (inner C-side)",
				      "Track/TrackEcalMatchMonitor/xEcal - xTRK (middle C-side)",
				      "Track/TrackEcalMatchMonitor/xEcal - xTRK (outer C-side)",
				      "Track/TrackEcalMatchMonitor/yEcal - yTRK (inner C-side)",
				      "Track/TrackEcalMatchMonitor/yEcal - yTRK (middle C-side)",
				      "Track/TrackEcalMatchMonitor/yEcal - yTRK (outer C-side)"],
        "Velo-Ecal match":
				      ["Track/TrackEcalMatchMonitor/yEcal - yVELO (inner A-side)",
				      "Track/TrackEcalMatchMonitor/yEcal - yVELO (middle A-side)",
				      "Track/TrackEcalMatchMonitor/yEcal - yVELO (outer A-side)",
				      "Track/TrackEcalMatchMonitor/yEcal - yVELO (inner C-side)",
				      "Track/TrackEcalMatchMonitor/yEcal - yVELO (middle C-side)",
				      "Track/TrackEcalMatchMonitor/yEcal - yVELO (outer C-side)"],
        "Overlap and holes on long tracks":
   				      ["Track/TrackMonitor/Long/NumITOTOverlapStations",
   				      "Track/TrackMonitor/Long/NumITOverlapStations",
				      "Track/TrackMonitor/Long/NumVeloOverlapStations",
				      "Track/TrackMonitor/Long/NumVeloHoles"],
        "Velo overlaps":
   				      ["Track/TrackVeloOverlapMonitor/overlapResidualR",
				      "Track/TrackVeloOverlapMonitor/residualAR",
				      "Track/TrackVeloOverlapMonitor/residualCR",
				      "Track/TrackVeloOverlapMonitor/overlapResidualPhi",
				      "Track/TrackVeloOverlapMonitor/residualAPhi",
				      "Track/TrackVeloOverlapMonitor/residualCPhi"],
        "Overlap in IT1 A/C tracks":
				      ["Track/TrackITOverlapMonitor/ITASideTrack/IT1TopBox/hitres",
				      "Track/TrackITOverlapMonitor/ITCSideTrack/IT1TopBox/hitres",
				      "Track/TrackITOverlapMonitor/ITASideTrack/IT1BottomBox/hitres",
				      "Track/TrackITOverlapMonitor/ITCSideTrack/IT1BottomBox/hitres"],
        "Overlap in IT1 B/T tracks":
				      ["Track/TrackITOverlapMonitor/ITBottomTrack/IT1ASideBox/hitres",
				      "Track/TrackITOverlapMonitor/ITTopTrack/IT1ASideBox/hitres",
				      "Track/TrackITOverlapMonitor/ITBottomTrack/IT1CSideBox/hitres",
				      "Track/TrackITOverlapMonitor/ITTopTrack/IT1CSideBox/hitres"],
        "Overlap in IT2 A/C tracks":
				      ["Track/TrackITOverlapMonitor/ITASideTrack/IT2TopBox/hitres",
				      "Track/TrackITOverlapMonitor/ITCSideTrack/IT2TopBox/hitres",
				      "Track/TrackITOverlapMonitor/ITASideTrack/IT2BottomBox/hitres",
				      "Track/TrackITOverlapMonitor/ITCSideTrack/IT2BottomBox/hitres"],
        "Overlap in IT2 B/T tracks":
				      ["Track/TrackITOverlapMonitor/ITBottomTrack/IT2ASideBox/hitres",
				      "Track/TrackITOverlapMonitor/ITTopTrack/IT2ASideBox/hitres",
				      "Track/TrackITOverlapMonitor/ITBottomTrack/IT2CSideBox/hitres",
				      "Track/TrackITOverlapMonitor/ITTopTrack/IT2CSideBox/hitres"],
        "Overlap in IT3 A/C tracks":
				      ["Track/TrackITOverlapMonitor/ITASideTrack/IT3TopBox/hitres",
				      "Track/TrackITOverlapMonitor/ITCSideTrack/IT3TopBox/hitres",
				      "Track/TrackITOverlapMonitor/ITASideTrack/IT3BottomBox/hitres",
				      "Track/TrackITOverlapMonitor/ITCSideTrack/IT3BottomBox/hitres"],
        "Overlap in IT3 B/T tracks":
				      ["Track/TrackITOverlapMonitor/ITBottomTrack/IT3ASideBox/hitres",
				      "Track/TrackITOverlapMonitor/ITTopTrack/IT3ASideBox/hitres",
				      "Track/TrackITOverlapMonitor/ITBottomTrack/IT2CSideBox/hitres",
				      "Track/TrackITOverlapMonitor/ITTopTrack/IT3CSideBox/hitres"],
        "Overlap in IT with OT A-side tracks":
				      ["Track/TrackITOverlapMonitor/OTASideTrack/IT1ASideBox/hitres",
				      "Track/TrackITOverlapMonitor/OTASideTrack/IT2ASideBox/hitres",
				      "Track/TrackITOverlapMonitor/OTASideTrack/IT3ASideBox/hitres",
				      "Track/TrackITOverlapMonitor/OTASideTrack/IT1TopBox/hitres",
				      "Track/TrackITOverlapMonitor/OTASideTrack/IT2TopBox/hitres",
				      "Track/TrackITOverlapMonitor/OTASideTrack/IT3TopBox/hitres",
				      "Track/TrackITOverlapMonitor/OTASideTrack/IT1BottomBox/hitres",
				      "Track/TrackITOverlapMonitor/OTASideTrack/IT2BottomBox/hitres",
				      "Track/TrackITOverlapMonitor/OTASideTrack/IT3BottomBox/hitres"],
        "Overlap in IT with OT C-side tracks":
				      ["Track/TrackITOverlapMonitor/OTCSideTrack/IT1ASideBox/hitres",
				      "Track/TrackITOverlapMonitor/OTCSideTrack/IT2ASideBox/hitres",
				      "Track/TrackITOverlapMonitor/OTCSideTrack/IT3ASideBox/hitres",
				      "Track/TrackITOverlapMonitor/OTCSideTrack/IT1TopBox/hitres",
				      "Track/TrackITOverlapMonitor/OTCSideTrack/IT2TopBox/hitres",
				      "Track/TrackITOverlapMonitor/OTCSideTrack/IT3TopBox/hitres",
				      "Track/TrackITOverlapMonitor/OTCSideTrack/IT1BottomBox/hitres",
				      "Track/TrackITOverlapMonitor/OTCSideTrack/IT2BottomBox/hitres",
				      "Track/TrackITOverlapMonitor/OTCSideTrack/IT3BottomBox/hitres"],
        "IT overlap residuals":
				      ["Track/ITTrackMonitor/ASide/overlapResiduals",
				      "Track/ITTrackMonitor/CSide/overlapResiduals",
				      "Track/ITTrackMonitor/Top/overlapResiduals",
				      "Track/ITTrackMonitor/Bottom/overlapResiduals"],
        "TT overlap residuals":
				      ["Track/TTTrackMonitor/TTaX/Overlap residual",
				      "Track/TTTrackMonitor/TTaU/Overlap residual",
				      "Track/TTTrackMonitor/TTbV/Overlap residual",
				      "Track/TTTrackMonitor/TTbX/Overlap residual"],
        "TT residuals in overlap region":
				      ["Track/TTTrackMonitor/TTaX/Residuals in overlaps (right)",
				      "Track/TTTrackMonitor/TTaU/Residuals in overlaps (right)",
				      "Track/TTTrackMonitor/TTbV/Residuals in overlaps (right)",
				      "Track/TTTrackMonitor/TTbX/Residuals in overlaps (right)"]
                      }

if not os.path.exists('img'): os.makedirs('img')
monApp = ConfigureMonApp("img/TrackingMonitoringPlots", Pages)
for label, f in Files.iteritems(): monApp.addFile(label+':'+f)
monApp.draw(True, False)
