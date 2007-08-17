#!/usr/bin/env python
# =============================================================================
## The simple Bender-based example: plot dikaon mass peak
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
#  @date 2006-10-12
# =============================================================================
""" The simple Bender-based example plot dikaon mass peak """
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================

# =============================================================================
## import everything form bender 
from Bender.Main import * 
# =============================================================================

# =============================================================================
## Simple class to plot dikaon mass peak
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class Phi(Algo) :
    """ simple class to plot dikaon mass peak """
    
    ## standard constructor
    def __init__ ( self , name = 'Phi' ) :
        """ standard constructor """ 
        return Algo.__init__ ( self , name )

    ## standard mehtod for analysis
    def analyse( self ) :
        """
        The standard method for analysis
        """

        ## select all kaons 
        kaons = self.select( 'kaons'  , 'K+'  == ABSID )
        
        self.select( "K+" , kaons , 0 < Q )
        self.select( "K-" , kaons , 0 > Q )
        
        dikaon = self.loop( "K+ K-" , "phi(1020)" )
        for phi in dikaon :
            m12 = phi.mass(1,2) / 1000 
            if 0 > m12 or 1.1 < m12 : continue
            self.plot( M(phi)/1000 , "K+K- mass " , 1. , 1.1 ) 
            chi2 = VCHI2( phi )
            if 0 > chi2 or 49 < chi2 : continue
            self.plot( M(phi)/1000 , "K+K- mass chi2<49  " , 1. , 1.1 ) 
            
        self.setFilterPassed( True ) 
        return SUCCESS
    
# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
        
    ## get the input data
    import data_Bs2Jpsiphi_mm as input 

    ## read external configuration files
    gaudi.config (
        files   = [ 
        '$DAVINCIROOT/options/DaVinciCommon.opts'           ,
        '$COMMONPARTICLESROOT/options/StandardKaons.opts' ] ,
        options = [
        "PoolDbCacheSvc.Catalog = %s "%input.catalog_CERN ]
        )
    
    ## I am very old-fashioned person - I like HBOOK
    if os.environ.has_key('HBOOKCNVROOT') :
        if not 'HbookCnv' in gaudi.DLls : gaudi.DLLs += ['HbookCnv']
        gaudi.HistogramPersistency = "HBOOK"
        hps = gaudi.service('HistogramPersistencySvc')
        hps.OutputFile = args.get('histos','PhiMC.hbook')
        ## add the printout of the histograms
        hsvc = gaudi.service( 'HbookHistSvc' )
        hsvc.PrintHistos = True
        
    ## StagerSvc at CERN
    if 'CERN' == os.environ.get('CMTSITE',None) and \
           os.environ.has_key('GAUDISITESVCROOT') :
        stager = gaudi.service('StagerSvc')
        stager.BlockSize    =  20
        stager.InitialStage =   5 
        if not 'GaudiSiteSvc' in gaudi.DLLs   : gaudi.DLLs   += [ 'GaudiSiteSvc']
        if not 'StagerSvc'    in gaudi.ExtSvc : gaudi.ExtSvc += [ 'StagerSvc'   ]    
        
    ## create local algorithm:
    alg = Phi()

    gaudi.addAlgorithm( alg )
    
    ## configure the desktop
    desktop = gaudi.tool ( 'Phi.PhysDesktop' )
    desktop.InputLocations = [ '/Event/Phys/StdLooseKaons' ]    
    
    ## get input data 
    evtSel = gaudi.evtSel()    
    evtSel.open ( input.FILEs ) 
    evtSel.PrintFreq = 100
    

    return SUCCESS 
    
# =============================================================================
## job steering 
if __name__ == '__main__' :
    
    ## make printout of the own documentations 
    print __doc__
    
    ## configure the job:
    configure( histos = 'histos.hbook' )
    
    ## run the job
    gaudi.run(5000)
    
# =============================================================================
# The END 
# =============================================================================
