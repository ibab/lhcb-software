from GaudiKernel.SystemOfUnits import MeV, picosecond, mm

class Jets_25ns_Draft2016:

    __all__ = ( 'ActiveHlt2Lines' )

    def ActiveHlt2Lines(self) :
        """Returns a list of active lines."""

        lines = ['Hlt2DiJetSV',
                 'Hlt2DiJetMu',
                 'Hlt2DiJetSVSV',
                 'Hlt2DiJetSVMu',
                 'Hlt2DiJetMuMu']

        return lines
    
    def Thresholds(self) :
        """Return the trigger thresholds."""

        d = {}

        from Hlt2Lines.Jets.Lines import JetsLines
        d.update( 
            { JetsLines : {
                'Common' : {
                    'GHOSTPROB' : 0.2,
                    'DPHI'      : 1.5,
                    'SV_VCHI2'  : 10,
                    'SV_TRK_PT' : 500*MeV,
                    'SV_TRK_IPCHI2' : 16,
                    'SV_FDCHI2' : 25,
                    'MU_PT'     : 1000*MeV,
                    'MU_PROBNNMU'  : 0.5}
            }
          }
        )

        return d
