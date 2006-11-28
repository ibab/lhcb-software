#!/usr/bin/env python2.4
# =============================================================================
# $Id: Jets.py,v 1.2 2006-11-28 18:26:45 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
# =============================================================================
## Trivial algorithm to access to study jets
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date   2006-11-10
# =============================================================================
""" Trivial algorithm to access to study jets  """
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================

from bendermodule import * 

## Simple algorithm for jets studies
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date   2006-11-10
class Jets(AlgoMC) :
    """ Trivial algorithm to access to study jets  """
    ## standard constructor 
    def __init__ ( self , name = 'Jets' ) :
        """ standard constructor """
        return AlgoMC.__init__( self , name )
    
    ## standard initialization 
    def initialize ( self ) :
        """ standard initialization """
        sc = AlgoMC.initialize( self )
        if sc.isFailure() : return sc
        ## event counter 
        self._nev = 0 
        return sc
    
    ## major method for analysis 
    def analyse  ( self ) :
        """ major  method for analysis """
        
        self._nev += 1
        
        mccut = MCVXFUN( MCVZ ) < 300  
        mccut = mccut
        
        # select all stable MC particles, produced near collision point
        mcall = self.mcselect( 'mcall' ,
                               mccut & ( ( 'e+'    == MCABSID ) |
                                         ( 'mu+'   == MCABSID ) |
                                         ( 'K+'    == MCABSID ) |
                                         ( 'p+'    == MCABSID ) |
                                         ( 'pi+'   == MCABSID ) |
                                         ( 'gamma' == MCABSID ) ) )
        
        tup = self.nTuple ( 'MC-tuple' )
        # fill energy flow histograms for MC particles   
        for part in mcall :
            p   = part.momentum()  
            eta = p.Eta ()
            phi = p.Phi ()  
            
            tup.column ( 'pid'  , MCID    ( part )  )
            tup.column ( 'pt'   , MCPT    ( part )  )
            tup.column ( 'e'    , MCE     ( part )  )
            tup.column ( 'phi'  , phi               )
            tup.column ( 'eta'  , eta               )            
            tup.column ( 'nev'  , self._nev         )
            tup.write() 
            
        # select all recontructed particles 
        all = self.select ( 'all' , ALL )
        tup = self.nTuple ( 'RC-tuple')
        for part in all :
            p   = part.momentum()  
            eta = p.Eta ()
            phi = p.Phi ()  
            
            tup.column ( 'pid'  , ID    ( part )   )
            tup.column ( 'pt'   , PT    ( part )   )
            tup.column ( 'e'    , E     ( part )   )
            tup.column ( 'phi'  , phi              )
            tup.column ( 'eta'  , eta              )            
            tup.column ( 'nev'  , self._nev        )
            tup.write() 

        return SUCCESS
# =============================================================================
    
# =============================================================================
## configure the job 
# =============================================================================
def configure( **args ) :
    """ configure the job """ 
    
    ## read external configutation files
    gaudi.config ( files = [ 
        '$DAVINCIROOT/options/DaVinciCommon.opts'   ,
        '$LOKIJETSROOT/options/StdKtJets.opts'      ,
        '$LOKIJETSROOT/options/StdMCKtJets.opts'    , 
        '$LOKIJETSROOT/options/StdHepMCKtJets.opts' , 
        ] )
        
    ## I am old-fashioned person - I like HBOOK 
    if not 'HbookCnv' in gaudi.DLls : gaudi.DLLs += ['HbookCnv']
    gaudi.HistogramPersistency = "HBOOK"
    hps = gaudi.service('HistogramPersistencySvc')
    hps.OutputFile = args.get('histos','Jets_histos.hbook')
    
    ## StagerSvc at CERN
    if 'CERN' == os.environ.get('CMTPATH',None) and \
           os.environ.has_key('GaudiSiteSvcShr') :
        stager = gaudi.service('GaudiSiteSvc')
        stager.BlockSize    = 20
        stager.InitialStage =  5 
        if not 'GaudiSiteSvc' in gaudi.DLLs   : gaudi.DLLs   += [ 'GaudiSiteSvc']
        if not 'StagerSvc'    in gaudi.ExtSvc : gaudi.ExtSvc += [ 'StagerSvc'   ]
        
    ## create locla algorithms 
    alg = Jets()         
    gaudi.addAlgorithm( alg )

    ## configure the N-Tuples:
    ntsvc = gaudi.ntupleSvc()
    ntsvc.Output = [ "JETS DATAFILE='jets.hbook' OPT='NEW' TYP='HBOOK'" ]     
    #ntsvc.defineOutput( { 'JETS' : 'jets.hbook' } , 'HBOOK' )

    alg = gaudi.algorithm('Jets')
    alg.NTupleLUN = "JETS" ;
    
    ## configure the desktop
    desktop = gaudi.tool ( 'Jets.PhysDesktop' )
    desktop.InputLocations = [ 'Phys/StdMCKtJets' ]
    
    ## add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True

    ## get the input data
    import data_Bs2Jpsiphi_mm as input 
    
    ## get input data 
    evtSel = gaudi.evtSel()    
    evtSel.open ( input.PFNs ) 
    evtSel.PrintFreq = 10
    
    return SUCCESS
# =============================================================================


# =============================================================================
## job steering
# =============================================================================
if __name__ == '__main__' :
    """ job steering """

    ## configure the application 
    configure() 

    ## run it! 
    gaudi.run( 500 )
# =============================================================================

    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
