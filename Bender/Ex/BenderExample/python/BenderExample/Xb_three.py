#!/usr/bin/env python
# =============================================================================
# $Id: Xb_three.py,v 1.1 2009-03-04 11:53:39 ibelyaev Exp $ 
# =============================================================================
## @file BenderExample/Xb_three.py
#  The simple Bender-based example: find recontructed Xb -> h h h candidates 
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
#  @date 2009-01-16
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
# =============================================================================
"""
The simple Bender-based example: find recontructed Xb -> h h h candidates 
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ "
# =============================================================================
## import everything form bender
import GaudiKernel.SystemOfUnits as Units 
from   Bender.Awesome            import *
import LoKiMC.trees              as Trees  
import PartProp.Nodes            as Nodes
from   LoKiCore.functions        import *
# =============================================================================
## Simple class for access MC-truth 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class Xb_three(AlgoMC) :
    """
    find recontructed Xb -> hhh candidates 
    """
    ## standard constructor
    def __init__ ( self , name = 'Xb_three' ) :
        """
        Standard constructor
        """ 
        return AlgoMC.__init__ ( self , name )
    
    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """
        
        # gen header
        header  = self.get('Gen/Header')
        evtType = header.evType()

        # Xb -> K K K 
        xb1  = Trees.MCExclusive  ( Nodes.Beauty )
        h1   = Nodes.Pid ('K+') 
        h2   = Nodes.Pid ('K-') 
        h3   = Nodes.CC  ('K+')  
        xb1 += h1 
        xb1 += h2 
        xb1 += h3 
        
        xb1 = Trees.Photos ( xb1 ) 
        st  = xb1.validate ( self.ppSvc() )
        if st.isFailure()  : return sc 
                
        # Xb -> K K pi  
        xb2  = Trees.MCExclusive  ( Nodes.Beauty )
        h1   = Nodes.Pid ('K+') 
        h2   = Nodes.Pid ('K-') 
        h3   = Nodes.CC  ('pi+')  
        xb2 += h1 
        xb2 += h2 
        xb2 += h3 
        
        xb2 = Trees.Photos ( xb2 ) 
        st  = xb2.validate ( self.ppSvc() )
        if st.isFailure()  : return sc 
        
        
        # Xb -> pi pi K   
        xb3  = Trees.MCExclusive  ( Nodes.Beauty )
        h1   = Nodes.Pid ('pi+') 
        h2   = Nodes.Pid ('pi-') 
        h3   = Nodes.CC  ('K+')  
        xb3 += h1 
        xb3 += h2 
        xb3 += h3 
        
        xb3 = Trees.Photos ( xb3 ) 
        st  = xb3.validate ( self.ppSvc() )
        if st.isFailure()  : return sc 
        
        
        # Xb -> pi pi pi  
        xb4  = Trees.MCExclusive  ( Nodes.Beauty )
        h1   = Nodes.Pid ('pi+') 
        h2   = Nodes.Pid ('pi-') 
        h3   = Nodes.CC  ('pi+')  
        xb4 += h1 
        xb4 += h2 
        xb4 += h3
        
        xb4 = Trees.Photos ( xb4 ) 
        st  = xb4.validate ( self.ppSvc() )
        if st.isFailure()  : return sc 
        
        
        # Xb -> p p K 
        xb5  = Trees.MCExclusive  ( Nodes.Beauty )
        h1   = Nodes.Pid ('p+') 
        h2   = Nodes.Pid ('p~-') 
        h3   = Nodes.CC  ('K+')  
        xb5 += h1 
        xb5 += h2 
        xb5 += h3
        
        xb5 = Trees.Photos ( xb5 ) 
        st  = xb5.validate ( self.ppSvc() )
        if st.isFailure()  : return sc 
        
        
        # Xb -> p p pi 
        xb6  = Trees.MCExclusive  ( Nodes.Beauty )
        h1   = Nodes.Pid ('p+') 
        h2   = Nodes.Pid ('p~-') 
        h3   = Nodes.CC  ('pi+')  
        xb6 += h1 
        xb6 += h2 
        xb6 += h3
        
        xb6 = Trees.Photos ( xb6 ) 
        st  = xb6.validate ( self.ppSvc() )
        if st.isFailure()  : return sc 
        

        mcx01 = NONE
        mcx02 = NONE
        mcx03 = NONE
        mcx04 = NONE
        mcx05 = NONE
        mcx05 = NONE
        mcx06 = NONE

        
        
        if 12103012 == evtType :
            mc    = self.mcTruth()
            cut01 = MCDECTREE ( xb1 )
            mcb01 = self.mcselect ( 'xb1' , cut01 )
            mcx01 = NONE if mcb01.empty() else MCTRUTH ( mc , mcb01 )
            
        if 12103032 == evtType :
            mc    = self.mcTruth()
            cut02 = MCDECTREE ( xb2 )
            mcb02 = self.mcselect ( 'xb2' , cut02 )
            mcx02 = NONE if mcb02.empty() else MCTRUTH ( mc , mcb02 )
            
        if 12103022 == evtType : 
            mc    = self.mcTruth()
            cut03 = MCDECTREE ( xb3 )
            mcb03 = self.mcselect ( 'xb3' , cut03 )
            mcx03 = NONE if mcb03.empty() else MCTRUTH ( mc , mcb03 )

        if 12103002 == evtType :
            mc    = self.mcTruth()
            cut04 = MCDECTREE ( xb4 )
            mcb04 = self.mcselect ( 'xb4' , cut04 )
            mcx04 = NONE if mcb04.empty() else MCTRUTH ( mc , mcb04 )
            
        if 12103041 == evtType :  
            mc    = self.mcTruth()
            cut05 = MCDECTREE ( xb5 )
            mcb05 = self.mcselect ( 'xb5' , cut05 )
            mcx05 = NONE if mcb05.empty() else MCTRUTH ( mc , mcb05 )

        if 12103051 == evtType :
            mc    = self.mcTruth()
            cut06 = MCDECTREE ( xb6 )
            mcb06 = self.mcselect ( 'xb6' , cut06 )
            mcx06 = NONE if mcb06.empty() else MCTRUTH ( mc , mcb06 )

            
            
        wm1  = WMASS ( 'K+'  , 'K-'  , 'K+'  )


        wm2  = WMASS ( 'K+'  , 'K-'  , 'pi+' )        
        wm21 = WMASS ( 'pi+' , 'K-'  , 'K+'  )
        wm22 = WMASS ( 'K+'  , 'pi-' , 'K-'  )
        
        wm3  = WMASS ( 'pi+' , 'pi-' , 'K+'  )
        wm31 = WMASS ( 'K+'  , 'pi-' , 'pi+' )
        wm32 = WMASS ( 'pi+' , 'K-'  , 'pi-' )
        
        wm4  = WMASS ( 'pi+' , 'pi-' , 'pi+' )

        wm5  = WMASS ( 'p+'  , 'p~-' , 'K+'  )
        wm51 = WMASS ( 'K+'  , 'p~-' , 'p+'  )
        wm52 = WMASS ( 'p+'  , 'K-'  , 'p~-' )

        wm6  = WMASS ( 'p+'  , 'p~-' , 'pi+' )
        wm61 = WMASS ( 'pi+' , 'p~-' , 'p+'  )
        wm62 = WMASS ( 'p+'  , 'pi-' , 'p~-' )

        
        mcut = 100 * Units.MeV
        
        dm1 =         ADWM ( 'B+' , wm1  ) < mcut 
        
        dm2 =         ADWM ( 'B+' , wm2  ) < mcut
        dm2 = dm2 | ( ADWM ( 'B+' , wm21 ) < mcut ) 
        dm2 = dm2 | ( ADWM ( 'B+' , wm22 ) < mcut )
        
        dm3 =         ADWM ( 'B+' , wm3  ) < mcut 
        dm3 = dm3 | ( ADWM ( 'B+' , wm31 ) < mcut )
        dm3 = dm3 | ( ADWM ( 'B+' , wm32 ) < mcut )
        
        dm4 =         ADWM ( 'B+' , wm4  ) < mcut
        
        dm5 =         ADWM ( 'B+' , wm5  ) < mcut 
        dm5 = dm5 | ( ADWM ( 'B+' , wm51 ) < mcut )
        dm5 = dm5 | ( ADWM ( 'B+' , wm52 ) < mcut )
        
        dm6 =         ADWM ( 'B+' , wm6  ) < mcut 
        dm6 = dm6 | ( ADWM ( 'B+' , wm61 ) < mcut )
        dm6 = dm6 | ( ADWM ( 'B+' , wm62 ) < mcut )

        dm = dm1 | dm2
        dm = dm  | dm3
        dm = dm  | dm4
        dm = dm  | dm5
        dm = dm  | dm6
        
        # dm =        M < 5.50 * Units.GeV       
        # dm = dm & ( M > 4.00 * Units.GeV ) 


        primaries = self.vselect( 'PVs' , ISPRIMARY )
        if primaries.empty() :
            return self.Warning('No Primary vertices are found!', SUCCESS)


        mips = MIPCHI2( primaries, self.geo() ) 
        
        if 30000000 == evtType :
            hadrons = self.select ( 'h'  , ( 'pi+' == ABSID ) &  ( mips >  9 ) )
        else :
            hadrons = self.select ( 'h'  , ( 'pi+' == ABSID ) &  ( mips > 25 ) )
            
        nh  = self.counter('#hadron')
        nh += hadrons.size()
        
        if hadrons.size() < 3 : return SUCCESS
        
        h1  = self.select ( 'h+'  , hadrons , Q > 0 ) 
        h2  = self.select ( 'h-'  , hadrons , Q < 0 )
        
        if 50 < h1.size() or 50 < h2.size() :  
            return self.Warning( ' Skip "busy"-event ' , SUCCESS )


        tup = self.nTuple ('Xb -> 3h') 
       
        xb = self.loop ( 'h+ h- h' , 'B+' )

        ok01 = False
        ok02 = False
        ok03 = False
        ok04 = False
        ok05 = False
        ok06 = False
        
        for b in xb :
            
            m123 = b.mass(1,2,3) / Units.GeV
            if not 2 < m123 < 7     : continue

            p1 = b(1)
            p2 = b(2)
            p3 = b(3)
            
            if p1.key() == p3.key() : continue
            if p2.key() == p3.key() : continue
            
            chi2 = VCHI2 ( b )
            if not 0 <=  chi2 < 9  : continue

            m = M(b) / Units.GeV 
            
            self.plot ( m , 'mass h+ h- h'     , 3 , 7 , 200 )
            if not dm ( b ) : continue
            self.plot ( m , 'mass h+ h- h ACC' , 3 , 7 , 200 )
            
            tup.column ( 'm'     ,         m   )
            tup.column ( 'vchi2' ,      chi2   )
            
            imc01 = mcx01 ( b )
            imc02 = mcx02 ( b )
            imc03 = mcx03 ( b )
            imc04 = mcx04 ( b )
            imc05 = mcx05 ( b )
            imc06 = mcx06 ( b )
            
            tup.column ( 'mc01'  , imc01 )
            tup.column ( 'mc02'  , imc02 )
            tup.column ( 'mc03'  , imc03 )
            tup.column ( 'mc04'  , imc04 )
            tup.column ( 'mc05'  , imc05 )
            tup.column ( 'mc06'  , imc06 )

            if imc01 and 12103012 == evtType : ok01 = True 
            if imc02 and 12103032 == evtType : ok02 = True 
            if imc03 and 12103022 == evtType : ok03 = True 
            if imc04 and 12103002 == evtType : ok04 = True 
            if imc05 and 12103041 == evtType : ok05 = True 
            if imc06 and 12103051 == evtType : ok06 = True 

            tup.column ( 'wm01'  , wm1  ( b ) )
            tup.column ( 'wm02'  , wm2  ( b ) )
            tup.column ( 'wm03'  , wm3  ( b ) )
            tup.column ( 'wm04'  , wm4  ( b ) )
            tup.column ( 'wm05'  , wm5  ( b ) )
            tup.column ( 'wm06'  , wm6  ( b ) )
            
            tup.column ( 'wm21'  , wm21 ( b ) )
            tup.column ( 'wm31'  , wm31 ( b ) )
            tup.column ( 'wm51'  , wm51 ( b ) )
            tup.column ( 'wm61'  , wm61 ( b ) )

            tup.column ( 'wm22'  , wm22 ( b ) )
            tup.column ( 'wm32'  , wm32 ( b ) )
            tup.column ( 'wm52'  , wm52 ( b ) )
            tup.column ( 'wm62'  , wm62 ( b ) )

            tup.column ( 'evt'   , evtType    )
            tup.write  ()
            
            b.save ('Xb') 
            
            
        if 12103012 == evtType: 
            cnt01 = self.counter ( '#acc K+ K- K'    )
            cnt01 += ok01
            
        if 12103032 == evtType :
            cnt02 = self.counter ( '#acc K+ K- pi'   )
            cnt02 += ok02

        if 12103022 == evtType :
            cnt03 = self.counter ( '#acc pi+ pi- K'  )
            cnt03 += ok03

        if 12103002 == evtType :
            cnt04 = self.counter ( '#acc pi+ pi- pi' )
            cnt04 += ok04

        if 12103041 == evtType : 
            cnt05 = self.counter ( '#acc p p K'  )
            cnt05 += ok05

        if 12103051 == evtType : 
            cnt06 = self.counter ( '#acc p p pi' )
            cnt06 += ok06            

        Xb = self.selected('Xb')
        self.setFilterPassed ( not Xb.empty()  )
            
        return SUCCESS

# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
        
    from Configurables import DaVinci, HistogramPersistencySvc , EventSelector, NTupleSvc 
    
    daVinci = DaVinci (
        # SkipEvents = 22000       ,
        DataType   = 'DC06'      , # default  
        Simulation = True        ,
        HltType    = '' ) 
    
    HistogramPersistencySvc ( OutputFile = 'Xb2hhh_Histos.root' ) 
    NTupleSvc ( Output = [ "HHH DATAFILE='Xb2hhh_Tuples.root' TYPE='ROOT' OPT='NEW'" ] )
    
    EventSelector (
        PrintFreq = 100 , 
        Input   = [
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2hhh_3K.dst'    TYP='POOL_ROOTTREE' OPT='READ'" ,
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2hhh_3pi.dst'   TYP='POOL_ROOTTREE' OPT='READ'" ,
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2hhh_piKK.dst'  TYP='POOL_ROOTTREE' OPT='READ'" ,
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2hhh_pipiK.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2hhh_ppK.dst'   TYP='POOL_ROOTTREE' OPT='READ'" ,
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2hhh_pppi.dst'  TYP='POOL_ROOTTREE' OPT='READ'"
        ]
        )

    # import L0mb

    gaudi = appMgr() 
  
    ## create local algorithm:
    alg = Xb_three( 'Xb2hhh')
    
    ##gaudi.addAlgorithm ( alg ) 
    gaudi.setAlgorithms( [alg] )
    
    alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]
    alg.NTupleLUN = "HHH"
    
    ## print histos 
    alg.HistoPrint = True
    
    ## configure the desktop
    desktop = gaudi.tool ( 'Xb2hhh.PhysDesktop' )
    desktop.InputLocations = [
        '/Event/Phys/StdNoPIDsPions' 
        ]


    return SUCCESS 
    
# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## make printout of the own documentations 
    print __doc__

    ## configure the job:
    configure()

    ## run the job
    run(5000)
    

# =============================================================================
# The END 
# =============================================================================
