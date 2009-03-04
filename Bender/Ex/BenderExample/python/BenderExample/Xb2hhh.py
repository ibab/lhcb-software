#!/usr/bin/env python
# =============================================================================
## @file BenderExample/Bs2DsK.py
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
class Xb2hhh(AlgoMC) :
    """
    find recontructed Xb -> hhh candidates 
    """
    ## standard constructor
    def __init__ ( self , name = 'Xb2hhh' ) :
        """
        Standard constructor
        """ 
        return AlgoMC.__init__ ( self , name )
    
    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """
        
        # Xb -> mu+ mu- h+ h- 
        ##xb = Trees.MCExclusive  ( Nodes.Beauty , 1 , True )
        xb = Trees.MCExclusive  ( Nodes.Beauty )
        h1 = Nodes.Pid ('pi+') | Nodes.Pid ('K+') | Nodes.Pid( 'p+'  ) 
        h2 = Nodes.Pid ('pi-') | Nodes.Pid ('K-') | Nodes.Pid( 'p~-' )
        h3 = Nodes.CC  ('pi+') | Nodes.CC  ('K+') 
        xb += h1 
        xb += h2 
        xb += h3 

        xb = Trees.Photos ( xb ) 
        
        st =  xb.validate ( self.ppSvc() )
        if st.isFailure()  : 
            return self.Error ( 'Invalid decay descriptor' , st )        

        cut = MCDECTREE( xb ) 
        mcxb = self.mcselect ('mcxb' , cut )
        if mcxb.empty() :
            return self.Warning ( 'No mc-trees are found' , SUCCESS )        

        primaries = self.vselect( 'PVs' , ISPRIMARY )
        if primaries.empty() :
            return self.Warning('No Primary vertices are found!', SUCCESS)
        
        mcXb = MCTRUTH ( self.mcTruth() , mcxb )        
        
        hadrons = self.select ( 'h'  ,
                                ( ( 'pi+' == ABSID ) |
                                  ( 'K+'  == ABSID ) |
                                  ( 'p+'  == ABSID ) ) & mcXb )
        
        nh  = self.counter('#hadron')
        nh += hadrons.size()
        
        if hadrons.size() < 3 : return SUCCESS
        
        h1  = self.select ( 'h+'  , hadrons , Q > 0 ) 
        h2  = self.select ( 'h-'  , hadrons , Q < 0 ) 
        m   = self.select ( 'pk'  , hadrons ,
                            ( 'pi+' == ABSID ) | ( 'K+' == ABSID ) )
        
        xb = self.loop ( 'h+ h- pk' , 'B+' )

        prot = 'p+' == ABSID 
        for b in xb :
            m123 = b.mass(1,2,3) / Units.GeV
            if not 3 < m123 < 7     : continue
            p1 = prot ( b(1) )
            p2 = prot ( b(2) )
            if p1 and not p2 : continue
            if p2 and not p1 : continue
                
            chi2 = VCHI2 ( b )
            if not 0 <=  chi2 < 36  : continue
            if  not mcXb ( b )      : continue 
            self.plot ( M( b ) / Units.GeV , 'mass h+ h- h' , 3 , 7 , 200 )
            b.save ('Xb') 


        Xb = self.selected('Xb')
        self.setFilterPassed ( not Xb.empty()  )
            
        return SUCCESS

# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
        
    from Configurables import DaVinci, HistogramPersistencySvc , EventSelector 
    
    daVinci = DaVinci (
        # SkipEvents = 22000       ,
        DataType   = 'DC06'      , # default  
        Simulation = True        ,
        HltType    = '' ) 
    
    HistogramPersistencySvc ( OutputFile = 'Xb2hhh_Histos.root' ) 
    
    EventSelector (
        Input   = [
        # b -> pp pi
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000010_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000012_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000013_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000014_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000015_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000017_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000018_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000020_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000021_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000022_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000023_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000024_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000025_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000026_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000027_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000028_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000029_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000030_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000031_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000032_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000033_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000034_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000035_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000036_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000037_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000038_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000039_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000040_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000041_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000042_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000043_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000044_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000046_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000047_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000048_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000049_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000050_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000051_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000053_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000055_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000056_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000057_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002099/DST/0000/00002099_00000058_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##         # b -> pp K 
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000003_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000004_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000007_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000009_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000010_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000011_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000012_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000013_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000014_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000015_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000016_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000017_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000018_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000019_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000020_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000021_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000022_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000023_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000025_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000026_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000028_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000029_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000030_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000031_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000032_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000033_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000036_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000037_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000038_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000039_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000040_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000041_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000042_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000044_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000045_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000046_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001976/DST/0000/00001976_00000047_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##        # B -> pi KK 
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000007_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000011_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000012_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000014_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000016_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000018_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000019_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000021_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000022_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000023_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000024_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000027_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000028_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000029_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000032_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000033_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000034_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000035_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000036_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000037_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000038_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000039_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000041_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000042_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000043_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000044_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000045_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000048_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000049_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000050_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000051_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000052_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000053_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000056_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000057_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000058_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000059_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000060_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000061_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000062_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000063_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000065_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000067_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000068_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000069_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002098/DST/0000/00002098_00000070_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"      
##         # b -> pipiK 
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000010_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000011_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000012_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000013_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000015_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000017_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000018_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000020_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000021_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000022_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000023_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000024_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000025_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000026_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000027_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000028_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000030_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000031_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000032_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000033_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000034_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000035_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000037_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000038_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000039_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000040_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000041_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000042_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000043_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000045_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000046_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000049_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000051_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000052_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000053_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000054_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000055_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000057_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000058_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000060_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000062_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002100/DST/0000/00002100_00000063_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"        
##        # B -> 3 K 
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000011_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000013_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000014_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000016_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000017_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000018_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000020_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000021_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000023_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000024_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000025_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000026_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000027_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000029_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000030_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000031_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000033_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000034_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000035_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000036_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000041_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000042_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000044_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000045_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000046_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000047_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000048_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000049_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000050_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000052_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000053_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000054_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000055_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000056_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000057_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000058_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000059_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002097/DST/0000/00002097_00000060_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"
##         # B -> 3 pi 
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000013_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000014_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000015_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000016_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000017_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000019_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000020_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000021_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000022_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000023_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000024_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000026_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000027_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000028_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000030_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000031_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000032_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000033_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000034_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000035_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000036_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000037_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000038_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000040_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000041_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000043_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000044_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000045_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000047_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000048_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000049_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000050_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000051_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000052_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000053_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000054_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000055_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000057_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000059_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000060_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000061_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000062_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000063_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000064_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000065_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000066_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000067_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000068_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000070_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000071_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000072_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000073_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000074_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000075_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000077_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000078_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000079_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000080_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002096/DST/0000/00002096_00000081_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"
        ]
        )
    
    
    from Configurables import OutputStream, ApplicationMgr
    
    dst = OutputStream ( 'DstWriter' )
    dst.Output = "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/Xb2hhh_pppi.dst' TYP='POOL_ROOTTREE' OPT='RECREATE'"
    
    # rDST content 
    dst.ItemList += [
        "/Event/DAQ/ODIN#1"
        , "/Event/Rec/Header#1"
        , "/Event/Rec/Status#1"
        , "/Event/Rec/Track/Best#1"
        , "/Event/Rec/Calo/Electrons#1"
        , "/Event/Rec/Calo/Photons#1"
        , "/Event/Rec/Calo/MergedPi0s#1"
        , "/Event/Rec/Calo/SplitPhotons#1"
        , "/Event/Rec/ProtoP/Charged#1"
        , "/Event/Rec/ProtoP/Neutrals#1"
        , "/Event/Rec/Vertex/Primary#1"
        ]
    # DstContent 
    dst.ItemList += [    
        "/Event/DAQ/RawEvent#1"
        ]
    dst.OptItemList += [    
        "/Event/Rec/Track/Muon#1"
        ]
    # MCDstContent 
    dst.ItemList += [
        # Links to MCParticles created in Brunel
        "/Event/Link/Rec/Track/Best#1"
        #  Objects propagated from Gauss
        , "/Event/Gen/Header#1"
        , "/Event/Gen/Collisions#1"
        , "/Event/Gen/HepMCEvents#1"
        , "/Event/MC/Header#1"
        , "/Event/pSim/MCParticles#1"
        , "/Event/pSim/MCVertices#1"
        # Objects propagated from Boole
        , "/Event/MC/DigiHeader#1"
        , "/Event/MC/TrackInfo#1"
        , "/Event/MC/Rich/DigitSummaries#1"
        , "/Event/MC/Muon/DigitsInfo#1"
        , "/Event/Link/Raw/Velo/Clusters#1"
        , "/Event/Link/Raw/TT/Clusters#1"
        , "/Event/Link/Raw/IT/Clusters#1"
        , "/Event/Link/Raw/OT/Times#1"
        , "/Event/Link/Raw/Ecal/Digits#1"
        , "/Event/Link/Raw/Muon/Digits#1"
        , "/Event/Link/Trig/L0/FullCalo#1"
        ]
    #
    dst.OptItemList += [
        "/Event/Prev/MC/Header#1"
        , "/Event/PrevPrev/MC/Header#1"
        , "/Event/Next/MC/Header#1"
        # Links for  extended Rich info if corresponding data available on input file
        , "/Event/Link#1"
        , "/Event/Link/MC#1"
        , "/Event/Link/MC/Particles2MCRichTracks#1"
        , "/Event/Link/MC/Rich/Hits2MCRichOpticalPhotons#1"
        # Objects propagated from Boole, not available in DC06
        , "/Event/Link/Raw/Hcal/Digits#1"  # // From Boole v14r9
        ]
    # own stuf f
    dst.OptItemList += [
        '/Event/Relations#1'
        , '/Event/Relations/Rec#1'
        , '/Event/Relations/Rec/ProtoP#1'
        , '/Event/Relations/Rec/ProtoP/Charged#1'
        ]
    dst.OptItemList += [
        '/Event/Phys#1'
        , '/Event/Phys/StdLooseKaons#*'
        , '/Event/Phys/StdLoosePions#*'
        , '/Event/Phys/StdLooseProtons#*'
        ]
    
    dst.RequireAlgs = [ 'Xb2hhh' ]
    
    ApplicationMgr ( OutStream = [ dst ] )
    
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Xb2hhh()
    
    gaudi.addAlgorithm ( alg ) 
    ##gaudi.setAlgorithms( [alg] )
    
    alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]
    ## print histos 
    alg.HistoPrint = True
    
    ## configure the desktop
    desktop = gaudi.tool ( 'Xb2hhh.PhysDesktop' )
    desktop.InputLocations = [
        '/Event/Phys/StdLooseKaons'   ,
        '/Event/Phys/StdLoosePions'   ,
        '/Event/Phys/StdLooseProtons' 
        ]
    #desktop.PropertiesPrint = True
    
    ## get input data 
    ##import LoKiExample.Bs2Jpsiphi_mm_data as input 
    evtSel = gaudi.evtSel()    
    #evtSel.open ( 'Bs2DsK_1.dst') 
    evtSel.PrintFreq = 50

    return SUCCESS 
    
# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## make printout of the own documentations 
    print __doc__

    ## configure the job:
    configure()

    ## run the job
    run(10000)
    

# =============================================================================
# The END 
# =============================================================================
