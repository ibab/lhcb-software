from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond

def mass_cuts(motherMasses, massWindow = 75, aMassExtra = 10.) :
    if not hasattr(motherMasses, '__iter__') :
        motherMasses = (motherMasses,)
    motherMassMin = min(motherMasses) - massWindow
    motherMassMax = max(motherMasses) + massWindow
    return {
        'AM_MIN'                   :  motherMassMin - aMassExtra,
        'AM_MAX'                   :  motherMassMax + aMassExtra,
        'Mass_M_MIN'               :  motherMassMin,
        'Mass_M_MAX'               :  motherMassMax,
        }

class CharmHadHc2HHHHLines() :
    def localcuts(self) :
        Xic0OmegacMasses = (2471., 2695.)
        Xic0OmegacMassCuts = mass_cuts(Xic0OmegacMasses)
        cuts = {
            'Xic02PKKPi_LTUNB' : 
            dict(TisTosSpec = "Hlt1Track.*Decision%TIS",
                 BPVLTIME_MIN = 0.1*picosecond, 
                 BPVDIRA_MIN = 0.9,
                 ASUMPT_MIN = 3000.*MeV,
                 **Xic0OmegacMassCuts),
            'Xic02PKKPi' : 
            dict(#Trk_ALL_TRCHI2DOF_MAX = ,
                Trk_ALL_PT_MIN = 200.*MeV,
                #Trk_ALL_P_MIN = ,
                Trk_ALL_MIPCHI2DV_MIN = 4.0,
                ASUMPT_MIN = 3000.*MeV,
                Trk_1OF4_PT_MIN = 400.*MeV,
                Trk_2OF4_PT_MIN = 1000.*MeV,
                Trk_1OF4_MIPCHI2DV_MIN = 12.0,
                Trk_2OF4_MIPCHI2DV_MIN = 8.0,
                #VCHI2PDOF_MAX = ,
                BPVDIRA_MIN = 0.99995, 
                BPVVDCHI2_MIN = 10.0,
                BPVLTIME_MIN = 0.1 * picosecond,
                **Xic0OmegacMassCuts),
            }
        return cuts

    def locallines(self) :
        from Stages import MassFilter, Xic02PKKPi_LTUNB, Xic02PKKPi
        
        stages = {
            'Xic02PKKPi_LTUNB' : [MassFilter('Xic02PKKPi_LTUNB',
                                             inputs = [Xic02PKKPi_LTUNB('Xic02PKKPi_LTUNB')])],
            'Xic02PKKPi' : [MassFilter('Xic02PKKPi',
                                       inputs = [Xic02PKKPi('Xic02PKKPi')])],
            }
        return stages
