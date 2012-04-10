#!/usr/bin/env ipython
# ==========================================================================================
## @file BenderExample/TisTosGet.py
#
#  Simple algorithm to get tis/tos-decisions for charm particles 
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
#  @date   2011-07-04
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""

Simple algorithm to get tis/tos-decisions for charm particles 

This file is a part of BENDER project:
    ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
    ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campain of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
## =============================================================================
__author__  = 'Vanya BELYAEV  Ivan.Belyaev@cern.ch'
__date__    = "2011-07-04"
__version__ = '$Revision$' 
# ===========================================================================================
import ROOT                           ## needed to produce/visualize the histograms
from   Bender.Main         import *   ## import all bender goodies 
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Gaudi.Configuration import *   ## needed for job configuration
# ===========================================================================================
import BenderTools.TisTos             ## add methods for TisTos 
# =============================================================================
## @class TisTosGet
#  Simple algorithm to get tis/tos-decisions for charm particles 
#  @date   2010-12-01
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
class TisTosGet(Algo) :
    """
    Simple algorithm to study trigger efficiencies 
    """

    ## initialize algorithm 
    def initialize ( self ) :
        """
        Initialization
        """
        sc = Algo.initialize        ( self )
        if sc.isFailure() : return sc

        ## initialize tis-tos machinery 
        sc = self.tisTos_initialize (      )
        if sc.isFailure() : return sc

        ## book the dictionary for trigger lines "per-particle" type
        lines = {}
        lines [ 'D0'  ] = {}
        lines [ 'D+'  ] = {}
        lines [ 'Ds+' ] = {}
        #
        ## D0:
        lines [ 'D0'  ][   'L0TOS' ] = 'L0HadronDecision'
        lines [ 'D0'  ][   'L0TIS' ] = 'L0(Hadron|DiMuon|Muon|Electron|Photon)Decision'
        lines [ 'D0'  ][ 'Hlt1TOS' ] = 'Hlt1TrackAllL0Decision'
        lines [ 'D0'  ][ 'Hlt1TIS' ] = 'Hlt1(Track|Muon|SingleMuon|DiMion).*Decision'
        lines [ 'D0'  ][ 'Hlt2TOS' ] = 'Hlt2CharmHadD02HH.*Decision'
        lines [ 'D0'  ][ 'Hlt2TIS' ] = 'Hlt2(Charm|Topo|DiMuon|Single).*Decision'
        #
        ## D+
        from copy import deepcopy
        lines [ 'D+'  ] = deepcopy ( lines [ 'D0'  ] ) 
        lines [ 'D+'  ][ 'Hlt2TOS' ] = 'Hlt2CharmHadD.*Decision'    ## ATTENTION
        #
        ## Ds+
        lines [ 'Ds+' ] = deepcopy ( lines [ 'D+'  ] ) 
        
        ## initialize tis-tos machinery 
        sc = self.tisTos_initialize ( lines = lines  )
        if sc.isFailure() : return sc

        return SUCCESS


    ## the only one essential method
    def analyse ( self ) :
        """
        Analyse it! 
        """

        ## get proper D0  decays 
        d0  = self.select ( 'D0'  , '[D0   -> K-  pi+ ]CC '     )        
        ## get proper D+  decays 
        dp  = self.select ( 'D+'  , '[D+   -> K-  pi+ pi+ ]CC ' )
        ## get proper Ds+ decays 
        ds  = self.select ( 'Ds'  , '[D_s+ -> K- K+ pi+ ]CC '   )

        tD0 = self.nTuple ('D0')
        for d in d0 :
            tD0.column  ( 'm'  ,  M ( d )  / GeV )
            tD0.column  ( 'pt' , PT ( d )  / GeV )
            self.tisTos ( d    , tD0 , 'D0_' , self.lines['D0'] )
            tD0.write()

        tDp = self.nTuple ('Dp')
        for d in dp :
            tDp.column  ( 'm'  ,  M ( d )  / GeV )
            tDp.column  ( 'pt' , PT ( d )  / GeV )
            self.tisTos ( d    , tDp , 'Dp_' , self.lines['D+'] )
            tDp.write()            

        tDs = self.nTuple ('Ds')
        for d in ds :
            tDs.column  ( 'm'  ,  M ( d )  / GeV )
            tDs.column  ( 'pt' , PT ( d )  / GeV )
            self.tisTos ( d    , tDs , 'Ds_' , self.lines['Ds+'] )
            tDs.write()            

        return SUCCESS


    ## finalize 
    def finalize ( self ) :

        ## finalize tis-tos machinery (& print decisions)
        self.tisTos_finalize () 

        return Algo.finalize ( self )


