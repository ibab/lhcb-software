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
        }

    def locallines(self):
        from Stages import D02KsPiPi_LL
        from Stages import D02KsPiPi_DD
        from Stages import D02KsKPi_LL
        from Stages import D02KsKPi_DD
        from Stages import D02KsKK_LL, D02KsKK_DD
        from Stages import MassFilter
        stages = {'D02KsPiPi_LL'     : [MassFilter('D02KshPiPi_LL',inputs=[D02KsPiPi_LL])],
                  'D02KsPiPi_DD'     : [MassFilter('D02KshPiPi_DD',inputs=[D02KsPiPi_DD])],
                  'D02KsKPi_LL'      : [MassFilter('D02KshKPi_LL',inputs=[D02KsKPi_LL])],
                  'D02KsKPi_DD'      : [MassFilter('D02KshKPi_DD',inputs=[D02KsKPi_DD])],
                  'D02KsKK_LL'       : [MassFilter('D02KshKK_LL',inputs=[D02KsKK_LL])],
                  'D02KsKK_DD'       : [MassFilter('D02KshKK_DD',inputs=[D02KsKK_DD])] }

        return stages
