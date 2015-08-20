

__all__ = ('configureHltReportDecoding',
           'configureL0ReportDecoding',
           'configureL0FromRawBank',
           'configureL0AndHltDecoding')

from Configurables import DataOnDemandSvc, ApplicationMgr

def fixTrunk(trunk) :
    """
    Make sure trunk ends with '/'.
    Usage:
    >>> name = '/Event/Hello/World'
    >>> fixTrunk(name)
    '/Event/Hello/World/'
    >>> name = '/Event/Hello/World/'
    >>> fixTrunk(name)
    '/Event/Hello/World/'
    """
    
    trunk += '/'
    return trunk.replace('//', '/')

def trunkName(trunk) :
    return trunk.replace('/','')

def configureHltReportDecoding(trunk) :
    """
    Create HltDecReports and HltSelReports from DAQ/RawEvent banks.
    Fetches DAQ/RawEvent from trunk+DAQ/RawEvent, places reports in trunk + Hlt/DecReports and trunk + Hlt/SelReports respectively. Action is on-demand via the DataOnDemandSvs.
    """
    
    locationRoot = fixTrunk(trunk)

    name = trunkName(trunk)
 
    rawEventLoc  = locationRoot + "DAQ/RawEvent"
    #decReportLoc = locationRoot + "Hlt/DecReports"
    #selReportLoc = locationRoot + "Hlt/SelReports"
 
    ApplicationMgr().ExtSvc +=  [ DataOnDemandSvc() ]
    from DAQSys.Decoders import DecoderDB
    from DAQSys.DecoderClass import decodersForBank
    for bank in ["Sel","Dec","Vertex","Track"]:
        for d in decodersForBank(DecoderDB,"Hlt"+bank+"Reports"):
            d.overrideInputs(rawEventLoc)
            d.overrideOutputs([locationRoot+loc for loc in d.listOutputs()])

    #            
    #DataOnDemandSvc().AlgMap[selReportLoc] = selReportsDecoder
    #DataOnDemandSvc().AlgMap[decReportLoc] = decReportsDecoder

def configureL0ReportDecoding(trunk) :
    """
    Create L0DecReports and L0SelReports on-demand. Places the reports in trunk + HltLikeL0/DecReports and trunk + HltLikeL0/SelReports respectively.
    """
    
    from Configurables import L0DecReportsMaker, L0SelReportsMaker

    locationRoot = fixTrunk(trunk)
    name = trunkName(trunk)
        
    DataOnDemandSvc().AlgMap[locationRoot+"HltLikeL0/DecReports"] = L0DecReportsMaker(name = "L0DecReportsMaker"+name,
                                                                                      RootInTES = locationRoot)
    DataOnDemandSvc().AlgMap[locationRoot+"HltLikeL0/SelReports"] = L0SelReportsMaker(name = "L0SelReportsMaker"+name,
                                                                                      RootInTES = locationRoot)

def configureL0FromRawBank(trunk) :
    """
    Build L0 Muon, Calo candidates and L0DUReport from raw banks on-demand. Puts data in trunk + <standard locations>
    """
    
    from Configurables import L0Conf
    L0Conf().FullL0MuonDecoding = True
    L0Conf().EnableL0DecodingOnDemand = True

    from Configurables import ( L0MuonCandidatesFromRaw,
                                L0CaloCandidatesFromRaw,
                                L0DUFromRawAlg )

    locationRoot = fixTrunk(trunk)
    name = trunkName(trunk)
       
    DataOnDemandSvc().AlgMap[locationRoot+"Trig/L0/MuonBCSU"] = L0MuonCandidatesFromRaw(name="L0MuonFromRaw"+name,
                                                                                        RootInTES = locationRoot)
    DataOnDemandSvc().AlgMap[locationRoot+"Trig/L0/FullCalo"] = L0CaloCandidatesFromRaw(name="L0CaloFromRaw"+name, 
                                                                                        RootInTES = locationRoot)

    DataOnDemandSvc().AlgMap[locationRoot+"Trig/L0/L0DUReport"] = L0DUFromRawAlg(name="L0DUFromRaw"+name, 
                                                                                 RootInTES = locationRoot)
    
def configureL0AndHltDecoding(trunk) :

    configureHltReportDecoding(trunk)
    configureL0ReportDecoding(trunk)
    configureL0FromRawBank(trunk)
