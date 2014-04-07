"""
A very neat little gaudirun options file script, which uses PostConfig actions to add various little pieces of info to the printout.
"""

def addTT():
    import Gaudi
    from Gaudi.Configuration import ApplicationMgr, AuditorSvc
    try:
        from Configurables import LHCbTimingAuditor as TA
        from Configurables import LHCbSequencerTimerTool as STT
    except ImportError:
        from Configurables import TimingAuditor as TA
        from Configurables import SequencerTimerTool as STT
    timer=TA("TIMER")
    timer.addTool(STT,"TIMER")
    if hasattr(timer.TIMER, "SummaryFile") or "SummaryFile" in timer.TIMER.__slots__:
        timer.TIMER.SummaryFile=Gaudi.Diff["prefix"]+".csv"
    if  'AuditorSvc' not in ApplicationMgr().ExtSvc :
        ApplicationMgr().ExtSvc.append( 'AuditorSvc' )
    #swap your timer for mine ;)
    remove=[]
    for auditor in AuditorSvc().Auditors:
        sauditor=auditor
        if type(sauditor) is not str:
            sauditor=auditor.getFullName()
        
        if "TIMER" or "TimingAuditor" in auditor:
            remove.append(auditor)
    AuditorSvc().Auditors=[a for a in AuditorSvc().Auditors if a not in remove]
    AuditorSvc().Auditors.append(timer)
    timer.Enable=True


import Gaudi
from Configurables import LHCbApp
LHCbApp().XMLSummary=Gaudi.Diff["prefix"]+".xml"

import Gaudi
from Gaudi.Configuration import appendPostConfigAction
appendPostConfigAction(addTT)

