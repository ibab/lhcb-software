#!/usr/bin/env python
# =============================================================================
# $Id: RCMCSelect.py,v 1.2 2004-10-27 14:20:44 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================

# =============================================================================
# @file
#
# "Solution"-file for 'RCMCselect.py' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================

# import everything from BENDER
from bendermodule import *

# get the CONFIGURATION utilities
import benderconfig as bender

# =============================================================================
# the algorithm itself 
# =============================================================================
class RCSelect(Algo):
    def analyse( self ) :

        mc = self.mctruth( name = 'MCdecayMatch')
        
        # find all MC trees  
        mcBs  = mc.find( decay = '             [B_s0 -> ( J/psi(1S) -> mu+  mu- ) phi(1020) ]cc' )
    
        # get all MC phis from the tree :
        mcPhi = mc.find( decay = ' phi(1020) : [B_s0 -> ( J/psi(1S) -> mu+  mu- ) phi(1020) ]cc' )
        
        # get all MC psis from the tree :
        mcPsi = mc.find( decay = ' J/psi(1S) : [B_s0 -> ( J/psi(1S) -> mu+  mu- ) phi(1020) ]cc' )

        # prepare "Monte-Carlo Cuts"
        mcCutBs  = MCTRUTH( mc , mcBs )
        mcCutPhi = MCTRUTH( mc , mcPhi )
        mcCutPsi = MCTRUTH( mc , mcPsi )
                
        # select muons for J/Psi reconstruction 
        muons = self.select( tag  = "mu" ,
                             cuts = ( "mu+" == ABSID ) & ( PT > ( 0.5 * GeV ) ) )
        if muons.empty() : return SUCCESS
        
        # select kaons for Phi reconstruction
        kaons = self.select( tag = "K"  ,
                             cuts = ( "K+" == ABSID ) & ( PIDK > 0.0  ) )
        if kaons.empty() : return SUCCESS

        # delta mass cut fro J/psi  
        dm1 = ADMASS('J/psi(1S)') < ( 50 * MeV )
        
        psis = self.loop ( formula = 'mu mu' , pid = 'J/psi(1S)' )
        for psi in psis :
            #
            # use Monte-Carlo cuts
            if not mcCutPsi( psi ) : continue 
            #
            mass = psi.mass(1,2)
            if not 2.5*GeV <  mass < 3.5*GeV : continue
            # neutral combination?
            if not 0 == SUMQ( psi )          : continue 
            # check the chi2 of the vertex fit
            if not 0 <= VCHI2( psi )  < 49   : continue
            
            self.plot( title = " dimuon invariantt mass " ,
                       value = M(psi) / GeV ,
                       low   = 2.5          ,
                       high  = 3.5          ) 
            if not dm1( psi ) : continue
            psi.save( 'psi' )
            
        # delta mass cut for phi 
        dm2 = ADMASS('phi(1020)') < ( 12 * MeV )        
            
        phis = self.loop( formula = 'K K' , pid = 'phi(1020)' )
        for phi in phis :
            #
            # use Monte-Carlo cuts
            if not mcCutPhi( phi ) : continue
            #
            if phi.mass( 1 ,2 ) > 1050 * MeV : continue 
            # neutral combination ?
            if not 0 == SUMQ( phi )          : continue
            if not 0 <= VCHI2( phi ) < 49    : continue
            
            self.plot ( title = " dikaon invarinat mass " ,
                        value = M(phi) / GeV  ,
                        low   = 1.0           ,
                        high  = 1.050         )
            if not dm2( phi ) : continue
            phi.save('phi')

        # delta mass cut for Bs 
        dm3 = ADMASS('B_s0') < ( 100 * MeV )      
        bs = self.loop( formula = 'psi phi' , pid = 'B_s0' )
        for B in bs :
            #
            # use Monte-Carlo cuts
            if not mcCutBs( B ) : continue
            #
            if not 4.5*GeV <   B.mass(1,2) < 6.5*GeV  : continue
            if not 0       <=  VCHI2( B )  < 49       : continue
            
            self.plot ( title = " psi phi invarinat mass " ,
                        value = M(B) / GeV ,
                        low   = 5.0        ,
                        high  = 6.0        )
            if not dm2 ( B ) : continue
            B.save('Bs')

        Bs = self.selected('Bs')
        if not Bs.empty() : self.setFilterPassed ( TRUE )
        
        return SUCCESS 
# =============================================================================

# =============================================================================
# configure the job
# =============================================================================
def configure() :
    
    bender.config ( files = ['$BENDERTUTOROPTS/BenderTutor.opts' ] )
    
    # modify/update the configuration:
    
    # Preload all charged particles 
    g.TopAlg +=  [ 'LoKiPreLoad/Charged' ]
    
    import benderPreLoad as preload
    
    # useful configuration of LoKiPreloadAlgorithm
    preload.Charged ( Name  = 'Charged'                                , 
                      Kaons = [ "det='RICH' k-pi='-5.0' k-p='-5.0'"  ] ,
                      Pions = [ "det='RICH' pi-k='-5.0' pi-p='-5.0'" ] )
    
    # 1) create the algorithm
    alg = RCSelect( 'RCSelect' )
    
    # 2) replace the list of top level algorithm by only *THIS* algorithm
    g.TopAlg += [ 'RCSelect' ]
    
    # 3) configure algorithm
    desktop = g.property('RCSelect.PhysDesktop')
    desktop.InputLocations = [ "/Event/Phys/Charged" ]
    
    # configure the histograms:
    g.HistogramPersistency = 'HBOOK' 
    hsvc = g.service('HistogramPersistencySvc')
    hsvc.OutputFile = 'rcmc.histos'
    
    # redefine input files 
    evtsel = g.evtSel()
    evtsel.open( [ 'LFN:/lhcb/production/DC04/v1/DST/00000543_00000017_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000018_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000016_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000020_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000024_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000019_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000021_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000022_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000001_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000002_5.dst' ] ) 

    return SUCCESS
# =============================================================================

# =============================================================================
# The control flow 
# =============================================================================
if __name__ == '__main__' :

    # job configuration
    configure()

    # event loop 
    g.run(500)

    # for the interactive mode it is better to comment the last line
    g.exit()
# =============================================================================

# =============================================================================
# The END 
# =============================================================================
