#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
# $URL$
# =============================================================================
## The simple Bender-script to check generator 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @date 2009-05-18
#  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
#
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
The simple Bender-script to check generator 

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2009-05-18 "
__version__ = " Version $Revision$ "
# =============================================================================
## import everything from bender
import ROOT 
from   Bender.All                    import *
from   GaudiKernel.SystemOfUnits     import GeV
from   GaudiKernel.PhysicalConstants import c_light
from   LoKiCore.functions            import * 
import LoKiGen.HepMC 

# =============================================================================
## Simple class to check geenrator 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-05-15
class CheckGen(AlgoMC) :
    """
    Simple class to process MicorDST data 
    """
    
    ## standard method for analyses
    def analyse ( self ) :
        """
        Standard method for analyses
        """
        
        ## get all B0 particles
        bs1 = self.gselect ( 'bs1' ,  "[ [B0]cc => ( D+ => K- pi+ pi+ ) pi-]CC ")
        bs2 = self.gselect ( 'bs2' ,  "[ [B0]cc -> ( D+ -> K- pi+ pi+ ) pi-]CC ")

        cnt   = self.counter("#1 + photos ")
        cnt  += bs1.size()
        
        cnt   = self.counter("#2 - photos ")
        cnt  += bs2.size()

        if len(bs1) != len(bs2) :
            self.Warning(" FOUND!!!!" , SUCCESS )
            for b in bs1:
                print ' With    PHOTOS: ', b.decay() , b.barcode()
            for b in bs2:
                print ' Without PHOTOS: ', b.decay() , b.barcode()
                
            
        return SUCCESS                                           # RETURN 

   
# =============================================================================
## configure the job
def configure ( datafiles , catalogs = [] ) :
    """
    Configure the job
    """
    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType   = 'MC09', 
        Simulation = True 
        ) 
    
    from Configurables import HistogramPersistencySvc
    HistogramPersistencySvc ( OutputFile = 'muDst_Histos_.root') 
    
    from Configurables import NTupleSvc
    NTupleSvc ( Output = [
        "MUDST DATAFILE='muDst_Tuples.root' TYPE='ROOT' OPT='NEW'" ]
                )
    
    ## define/set the input data 
    setData ( datafiles , catalogs )
    
    ##
    ## jump into the world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr()
    
    
    ## create local algorithm:
    
    alg = CheckGen (
        'Check'                      ,
        PropertiesPrint  = True      , 
        HistoPrint       = True      ,
        NTupleLUN        = "CHECK"   ,
        PP2MCs           = [] 
        )
    
    ## finally inform Application Manager about our algorithm
    gaudi.setAlgorithms( [
        alg 
        ] )
    
    return SUCCESS 
    
# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120
    
    ## configure the job:
    inputdata = [
        '/castor/cern.ch/grid/lhcb/MC/2010/DST/00006645/0000/00006645_000000' + ( '%02d_1.dst') % f for f in range ( 1 , 28 )
        ] 
    configure ( inputdata ) 
    
    ## run the job
    run (5000)

    
# =============================================================================
# The END 
# =============================================================================

