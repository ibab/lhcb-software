#! /usr/bin/env python
# =============================================================================
"""@namespace benchmarkchannels

@dictionaries for HLT benchmark channels by DC06 selection

@author Antonio PEREZ-CALERO Yzquierdo aperez@ecm.ub.es
@author Hugo RUIZ hugo.ruiz@cern.ch

@date 2008-6-27
"""
# =============================================================================

import datacard_dst06L0_mblumi2_2 as mblumi2

def configuration(channel):
    """ for a given channel returns the list of files needs for the selection,
    the extraopts to include the data cards with the DSTs, and a dictionary with some extra data information, i.e path in the TES of the selection, number of evetns etc
    """
    datacard = createOptLines(channel)
    eopts = []
    files = selectionOptsFiles[channel]
    eopts.append(datacard)
    data = {}
    data["TESPATH"] = TESPath[channel]
    return files,eopts,data


#---------------------------------------------------
# Dst Files for each selected sample DC06:

dstDataCards = {
    'Bd2MuMuKstar'     : ['/d/dijkstra/Selections-DC06/Bd2KstarMuMu-lum2.dst'],
    'Bd2Jpsi2MuMuKs'   : ['/d/dijkstra/Selections-DC06/Bd2Jpsi2MuMuKS-lum2.dst'],
    'Bd2DstarMuNu'     : ['/d/dijkstra/Selections-DC06/Bd2DstarMu-lum2.dst'],
    'Bs2MuMuSR'        : ['/d/diegoms/13112001_DC06_phys_v2_lumi2_SR.dst'], #Selected events in sensitivity region for Bs->mumu analysis
    'Bs2MuMu'          : ['/d/dijkstra/Selections-DC06/Bs2MuMu.dst'],
    'Bs2Jpsi2MuMuPhi'  : ['/d/dijkstra/Selections-DC06/Bs2JpsiPhi-lum2.dst'],
    'Bs2DsMuNu'        : ['/d/dijkstra/Selections-DC06/B02DMuX_D2KKpi.dst'],  #Common selection by R. Lambert
##     'Bs2DsMuNu'        : ['/a/aperezca/Bs2Dsmunu/Bs2DsMuNu1.dst',   # SELECTED ACCORDING TO DC04 OPTIMIZATION! Old files now.
##                           '/a/aperezca/Bs2Dsmunu/Bs2DsMuNu2.dst',
##                           '/a/aperezca/Bs2Dsmunu/Bs2DsMuNu3.dst',
##                           '/a/aperezca/Bs2Dsmunu/Bs2DsMuNu4.dst',
##                           '/a/aperezca/Bs2Dsmunu/Bs2DsMuNu5.dst',
##                           '/a/aperezca/Bs2Dsmunu/Bs2DsMuNu6.dst',
##                           '/a/aperezca/Bs2Dsmunu/Bs2DsMuNu7.dst',
##                           '/a/aperezca/Bs2Dsmunu/Bs2DsMuNu8.dst',
##                           '/a/aperezca/Bs2Dsmunu/Bs2DsMuNu9.dst',
##                           '/a/aperezca/Bs2Dsmunu/Bs2DsMuNu10.dst',
##                           '/a/aperezca/Bs2Dsmunu/Bs2DsMuNu11.dst',
##                           '/a/aperezca/Bs2Dsmunu/Bs2DsMuNu12.dst',
##                           '/a/aperezca/Bs2Dsmunu/Bs2DsMuNu13.dst'],
    'Z2MuMu'           : [''],  #not yet!!
    'Bd2D0Kstar'       : ['/d/dijkstra/Selections-DC06/Bd2D0Kst_D02HH-lum2.dst'],
    'Bd2RhoPi'         : [''],  #not yet!!
    'Bd2KPi'           : ['/d/dijkstra/Selections-DC06/B2HH-lum2.dst'], #B->hh common selection
    'Bd2PiPi'          : ['/d/dijkstra/Selections-DC06/B2HH-lum2.dst'], #B->hh common selection
    'Bd2KK'            : ['/d/dijkstra/Selections-DC06/B2HH-lum2.dst'], #B->hh common selection
    'Bs2PiPi'          : ['/d/dijkstra/Selections-DC06/B2HH-lum2.dst'], #B->hh common selection
    'Bs2PiK'           : ['/d/dijkstra/Selections-DC06/B2HH-lum2.dst'], #B->hh common selection
    'Bs2KK'            : ['/d/dijkstra/Selections-DC06/B2HH-lum2.dst'], #B->hh common selection
    'Bu2KD2KSPiPi'     : ['/d/dijkstra/Selections-DC06/Bu2KD-KSPiPi-lum2.dst'], 
    'Bs2PhiPhi'        : ['/d/dijkstra/Selections-DC06/BsPhiPhi.dst'],
    #'Bs2PhiPhi'        : ['/n/nstyles/phiphi_signal.dst'],  #Old file 
    'Bs2DsPi'          : ['/d/dijkstra/Selections-DC06/Bs2Dspi-lum2.dst'],
    'Bs2DsK'           : ['/d/dijkstra/Selections-DC06/Bs2DsK-lum2.dst'],
    'Bs2DsDs'          : [''],  #not yet!!
    'Dstar2D2hhPi'     : ['/d/dijkstra/Selections-DC06/Dstar2D0Pi_D02HH-lum2.dst'],
    'Bu2eeK'           : ['/d/dijkstra/Selections-DC06/Bu2eeK-lum2.dst'],
    'Bu2JpsiK'         : ['/d/diegoms/buJPsiK.dst'],
    'Bs2PhiGamma'      : ['/d/dijkstra/Selections-DC06/Bs2PhiGamma.dst']
##     'Bs2PhiGamma'      : ['/l/lshchuts/phigamma1a.dst',
##                           '/l/lshchuts/phigamma1b.dst',
##                           '/l/lshchuts/phigamma1c.dst',
##                           '/l/lshchuts/phigamma2a.dst',
##                           '/l/lshchuts/phigamma2b.dst',
##                           '/l/lshchuts/phigamma2c.dst',
##                           '/l/lshchuts/phigamma2d.dst',
##                           '/l/lshchuts/phigamma2e.dst']
    }

