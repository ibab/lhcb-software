#!/usr/bin/env python
# =============================================================================
## The simple Bender-based example: find recontructed Bs -> BsK candidates 
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
The simple Bender-based example: find recontructed Bs -> BsK candidates 
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
class Bs2DsK(AlgoMC) :
    """
    find recontructed Bs -> Ds K candidates 
    """
    ## standard constructor
    def __init__ ( self , name = 'Bs2DsK' ) :
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

        ## Xb -> Ds+/Ds- K+/K-
        bs = Trees.MCExclusive ( Nodes.Beauty )
        bs += mark ( ds ) 
        bs += Nodes.CC("K+") 
        
        st =  bs.validate ( self.ppSvc() )
        if st.isFailure()  : return st

        cut = MCDECTREE(bs) 
        mcbs = self.mcselect ('mcbs' , cut )
        if mcbs.empty() or 1 < mcbs.size() :
            return self.Warning ( 'No mc-trees are found' , SUCCESS )        

        mcds = std.vector('const LHCb::MCParticle*')()

        bs.reset() 
        for b in mcbs :
            if bs ( b ) :
                bs.collect ( mcds )
                bs.reset()
                
        mcBs = MCTRUTH ( self.mcTruth() , mcbs )        
        mcDs = MCTRUTH ( self.mcTruth() , mcds )
        
        
        kaons = self.select ( 'bkaons' ,
                              ( 'K+' == ABSID ) &
                              ( PIDK > 2      ) & mcDs )
        pions  = self.select ( 'pion' ,
                               ( 'pi+' == ABSID )  &
                               ( PIDK  < 1       ) &
                               ( PIDe  < 1       ) &
                               ( PIDmu < 1       ) & mcDs )
        
        bkaons = self.select ( 'bk' ,
                               ( 'K+' == ABSID ) & mcBs & ~mcDs )
        
        k1p = self.select ( 'k+' , kaons , 0 < Q )
        k1m = self.select ( 'k-' , kaons , 0 > Q )

        ds = self.loop ( 'k+ k- pion' , 'D_s+' )
        for D in ds :
            m123 = D.mass(1,2,3) / Units.GeV
            if not 1.4 < m123 < 2.0 : continue
            chi2 = VCHI2 ( D )
            if not 0 <=  chi2 < 100 : continue
            if  not mcDs ( D )      : continue 
            self.plot ( M(D) / Units.GeV , 'mass K+ K- pi' , 1.8 , 2.1 , 300 )
            sq = SUMQ( D )
            if sq < 0 : D.setPID ( 'D_s-' )
            D.save('Ds')

        bs = self.loop ( 'Ds bk' , 'B_s0')
        for B in bs :
            m12 = B.mass(1,2) / Units.GeV
            if not 3 < m12  < 7   : continue
            chi2 = VCHI2 ( B ) 
            if not 0<= chi2 < 100 : continue 
            if not mcDs ( B(1) )  : continue
            if not mcBs ( B(2) )  : continue
            if not mcBs ( B    )  : continue 
            self.plot ( M(B) / Units.GeV , 'mass Ds K' , 4.0 , 6.0 , 200 )
            B.save ('Bs') 

        Bs = self.selected('Bs')
        self.setFilterPassed ( not Bs.empty()  )
            
        return SUCCESS

# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
        
    from Configurables import DaVinci, HistogramPersistencySvc , EventSelector 
    
    daVinci = DaVinci (
        SkipEvents = 22000       ,
        DataType   = 'DC06'      , # default  
        Simulation = True        ,
        HltType    = '' ) 
    
    HistogramPersistencySvc ( OutputFile = 'Bs2DsK_Histos.root' ) 
    
    EventSelector (
        ##-- GAUDI data cards generated on 12/22/08 2:34 PM
        ##-- For Event Type = 13264011 / Data type = DST 1
        ##--     Configuration = DC06 - phys-v2-lumi2
        ##--     DST 1 datasets produced by Brunel - v30r17
        ##--     From DIGI 1 datasets produced by Boole - v12r10
        ##--     From SIM 1 datasets produced by Gauss - v25r12
        ##--     Database version = v30r14
        ##--     Cards content = physical-physical
        ##--     
        ##-- Datasets replicated at CERN
        ##-- 50 dataset(s) - NbEvents = 24847
        ##-- 
        Input   = [
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000007_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000009_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000011_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000013_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000014_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000015_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000016_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000017_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000018_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000019_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000020_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000021_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000022_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000023_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000024_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000026_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000029_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000030_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000031_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000032_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000033_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000034_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000035_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000036_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000038_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000039_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000040_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000043_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000044_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000046_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000047_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000048_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000049_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000050_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000051_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000052_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000055_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000056_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000057_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000058_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000059_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000060_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000061_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000062_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000063_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
        "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001980/DST/0000/00001980_00000064_5.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
        ] )
    
    from Configurables import OutputStream, ApplicationMgr
    
    dst = OutputStream ( 'DstWriter' )
    dst.Output = "DATAFILE='Bs2DsK_5.dst' TYP='POOL_ROOTTREE' OPT='RECREATE'"
    
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
        , '/Event/Phys/StdTightKaons#*'
        , '/Event/Phys/StdTightPions#*'
        , '/Event/Phys/StdNoPIDsKaons#*'
        ]

    dst.RequireAlgs = [ 'Bs2DsK' ]
    
    ApplicationMgr ( OutStream = [ dst ] )
    
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Bs2DsK()
    
    gaudi.addAlgorithm ( alg ) 
    ##gaudi.setAlgorithms( [alg] )
    
    alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]
    ## print histos 
    alg.HistoPrint = True
   
    ## configure the desktop
    desktop = gaudi.tool ( 'Bs2DsK.PhysDesktop' )
    desktop.InputLocations = [
        '/Event/Phys/StdTightKaons' ,
        '/Event/Phys/StdTightPions' ,
        '/Event/Phys/StdNoPIDsKaons' 
        ]
    #desktop.PropertiesPrint = True
        
    ## get input data 
    ##import LoKiExample.Bs2Jpsiphi_mm_data as input 
    evtSel = gaudi.evtSel()    
    #evtSel.open ( 'Bs2DsK_1.dst') 
    evtSel.PrintFreq = 20

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
