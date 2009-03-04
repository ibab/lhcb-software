#!/usr/bin/env python
# =============================================================================
# $Id: GammaForThomas.py,v 1.2 2009-03-04 11:53:39 ibelyaev Exp $ 
# =============================================================================
## @file BenderExample/GammaForThomas.py
#  Simple module to test/verify MC-association for the photons
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-02-10
# =============================================================================
"""
Simple module to test/verify MC-association for the photons
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
from Bender.MainMC import * 
import GaudiKernel.SystemOfUnits as Units

# =============================================================================
## @class GammaForThomas
#  simple class/algorithm to test MC-associations for the photons
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-02-10
class GammaForThomas(AlgoMC) :
    """
    Simple class to test MC-associations for photons from Bs -> phi gamma 
    """
    ## the only one essential method 
    def analyse ( self ) :
        """
        The only one essential method
        """

        finder = self.mcFinder()
        
        mcbs  = finder.find("[ B_s0 -> (  phi(1020) ->  K+  K- )  gamma]cc")
        
        if mcbs.empty() : return self.Warning('No MC-Bs are found', SUCCESS ) 
        mcphi = finder.find("[ B_s0 -> ( ^phi(1020) ->  K+  K- )  gamma]cc") 
        mck   = finder.find("[ B_s0 -> (  phi(1020) -> ^K+ ^K- )  gamma]cc")
        mcg   = finder.find("[ B_s0 -> (  phi(1020) ->  K+  K- ) ^gamma]cc") 

        if mcphi.empty() or mck.empty() or mcg.empty() : 
            return self.Warning ('No MC are found', SUCCESS ) 


        # get CaloCluster -> MCParticle relation table 
        table = self.get('Relations/Rec/Calo/Clusters')


        ##invert table: MCParticle -> CaloCluster         
        itable = LHCb.RelationWeighted1D(LHCb.MCParticle,LHCb.CaloCluster,'float') ( table , 1 )


        cnt1 = self.counter('# links direct' )
        cnt2 = self.counter('# links inverse')

        cnt1 +=  table.relations().size()
        cnt2 += itable.relations().size()
        
        
        ## get Ecal clusters
        clusters = self.get('Rec/Calo/EcalClusters')

        cnt3  = self.counter('# clusters')
        cnt3 += clusters.size()
        
        
        # define "in Ecal acceptance" functor for MC-photons:
        
        inEcal = ( 0 < MCPZ )  & (   abs(MCPX/MCPZ)<(4000./12500.) )    # check the numbers here! 
        inEcal = inEcal        & (   abs(MCPY/MCPZ)<(3000./12500.) )    # check the numbers here!
        inEcal = inEcal        & ( ( abs(MCPX/MCPZ)>(300. /12500.) ) | 
                                   ( abs(MCPY/MCPZ)>(300. /12500.) ) )  # check the numbers here!        

        cnt = self.counter ('in Ecal')


        ## MC -> Clusters

        
        # 1) loop over true MC-photons from the decay:
        for g in mcg :

            # use the 'in Ecal acceptance' counter
            cnt += inEcal(g)
            
            # get ALL related Ecal clusters:
            links = itable.relations( g )
            
            print 'True MC-photon E/ET: ', MCE(g) / Units.GeV , MCPT(g) / Units.GeV , ' inEcal: ', inEcal(g) , ' # clusters' , links.size()   
        
            # the first 'end/interaction point', useful to define conversion and/or interactions: 
            zEnd = 100000000
            endvertices = g.endVertices()
            for ev in endvertices :
                zEnd = min ( zEnd , ev.position().Z() )

            # loop over related Ecal clusters ( links are ordered according to 'weight')
            for link in links:

                cluster = link.to()
                weight  = link.weight()
                

        ## Clusters -> MC 
                
        # 2) loop over all clusters:
        for clust in clusters:

            # get all related MC-particles:
            links = table.relations ( clust ) 

            # loop over all links

            for link in links:

                mc = link.to     ()
                w  = link.weight () 

                # our good photon from decay? 
                ok = False
                for g in mcg:
                    if g != mc : continue 
                    ok = True
                    break 


        ## OPTIONAL, LoKi-style, uncomment 'InputLocations' 
                
                
        ## get all energetic recontructed photons:
        gamma = self.select ( 'gamma' , ( 'gamma' == ID )  & ( 1.5 * Units.GeV < PT ) )
        if gamma.empty() : return self.Warning ('No RC-photons are found', SUCCESS ) 

        
        # MC-match object:
        mc = self.mcTruth()

        # functor to select "true" photons 
        mcGamma = MCTRUTH ( mc , mcg   )

        # functor to select "true" phis 
        mcPhi   = MCTRUTH ( mc , mcphi )

        # functor to select "true" kaons 
        mcK      = MCTRUTH ( mc , mck )


        return SUCCESS 

    
# =============================================================================
# make the real configuration 
# =============================================================================
def configure() :
    """
    Perform the real configuration of the job
    """

    from Configurables import DaVinci, HistogramPersistencySvc, EventSelector, CellularAutomaton 
    
    daVinci = DaVinci (
        DataType   = 'DC06'     , # default  
        Simulation = True       ,
        HltType    = '' ) 
    
    HistogramPersistencySvc ( OutputFile = 'PhiGamma_Histos.root' ) 

    CellularAutomaton ( 'EcalClust' , Sort = False ) 
        
    EventSelector ( Input   = [
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000003_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000007_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000010_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000011_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000012_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000016_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000017_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000019_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000021_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000022_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000023_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000024_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000025_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000026_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000027_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000028_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000029_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000030_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000032_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000033_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000034_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000035_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000036_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000037_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000038_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00002019/DST/0000/00002019_00000039_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"] )
    
    ## get the actual C++ application manager (it is not Configurable!!!)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = GammaForThomas( 'Gamma')
    
    ## print histos 
    alg.HistoPrint = True

    ## be efficient: (not needed here)... 
    alg.PP2MCs = []
    
    ## gaudi.addAlgorithm ( alg ) 
    gaudi.setAlgorithms( [alg] )
     
    ## configure the desktop
    desktop = gaudi.tool ( 'Gamma.PhysDesktop' )
    desktop.InputLocations = [
        ## NB: empty !!!
        #'Phys/StdLooseAllPhotons'
        ]    
    
    
    return SUCCESS 



# =============================================================================
## the actual job steering 
if '__main__' == __name__ :

    configure()
    run(1000) 

# =============================================================================
# The END 
# =============================================================================
