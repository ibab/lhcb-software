#!/usr/bin/env python
# =============================================================================
# $Id: sDstar.py,v 1.1 2010-08-28 13:43:12 ibelyaev Exp $ 
# =============================================================================
## The simple Bender-script to read Strippied DSTs
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
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
#
# =============================================================================
"""
The simple Bender-script to read Stripped DSTs

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''


                    $Revision$
  Last modification $Date$
                 by $Author$

"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2009-05-18 "
__version__ = " Version $Revision: 1.1 $ "
# =============================================================================
## import everything from bender
import ROOT 
from   Bender.All                    import *
from   GaudiKernel.SystemOfUnits     import GeV, MeV , micrometer
from   GaudiKernel.PhysicalConstants import c_light
from   LoKiCore.functions            import * 
import LoKiPhys.Phys

# =============================================================================
## Simple class to perform simple action on muDST
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.n
#  @date 20010-07-15
class Dstar(Algo) :
    """
    Simple class to process MicroDST data 
    """
    
    ## standard method for analyses
    def analyse ( self ) :
        """
        Standard method for analyses
        """
        
        # get D*+ decays 
        dstars = self.select ('kpi_1' , '[D*(2010)+ -> ( D0 -> K-  pi+ ) pi+]CC' )
        
        deltaM = M - M1 
        dm     = abs ( deltaM  - 145.5 * MeV )
        
        ctau  = BPVLTIME ( 9 ) * c_light
        ctau1 = CHILD ( 1 , ctau ) 
        
        good =        ( MINTREE( 'K+'   == ABSID , PIDK ) > 0 )
        good = good & ( ctau1 > 90* micrometer )  
        
        for dstar in dstars :
            
            print dstar.decay()
            if good ( dstar ) :
                
                self.plot ( M1     ( dstar ) / GeV , 'mass Kpi'       , 1.760 , 1.960      )
                self.plot ( deltaM ( dstar ) / GeV , 'delta mass Kpi' , 0.135 , 0.155 , 20 )
                
                d0     = dstar.child(1)
                pislow = dstar.child(2) 
                
                k_d0   = d0.child(1)
                pi_d0  = d0.child(2) 
                
                print d0.decay()
                
                        
        return SUCCESS                                           # RETURN 

    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        histos = self.Histos()
        for key in histos :
            h = histos[key]
            if hasattr ( h , 'dump' ) : print h.dump(50,30,True)
        return Algo.finalize ( self )


# =============================================================================
## configure the job
def configure ( datafiles , catalogs = [] ) :
    """
    Configure the job
    """
    
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters (
        HLT_Code   = " HLT_PASS_RE ('Hlt1MBMicro.*Decision') | HLT_PASS_RE ('Hlt1.*Hadron.*Decision') " ,
        STRIP_Code = " HLT_PASS('StrippingDstarForPromptCharmDecision') " ,
        VOID_Code  = " EXISTS ('/Event/Charm') " 
        )
    
    from Configurables import DaVinci, GaudiSequencer
    daVinci = DaVinci (
        DataType        = '2010', 
        PrintFreq       = 1000  ,
        #
        DDDBtag         = "head-20101026" ,
        CondDBtag       = "head-20101112" ,
        #
        EventPreFilters = fltrs.filters('Filters') ,
        #
        Lumi = True 
        ) 
    
    from Configurables import CondDB
    CondDB( IgnoreHeartBeat = True ) 
    
    from Configurables import HistogramPersistencySvc
    HistogramPersistencySvc ( OutputFile = 'Dstar_Histos_.root') 
    
    from Configurables import NTupleSvc
    ntSvc = NTupleSvc ()
    ntSvc.Output += [ "DSTAR DATAFILE='Dstar_Tuples.root' TYPE='ROOT' OPT='NEW'" ]
    
    ## define/set the input data 
    setData ( datafiles , catalogs )
    
    ##
    ## jump into the world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr()
    
    ## create local algorithm:
    
    SELECTION = 'DstarForPromptCharm'
    #
    alg = Dstar (
        'Dstar' ,
        ##
        PropertiesPrint  = True             , 
        HistoPrint       = True             ,
        NTupleLUN        = "DSTAR"          ,
        RootInTES        =  '/Event/Charm/' , 
        InputLocations   = [ SELECTION ]
        )
    
    ## finally inform Application Manager about our algorithm
    userSeq = gaudi.algorithm ('GaudiSequencer/DaVinciUserSequence',True)    
    userSeq.Members += [ alg.name() ]

    
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

    
    ## stripping-10 CHARM-DST
    files = [
        '/castor/cern.ch/grid/lhcb/data/2010/CHARM.DST/00007954/0000/00007954_00000%03d_1.charm.dst' % i for i in range(1,700) 
        ]
    
    configure ( files  )
    
    ## run the job
    run (1000)

    
# =============================================================================
# The END 
# =============================================================================

