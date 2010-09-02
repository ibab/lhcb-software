from Gaudi.Configuration import*

from Configurables import ( GaudiSequencer,
                            HltRateMonitor,
                            HltCompositionMonitor,
                            HltDecReportsDecoder )

def ConfiguredHltMonitorSequence( Name = "HltMonitorSequence", HistoPrint = False ):

    seq = GaudiSequencer( Name )
    # We need the decreports
    seq.Members.append( HltDecReportsDecoder() )

    # Rate Monitor
    rateMon = HltRateMonitor()
    rateMon.HistoPrint = HistoPrint
    rateMon.SecondsPerBin = 20
    seq.Members.append( rateMon )

    # Compostion monitor
    compMon = HltCompositionMonitor()
    compMon.HistoPrint = HistoPrint
    seq.Members.append( compMon )

    return seq
