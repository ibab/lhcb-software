#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
"""Decorate the algorithm with proper methods to fill n-tuples

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    
                                                                 
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
from   LoKiCore.basic                import cpp,LoKi 
from   LoKiPhys.decorators           import *
from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.Fill' )
else                      : logger = getLogger ( __name__ )
# ==============================================================================
SUCCESS                 = cpp.StatusCode( cpp.StatusCode.SUCCESS , True )
FAILURE                 = cpp.StatusCode( cpp.StatusCode.FAILURE , True )
# ==============================================================================
## initialize te internal machinery
def fill_initialize ( self ) :
    """
    Initialize the internal machinery 
    """
    self._pions           = LoKi.Child.Selector ( 'pi+'   == ABSID )
    self._kaons           = LoKi.Child.Selector ( 'K+'    == ABSID )
    self._protons         = LoKi.Child.Selector ( 'p+'    == ABSID )
    self._muons           = LoKi.Child.Selector ( 'mu+'   == ABSID )
    self._gamma           = LoKi.Child.Selector ( 'gamma' == ID    )
    self._digamma         = LoKi.Child.Selector ( DECTREE ( ' ( pi0 | eta ) -> <gamma> <gamma>' ) ) 
    self._pi0             = LoKi.Child.Selector ( 'pi0'   == ID    )
    self._tracks          = LoKi.Child.Selector (       HASTRACK   )
    self._basic           = LoKi.Child.Selector (        ISBASIC   )
    #
    self._ctau            = BPVLTIME (    ) * c_light
    self._ctau_9          = BPVLTIME (  9 ) * c_light
    self._ctau_25         = BPVLTIME ( 25 ) * c_light
    self._lv01            = LV01  
    self._vchi2           = VFASPF       ( VCHI2     )
    self._vchi2ndf        = VFASPF       ( VCHI2PDOF )
    self._dtfchi2         = DTF_CHI2NDOF ( True      )
    self._ipchi2          = BPVIPCHI2    ()
    self._dls             = LoKi.Particles.DecayLengthSignificanceDV () 
    #
    self._min_dll_K       = MINTREE ( 'K+'  == ABSID   , PIDK  - PIDpi ) 
    self._min_dll_Pi      = MINTREE ( 'pi+' == ABSID   , PIDpi - PIDK  ) 
    self._min_dll_PK      = MINTREE ( 'p+'  == ABSID   , PIDp  - PIDK  ) 
    self._min_dll_Ppi     = MINTREE ( 'p+'  == ABSID   , PIDp  - PIDpi ) 
    self._min_dll_Mu      = MINTREE ( 'mu+' == ABSID   , PIDmu - PIDpi )
    #
    self._min_annpid_K    = MINTREE ( 'K+'  == ABSID     , PROBNNk      ) 
    self._min_annpid_Pi   = MINTREE ( 'pi+' == ABSID     , PROBNNpi     ) 
    self._min_annpid_Mu   = MINTREE ( 'mu+' == ABSID     , PROBNNmu     ) 
    self._min_annpid_E    = MINTREE ( 'e+'  == ABSID     , PROBNNe      )
    self._min_annpid_P    = MINTREE ( 'p+'  == ABSID     , PROBNNp      )
    #
    self._min_Pt          = MINTREE ( ISBASIC & HASTRACK , PT  ) / GeV 
    self._min_Eta         = MINTREE ( ISBASIC & HASTRACK , ETA ) 
    self._max_Eta         = MAXTREE ( ISBASIC & HASTRACK , ETA )
    #
    self._min_CL_gamma    = MINTREE ( 'gamma' == ID , CL ) 
    self._min_Et_gamma    = MINTREE ( 'gamma' == ID , PT ) / GeV 
    self._min_E_gamma     = MINTREE ( 'gamma' == ID ,  E ) / GeV 
    #
    self._maxTrChi2       = MAXTREE ( ISBASIC & HASTRACK , TRCHI2DOF    )
    self._maxTrGhost      = MAXTREE ( ISBASIC & HASTRACK , TRGHOSTPROB  )
    self._minTrKL         = MINTREE ( ISBASIC & HASTRACK , CLONEDIST    ) 
    self._minTrIPchi2     = MINTREE ( ISBASIC & HASTRACK , BPVIPCHI2()  )
    self._max_anngh_track = MAXTREE ( ISBASIC & HASTRACK , PROBNNghost  )
    #
    ##
    self._EtC             = PINFO   ( 55001 , -100 * GeV ) 
    self._PtC             = PINFO   ( 55002 , -100 * GeV )  
    self._maxEtC          = MAXTREE ( 'mu+' == ABSID , self._EtC ) 
    self._maxPtC          = MAXTREE ( 'mu+' == ABSID , self._PtC )
    ## 
    self._EcalE           = PPINFO  ( LHCb.ProtoParticle.CaloEcalE , -100 * GeV ) 
    self._HcalE           = PPINFO  ( LHCb.ProtoParticle.CaloHcalE , -100 * GeV )  
    self._maxEcalE        = MAXTREE ( 'mu+' == ABSID , self._EcalE )
    self._maxHcalE        = MAXTREE ( 'mu+' == ABSID , self._HcalE )
    #
    self._delta_m2        = LoKi.PhysKinematics.deltaM2
    self._masses          = {} 
    #
    self.fill_initialized = True 
    return SUCCESS 

# ==============================================================================
## finalize 
def fill_finalize   ( self ) :
    """
    Finalie the internal machinery 
    """
    del self._pions           # = None 
    del self._kaons           # = None 
    del self._protons         # = None 
    del self._muons           # = None 
    del self._gamma           # = None 
    del self._digamma         # = None 
    del self._pi0             # = None 
    del self._tracks          # = None 
    del self._basic           # = None 
    #
    del self._ctau            # = None 
    del self._ctau_9          # = None 
    del self._ctau_25         # = None 
    del self._lv01            # = None 
    del self._vchi2           # = None 
    del self._vchi2ndf        # = None 
    del self._dtfchi2         # = None 
    del self._ipchi2          # = None 
    del self._dls             # = None 
    #
    del self._min_dll_K       # = None 
    del self._min_dll_Pi      # = None 
    del self._min_dll_PK      # = None 
    del self._min_dll_Ppi     # = None 
    del self._min_dll_Mu      # = None
    #
    del self._min_annpid_K    # = None 
    del self._min_annpid_Pi   # = None 
    del self._min_annpid_Mu   # = None 
    del self._min_annpid_E    # = None 
    del self._min_annpid_P    # = None 

    del self._min_Pt          # = None 
    del self._min_Eta         # = None 
    del self._max_Eta         # = None
    #
    del self._min_E_gamma     # = None 
    del self._min_Et_gamma    # = None 
    del self._min_CL_gamma    # = None 
    #
    del self._maxTrChi2       # = None 
    del self._maxTrGhost      # = None 
    del self._minTrKL         # = None 
    del self._minTrIPchi2     # = None 
    del self._max_anngh_track # = None 
    #
    del self._EtC             # = None 
    del self._PtC             # = None 
    del self._maxEtC          # = None 
    del self._maxPtC          # = None 
    ## 
    del self._EcalE           # = None
    del self._HcalE           # = None 
    del self._maxEcalE        # = None 
    del self._maxHcalE        # = None 
    #
    del self._delta_m2        # = None
    del self._masses          # = None 
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
    #
    ## initialize it
    #
    if not self.fill_initialized : fill_initialize ( self )
    ##    
    if hasattr ( p , 'particle' ) : p = p.particle() 
    #
    ## get all pions from decay tree
    #
    good  = LHCb.Particle.ConstVector()
    p.children ( self._pions , good ) 
    #
    _cnt  = self.counter ( '#pion' + suffix )
    _cnt += good.size()
    #
    sc = tup.column_float ( 'mindll_piK' + suffix , self._min_dll_Pi    ( p ) ) 
    sc = tup.column_float ( 'minann_pi'  + suffix , self._min_annpid_Pi ( p ) ) 
    #
    tup.fArrayP (
        'p_pion'     + suffix , P   / GeV    ,  
        'pt_pion'    + suffix , PT  / GeV    , 
        'eta_pion'   + suffix , ETA          , 
        LHCb.Particle.Range ( good )         ,
        'n_pion'     + suffix , 10           )
    return tup.fArrayP (
        'pid_pion'   + suffix , PIDpi - PIDK ,  
        'ann_pion'   + suffix , PROBNNpi     ,  
        'ann_pion_P' + suffix , PROBNNK      ,  
        'ann_pion_K' + suffix , PROBNNp      ,  
        LHCb.Particle.Range ( good )         ,
        'n_pion'     + suffix , 10           )

# ==============================================================================
## add kaon information into n-tuple
#  @param tup   n-tuple
#  @param p     the particle 
def treatKaons ( self         ,
                 tup          ,
                 p            ,
                 suffix  = '' ) :

    #
    ## initialize it
    #
    if not self.fill_initialized : fill_initialize ( self )
    ## 
    if hasattr ( p , 'particle' ) : p = p.particle() 
    #
    ## get all kaons form decay tree
    #
    good  = LHCb.Particle.ConstVector()
    p.children ( self._kaons , good ) 
    #
    _cnt  = self.counter ( '#kaon' + suffix )
    _cnt += good.size()
    #
    sc = tup.column_float ( 'mindll_K' + suffix , self._min_dll_K     ( p ) ) 
    sc = tup.column_float ( 'minann_K' + suffix , self._min_annpid_K  ( p ) ) 
    #
    tup.fArrayP (
        'p_kaon'      + suffix , P   / GeV    , 
        'pt_kaon'     + suffix , PT  / GeV    , 
        'eta_kaon'    + suffix , ETA          ,  
        LHCb.Particle.Range ( good )          ,
        'n_kaon'      + suffix , 10           )
    return tup.fArrayP (
        'pid_kaon'    + suffix , PIDK - PIDpi ,  
        'ann_kaon'    + suffix , PROBNNk      ,
        'ann_kaon_PI' + suffix , PROBNNpi     ,  
        'ann_kaon_P'  + suffix , PROBNNp      ,  
        LHCb.Particle.Range ( good )          ,
        'n_kaon'      + suffix , 10           )

# ==============================================================================
## add proton information into n-tuple
#  @param tup   n-tuple
#  @param p     the particle 
def treatProtons ( self         ,
                   tup          ,
                   p            ,
                   suffix  = '' ) :

    #
    ## initialize it
    #
    if not self.fill_initialized : fill_initialize ( self )
    ## 
    if hasattr ( p , 'particle' ) : p = p.particle() 
    #
    ## get all protons form decay tree
    #
    good   = LHCb.Particle.ConstVector()
    p.children ( self._protons , good ) 
    #
    _cnt  = self.counter ( '#proton' + suffix )
    _cnt += good.size()
    #
    sc = tup.column_float ( 'mindll_pK'  + suffix , self._min_dll_PK   ( p ) ) 
    sc = tup.column_float ( 'mindll_ppi' + suffix , self._min_dll_Ppi  ( p ) ) 
    sc = tup.column_float ( 'minann_P'   + suffix , self._min_annpid_P ( p ) ) 
    #
    tup.fArrayP (
        'p_proton'      + suffix , P   / GeV    , 
        'pt_proton'     + suffix , PT  / GeV    , 
        'eta_proton'    + suffix , ETA          , 
        'ann_proton'    + suffix , PROBNNp      ,
        LHCb.Particle.Range ( good )            ,
        'n_proton'      + suffix , 10           )
    return tup.fArrayP (
        'pid_proton_pi' + suffix , PIDp - PIDpi ,
        'pid_proton_K'  + suffix , PIDp - PIDK  ,
        'ann_proton_PI' + suffix , PROBNNpi     ,
        'ann_proton_K'  + suffix , PROBNNk      ,
        LHCb.Particle.Range ( good )            ,
        'n_proton'      + suffix , 10           )

# ==============================================================================
## add photon information into n-tuple
#  @param tup   n-tuple
#  @param p     the particle 
def treatPhotons ( self         ,
                   tup          ,
                   p            ,
                   suffix  = '' ) :

    #
    ## initialize it
    #
    if not self.fill_initialized : fill_initialize ( self )
    ## 
    if hasattr ( p , 'particle' ) : p = p.particle() 
    #
    ## get all protons form decay tree
    #
    good   = LHCb.Particle.ConstVector()
    p.children ( self._gamma , good ) 
    #
    _cnt  = self.counter ( '#gamma' + suffix )
    _cnt += good.size()
    #
    sc = tup.column_float ( 'minEt_gamma'  + suffix , self._min_Et_gamma ( p ) ) 
    sc = tup.column_float ( 'minE_gamma'   + suffix , self._min_E_gamma  ( p ) ) 
    sc = tup.column_float ( 'minCl_gamma'  + suffix , self._min_CL_gamma ( p ) ) 
    #
    tup.fArrayP (
        'e_photon'    + suffix       , P   / GeV , 
        'et_photon'   + suffix       , PT  / GeV , 
        'CL_photon'   + suffix       , CL        , 
        LHCb.Particle.Range ( good ) ,
        'n_photon'    + suffix       , 10        )
    return tup.fArrayP (
        'eta_photon'  + suffix       , ETA       , 
        'tx_photon'   + suffix       , PX / PZ   , 
        'ty_photon'   + suffix       , PY / PZ   , 
        LHCb.Particle.Range ( good ) ,
        'n_photon'    + suffix       , 10        )
    
# ==============================================================================
## add di-gamma information into n-tuple
#  @param tup   n-tuple
#  @param p     the particle 
def treatDiGamma ( self         ,
                   tup          ,
                   p            ,
                   suffix  = '' ) :
    
    #
    ## initialize it
    #
    if not self.fill_initialized : fill_initialize ( self )
    ##
    if hasattr ( p , 'particle' ) : p = p.particle() 
    #
    ## get all protons form decay tree
    #
    good   = LHCb.Particle.ConstVector()
    p.children ( self._digamma , good ) 
    #
    _cnt  = self.counter ( '#digamma' + suffix )
    _cnt += good.size()
    #
    tup.fArrayP (
        'e_digamma'    + suffix       , P   / GeV , 
        'et_digamma'   + suffix       , PT  / GeV , 
        'ID_digamma'   + suffix       , ID        ,
        LHCb.Particle.Range ( good ) ,
        'n_digamma'    + suffix       , 10        )
    return tup.fArrayP (
        'm_digamma'    + suffix       , M   / GeV , 
        'lv01_digamma' + suffix       , LV01      ,
        LHCb.Particle.Range ( good ) ,
        'n_digamma'    + suffix       , 10        )

# ==============================================================================
## add muon information into n-tuple
#  @param tup   n-tuple
#  @param p     the particle 
def treatMuons ( self         ,
                 tup          ,
                 p            ,
                 suffix  = '' ) :
    
    #
    ## initialize it
    #
    if not self.fill_initialized : fill_initialize ( self )
    ## 
    if hasattr ( p , 'particle' ) : p = p.particle() 
    #
    ## get all muons from decay tree
    #
    good    = LHCb.Particle.ConstVector()
    p.children ( self._muons , good ) 
    #
    _cnt  = self.counter ( '#muon' + suffix )
    _cnt += good.size()
    #
    sc = tup.column_float ( 'mindll_mu'   + suffix , self._min_dll_Mu    ( p ) ) 
    sc = tup.column_float ( 'minann_mu'   + suffix , self._min_annpid_Mu ( p ) ) 
    #
    sc = tup.fArrayP   (
        'p_mu'       + suffix , P   / GeV     ,  
        'pt_mu'      + suffix , PT  / GeV     , 
        'eta_mu'     + suffix , ETA           , 
        LHCb.Particle.Range ( good )          ,
        'n_muon'     + suffix ,  10           )
    return tup.fArrayP (
        'eEcal_mu'   + suffix , self._EcalE   ,
        'eHcal_mu'   + suffix , self._HcalE   ,
        'pid_mu'     + suffix , PIDmu - PIDpi ,  
        'ann_mu'     + suffix , PROBNNmu      ,  
        LHCb.Particle.Range ( good )          ,
        'n_muon'     + suffix , 10            )

# ==============================================================================
## add tracks information into n-tuple
#  @param tup   n-tuple
#  @param p     the particle 
def treatTracks ( self         ,
                  tup          ,
                  p            ,
                  suffix  = '' ) :
    
    #
    ## initialize it
    #
    if not self.fill_initialized : fill_initialize ( self )
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
    _cnt  = self.counter ( '#track' + suffix )
    _cnt += good.size()
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
            
    sc = tup.column_float ( 'm2min_track'     + suffix , m2min                       )    
    sc = tup.column_float ( 'minPt_track'     + suffix , self._min_Pt          ( p ) )
    sc = tup.column_float ( 'minEta_track'    + suffix , self._min_Eta         ( p ) )
    sc = tup.column_float ( 'maxEta_track'    + suffix , self._max_Eta         ( p ) )
    sc = tup.column_float ( 'maxChi2_track'   + suffix , self._maxTrChi2       ( p ) )
    sc = tup.column_float ( 'maxTrGh_track'   + suffix , self._maxTrGhost      ( p ) )
    sc = tup.column_float ( 'minKL_track'     + suffix , self._minTrKL         ( p ) )
    sc = tup.column_float ( 'maxAnnGh_track'  + suffix , self._max_anngh_track ( p ) )
    
    tup.fArrayP (
        'p_track'     + suffix , P   / GeV   , 
        'pt_track'    + suffix , PT  / GeV   , 
        'eta_track'   + suffix , ETA         , 
        'phi_track'   + suffix , PHI         ,                        
        LHCb.Particle.Range ( good )         , 
        'n_track'     + suffix , 20          )
    
    return tup.fArrayP (
        'chi2_track'  + suffix , TRCHI2DOF   ,
        'PChi2_track' + suffix , TRPCHI2     , 
        'ann_track'   + suffix , PROBNNghost ,                        
        'trgh_track'  + suffix , TRGHOSTPROB ,                        
        LHCb.Particle.Range ( good )         , 
        'n_track'     + suffix , 20          )

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
    #
    ## initialize it
    #
    if not self.fill_initialized : fill_initialize ( self )
    ##
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
#
#  @code
#
#  tup = ...
#    
#  b   = ...
#
#  ## var-names will be m12, m23, ...
#  self.fillMasses ( tup , b )  ## no constraints are applies 
#    
#  ## var-names will be m12c, m23c, ...
#  ## last two parameters are propagated to Decay Tree Fitter
#  self.fillMasses ( tup , b , 'c' , True , 'J/psi(1S)')  ## with constraints
#
#  @endcode
#
def fillMasses ( self        ,
                 tup         ,
                 b           ,
                 suffix      ,
                 *args       ) :
    """
    Fill information about mass of all sub-combinations 
    
    tup = ...
    
    b   = ...
    
    ## var-names will be m12, m23, ...
    self.fillMasses ( tup , b )  ## no constraints are applies 
    
    ## var-names will be m12c, m23c, ...
    ## last parameters are propagated to Decay Tree Fitter
    self.fillMasses ( tup , b , 'c' , True , 'J/psi(1S)')  ## with constraints
    
    """
    #
    ## initialize it
    #
    if not self.fill_initialized : fill_initialize ( self )
    ##
    if not self._masses.has_key (suffix) : self._masses[suffix] = {}
        
    nc = b.nChildren()
        
    _ms = self._masses[suffix]
    
    nc = b.nChildren()

    if not _ms :

        ##
        for i in range ( 1 , nc + 1 )  :
            
            bi = b(i)
            if bi and not bi.isBasicParticle() : 
                k1 = "m%d" % i 
                m1 = MASS  ( i )
                if args : m1 = DTF_FUN ( m1 , *args )
                _ms [ k1 + suffix ]  =   m1

            ##
            for j in range ( i + 1 , nc + 1 ) :
                
                k2 = "m%d%d" % ( i , j )
                m2 =   MASS    ( i , j )
                if args : m2 = DTF_FUN ( m2 , *args )
                _ms [ k2 + suffix ]  =   m2

                ##
                for k in range ( j + 1 , nc + 1 ) :
                    
                    k3 = "m%d%d%d" % ( i , j , k )
                    m3 =     MASS    ( i , j , k )
                    if args : m3 = DTF_FUN ( m3 , *args )
                    _ms [ k3 + suffix ]  =   m3

                    ## 
                    for l in range ( k + 1 , nc + 1 ) :
                        
                        k4 = "m%d%d%d%d" % ( i , j , k , l )
                        m4 =       MASS    ( i , j , k , l )
                        if args : m4 = DTF_FUN ( m4 , *args )
                        _ms [ k4 + suffix ]  =   m4
                        
                        ## 
                        for m in range ( l + 1 , nc + 1 ) :
                            
                            k5 = "m%d%d%d%d%d" % ( i , j , k , l , m )
                            
                            _ind5 = LoKi.Particles.InvariantMass.Indices ()
                            
                            _ind5 . push_back ( i )
                            _ind5 . push_back ( j )
                            _ind5 . push_back ( k )
                            _ind5 . push_back ( l )
                            _ind5 . push_back ( m )
                            
                            m5 = MASS     ( _ind5 )
                            if args : m5 = DTF_FUN ( m5 , *args )
                            _ms [ k5 + suffix ]  =   m5
                            
                            for n in range ( m + 1 , nc + 1 ) :
                                
                                k6 = "m%d%d%d%d%d%d" % ( i , j , k , l , m , n )
                                
                                _ind6 = LoKi.Particles.InvariantMass.Indices ()
                                
                                _ind6 . push_back ( i )
                                _ind6 . push_back ( j )
                                _ind6 . push_back ( k )
                                _ind6 . push_back ( l )
                                _ind6 . push_back ( m )
                                _ind6 . push_back ( n )
                                
                                m6 = MASS     ( _ind6 )
                                if args : m6 = DTF_FUN ( m6 , *args )
                                _ms [ k6 + suffix ]  =   m6
                                
                                for o in range ( n + 1 , nc + 1 ) :
                                    
                                    k7 = "m%d%d%d%d%d%d%d" % ( i , j , k , l , m , n , o )
                                
                                    _ind7 = LoKi.Particles.InvariantMass.Indices ()
                                    
                                    _ind7 . push_back ( i )
                                    _ind7 . push_back ( j )
                                    _ind7 . push_back ( k )
                                    _ind7 . push_back ( l )
                                    _ind7 . push_back ( m )
                                    _ind7 . push_back ( n )
                                    _ind7 . push_back ( o )
                                    
                                    m7 = MASS     ( _ind7 )
                                    if args : m7 = DTF_FUN ( m7 , *args )
                                    _ms [ k7 + suffix ]  =   m7

                                
    ## finally fill n-tuple 
    for k in _ms :
        tup.column_float ( k , _ms [ k ] ( b ) / GeV )

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
    nTracks = int ( data ( LHCb.RecSummary.nTracks            ) ) 
    #
    hRich1  = int ( data ( LHCb.RecSummary.nRich1Hits         ) ) 
    hRich2  = int ( data ( LHCb.RecSummary.nRich2Hits         ) )
    hVelo   = int ( data ( LHCb.RecSummary.nVeloClusters      ) ) 
    hIT     = int ( data ( LHCb.RecSummary.nITClusters        ) ) 
    hTT     = int ( data ( LHCb.RecSummary.nTTClusters        ) ) 
    hOT     = int ( data ( LHCb.RecSummary.nOTClusters        ) ) 
    hSPD    = int ( data ( LHCb.RecSummary.nSPDhits           ) ) 
    
    tup.column ( 'nPV'       , nPV      , 0 ,    30 )
    tup.column ( 'nLong'     , nLong    , 0 ,  1000 ) 
    tup.column ( 'nDown'     , nDown    , 0 ,   500 ) 
    tup.column ( 'nUp'       , nUp      , 0 ,   500 ) 
    tup.column ( 'nVelo'     , nVelo    , 0 ,  1000 ) 
    tup.column ( 'nTT'       , nTT      , 0 ,   500 ) 
    tup.column ( 'nBack'     , nBack    , 0 ,   200 ) 
    tup.column ( 'nTracks'   , nTracks  , 0 ,  2000 ) 
    
    tup.column ( 'hSPD'      , hSPD     , 0 ,  2000 ) 
    tup.column ( 'hRich1'    , hRich1   , 0 , 30000 ) 
    tup.column ( 'hRich2'    , hRich2   , 0 , 30000 ) 
    tup.column ( 'hVelo'     , hVelo    , 0 , 60000 ) 
    tup.column ( 'hIT'       , hIT      , 0 , 30000 ) 
    tup.column ( 'hTT'       , hTT      , 0 , 30000 ) 
    tup.column ( 'hOT'       , hOT      , 0 , 30000 ) 
    
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
LoKi.Algo.treatPions       =  treatPions
LoKi.Algo.treatKaons       =  treatKaons
LoKi.Algo.treatProtons     =  treatProtons
LoKi.Algo.treatPhotons     =  treatPhotons
LoKi.Algo.treatMuons       =  treatMuons
LoKi.Algo.treatTracks      =  treatTracks
LoKi.Algo.treatKine        =  treatKine
LoKi.Algo.treatDiGamma     =  treatDiGamma

LoKi.Algo.fillMasses       =  fillMasses

LoKi.Algo.addRecSummary    =  addRecSummary 
LoKi.Algo.addGecInfo       =  addGecInfo 

LoKi.Algo.fill_initialize  = fill_initialize
LoKi.Algo.fill_finalize    = fill_finalize

## class attribute 
LoKi.Algo.fill_initialized = False ## class attribute 

# =============================================================================
## insert "fill" finalization into the global finalization
def decorateFill ( ALGO ) :
    "Insert ``fill'' finalization into the global finalization"
    if hasattr ( ALGO , '_prev_fill_finalize_' ) : return      ## SKIP IT!
    # =========================================================================
    ## updated finalization 
    def _algo_fill_finalize_ ( algorithm ) :
        """Finalize ``specific'' stuff related BenderTools.Fill module
        and then continue with the base-class finalization
        """
        if algorithm.fill_initialized :
            algorithm.Print( 'Finalize "Fill"-machinery' , 10 , 2 )
            algorithm.fill_finalize  ()
        return algorithm._prev_fill_finalize_()
    # =====================================================================
    ALGO._prev_fill_finalize_ = ALGO.finalize 
    ALGO.finalize             = _algo_fill_finalize_ 

# =============================================================================
if '__main__' == __name__ :
    
    logger.info ( 80*'*') 
    logger.info (  __doc__ ) 
    logger.info ( ' Author  : %s' %        __author__    )
    logger.info ( ' Version : %s' %        __version__   ) 
    logger.info ( ' Date    : %s' %        __date__      ) 
    logger.info ( ' Symbols : %s' % list ( __all__     ) ) 
    logger.info ( 80*'*') 

# ==============================================================================
# The END 
# ==============================================================================
