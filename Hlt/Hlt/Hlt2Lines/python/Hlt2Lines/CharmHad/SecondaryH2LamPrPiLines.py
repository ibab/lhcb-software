from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser


class SecondaryH2LamPrPiLines(Hlt2LinesConfigurableUser):
    def localcuts(self) :
        return {
                 # cuts for the mass filters
                 'SecondaryH2LambdaPrPi_LLLL_Mass' : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'SecondaryH2LambdaPrPi_DDLL_Mass' : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'SecondaryH2LambdaPrPi_DDDD_Mass' : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
        }

    def locallines(self):
        from Stages import MassFilter
        from Stages import H2LambdaPrPi_LLLL, H2LambdaPrPi_DDLL
        from Stages import H2LambdaPrPi_DDDD
        from Stages import H2LambdaPiPr_LLLL, H2LambdaPiPr_DDLL
        from Stages import H2LambdaPiPr_DDDD

#  These are the "real" H --> Lambda,p+,pi- lines
        Filtered_LamPrPi_LLLL = MassFilter('SecondaryH2LambdaPrPi_LLLL_Mass',
                                            inputs=[H2LambdaPrPi_LLLL])
        Filtered_LamPrPi_DDLL = MassFilter('SecondaryH2LambdaPrPi_DDLL_Mass',
                                            inputs=[H2LambdaPrPi_DDLL])
        Filtered_LamPrPi_DDDD = MassFilter('SecondaryH2LambdaPrPi_DDLL_Mass',
                                            inputs=[H2LambdaPrPi_DDDD])

#  These are the "fake" H --> Lambda,pi+,pi- lines (no net baryon number)
        Filtered_LamPiPr_LLLL = MassFilter('SecondaryH2LambdaPrPi_LLLL_Mass',
                                            inputs=[H2LambdaPiPr_LLLL])
        Filtered_LamPiPr_DDLL = MassFilter('SecondaryH2LambdaPrPi_DDLL_Mass',
                                            inputs=[H2LambdaPiPr_DDLL])
        Filtered_LamPiPr_DDDD = MassFilter('SecondaryH2LambdaPrPi_DDLL_Mass',
                                            inputs=[H2LambdaPiPr_DDDD])


        stages = {
                  'SecondaryH2LamPpPimLLLLTurbo'  :  [Filtered_LamPrPi_LLLL],
                  'SecondaryH2LamPpPimDDLLTurbo'  :  [Filtered_LamPrPi_DDLL],
                  'SecondaryH2LamPpPimDDDDTurbo'  :  [Filtered_LamPrPi_DDDD],
                  'SecondaryH2LamPipPmLLLLTurbo'  :  [Filtered_LamPiPr_LLLL],
                  'SecondaryH2LamPipPmDDLLTurbo'  :  [Filtered_LamPiPr_DDLL],
                  'SecondaryH2LamPipPmDDDDTurbo'  :  [Filtered_LamPiPr_DDDD],
                 }
        

        return stages
