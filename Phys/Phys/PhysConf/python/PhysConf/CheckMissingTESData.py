
def checkForMissingData(mainSeq):

    from Configurables import GaudiSequencer, TESCheck

    # Make sure Trigger RawEvent is there
    rawEvSeq = GaudiSequencer("RawEventCheckSeq")
    rawEvSeq.ReturnOK = True
    rawEvSeq.ModeOR = True
    mainSeq.Members += [ rawEvSeq ]
    trigEvTES = "Trigger/RawEvent"
    rawEvSeq.Members += [ TESCheck( "CheckTriggerRawEvent",
                                    Inputs = [trigEvTES],
                                    Stop = False ) ]
    from Configurables import RawEventSelectiveCopy 
    trigRawBankCopy = RawEventSelectiveCopy('CopyTriggerRawEvent') 
    trigRawBankCopy.RawBanksToCopy =[ 'ODIN', 
                                      'HltSelReports' , 
                                      'HltDecReports', 
                                      'L0Calo', 
                                      'L0CaloFull', 
                                      'L0DU', 
                                      'L0Muon', 
                                      'L0MuonProcCand', 
                                      'L0PU' 
                                      ] 
    trigRawBankCopy.OutputRawEventLocation = trigEvTES
    rawEvSeq.Members += [ trigRawBankCopy ]

    # Make sure RecSummary is there
    recSumSeq = GaudiSequencer("RecSummaryCheckSeq")
    recSumSeq.ReturnOK = True
    recSumSeq.ModeOR = True
    mainSeq.Members += [ recSumSeq ]
    recSumTES = "Rec/Summary"
    recSumSeq.Members += [ TESCheck( "CheckRecSummary",
                                     Inputs = [recSumTES],
                                     Stop = False ) ]
    from Configurables import RecSummaryAlg
    summary = RecSummaryAlg("CreateRecSummary")
    recSumSeq.Members += [summary]
    
