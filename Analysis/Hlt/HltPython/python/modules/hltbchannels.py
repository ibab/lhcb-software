#! /usr/bin/env python
# =============================================================================
"""@namespace hltbchannels

@dictionaries for HLT benchmark channels by DC06 selection

@author Antonio Perez-Calero Yzquierdo aperez@ecm.ub.es

@date 2008-6-27
"""
# =============================================================================

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
# Dst Files for each sample:

dstDataCards = {
    'Bd2MuMuKstar'     : ['/d/dijkstra/Selections-DC06/Bd2KstarMuMu-lum2.dst'],
    'Bd2Jpsi2MuMuKs'   : ['/d/dijkstra/Selections-DC06/Bd2Jpsi2MuMuKS-lum2.dst'],
    'Bd2DstarMuNu'     : ['/d/dijkstra/Selections-DC06/Bd2DstarMu-lum2.dst'],
    'Bs2MuMu'          : ['/d/diegoms/A_ver_se_vai.dst'],
    'Bs2Jpsi2MuMuPhi'  : ['/d/dijkstra/Selections-DC06/Bs2JpsiPhi-lum2.dst'],
    'Bs2DsMuNu'        : [''],  #not yet!!
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
    'Bs2PhiPhi'        : ['/n/nstyles/phiphi_signal.dst'], 
    'Bs2DsPi'          : ['/d/dijkstra/Selections-DC06/Bs2Dspi-lum2.dst'],
    'Bs2DsK'           : ['/d/dijkstra/Selections-DC06/Bs2DsK-lum2.dst'],
    'Bs2DsDs'          : [''],  #not yet!!
    'Dstar2D2hhPi'     : ['/d/dijkstra/Selections-DC06/Dstar2D0Pi_D02HH-lum2.dst'],
    'Bu2eeK'           : ['/d/dijkstra/Selections-DC06/Bu2eeK-lum2.dst'],
    'Bs2PhiGamma'      : ['/l/lshchuts/phigamma1a.dst',
                          '/l/lshchuts/phigamma1b.dst',
                          '/l/lshchuts/phigamma1c.dst',
                          '/l/lshchuts/phigamma2a.dst',
                          '/l/lshchuts/phigamma2b.dst',
                          '/l/lshchuts/phigamma2c.dst',
                          '/l/lshchuts/phigamma2d.dst',
                          '/l/lshchuts/phigamma2e.dst']
    }

#---------------------------------------------------
# We need to create an option string like this
#EventSelector.Input   = {
#    "DATAFILE='file1' TYP='POOL_ROOTTREE' OPT='READ'",
#    "DATAFILE='file2' TYP='POOL_ROOTTREE' OPT='READ'",
#    ....
#    "DATAFILE='fileN' TYP='POOL_ROOTTREE' OPT='READ'"
#    };

def createOptLines(sample):
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
    'Bs2MuMu'          : [''],  #??
    'Bs2Jpsi2MuMuPhi'  : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoDC06SelBs2Jpsi2MuMuPhi2KK_lifetime_unbiased.opts',
                          '/afs/cern.ch/user/d/dijkstra/python/offline-sel/Jpsi2MuMu_forDC06selBs2Jpsi2MuMuPhi2KK_lifetime_unbiased.opts',
                          '/afs/cern.ch/user/d/dijkstra/python/offline-sel/Phi2KK_forDC06selBs2Jpsi2MuMuPhi2KK_lifetime_unbiased.opts',
                          '/afs/cern.ch/user/d/dijkstra/python/offline-sel/DC06SelBs2JpsiPhi_lifetime_unbiased.opts',
                          StandardParticles, StandardJPsi],
    'Bs2DsMuNu'        : [''],  #not yet!!
    'Z2MuMu'           : [''],  #not yet!!
    'Bd2D0Kstar'       : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoDC06SelBd2D0Kst_D02HHhans.opts',
                          StandardParticles, StandardJPsi],
    'Bd2RhoPi'         : [''],  #not yet!!

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
    'Bs2PhiPhi'        : [''], #??
    'Bs2DsPi'          : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoDC06SelBs2DsH.opts',StandardParticles, StandardJPsi],
    'Bs2DsK'           : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoDC06SelBs2DsH.opts',StandardParticles, StandardJPsi],
    'Bs2DsDs'          : [''],  #not yet!!
    'Dstar2D2hhPi'     : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DVSelDstar2D0Pi_D02HH.opts',StandardParticles, StandardJPsi],
    'Bu2eeK'           : ['/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoDC06SelBu2eeK.opts',
                          '/afs/cern.ch/user/d/dijkstra/python/offline-sel/DoPreselBu2LLK.opts',
                          '/afs/cern.ch/user/d/dijkstra/python/offline-sel/PreselBu2LLK.opts',
                          StandardParticles, StandardJPsi],
    'Bs2PhiGamma'      : [''], #??
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
    'Bu2eeK'           : '/Event/Phys/DC06SelBu2eeK/Particles',
    'Bs2PhiGamma'      : '' #??
                         
    }

#-------------------------------------------------------------
# Testing channels!
import GaudiPython

def testChannel():    
    sampleList=dstDataCards.keys()
    extraOpts = []
    DVJOB = ["$HLTSYSROOT/options/HltJob.opts"]
    NoConfig=0
    NoRun=0
    for sample in sampleList:
        gaudi = GaudiPython.AppMgr(outputlevel=4)
        DVJOB += selectionOptsFiles[sample]
        DataCardOpts = createOptLines(sample)
        extraOpts.append(DataCardOpts)
        print
        print "Configure",sample
        print
        print
        try:
            gaudi.config(files = DVJOB, options = extraOpts)
        except:
            print "Could not configure ",sample
            NoConfig+=1
        gaudi.initialize()
        print
        print "Run",sample
        print
        print
        try:
            gaudi.run(20)
        except:
            print "Could not run on",sample
            NoRun+=1
        gaudi.finalize()
        del gaudi
    print "No config",NoConfig," No Run",NoRun
    return 
