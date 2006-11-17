#!/usr/bin/env python2.4
# =============================================================================
# $Id: RCSelect.py,v 1.8 2006-11-17 11:59:47 ibelyaev Exp $
# =============================================================================
# CVS tag     $Name: not supported by cvs2svn $ , version $Revision: 1.8 $
# =============================================================================
""" 'Solution'-file for 'RCselect.py' example (Bender Tutorial) """ 
# =============================================================================
# @file
#
# "Solution"-file for 'RCselect.py' example (Bender Tutorial)
#
# @author Vanya BELYAEV ibelyaev@physics.syr.edu
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV  ibelyaev@physics.syr.edu'
# =============================================================================


## import everything from BENDER
from bendermodule import *

# =============================================================================
## @class RCselect
#  simple demo-algorithm 
class RCSelect(Algo):
    """  simple demo-algorithm """

    ## the main analysis algorithm 
    def analyse( self ) :
        """ the main analysis algorithm """
        
        ## select muons for J/Psi reconstruction 
        muons = self.select( "mu" , ( "mu+" == ABSID ) & ( PT > 500  ) )
        if muons.empty() : return SUCCESS
        
        ## select kaons for Phi reconstruction
        kaons = self.select( "K"  , ( "K+" == ABSID ) & ( PIDK > 0.0  ) )
        if kaons.empty() : return SUCCESS

        ## delta mass cut fro J/psi  
        dmPsi = ADMASS('J/psi(1S)') < 50
        
        psis = self.loop ( 'mu mu' , 'J/psi(1S)' )
        for psi in psis :
            m = psi.mass(1,2) / 1000 
            if not 2.5 < m < 3.5     : continue
            # neutral combination?
            if not 0 == SUMQ( psi )  : continue 
            # check the chi2 of the vertex fit
            if not 0 <= VCHI2( psi ) < 49 : continue
            
            self.plot( M ( psi ) / 1000  ,
                       " dimuon invariant mass " ,
                       2.5 , 3.5 )

            if not dmPsi( psi ) : continue
            psi.save( 'psi' )
            
        # delta mass cut for phi 
        dmPhi = ADMASS('phi(1020)') < 20 
            
        phis = self.loop( 'K K' , 'phi(1020)' )
        for phi in phis :
            if phi.mass( 1 ,2 ) > 1050     : continue 
            # neutral combination ?
            if not 0 == SUMQ ( phi )       : continue
            # check the chi2 of the vertex fit
            if not 0 <= VCHI2( phi ) < 49 : continue
            
            self.plot ( M(phi) / 1000 ,
                        " dikaon invariant mass " ,
                        1.0 , 1.050 )

            if not dmPhi( phi ) : continue
            phi.save('phi')

        # delta mass cut for Bs 
        dmBs = ADMASS('B_s0') < 100      
        bs = self.loop( 'psi phi' , 'B_s0' )
        for B in bs :
            mass = B.mass(1,2) / 1000 
            if not 4.5 < mass < 6.5      : continue
            if not 0 <= VCHI2( B ) <  49 : continue
            
            self.plot ( M(B) / 1000 ,
                        " psi phi invarinat mass " ,
                        5.0 , 6.0 )
            
            if not dmBs ( B ) : continue
            B.save('Bs')
            
        Bs = self.selected('Bs')
        if not Bs.empty() : self.setFilterPassed ( True )
        
        return SUCCESS 
    
# =============================================================================
## Job configuration:
def configure() :
    """ Job configuration: """ 
    
    gaudi.config ( files = [
        '$DAVINCIROOT/options/DaVinciCommon.opts' ,
        '$COMMONPARTICLESROOT/options/StandardKaons.opts' ,
        '$COMMONPARTICLESROOT/options/StandardMuons.opts' 
        ] )
    
    # modify/update the configuration:
    

    
    # 1) create the algorithm
    alg = RCSelect( 'RCSelect' )
    
    # 2) add the algorithm to the list of top-level algorithms 
    gaudi.addAlgorithm( alg ) 
    
    # 3) configure algorithm
    desktop = gaudi.tool('RCSelect.PhysDesktop')
    desktop.InputLocations = [
        'Phys/StdLooseKaons' , 
        'Phys/StdLooseMuons'
        ]
    
    ## configure the histograms:
    if 'HbookCnv' not in gaudi.DLLs : gaudi.DLLs += ['HbookCnv']
    gaudi.HistogramPersistency = "HBOOK"
    hps = gaudi.service('HistogramPersistencySvc')
    hps.OutputFile = 'RCselect_histos.hbook'

    # add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True

    myAlg = gaudi.algorithm('RCSelect')
 
    # redefine input files 
    evtSel = gaudi.evtSel()
    evtSel.PrintFreq = 20
    import data_tutorial as data 
    evtSel.open( data.FILES ) 
    
    return SUCCESS
# =============================================================================

# =============================================================================
## Job steering 
if __name__ == '__main__' :

    ## job configuration
    configure()

    ## event loop 
    gaudi.run(500)

# =============================================================================
# $Log: not supported by cvs2svn $
# 
# =============================================================================
# The END 
# =============================================================================
