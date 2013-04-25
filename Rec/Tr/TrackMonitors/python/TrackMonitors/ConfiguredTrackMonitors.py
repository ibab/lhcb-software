from Configurables import (GaudiSequencer, TrackMonitor, TrackVertexMonitor,
                           TrackAlignMonitor,
                           TrackFitMatchMonitor,TrackV0Monitor,TrackDiMuonMonitor,
                           OTTrackMonitor, OTHitEfficiencyMonitor,OTTimeMonitor,
                           TrackCaloMatchMonitor,TrackMuonMatchMonitor,
                           TrackITOverlapMonitor,TrackVeloOverlapMonitor,
                           TTTrackMonitor, ITTrackMonitor,TrackTimingMonitor,
                           TrackPV2HalfAlignMonitor,
			   HitEffPlotter)
from Configurables import (RecSysConf, RecMoniConf, TrackSys)

def ConfiguredTrackMonitorSequence(Name = "TrackMonitorSequence",
                                   HistoPrint = False):

    # figure out detectors
    seq = GaudiSequencer(Name)
    from Configurables import LHCbApp
    if hasattr(LHCbApp(),"Detectors"):
        if LHCbApp().isPropertySet("Detectors"):
            subDets = LHCbApp().getProp("Detectors")
    seq.Members.append( TrackMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackDiMuonMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( TrackVertexMonitor(HistoPrint=HistoPrint) )

    from Configurables import TrackSys
    if TrackSys().timing():
      seq.Members.append( TrackTimingMonitor(HistoPrint=HistoPrint) )

    if not RecMoniConf().getProp("Histograms") is "Online":
        seq.Members.append( TrackV0Monitor(HistoPrint=HistoPrint) )
        seq.Members.append( TrackFitMatchMonitor(HistoPrint=HistoPrint) )
        seq.Members.append( TrackAlignMonitor(HistoPrint=HistoPrint) )
        if ("IT" in subDets):   seq.Members.append( TrackITOverlapMonitor(HistoPrint=HistoPrint) )
        if ("Velo" in subDets): seq.Members.append( TrackVeloOverlapMonitor(HistoPrint=HistoPrint) )
        if ("TT" in subDets):   seq.Members.append( TTTrackMonitor(HistoPrint=HistoPrint) )
        if ("IT" in subDets):   seq.Members.append( ITTrackMonitor(HistoPrint=HistoPrint) )
        seq.Members.append( TrackPV2HalfAlignMonitor(HistoPrint=HistoPrint) )
	if RecMoniConf().expertHistos() :
            if [det for det in ['Velo', 'TT', 'IT', 'OT'] if det in subDets]:
                seq.Members.append( HitEffPlotter(HistoPrint=HistoPrint) )
            #else:
            #    log.warning("HitEffPlotter not defined for upgrade")
        if "CALO" in RecSysConf().RecoSequence:
            if ("Ecal" in subDets):
                seq.Members.append( TrackCaloMatchMonitor("TrackEcalMatchMonitor", CaloSystem='Ecal', HistoPrint=HistoPrint) )
            #if ("Hcal" in subDets):
                #seq.Members.append(TrackCaloMatchMonitor("TrackHcalMatchMonitor", CaloSystem='Hcal', HistoPrint=HistoPrint))
            if ("Spd" in subDets):
                seq.Members.append(TrackCaloMatchMonitor("TrackSpdMatchMonitor", CaloSystem='Spd',HistoPrint=HistoPrint))
            if ("Prs" in subDets):
                seq.Members.append(TrackCaloMatchMonitor("TrackPrsMatchMonitor", CaloSystem='Prs',HistoPrint=HistoPrint))

        if "MUON" in RecSysConf().RecoSequence:
            if ("Muon" in subDets):
                seq.Members.append(TrackMuonMatchMonitor("TrackMuonMatchMonitor", HistoPrint=HistoPrint))

    return seq
    
def ConfiguredOTMonitorSequence(Name = "MoniOTSeq",
                                HistoPrint = False ):
    seq = GaudiSequencer(Name)
    if RecMoniConf().getProp("Histograms") is "Online":
    	OTTrackMonitor().Online = True
    	OTHitEfficiencyMonitor().Online = True
    
    seq.Members.append( OTTimeMonitor(HistoPrint=HistoPrint) )	    
    seq.Members.append( OTTrackMonitor(HistoPrint=HistoPrint) )
    seq.Members.append( OTHitEfficiencyMonitor(HistoPrint=HistoPrint) )

    from Configurables import TrackSys
    if TrackSys().cosmics():
        OTTrackMonitor().RawBankDecoder = 'OTMultiBXRawBankDecoder'
        OTHitEfficiencyMonitor().RawBankDecoder = 'OTMultiBXRawBankDecoder'
        OTTimeMonitor().RawBankDecoder = 'OTMultiBXRawBankDecoder'

    return seq