#---------------------------------------------------

def createOptLines(sample):
    """ Returns input dst data files as lines to include in options
    @param sample Standard name of the data sample
    @returns input option lines 
    @autor Antonio Perez-Calero, aperez@ecm.ub.es
    """
    dataOptline='EventSelector.Input = {'
    for ifile in dstDataCards[sample]:
        line='"'+'DATAFILE='+"'"+"PFN:castor:/castor/cern.ch/user"+ifile+"'"+" TYP="+"'"+"POOL_ROOTTREE"+"'"+" OPT="+"'"+"READ"+"'"+'",'
        dataOptline+=line
    return str(dataOptline[:-1]+'}')
        
#---------------------------------------------------
# Option files for each selection:

StandardParticles = '/afs/cern.ch/user/d/dijkstra/python/offline-sel/StandardOptions.opts'  #create intermediate particles!
StandardJPsi = '/afs/cern.ch/user/d/dijkstra/python/offline-sel/StandardJpsi.opts'          #create JPsi's!


selectionOptsFiles = {

    'Bd2MuMuKstar'     : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoDC06SelBd2KstarMuMu.opts',
                          '/afs/cern.ch/user/d/dijkstra/python/offline-sel/DVDC06SelBd2KstarMuMu.opts',
                          StandardParticles, StandardJPsi],
    'Bd2Jpsi2MuMuKs'   : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DVDC06selBd2Jpsi2MuMuKS_lifetime_unbiased.opts',
                          StandardParticles, StandardJPsi],
    'Bd2DstarMuNu'     : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoDC06SelBd2DstarMu.opts',
                          StandardParticles, StandardJPsi],
    'Bs2MuMu'          : [],  #??
    'Bs2Jpsi2MuMuPhi'  : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoDC06SelBs2Jpsi2MuMuPhi2KK_lifetime_unbiased.opts',
                          '/afs/cern.ch/user/d/dijkstra/python/offline-sel/Jpsi2MuMu_forDC06selBs2Jpsi2MuMuPhi2KK_lifetime_unbiased.opts',
                          '/afs/cern.ch/user/d/dijkstra/python/offline-sel/Phi2KK_forDC06selBs2Jpsi2MuMuPhi2KK_lifetime_unbiased.opts',
                          '/afs/cern.ch/user/d/dijkstra/python/offline-sel/DC06SelBs2JpsiPhi_lifetime_unbiased.opts',
                          StandardParticles, StandardJPsi],
    'Bs2DsMuNu'        : [],  #not yet!!
    'Z2MuMu'           : [],  #not yet!!
    'Bd2D0Kstar'       : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoDC06SelBd2D0Kst_D02HHhans.opts',
                          StandardParticles, StandardJPsi],
    'Bd2RhoPi'         : [],  #not yet!!

                                                          #B->hh common selection
    'Bd2KPi'           : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DVSelB2HH.opts',StandardParticles, StandardJPsi], 
    'Bd2PiPi'          : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DVSelB2HH.opts',StandardParticles, StandardJPsi],
    'Bd2KK'            : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DVSelB2HH.opts',StandardParticles, StandardJPsi], 
    'Bs2PiPi'          : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DVSelB2HH.opts',StandardParticles, StandardJPsi], 
    'Bs2PiK'           : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DVSelB2HH.opts',StandardParticles, StandardJPsi], 
    'Bs2KK'            : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DVSelB2HH.opts',StandardParticles, StandardJPsi], 

    'Bu2KD2KSPiPi'     : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DVDC06SelectionBu2D0K_D02KsPiPiForStripped.opts',
                          '/afs/cern.ch/user/d/dijkstra/python/offline-sel/DC06SelBu2D0K_D02KsPiPi.opts',
                          '/afs/cern.ch/user/d/dijkstra/python/offline-sel/Sel_D02KsPiPi_4Bu2D0K.opts',
                          StandardParticles, StandardJPsi],
    'Bs2PhiPhi'        : [], #??
    'Bs2DsPi'          : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoDC06SelBs2DsH.opts',StandardParticles, StandardJPsi],
    'Bs2DsK'           : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoDC06SelBs2DsH.opts',StandardParticles, StandardJPsi],
    'Bs2DsDs'          : [],  #not yet!!
    'Dstar2D2hhPi'     : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DVSelDstar2D0Pi_D02HH.opts',StandardParticles, StandardJPsi],
    'Bu2eeK'           : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoDC06SelBu2eeK.opts',
                          '/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoPreselBu2LLK.opts',
                          '/afs/cern.ch/user/d/dijkstra/python/offline-sel/PreselBu2LLK.opts',
                          StandardParticles, StandardJPsi],
    'Bu2JpsiK'         : [], #??
    'Bs2PhiGamma'      : [], #??
    }


