#
#   Charm stripping selection
#
#   all physics channels with hadronic final states from charm decay
#
#   @author A. Poluektov
#   @date 2010-01-21
# 

from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections.StrippingD2KS0h_KS02PiPi import StrippingD2KS0h_KS02PiPiConf
from StrippingSelections import StrippingD2hhh
from StrippingSelections import StrippingD2hhhXS
from StrippingSelections import StrippingD5h
from StrippingSelections import StrippingDstarD2KShh
from StrippingSelections import StrippingDstarD2KShhRobust
from StrippingSelections import StrippingDstar_D2KPiPi0
from StrippingSelections import StrippingD2hhNoPID
from StrippingSelections import StrippingD2hhNoPIDXsec
from StrippingSelections import StrippingD2Kh
from StrippingSelections import StrippingD2KhLoose
from StrippingSelections import StrippingDstarPromptWithD02HHNoPt
from StrippingSelections import StrippingDstarD02KKpipiRobust
from StrippingSelections import StrippingDstarD02KKpipiRegular
from StrippingSelections import StrippingDstarD02KKmumuRobust
from StrippingSelections import StrippingDstarD02KKmumuRegular
#from StrippingSelections.StrippingDstarPromptWithD02K3Pi import StrippingDstarPromptWithD02K3PiConf
#from StrippingSelections.StrippingD02K3PiForCrossSection import StrippingD02K3PiForCrossSectionConf

from StrippingSelections.StrippingDstarPromptWithD02HH import StrippingDstarPromptWithD02HHConf
from StrippingSelections.StrippingDstarVeryLooseWithD02HH import StrippingDstarVeryLooseWithD02HHConf
from StrippingSelections.StrippingDstarD2KShh          import StrippingDstarD2KShhConf
from StrippingSelections.StrippingDstarD02xx import  StrippingDstarD02xxConf
from StrippingSelections.StrippingCcbar2Ppbar import StrippingCcbar2PpbarConf
from StrippingSelections.StrippingD0ForBXX import StrippingD0forBXXConf
from StrippingSelections.StrippingDForBSemi import StrippingDforBSemiConf

from StrippingSelections.StrippingBd2KstarMuMu import StrippingBd2KstarMuMuConf
from StrippingSelections.StrippingLambdac      import StrippingLambdacConf

from StrippingSelections.StrippingDs2piPhiNoPt import StrippingDs2piPhiConf

from StrippingSelections.StrippingD02K3Pi import StrippingD02K3PiForXSectionConf 

stream = StrippingStream("Charm")

