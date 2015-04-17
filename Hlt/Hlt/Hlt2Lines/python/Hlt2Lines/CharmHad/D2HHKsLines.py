from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2HHKshLines(Hlt2LinesConfigurableUser):
    def localcuts(self) :
        return {
                 'D02KshPiPi_LL' : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshPiPi_DD' : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshKPi_LL'  : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshKPi_DD'  : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshPiK_LL'  : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshPiK_DD'  : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshKK_LL'   : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshKK_DD'   : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'Dst2D0pi_D02HHKsh' : {
                                        'DeltaM_AM_MIN'    : 135 * MeV,
                                        'DeltaM_AM_MAX'    : 175 * MeV,
                                        'DeltaM_MIN'       : 135 * MeV,
                                        'DeltaM_MAX'       : 165 * MeV,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                        'TisTosSpec'       : "Hlt1.*MVA.*Decision%TOS",
                                       }
                            
        }

    def locallines(self):
        from Stages import MassFilter
        from Stages import D02KsPiPi_LL
        from Stages import D02KsPiPi_DD
        from Stages import D02KsKPi_LL
        from Stages import D02KsKPi_DD
        from Stages import D02KsPiK_LL, D02KsPiK_DD
        from Stages import D02KsKK_LL, D02KsKK_DD
        from Stages import TagDecay
        from Stages import SharedSoftTagChild_pi

        stages = {'D02KS0PiPi_LL'     : [MassFilter('D02KshPiPi_LL',inputs=[D02KsPiPi_LL])],
                  'D02KS0PiPi_DD'     : [MassFilter('D02KshPiPi_DD',inputs=[D02KsPiPi_DD])],
                  'D02KS0KPi_LL'      : [MassFilter('D02KshKPi_LL',inputs=[D02KsKPi_LL])],
                  'D02KS0KPi_DD'      : [MassFilter('D02KshKPi_DD',inputs=[D02KsKPi_DD])],
                  'D02KS0PiK_LL'      : [MassFilter('D02KshPiK_LL',inputs=[D02KsPiK_LL])],
                  'D02KS0PiK_DD'      : [MassFilter('D02KshPiK_DD',inputs=[D02KsPiK_DD])],
                  'D02KS0KK_LL'       : [MassFilter('D02KshKK_LL',inputs=[D02KsKK_LL])],
                  'D02KS0KK_DD'       : [MassFilter('D02KshKK_DD',inputs=[D02KsKK_DD])],
##  
##  We can probably use only D*-tagged HHKs candidates, so let's make the
##  corresponding D*-tagged lines
##  
##  recall that the first argument of Dst2D0pi is the "name" used in the dictionary of cuts
##  and the second argument is the "d0" list to be used.
##  
##  All of the following lines use a common dictionary "name":  Dst2D0pi_D02HHKsh
##  as the channels should be sufficiently similar to use common cuts.
##
                  'DstD02KshPiPi_LL' : [TagDecay('Dst2D0pi_D02HHKsh',
                     decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"],
                     inputs = [D02KsPiPi_LL,SharedSoftTagChild_pi])],
                  'DstD02KshPiPi_DD' : [TagDecay('Dst2D0pi_D02HHKsh',
                     decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"],
                     inputs = [D02KsPiPi_DD,SharedSoftTagChild_pi])],

## These lines will produce D*+ --> D0,pi+; D0 --> D0 --> K-,pi+,Kshort candidates 
## and also D*- --> D0bar,pi-;  D0bar --> K+,pi-,Kshort candidates; Ignoring CPV 
##  in K0 and K0bar decays to Kshort, these are CP conjugate decays.
                  'DstD02KshKPi_LL'  : [TagDecay('Dst2D0pi_D02HHKsh',
                     decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D~0 pi-"],
                     inputs = [D02KsKPi_LL,SharedSoftTagChild_pi])],
                  'DstD02KshKPi_DD'  : [TagDecay('Dst2D0pi_D02HHKsh',
                     decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D~0 pi-"],
                     inputs = [D02KsKPi_DD,SharedSoftTagChild_pi])],

## These lines will produce D*+ --> D0,pi+; D0 --> D0 --> K+,pi-,Kshort candidates
## and also D*- --> D0bar,pi-;  D0bar --> K-,pi+,Kshort candidates; Ignoring CPV
##  in K0 and K0bar decays to Kshort, these are CP conjugate decays.
                  'DstD02KshPiK_LL'  : [TagDecay('Dst2D0pi_D02HHKsh',
                     decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D~0 pi-"],
                     inputs = [D02KsPiK_LL,SharedSoftTagChild_pi])],
                  'DstD02KshPiK_DD'  : [TagDecay('Dst2D0pi_D02HHKsh',
                     decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D~0 pi-"],
                     inputs = [D02KsPiK_DD,SharedSoftTagChild_pi])],

                  'DstD02KshKK_LL'  : [TagDecay('Dst2D0pi_D02HHKsh',
                     decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"],
                     inputs = [D02KsKK_LL,SharedSoftTagChild_pi])],
                  'DstD02KshKK_DD'  : [TagDecay('Dst2D0pi_D02HHKsh',
                     decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"],
                     inputs = [D02KsKK_DD,SharedSoftTagChild_pi])]
                 }
        

        return stages
