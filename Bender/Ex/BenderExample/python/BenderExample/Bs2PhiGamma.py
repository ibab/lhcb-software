#!/usr/bin/env python
# =============================================================================
# $Id: Bs2PhiGamma.py,v 1.6 2010-01-12 10:58:34 ibelyaev Exp $ 
# =============================================================================
## @file BenderExample/Bs2PhiGamma.py
#
#  Simple module to test/verify MC-association problem for the photons
#  claimed by Victor Yakovenko (LAL)
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
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-12-19
# =============================================================================
"""

Simple module to test/verify MC-association problem for the photons
claimed by Victor Yakovenko (LAL)

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $"
# =============================================================================
from Bender.MainMC import * 
import GaudiKernel.SystemOfUnits as Units

# =============================================================================
## @class Bs2PhiGamma
#  simple class/algorithm to test MC-associations for the photons
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-12-19
class Bs2PhiGamma(AlgoMC) :
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

        mc = self.mcTruth() 
        mcGamma = MCTRUTH ( mc , mcg   )
        mcPhi   = MCTRUTH ( mc , mcphi )

        ## get the photons
        gamma = self.select ( 'gamma' ,
                              ( 'gamma' == ID )  & ( 2.5 * Units.GeV < PT ) )
        if gamma.empty() : return self.Warning ('No RC-photons are found', SUCCESS ) 
        
        ## get the reconstructed kaons
        kaons  = self.select ( "kaons" , "K+" == ABSID )
        kplus  = self.select ( "k+"    , kaons , 0 < Q )
        kminus = self.select ( "k-"    , kaons , 0 > Q )
        
        if kplus.empty() or kminus.empty() :
            return self.Warning ('No RC-kaons are found', SUCCESS ) 
        
        phis = self.loop ( "k+ k-" , "phi(1020)" )
        for phi in phis :
            m12  = phi.mass(1,2)
            if m12 > 1050 * Units.MeV : continue
            chi2 = VCHI2 ( phi )
            if chi2 < 0 or chi2 > 100 : continue
            phi.save('phi')
            
        phis = self.selected('phi')
        if phis.empty() : return self.Warning('No RC-phis are found', SUCCESS ) 
        
        Bs = self.loop ( "phi gamma" , "B_s0" )
        for B in Bs  :
            m12 = B.mass(1,2) / Units.GeV 
            if m12 < 4  or m12 > 6 : continue

            self.plot ( m12 , 'mass phi gamma' , 4 , 6 , 100 )

            Gamma = B(2)
            Phi   = B(1)
            
            gamma = mcGamma ( B(2) )
            phi   = mcPhi   ( B(1) )
            
            if gamma :
                self.plot ( m12 , 'mass phi (true)gamma'       , 4 , 6 )
            if phi   :
                self.plot ( m12 , 'mass (true)phi gamma'       , 4 , 6 )
            if phi and gamma :
                self.plot ( m12 , 'mass (true)phi (true)gamma' , 4 , 6 )


        return SUCCESS 

    
# =============================================================================
# make the real configuration 
# =============================================================================
def configure() :
    """
    Perform the real configuration of the job
    """
    
    from Configurables import DaVinci
    
    daVinci = DaVinci (
        DataType   = 'MC09' , # default  
        Simulation = True   
        )
    
    from Configurables import HistogramPersistencySvc, EventSelector
    HistogramPersistencySvc ( OutputFile = 'PhiGamma_Histos.root' ) 
    
    EventSelector (
        Input   = [
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000311_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000312_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000313_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000314_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000315_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000316_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000317_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000318_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000319_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000322_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'"]
        )
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Bs2PhiGamma(
        'Bs2PhiGamma'         ,
        # print histos 
        HistoPrint     = True ,
        # input particles 
        InputLocations = [
        'StdLooseKaons'       ,
        'StdLooseAllPhotons'
        ]
        )
        
    gaudi.addAlgorithm ( alg ) 
    
    return SUCCESS 


# =============================================================================
## the actual job steering 
if '__main__' == __name__ :

    configure()
    run(1000) 

# =============================================================================
# The END 
# =============================================================================
