#!/usr/bin/env python
# =============================================================================
# @file BenderTools/Fill.py
#
# Decorate the algorithm with proper methods to fill n-tuples 
#  
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  <b>``C++ ToolKit for Smart and Friendly Physics Analysis''</b>
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software''
#
# @author Vanya BELYAEV Ivan.Belyaev@cern.ch
# @date   2011-06-21
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
"""
Decorate the algorithm with proper methods to fill n-tuples

This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''

"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__date__    = "2011-06-07"
# =============================================================================
__all__= (
    'treatPions'    , ## add information about pions 
    'treatKaons'    , ## add information about kaons 
    'treatProtons'  , ## add information about protons
    'treatPhotons'  , ## add information about photons
    'treatMuons'    , ## add information about muons 
    'treatTracks'   , ## add information about the tracks
    'treatKine'     , ## add detailed information for particle ,
    'fillMasses'    , ## masses of sub-combinations
    'addRecSummary' , ## add rec-summary information
    'addGecInfo'    , ## add some GEC-info 
    )
# ==============================================================================
from   Bender.Main                   import LoKi, SUCCESS  
from   LoKiPhys.decorators           import *
from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm 

# ==============================================================================
## initialize te internal machinery
def _fill_initialize ( self ) :
    """
    Initialize the internal machinery 
    """
    self._pions         = LoKi.Child.Selector ( 'pi+'   == ABSID )
    self._kaons         = LoKi.Child.Selector ( 'K+'    == ABSID )
    self._protons       = LoKi.Child.Selector ( 'p+'    == ABSID )
    self._muons         = LoKi.Child.Selector ( 'mu+'   == ABSID )
    self._gamma         = LoKi.Child.Selector ( 'gamma' == ID    )
    self._pi0           = LoKi.Child.Selector ( 'pi0'   == ID    )
    self._tracks        = LoKi.Child.Selector (       HASTRACK   )
    self._basic         = LoKi.Child.Selector (        ISBASIC   )
    #
    self._ctau          = BPVLTIME (    ) * c_light
    self._ctau_9        = BPVLTIME (  9 ) * c_light
    self._ctau_25       = BPVLTIME ( 25 ) * c_light
    self._lv01          = LV01  
    self._vchi2         = VFASPF       ( VCHI2     )
    self._vchi2ndf      = VFASPF       ( VCHI2PDOF )
    self._dtfchi2       = DTF_CHI2NDOF ( True      )
    self._ipchi2        = BPVIPCHI2    ()
    self._dls           = LoKi.Particles.DecayLengthSignificanceDV () 
    #
    self._min_dll_K     = MINTREE ( 'K+'  == ABSID   , PIDK  - PIDpi ) 
    self._min_dll_Pi    = MINTREE ( 'pi+' == ABSID   , PIDpi - PIDK  ) 
    self._min_dll_PK    = MINTREE ( 'p+'  == ABSID   , PIDp  - PIDK  ) 
    self._min_dll_Ppi   = MINTREE ( 'p+'  == ABSID   , PIDp  - PIDpi ) 
    self._min_dll_Mu    = MINTREE ( 'mu+' == ABSID   , PIDmu - PIDpi )
    #
    self._min_Pt        = MINTREE ( ISBASIC & HASTRACK , PT  ) / GeV 
    self._min_Eta       = MINTREE ( ISBASIC & HASTRACK , ETA ) 
    self._max_Eta       = MAXTREE ( ISBASIC & HASTRACK , ETA )
    #
    self._min_CL_gamma  = MINTREE ( 'gamma' == ID , CL ) 
    self._min_Et_gamma  = MINTREE ( 'gamma' == ID , PT ) / GeV 
    #
    self._maxTrChi2     = MAXTREE ( ISBASIC & HASTRACK , TRCHI2DOF    )
    self._maxTrGhost    = MAXTREE ( ISBASIC & HASTRACK , TRGHOSTPROB  )
    self._minTrIPchi2   = MINTREE ( ISBASIC & HASTRACK , BPVIPCHI2()  )
    #
    ##
    self._EtC           = PINFO   ( 55001 , -100 * GeV ) 
    self._PtC           = PINFO   ( 55002 , -100 * GeV )  
    self._maxEtC        = MAXTREE ( 'mu+' == ABSID , self._EtC ) 
    self._maxPtC        = MAXTREE ( 'mu+' == ABSID , self._PtC )
    ## 
    self._EcalE         = PPINFO  ( LHCb.ProtoParticle.CaloEcalE , -100 * GeV ) 
    self._HcalE         = PPINFO  ( LHCb.ProtoParticle.CaloHcalE , -100 * GeV )  
    self._maxEcalE      = MAXTREE ( 'mu+' == ABSID , self._EcalE )
    self._maxHcalE      = MAXTREE ( 'mu+' == ABSID , self._HcalE )
    #
    self._delta_m2      = LoKi.PhysKinematics.deltaM2 
    #
    return SUCCESS 

# ==============================================================================
## finalize 
def _fill_finalize   ( self ) :
    """
    Finalie the internal machinery 
    """
    self._pions        = None 
    self._kaons        = None 
    self._protons      = None 
    self._muons        = None 
    self._gamma        = None 
    self._pi0          = None 
    self._tracks       = None 
    self._basic        = None 
    #
    self._ctau         = None 
    self._ctau_9       = None 
    self._ctau_25      = None 
    self._lv01         = None 
    self._vchi2        = None 
    self._vchi2ndf     = None 
    self._dtfchi2      = None 
    self._ipchi2       = None 
    self._dls          = None 
    #
    self._min_dll_K    = None 
    self._min_dll_Pi   = None 
    self._min_dll_PK   = None 
    self._min_dll_Ppi  = None 
    self._min_dll_Mu   = None
    #
    self._min_Pt       = None 
    self._min_Eta      = None 
    self._max_Eta      = None
    #
    self._min_Et_gamma = None 
    self._min_CL_gamma = None 
    #
    self._maxTrChi2    = None 
    self._maxTrGhost   = None 
    self._minTrIPchi2  = None 
    #
    self._EtC          = None 
    self._PtC          = None 
    self._maxEtC       = None 
    self._maxPtC       = None 
    ## 
    self._EcalE        = None
    self._HcalE        = None 
    self._maxEcalE     = None 
    self._maxHcalE     = None 
    #
    self._delta_m2     = None 
    #
    return SUCCESS 


# =============================================================================
## add pion information into n-tuple
#  @param tup   n-tuple
#  @param p     the particle 
def treatPions ( self         ,
                 tup          ,
                 p            ,
                 suffix  = '' ) :
    
    ## 
    if hasattr ( p , 'particle' ) : p = p.particle() 
    #
    ## get all pions from decay tree
    #
    good  = LHCb.Particle.ConstVector()
    p.children ( self._pions , good ) 
    #
    sc = tup.column_float ( 'mindll_piK' + suffix , self._min_dll_Pi ( p ) ) 
    #
    return tup.fArrayP ( 'p_pion'   + suffix          , P   / GeV    ,  
                         'pt_pion'  + suffix          , PT  / GeV    , 
                         'eta_pion' + suffix          , ETA          , 
                         'pid_pion' + suffix          , PIDpi - PIDK ,  
                         LHCb.Particle.Range ( good ) ,
                         'n_pion'   + suffix          , 10           )

# ==============================================================================
## add kaon information into n-tuple
#  @param tup   n-tuple
#  @param p     the particle 
def treatKaons ( self         ,
                 tup          ,
                 p            ,
                 suffix  = '' ) :
    
    ## 
    if hasattr ( p , 'particle' ) : p = p.particle() 
    #
    ## get all kaons form decay tree
    #
    good  = LHCb.Particle.ConstVector()
    p.children ( self._kaons , good ) 
    #
    sc = tup.column_float ( 'mindll_K' + suffix , self._min_dll_K  ( p ) ) 
    #
    return tup.fArrayP ( 'p_kaon'   + suffix          , P   / GeV    , 
                         'pt_kaon'  + suffix          , PT  / GeV    , 
                         'eta_kaon' + suffix          , ETA          , 
                         'pid_kaon' + suffix          , PIDK - PIDpi ,  
                         LHCb.Particle.Range ( good ) ,
                         'n_kaon'   + suffix          , 10        )

# ==============================================================================
## add proton information into n-tuple
#  @param tup   n-tuple
#  @param p     the particle 
def treatProtons ( self         ,
                   tup          ,
                   p            ,
                   suffix  = '' ) :
    
    ## 
    if hasattr ( p , 'particle' ) : p = p.particle() 
    #
    ## get all protons form decay tree
    #
    good   = LHCb.Particle.ConstVector()
    p.children ( self._protons , good ) 
    #
    sc = tup.column_float ( 'mindll_pK'  + suffix , self._min_dll_PK  ( p ) ) 
    sc = tup.column_float ( 'mindll_ppi' + suffix , self._min_dll_Ppi ( p ) ) 
    #
    tup.fArrayP ( 'p_proton'    + suffix       , P   / GeV , 
                  'pt_proton'   + suffix       , PT  / GeV , 
                  'eta_proton'  + suffix       , ETA       , 
                  LHCb.Particle.Range ( good ) ,
                  'n_proton'    + suffix       , 10        )
    
    return tup.fArrayP ( 'pid_proton_pi' + suffix , PIDp - PIDpi ,
                         'pid_proton_K'  + suffix , PIDp - PIDK  ,
                         LHCb.Particle.Range ( good ) ,
                         'n_proton'    + suffix       , 10        )

# ==============================================================================
## add photon information into n-tuple
#  @param tup   n-tuple
#  @param p     the particle 
def treatPhotons ( self         ,
                   tup          ,
                   p            ,
                   suffix  = '' ) :
    
    ## 
    if hasattr ( p , 'particle' ) : p = p.particle() 
    #
    ## get all protons form decay tree
    #
    good   = LHCb.Particle.ConstVector()
    p.children ( self._gamma , good ) 
    #
    sc = tup.column_float ( 'minEt_gamma'  + suffix , self._min_Et_gamma ( p ) ) 
    sc = tup.column_float ( 'minCl_gamma'  + suffix , self._min_CL_gamma ( p ) ) 
    #
    tup.fArrayP ( 'e_photon'    + suffix       , P   / GeV , 
                  'et_photon'   + suffix       , PT  / GeV , 
                  'CL_photon'   + suffix       , CL        , 
                  LHCb.Particle.Range ( good ) ,
                  'n_photon'    + suffix       , 10        )
    tup.fArrayP ( 'eta_photon'  + suffix       , ETA       , 
                  'tx_photon'   + suffix       , PX / PZ   , 
                  'ty_photon'   + suffix       , PY / PZ   , 
                  LHCb.Particle.Range ( good ) ,
                  'n_photon'    + suffix       , 10        )

# ==============================================================================
## add muon information into n-tuple
#  @param tup   n-tuple
#  @param p     the particle 
def treatMuons ( self         ,
                 tup          ,
                 p            ,
                 suffix  = '' ) :
    
    ## 
    if hasattr ( p , 'particle' ) : p = p.particle() 
    #
    ## get all muons from decay tree
    #
    good    = LHCb.Particle.ConstVector()
    p.children ( self._muons , good ) 
    #
    sc = tup.column_float ( 'mindll_mu'   + suffix , self._min_dll_Mu    ( p ) ) 
    #
    sc = tup.fArrayP   ( 'p_mu'       + suffix          , P   / GeV     ,  
                         'pt_mu'      + suffix          , PT  / GeV     , 
                         'eta_mu'     + suffix          , ETA           , 
                         'pid_mu'     + suffix         , PIDmu - PIDpi ,  
                         LHCb.Particle.Range ( good ) ,
                         'n_muon'     + suffix ,  10  )
    return tup.fArrayP ( 'eEcal_mu'   + suffix , self._EcalE ,
                         'eHcal_mu'   + suffix , self._HcalE ,
                         LHCb.Particle.Range ( good )        ,
                         'n_muon'     + suffix , 10          )

# ==============================================================================
## add tracks information into n-tuple
#  @param tup   n-tuple
#  @param p     the particle 
def treatTracks ( self         ,
                  tup          ,
                  p            ,
                  suffix  = '' ) :
    
    ## 
    if hasattr ( p , 'particle' ) : p = p.particle() 
    #
    ## get all tracks from decay tree
    #
    from math import sqrt, acos 
    #
    #
    good   = LHCb.Particle.ConstVector()
    p.children ( self._tracks , good ) 
    #
    ## calcualate the minimal mass variable 
    m2min = 2.0
    for i in range ( 0 , good.size() ) :
        p_i = good[i]
        q_i = Q ( p_i )
        
        for j in range ( i + 1 , good.size() ) :
            p_j = good [j]
            q_j = Q ( p_j )
            
            if q_i * q_j < 0 : continue
            
            m2    = self._delta_m2 ( p_i , p_j )
            m2min = min ( m2min , m2 )
            
    sc = tup.column_float ( 'm2min_track'   + suffix , m2min                  )    
    sc = tup.column_float ( 'minPt_track'   + suffix , self._min_Pt     ( p ) )
    sc = tup.column_float ( 'minEta_track'  + suffix , self._min_Eta    ( p ) )
    sc = tup.column_float ( 'maxEta_track'  + suffix , self._max_Eta    ( p ) )
    sc = tup.column_float ( 'maxChi2_track' + suffix , self._maxTrChi2  ( p ) )
    sc = tup.column_float ( 'maxTrGh_track' + suffix , self._maxTrGhost ( p ) )
    
    sc = tup.fArrayP ( 'p_track'     + suffix        , P   / GeV  , 
                       'pt_track'    + suffix        , PT  / GeV  , 
                       'eta_track'   + suffix        , ETA        , 
                       'PChi2_track' + suffix        , TRPCHI2    , 
                       LHCb.Particle.Range ( good )               , 
                       'n_track'     + suffix        , 20         )
    
    sc = tup.fArrayP ( 'chi2_track'  + suffix        , TRCHI2DOF ,
                       'phi_track'   + suffix        , PHI       ,                        
                       LHCb.Particle.Range ( good )              , 
                       'n_track'     + suffix        , 20        )
    return sc

# =============================================================================
## add basic kinematical information into N-tuple
def treatKine ( self          ,
                tup           ,
                p             ,
                suffix        ,
                good   = None ) :
    """
    Add basic kinematical information into N-tuple
    """
    if hasattr ( p , 'particle' ) : p = p.particle() 
    ##
    tup.column_int   ( 'pid'   + suffix , int ( ID      ( p ) )     )
    tup.column_float ( 'pt'    + suffix ,       PT      ( p ) / GeV )
    tup.column_float ( 'm'     + suffix ,        M      ( p ) / GeV )
    tup.column_float ( 'eta'   + suffix ,      ETA      ( p )       )
    tup.column_float ( 'phi'   + suffix ,      PHI      ( p )       )
    #
    ## 4-vector
    #
    tup.column       ( 'p4'    + suffix , p.momentum() / GeV  )
    tup.column_float ( 'c2ip'  + suffix , self._ipchi2  ( p ) )
    ##
    if not p.isBasicParticle() :
        
        tup.column_float ( 'y'     + suffix , Y          ( p ) )
        tup.column_float ( 'lv01'  + suffix , self._lv01 ( p ) )
        
        if p.endVertex() :
            
            tup.column_float ( 'ctau'    + suffix , self._ctau     ( p ) )
            tup.column_float ( 'ctau9'   + suffix , self._ctau_9   ( p ) )
            tup.column_float ( 'ctau25'  + suffix , self._ctau_25  ( p ) )
            tup.column_float ( 'dtf'     + suffix , self._dtfchi2  ( p ) )
            tup.column_float ( 'dls'     + suffix , self._dls      ( p ) )
            tup.column_float ( 'vchi2'   + suffix , self._vchi2    ( p ) )
            tup.column_float ( 'vchi2ndf'+ suffix , self._vchi2ndf ( p ) )
        
    ##
    ok = True 
    if good : ok = good ( p )
    
    tup.column_bool ( 'good' + suffix ,  ok )
        
    return SUCCESS

# =============================================================================
## fill information about masses 
def fillMasses ( self        ,
                 tup         ,
                 b           ,
                 suffix = '' ) :
    """
    fill infomation about masses         
    """
    nc = b.nChildren()
    for i in range ( 1 , nc + 1 )  :
        for j in range ( i + 1 , nc + 1 ) :
            m2 = MASS ( i , j )
            tup.column_float ( 'm%s%s' % ( i , j ) + suffix , m2 ( b ) / GeV )
            for k in range ( j + 1 , nc + 1 ) :
                m3 = MASS ( i , j , k )
                tup.column_float ( 'm%s%s%s' % ( i , j , k ) + suffix , m3 ( b ) / GeV )
                for l in range ( k + 1 , nc + 1 ) :
                    m4 = MASS ( i , j , k , l )
                    tup.column_float ( 'm%s%s%s%s' % ( i , j , k , l ) + suffix , m4 ( b ) / GeV )
                    
    return SUCCESS

# =============================================================================
## add some event summary information
#  @see LHCb::RecSummary
def addRecSummary ( self  ,
                    tup   ,
                    data  ) :
    """
    Add event summary information
    """
    #
    nPV     = int ( data ( LHCb.RecSummary.nPVs               ) ) 
    nLong   = int ( data ( LHCb.RecSummary.nLongTracks        ) )
    nDown   = int ( data ( LHCb.RecSummary.nDownstreamTracks  ) ) 
    nUp     = int ( data ( LHCb.RecSummary.nUpstreamTracks    ) ) 
    nVelo   = int ( data ( LHCb.RecSummary.nVeloTracks        ) ) 
    nTT     = int ( data ( LHCb.RecSummary.nTTracks           ) ) 
    nBack   = int ( data ( LHCb.RecSummary.nBackTracks        ) ) 
    #
    hRich1  = int ( data ( LHCb.RecSummary.nRich1Hits         ) ) 
    hRich2  = int ( data ( LHCb.RecSummary.nRich2Hits         ) )
    hVelo   = int ( data ( LHCb.RecSummary.nVeloClusters      ) ) 
    hIT     = int ( data ( LHCb.RecSummary.nITClusters        ) ) 
    hTT     = int ( data ( LHCb.RecSummary.nTTClusters        ) ) 
    hOT     = int ( data ( LHCb.RecSummary.nOTClusters        ) ) 
    hSPD    = int ( data ( LHCb.RecSummary.nSPDhits           ) ) 
    
    tup.column ( 'nPV'       , nPV    , 0 ,    30 )
    tup.column ( 'nLong'     , nLong  , 0 ,  1000 ) 
    tup.column ( 'nDown'     , nDown  , 0 ,   500 ) 
    tup.column ( 'nUp'       , nUp    , 0 ,   500 ) 
    tup.column ( 'nVelo'     , nVelo  , 0 ,  1000 ) 
    tup.column ( 'nTT'       , nTT    , 0 ,   500 ) 
    tup.column ( 'nBack'     , nBack  , 0 ,   200 ) 
    
    tup.column ( 'hSPD'      , hSPD   , 0 ,  2000 ) 
    tup.column ( 'hRich1'    , hRich1 , 0 , 30000 ) 
    tup.column ( 'hRich2'    , hRich2 , 0 , 30000 ) 
    tup.column ( 'hVelo'     , hVelo  , 0 , 60000 ) 
    tup.column ( 'hIT'       , hIT    , 0 , 30000 ) 
    tup.column ( 'hTT'       , hTT    , 0 , 30000 ) 
    tup.column ( 'hOT'       , hOT    , 0 , 30000 ) 
    
    return SUCCESS

# =============================================================================
## add some event summary information 
def addGecInfo  ( self  ,
                  tup   ,
                  data  ) :
    """
    Add global event counters 
    """
    for key in data :
        
        v = int ( data[key] )
        
        tup.column_int ( key + '_gec', v )
        
    return SUCCESS


# =============================================================================
## decorate 
LoKi.Algo.treatPions      =  treatPions
LoKi.Algo.treatKaons      =  treatKaons
LoKi.Algo.treatProtons    =  treatProtons
LoKi.Algo.treatPhotons    =  treatPhotons
LoKi.Algo.treatMuons      =  treatMuons
LoKi.Algo.treatTracks     =  treatTracks
LoKi.Algo.treatKine       =  treatKine

LoKi.Algo.fillMasses      =  fillMasses

LoKi.Algo.addRecSummary   =  addRecSummary 
LoKi.Algo.addGecInfo      =  addGecInfo 

LoKi.Algo.fill_initialize = _fill_initialize
LoKi.Algo.fill_finalize   = _fill_finalize

# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'

# ==============================================================================
# The END 
# ==============================================================================