#---------------------------------------------------
# TES path for B's from each selection:
TESPath = {
    'Bd2MuMuKstar'     : '/Event/Phys/DC06SelBd2KstarMuMu/Particles',
    'Bd2Jpsi2MuMuKs'   : '/Event/Phys/Bd2JpsiKS/Particles',
    'Bd2DstarMuNu'     : '/Event/Phys/DC06SelBd2DstarMu/Particles',
    'Bs2MuMu'          : '', #??
    'Bs2Jpsi2MuMuPhi'  : '/Event/Phys/DC06selBs2JpsiPhi_unbiased/Particles',
    'Bs2DsMuNu'        : '', #??
    'Z2MuMu'           : '', #??
    'Bd2D0Kstar'       : '/Event/Phys/DC06SelBd2D0Kst_D02HH/Particles',
    'Bd2RhoPi'         : '', #??
    'Bd2KPi'           : '/Event/Phys/SelB2HH/Particles',
    'Bd2PiPi'          : '/Event/Phys/SelB2HH/Particles',
    'Bd2KK'            : '/Event/Phys/SelB2HH/Particles',
    'Bs2PiPi'          : '/Event/Phys/SelB2HH/Particles',
    'Bs2PiK'           : '/Event/Phys/SelB2HH/Particles',
    'Bs2KK'            : '/Event/Phys/SelB2HH/Particles',
    'Bu2KD2KSPiPi'     : '', #??
    'Bs2PhiPhi'        : '', #??
    'Bs2DsPi'          : '/Event/Phys/DC06SelBs2DsH/Particles',
    'Bs2DsK'           : '/Event/Phys/DC06SelBs2DsH/Particles',
    'Bs2DsDs'          : '', #??
    'Dstar2D2hhPi'     : '/Event/Phys/SelDstar2D0Pi_D02HH/Particles',
    'Bu2JpsiK'         : '', #??
    'Bu2eeK'           : '/Event/Phys/DC06SelBu2eeK/Particles',
    'Bs2PhiGamma'      : ''  #??
                         
    }


