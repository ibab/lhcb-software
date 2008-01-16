#! /usr/bin/env python
# =============================================================================
""" @namespace desktop
@brief Shortcuts for options for dst writting

@author Hugo Ruiz hugo.ruiz@cern.ch
@date 2008-1-15
"""
# =============================================================================


#---------------------------------------------------
def dstOpts( outputCastorFile, requireAlgs = [] ):
    """ Options to write a dst
    @param outputCastorFile Castor file name for output
    @param requireAlgs List of algorithms whose filterPass() is required to be true to write
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    options = []
    options.append('ApplicationMgr.OutStream = {"DstWriter"}')
    options.append('DstWriter.Preload  = false')
    options.append('DstWriter.Output = "DATAFILE=\'PFN:castor:'+outputCastorFile+'\' TYP=\'POOL_ROOTTREE\' OPT=\'RECREATE\'" ')
    if requireAlgs:
        quotedAlgs = map(lambda a:'"'+a+'"', requireAlgs)
        options.append('DstWriter.RequireAlgs = {'+reduce(lambda a,b:a+', '+b, quotedAlgs)+'}')

    options += dstItemOpts

    return options




#---------------------------------------------------
# List of options with items to fill in a Dst
dstItemOpts = ['DstWriter.ItemList += {"/Event/Gen#1"}', 
               'DstWriter.ItemList += {"/Event/MC#1"}', 
               'DstWriter.ItemList += {"/Event/MC/Header#1"}', 
               #'DstWriter.ItemList += {"/Event/MC/Velo#1"}', 
               #'DstWriter.ItemList += {"/Event/MC/PuVeto#1"}', 
               #'DstWriter.ItemList += {"/Event/MC/TT#1"}', 
               #'DstWriter.ItemList += {"/Event/MC/IT#1"}', 
               #'DstWriter.ItemList += {"/Event/MC/OT#1"}', 
               #'DstWriter.ItemList += {"/Event/MC/Muon#1"}', 
               #'DstWriter.ItemList += {"/Event/MC/Spd#1"}', 
               #'DstWriter.ItemList += {"/Event/MC/Prs#1"}', 
               #'DstWriter.ItemList += {"/Event/MC/Ecal#1"}', 
               #'DstWriter.ItemList += {"/Event/MC/Hcal#1"}', 
               #'DstWriter.ItemList += {"/Event/MC/Rich#1"}', 
               'DstWriter.ItemList += {"/Event/MC/DigiHeader#1"}', 
               'DstWriter.ItemList += {"/Event/MC/TrackInfo#1"}', 
               #'DstWriter.ItemList += {"/Event/MC/Particles#1"}', 
               #'DstWriter.ItemList += {"/Event/MC/Vertices#1"}', 
               'DstWriter.ItemList += {"/Event/DAQ#1"}', 
               'DstWriter.ItemList += {"/Event/DAQ/RawEvent#1"}', 
               'DstWriter.ItemList += {"/Event/DAQ/ODIN#1"}', 
##               'DstWriter.ItemList += {"/Event/PrevPrev#1"}', 
##               'DstWriter.ItemList += {"/Event/Prev#1"}', 
               'DstWriter.ItemList += {"/Event/Link#1"}', 
               'DstWriter.ItemList += {"/Event/pSim#1"}', 
               'DstWriter.ItemList += {"/Event/pSim/MCVertices#1"}', 
               'DstWriter.ItemList += {"/Event/pSim/MCParticles#1"}', 
               #
               'DstWriter.ItemList += {"/Event/Rec#1"}', 
               'DstWriter.ItemList += {"/Event/Rec/Header#1"}', 
               'DstWriter.ItemList += {"/Event/Rec/Status#1"}', 
               'DstWriter.ItemList += {"/Event/Rec/Track#1"}', 
               'DstWriter.ItemList += {"/Event/Rec/Vertex#1"}', 
               'DstWriter.ItemList += {"/Event/Rec/Rich#1"}', 
               'DstWriter.ItemList += {"/Event/Rec/Calo#1"}', 
               'DstWriter.ItemList += {"/Event/Rec/ProtoP#1"}', 
##                 'DstWriter.ItemList += {"/Event/Trig#1"}', 
##                 'DstWriter.ItemList += {"/Event/Trig/L0#1"}', 
##                 'DstWriter.ItemList += {"/Event/Trig/L0/L0DUCaloData#1"}', 
##                 'DstWriter.ItemList += {"/Event/Trig/L0/FullCalo#1"}', 
##                 'DstWriter.ItemList += {"/Event/Trig/L0/Calo#1"}', 
##                 'DstWriter.ItemList += {"/Event/Trig/L0/MuonCtrl#1"}', 
##                 'DstWriter.ItemList += {"/Event/Trig/L0/MuonBCSU#1"}', 
##                 'DstWriter.ItemList += {"/Event/Trig/L0/MuonPU#1"}', 
##                 'DstWriter.ItemList += {"/Event/Trig/L0/MuonData#1"}', 
##                 'DstWriter.ItemList += {"/Event/Trig/L0/L0DUMuonData#1"}', 
##                 'DstWriter.ItemList += {"/Event/Trig/L0/L0DUPusData#1"}', 
##                 'DstWriter.ItemList += {"/Event/Trig/L0/L0DU#1"}'
               ]

