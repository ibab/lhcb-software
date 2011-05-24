"""
Line config dictionaries for the Charm WG


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""


from GaudiKernel.SystemOfUnits import *


#Micro-DST, Vanya BELYAEV
#Test: Yes
PromptCharm = {
    'BUILDERTYPE' : 'StrippingPromptCharmConf', 
    'CONFIG'      : {
    'D0Prescale'      : 1.00,
    'DstarPrescale'   : 1.00,
    'DsPrescale'      : 1.00,
    'DplusPrescale'   : 1.00,
    'LamCPrescale'    : 1.00
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }

