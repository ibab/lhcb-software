# $Id: StreamCharm.py,v 1.4 2010-09-08 09:05:18 gcowan Exp $
#
#   Charm stripping selection
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

stream = StrippingStream("CharmControl")

# Who are these from?
from StrippingSelections.StrippingD2hhh_conf import StrippingD2hhhConf
stream.appendLines( [
    StrippingD2hhhConf().stripDs2PPP() ,
    StrippingD2hhhConf().stripDs2KKP() ,
    StrippingD2hhhConf().stripDs2KPPos()
    ] )

#
# Lines from Semileptonic group
# Rob Lambert
#
# Marta Calvi, Stefania Vecchi

from StrippingSelections.StrippingDstarVeryLooseWithD02Kpi import DstarVeryLooseWithD02KpiAllLinesConf
from StrippingSelections.StrippingDstarVeryLooseWithD02Kpi import confdict as DstarVeryLooseWithD02KpiConfDict

DstarVeryLooseWithD02Kpi = DstarVeryLooseWithD02KpiAllLinesConf( DstarVeryLooseWithD02KpiConfDict )
stream.appendLines( DstarVeryLooseWithD02Kpi.Lines )

# Liming Zhang
from StrippingSelections.StrippingD0ForBXX import StrippingD0forBXXConf
stream.appendLines( [ StrippingD0forBXXConf().D02KPiforBXXLine()
                      , StrippingD0forBXXConf().D02K3PiforBXXLine() ] )

from StrippingSelections.StrippingDForBSemi import StrippingDforBSemiConf
stream.appendLines( [ StrippingDforBSemiConf().DpforBSemiLine()
                      , StrippingDforBSemiConf().DsforBSemiLine()
                      , StrippingDforBSemiConf().LcforBSemiLine() ] )

#
# Lines from Gamma from trees
# Tim Gershon, Anton Poluektov, Andrea Contu
#

from StrippingSelections.StrippingDstar_D2KPiPi0 import lineWS, lineRS
stream.appendLines( [ lineWS, lineRS ] )

#
# Lines from Charm RD WG
# Walter Bonivento
#

from StrippingSelections.StrippingD02MuMu import lines as D02MuMuLines
stream.appendLines( D02MuMuLines )

from StrippingSelections.StrippingDstarD02xx import  StrippingDstarD02xxConf
stream.appendLines( StrippingDstarD02xxConf("DstarD02xxFull",LinePrefix="Full").lines() )

#
# Lines from Flavour WG
#

from StrippingSelections.StrippingCcbar2Ppbar import StrippingCcbar2PpbarConf
stream.appendLines( [ StrippingCcbar2PpbarConf().Nominal_Line() ] )


#
# Lines from CP Charm WG
# Marco Gersabeck and Patrick Spradlin
#


CharmLines = []

from StrippingSelections.StrippingD2hh import StrippingD2hhConf
CharmLines += StrippingD2hhConf().lines()


from StrippingSelections.StrippingPromptCharm import StrippingPromptCharmConf
from StrippingSettings.Stripping11.LineConfigDictionaries import PromptCharmConfig

promptCharm = StrippingPromptCharmConf ( config = PromptCharmConfig ) 
CharmLines += promptCharm.lines()


from StrippingSelections.StrippingD2hhh_conf import StrippingD2hhhConf
CharmLines += [ StrippingD2hhhConf().stripD2PPP(),
             StrippingD2hhhConf().stripD2KPP(),
             StrippingD2hhhConf().stripD2KKP(),
             StrippingD2hhhConf().stripD2KPPos(),
             StrippingD2hhhConf().stripD2KKK(),
             StrippingD2hhhConf().stripD2hhh_inc(),
             ]

from StrippingSelections.StrippingDstarD2KShh import StrippingDstarD2KShhConf
CharmLines += StrippingDstarD2KShhConf().MakeLines() 

from StrippingSelections.StrippingD2KS0h_KS02PiPi import StrippingD2KS0h_KS02PiPiConf
CharmLines += [ StrippingD2KS0h_KS02PiPiConf().D2KS0h_KS02PiPi() ]

from StrippingSelections.StrippingDstarPromptWithD02K3Pi import StrippingDstarPromptWithD02K3PiConf
DstarPromptWithD02K3Pi = StrippingDstarPromptWithD02K3PiConf(
    'DstarPromptWithD02K3Pi',
    config = StrippingDstarPromptWithD02K3PiConf._default_config
    )

CharmLines += DstarPromptWithD02K3Pi.lines

from StrippingSelections.StrippingDstarD02KKpipiRegular import StrippingDstarD02KKpipiRegularConf
from StrippingSettings.Stripping11.LineConfigDictionaries import D02KKpipiRegularConfig

DstarD0KKpipi = StrippingDstarD02KKpipiRegularConf('DstarD02KKpipiRegular',D02KKpipiRegularConfig)
CharmLines +=  DstarD0KKpipi.lines

# Make a cloned copy of the charm CP lines to be used in the full stream
# with a set prescale. The CP lines will also appear in StreamCharmMicroDST
        
fullDSTLines = [ CharmLine.clone( 'Full' + CharmLine._name ) for CharmLine in CharmLines ]

# clone all lines with a prescale
# scale = 0.1
# fullDSTLines = [ CharmLine.clone( 'Full' + CharmLine._name, prescale = scale*CharmLine._prescale ) for CharmLine in CharmLines ]

stream.appendLines( fullDSTLines )

# Unfortunate hack to introduce two copies of the StrippingDstarPromptWithD02HH lines. Cloning
# fails for these lines.

from StrippingSelections.StrippingDstarPromptWithD02HH import StrippingDstarPromptWithD02HHConf
StrippingDstarPromptWithD02HHConf().LinePrefix = 'FullDstarPrompt'
stream.appendLines( StrippingDstarPromptWithD02HHConf().linesDstarOnly() )
