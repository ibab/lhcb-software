# $Id: StreamCharm.py,v 1.4 2010-09-08 09:05:18 gcowan Exp $
#
#   Charm stripping selection for microDST
#
#   all physics channels with hadronic final states from charm decay
#
#   @author Greig Cowan and Tom Blake 
#   @date 2010-09-07
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#


from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream("Charm") 

cloneableCharmLines = []

#
# Lines from Charm RD
# Ulrik Egede, Walter Bonivento
#

# Walter Bonivento, D0 -> mu mu searches
from StrippingSelections.StrippingD02MuMu import lines as D02MuMuLines
cloneableCharmLines += D02MuMuLines

from StrippingSelections.StrippingJpsippForD0MuMu import StrippingJpsippForD0MuMuConf
cloneableCharmLines += [ StrippingJpsippForD0MuMuConf().ForD0mumu_Line() ]

# Benoit Viaud ( 4-body SCS lines )
from StrippingSelections.StrippingDstarD02KKpipiRegular import StrippingDstarD02KKpipiRegularConf
from StrippingSettings.Stripping12.LineConfigDictionaries import D02KKpipiRegularConfig
DstarD0KKpipi = StrippingDstarD02KKpipiRegularConf('DstarD02KKpipiRegular',D02KKpipiRegularConfig)
cloneableCharmLines += DstarD0KKpipi.lines

# Benoit Viaud (D*+ -> D0 pi+, D0 -> K- K+ mu- mu+)
from StrippingSelections.StrippingDstarD02KKmumuRegular import StrippingDstarD02KKmumuRegularConf
DstarD0KKmumu = StrippingDstarD02KKmumuRegularConf('DstarD02KKmumuRegular')
cloneableCharmLines += DstarD0KKmumu.lines

#
# Charm CP Lines
# Patrick Spradlin, Marco Gersabeck 
#

# Marco Gersabeck and Harry Cliff ( Two-body untagged charm lines )
from StrippingSelections.StrippingD2hh import StrippingD2hhConf
cloneableCharmLines += StrippingD2hhConf().lines()

# Vanya Belyaev
from StrippingSelections.StrippingPromptCharm import StrippingPromptCharmConf
PromptCharmConf = StrippingPromptCharmConf ( config = {} )
cloneableCharmLines += PromptCharmConf.lines()

# 3-body lines (Rio + Oxford)
from StrippingSelections.StrippingD2hhh_conf import StrippingD2hhhConf

cloneableCharmLines += [ StrippingD2hhhConf().stripD2PPP(),
                      StrippingD2hhhConf().stripD2KKP(),
                      StrippingD2hhhConf().stripD2KKK() ]

cloneableCharmLines += [ StrippingD2hhhConf().stripD2KPP(),
                      StrippingD2hhhConf().stripD2KPPos() ]

cloneableCharmLines += [ StrippingD2hhhConf().stripD2hhh_inc() ]


# Matt Charles (KShh lines)
from StrippingSelections.StrippingDstarD2KShh import StrippingDstarD2KShhConf
cloneableCharmLines += StrippingDstarD2KShhConf().MakeLines()

# Cambridge (Ksh line)
from StrippingSelections.StrippingD2KS0h_KS02PiPi import StrippingD2KS0h_KS02PiPiConf
cloneableCharmLines += [StrippingD2KS0h_KS02PiPiConf().D2KS0h_KS02PiPi() ]

# Harry Cliff, Vava (lifetime unbiased line)
from StrippingSelections.StrippingD2hhLTUnbiased import StrippingD2hhLTUnbiasedConf
cloneableCharmLines += StrippingD2hhLTUnbiasedConf().lines()

# Harry Cliff, Vava. Lifetime-unbiased line with micro/no-bias trigger 
from StrippingSelections.StrippingD2hhLTUnbiasedMBNB import StrippingD2hhLTUnbiasedMBNBConf
cloneableCharmLines += StrippingD2hhLTUnbiasedMBNBConf().lines()

stream.appendLines( cloneableCharmLines )


# Can't clone these lines

# Philip Hunt (4-body lines)
from StrippingSelections.StrippingDstarPromptWithD02K3Pi import DstarPromptWithD02K3PiAllLinesConf
DstarPromptWithD02K3PiConf = DstarPromptWithD02K3PiAllLinesConf()
stream.appendLines( DstarPromptWithD02K3PiConf.Lines )

# Sebastian Bachmann (Double D0 for tag and probe)
from StrippingSelections.StrippingX2D0D0 import StrippingX2D0D0Conf
stream.appendLines( [ StrippingX2D0D0Conf().line() ] )

# Patrick Spradlin, Philip Xing ( D*-tagged yCP lines )
from StrippingSelections.StrippingDstarPromptWithD02HH import StrippingDstarPromptWithD02HHConf
StrippingDstarPromptWithD02HHConf().prefix = 'DstarPrompt'
stream.appendLines( StrippingDstarPromptWithD02HHConf().linesDstarOnly() )

# Phillip Urquijo
from StrippingSelections.StrippingD2XMuMuSS import StrippingD2XMuMuSSConf
StrippingD2XMuMuSSConf().prefix = 'MicroDST'
stream.appendLines( [ StrippingD2XMuMuSSConf().PiSS_line(),
                      StrippingD2XMuMuSSConf().PiOS_line(),
                      StrippingD2XMuMuSSConf().KSS_line(),
                      StrippingD2XMuMuSSConf().KOS_line() ] )


# Walter Bonivento, D0 -> mu mu searches
from StrippingSelections.StrippingDstarD02xx import  StrippingDstarD02xxConf
stream.appendLines( StrippingDstarD02xxConf("DstarD02xx", prefix = "MicroDST").lines() )



