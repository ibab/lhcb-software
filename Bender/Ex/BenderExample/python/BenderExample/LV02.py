#!/usr/bin/env python
# =============================================================================
# $Id: LV02.py,v 1.2 2009-03-04 15:09:12 ibelyaev Exp $ 
# =============================================================================
## @file BenderExample/LV02.py
# The simple Bender-based example:
# show "Bs -> Ds pi" to "Bs -> Ds K" reflection
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The simple Bender-based example:
show 'Bs -> Ds pi' to 'Bs -> Ds K' reflection
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ "
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
    ## standard constructor
    def __init__ ( self , name = 'LV02Alg' ) :
        """
        Standard constructor
        """ 
        return AlgoMC.__init__ ( self , name )
    
    
    
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
        bsk = Trees.MCExclusive ( Nodes.CC('B_s0')  )
        bsk += mark ( ds ) 
        bsk += Nodes.CC("K+") 
        
        st =  bsk.validate ( self.ppSvc() )
        if st.isFailure()  : return st

        ## B_s0 -> Ds+/Ds- pi-/pi+
        bspi = Trees.MCExclusive ( Nodes.CC('B_s0')  )
        bspi += mark ( ds ) 
        bspi += Nodes.CC("pi+") 
        
        st =  bspi.validate ( self.ppSvc() )
        if st.isFailure()  : return st
        
        cutk   = MCDECTREE( bsk  ) 
        mcbsk  = self.mcselect ('mcbsk'  , cutk  )
        
        cutpi  = MCDECTREE( bspi ) 
        mcbspi = self.mcselect ('mcbspi' , cutpi )
        
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


# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
    
    from Configurables import DaVinci, HistogramPersistencySvc,EventSelector, NTupleSvc
    
    daVinci = DaVinci (
        DataType   = 'DC06' , # default  
        Simulation = True   ,
        HltType    = 'NONE' ) 
    
    HistogramPersistencySvc ( OutputFile = 'LV02_Histos.root' ) 
    
    EventSelector (
        PrintFreq = 100 , 
        Input = [
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/Bs2DsK_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/Bs2DsK_2.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/Bs2DsK_3.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/Bs2DsK_4.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/Bs2DsK_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/Bs2DsPi_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/Bs2DsPi_2.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/Bs2DsPi_3.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:/afs/cern.ch/lhcb/group/calo/ecal/vol10/DATA/Bs2DsPi_4.dst' TYP='POOL_ROOTTREE' OPT='READ'"
        ]
        )
    NTupleSvc ( Output = [ "LV02 DATAFILE='LV02_Tuples.root' TYPE='ROOT' OPT='NEW'"] )
    
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = LV02Alg()
    alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]
    ## print histos 
    alg.HistoPrint = True
    alg.NTupleLUN = 'LV02'
    
    ## configure the desktop
    desktop = gaudi.tool ( 'LV02Alg.PhysDesktop' )
    desktop.InputLocations = [
        '/Event/Phys/StdTightPions' ,
        '/Event/Phys/StdNoPIDsKaons' 
        ]
    
    ##gaudi.addAlgorithm ( alg ) 
    gaudi.setAlgorithms( [alg] )
   
    return SUCCESS 

    
# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## make printout of the own documentations 
    print __doc__

    ## configure the job:
    configure()

    ## run the job
    run(1000)
    

# =============================================================================
# The END 
# =============================================================================
