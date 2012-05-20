#!/usr/bin/env python
# =============================================================================
# $Id$ 
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
#  with the smear campaign of Dr.O.Callot et al.: 
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
with the smear campaign of Dr.O.Callot et al.: 
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
        
        mcbsk  = self.mcselect ('mcbsk'  , "[ Beauty -> ( D_s+ --> K- K+ pi+ ) K- ]CC")        
        mcbspi = self.mcselect ('mcbspi' , "[ Beauty -> ( D_s+ --> K- K+ pi+ ) pi-]CC")
        
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
def configure ( datafiles , catalogs = [] , castor = False ) :
    """
    Configure the job
    """
    
    ##
    ## Static Configuration (``Configurables'')
    ##
    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType      = '2010' , 
        Simulation    = True   ,
        HistogramFile = 'LV02_Histos.root' , 
        TupleFile     = 'LV02_Tuples.root' 
        ) 
    
    from StandardParticles import StdTightPions , StdNoPIDsKaons
    InputParticles = [
        StdTightPions  . outputLocation () ,
        StdNoPIDsKaons . outputLocation ()
        ]

    ## define the input data 
    setData ( datafiles , catalogs , castor ) 
    
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
        ## Input particles 
        Inputs     = InputParticles 
        )
    
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
    
    
    ## configure the job:
    inputdata1 = [
        '/lhcb/MC/MC10/ALLSTREAMS.DST/00008506/0000/00008506_00000%03d_1.allstreams.dst' % i for i in range ( 2 , 29 ) 
        ]
    inputdata2 = [
        '/lhcb/MC/MC10/ALLSTREAMS.DST/00008566/0000/00008566_00000%03d_1.allstreams.dst' % i for i in range ( 1 , 83 ) 
        ]
    
    inputdata = []
    for i in range ( 1 , min ( len ( inputdata1 ) , len ( inputdata2 ) ) ) :
        inputdata += [ inputdata1[i] ]
        inputdata += [ inputdata2[i] ]

        
    configure( inputdata , castor = True )
    
    ## run the job
    run(1000)
    
# =============================================================================
# The END 
# =============================================================================
