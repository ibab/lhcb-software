#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file BenderExample/Jibo.py
#  The simple Bender-based example to read J/psi from microDST 
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
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2012-04-24
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
The simple Bender-based example to read J/psi from microDST 

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

                  $Revision$
Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = " 2012-02-24 " 
__version__ = " Version $Revision$"
# =============================================================================
## import everything from bender 
from   Bender.Main               import *
from   GaudiKernel.SystemOfUnits import MeV , GeV, mm 
import BenderTools.TisTos 
# =============================================================================
## Simple class to look for J/psi peak 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2012-04-24
class Jpsi(Algo) :
    """
    Simple class to look for J/psi peak
    """
    ## the standard initialization 
    def initialize ( self ) :
        """
        Initialization
        """
        sc = Algo.initialize        ( self )
        if sc.isFailure() : return sc

        
        #
        ## for TisTos, optional
        #
        triggers         = {} 
        triggers[ 'psi'] = {}
        
        lines         = {}
        lines ['psi'] = {}
        lines ['psi'] ['L0TOS'  ] = 'L0(Muon|DiMuon).*Decision'
        lines ['psi'] ['L0TIS'  ] = 'L0(Hadron|Muon|DiMuon|Photon|Electron)Decision'
        lines ['psi'] ['Hlt1TOS'] = 'Hlt1(DiMuon|SingleMuonHighPT|MuonTrack).*Decision'
        lines ['psi'] ['Hlt1TIS'] = 'Hlt1TrackAll.*Decision'
        lines ['psi'] ['Hlt2TOS'] = 'Hlt2(DiMuon|SingleMuonHighPT).*Decision'
        lines ['psi'] ['Hlt2TIS'] = 'Hlt2(Charm|Topo|Single|Express|Inc|Tri).*Decision'
        
        sc = self.tisTos_initialize ( triggers , lines )
        if sc.isFailure() : return sc
        
        return SUCCESS
    
    ## standard mehtod for analysis
    def analyse( self ) :
        """
        The standard method for analysis
        """
        
        ## get J/psi from input particles 
        jpsi = self.select  ( 'psi' , 'J/psi(1S) -> mu+ mu-' )
        if jpsi.empty()  :
            return self.Warning ( "No J/psi are found!" , SUCCESS )

        ## some counter 
        cnt = self.counter('#J/psi')
        cnt += len ( jpsi ) 
        
        ## loop over selected J/psi 
        for j in jpsi :
            
            ## print it! 
            print j 
            
            
            ## collect TisTos-statistics (optional)
            self.decisions      ( j , self.triggers[ 'psi' ] )

            ## mass-plot 
            self.plot ( M ( j ) / GeV , 'm(psi)' , 2.75 , 4.0 , 250 )


        #
        ## predefine some functors:
        #
        mJpsi    = DTF_FUN     ( M       , True ) ## Jpsi mass with PV-constraint
        chi2_dtf = DTF_CHI2NDOF(           True ) ## chi2(DTF) for J/psi
        minDLLmu = MINTREE     ( ISBASIC , PIDmu - PIDpi ) ## muon ID 
        kullback = MINTREE     ( ISBASIC & HASTRACK , CLONEDIST ) ## KL-distance
            
        ## book n-=tuple
        tup = self.nTuple('J/psi')

        ## loop over J/psi and fill n-tuple
        for j in jpsi :

            tup.column ( 'm'  , M  ( j ) / GeV )   ## mass                 ( in GeV/c2 )
            tup.column ( 'pt' , PT ( j ) / GeV )   ## transverse momentum  ( in GeV/c  )
            tup.column ( 'y'  , Y  ( j )       )   ## rapidity 

            tup.column ( 'p4' , j.momentum() / GeV ) ## 4-momntum (4 component) in GeV/c2 

            
            ## get the children
            mu1 = j.child(1)     ## the first muon 
            mu2 = j.child(2)     ## the secon muon
            
            tup.column ( 'pt1' , PT ( mu1 ) / GeV )  ## pT of the first  muon ( in GeV/c ) 
            tup.column ( 'pt2' , PT ( mu2 ) / GeV )  ## pT of the second muon ( in GeV/c ) 

            ## the worst muon ID
            tup.column ( 'minDLLmu' , minDLLmu ( j ) )
            ## the minimal KL-distance
            tup.column ( 'minKL'    , kullback ( j ) )
            
            ## vertex
            vx = j.endVertex() 
            tup.column ( 'chi2_vx' , VCHI2 ( vx ) ) ## chi2(vx) for J/psi
            tup.column ( 'vx'      , vx.position() ) ## decay vertex (3 components) 

            ## primary vertex 
            pv = self.bestVertex ( j )
            if not pv : continue
            
            tup.column('pv' , pv.position() ) ## decay vertex (3 components) 
            
            ## get some kind of "time", see LoKi::Particles::TimeDotDistance 
            ctau = TDOT  ( pv ) 
            tup.column   ( 'ctau' , ctau ( j ) )
            
            #
            ## fill TisTos info
            self.tisTos ( j                 ,
                          tup               ,
                          'Jpsi_'           ,
                          self.lines['psi'] ) 
            
            tup.write()


            
        self.setFilterPassed( True )
        
        return SUCCESS
        
    ## finalization 
    def finalize ( self ) :
        """
        Standard finalization 
        """
        #
        histos = self.Histos()
        for key in histos :
            h = histos[key]
            if hasattr ( h , 'dump' ) : print h.dump(50,30,True)
        #
        Algo.tisTos_finalize ( self )
        return Algo.finalize ( self )

