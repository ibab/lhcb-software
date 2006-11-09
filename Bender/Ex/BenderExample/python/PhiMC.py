#!/usr/bin/env python2.4
# =============================================================================
# $Id: PhiMC.py,v 1.1 2006-11-09 14:16:01 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revison:$
# =============================================================================
""" The simple Bender-based example plot dikaon mass peak with MC-truth"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================
## @file
#  The simple Bender-based example: plot dikaon mass peak with MC-truth
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================

## import everything form bender 
from bendermodule import * 

## @class Phi
#  simple class to cound different particles species
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class PhiMC(AlgoMC) :
    """ simple class to plot dikaon mass peak """
    
    ## standard constructor
    def __init__ ( self , name = 'PhiMC' ) :
        """ standard constructor """ 
        return AlgoMC.__init__ ( self , name )

    ## standard mehtod for analyses
    def analyse( self ) :
        """ standard method for analyses """

        finder  = self.mcFinder()
        mcphi   = finder.find ( ' phi(1020) -> K+ K- ')
        
        if mcphi.empty() : self.Warning('No MC-phi is found!')

        for i in mcphi :
            print ' MC: %s' % printDecay ( i ) 
            
        matcher = self.mcTruth ()
        mc  = MCTRUTH( matcher , mcphi ) 
        
        ## select all kaons 
        kaons = self.select( 'kaons'  , ( 'K+'  == ABSID ) & mc )
        
        self.select( "K+" , kaons , 0 < Q )
        self.select( "K-" , kaons , 0 > Q )
        
        dikaon = self.loop( "K+ K-" , "phi(1020)" )
        for phi in dikaon :
            m12 = phi.mass(1,2) / 1000 
            if 1.1 < m12  : continue
            mass = M(phi)/1000
            if 0   > mass : continue 
            self.plot ( mass , "K+K- mass            " , 1. , 1.1 ) 
            chi2 = VCHI2( phi )
            if 0 > chi2 or 49 < chi2 : continue
            self.plot ( mass , "K+K- mass 0<chi2<49  " , 1. , 1.1 )  
            if not mc ( phi ) : continue 
            self.plot ( mass , "K+K- mass mctruth    " , 1. , 1.1 ) 
           
        self.setFilterPassed( True ) 
        return SUCCESS
    
## configure the job
def configure ( **args ) :
    """ Configure the job """
    
    ## read external configruation files
    gaudi.config ( files = [
        '$DAVINCIROOT/options/DaVinciCommon.opts'         ,
        '$COMMONPARTICLESROOT/options/StandardKaons.opts'
        ] )
    
    ## I am old-fashioned person - I like HBOOK 
    if not 'HbookCnv' in gaudi.DLls : gaudi.DLLs += ['HbookCnv']
    gaudi.HistogramPersistency = "HBOOK"
    hps = gaudi.service('HistogramPersistencySvc')
    hps.OutputFile = args.get('histos','PhiMC.hbook')
    
    ## StagerSvc at CERN
    if 'CERN' == os.environ.get('CMTPATH',None) and os.environ.has_key('GaudiSiteSvcShr') :
        stager = gaudi.service('GaudiSiteSvc')
        stager.BlockSize    = 20
        stager.InitialStage =  5 
        if not 'GaudiSiteSvc' in gaudi.DLLs   : gaudi.DLLs   += [ 'GaudiSiteSvc']
        if not 'StagerSvc'    in gaudi.ExtSvc : gaudi.ExtSvc += [ 'StagerSvc'   ]
        
   ## create local algorithm:
    alg = PhiMC()
    
    gaudi.addAlgorithm( alg )
    ## add to main sequence in Davinci
    #davinci = gaudi.algorithm('DaVinciMainSeq')
    #davinci.Members += ['Phi']

    alg=gaudi.algorithm('PhiMC')
    alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]
    
    ## configure the desktop
    desktop = gaudi.tool ( 'PhiMC.PhysDesktop' )
    desktop.InputLocations = [ '/Event/Phys/StdLooseKaons' ]

    desktop.PropertiesPrint = True
    
    # add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True
    
    ## get the input data
    import data_Bs2Jpsiphi_mm as input 
    
    ## get input data 
    evtSel = gaudi.evtSel()    
    evtSel.open ( input.PFNs ) 
    evtSel.PrintFreq = 100
    
    return SUCCESS 
    
## report about what is imported
if __name__ == '__main__' :

    ## configure the job:
    configure()

    ## run the job
    gaudi.run(5000)
    

# =============================================================================
# $Log: not supported by cvs2svn $ 
# =============================================================================
# The END 
# =============================================================================
