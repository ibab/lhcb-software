#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
# $URL$ 
# =============================================================================
## @file BenderExample/LV02.py
# The simple Bender-based example:
#    show ``Bs -> Ds pi'' to ``Bs -> Ds K'' reflection
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
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
The simple Bender-based example:
show ``Bs -> Ds pi'' to ``Bs -> Ds K'' reflection

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
__date__    = " 2006-10-12 "
__version__ = " Version $Revision$ "
# =============================================================================
## import everything form bender
import GaudiKernel.SystemOfUnits as Units 
from   Bender.Awesome            import *
import LoKiMC.trees              as Trees  
import PartProp.Nodes            as Nodes
from   LoKiCore.functions        import *
from   LoKiPhys.Phys             import *
# =============================================================================
## Simple class for access MC-truth 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class LV02Alg(AlgoMC) :
    """
    Simple class for access MC-truth 
    """
    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """
        
        # Ds+/Ds- -> K+ K- pi+/pi-
        ds = Trees.MCExclusive  ( Nodes.CC('D_s+') , 1 , True )
        ds += 'K+'
        ds += 'K-'
        ds += Nodes.CC('pi+')
        
        ## B_s0 -> Ds+/Ds- K-/K+
        bsk  = Trees.MCExclusive ( Nodes.CC('B_s0')  )
        bsk += ds 
        bsk += Nodes.CC("K+") 
        
        ## B_s0 -> ^Ds+/Ds- K-/K+
        bskm = Trees.MCExclusive ( Nodes.CC('B_s0')  )
        bskm += mark ( ds ) 
        bskm += Nodes.CC("K+") 

        st =  bsk.validate  ( self.ppSvc() )
        if st.isFailure()  : return st
        
        st =  bskm.validate ( self.ppSvc() )
        if st.isFailure()  : return st

        ## B_s0 -> Ds+/Ds- pi-/pi+
        bspi = Trees.MCExclusive ( Nodes.CC('B_s0')  )
        bspi += ds 
        bspi += Nodes.CC("pi+") 
        
        ## B_s0 -> ^Ds+/Ds- pi-/pi+
        bspim = Trees.MCExclusive ( Nodes.CC('B_s0')  )
        bspim += mark ( ds ) 
        bspim += Nodes.CC("pi+") 
        
        st =  bspi.validate ( self.ppSvc() )
        if st.isFailure()  : return st

        st =  bspim.validate ( self.ppSvc() )
        if st.isFailure()  : return st

        cutk   = MCDECTREE( bsk  ) 
        mcbsk  = self.mcselect ('mcbsk'  , ( 'B_s0' == MCABSID ) & cutk  )
        
        cutpi  = MCDECTREE( bspi ) 
        mcbspi = self.mcselect ('mcbspi' , ( 'B_s0' == MCABSID ) & cutpi )
        
        if mcbsk.empty() and mcbspi.empty() :
            return self.Warning ( 'No mc-trees are found' , SUCCESS )        
        
        mcBsK  = MCTRUTH ( self.mcTruth() , mcbsk  )        
        mcBsPi = MCTRUTH ( self.mcTruth() , mcbspi )
        
        pions = self.select ( 'pi' ,
                              ( 'pi+' == ABSID   ) &
                              ( PIDpi - PIDK > 0 ) &
                              ( mcBsK | mcBsPi   ) )

        self.select ( 'pi+' , pions , Q > 0 )
        self.select ( 'pi-' , pions , Q < 0 )
        
        kaons = self.select ( 'K' ,
                              ( 'K+' == ABSID   ) &
                              ( mcBsK | mcBsPi  ) )
        
        kplus  = self.select ( 'K+'  , kaons , ( Q > 0 ) )
        kminus = self.select ( 'K-'  , kaons , ( Q < 0 ) )
        self.select ( 'gK+' , kplus  , PIDK > 3 )
        self.select ( 'gK-' , kminus , PIDK > 3 )

        
        ## construct Ds -> K K pi
        ds = self.loop ( 'gK+ gK- pi' , 'D_s+' )
        dmds = ADMASS('D_s+') < ( 30 * Units.MeV ) 
        for d in ds :
            m123 = d.mass(1,2,3) / Units.GeV
            if not 1.8 < m123 < 2.1 : continue
            chi2 = VCHI2 ( d )
            if not 0.0 <= chi2 < 25 : continue 
            mc1 = mcBsK  ( d )
            mc2 = mcBsPi ( d )
            if mc1 or mc2 : 
                self.plot ( M(d) / Units.GeV , 'MASS Ds+' , 1.8 , 2.1 , 300 )
            else: continue
            if not dmds ( d )       : continue
            sc = d.reFit('Mass')
            if sc.isFailure()       : continue 
            pion = d(3)
            q = Q(pion)
            if   q < 0 : d.setPID( 'D_s-' )
            else       : d.setPID( 'D_s+' )
            d.save('Ds')
            
        tup = self.nTuple ( 'my N-tuple' ) 
        bs = self.loop ( 'Ds K' , 'B_s0' )
        for b in bs :
            m12 = b.mass(1,2) / Units.GeV
            if not 4.5 < m12 < 7.0  : continue
            chi2 = VCHI2 ( b )
            if not 0.0 <= chi2 < 25 : continue 
            q = SUMQ( b )
            if 0 != q               : continue 
            mc1 = mcBsK  ( b )
            mc2 = mcBsPi ( b )
            m = M(b) / Units.GeV
            if   mc1 : self.plot ( m , 'Bs0 mc1' , 5.0 , 6.2 , 120 )
            elif mc2 : self.plot ( m , 'Bs0 mc2' , 5.0 , 6.2 , 120 )
            else : continue

            tup.column ( 'm'    , m       )
            tup.column ( 'mc1'  , mc1     )
            tup.column ( 'mc2'  , mc2     )
            tup.column ( 'lv02' , LV02(b) )

            k = b( 2 )
            tup.column ( 'dllk' , PIDK(k) )
            
            tup.write()

            
            b.save('Bs')

        Bs = self.selected('Bs')
        self.setFilterPassed ( not Bs.empty()  )
            
        return SUCCESS

    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        histos = self.Histos()
        for key in histos :
            h = histos[key]
            if hasattr ( h , 'dump' ) : print h.dump(50,30,True)
        return AlgoMC.finalize ( self )


# =============================================================================
## configure the job
def configure ( datafiles , catalogs = [] ) :
    """
    Configure the job
    """
    
    ##
    ## Static Configuration (``Configurables'')
    ##
    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType   = 'DC06' , 
        Simulation = True   ) 
    
    from Configurables import HistogramPersistencySvc
    HistogramPersistencySvc ( OutputFile = 'LV02_Histos.root' ) 
    
    from Configurables import NTupleSvc
    NTupleSvc ( Output = [ "LV02 DATAFILE='LV02_Tuples.root' TYPE='ROOT' OPT='NEW'"] )
    
    ## define the input data 
    setData ( datafiles , catalogs ) 
    
    ##
    ## Dynamic Configuration: Jump into the wonderful world of GaudiPython 
    ##
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = LV02Alg(
        'LV02Alg' ,
        ## MC-links 
        PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ] ,
        ## print histos 
        HistoPrint = True   , 
        ## LUN for N-tuples 
        NTupleLUN = 'LV02'  ,
        ## Input particles 
        InputLocations = [ 'StdTightPions' , 'StdNoPIDsKaons' ]
        )
    
    ##gaudi.addAlgorithm ( alg ) 
    gaudi.setAlgorithms( [alg] )
   
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
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print '*'*120  
    
    
    ## configure the job:
    inputdata = [
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_1.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_2.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_3.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_4.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsK_5.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsPi_1.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsPi_2.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsPi_3.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Bs2DsPi_4.dst'    TYP='POOL_ROOTTREE' OPT='READ'",
        ]

    configure( inputdata ) 
    ## run the job
    run(1000)
    
# =============================================================================
# The END 
# =============================================================================
