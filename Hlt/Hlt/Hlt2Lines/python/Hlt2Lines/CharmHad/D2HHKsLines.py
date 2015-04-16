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
                                        'TisTosSpec'       : "Hlt1TwoTrackMVADecision%TOS",
                                       }
                            
        }

    def locallines(self):
        from Stages import MassFilter
        from Stages import D02KsPiPi_LL
        from Stages import D02KsPiPi_DD
        from Stages import D02KsKPi_LL
        from Stages import D02KsKPi_DD
        from Stages import D02KsKK_LL, D02KsKK_DD
        from Stages import Dst2D0pi
        stages = {'D02KsPiPi_LL'     : [MassFilter('D02KshPiPi_LL',inputs=[D02KsPiPi_LL])],
                  'D02KsPiPi_DD'     : [MassFilter('D02KshPiPi_DD',inputs=[D02KsPiPi_DD])],
                  'D02KsKPi_LL'      : [MassFilter('D02KshKPi_LL',inputs=[D02KsKPi_LL])],
                  'D02KsKPi_DD'      : [MassFilter('D02KshKPi_DD',inputs=[D02KsKPi_DD])],
                  'D02KsKK_LL'       : [MassFilter('D02KshKK_LL',inputs=[D02KsKK_LL])],
                  'D02KsKK_DD'       : [MassFilter('D02KshKK_DD',inputs=[D02KsKK_DD])],
##  
##  We can probably ue only D*-tagged HHKs candidates, so let's make the
##  corresponding D*-tagged lines
##  
##  recall that the first argument of Dst2D0pi is the "name" used in the dictionary of cuts
##  and the second argument is the "d0" list to be used.
##  
##  All of the following lines use a common dictionary "name":  Dst2D0pi_D02HHKsh
##  as the channels should be sufficiently similar to use common cuts.
##
                  'DstD02KshPiPi_LL' : [Dst2D0pi('Dst2D0pi_D02HHKsh',D02KsPiPi_LL)],
                  'DstD02KshPiPi_DD' : [Dst2D0pi('Dst2D0pi_D02HHKsh',D02KsPiPi_DD)],
                  'DstD02KshKPi_LL'  : [Dst2D0pi('Dst2D0pi_D02HHKsh',D02KsKPi_LL)],
                  'DstD02KshKPi_DD'  : [Dst2D0pi('Dst2D0pi_D02HHKsh',D02KsKPi_DD)],
                  'DstD02KshKK_LL'   : [Dst2D0pi('Dst2D0pi_D02HHKsh',D02KsKK_LL)],
                  'DstD02KshKK_DD'   : [Dst2D0pi('Dst2D0pi_D02HHKsh',D02KsKK_DD)]}

        return stages
