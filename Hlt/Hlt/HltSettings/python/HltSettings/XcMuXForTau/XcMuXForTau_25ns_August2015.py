from GaudiKernel.SystemOfUnits import GeV, mm, MeV, picosecond

class XcMuXForTau_25ns_August2015 :
    """
    Threshold settings for Hlt2 XcMuXForTau lines: XcMuXForTau_25ns_August2015
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    EDITED BY G.Ciezarek on 2015-08-30 to run with updated Hlt2Line 
    @author P. Owen < patrickowen22@gmail.com > 
    @date 2015-08-30
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        """
        lines = [
            'B2D0KPiMu',
            'B2D0K3PiMu',
            'B2DplusKPiPiMu',
            'B2DsKKPiMu',
            'B2LcPKPiMu',
            'B2D0KPiFakeMu',
            'B2D0K3PiFakeMu',
            'B2DplusKPiPiFakeMu',
            'B2DsKKPiFakeMu',
            'B2LcPKPiFakeMu'
            ]
        """
        lines = [
            'B2XcMu',
            'B2XcFakeMu'
            ]
        return ['Hlt2XcMuXForTau' + l for l in lines]


    def Thresholds(self) :
        
        d = {}
        
        from Hlt2Lines.XcMuXForTau.Lines import XcMuXForTauLines

        d.update (
            {
                XcMuXForTauLines: { 
                 'Prescale' : {},
                 'Prescale' : {'Hlt2XcMuXForTauB2XcFakeMu' :   0.1},
                 'XcDaughter'  : {
                             'DTrk_ALL_PT_MIN'           : 200.0 * MeV
                            , 'DTrk_ALL_P_MIN'            : 5.0  * GeV
                            , 'DTrk_ALL_MIPCHI2DV_MIN'    : 9.0
                            , 'K_PIDK'    : 2 
                            , 'Pi_PIDK'    : 4 
                            , 'P_PIDp'    : 0 
                 
                            },
                 'Xc' : {
                              'Pair_AMINDOCA_MAX'        : 0.10 * mm
                            , 'MIN_D_CHILDPT'          : 800.0 * MeV
                            , 'D0_BPVVDCHI2_MIN'         : 25.0       # neuter
                            , 'D0_BPVDIRA_MIN'           : 0.999    # neuter
                            , 'D0_VCHI2PDOF_MAX'         : 10.0       # neuter
                            , 'D0_PT_MIN'                : 2000.0 * MeV
                            , 'D_SUMPT'    : 2500 * MeV
                            , 'D_AM_MIN'              : 1830.0 * MeV
                            , 'D_AM_MAX'              : 1910.0 * MeV
                            , 'Ds_AM_MIN'              : 1920.0 * MeV
                            , 'Ds_AM_MAX'              : 2010.0 * MeV
                            , 'Lc_AM_MIN'              : 2230.0 * MeV
                            , 'Lc_AM_MAX'              : 2330.0 * MeV
                            },
                 'Muon' : {
                             'Mu_IPCHI2'    : 16
                            },
                 'B' : {
                
                             'XcMu_VTXCHI2'        : 15
                            , 'XcMu_DOCA_MAX'        : 0.50 * mm
                            , 'XcMu_DIRA'        : 0.999
                            , 'XcMu_FDCHI2'        : 50
                            }
		   
                 }
               }
            )
            
        return d


    
