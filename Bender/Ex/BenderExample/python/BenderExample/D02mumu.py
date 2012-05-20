#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file BEnderExample/D02mumu.py
#  The simple Bender-based example: find recontructed D0 -> mumu candidates 
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
The simple Bender-based example: find recontructed D0 -> mumu candidates

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
    def __init__ ( self , name = 'D02mumu' , **kwargs ) :
        """
        Standard constructor
        """ 
        return AlgoMC.__init__ ( self , name , **kwargs )

    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """

        # D0/D~0 -> mu+ mu- 
        mcd0 = self.mcselect ('mcd0' , ' [D0 => mu+ mu-]CC ')
        if mcd0.empty() :
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
            self.plot ( M(D) / Units.GeV , 'mass mu+ mu-' , 1.2 , 2.2 , 500 )
            D.save('D0')

        D0 = self.selected('D0')
        self.setFilterPassed ( not D0.empty()  )
            
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
## configure the job
def configure ( datafiles , catalogs = [] , castor = False ) :
    """
    Configure the job
    """
        
    ##
    ## Static Configuration (``Configurables'')
    ##
    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType      = '2010' ,
        Simulation    = True   ,
        HistogramFile = 'D02mumu_Histos.root' 
        ) 
    
    from Configurables import HistogramPersistencySvc 
    
    from StandardParticles import StdLooseMuons 
    InputParticles = [
        StdLooseMuons  . outputLocation ()
        ]

    ## define the input data 
    setData ( datafiles , catalogs , castor ) 
    
    ##
    ## Dynamic Configuration: Jump into the wonderful world of GaudiPython 
    ##
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = D02mumu(
        'D02mumu' ,
        ## MC-links:
        PP2MCs     = [ 'Relations/Rec/ProtoP/Charged' ] , 
        ## print histos 
        HistoPrint = True , 
        ## input particles
        Inputs     = InputParticles 
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
        '/lhcb/MC/MC10/ALLSTREAMS.DST/00008828/0000/00008828_00000%03d_1.allstreams.dst' % i for i in range ( 1 , 45 ) 
        ]
    
    configure( inputdata , castor  = True )  
    
    ## run the job
    run(501)
    
# =============================================================================
# The END 
# =============================================================================
