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

from StrippingSelections import StrippingD2hhh
from StrippingSelections import StrippingDstarD2KShh
from StrippingSelections import StrippingDstarD2KShhRobust
from StrippingSelections import StrippingDstarD02PipiMuMuPiMuKPiKmu
from StrippingSelections import StrippingDstar_D2KPiPi0
from StrippingSelections import StrippingD2hhNoPID

from StrippingSelections.StrippingDstarPromptWithD02HH import StrippingDstarPromptWithD02HHConf
from StrippingSelections.StrippingDstarD2KShh          import StrippingDstarD2KShhConf
from StrippingSelections.StrippingDstarD02xx import  StrippingDstarD02xxConf
from StrippingSelections.StrippingCcbar2Ppbar import StrippingCcbar2PpbarConf

stream = StrippingStream("Charm")

stream.appendLines( [ 
          StrippingD2hhh.lineD2PPP_A_NoPID_Bkg
        , StrippingD2hhh.lineD2PPP_B_NoPID_Bkg
        , StrippingD2hhh.lineD2PPP_A_NoPID_Sig
        , StrippingD2hhh.lineD2PPP_B_NoPID_Sig
        , StrippingD2hhh.lineD2KPP_A_NoPID_Bkg
        , StrippingD2hhh.lineD2KPP_B_NoPID_Bkg
        , StrippingD2hhh.lineD2KPP_A_LoosePID_Bkg
        , StrippingD2hhh.lineD2KPP_B_LoosePID_Bkg
        , StrippingD2hhh.lineD2KPP_A_NoPID_Sig
        , StrippingD2hhh.lineD2KPP_B_NoPID_Sig
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
        , StrippingD2hhh.lineDs2KPP_A_LoosePID_Sig
        , StrippingD2hhh.lineDs2KPP_B_LoosePID_Sig
        , StrippingD2hhh.lineDs2KPP_A_LoosePID_Bkg
        , StrippingD2hhh.lineDs2KPP_B_LoosePID_Bkg
        , StrippingD2hhh.lineD2KPP_DCS_A_LoosePID_Sig
        , StrippingD2hhh.lineD2KPP_DCS_B_LoosePID_Sig
        , StrippingD2hhh.lineD2KPP_DCS_A_LoosePID_Bkg
        , StrippingD2hhh.lineD2KPP_DCS_B_LoosePID_Bkg
        , StrippingD2hhh.lineD2HHH
	, StrippingDstarD2KShhRobust.line1 
	, StrippingDstarD2KShhRobust.line2 
	, StrippingDstarD2KShhRobust.line3 
	, StrippingDstarD2KShhRobust.line4 
	, StrippingDstarD2KShhRobust.line5 
	, StrippingDstarD2KShhRobust.line6 
	, StrippingDstarD2KShhRobust.line7 
	, StrippingDstarD2KShhRobust.line8 
	, StrippingDstarD2KShhRobust.line9 
	, StrippingDstarD2KShhRobust.line10 
	, StrippingDstarD2KShhRobust.line11 
	, StrippingDstarD2KShhRobust.line12
	, StrippingDstarD02PipiMuMuPiMuKPiKmu.lineDstarWithD02MuMu
	, StrippingDstarD02PipiMuMuPiMuKPiKmu.lineDstarWithD0PiPiForDstarWithD02MuMu
        , StrippingDstar_D2KPiPi0.lineRS
        , StrippingDstar_D2KPiPi0.lineWS
        , StrippingD2hhNoPID.line
    ] )
stream.appendLines( StrippingDstarPromptWithD02HHConf().lines() )
stream.appendLines( StrippingDstarD2KShhConf().MakeLines() )
stream.appendLines( StrippingDstarD02xxConf().lines() )
# Charmonium -> p pbar
stream.appendLines( [ StrippingCcbar2PpbarConf().Nominal_Line() ] )
stream.appendLines( [ StrippingCcbar2PpbarConf().Loose_Line() ] )