# =============================================================================

# WARNING following functions possibly outdated!



#---------------------------------------------------
def hansVolume( sample ):
    """ Returns number of volume where Hans put the corresponding data file in his presel stripping.
    @param sample Standard name of the data sample
    @returns Volume number where Hans has the corresponding file
    @autor Hugo Ruiz, hugo.ruiz@cern.ch
    """
    filesInVol1 = ['Bd2D0Kst', 'Bd2DstMu', 'Bd2Kpi-decprodcut', 'Bd2KstD2KSPiPi', 'Bs2DsDs', 'Bs2Dspi-Decprodcut', 'Bs2MuMu', 'Bs2PhiPhi', 'Bu2KD-KSPiPi', 'Bu2Kmumu','MBL0-lumi2-1.dst','MBL0-lumi2-2.dst']
    filesInVol3 = ['Bd2MuMuKst', 'Bd2PiPiPi', 'Bs2phigamma', 'Bs2psiphi-decprodcut', 'Bu2Kee','MBL0-lumi2-3.dst','MBL0-lumi2-4.dst']

    if hansFileDict[sample] in filesInVol1: return 1
    elif hansFileDict[sample] in filesInVol3: return 3
    else:
        print 'SAMPLE IS NOT IN HANS PRESELECTIONS'
        raise RuntimeError
    return
#---------------------------------------------------
# Conversion to Hans preselection names
hansFileDict ={
    'Bd2DstarMuNu'    : 'Bd2DstMu',
    'Bd2D0Kstar'      : 'Bd2D0Kst',
    'Bs2MuMu'         : 'Bs2MuMu',
    'Bd2KPi'          : 'Bd2Kpi-decprodcut',
    'Bd2KstarD2KSPiPi': 'Bd2KstD2KSPiPi',
    'Bs2DsDs'         : 'Bs2DsDs',
    'Bs2DsPi'         : 'Bs2Dspi-Decprodcut',
    'Bs2PhiPhi'       : 'Bs2PhiPhi',
    'Bu2KD2KSPiPi'    : 'Bu2KD-KSPiPi',
    'Bu2KMuMu'        : 'Bu2Kmumu',
    'Bd2MuMuKstar'    : 'Bd2MuMuKst',
    'Bd2PiPiPi0'      : 'Bd2PiPiPi',
    'Bs2PhiGamma'     : 'Bs2phigamma',
    'Bs2PsiPhi'       : 'Bs2psiphi-decprodcut',
    'Bu2Kee'          : 'Bu2Kee'
}


#---------------------------------------------------
def optionForHansFile ( sample ):
    """ Returns an option including in the event selector the  corresponding data file in Hans's presel stripping.
    @param sample Standard name of the data sample
    @returns An option including in the event selector the  corresponding data file in Hans's presel stripping
    @autor Hugo Ruiz, hugo.ruiz@cern.ch
    """
    
    out = 'EventSelector.Input   = {"DATAFILE=\'PFN:/afs/cern.ch/lhcb/group/trigger/vol'
    out += str(hansVolume( sample ))+'/dijkstra/Selections/'+hansFileDict[sample]+'-lum2.dst\' TYP=\'POOL_ROOTTREE\' OPT=\'READ\'"}'
    return out


