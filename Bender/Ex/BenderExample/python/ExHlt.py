#!/usr/bin/env python
# =============================================================================
# $Id: ExHlt.py,v 1.5 2005-02-10 12:27:21 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $
# =============================================================================
# @file 
#    The first attempt to write something 'Hlt'-related 
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04
# =============================================================================
""" The first attempt to write something 'Hlt'-related  """

# import the Bender itself  
from   bendermodule  import *

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

class ExHlt(Algo):
    """
    The first attempt to write something 'Hlt'-related 
    """
    def analyse ( self ) :
        # get kaons
        
        pions  = self.select( tag = "pi" , cuts = ( 'pi+' == ABSID ) & HASTRGTR )
        muons  = self.select( tag = "mu" , cuts = ( 'mu+' == ABSID ) & HASTRGTR )
        kaons  = self.select( tag = "K"  , cuts = ( 'K+'  == ABSID ) & HASTRGTR )
        
        print ' # muons/pions/kaons = %2d/%2d/%2d ' %( len(muons),len(pions),len(kaons) ) 

        if muons.empty() : return SUCCESS
        
        mcpi    = self.mcselect( tag = 'mpi' , cuts = 'pi+' == MCABSID )
        mcK     = self.mcselect( tag = 'mK'  , cuts = 'K+'  == MCABSID )
        mcP     = self.mcselect( tag = 'mP'  , cuts = 'p+'  == MCABSID )
        mcE     = self.mcselect( tag = 'mE'  , cuts = 'e+'  == MCABSID )
        mcMu    = self.mcselect( tag = 'mM'  , cuts = 'mu+' == MCABSID )
        
        mc      = self.mctruth()
        truePi  = MCTRUTH( mc , mcpi )
        trueK   = MCTRUTH( mc , mcK  )
        trueP   = MCTRUTH( mc , mcP )
        trueE   = MCTRUTH( mc , mcE  )
        trueMu  = MCTRUTH( mc , mcMu )

        for muon in muons :
            line  = ' Muon PT: %5.2f [GeV]; ' % ( PT( muon ) / GeV, ) 
            line += ' MCTruth: Mu/pi/K/p/e %1d/%1d/%1d/%1d/%1d '
            line  = line % ( trueMu ( muon ) ,
                             truePi ( muon ) ,
                             trueK  ( muon ) ,
                             trueP  ( muon ) ,
                             trueE  ( muon ) )
            print line
            
        return SUCCESS 
    
    
# =============================================================================
# Generic job configuration 
# =============================================================================
def configure() :
    gaudi.config( files   =
                  [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ,   # general options 
                    '$BENDEREXAMPLEOPTS/PoolCatalogs.opts'  ,   # pool catalogs
                    '$LOKIEXAMPLEOPTS/Bs_phiphi_DC04.opts'  ,   # input data 
                    '$TRGSYSROOT/options/L1.opts'           ,   # L1 procession 
                    '$TRGSYSROOT/options/Hlt.opts'              # HLT configuration
                    #'$BENDEREXAMPLEOPTS/TrgTracks.opts'    ,   # HLT configuration
                    #'$TRGSYSROOT/options/TrgChecking.opts'
                    ] , # HLT configuration
                  options =
                  [ 'EcalPIDmu.OutputLevel     =   5  ' ,
                    'HcalPIDmu.OutputLevel     =   5  ' ,
                    'EcalPIDe.OutputLevel      =   5  ' ,
                    'HcalPIDe.OutputLevel      =   5  ' ,
                    'BremPIDe.OutputLevel      =   5  ' ,
                    'PrsPIDe.OutputLevel       =   5  ' ,
                    'NeutralPP2MC.OutputLevel  =   5  ' ,
                    'Hadrons.OutputLevel       =   5  ' ,
                    'EventSelector.PrintFreq   = 100  ' ] )
    
    _libs_ = ( 'TrgTools' , 'TrgVelo' , 'TrgVeloTT' ,
               'STDAQ' , 'TrgChecker' , 'PhysSelections' )
    for lib in _libs_ :
        if not lib in gaudi.DLLs : gaudi.DLLs += [ lib ]

    seq = gaudi.algorithm('TrgChecking')
    seq.MeasureTime = True
    seq = gaudi.algorithm('L1Processing')
    seq.MeasureTime = True
    seq = gaudi.algorithm('HLTPRocessing')
    seq.MeasureTime = True

    # specific job configuration 

    gaudi.addAlgorithm ( 'GaudiSequencer/Links2Tables' )
    seq = gaudi.algorithm ('Links2Tables')
    
    seq.Members = [ 'TrgTrackToContainer'             ,
                    'TrgTrack2MCParticle'             ,
                    'TrgLinks2TrgTables/Trg2MCTables' ]
    
    seq.MeasureTime = True
    
    # create analysis algorithm and add it to the list of
    ex = ExHlt('ExHlt')
    
    gaudi.addAlgorithm ( ex ) 
    
    ex        = gaudi.algorithm('ExHlt')
    ex.TRG2MC = [ 'Rec/Relations/TrgTracks2MCParticles' ]
    
    desktop = gaudi.tool('ExHlt.PhysDesktop')
    desktop.InputLocations  = [
        "Phys/Trg"      ,
        "Phys/HLTPions" ,
        "Phys/HLTMuons"
        ]
    
    return SUCCESS

# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # configure the job
    configure() 
    # run job 
    gaudi.run  ( 100 )
    evtSvc = gaudi.evtSvc()
    
    # terminate the Application Manager 
    gaudi.exit ()
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
