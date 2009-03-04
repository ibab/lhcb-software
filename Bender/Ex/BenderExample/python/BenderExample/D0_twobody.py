#!/usr/bin/env python
# =============================================================================
# $Id: D0_twobody.py,v 1.1 2009-03-04 11:53:39 ibelyaev Exp $ 
# =============================================================================
## @file BenedrExample/D0_twobody.py
#  The simple Bender-based example: find recontructed D0 -> hh candidates 
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
The simple Bender-based example: find recontructed D0 -> hh candidates 
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ "
# =============================================================================
## import everything from bender
import GaudiKernel.SystemOfUnits as Units 
from   Bender.Awesome            import *
import LoKiMC.trees              as Trees  
import PartProp.Nodes            as Nodes
from   LoKiCore.functions        import *
# =============================================================================
## Simple class for access MC-truth 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class D0_twobody(AlgoMC) :
    """
    find recontructed D0 -> hh  candidates 
    """
    ## standard constructor
    def __init__ ( self , name = 'D02hh' ) :
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

        # find proper decays
        
        # D0/D~0 -> pi+ pi- 
        d01  = Trees.MCExclusive  ( Nodes.CC('D0') )        
        d01 += Nodes.Pid('pi+') 
        d01 += Nodes.Pid('pi-') 
        d01  = Trees.MCPhotos ( d01 )
        st   = d01.validate ( self.ppSvc() )
        if st.isFailure()  : return st
        
        # D0/D~0 -> pi+ K-
        d02  = Trees.MCExclusive  ( Nodes.CC('D0') )        
        d02 += Nodes.Pid('pi+') 
        d02 += Nodes.Pid('K-') 
        d02  = Trees.MCPhotos ( d02 )
        st   = d02.validate ( self.ppSvc() )
        if st.isFailure()  : return st
        
        # D0/D~0 -> K+ K-
        d03  = Trees.MCExclusive  ( Nodes.CC('D0') )        
        d03 += Nodes.Pid('K+') 
        d03 += Nodes.Pid('K-') 
        d03  = Trees.MCPhotos ( d03 )
        st   = d03.validate ( self.ppSvc() )
        if st.isFailure()  : return st
        
        # D0/D~0 -> K+ pi-
        d04  = Trees.MCExclusive  ( Nodes.CC('D0') )        
        d04 += Nodes.Pid('K+') 
        d04 += Nodes.Pid('pi-') 
        d04  = Trees.MCPhotos ( d04 )
        st   = d04.validate ( self.ppSvc() )
        if st.isFailure()  : return st
        
        # D0/D~0 -> mu+ mu-
        d05  = Trees.MCExclusive  ( Nodes.CC('D0') )        
        d05 += Nodes.Pid('mu+') 
        d05 += Nodes.Pid('mu-') 
        d05  = Trees.MCPhotos ( d05 )
        st   = d05.validate ( self.ppSvc() )
        if st.isFailure()  : return st


        mcD01 = NONE
        mcD02 = NONE
        mcD03 = NONE
        mcD04 = NONE
        mcD05 = NONE

        # d0 -> hh 
        if 27363000 == evtType :  
            
            cut01  = MCDECTREE( d01 ) 
            mcd01 = self.mcselect ('mcd01' , cut01 )
            cut02  = MCDECTREE( d02 ) 
            mcd02 = self.mcselect ('mcd02' , cut02 )
            cut03  = MCDECTREE( d03 ) 
            mcd03 = self.mcselect ('mcd03' , cut03 )            
            cut04  = MCDECTREE( d04 ) 
            mcd04 = self.mcselect ('mcd04' , cut04 )
            mcD01 = NONE if mcd01.empty() else MCTRUTH ( self.mcTruth() , mcd01 ) 
            mcD02 = NONE if mcd02.empty() else MCTRUTH ( self.mcTruth() , mcd02 )
            mcD03 = NONE if mcd03.empty() else MCTRUTH ( self.mcTruth() , mcd03 )
            mcD04 = NONE if mcd04.empty() else MCTRUTH ( self.mcTruth() , mcd04 )
            
            # d0 -> mu mu              
        elif 22112001 == evtType :
                
            cut05  = MCDECTREE( d05 ) 
            mcd05 = self.mcselect ('mcd05' , cut05 )
            mcD05 = NONE if mcd05.empty() else MCTRUTH ( self.mcTruth() , mcd05 )

        
        wm01 = WMASS ( 'pi+' , 'pi-' )
        wm02 = WMASS ( 'pi+' , 'K-'  )
        wm03 = WMASS ( 'K+'  , 'K-'  )
        wm04 = WMASS ( 'K+'  , 'pi-' )
        wm05 = WMASS ( 'mu+' , 'mu-' )

        mcut = 200 * Units.MeV
        
        dm =        ADWM( 'D0' , wm01 ) < mcut 
        dm = dm | ( ADWM( 'D0' , wm02 ) < mcut ) 
        dm = dm | ( ADWM( 'D0' , wm03 ) < mcut ) 
        dm = dm | ( ADWM( 'D0' , wm04 ) < mcut ) 
        dm = dm | ( ADWM( 'D0' , wm05 ) < mcut ) 
        

        #dm =        M < 2.00 * Units.GeV
        #dm = dm & ( M > 1.00 * Units.GeV ) 
        
        # get all primary vertices
        primaries = self.vselect( 'PVs' , ISPRIMARY )

        if primaries.empty() :
            return self.Warning('No Primary vertices are found!', SUCCESS)

        mips = MIPCHI2( primaries, self.geo() ) 

        particles = self.select ( 'h' ,  ('pi+' == ABSID ) & ( mips > 9 ) )             
        self.select ( 'h+' , particles , 0 < Q )
        self.select ( 'h-' , particles , 0 > Q )
        
        tup = self.nTuple ('D0 ->Two body') 

        ok01 = False
        ok02 = False
        ok03 = False
        ok04 = False
        ok05 = False
        
        d0 = self.loop ( 'h+ h-' , 'D0' )
        for D in d0 :

            p1 = D(1).particleID().abspid() 
            p2 = D(2).particleID().abspid() 

            if 13 == p1 and p1 != p2 : continue 
            if 13 == p2 and p1 != p2 : continue 
            
            m12 = D.mass(1,2) / Units.GeV
            if m12 > 3.0  : continue
            if m12 < 0.6  : continue
            chi2 = VCHI2 ( D )
            if not 0 <=  chi2 < 9  : continue

            m = M(D) / Units.GeV
            
            self.plot ( m , 'mass h+ h- '         , 0.6 , 3.0 , 240 )        
            if not dm( D ) : continue
            self.plot ( m , 'mass h+ h- ACCEPTED' , 0.6 , 3.0 , 240 )        

            
            tup.column ( 'm'     ,         m   )
            tup.column ( 'vchi2' ,      chi2   )
            tup.column ( 'lv01'  , LV01  ( D ) )

            imc01 = mcD01 ( D )
            imc02 = mcD02 ( D )
            imc03 = mcD03 ( D )
            imc04 = mcD04 ( D )
            imc05 = mcD05 ( D )
            
            tup.column ( 'mc01'  , imc01 )
            tup.column ( 'mc02'  , imc02 )
            tup.column ( 'mc03'  , imc03 )
            tup.column ( 'mc04'  , imc04 )
            tup.column ( 'mc05'  , imc05 )

            if   27363000 == evtType  :
                if imc01 : ok01 = True
                if imc02 : ok02 = True
                if imc03 : ok03 = True
                if imc04 : ok04 = True
            elif 22112001 == evtType :
                if imc05 : ok05 = True
                
            tup.column ( 'wm01'  , wm01  ( D ) )
            tup.column ( 'wm02'  , wm02  ( D ) )
            tup.column ( 'wm03'  , wm03  ( D ) )
            tup.column ( 'wm04'  , wm04  ( D ) )
            tup.column ( 'wm05'  , wm05  ( D ) )
            tup.column ( 'evt'   , evtType     )
            tup.write  () 
                 
            
            D.save('D0')


        if 27363000 == evtType : 
            cnt01 = self.counter ('#acc pi+ pi-' )
            cnt01 += ok01
            cnt02 = self.counter ('#acc pi+ K-'  )
            cnt02 += ok02
            cnt03 = self.counter ('#acc K+  K-'  )
            cnt03 += ok03
            cnt04 = self.counter ('#acc K+  pi-' )
            cnt04 += ok04

            cnt06 = self.counter ('#acc K+  pi- | pi+ K-' )
            cnt06 += ok02 or ok04
            
        elif 22112001 == evtType :
            cnt05 = self.counter ('#acc mu+ mu-' )
            cnt05 += ok05

        D0 = self.selected('D0')

        self.setFilterPassed ( not D0.empty()  )
              
        return SUCCESS

# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
        
    from Configurables import DaVinci, HistogramPersistencySvc , EventSelector, NTupleSvc, ApplicationMgr  
    
    daVinci = DaVinci (
        # SkipEvents = 20000      , 
        DataType   = 'DC06'     , # default  
        Simulation = True       ,
        HltType    = '' ) 
    
    EventSelector (
        PrintFreq = 100 , 
        Input = [ 
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/D02hh_1.dst'   TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/D02hh_2.dst'   TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/D02hh_3.dst'   TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/D02mumu_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/D02mumu_2.dst' TYP='POOL_ROOTTREE' OPT='READ'"
        ]
        )
    
    #import L0mb
    #import BB 

    HistogramPersistencySvc ( OutputFile = 'D02body_Histos.root' ) 
    NTupleSvc ( Output = [ "D02HH DATAFILE='D02body_Tuples.root' TYPE='ROOT' OPT='NEW'" ] )
    
    ApplicationMgr( HistogramPersistency = 'HBOOK' )
    
    gaudi = appMgr()
    
    ## create local algorithm:
    alg = D0_twobody( 'D02hh' )
    
    ##gaudi.addAlgorithm ( alg ) 
    gaudi.setAlgorithms( [alg] )
    
    alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]
    ## print histos 
    alg.HistoPrint = True
    alg.NtupleLUN  = 'D02HH'
    
    ## configure the desktop
    desktop = gaudi.tool ( 'D02hh.PhysDesktop' )
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
    run(1000)
    
# =============================================================================
# The END 
# =============================================================================
