#!/usr/bin/env python
# =============================================================================
# $Id: Tools.py,v 1.1 2005-03-04 19:31:51 ibelyaev Exp $
# =============================================================================
# CVS version $Revision: 1.1 $
# =============================================================================
# CVS tag     $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# "Demo" algorithm for access to different tools 
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04
# =============================================================================

# import the Bender itself  
from   bendermodule  import *

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

class Tools(Algo):
    """
    Simple algorithm for testing extrapolators in Python/Bender environment
    """
    def analyse ( self ) :

        t2 = self.tool( ITrExtrapolator , 'TrParabolicExtrapolator' , parent=self )
        
        print t2 , type(t2), t2.name() , t2.type() 
        
        # get all tracks 
        tracks = self.get('Rec/Tr/Best')
        
        itr = 0 
        for track in tracks :
            states = track.states()
            if states.empty() : continue
            state = states[0]
            if not state      : continue
            z = state.z()
            
            if z > 0.5 * meter : continue
            
            itr += 1
            if itr > 3 : continue 

            # clone the state (prepare for extrapolation)
            state1 = state.clone()            

            print ' The state BEFORE extrapolation (type=%s) \n' % type(state1) , state1

            zNew = 1 * meter  

            # the next 2 lines causes segmentattion fault.. WHY ???
            #pid  = ParticleID(211)
            #sc = state1.extrapolate( t2 , zNew , pid  )
            
            sc = t2.propagate( state1 , zNew )

            print ' The state AFTER extrapolation (type=%s) \n' % type(state1) , state1
            
            # IMPORTANT: delete the cloned state 
            del state1
            
            
        return SUCCESS 
        

# =============================================================================
# Generic job configuration 
# =============================================================================
def configure() :
    gaudi.config( files   =
                  [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ,   # general options 
                    '$BENDEREXAMPLEOPTS/PoolCatalogs.opts'  ,   # pool catalogs
                    '$LOKIEXAMPLEOPTS/Bs_phiphi_DC04.opts'  ] , # input data 
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
    
    # specific job configuration 
    # create analysis algorithm and add it to the list of
    phi = Tools('Tools')    
    gaudi.setAlgorithms ( phi ) 
    
    # output histogram file 
    hsvc = gaudi.service( 'HistogramPersistencySvc' )
    hsvc.HistogramPersistency = "HBOOK" 
    hsvc.OutputFile = 'Tools.hbook'
    
    # add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True

    return SUCCESS

# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # configure the job
    configure() 
    # run job 
    gaudi.run  ( 10 )
    # terminate the Application Manager 
    gaudi.exit ()
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
