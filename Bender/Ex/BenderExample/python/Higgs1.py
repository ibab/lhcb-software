#!/usr/bin/env python
# =============================================================================
# $Id: Higgs1.py,v 1.1 2005-02-16 10:14:45 ibelyaev Exp $
# =============================================================================
# CVS version $Revision: 1.1 $
# =============================================================================
# CVS tag     $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# "Demo" algorithm for Python based phi -> K+ K- "analysis"
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04
# =============================================================================
"""
Trivial algorithm to access MC truth information for Higgs->bbar
"""
# =============================================================================
__author__ = "Vanya BELYAEV belyaev@lapp.in2p3.fr"
# =============================================================================

from bendermodule import * 
import benderPreLoad as preload
import math

myAlg = None 
# =============================================================================
# Specific analysis algorithm
# =============================================================================
class Higgs1(Algo) :
    """
    The simplest algorithm toaccess MCtruth information for Higgs -> b bar
    """
    def __init__ ( self , name = 'Higgs1' ) :
        Algo.__init__( self , name )

    def initialize ( self ) :
        sc = Algo.initialize( self ) 
        hsvc = self.histoSvc() 
        self._h1  = hsvc.book( 'H' , 1          , 'MC 2D' ,
                               30  , - math.pi  , math.pi ,
                               35  , 1.5        , 5.0     )
        self._h11 = hsvc.book( 'H' , 11         , 'MC 2D' ,
                               30  , - math.pi  , math.pi ,
                               35  , 1.5        , 5.0     )
        self._h2  = hsvc.book( 'H' , 2          , 'RC 2D' ,
                               30  , - math.pi  , math.pi ,
                               35  , 1.5        , 5.0     )
        self._h21 = hsvc.book( 'H' , 21         , 'RC 2D' ,
                               30  , - math.pi  , math.pi ,
                               35  , 1.5        , 5.0     )
        self._nev = 0 
        return sc
    

    def analyse  ( self ) :
        """
        The basic method for analysis
        """

        self._nev += 1
        self._h1  .reset() 
        self._h11 .reset() 
        self._h2  .reset() 
        self._h21 .reset() 

        mc = self.mctruth()
        
        mccut = MCVXFUN( MCVZ ) < (  30 *  cm ) 
        mccut = mccut

        # select all stable MC particles, produced near collision point
        mcall = self.mcselect( tag = 'mcall' ,
                               cuts = mccut & ( ( 'e+'    == MCABSID ) |
                                                ( 'mu+'   == MCABSID ) |
                                                ( 'K+'    == MCABSID ) |
                                                ( 'p+'    == MCABSID ) |
                                                ( 'pi+'   == MCABSID ) |
                                                ( 'gamma' == MCABSID ) ) )
        
        tup = self.nTuple( title = 'MC-tuple')
        # fill energy flow histograms for MC particles   
        for part in mcall :
            p   = part.momentum()  
            eta = p.pseudoRapidity()
            phi = p.phi ()  
            self._h1.fill  ( phi , eta , MCPT( part ) / GeV ) 
            self._h11.fill ( phi , eta , MCE ( part ) / GeV ) 
            tup.column_( 'pid'  , MCID    ( part )        )
            tup.column_( 'pt'   , MCPT    ( part ) / GeV  )
            tup.column_( 'e'    , MCE     ( part ) / GeV  )
            tup.column_( 'phi'  , phi                  )
            tup.column_( 'eta'  , eta                  )            
            tup.column_( 'nev'  , self._nev            )
            prim = False 
            if not part.mother() : prim = True
            tup.column_( 'prim' , prim                 )                        
            tup.write() 
            
        # get only MC leptons 
        mcleps = self.mcselect ( tag    = 'mclep' ,
                                 source = mcall   ,
                                 cuts   = ( ( 'e+'    == MCABSID ) |
                                            ( 'mu+'   == MCABSID ) ) ) 
        mccutlep = PNONE
        for l in mcleps :
            if l.mother()                : continue
            if not 'mu+' == MCABSID( l ) : continue 
            mccutlep = mccutlep | MCTRUTH( mc , l )
            
        # select all recontructed particles 
        all = self.select ( tag = 'all' , cuts = PALL )
        self._h2.reset() 
        self._h21.reset()
        tup = self.nTuple( title = 'RC-tuple')
        for part in all :
            p   = part.momentum()  
            eta = p.pseudoRapidity()
            phi = p.phi ()  
            self._h2.fill  ( phi , eta , PT( part ) / GeV ) 
            self._h21.fill ( phi , eta , E( part  ) / GeV ) 
            tup.column_( 'pid'  , ID    ( part )        )
            tup.column_( 'pt'   , PT    ( part ) / GeV  )
            tup.column_( 'e'    , E     ( part ) / GeV  )
            tup.column_( 'pide' , PIDe  ( part )        )
            tup.column_( 'pidm' , PIDmu ( part )        )
            tup.column_( 'phi'  , phi                  )
            tup.column_( 'eta'  , eta                  )            
            tup.column_( 'nev'  , self._nev            )
            tup.column_( 'mcm'  , mccutlep( part )     )
            tup.write() 

        for lep in mcleps:  
            if lep.mother() : continue
            res = ( nameFromPID( lep.particleID() ) ,
                    MCPT( lep ) / GeV ,
                    MCE ( lep ) / GeV , 
                    lep.momentum().phi()              ,
                    lep.momentum().pseudoRapidity()   ) 
            print ' name %3s PT=%6.2f[GeV] E=%7.2f[GeV] phi=%+3.1f eta=%3.1f ' % res
                   
                                
        return SUCCESS 
