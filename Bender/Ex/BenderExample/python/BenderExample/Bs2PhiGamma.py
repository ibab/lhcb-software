#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file BenderExample/Bs2PhiGamma.py
#
#  Simple module to test/verify MC-association problem for the photons
#  claimed by Victor Yakovenko (LAL)
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
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-12-19
#
#  Last modification $Date$
#                 by $Author$
#
# =============================================================================
"""

Simple module to test/verify MC-association problem for the photons
claimed by Victor Yakovenko (LAL)

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
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__date__    = '2008-12-19'
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision$"
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

        mcbs  = self.mcselect ( 'mcbs'  , '[ B_s0 -> (  phi(1020) =>  K+  K- )  gamma]CC')
        if mcbs.empty() : return self.Warning('No MC-Bs are found', SUCCESS )
        
        mcphi = self.mcselect ( 'mcphi' , '[ B_s0 -> ^( phi(1020) =>  K+  K- )  gamma]CC') 
        mck   = self.mcselect ( 'mck'   , '[ B_s0 ->  ( phi(1020) => ^K+ ^K- )  gamma]CC')
        mcg   = self.mcselect ( 'mcg'   , '[ B_s0 ->  ( phi(1020) =>  K+  K- ) ^gamma]CC') 
        
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
# make the real configuration 
# =============================================================================
def configure ( datafiles , catalogs  = [] , castor = False ) :
    """
    Perform the real configuration of the job
    """
    
    ##
    ## Static Configuration (``Configurables'')
    ##
    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType      = '2012' ,
        Simulation    = True   ,
        HistogramFile = 'PhiGamma_Histos.root'  
        )

    ## define the input data 
    setData ( datafiles , catalogs , castor ) 
    
    daVinci.UserAlgorithms  = [ 'Bs2PhiGamma']
    
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters(
        HLT1_Code  = "HLT_PASS_RE('Hlt1.*(Gamma|Photon).*')"  ,
        HLT2_Code  = "HLT_PASS_RE('Hlt2.*(Gamma|Photon).*')"  ,
        STRIP_Code = "HLT_PASS_RE('.*(Gamma|Photon).*')"          
        )
    
    daVinci.EventPreFilters = fltrs.filters ('RequireL0')

    
    from StandardParticles import StdTightKaons 
    InputParticles = [
        StdTightKaons  . outputLocation ()  ,
        'Phys/StdLooseAllPhotons/Particles'
        ]

    ##
    ## Dynamic Configuration: Jump into the wonderful world of GaudiPython 
    ##
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Bs2PhiGamma(
        'Bs2PhiGamma'         ,
        # print histos 
        HistoPrint     = True ,
        # input particles 
        Inputs = InputParticles
        )
    
    return SUCCESS 

# =============================================================================
## the actual job steering 
if '__main__' == __name__ :

    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  
    
    inputdata = [
        "/lhcb/MC/2012/ALLSTREAMS.DST/00029336/0000/00029336_00000%03d_1.allstreams.dst" % i for i in range(1,100) 
        ]
    
    configure ( inputdata , castor = True ) 
    
    run(1000) 

# =============================================================================
# The END 
# =============================================================================