stream.appendLines( [
          StrippingD2KS0h_KS02PiPiConf().D2KS0h_KS02PiPi()
        , StrippingD2hhh.lineD2PPP_A_NoPID_Bkg
#        , StrippingD2hhh.lineD2PPP_B_NoPID_Bkg
        , StrippingD2hhh.lineD2PPP_A_NoPID_Sig
#        , StrippingD2hhh.lineD2PPP_B_NoPID_Sig
        , StrippingD2hhh.lineD2KPP_A_NoPID_Bkg
#        , StrippingD2hhh.lineD2KPP_B_NoPID_Bkg
        , StrippingD2hhh.lineD2KPP_A_LoosePID_Bkg
        , StrippingD2hhh.lineD2KPP_B_LoosePID_Bkg
        , StrippingD2hhh.lineD2KPP_A_NoPID_Sig
#        , StrippingD2hhh.lineD2KPP_B_NoPID_Sig
        , StrippingD2hhh.lineD2KPP_A_LoosePID_Sig
        , StrippingD2hhh.lineD2KPP_B_LoosePID_Sig
        , StrippingD2hhh.lineD2KKP_B_NoPID_Bkg
        , StrippingD2hhh.lineD2KKP_A_LoosePID_Bkg
        , StrippingD2hhh.lineD2KKP_B_LoosePID_Bkg
        , StrippingD2hhh.lineD2KKP_B_NoPID_Sig
        , StrippingD2hhh.lineD2KKP_A_LoosePID_Sig
        , StrippingD2hhh.lineD2KKP_B_LoosePID_Sig
        , StrippingD2hhh.lineD2KKK_A_LoosePID_Bkg
        , StrippingD2hhh.lineD2KKK_B_LoosePID_Bkg
        , StrippingD2hhh.lineD2KKK_A_LoosePID_Sig
        , StrippingD2hhh.lineD2KKK_B_LoosePID_Sig
        , StrippingD2hhh.lineD2KKP_A_NoPID_Sig
        , StrippingD2hhh.lineD2KKP_A_NoPID_Bkg
#        , StrippingD2hhh.lineDs2KPP_A_LoosePID_Sig
#        , StrippingD2hhh.lineDs2KPP_B_LoosePID_Sig
#        , StrippingD2hhh.lineDs2KPP_A_LoosePID_Bkg
#        , StrippingD2hhh.lineDs2KPP_B_LoosePID_Bkg
#        , StrippingD2hhh.lineD2KPP_DCS_A_LoosePID_Sig
#        , StrippingD2hhh.lineD2KPP_DCS_B_LoosePID_Sig
#        , StrippingD2hhh.lineD2KPP_DCS_A_LoosePID_Bkg
#        , StrippingD2hhh.lineD2KPP_DCS_B_LoosePID_Bkg
        , StrippingD2hhh.lineD2HHH
#        , StrippingD2hhhXS.lineD2PPP_B_NoPID_BkgXS
#        , StrippingD2hhhXS.lineD2PPP_B_NoPID_SigXS
#        , StrippingD2hhhXS.lineD2KPP_B_NoPID_BkgXS
        , StrippingD2hhhXS.lineD2KPP_B_LoosePID_BkgXS
#        , StrippingD2hhhXS.lineD2KPP_B_NoPID_SigXS
        , StrippingD2hhhXS.lineD2KPP_B_LoosePID_SigXS
#        , StrippingD2hhhXS.lineD2KKP_B_NoPID_BkgXS
        , StrippingD2hhhXS.lineD2KKP_B_LoosePID_BkgXS
#        , StrippingD2hhhXS.lineD2KKP_B_NoPID_SigXS
        , StrippingD2hhhXS.lineD2KKP_B_LoosePID_SigXS
#        , StrippingD2hhhXS.lineD2KKK_B_LoosePID_BkgXS
#        , StrippingD2hhhXS.lineD2KKK_B_LoosePID_SigXS
#        , StrippingD2hhhXS.lineDs2KPP_B_LoosePID_SigXS
#        , StrippingD2hhhXS.lineDs2KPP_B_LoosePID_BkgXS
#        , StrippingD2hhhXS.lineD2KPP_DCS_B_LoosePID_SigXS
#        , StrippingD2hhhXS.lineD2KPP_DCS_B_LoosePID_BkgXS
#        , StrippingD5h.lineD2KKPPP_B_NoPID_Bkg 
#        , StrippingD5h.lineD2KKPPP_B_LoosePID_Bkg 
#        , StrippingD5h.lineD2KKPPP_B_NoPID_Sig    
#        , StrippingD5h.lineD2KKPPP_B_LoosePID_Sig
#        , StrippingD5h.lineD2KPPPP_B_NoPID_Bkg 
#        , StrippingD5h.lineD2KPPPP_B_LoosePID_Bkg 
#        , StrippingD5h.lineD2KPPPP_B_NoPID_Sig    
#        , StrippingD5h.lineD2KPPPP_B_LoosePID_Sig 
        , StrippingDstar_D2KPiPi0.lineRS
        , StrippingDstar_D2KPiPi0.lineWS
        , StrippingDstarVeryLooseWithD02HHConf().line()
        , StrippingD2hhNoPID.line
        , StrippingD2hhNoPIDXsec.line
        , StrippingD2Kh.DKhPreselLine
        , StrippingD2KhLoose.DKhPreselLooseLine
        , StrippingD0forBXXConf().D02KPiforBXXLine()
        , StrippingD0forBXXConf().D02K3PiforBXXLine()
        , StrippingDforBSemiConf().DpforBSemiLine()
        , StrippingDforBSemiConf().DsforBSemiLine()
        , StrippingDforBSemiConf().LcforBSemiLine()
        , StrippingBd2KstarMuMuConf().line_for_DKpipipi()
        , StrippingDstarPromptWithD02HHNoPt.lineDstarPromptWithD02HHNoPt
#        , StrippingDstarD02KKpipiRobust.line1
#        , StrippingDstarD02KKpipiRegular.line1
        , StrippingDstarD02KKmumuRobust.line1
        , StrippingDstarD02KKmumuRegular.line1
          ,StrippingDs2piPhiConf().line() 
    ] )
stream.appendLines( StrippingDstarPromptWithD02HHConf().lines() )
stream.appendLines( StrippingDstarD2KShhConf().MakeLines() )
stream.appendLines( StrippingDstarD02xxConf().lines() )
# Charmonium -> p pbar
stream.appendLines( [ StrippingCcbar2PpbarConf().Nominal_Line() ] )
stream.appendLines( [ StrippingCcbar2PpbarConf().Loose_Line() ] )
#stream.appendLines( StrippingDstarPromptWithD02K3PiConf().lines() )
#stream.appendLines( StrippingD02K3PiForCrossSectionConf().lines() )
D02K3PiForXSection = StrippingD02K3PiForXSectionConf(
  'D02K3PiForCrossSection'
  , config = StrippingD02K3PiForXSectionConf._default_config)

stream.appendLines( StrippingLambdacConf().lines() )

from StrippingSelections.StrippingPromptCharm import Lines as PromptCharmLines

stream.appendLines ( PromptCharmLines ) 

