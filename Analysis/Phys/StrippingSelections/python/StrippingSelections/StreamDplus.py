#
#   Stripping selections for D+ -> h+ h+ h-
#
#   @author Mat Charles <m.charles1@physics.ox.ac.uk>
#   @date 2010-01-18
#
#   Based on StreamDstar.py by A. Poluektov
#   Picks up several D+ -> 3 body streams developed
#   by UFRJ + PUC + CBPF + Oxford.

from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingD2hhh

stream = StrippingStream("Dplus")
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
        ] )

