from Gaudi.Configuration import*

from Configurables import ( GaudiSequencer,
                            HltRateMonitor,
                            HltCompositionMonitor,
                            HltDecReportsDecoder,
                            EventTimeMonitor,
                            MuMonitor)

def ConfiguredHltMonitorSequence( Name = "HltMonitorSequence", HistoPrint = False ):

    seq = GaudiSequencer( Name )
    # We need the decreports
    seq.Members.append( HltDecReportsDecoder() )

    # Rate Monitor
    rateMon = HltRateMonitor()
    rateMon.HistoPrint = HistoPrint
    rateMon.SecondsPerBin = 20
    seq.Members.append( rateMon )

    # Rate Monitor
    rateMon2 = HltRateMonitor("HltRateMonitorMB")
    rateMon2.HistoPrint = HistoPrint
    rateMon2.Regexes = ["Hlt1MB.*Decision"]
    rateMon2.SecondsPerBin = 20
    seq.Members.append( rateMon2 )

    # Compostion monitor
    compMon = HltCompositionMonitor()
    compMon.HistoPrint = HistoPrint
    seq.Members.append( compMon )

    # Mu Monitor 
    muMon = MuMonitor()
    muMon.HistoPrint = HistoPrint
    seq.Members.append( muMon )

    # Time Monitor 
    tMon = EventTimeMonitor()
    tMon.HistoTopDir = "HltMonitors/"
    seq.Members.append( tMon )

    return seq