# =============================================================================
## configure the job
def configure ( datafiles , catalogs  = [] , castor = False ) :
    """
    Configure the job
    """
    from Configurables           import DaVinci       ## needed for job configuration
    from Configurables           import EventSelector ## needed for job configuration 
    
    from PhysConf.Filters import LoKi_Filters
    
    fltrs = LoKi_Filters (
        STRIP_Code = """
        HLT_PASS_RE ( 'Stripping.*DiMuonInc.*Decision' )
        """ ,
        VOID_Code  = """
        0 < CONTAINS ('/Event/Leptonic/Phys/MicroDSTDiMuonDiMuonIncLine/Particles') 
        """
        )
    filters = fltrs.filters('Filters')
    
    davinci = DaVinci (
        DataType      = '2012' ,
        PrintFreq     = 1000   ,
        EventPreFilters = filters ,
        EvtMax        = -1     ,
        ##
        HistogramFile = 'Jpsi_Histos.root' ,
        TupleFile     = 'Jpsi_Tuples.root' ,
        ##
        Lumi          = True ,
        ##
        )

    
##     from Configurables import CondDB
##     CondDB ( UseLatestTags = ["2011"] , 
##              UseOracle     = True     )
    
    RootInTES = 'Leptonic'
    
    from BenderTools.MicroDST import uDstConf 
    uDstConf(RootInTES)
    
    from Configurables import Gaudi__IODataManager as IODataManager
    IODataManager().AgeLimit = 2 
    
    # come back to Bender 
    setData ( datafiles , catalogs , castor )
    
    gaudi = appMgr()
    
    alg = Jpsi (
        'Jpsi'                      ,   ## Algorithm name ,
        # input particles 
        Inputs    = [ 'Phys/MicroDSTDiMuonDiMuonIncLine' ] ,
        RootInTES = RootInTES 
        )
    #
    mainSeq = gaudi.algorithm ('GaudiSequencer/DaVinciUserSequence', True )
    mainSeq.Members += [ alg.name() ]
    
    return SUCCESS 

# =============================================================================
# The actual job steering
if '__main__' == __name__ :
    
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  
     
    data = [
        'PFN:castor:/castor/cern.ch/user/j/jhe/PreStripping18/Run97120_DVv30r1p2_Leptonic.mdst'
        ]
    
    configure ( data , castor = True )
    
    run ( 200 )
    
    gaudi = appMgr()
    
    alg           = gaudi.algorithm( 'Jpsi')
    alg.StatPrint = True
    
    # 
    histos        = alg.Histos()
    for key in histos :
        h = histos[key]
        if hasattr ( h , 'dump' ) : print h.dump(50,30,True)

# =============================================================================
# The END 
# =============================================================================