# =============================================================================
# application configuration
# =============================================================================
def configure() :
    """
    Simple function to configure the application
    """
    gaudi.config( files   =
                  [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ,   # general options 
                    '$BENDEREXAMPLEOPTS/PoolCatalogs.opts'  ,   # pool catalogs
                    '$LOKIEXAMPLEOPTS/Higgs_bb_DC04.opts'   ] ) # input data
    

    # preload algorithm(s)
    gaudi.addAlgorithm ( 'GaudiSequencer/HiggsSeq' )
    seq = gaudi.algorithm('HiggsSeq')
    seq.Members = [ 'LoKiPreLoad/Charged' ,
                    'LoKiPreLoad/Photons' ]
    preload.Charged( Particles = [ 'muon'       ,
                                   'electron'   ,
                                   'kaon'       ,
                                   'proton'     ,
                                   'pion'       ] ,
                     Exclusive = True  , 
                     Electrons = [ "det='RICH'  e-pi='-2.0'" ] ,
                     Pions     = [] )
    
    desktop = gaudi.tool('Photons.PhysDesktop')
    desktop.ParticleMakerType = "PhotonParticleMaker"
    
    alg = Higgs1('Higgs1')

    global myAlg 
    myAlg = alg
    
    gaudi.addAlgorithm( alg )
    alg = gaudi.algorithm( 'Higgs1' )
    desktop = gaudi.tool( 'Higgs1.PhysDesktop' )
    desktop.InputLocations =  [ "/Event/Phys/Charged" ,
                                "/Event/Phys/Photons" ]

    # configure the N-Tuples:
    ntsvc = gaudi.nTupleSvc()
    ntsvc.defineOutput( { 'HIGGS' : 'Higgs1.hbook' } , 'HBOOK' )
    
    alg = gaudi.algorithm('Higgs1')
    alg.NTupleLUN = "HIGGS" ;
        

    evtSel = gaudi.evtSel()
    evtSel.PrintFreq = 1
     
    return SUCCESS


# =============================================================================
# Job steering
# =============================================================================
if __name__ == '__main__' :
    # configure the job
    configure() 
    # run job
    
    #from benderROOT import plotter
    
    #def next() : 
    #    gaudi.run  ( 1)
    #    plotter.plot( myAlg._h2 )

    gaudi.run(500) 
    # terminate the Application Manager 
    gaudi.exit ()


# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
