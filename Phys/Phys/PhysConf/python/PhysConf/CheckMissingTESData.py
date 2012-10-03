
def checkForMissingData():

    from Configurables import GaudiSequencer, TESCheck

    from Configurables import DataOnDemandSvc
    dod = DataOnDemandSvc()
    dod.Dump = True
        
    trigEvTES = "Trigger/RawEvent"
    from Configurables import RawEventSelectiveCopy 
    trigRawBankCopy = RawEventSelectiveCopy('CopyTriggerRawEvent') 
    trigRawBankCopy.RawBanksToCopy =[ 'ODIN',
                                      'HltSelReports',
                                      'HltDecReports',
                                      'HltRoutingBits',
                                      'HltVertexReports',
                                      'L0Calo',
                                      'L0CaloFull',
                                      'L0DU',
                                      'L0Muon',
                                      'L0MuonProcCand',
                                      'L0PU'
                                      ] 
    trigRawBankCopy.OutputRawEventLocation = trigEvTES
  
    dod.AlgMap [ trigEvTES ] = trigRawBankCopy

    recSumTES = "Rec/Summary"
    from Configurables import RecSummaryAlg
    summary = RecSummaryAlg("CreateRecSummary")

    dod.AlgMap [ recSumTES ] = summary
