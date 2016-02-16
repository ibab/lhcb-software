#!/usr/bin/env python
# =============================================================================
# $Id:$ 
# =============================================================================
## @file BenderExample/MCuDST
# The simple Bender-based example to illustrate
#    usage of MC-uDST 
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
#  @date 2016-02-16
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#
#            Version $Revision:$
#  Last modification $Date: 2015-03-20 11:51:25 +0100 (Fri, 20 Mar 2015) $
#                 by $Author: ibelyaev $
# =============================================================================
"""
The simple Bender-based example to illustrate usage of MC-uDST 

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

          Version $Revision:$  
Last modification $Date: 2015-03-20 11:51:25 +0100 (Fri, 20 Mar 2015) $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2016-02-16 "
__version__ = "$Revision: 185719 $ "
# =============================================================================
## import everything form bender
import GaudiKernel.SystemOfUnits as Units 
from   Bender.MainMC             import *
# =============================================================================
## Simple class for access MC-truth 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2016-02-16
class MCuDST(AlgoMC) :
    """ Simple class for showing usage of  MC-truth for uDST 
    """
    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """
        
        b    = self.select('beauty', 'Beauty -> pi+ pi-' )
        if not b   : return self.Warning ( 'No reconstructed B are found' , SUCCESS )        
        
        mcb  = self.mcselect ('mc'  , 'Beauty & LongLived')        
        if not mcb : return self.Warning ( 'No longlived beauty is found' , SUCCESS )        

        
        mcB  = MCTRUTH ( self.mcTruth() , mcb  )        
        rcB  = RCTRUTH ( self.mcTruth() ,   b  )        

        ## get all MC-particles
        mca  = self.mcselect ('mcall' , MCALL )

        for p in mca :
            
            if not rcB ( p ) : continue 
            print 'MC particle that makes contribution: ', p.decay()

        ## get all particles
        all  = self.select ('all' , PALL )
        for p in all :
            if not mcB  ( p ) : continue
            print 'RC particle that gets contribution:', p.decay()
            
        return SUCCESS


# =============================================================================
## configure the job
def configure ( datafiles , catalogs = [] , castor = False ) :
    """
    Configure the job
    """
    root_in_TES = '/Event/AllStreams'
    the_line    = 'Hb2Charged2BodyB2Charged2BodyLine'
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters (
        STRIP_Code = """
        HLT_PASS_RE('.*%s.*')
        """ % the_line 
        ) 
    ##
    ## Static Configuration (``Configurables'')
    ##
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType        = '2012' , 
        Simulation      = True   ,
        ##
        InputType       = 'MDST'      ,
        RootInTES       = root_in_TES ,
        ##
        EventPreFilters = fltrs.filters('STRIP'),  
        ##
        DDDBtag         = 'dddb-20150928' , 
        CondDBtag       = 'sim-20150813-2-vc-md100'
        )

    
    InputParticles = [ 'Phys/%s/Particles' % the_line ]
    
    daVinci.UserAlgorithms = [ 'MCuDST' ]
    
    ## define the input data 
    setData ( datafiles , catalogs , castor ) 
    
    ##
    ## Dynamic Configuration: Jump into the wonderful world of GaudiPython 
    ##
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = MCuDST(
        'MCuDST' ,
        RootInTES         = root_in_TES    , ## We are reading uDST! 
        CollectP2MCPLinks = True           , ## autocollect P->MCP links  
        ## MC-links
        PP2MCs            = []             , ##  'Relations/Rec/ProtoP/Charged' ] ,
        ## Input particles 
        Inputs            = InputParticles ,
        )
   
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

    inputdata = [
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000022_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000044_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000014_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000034_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000001_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000048_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000009_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000004_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000008_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000026_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000003_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000050_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000012_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000046_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000002_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000013_2.AllStreams.mdst",
        "/lhcb/MC/Dev/ALLSTREAMS.MDST/00049484/0000/00049484_00000011_2.AllStreams.mdst"
        ]
        
    configure( inputdata , castor = True )
    
    ## run the job
    run(600)
    
# =============================================================================
# The END 
# =============================================================================
