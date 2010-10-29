# $Id: StreamBhadron.py,v 1.4 2010-09-08 09:05:18 gcowan Exp $
#
#   Bhadron stripping selection
#
#   all physics channels with hadronic final states from b decay
#
#   @author Greig Cowan and Tom Blake 
#   @date 2010-09-07
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream("Bhadron")


#
# Lines from Flavour WG
# Rolf Olderman
# 

from StrippingSelections.StrippingB2twobody import B2twobodyLine
stream.appendLines( B2twobodyLine() )

# Walters lines 
from StrippingSelections.StrippingB2twobody_prompt import B2twobody_promptLine
stream.appendLines( B2twobody_promptLine() )

#
# Lines from Semileptonic task force
# Rob Lambert and Anne Keune 
# 

from StrippingSelections.StrippingBd2DstarTauNu import line as Bd2DstarTauNuLine
stream.appendLines( [ Bd2DstarTauNuLine ] )

#
# Lines from Gamma from loops WG
# Vincenzo Vagnoni and Ulrich Kerzel
#

from StrippingSelections.StrippingB2hhLTUnbiased import StrippingB2hhLTUnbiasedConf as B2hhLTUnbiased
stream.appendLines( [B2hhLTUnbiased().StripB2hhLTUnbiasedLoose(),
                     B2hhLTUnbiased().StripB2hhLTUnbiased(),
                     B2hhLTUnbiased().StripB2hhLTUnbiasedTrigger()] )

from StrippingSelections.StrippingHb2Charged2Body import Hb2Charged2BodyLines
stream.appendLines( Hb2Charged2BodyLines().lines() )

from StrippingSelections.StrippingBu2hhh import line as Bu2hhhLine
from StrippingSelections.StrippingB2KShh import StrippingB2KShhConf
stream.appendLines( [StrippingB2KShhConf().B2KSLLhh(),
                     StrippingB2KShhConf().B2KSDDhh(),
                     Bu2hhhLine
                     ])

from StrippingSelections.StrippingB2HHPi0 import StrippingB2HHPi0Conf
stream.appendLines( [StrippingB2HHPi0Conf().B2HHPi0_Merged(),
                     StrippingB2HHPi0Conf().B2HHPi0_Resolved()
                     ])


#
# Lines from Gamma from trees WG
# Tim Gershon, Anton Poluektov, Vava Gligorov
#
from StrippingSelections.StrippingB2DX import B2DXLines
stream.appendLines ( B2DXLines().lines )

# Susan Haines 
from StrippingSelections.StrippingBu2D0h_D02KShh_NoPID import _StrippingBu2D0h_D02KShh_NoPIDConf as Bu2D0h_D02KShh_NoPIDConf
stream.appendLines ( [ Bu2D0h_D02KShh_NoPIDConf.Line_LL, Bu2D0h_D02KShh_NoPIDConf.Line_DD ] )

# Steve Blusk 
from StrippingSelections.StrippingB2D3H import StrippingB2D3HLoose
stream.appendLines( StrippingB2D3HLoose('Loose').lines ) 


#
# Lines from beta_s WG
# Olivier Leroy, Wouter Hulsebergen and Greig Cowan
#

from StrippingSelections.StrippingBs2EtacPhi import Bs2EtacPhiAllLinesConf
from StrippingSelections.StrippingBs2EtacPhi import confdict as Bs2EtacPhiConfDict
stream.appendLines( Bs2EtacPhiAllLinesConf(Bs2EtacPhiConfDict).Lines )

from StrippingSelections.StrippingBs2ChicPhi_Chic2KKPiPi import Bs2ChicPhi_Chic2KKPiPiAllLinesConf
from StrippingSelections.StrippingBs2ChicPhi_Chic2KKPiPi import confdict as Bs2ChicPhi_Chic2KKPiPiConfDict
stream.appendLines( Bs2ChicPhi_Chic2KKPiPiAllLinesConf(Bs2ChicPhi_Chic2KKPiPiConfDict).Lines )

from StrippingSelections.StrippingBs2ChicPhi_Chic2PiPiPiPi import Bs2ChicPhi_Chic2PiPiPiPiAllLinesConf
from StrippingSelections.StrippingBs2ChicPhi_Chic2PiPiPiPi import confdict as Bs2ChicPhi_Chic2PiPiPiPiConfDict
stream.appendLines( Bs2ChicPhi_Chic2PiPiPiPiAllLinesConf(Bs2ChicPhi_Chic2PiPiPiPiConfDict).Lines )

from StrippingSelections.StrippingBs2PhiPhi import StrippingBs2PhiPhiConf
Bs2PhiPhiConf = StrippingBs2PhiPhiConf("Bs2PhiPhi", StrippingBs2PhiPhiConf.config_Sept2010 )
stream.appendLines( Bs2PhiPhiConf.lines )









## #
## # Lines from Flavour WG
## # Rolf Olderman
## #
## from StrippingSelections.StrippingB2twobody import B2twobodyLine
## stream.appendLines( B2twobodyLine() )




## #
## # Lines from beta_s WG
## # Olivier Leroy, Wouter Hulsebergen and Greig Cowan
## #
## from StrippingSelections.StrippingBs2PhiPhi import StrippingBs2PhiPhiConf
## Bs2PhiPhiConf = StrippingBs2PhiPhiConf("Bs2PhiPhi", StrippingBs2PhiPhiConf.config_Sept2010 )
## stream.appendLines( Bs2PhiPhiConf.lines )

