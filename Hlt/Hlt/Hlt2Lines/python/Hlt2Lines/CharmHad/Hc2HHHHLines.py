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
            'Xic0ToPpKmKmPip_LTUNBTurbo' : 
            dict(TisTosSpec = "Hlt1Track.*Decision%TIS",
                 BPVLTIME_MIN = 0.1*picosecond, 
                 BPVDIRA_MIN = 0.95,
                 ASUMPT_MIN = 3000.*MeV,
                 **Xic0OmegacMassCuts),
            'Xic0ToPpKmKmPipTurbo' : 
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
        'PentaPhiPimPp' : {
                 'Trk_ALL_PT_MIN'           :  250 * MeV,
                 'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                 'AM12_MAX'                 : 1050.0 * MeV,
                 'AM_4'                     : (139.5) * MeV,
                 'AM_MIN'                   :  2700 * MeV,
                 'AM_MAX'                   :  2930 * MeV,
                 'ASUMPT_MIN'               :  1980.0 * MeV,
                 'ACHI2DOCA_MAX'            :  10.0,
                 'VCHI2PDOF_MAX'            :  12.0,
                 'BPVDIRA_MIN'              :  0.9998,
                 'BPVLTIME_MIN'             :  0.3*picosecond,
                 'PT_MIN'                   :  2000 * MeV,
                 'IPCHI2_MAX'               :  15.0,
                 'Mass_M_MIN'               :  2720 * MeV,
                 'Mass_M_MAX'               :  2915 * MeV,
                }
            }
        return cuts

    def locallines(self) :
        from Stages import MassFilter, Xic02PKKPi_LTUNB, Xic02PKKPi
        from Stages import PentaPhiPimPp
       
        PentaPhiPimPpFilt = MassFilter('Filt' , inputs = [ PentaPhiPimPp ]
                                              , nickname = 'PentaPhiPimPp')
 
        stages = {
            'Xic0ToPpKmKmPip_LTUNBTurbo' : [MassFilter('Xic0ToPpKmKmPip_LTUNBTurbo',
                                                       inputs = [Xic02PKKPi_LTUNB('Xic0ToPpKmKmPip_LTUNBTurbo')],
                                                       reFitPVs = True)],
            'Xic0ToPpKmKmPipTurbo' : [MassFilter('Xic0ToPpKmKmPipTurbo',
                                                 inputs = [Xic02PKKPi],
                                                 reFitPVs = True)],
            'PentaToPhiPpPimTurbo' : [PentaPhiPimPpFilt],
            }
        return stages
