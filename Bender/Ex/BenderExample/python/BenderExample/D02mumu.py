#!/usr/bin/env python
# =============================================================================
# #Id:$ 
# =============================================================================
## @file BEnderExample/D02mumu.py
#  The simple Bender-based example: find recontructed D0 -> mumu candidates 
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
The simple Bender-based example: find recontructed D0 -> mumu candidates 
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
# =============================================================================
## Simple class for access MC-truth 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class D02mumu(AlgoMC) :
    """
    find recontructed D0 -> mu mu  candidates 
    """
    ## standard constructor
    def __init__ ( self , name = 'D02mumu' ) :
        """
        Standard constructor
        """ 
        return AlgoMC.__init__ ( self , name )

    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """

        # D0/D~0 -> mu+ mu- 
        d0 = Trees.MCExclusive  ( Nodes.CC('D0') )
        d0 += 'mu+'
        d0 += 'mu-'

        d0 = Trees.MCPhotos ( d0 )
                
        st =  d0.validate ( self.ppSvc() )
        if st.isFailure()  : return st

        cut = MCDECTREE( d0 ) 
        mcd0 = self.mcselect ('mcd0' , cut )
        if mcd0.empty() or 1 < mcd0.size() :
            return self.Warning ( 'No mc-trees are found' , SUCCESS )        

        
        mcD0 = MCTRUTH ( self.mcTruth() , mcd0 )
        
        muons = self.select ( 'muons' , ( 'mu+' == ABSID ) & mcD0 )
        
        mup = self.select ( 'mu+' , muons , 0 < Q )
        mum = self.select ( 'mu-' , muons , 0 > Q )

        d0 = self.loop ( 'mu+ mu-' , 'D0' )
        for D in d0 :
            m12 = D.mass(1,2) / Units.GeV
            if not 1.2 < m12 < 2.0 : continue
            chi2 = VCHI2 ( D )
            if not 0 <=  chi2 < 100 : continue
            if  not mcD0 ( D )      : continue 
            self.plot ( M(D) / Units.GeV , 'mass mu+ mu-' , 1.2 , 2.0 , 400 )
            D.save('D0')


        D0 = self.selected('D0')
        self.setFilterPassed ( not D0.empty()  )
            
        return SUCCESS

# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
        
    from Configurables import DaVinci, HistogramPersistencySvc , EventSelector 
    
    daVinci = DaVinci (
        SkipEvents = 10000      ,
        DataType   = 'DC06'      , # default  
        Simulation = True        ,
        HltType    = '' ) 
    
    HistogramPersistencySvc ( OutputFile = 'D02mumu_Histos.root' ) 
    
    EventSelector (
        PrintFreq = 100 , 
##         ##-- GAUDI data cards generated on 12/22/08 6:18 PM
##         ##-- For Event Type = 22112001 / Data type = DST 1
##         ##--     Configuration = DC06 - phys-v2-lumi2
##         ##--     DST 1 datasets produced by Brunel - v30r15
##         ##--     From DIGI 1 datasets produced by Boole - v12r10
##         ##--     From SIM 1 datasets produced by Gauss - v25r9
##         ##--     Database version = v30r14
##         ##--     Cards content = physical-physical
##         ##--     
##         ##-- Datasets replicated at CERN
##         ##-- 120 dataset(s) - NbEvents = 59674
##         ##-- 
##         Input   = [
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000003_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000004_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000009_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000010_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000011_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000015_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000019_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000020_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000021_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000023_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000027_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000029_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000034_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000038_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000054_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000057_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000059_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000068_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000070_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000074_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000075_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000078_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000091_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000092_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000093_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000094_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000095_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000096_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000097_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000098_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000099_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000100_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000101_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000103_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000104_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000105_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000107_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000108_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000110_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000111_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000112_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000113_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000114_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000115_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000116_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000117_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000118_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000119_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000120_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000121_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000123_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000125_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000126_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000127_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000130_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000131_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000133_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000135_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000138_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000143_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000144_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000145_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000147_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000148_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000149_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000150_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000151_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000152_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000153_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000154_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000155_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000157_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000158_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000159_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000161_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000164_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000165_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000166_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000167_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000168_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000170_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000171_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000173_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000174_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000175_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000176_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000177_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000178_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000180_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000181_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000182_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000183_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000184_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000185_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000186_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000187_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000188_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000189_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000190_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000191_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000192_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000193_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000195_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000196_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000197_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000198_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000200_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000201_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000203_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000204_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000205_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000206_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000207_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000208_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
##         "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001833/DST/0000/00001833_00000209_5.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
##         ]
        Input = [
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/D02mumu_1.dst' TYP='POOL_ROOTTREE' OPT='READ'", 
        "DATAFILE='PFN:castor:/castor/cern.ch/user/i/ibelyaev/DaVinci/LoKiExamples/D02mumu_2.dst' TYP='POOL_ROOTTREE' OPT='READ'"
        ]
        )
    
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = D02mumu()
    
    ## gaudi.addAlgorithm ( alg ) 
    gaudi.setAlgorithms( [alg] )
    
    alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]
    ## print histos 
    alg.HistoPrint = True
   
    ## configure the desktop
    desktop = gaudi.tool ( 'D02mumu.PhysDesktop' )
    desktop.InputLocations = [
        '/Event/Phys/StdLooseMuons' 
        ]
    desktop.PropertiesPrint = True
        
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
