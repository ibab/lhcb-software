#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file BenderExample/PhiMC.py
# The simple Bender-based example: plot dikaon mass peak with MC-truth
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
The simple Bender-based example plot dikaon mass peak with MC-truth

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
from Bender.MainMC import *

# =============================================================================
## Simple class for access MC-truth 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class PhiMC(AlgoMC) :
    """
    Simple class to plot dikaon mass peak
    """
    ## standard constructor
    def __init__ ( self , name = 'PhiMC' , **args ) :
        """
        Standard constructor
        """ 
        AlgoMC.__init__ ( self , name , **args )

    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """
        
        mcphi   = self.mcselect ( 'mcPhi', 'phi(1020) => K+ K-')        
        if mcphi.empty() : return self.Warning('No MC-phi is found!', SUCCESS )
        
        matcher = self.mcTruth ()
        mc  = MCTRUTH( matcher , mcphi ) 
        
        ## select all kaons 
        kaons = self.select( 'kaons'  , ( 'K+'  == ABSID ) & mc )
        
        self.select( "K+" , kaons , 0 < Q )
        self.select( "K-" , kaons , 0 > Q )
        
        dikaon = self.loop( "K+ K-" , "phi(1020)" )
        for phi in dikaon :
            m12 = phi.mass(1,2) / 1000 
            if 1.1 < m12  : continue
            mass = M(phi)/1000
            if 0   > mass : continue 
            self.plot ( mass , 'K+ K- mass'           , 1. , 1.1 ) 
            chi2 = VCHI2( phi )
            if 0 > chi2 or 49 < chi2 : continue
            self.plot ( mass , 'K+ K- mass,0<chi2<49' , 1. , 1.1 )  
            if not mc ( phi ) : continue 
            self.plot ( mass , 'K+ K- mass,mctruth'   , 1. , 1.1 ) 
           
        self.setFilterPassed( True ) 
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
def configure ( datafiles , catalogs  = [] , castor = False ) :
    """
    Perform the real configuration of the job
    """
    
    ##
    ## Static Configuration (``Configurables'')
    ##    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType       = '2010' ,
        Simulation    = True    ,
        HistogramFile = 'PhiMC_Histos.root' 
        )

    from StandardParticles import StdTightKaons
    InputParticles = [ StdTightKaons.outputLocation() ]
    
    ## define the input data 
    setData ( datafiles , catalogs , castor ) 
    
    ##
    ## Dynamic Configuration: Jump into the wonderful world of GaudiPython 
    ##
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
        
    ## create local algorithm:
    alg = PhiMC(
        HistoPrint     = True                ,  ## print histos 
        PP2MCs         = [
        'Relations/Rec/ProtoP/Charged' ]     ,  ## MC-truth relation tables
        Inputs         = InputParticles         ## input particles 
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
    inputdata = [
        '/lhcb/MC/MC10/ALLSTREAMS.DST/00008919/0000/00008919_00000%03d_1.allstreams.dst' % i for i in range ( 11 , 90 ) 
        ]
    
    configure( inputdata , castor = True )  
    
    run(500) 

# =============================================================================
# The END 
# =============================================================================
