# 
# Options needed for the stripping during the LHCb startup phase: 
#   - killing VELO non-zero-suppressed banks
#   - Redoing the PV
#
#   @author A. Poluektov
#   @date 2010-01-18
#

from Gaudi.Configuration import *

# 
# killing VELO non-zero-suppressed banks
#
def veloNZSKiller() :
    from Configurables import bankKiller
    return bankKiller('RemoveVeloFullBank',
                      BankTypes =["VeloFull","L0PUFull"])


#
# Redoing the PV
#
def redoPV() : 

    importOptions("$PATPVROOT/options/PVLoose.py")

    from Configurables import TESCheck, EventNodeKiller
    from Configurables import PatPVOffline

    PVCheck = TESCheck('PVCheck')
    PVCheck.Inputs = ['Rec/Vertex']
    eventNodeKiller = EventNodeKiller('PVkiller')
    eventNodeKiller.Nodes = ['pRec/Vertex','Rec/Vertex']
    removePV = GaudiSequencer( "RemovePV",
                         Members = [PVCheck, eventNodeKiller] )
    return GaudiSequencer( "RedoPV",
                           IgnoreFilterPassed=True, 
                           Members = [ removePV, PatPVOffline("PatPVOffline") ] ) 
