#!/usr/bin/env python
# =============================================================================
# $Id: StoreInspection.py,v 1.3 2005-08-01 16:04:24 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.3 $
# =============================================================================
"""
'Solution'-file for 'StoreInspection' example (Bender Tutorial)
"""
# =============================================================================
# @file
#
# "Solution"-file for 'StoreInspection' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV  belyaev@lapp.in2p3.fr'
# =============================================================================

# import everything from BENDER
from bendermodule import *

# =============================================================================
# @class GetData
# define the primitive algorithm 
# =============================================================================
class GetData(Algo):
    def analyse( self ) :

        evtSvc = self.evtSvc()

        data = evtSvc['/Event/MC']
        
        print ' perform data inspection INSIDE the algorithm scope ' 
        evtSvc.dir('Rec')
                
        return SUCCESS
# =============================================================================

# =============================================================================
# The configuration fo the job
# =============================================================================
def configure() :
    
    gaudi.config ( files = ['$DAVINCIROOT/options/DaVinci.opts' ] )
    
    # modify/update the configuration:
    
    # 1) create the algorithm
    alg = GetData( 'GetData' )
    
    # (2) replace the list of top level algorithm by
    #     new list, which contains only *THIS* algorithm
    gaudi.setAlgorithms( [ alg ] )
    
    # redefine input files 
    evtsel = gaudi.evtSel()
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
# Job steering 
# =============================================================================
if __name__ == '__main__' :
    
    # job configuration
    configure()
    
    # event loop 
    gaudi.run(5)

    # inspect the data outside the algorithm
    print ' perform data inspection OUTSIDE the algorithm scope ' 
    evtSvc  = gaudi.evtSvc()
    data    = evtSvc['Rec'] 
    evtSvc.dir( data )
    data = evtSvc['Phys/GetData/Particles']
    evtSvc.ls(data)
    
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END
# =============================================================================
