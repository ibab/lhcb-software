#!/usr/bin/env python
# =============================================================================
# $Id: MCTrees.py,v 1.7 2006-06-06 20:03:27 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.7 $ 
# =============================================================================
""" 'Solution'-file for 'MCTrees.py' example (Bender Tutorial) """
# =============================================================================
# @file
#
# "Solution"-file for 'MCTrees.py' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV  belyaev@lapp.in2p3.fr'
# =============================================================================

# import everything from BENDER
from bendermodule import *

# =============================================================================
# The algorthmm itself 
# =============================================================================
class MCTrees( Algo ) :
    def analyse( self ) :

        # get the helper object 
        mc = self.mctruth()
        
        # find all MC trees of interest 
        trees  = mc.find(
            decay = ' [B_s0 -> ( J/psi(1S) ->  mu+  mu- ) phi(1020) ]cc' )
        
        # get all kaons from the tree :
        phis   = mc.find(
            decay = ' [B_s0 -> ( J/psi(1S) ->  mu+  mu- ) ^phi(1020) ]cc' )
        
        # get marked particles from the tree:
        mus    = mc.find(
            decay = ' [B_s0 -> ( J/psi(1S) -> ^mu+ ^mu- ) phi(1020) ]cc' )
        
        print ' found MCtrees/Phis/Mus: %s/%s/%s' % ( trees.size () ,
                                                      phis.size  () ,
                                                      mus.size   () )
        
        # fill the histogram 
        for mu in mus :
            self.plot ( title = ' PT of Muons from J/psi ' ,
                        value = MCPT( mu ) / GeV            ,
                        high  = 10                         ) 
            
        tup = self.nTuple( title = 'My N-Tuple' )
        zOrig = MCVXFUN( MCVZ )
        
        for mu in mus :
            tup.column( name = 'P'   , value = MCP ( mu ) / GeV )
            tup.column( name = 'PT'  , value = MCPT( mu ) / GeV )
            tup.column( name = 'ID'  , value = MCID( mu )       )
            tup.column( name = 'Q3'  , value = MC3Q( mu )       )
            tup.column( name = 'ZOR' , value = zOrig( mu ) / mm ) 
            tup.write() 
            
        return SUCCESS
# =============================================================================
    
# =============================================================================
# configure the job
# =============================================================================
def configure() :
    
    gaudi.config ( files = ['$DAVINCIROOT/options/DaVinciCommon.opts',
                            '$STDOPTS/Hbook.opts'] )
    
    # 1) create the algorithm
    alg = MCTrees( 'McTree' )

    # 2) replace the list of top level algorithm by only *THIS* algorithm
    gaudi.setAlgorithms ( [ alg ] ) 
    
    # configure the histograms:
    hsvc = gaudi.histoSvc()
    hsvc.setOutput( 'mctreeshistos.hbook', 'HBOOK' )

    # add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True

    # configure the N-Tuples:
    ntsvc = gaudi.nTupleSvc()
    ntsvc.defineOutput( { 'MC' : 'mytuples.hbook' } , 'HBOOK' )

    # configure my own algorithm
    myAlg = gaudi.algorithm('McTree')
    myAlg.NTupleLUN = 'MC'
    myAlg.PP2MC = []
    
    # redefine input files 
    evtsel = gaudi.evtSel()
    evtsel.PrintFreq = 10 
    # Bs -> Kpsi(mu+mu-) phi(K+K-_) data 
    evtsel.open( stream = [
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000017_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000018_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000016_5.dst' , 
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000020_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000024_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000019_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000021_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000022_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000001_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000002_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000003_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000004_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000005_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000006_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000007_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000008_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000009_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000010_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000012_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000013_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000014_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000015_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000023_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000025_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000026_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000027_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000028_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000029_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000030_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000031_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000032_5.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000543_00000033_5.dst' ] )
    

    return SUCCESS
# =============================================================================

# =============================================================================
# The control flow 
# =============================================================================
if __name__ == '__main__' :

    # job configuration
    configure()

    # event loop 
    gaudi.run(100)

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
