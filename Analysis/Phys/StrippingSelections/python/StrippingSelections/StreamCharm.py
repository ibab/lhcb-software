#
#   Charm stripping selection
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

from StrippingSelections.StrippingDstarPromptWithD02HH import StrippingDstarPromptWithD02HHConf


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
        , StrippingD2hhh.lineD2HHH
	, StrippingDstarD2KShh.line1
	, StrippingDstarD2KShh.line2
	, StrippingDstarD2KShh.line3
	, StrippingDstarD2KShh.line4
	, StrippingDstarD2KShh.line5
	, StrippingDstarD2KShh.line6
	, StrippingDstarD2KShh.line7
	, StrippingDstarD2KShh.line8
	, StrippingDstarD2KShh.line9
	, StrippingDstarD2KShh.line10
	, StrippingDstarD2KShh.line11
	, StrippingDstarD2KShh.line12
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
    ] )
stream.appendLines( StrippingDstarPromptWithD02HHConf().lines() )
