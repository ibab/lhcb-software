#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file BenderTools/RunWGMC.py
#
# Helper moduel to simplify run of WG-selections on MC samples 
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
"""Helper module to simplify run of WG-selections on MC samples 

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
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-07-14"
__all__     = (
    'wg_Y_selection'     , ## use No_PID selection of Y->mumu
    'defineInputHadrons' , ## redefine input selections for hadrons
    'defineInputDiMuons' , ## redefine selection of dimuons  
    'makeMergedDiMuons'  , ## construct merged (J/psi+psi(2S) dimuons) 
    )
# =============================================================================
# logging 
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.RunWGMC' )
else                      : logger = getLogger ( __name__ )
# ==============================================================================
## get WG-selection for Y ->mu+mu- the given year
#  Essentially re-run stripping & WG-selection without muon-ID cuts 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
def wg_Upsilon_selection ( year )  :
    """    
    get WG selection for Y->mu+mu (MC) 
    Essentially re-run stripping & WG-selection withotu muon-ID cuts 
    """
    if   '2012' == year : 
        import StrippingArchive.Stripping20.StrippingDiMuonNew              as DiMuon 
        import StrippingSettings.Stripping20.LineConfigDictionaries_BandQ   as LineSettings
        logger.info ( 'Use Stirpping20   settings')
    elif '2011' == year :
        import StrippingArchive.Stripping20r1.StrippingDiMuonNew            as DiMuon 
        import StrippingSettings.Stripping20r1.LineConfigDictionaries_BandQ as LineSettings
        logger.info ( 'Use Stirpping20r1 settings')
    else :
        raise AttributeError,"Invalid year %s " % year 
        
    #
    ## stripping line builder
    config = LineSettings.FullDSTDiMuon['CONFIG']
    
    ## switch off PID-cut ! 
    config['DiMuonHighMass_MuonPIDCut'] = ''
    logger.info ( 'Switch PID-cuts for DiMuonHighMass line')
    
    ## use builder 
    builder = DiMuon.DiMuonConf ( 'FullDST' , config )
    
    ## Y-selection at stripping DST
    strip_ups = builder.SelDiMuonHighMass 
    logger.info  ( "ALGO:\n" + str( strip_ups.algorithm() ) ) 
    
    # =============================================================================
    ## Upsilon -> mumu, cuts by  Giulia Manca 
    # ============================================================================= 
    from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
    from PhysSelPython.Wrappers                import SimpleSelection
    sel_ups = SimpleSelection (
        'Upsilon'     ,
        FilterDesktop , 
        [ strip_ups ] , 
        ## algorithm parameters 
        Code     = """
        ( M > 7 * GeV ) & 
        DECTREE   ('Meson -> mu+ mu-'  )                      &
        CHILDCUT( 1 , HASMUON & ISMUON )                      &
        CHILDCUT( 2 , HASMUON & ISMUON )                      & 
        ( MINTREE ( 'mu+' == ABSID , PT ) > 1 * GeV         ) &
        ( MAXTREE ( ISBASIC & HASTRACK , TRCHI2DOF ) < 4    ) & 
        ( MINTREE ( ISBASIC & HASTRACK , CLONEDIST ) > 5000 ) & 
        ( VFASPF  ( VPCHI2 ) > 0.5/100 ) 
        & ( abs ( BPV ( VZ    ) ) <  0.5 * meter     ) 
        & (       BPV ( vrho2 )   < ( 10 * mm ) ** 2 ) 
        """ ,
        Preambulo = [ "vrho2 = VX**2 + VY**2" ] ,
        ReFitPVs  =   True
        )

    return sel_ups


from StandardParticles import StdNoPIDsPions   as NoPIDPions  
from StandardParticles import StdNoPIDsKaons   as NoPIDKaons  
from StandardParticles import StdNoPIDsProtons as NoPIDProtons
# =============================================================================
## decorate line builder to be used with MC (no PID-cuts)
#  @attention: The method needs to be used BEFORE instantiation of line-builder 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
def defineInputHadrons ( builder                       ,
                         input_pions    = NoPIDPions   , 
                         input_kaons    = NoPIDKaons   , 
                         input_protons  = NoPIDProtons ) :
    """
    Decorate line builder to be used with MC (no PID-cuts)
    ATTENTION: The method needs to be used BEFORE instantiation of line-builder
    """
    if input_pions  and hasattr ( builder , 'pions' ) :
        def _pions_    ( self ) :
            """Pions"""
            from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
            return self.make_selection (
                'Pion'                 ,
                FilterDesktop          ,
                [ input_pions ]        ,
                Code = self['PionCut'] ,
                )
        builder.pions = _pions_
        logger.info ('Redefine input pions')

    if input_kaons and hasattr ( builder , 'kaons' ) : 
        def _kaons_    ( self ) :
            """Kaons"""
            from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
            return self.make_selection (
                'Kaon'                 ,
                FilterDesktop          ,
                [ input_kaons ]        ,
                Code = self['KaonCut'] ,
                )
        builder.kaons = _kaons_
        logger.info ('Redefine input kaons')
        
    if input_protons and hasattr ( builder , 'protons' ) : 
        def _protons_    ( self ) :
            """Protons"""
            from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
            return self.make_selection (
                'Proton'                 ,
                FilterDesktop            ,
                [ input_protons ]        ,
                Code = self['ProtonCut'] ,
                )
        builder.protons = _protons_
        logger.info ( 'Redefine input protons' )


# =============================================================================
## make input dimuons (sum of J/psi and psi(2S) 
def makeMergedDiMuons ( name_jpsi  = 'FullDSTDiMuonJpsi2MuMuDetachedLine' , 
                        name_psi2s = 'FullDSTDiMuonPsi2MuMuDetachedLine'  ) :
    
    from PhysSelPython.Wrappers import AutomaticData
    sel_jpsi  = AutomaticData ( '/Event/AllStreams/Phys/%s/Particles' % name_jpsi  )
    sel_psi2s = AutomaticData ( '/Event/AllStreams/Phys/%s/Particles' % name_psi2s )
    #
    ## merged selectoon for J/psi & psi'
    #
    from PhysSelPython.Wrappers import MergedSelection
    return  MergedSelection (
        'SelPsisForBandQ' ,
        RequiredSelections = [ sel_jpsi , sel_psi2s ]
        )

# =============================================================================
## decorate line builder to be used with MC (no PID-cuts)
#  @attention: The method needs to be used BEFORE instantiation of line-builder 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
def defineInputDiMuons ( builder , dimuon_sel = None ) :
    """
    Decorate line builder to be used with MC (no PID-cuts)
    ATTENTION: The method needs to be used BEFORE instantiation of line-builder 
    """
    ## 
    if not dimuon_sel :
        dimuon_sel = makeMergedDiMuons()
        
    if hasattr ( builder , 'psi' ) : 
        builder.psis =  dimuon_sel 
        logger.info ('Redefine input psis')
        

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
