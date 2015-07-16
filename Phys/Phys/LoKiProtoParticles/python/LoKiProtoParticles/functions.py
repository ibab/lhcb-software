#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file LoKiProtoParticles/functions.py
#  The set of basic decorator for objects from Phys/LoKiProtoPaticles library
#  The file is a part of LoKi and Bender projects
#
#        This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#    ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#    
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-05-26
#
#                  $Revision$
# LastModification $Date$
#               by $Author$ 
# =============================================================================
"""
The set of basic decorators for objects from Phys/LoKiProtoParticles library

      This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

                 $Revision$
LastModification $Date$
              by $Author$ 
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.BElyaev@nikhef.nl"
__date__    = "2010-05-26"
__version__ = "Version $Revision$ "
# =============================================================================

import LoKiCore.decorators    as      _LoKiCore

# Namespaces:
cpp      = _LoKiCore.cpp
std      = _LoKiCore.std
LoKi     = _LoKiCore.LoKi
LHCb     = _LoKiCore.LHCb
Gaudi    = cpp.Gaudi 


_PP = 'const LHCb::ProtoParticle*'

# =============================================================================
## "The main" types
# =============================================================================

## @see LoKi::Types::PPFunc
PPFunc  = LoKi.Functor               ( _PP , 'double' ) 
## @see LoKi::Types::PPCuts
PPCuts  = LoKi.Functor               ( _PP ,  bool    ) 
## @see LoKi::Types::PPFun
PPFun   = LoKi.FunctorFromFunctor    ( _PP , 'double' ) 
## @see LoKi::Types::PPCut
PPCut   = LoKi.FunctorFromFunctor    ( _PP ,  bool    ) 

## @see LoKi::Cuts::PP_TRUE
PP_TRUE     = LoKi.Constant( _PP , bool )(True)
## @see LoKi::Cuts::PP_FALSE
PP_FALSE    = LoKi.Constant( _PP , bool )(False)
## @see LoKi::Cuts::PP_ALL
PP_ALL      = PP_TRUE
## @see LoKi::Cuts::PP_NONE
PP_NONE     = PP_FALSE
## @see LoKi::Cuts::PP_ONE
PP_ONE      = LoKi.Constant     ( _PP , 'double' ) (1.0)
## @see LoKi::Cuts::PP_ZERO
PP_ZERO     = LoKi.Constant     ( _PP , 'double' ) (0.0)

## @see LoKi::Cuts::PP_FILTER 
PP_FILTER = LoKi.ProtoParticles.Filter

## @see LoKi::Cuts::PP_VALID
PP_VALID    = LoKi.Valid(_PP)()
## @see LoKi::Cuts::PP_HASINFO
PP_HASINFO  = LoKi.ProtoParticles.HasInfo
## @see LoKi::Cuts::PP_INFO
PP_INFO     = LoKi.ProtoParticles.Info


## @see LoKi::Cuts::PP_HASRICHPID 
PP_HASRICHPID  = LoKi.ProtoParticles.HasRichPID  ()
## @see LoKi::Cuts::PP_HASMUONPID 
PP_HASMUONPID  = LoKi.ProtoParticles.HasMuonPID  ()
## @see LoKi::Cuts::PP_HASTRACK
PP_HASTRACK    = LoKi.ProtoParticles.HasTrack    () 
#a# @see LoKi::Cuts::PP_NCALOS 
PP_NCALOS      = LoKi.ProtoParticles.Calos       () 

## @see LoKi::Cuts::PP_CHARGED 
PP_CHARGED     = LoKi.ProtoParticles.Charged ()
## @see LoKi::Cuts::PP_NEUTRAL
PP_NEUTRAL     = LoKi.ProtoParticles.Neutral ()

## @see LoKi::Cuts::PP_HASAEROGEL
PP_HASAEROGEL  = LoKi.ProtoParticles.HasAerogel  () 
## @see LoKi::Cuts::PP_HASRICH1GAS
PP_HASRICH1GAS = LoKi.ProtoParticles.HasRich1Gas () 
## @see LoKi::Cuts::PP_HASRICH2GAS
PP_HASRICH2GAS = LoKi.ProtoParticles.HasRich2Gas () 


## @see LoKi::Cuts::PP_HASCALOSPD 
PP_HASCALOSPD   = PP_HASINFO ( LHCb.ProtoParticle.InAccSpd      )
## @see LoKi::Cuts::PP_HASCALOPRS
PP_HASCALOPRS   = PP_HASINFO ( LHCb.ProtoParticle.InAccPrs      )
## @see LoKi::Cuts::PP_HASCALOECAL
PP_HASCALOECAL  = PP_HASINFO ( LHCb.ProtoParticle.InAccEcal     )
## @see LoKi::Cuts::PP_HASCALOHCAL
PP_HASCALOHCAL  = PP_HASINFO ( LHCb.ProtoParticle.InAccHcal     )
## @see LoKi::Cuts::PP_HASCALOBREM
PP_HASCALOBREM  = PP_HASINFO ( LHCb.ProtoParticle.InAccBrem     )
## @see LoKi::Cuts::PP_HASMUONINFO
PP_HASMUONINFO  = PP_HASINFO ( LHCb.ProtoParticle.InAccMuon     )
## @see LoKi::Cuts::PP_HASMUONDLL
PP_HASMUONDLL   = PP_HASINFO ( LHCb.ProtoParticle.MuonPIDStatus )
## @see LoKi::Cuts::PP_HASRICHDLL
PP_HASRICHDLL   = PP_HASINFO ( LHCb.ProtoParticle.RichPIDStatus )

## @see LoKi::Cuts::PP_HASDETECTOR
PP_HASDETECTOR  = LoKi.ProtoParticles.HasDetector

## @see LoKi::Cuts::PP_HASRICHDLL 
PP_HASRICHDLL   = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.RICH ) 
## @see LoKi::Cuts::PP_HASCALODLL 
PP_HASCALODLL   = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO ) 
## @see LoKi::Cuts::PP_HASMUONINFO 
PP_HASMUONINFO  = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.MUON ) 

## @see LoKi::Cuts::PP_USEDAEROGEL 
PP_USEDAEROGEL  = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.RICH_AEROGEL  ) 
## @see LoKi::Cuts::PP_USEDRICH1GAS
PP_USEDRICH1GAS = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.RICH_RICH1GAS ) 
## @see LoKi::Cuts::PP_USEDRICH2GAS
PP_USEDRICH2GAS = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.RICH_RICH2GAS ) 

## @see LoKi::Cuts::PP_INSPDACC 
PP_INSPDACC    = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_SPDACC  ) 
## @see LoKi::Cuts::PP_INPRSACC 
PP_INPRSACC    = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_PRSACC  ) 
## @see LoKi::Cuts::PP_INECALACC 
PP_INECALACC   = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_ECALACC ) 
## @see LoKi::Cuts::PP_INHCALACC 
PP_INHCALACC   = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_HCALACC ) 
## @see LoKi::Cuts::PP_INBREMACC 
PP_INBREMACC   = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_BREMACC ) 
## @see LoKi::Cuts::PP_INCALOACC 
PP_INCALOACC   = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_CALOACC ) 

## @see LoKi::Cuts::PP_HASSPDINFO
PP_HASSPDINFO  = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_SPDINFO  ) 
## @see LoKi::Cuts::PP_HASPRSINFO
PP_HASPRSINFO  = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_PRSINFO  ) 
## @see LoKi::Cuts::PP_HASECALINFO
PP_HASECALINFO = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_ECALINFO ) 
## @see LoKi::Cuts::PP_HASHCALINFO
PP_HASHCALINFO = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_HCALINFO ) 
## @see LoKi::Cuts::PP_HASBREMINFO
PP_HASBREMINFO = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_BREMINFO ) 
## @see LoKi::Cuts::PP_HASCALOINFO
PP_HASCALOINFO = PP_HASDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_CALOINFO ) 

## @see LoKi::Cuts::PP_ONLYDETECTOR
PP_ONLYDETECTOR  = LoKi.ProtoParticles.OnlyDetector

## @see LoKi::Cuts::PP_ONLYRICHDLL 
PP_ONLYRICHDLL   = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.RICH ) 
## @see LoKi::Cuts::PP_ONLYCALODLL 
PP_ONLYCALODLL   = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO ) 
## @see LoKi::Cuts::PP_ONLYMUONINFO 
PP_ONLYMUONINFO  = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.MUON ) 

## @see LoKi::Cuts::PP_ONLYAEROGEL 
PP_ONLYAEROGEL  = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.RICH_AEROGEL  ) 
## @see LoKi::Cuts::PP_USEDRICH1GAS
PP_ONLYRICH1GAS = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.RICH_RICH1GAS ) 
## @see LoKi::Cuts::PP_USEDRICH2GAS
PP_ONLYRICH2GAS = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.RICH_RICH2GAS ) 

## @see LoKi::Cuts::PP_ONLYSPDACC 
PP_ONLYSPDACC    = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_SPDACC  ) 
## @see LoKi::Cuts::PP_ONLYPRSACC 
PP_ONLYPRSACC    = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_PRSACC  ) 
## @see LoKi::Cuts::PP_ONLYECALACC 
PP_ONLYECALACC   = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_ECALACC ) 
## @see LoKi::Cuts::PP_ONLYHCALACC 
PP_ONLYHCALACC   = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_HCALACC ) 
## @see LoKi::Cuts::PP_ONLYBREMACC 
PP_ONLYBREMACC   = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_BREMACC ) 
## @see LoKi::Cuts::PP_ONLYCALOACC 
PP_ONLYCALOACC   = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_CALOACC ) 

## @see LoKi::Cuts::PP_ONLYSPDINFO
PP_ONLYSPDINFO  = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_SPDINFO  ) 
## @see LoKi::Cuts::PP_ONLYPRSINFO
PP_ONLYPRSINFO  = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_PRSINFO  ) 
## @see LoKi::Cuts::PP_ONLYECALINFO
PP_ONLYECALINFO = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_ECALINFO ) 
## @see LoKi::Cuts::PP_ONLYHCALINFO
PP_ONLYHCALINFO = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_HCALINFO ) 
## @see LoKi::Cuts::PP_ONLYBREMINFO
PP_ONLYBREMINFO = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_BREMINFO ) 
## @see LoKi::Cuts::PP_ONLYCALOINFO
PP_ONLYCALOINFO = PP_ONLYDETECTOR ( LoKi.ProtoParticles.HasDetector.CALO_CALOINFO ) 


## @see LoKi::Cuts::PP_RichDLLe
PP_RichDLLe        = PP_INFO ( LHCb.ProtoParticle.RichDLLe        , -1000 )
## @see LoKi::Cuts::PP_RichDLLmu
PP_RichDLLmu       = PP_INFO ( LHCb.ProtoParticle.RichDLLmu       , -1000 )
## @see LoKi::Cuts::PP_RichDLLpi
PP_RichDLLpi       = PP_INFO ( LHCb.ProtoParticle.RichDLLpi       , -1000 )
## @see LoKi::Cuts::PP_RichDLLk
PP_RichDLLk        = PP_INFO ( LHCb.ProtoParticle.RichDLLk        , -1000 )
## @see LoKi::Cuts::PP_RichDLLp
PP_RichDLLp        = PP_INFO ( LHCb.ProtoParticle.RichDLLp        , -1000 )
## @see LoKi::Cuts::PP_RichDLLbt
PP_RichDLLbt       = PP_INFO ( LHCb.ProtoParticle.RichDLLbt       , -1000 )
## @see LoKi::Cuts::PP_RichPIDStatus
PP_RichPIDStatus   = PP_INFO ( LHCb.ProtoParticle.RichPIDStatus   , 0 , 0 ) 
  
## @see LoKi::Cuts::PP_MuonMuLL
PP_MuonMuLL        = PP_INFO ( LHCb.ProtoParticle.MuonMuLL        , -1000 )
## @see LoKi::Cuts::PP_MuonBkgLL
PP_MuonBkgLL       = PP_INFO ( LHCb.ProtoParticle.MuonBkgLL       , -1000 )
## @see LoKi::Cuts::PP_MuonNshared
PP_MuonNShared     = PP_INFO ( LHCb.ProtoParticle.MuonNShared     ,    -1 )
## @see LoKi::Cuts::PP_MuonPIDStatus
PP_MuonPIDStatus   = PP_INFO ( LHCb.ProtoParticle.MuonPIDStatus   , 0 , 0 ) 

## @see LoKi::Cuts::PP_InAccMuon
PP_InAccMuon       = PP_INFO ( LHCb.ProtoParticle.InAccMuon       , False , False )
## @see LoKi::Cuts::PP_InAccSpd
PP_InAccSpd        = PP_INFO ( LHCb.ProtoParticle.InAccSpd        , False , False )
## @see LoKi::Cuts::PP_InAccPrs
PP_InAccPrs        = PP_INFO ( LHCb.ProtoParticle.InAccPrs        , False , False )
## @see LoKi::Cuts::PP_InAccEcal
PP_InAccEcal       = PP_INFO ( LHCb.ProtoParticle.InAccEcal       , False , False )
## @see LoKi::Cuts::PP_InAccHcal
PP_InAccHcal       = PP_INFO ( LHCb.ProtoParticle.InAccHcal       , False , False )
## @see LoKi::Cuts::PP_InAccBrem
PP_InAccBrem       = PP_INFO ( LHCb.ProtoParticle.InAccBrem       , False , False )

## @see LoKi::Cuts::PP_CaloTrMatch
PP_CaloTrMatch     = PP_INFO ( LHCb.ProtoParticle.CaloTrMatch     , -1000   )
## @see LoKi::Cuts::PP_CaloElectronMatch
PP_CaloElectronMatch   = PP_INFO ( LHCb.ProtoParticle.CaloElectronMatch   , -1000 )
## @see LoKi::Cuts::PP_CaloBremMatch
PP_CaloBremMatch   = PP_INFO ( LHCb.ProtoParticle.CaloBremMatch   , -1000   )

## @see LoKi::Cuts::PP_CaloChargedSpd
PP_CaloChargedSpd  = PP_INFO ( LHCb.ProtoParticle.CaloChargedSpd  ,    -1   )
## @see LoKi::Cuts::PP_CaloChargedPrs
PP_CaloChargedPrs  = PP_INFO ( LHCb.ProtoParticle.CaloChargedPrs  , -1.e+10 )
## @see LoKi::Cuts::PP_CaloChargedEcal
PP_CaloChargedEcal = PP_INFO ( LHCb.ProtoParticle.CaloChargedEcal , -1.e+10 )

## @see LoKi::Cuts::PP_CaloDepositID
PP_CaloDepositID   = PP_INFO ( LHCb.ProtoParticle.CaloDepositID   , -1.e+10 )
## @see LoKi::Cuts::PP_CaloShowerShape
PP_ShowerShape     = PP_INFO ( LHCb.ProtoParticle.ShowerShape     , -1.e+10 )
## @see LoKi::Cuts::PP_CaloClusterMass
PP_ClusterMass     = PP_INFO ( LHCb.ProtoParticle.ClusterMass     , -1.e+10 )

## @see LoKi::Cuts::PP_CaloNeutralSpd
PP_CaloNeutralSpd  = PP_INFO ( LHCb.ProtoParticle.CaloNeutralSpd  ,    -1   )
## @see LoKi::Cuts::PP_CaloNeutralPrs
PP_CaloNeutralPrs  = PP_INFO ( LHCb.ProtoParticle.CaloNeutralPrs  , -1.e+10 )
## @see LoKi::Cuts::PP_CaloNeutralEcal
PP_CaloNeutralEcal = PP_INFO ( LHCb.ProtoParticle.CaloNeutralEcal , -1.e+10 )

## @see LoKi::Cuts::PP_CaloSpdE
PP_CaloSpdE        = PP_INFO ( LHCb.ProtoParticle.CaloSpdE        , -1.e+10 )
## @see LoKi::Cuts::PP_CaloPesE
PP_CaloPrsE        = PP_INFO ( LHCb.ProtoParticle.CaloPrsE        , -1.e+10 )
## @see LoKi::Cuts::PP_CaloEcalE
PP_CaloEcalE       = PP_INFO ( LHCb.ProtoParticle.CaloEcalE       , -1.e+10 )
## @see LoKi::Cuts::PP_CaloHcalE
PP_CaloHcalE       = PP_INFO ( LHCb.ProtoParticle.CaloHcalE       , -1.e+10 )

## @see LoKi::Cuts::PP_CaloEcalChi2
PP_CaloEcalChi2    = PP_INFO ( LHCb.ProtoParticle.CaloEcalChi2    , -1.e+10 )
## @see LoKi::Cuts::PP_CaloBremChi2
PP_CaloBremChi2    = PP_INFO ( LHCb.ProtoParticle.CaloBremChi2    , -1.e+10 )
## @see LoKi::Cuts::PP_CaloClusChi2
PP_CaloClusChi2    = PP_INFO ( LHCb.ProtoParticle.CaloClusChi2    , -1.e+10 )

## @see LoKi::Cuts::PP_CaloTrajectoryL
PP_CaloTrajectoryL = PP_INFO ( LHCb.ProtoParticle.CaloTrajectoryL , -1.e+10 )

## @see LoKi::Cuts::PP_EcalPIDe
PP_EcalPIDe        = PP_INFO ( LHCb.ProtoParticle.EcalPIDe        , -1.e+10 )
## @see LoKi::Cuts::PP_PrsPIDe
PP_PrsPIDe         = PP_INFO ( LHCb.ProtoParticle.PrsPIDe         , -1.e+10 )
## @see LoKi::Cuts::PP_BremPIDe
PP_BremPIDe        = PP_INFO ( LHCb.ProtoParticle.BremPIDe        , -1.e+10 )
## @see LoKi::Cuts::PP_HcalPIDe
PP_HcalPIDe        = PP_INFO ( LHCb.ProtoParticle.HcalPIDe        , -1.e+10 )

## @see LoKi::Cuts::PP_HcalPIDmu
PP_HcalPIDmu       = PP_INFO ( LHCb.ProtoParticle.HcalPIDmu       , -1.e+10 )
## @see LoKi::Cuts::PP_EcalPIDmu
PP_EcalPIDmu       = PP_INFO ( LHCb.ProtoParticle.EcalPIDmu       , -1.e+10 )

## @see LoKi::Cuts::PP_PhotonID
PP_PhotonID        = PP_INFO ( LHCb.ProtoParticle.PhotonID        , -1.e+10 )

## @see LoKi::Cuts::PP_VeloCharge
PP_VeloCharge      = PP_INFO ( LHCb.ProtoParticle.VeloCharge      , -1.e+10 )

## @see LoKi::Cuts::PP_TrackChi2PerDof
PP_TrackChi2PerDof = PP_INFO ( LHCb.ProtoParticle.TrackChi2PerDof , -1.e+10 )
## @see LoKi::Cuts::PP_TrackNumDof
PP_TrackNumDof     = PP_INFO ( LHCb.ProtoParticle.TrackNumDof     ,      -1 )

## @see LoKi::Cuts::PP_TrackType
PP_TrackType       = PP_INFO ( LHCb.ProtoParticle.TrackType       , LHCb.Track.TypeUnknown    )
## @see LoKi::Cuts::PP_TrackHistory
PP_TrackHistory    = PP_INFO ( LHCb.ProtoParticle.TrackHistory    , LHCb.Track.HistoryUnknown )
## @see LoKi::Cuts::PP_TrackP
PP_TrackP          = PP_INFO ( LHCb.ProtoParticle.TrackP          , -1.e+10 )
## @see LoKi::Cuts::PP_TrackPt
PP_TrackPt         = PP_INFO ( LHCb.ProtoParticle.TrackPt         , -1.e+10 )

## @see LoKi::Cuts::PP_CombDLLe
PP_CombDLLe        = PP_INFO ( LHCb.ProtoParticle.CombDLLe        , -1000  )
## @see LoKi::Cuts::PP_CombDLLmu
PP_CombDLLmu       = PP_INFO ( LHCb.ProtoParticle.CombDLLmu       , -1000  )
## @see LoKi::Cuts::PP_CombDLLpi
PP_CombDLLpi       = PP_INFO ( LHCb.ProtoParticle.CombDLLpi       , -1000  )
## @see LoKi::Cuts::PP_CombDLLk
PP_CombDLLk        = PP_INFO ( LHCb.ProtoParticle.CombDLLk        , -1000  )
## @see LoKi::Cuts::PP_CombDLLp
PP_CombDLLp        = PP_INFO ( LHCb.ProtoParticle.CombDLLp        , -1000  )

## @see LoKi::Cuts::PP_ProbNNe
PP_ProbNNe         = PP_INFO ( LHCb.ProtoParticle.ProbNNe         , -1000  )
## @see LoKi::Cuts::PP_ProbNNmu
PP_ProbNNmu        = PP_INFO ( LHCb.ProtoParticle.ProbNNmu        , -1000  )
## @see LoKi::Cuts::PP_ProbNNghost
PP_ProbNNghost     = PP_INFO ( LHCb.ProtoParticle.ProbNNghost     , -1000  )


## @see LoKi::Cuts::PP_ISMUON 
PP_ISMUON          = LoKi.ProtoParticles.IsMuon      ()
## @see LoKi::Cuts::PP_ISLOOSEMUON 
PP_ISLOOSEMUON     = LoKi.ProtoParticles.IsLooseMuon ()
## @see LoKi::Cuts::PP_ISMUONLOOSE
PP_ISMUONLOOSE     = LoKi.ProtoParticles.IsLooseMuon ()
## @see LoKi::Cuts::PP_ISTIGHTMUON 
PP_ISTIGHTMUON     = LoKi.ProtoParticles.IsTightMuon ()
## @see LoKi::Cuts::PP_ISMUONTIGHT
PP_ISMUONTIGHT     = LoKi.ProtoParticles.IsTightMuon ()
## @see LoKi::Cuts::PP_NSHAREDMU
PP_NSHAREDMU       = LoKi.ProtoParticles.NShared     ()

#
# adaptors:
#

## @see LoKi::Cuts::PP_TRCUT 
PP_TRCUT  = LoKi.ProtoParticles.TrackCut
## @see LoKi::Cuts::PP_TRFUN
PP_TRFUN  = LoKi.ProtoParticles.TrackFun


## @see LoKi::Cuts::PP_RICHTHRES 
PP_RICHTHRES    = LoKi.ProtoParticles.RichAboveThres
## @see LoKi::Cuts::PP_RICHTHRES_E  
PP_RICHTHRES_E  = LoKi.ProtoParticles.RichAboveThres ( 0 ) ## cpp.Rich.Electron ) 
## @see LoKi::Cuts::PP_RICHTHRES_MU
PP_RICHTHRES_MU = LoKi.ProtoParticles.RichAboveThres ( 1 ) ## cpp.Rich.Muon     ) 
## @see LoKi::Cuts::PP_RICHTHRES_PI
PP_RICHTHRES_PI = LoKi.ProtoParticles.RichAboveThres ( 2 ) ## cpp.Rich.Pion     ) 
## @see LoKi::Cuts::PP_RICHTHRES_K
PP_RICHTHRES_K  = LoKi.ProtoParticles.RichAboveThres ( 3 ) ## cpp.Rich.Kaon     ) 
## @see LoKi::Cuts::PP_RICHTHRES_P
PP_RICHTHRES_P  = LoKi.ProtoParticles.RichAboveThres ( 4 ) ## cpp.Rich.Proton   ) 

# =============================================================================
# vector-functions & streamers 
# =============================================================================

_ppv       = 'std::vector<const LHCb::ProtoParticle*>'
_vd        = 'std::vector<double>'
#
PPMaps      = LoKi.Functor             ( _ppv , _vd      )
PPMap       = LoKi.FunctorFromFunctor  ( _ppv , _vd      )
PPPipes     = LoKi.Functor             ( _ppv , _ppv     )
PPPipe      = LoKi.FunctorFromFunctor  ( _ppv , _ppv     )
PPFunVals   = LoKi.Functor             ( _ppv , 'double' )
PPFunVal    = LoKi.FunctorFromFunctor  ( _ppv , 'double' )
PPCutVals   = LoKi.Functor             ( _ppv , bool     )
PPCutVal    = LoKi.FunctorFromFunctor  ( _ppv , bool     )
PPSources   = LoKi.Functor             ('void', _ppv     )
PPSource    = LoKi.FunctorFromFunctor  ('void', _ppv     )
#

## @see LoKi::Cuts::PP_SOURCE 
PP_SOURCE   = LoKi.ProtoParticles.SourceTES 
## @see LoKi::Cuts::PP_NUM
PP_NUM      = LoKi.ProtoParticles.TESCounter 

PP_EMPTY    = LoKi.Functors.Empty ( _PP ) ()
PP_SIZE     = LoKi.Functors.Size  ( _PP ) ()
PP_VALID    = LoKi.Valid          ( _PP ) ()

# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__
    print 80*'*'
    
# =============================================================================
# The END 
# =============================================================================
