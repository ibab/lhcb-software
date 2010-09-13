#!/usr/bin/env python
# =============================================================================
# $Id: SelectDplus.py,v 1.1 2010-09-13 13:24:04 ibelyaev Exp $ 
# =============================================================================
# $URL$ 
# =============================================================================
## @file solution/SelectDplus.py
#
#  Simple example that illustrates the selection of D+/D_s+ from stripped DSTs
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
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-09-10
#
#  Last modification $Date: 2010-09-13 13:24:04 $
#                 by $Author: ibelyaev $
# =============================================================================
"""
Simple example that illustrates the selection of D+/D_s+ from stripped DSTs

This file is a part of BENDER project:
    ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
    ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date: 2010-09-13 13:24:04 $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2010-10-10 " 
__version__ = " Version $Revision: 1.1 $ "
# =============================================================================
## import everything from BENDER
from Bender.Main import *
# =============================================================================
## @class SelectDplus
#  Simple algoritm to select D+/D_s+ from stripped DSTs 
class SelectDplus(Algo):
    
    # =========================================================================
    ## the main method: analyse the event
    def analyse( self ) :
        """
        the main method: analyse the event
        """
        
        # select from input particles according to the decay pattern:
        d_1  = self.select ( 'd1'  , "[ D+ -> K- pi+ pi+]CC " )
        d_2  = self.select ( 'd2'  , "[ D+ -> K- K+  pi+]CC " )
        
        ds_1 = self.select ( 'ds1' , "[ D_s+ -> K- K+  pi+]CC "                 )
        ds_2 = self.select ( 'ds2' , "[ D_s+ -> ( phi(1020) -> K- K+ ) pi+]CC " )

        # retrieve N-tuple (book-on-demand)
        tup = self.nTuple('My N-tuple')
        for d in d_1  :
            print 'D1 : ' , d.decay() , M(d)
            self.plot ( M(d) / GeV , 'D^{+} #rightarrow K^{-} #pi^{+} #pi^{+}'            , 1.8 , 1.9 , 100 )
            
            pv = self.bestVertex( d )
            if not pv : continue
            
            fun  = LTIME ( self.lifetimeFitter() , pv ) * c_light

            ctau = fun ( d )
            if 0.1 < ctau < 1.0 : 
                self.plot ( ctau  , 'c*tau for D^{+}' , 0.1 , 1 , 90 ) 

            # add the columns into n-tuple 
            tup.column ( 'm'    , M  ( d ) )
            tup.column ( 'pt'   , PT ( d ) )
            tup.column ( 'ctau' , ctau )
            # commit the row 
            tup.write()
            
        for d in d_2  :
            print 'D2 : ' , d.decay() , M(d)
            self.plot ( M(d) / GeV , 'D^{+}/D_{s}^{+} #rightarrow K^{-} K^{+} #pi^{+} 1 ' , 1.8 , 2.050 , 250 ) 
            
        for d in ds_1 :
            print 'Ds1: ' , d.decay() , M(d)
            self.plot ( M(d) / GeV , 'D^{+}/D_{s}^{+} #rightarrow K^{-} K^{+} #pi^{+} 2 ' , 1.8 , 2.050 , 250 ) 
            
        for d in ds_2 :
            print 'Ds2: ' , d.decay() , M(d)  
            self.plot ( M(d) / GeV , 'D_{s}^{+} #rightarrow ( #phi #rightarrow K^{-} K^{+}) #pi^{+} ' , 1.770 , 2.070 , 300 ) 

        return SUCCESS


    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        #
        self.dumpHistos ( 50 , 30 , True )
        #
        return Algo.finalize ( self )
    


## The configuration of the job
def configure( inputdata , catalogs = [] ) :
    """
    The configuration of the job
    """
    
    ## define event-levele filters:
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters (
        HLT_Code   = " HLT_PASS_RE ('Hlt1MBMicro.*Decision') | HLT_PASS_RE ('Hlt1.*Hadron.*Decision') " ,
        ## get Rio's lines for D->hhh and Conor's line for Ds->phi pi
        STRIP_Code = """
        HLT_PASS    ( 'StrippingDs2piPhiLineDecision'                       ) |
        HLT_PASS_RE ('StrippingStripD2K(KP|PP)_A_LoosePID_(Sig|Bkg).*Decision') 
        """      ,
        ## select one or 2 vertices 
        VOID_Code  = " CONTAINS    ('/Event/Charm/Rec/Vertex/Primary') < 3.1 " 
        )
    
    from Configurables import DaVinci , GaudiSequencer 
    DaVinci ( DataType        = '2010' ,
              EventPreFilters = fltrs.filters ('Filters') ,
              Lumi            = False  ,
              HistogramFile   = 'Dplus_Histos.root' ,
              TupleFile       = 'Dplus_Tuples.root' )
    
    from Configurables import CondDB
    CondDB  ( IgnoreHeartBeat = True )
    
    setData ( inputdata , catalogs )
    
    ## get/create Application Manager
    gaudi = appMgr()
    
    # modify/update the configuration:
    
    ## create the algorithm
    alg = SelectDplus (
        'Dplus'                               , 
        ## Reco05-Stripping09 : 
        # RootInTES      = '/Event/Hadronic'    ,
        ## Reco04-Stripping07 :
        RootInTES      = '/Event/Charm'    ,
        # 
        InputLocations = [
        'Ds2piPhiLine'                               , ## Conor's line 
        ##
        'StrippingStripD2KPP_A_LoosePID_SigFilter'   , ## Rio's lines 
        'StrippingStripD2KPP_A_LoosePID_BkgFilter'   ,
        ##
        'StrippingStripD2KKP_A_LoosePID_SigFilter'   ,
        'StrippingStripD2KKP_A_LoosePID_BkgFilter'   ,
        ] 
        )
    
    ## add algorithm into main DaVinci sequence
    dvMain = gaudi.algorithm('GaudiSequencer/DaVinciMainSequence' , True )
    dvMain.Members += [ alg.name() ]
    
    return SUCCESS 
# =============================================================================


# =============================================================================
## Job steering 
if __name__ == '__main__' :
    
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120
    
    ## job configuration
    inputdata = [
        ## dbpath: /LHCb/Collision10/Beam3500GeV-VeloClosed-MagDown/Reco05-Striping09-Merged/90000000/HADRONIC.DST
        '/castor/cern.ch/grid' + '/lhcb/data/2010/HADRONIC.DST/00007580/0000/00007580_0000%04d_1.hadronic.dst' % i for i in range ( 2 , 9900 )
        ## dbpath: /LHCb/Collision10/Beam3500GeV-VeloClosed-MagDown/Reco04-Striping07/90000000/CHARM.DST
        #'/castor/cern.ch/grid' + '/lhcb/data/2010/CHARM.DST/00007502/0000/00007502_0000%04d_1.charm.dst' % i for i in range ( 1 , 2400 )
        ]
        
    configure( inputdata )
    
    ## event loop 
    run(10 )
    
# =============================================================================
# The END
# =============================================================================
