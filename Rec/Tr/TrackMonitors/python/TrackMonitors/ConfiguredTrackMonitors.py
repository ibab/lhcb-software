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
    seq = GaudiSequencer(Name)
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
        seq.Members.append( TrackITOverlapMonitor(HistoPrint=HistoPrint) )
        seq.Members.append( TrackVeloOverlapMonitor(HistoPrint=HistoPrint) )
        seq.Members.append( TTTrackMonitor(HistoPrint=HistoPrint) )
        seq.Members.append( ITTrackMonitor(HistoPrint=HistoPrint) )
        seq.Members.append( TrackPV2HalfAlignMonitor(HistoPrint=HistoPrint) )
	if RecMoniConf().expertHistos() : 
            seq.Members.append( HitEffPlotter(HistoPrint=HistoPrint) )

        if "CALO" in RecSysConf().RecoSequence:
            seq.Members.append( TrackCaloMatchMonitor("TrackEcalMatchMonitor", CaloSystem='Ecal', HistoPrint=HistoPrint) )
            #seq.Members.append(TrackCaloMatchMonitor("TrackHcalMatchMonitor", CaloSystem='Hcal', HistoPrint=HistoPrint))
            seq.Members.append(TrackCaloMatchMonitor("TrackSpdMatchMonitor", CaloSystem='Spd',HistoPrint=HistoPrint))
            seq.Members.append(TrackCaloMatchMonitor("TrackPrsMatchMonitor", CaloSystem='Prs',HistoPrint=HistoPrint))

        if "MUON" in RecSysConf().RecoSequence:
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

