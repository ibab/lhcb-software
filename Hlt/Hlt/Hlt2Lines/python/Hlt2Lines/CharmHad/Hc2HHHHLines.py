from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond

def LTUNB_Cuts(motherMasses, massWindow = 75., aMassExtra = 10.,
               minTime = 0.1*picosecond, minDira = 0.9, minASumPt = 3000.*MeV) :
    if not hasattr(motherMasses, '__iter__') :
        motherMasses = (motherMasses,)
    motherMassMin = min(motherMasses) - massWindow
    motherMassMax = max(motherMasses) + massWindow
    return {
        'BPVLTIME_MIN'             :  minTime, 
        'BPVDIRA_MIN'              :  minDira,
        'ASUMPT_MIN'               :  minASumPt, 
        'AM_MIN'                   :  motherMassMin - aMassExtra,
        'AM_MAX'                   :  motherMassMax + aMassExtra,
        'Mass_M_MIN'               :  motherMassMin,
        'Mass_M_MAX'               :  motherMassMax,
        }

class CharmHadHc2HHHHLines() :
    def localcuts(self) :
        cuts = {
            'Xic02PKKPi_LTUNB' : LTUNB_Cuts((2471., 2695.))
            }
        return cuts

    def locallines(self) :
        from Stages import MassFilter, Xic02PKKPi_LTUNB
        
        stages = {
            'Xic02PKKPi_LTUNB' : [MassFilter('Xic02PKKPi_LTUNB',
                                             inputs = [Xic02PKKPi_LTUNB('Xic02PKKPi_LTUNB')])]
            }
        return stages
