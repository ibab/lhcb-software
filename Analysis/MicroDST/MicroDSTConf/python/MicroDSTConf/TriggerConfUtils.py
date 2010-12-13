

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

def configureHltReportDecoding(trunk) :
    """
    Create HltDecReports and HltSelReports from DAQ/RawEvent banks.
    Fetches DAQ/RawEvent from trunk+DAQ/RawEvent, places reports in trunk + Hlt/DecReports and trunk + Hlt/SelReports respectively. Action is on-demand via the DataOnDemandSvs.
    """
    
    from Configurables import (HltDecReportsDecoder,
                               HltSelReportsDecoder,
                               ANNDispatchSvc)
    locationRoot = fixTrunk(trunk)
 
    rawEventLoc = locationRoot + "DAQ/RawEvent"
    decReportLoc = locationRoot + "Hlt/DecReports"
    selReportLoc = locationRoot + "Hlt/SelReports"
 
    ANNDispatchSvc().RawEventLocation = rawEventLoc
    ApplicationMgr().ExtSvc +=  [ DataOnDemandSvc(),
                                  ANNDispatchSvc() ]
 
    decReportsDecoder = HltDecReportsDecoder( InputRawEventLocation = rawEventLoc,
                                              OutputHltDecReportsLocation = decReportLoc)
    selReportsDecoder = HltSelReportsDecoder( InputRawEventLocation = rawEventLoc,
                                              OutputHltSelReportsLocation = selReportLoc,
                                              HltDecReportsLocation = decReportLoc
                                              )
 
    DataOnDemandSvc().AlgMap[selReportLoc] = selReportsDecoder
    DataOnDemandSvc().AlgMap[decReportLoc] = decReportsDecoder

def configureL0ReportDecoding(trunk) :
    """
    Create L0DecReports and L0SelReports on-demand. Places the reports in trunk + HltLikeL0/DecReports and trunk + HltLikeL0/SelReports respectively.
    """
    
    from Configurables import L0DecReportsMaker, L0SelReportsMaker

    locationRoot = fixTrunk(trunk)
    
    DataOnDemandSvc().AlgMap[locationRoot+"HltLikeL0/DecReports"] = L0DecReportsMaker(RootInTES = locationRoot)
    DataOnDemandSvc().AlgMap[locationRoot+"HltLikeL0/SelReports"] = L0SelReportsMaker(RootInTES = locationRoot)

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
    
    DataOnDemandSvc().AlgMap[locationRoot+"Trig/L0/MuonBCSU"] = L0MuonCandidatesFromRaw("L0MuonFromRaw",
                                                                                        RootInTES = locationRoot)
    DataOnDemandSvc().AlgMap[locationRoot+"Trig/L0/FullCalo"] = L0CaloCandidatesFromRaw("L0CaloFromRaw", 
                                                                                        RootInTES = locationRoot)

    DataOnDemandSvc().AlgMap[locationRoot+"Trig/L0/L0DUReport"] = L0DUFromRawAlg("L0DUFromRaw", 
                                                                                 RootInTES = locationRoot)
    
def configureL0AndHltDecoding(trunk) :

    configureHltReportDecoding(trunk)
    configureL0ReportDecoding(trunk)
    configureL0FromRawBank(trunk)
