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


# Harry Cliff, Vava (lifetime unbiased line)
from StrippingSelections.StrippingD2hhLTUnbiased import StrippingD2hhLTUnbiasedConf
cloneableCharmLines += StrippingD2hhLTUnbiasedConf().lines()

stream.appendLines( cloneableCharmLines )

# Can't clone these lines

# Sebastian Bachmann (Double D0 for tag and probe)
from StrippingSelections.StrippingX2D0D0 import StrippingX2D0D0Conf
stream.appendLines( [ StrippingX2D0D0Conf().line() ] )


## # Matt Charles (KShh lines)
## from StrippingSelections.StrippingDstarD2KShh import StrippingDstarD2KShhConf
## cloneableCharmLines += StrippingDstarD2KShhConf().MakeLines()

## # Cambridge (Ksh line)
## from StrippingSelections.StrippingD2KS0h_KS02PiPi import StrippingD2KS0h_KS02PiPiConf
## cloneableCharmLines += [StrippingD2KS0h_KS02PiPiConf().D2KS0h_KS02PiPi() ]

## # Harry Cliff, Vava. Lifetime-unbiased line with micro/no-bias trigger 
## from StrippingSelections.StrippingD2hhLTUnbiasedMBNB import StrippingD2hhLTUnbiasedMBNBConf
## cloneableCharmLines += StrippingD2hhLTUnbiasedMBNBConf().lines()

## # Philip Hunt (4-body lines)
## from StrippingSelections.StrippingDstarPromptWithD02K3Pi import DstarPromptWithD02K3PiAllLinesConf
## DstarPromptWithD02K3PiConf = DstarPromptWithD02K3PiAllLinesConf()
## stream.appendLines( DstarPromptWithD02K3PiConf.Lines )


## # Patrick Spradlin, Philip Xing ( D*-tagged yCP lines )
## from StrippingSelections.StrippingDstarPromptWithD02HH import StrippingDstarPromptWithD02HHConf
## StrippingDstarPromptWithD02HHConf().prefix = 'DstarPrompt'
## stream.appendLines( StrippingDstarPromptWithD02HHConf().linesDstarOnly() )

