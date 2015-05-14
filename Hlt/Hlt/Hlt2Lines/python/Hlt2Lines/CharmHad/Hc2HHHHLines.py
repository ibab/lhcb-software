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
            'Xic02PKKPi_LTUNB_Turbo' : 
            dict(TisTosSpec = "Hlt1Track.*Decision%TIS",
                 BPVLTIME_MIN = 0.1*picosecond, 
                 BPVDIRA_MIN = 0.95,
                 ASUMPT_MIN = 3000.*MeV,
                 **Xic0OmegacMassCuts),
            'Xic02PKKPi_Turbo' : 
            dict(#Trk_ALL_TRCHI2DOF_MAX = , # defined in Common.
                Trk_ALL_PT_MIN = 500.*MeV,
                #Trk_ALL_P_MIN = , # defined in Common.
                Trk_ALL_MIPCHI2DV_MIN = 4.0,
                ASUMPT_MIN = 3000.*MeV,
                Trk_1OF4_PT_MIN = 1000.*MeV, 
                Trk_2OF4_PT_MIN = 500.*MeV,
                Trk_1OF4_MIPCHI2DV_MIN = 8.0,
                Trk_2OF4_MIPCHI2DV_MIN = 6.0,
                #VCHI2PDOF_MAX = , # defined in Common.
                BPVDIRA_MIN = 0.99995, 
                BPVVDCHI2_MIN = 10.0,
                BPVLTIME_MIN = 0.1 * picosecond,
                **Xic0OmegacMassCuts),
            }
        return cuts

    def locallines(self) :
        from Stages import MassFilter, Xic02PKKPi_LTUNB, Xic02PKKPi
        
        stages = {
            'Xic02PKKPi_LTUNB_Turbo' : [MassFilter('Xic02PKKPi_LTUNB_Turbo',
                                                   inputs = [Xic02PKKPi_LTUNB('Xic02PKKPi_LTUNB_Turbo')])],
            'Xic02PKKPi_Turbo' : [MassFilter('Xic02PKKPi_Turbo',
                                             inputs = [Xic02PKKPi('Xic02PKKPi_Turbo')])],
            }
        return stages