# =============================================================================
## configure the job
# =============================================================================
## configure the job 
def configure ( datafiles , catalogs = [] ) :
    """
    Job configuration 
    """
        
    from Configurables           import DaVinci       ## needed for job configuration
    from Configurables           import EventSelector ## needed for job configuration 
    from GaudiConf.Configuration import FileCatalog   ## needed for job configuration 
    from GaudiConf.Configuration import NTupleSvc     ## needed for job configuration 

    ## define the proper event prefilters filters 
    from PhysConf.Filters import LoKi_Filters    
    fltrs = LoKi_Filters (
        ## check the strinpping decision
        STRIP_Code = """
        HLT_PASS_RE ( 'Stripping.*ForPromptCharm.*Decision'  ) 
        """ ,
        ## check TES location
        VOID_Code  = """
        ( 0.5 < CONTAINS ('/Event/Charm/Phys/D02HHForPromptCharm/Particles'   ) )
        |
        ( 0.5 < CONTAINS ('/Event/Charm/Phys/DForPromptCharm/Particles'       ) )
        |
        ( 0.5 < CONTAINS ('/Event/Charm/Phys/DsForPromptCharm/Particles'      ) ) 
        """
        )
    filters = fltrs.filters('Filters')
    filters.reverse()

    ## confugure DaVinci 
    davinci = DaVinci (
        DataType        =  '2011' , 
        InputType       =  'MDST' ,
        Simulation      =   False ,
        PrintFreq       =     100 ,
        EventPreFilters = filters ,
        EvtMax          =      -1 ,
        ##
        HistogramFile   = 'TisTosGet_Histos.root' ,
        TupleFile       = 'TisTosGet.root'        ,
        ##
        Persistency     = 'ROOT'
        ##
        )

    ## use the latest tags
    from Configurables import CondDB
    CondDB ( UseLatestTags = ["2011"] )   
    
    ## configure TIS-TOS 
    rootInTES = '/Event/Charm' 
    
    # ------- decoding set-up start ----------
    
    from MicroDSTConf.TriggerConfUtils import configureL0AndHltDecoding
    configureL0AndHltDecoding(rootInTES)
    
    # ------- decoding set-up end  -----------
    
    
    # come back to Bender 
    
    setData ( datafiles , catalogs )
    
    gaudi = appMgr()
    
    ## Set properties of the TisTosTools
    for t in ( gaudi.tool ( 'TisTosGet.L0TriggerTisTos' ) ,
               gaudi.tool ( 'TisTosGet.TriggerTisTos'   ) ) : 
        
        t . UseParticle2LHCbIDsMap = 2
        t . PropertiesPrint        = True
        
    alg = TisTosGet (
        'TisTosGet'                     ,   ## Algorithm name ,
        RootInTES = rootInTES             ,
        Inputs = [ 'Phys/D02HHForPromptCharm/Particles'   ,
                   'Phys/DForPromptCharm/Particles'       ,
                   'Phys/DsForPromptCharm/Particles'      ]
        )
    #
    mainSeq = gaudi.algorithm ('GaudiSequencer/DaVinciUserSequence', True )
    mainSeq.Members += [ alg.name() ]
    
    return SUCCESS 

 
# =============================================================================
# The actual job steering
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'
    
    files = [
        ## Charm microDST, stripping 13b 
        "/castor/cern.ch/grid/lhcb/LHCb/Collision11/CHARM.MDST/00010646/0000/00010646_000000%02d_1.charm.mdst" % i for i in range(1,20) 
        ]
    
    configure ( files  )

    run ( 1000 )

# =============================================================================
# The END 
# =============================================================================
