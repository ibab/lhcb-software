# $Id: StrippingBs2JpsiPhiPrescaledAndDetatched.py,v 1.1 2010-06-30 12:53:17 jpalac Exp $
__author__ = ['Marco Gersabeck']
__date__ = '21/02/2011'
__version__ = '$Revision: 1.1 $'

'''
This file contains stripping lines for time-dependent two-body charm analyses of mixing, yCP, and A_Gamma
'''
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsKaons, StdAllNoPIDsPions

default_config = { 
    'NAME'        : 'D2hh',
    'WGs'         : ['Charm'],
    'STREAMS'     : [ 'Charm' ],
    'BUILDERTYPE' : 'D2hhLines',
    'CONFIG'      : {
           'DaugPtMin': 800.,
           'DaugPtMax': 1500.,
           'DaugPtLoose': 500.,
           'DaugP': 5000.,
           'DaugPLoose': 3000.,
           'DaugIPChi2Loose': 4.,
           'DaugIPChi2': 9.,
           'DaugTrkChi2': 3.,
           'DaugTrkChi2Loose': 4.,
           'HighPIDK': 5.,
           'LowPIDK': 0.,
           'D0Pt': 2000.,
           'D0PtLoose': 1500.,
           'D0MassWindowCentre': 1865.,
           'D0MassWindowWidth': 100.,
           'D0KPiMassWindowWidthLow':  -100.,
           'D0KPiMassWindowWidthHigh': 200.,
           'D0PiPiMassWindowWidthLow':  -75.,
           'D0PiPiMassWindowWidthHigh': 200.,
           'D0KKMassWindowWidthLow': -100.,
           'D0KKMassWindowWidthHigh': 200.,
           'D0P': 5000.,
           'D0VtxChi2Ndof': 10.,
           'D0Tau': 0.,
           'D0FDChi2': 40.,
           'D0BPVDira': 0.9999,
           'D0DOCA': 0.07,
           'Daug_TRCHI2DOF_MAX': 5.,
           'Dstar_AMDiff_MAX': 165.,
           'Dstar_VCHI2VDOF_MAX': 100.,
           'Dstar_MDiff_MAX': 160.,
           'UntaggedCFLinePrescale': 1.,
           'UntaggedCFLinePostscale': 1.,
           'UntaggedSCSLinePrescale': 1.,
           'UntaggedSCSLinePostscale': 1.,
           'TaggedRSLinePrescale': 1.,
           'TaggedRSLinePostscale': 1.,
           'TaggedWSLinePrescale': 1.,
           'TaggedWSLinePostscale': 1.,
           'TaggedSCSLinePrescale': 1.,
           'TaggedSCSLinePostscale': 1.,
           'TaggedRSSSLinePrescale': 1.,
           'TaggedRSSSLinePostscale': 1.,
           'TaggedSCSSSLinePrescale': 1.,
           'TaggedSCSSSLinePostscale': 1.,
           'UntaggedTISLinePrescale': 1.,
           'UntaggedTISLinePostscale': 1.,
           'UntaggedKpiOnly': False,
           'RunSameSign': True,
           'RunTISLines': True,
           'RunDefault': True,
           'UseTOSFilter': False,
           'AddPartialD': False,
           'Hlt1TOS': { 'Hlt1TrackAllL0Decision%TOS' : 0 },
           'Hlt2TOSKPi': { 'Hlt2CharmHadD02HH_D02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02KPiWideMassDecision%TOS' : 0 },
           'Hlt2TOSKK': { 'Hlt2CharmHadD02HH_D02KKDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KKWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KKDecision%TOS' : 0, 'Hlt2CharmHadD02KKWideMassDecision%TOS' : 0 },
           'Hlt2TOSPiPi': { 'Hlt2CharmHadD02HH_D02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02PiPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiWideMassDecision%TOS' : 0 }
         }
}

class D2hhConf(LineBuilder) :

    '''
    Produces 7 lines, 3 untagged and 4 tagged D->hh lines:
    untagged:
    - D->Kpi
    - D->KK
    - D->pipi
    tagged, D*->D0pi with:
    - D->Kpi RS
    - D->Kpi WS
    - D->KK
    - D->pipi

    Usage:
    from StrippingSelections import StrippingD2hh
    confD2hh = StrippingD2hh.D2hhConf("D2hh",StrippingD2hh.default_config)
    stream.appendLines( confD2hh.lines() )
    '''

    __configuration_keys__ = ('DaugPtMin',
                              'DaugPtMax',
                              'DaugPtLoose',
                              'DaugP',
                              'DaugPLoose',
                              'DaugIPChi2',
                              'DaugIPChi2Loose',
                              'DaugTrkChi2',
                              'DaugTrkChi2Loose',
                              'HighPIDK',
                              'LowPIDK',
                              'D0Pt',
                              'D0PtLoose',
                              'D0MassWindowCentre',
                              'D0MassWindowWidth',
                              'D0KPiMassWindowWidthLow',
                              'D0KPiMassWindowWidthHigh',
                              'D0PiPiMassWindowWidthLow',
                              'D0PiPiMassWindowWidthHigh',
                              'D0KKMassWindowWidthLow',
                              'D0KKMassWindowWidthHigh',
                              'D0P',
                              'D0VtxChi2Ndof',
                              'D0Tau',
                              'D0FDChi2',
                              'D0BPVDira',
                              'D0DOCA',
                              'Daug_TRCHI2DOF_MAX',
                              'Dstar_AMDiff_MAX',
                              'Dstar_VCHI2VDOF_MAX',
                              'Dstar_MDiff_MAX',
                              'UntaggedCFLinePrescale',
                              'UntaggedCFLinePostscale',
                              'UntaggedSCSLinePrescale',
                              'UntaggedSCSLinePostscale',
                              'TaggedRSLinePrescale',
                              'TaggedRSLinePostscale',
                              'TaggedWSLinePrescale',
                              'TaggedWSLinePostscale',
                              'TaggedSCSLinePrescale',
                              'TaggedSCSLinePostscale',
                              'TaggedRSSSLinePrescale',
                              'TaggedRSSSLinePostscale',
                              'TaggedSCSSSLinePrescale',
                              'TaggedSCSSSLinePostscale',
                              'UntaggedTISLinePrescale',
                              'UntaggedTISLinePostscale',
                              'UntaggedKpiOnly',
                              'RunSameSign',
                              'RunTISLines',
                              'RunDefault',
                              'UseTOSFilter',
                              'AddPartialD',
                              'Hlt1TOS',
                              'Hlt2TOSKPi',
                              'Hlt2TOSKK',
                              'Hlt2TOSPiPi'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        stdNoPIDsKaons = StdAllNoPIDsKaons
        stdNoPIDsPions = StdAllNoPIDsPions

        d2kpi_name = name+'PromptD2KPi'
        d0RS_name =  name+'PromptD0RS'
        d0WS_name = name+'PromptD0WS'
        d2kk_name = name+'PromptD2KK'
        d2pipi_name = name+'PromptD2PiPi'
        dst2DPiPi_name = name+'PromptDst2D2PiPi'
        dst2DKK_name = name+'PromptDst2D2KK'
        dst2DRS_name = name+'PromptDst2D2RS'
        dst2DWS_name = name+'PromptDst2D2WS'
        dPartial_name = name+'PromptDPartial'
        dstarPartial_name = name+'PromptDstarPartial'
        pseudoPsi_name = name+'PromptD0D0FromDstarPartial'

        # D0 -> hh' selections

        self.selD2Kpi = makeD2hhAsymm(d2kpi_name,  
                                 config,
                                 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
                                 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
                                 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
                                 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
                                 CombPIDK_string = '',
                                 DecayDescriptor = '[D0 -> K- pi+]cc',
                                 inputSel = [stdNoPIDsPions, stdNoPIDsKaons],
                                 useTOS = config['UseTOSFilter'],
                                 Hlt1TOS = config['Hlt1TOS'],
                                 Hlt2TOS = config['Hlt2TOSKPi']
                                )

        self.selD0WS = makeD2hhAsymm(d0WS_name,  
                                 config,
                                 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
                                 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
                                 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
                                 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
                                 CombPIDK_string = '',
                                 DecayDescriptor = '[D0 -> K+ pi-]cc',
                                 inputSel = [stdNoPIDsPions, stdNoPIDsKaons],
                                 useTOS = config['UseTOSFilter'],
                                 Hlt1TOS = config['Hlt1TOS'],
                                 Hlt2TOS = config['Hlt2TOSKPi']
                                )

        self.selD0KKsgl = makeD2hhAsymm(d2kk_name+'_single',  
                                config,
                                KPIDK_string = ' & (PIDK > %(LowPIDK)s)',
                                PiPIDK_string = '',
                                Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KKMassWindowWidthLow)s* MeV)',
                                Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KKMassWindowWidthHigh)s* MeV)',
                                CombPIDK_string = ' & (AHASCHILD( PIDK > %(HighPIDK)s ) )',
                                DecayDescriptor = 'D0 -> K+ K-',
                                inputSel = [stdNoPIDsKaons],
                                useTOS = config['UseTOSFilter'],
                                Hlt1TOS = config['Hlt1TOS'],
                                Hlt2TOS = config['Hlt2TOSKK']
                               )

        self.selD0PiPisgl = makeD2hhAsymm(d2pipi_name+'_single',  
                                  config,
                                  KPIDK_string = '',
                                  PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
                                  Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0PiPiMassWindowWidthLow)s* MeV)',
                                  Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0PiPiMassWindowWidthHigh)s* MeV)',
                                  CombPIDK_string = '',
                                  DecayDescriptor = 'D0 -> pi+ pi-',
                                  inputSel = [stdNoPIDsPions], 
                                  useTOS = config['UseTOSFilter'],
                                  Hlt1TOS = config['Hlt1TOS'],
                                  Hlt2TOS = config['Hlt2TOSPiPi']
                                 )

        self.selD0KK = makeD2hhAsymm(d2kk_name,  
                                config,
                                KPIDK_string = ' & (PIDK > %(LowPIDK)s)',
                                PiPIDK_string = '',
                                Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KKMassWindowWidthLow)s* MeV)',
                                Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KKMassWindowWidthHigh)s* MeV)',
                                CombPIDK_string = ' & (AHASCHILD( PIDK > %(HighPIDK)s ) )',
                                DecayDescriptor = '[D0 -> K+ K-]cc',
                                inputSel = [stdNoPIDsKaons],
                                useTOS = config['UseTOSFilter'],
                                Hlt1TOS = config['Hlt1TOS'],
                                Hlt2TOS = config['Hlt2TOSKK']
                               )

        self.selD0PiPi = makeD2hhAsymm(d2pipi_name,  
                                  config,
                                  KPIDK_string = '',
                                  PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
                                  Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0PiPiMassWindowWidthLow)s* MeV)',
                                  Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0PiPiMassWindowWidthHigh)s* MeV)',
                                  CombPIDK_string = '',
                                  DecayDescriptor = '[D0 -> pi+ pi-]cc',
                                  inputSel = [stdNoPIDsPions], 
                                  useTOS = config['UseTOSFilter'],
                                  Hlt1TOS = config['Hlt1TOS'],
                                  Hlt2TOS = config['Hlt2TOSPiPi']
                                 )

        self.selD2KpiSS = makeD2hhAsymm(d2kpi_name+'SS',  
                                 config,
                                 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
                                 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
                                 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
                                 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
                                 CombPIDK_string = '',
                                 DecayDescriptor = '[D0 -> K- pi-]cc',
                                 inputSel = [stdNoPIDsPions, stdNoPIDsKaons],
                                 useTOS = False,
                                 Hlt1TOS = config['Hlt1TOS'],
                                 Hlt2TOS = config['Hlt2TOSKPi']
                                )

        self.selD0KKSS = makeD2hhAsymm(d2kk_name+'SS',  
                                config,
                                KPIDK_string = ' & (PIDK > %(LowPIDK)s)',
                                PiPIDK_string = '',
                                Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KKMassWindowWidthLow)s* MeV)',
                                Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KKMassWindowWidthHigh)s* MeV)',
                                CombPIDK_string = ' & (AHASCHILD( PIDK > %(HighPIDK)s ) )',
                                DecayDescriptor = '[D0 -> K- K-]cc',
                                inputSel = [stdNoPIDsKaons],
                                useTOS = False,
                                Hlt1TOS = config['Hlt1TOS'],
                                Hlt2TOS = config['Hlt2TOSKK']
                               )

        self.selD0PiPiSS = makeD2hhAsymm(d2pipi_name+'SS',  
                                  config,
                                  KPIDK_string = '',
                                  PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
                                  Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0PiPiMassWindowWidthLow)s* MeV)',
                                  Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0PiPiMassWindowWidthHigh)s* MeV)',
                                  CombPIDK_string = '',
                                  DecayDescriptor = '[D0 -> pi- pi-]cc',
                                  inputSel = [stdNoPIDsPions], 
                                  useTOS = False,
                                  Hlt1TOS = config['Hlt1TOS'],
                                  Hlt2TOS = config['Hlt2TOSPiPi']
                                 )

        self.selD2KpiTIS = makeD2hhAsymm(d2kpi_name+'TIS',  
                                 config,
                                 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
                                 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
                                 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
                                 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
                                 CombPIDK_string = '',
                                 DecayDescriptor = '[D0 -> K- pi+]cc',
                                 inputSel = [stdNoPIDsPions, stdNoPIDsKaons],
                                 useTOS = True,
                                 Hlt1TOS = { 'Hlt1Global%TIS' : 0 },
                                 Hlt2TOS = { 'Hlt2Global%TIS' : 0 }
                                )

        self.selD0KKTIS = makeD2hhAsymm(d2kk_name+'TIS',  
                                config,
                                KPIDK_string = ' & (PIDK > %(LowPIDK)s)',
                                PiPIDK_string = '',
                                Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KKMassWindowWidthLow)s* MeV)',
                                Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KKMassWindowWidthHigh)s* MeV)',
                                CombPIDK_string = ' & (AHASCHILD( PIDK > %(HighPIDK)s ) )',
                                DecayDescriptor = 'D0 -> K+ K-',
                                inputSel = [stdNoPIDsKaons],
                                useTOS = True,
                                Hlt1TOS = { 'Hlt1Global%TIS' : 0 },
                                Hlt2TOS = { 'Hlt2Global%TIS' : 0 }
                               )

        self.selD0PiPiTIS = makeD2hhAsymm(d2pipi_name+'TIS',  
                                  config,
                                  KPIDK_string = '',
                                  PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
                                  Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0PiPiMassWindowWidthLow)s* MeV)',
                                  Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0PiPiMassWindowWidthHigh)s* MeV)',
                                  CombPIDK_string = '',
                                  DecayDescriptor = 'D0 -> pi+ pi-',
                                  inputSel = [stdNoPIDsPions], 
                                  useTOS = True,
                                  Hlt1TOS = { 'Hlt1Global%TIS' : 0 },
                                  Hlt2TOS = { 'Hlt2Global%TIS' : 0 }
                                 )

        # Dstar -> D0 pi selections
        self.selDstRS = makeDstar2D0Pi( dst2DRS_name
                                   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2Kpi, stdNoPIDsPions]
                                 )

        self.selDstWS = makeDstar2D0Pi( dst2DWS_name
                                   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD0WS, stdNoPIDsPions]
                                 )

        self.selDstKK = makeDstar2D0Pi( dst2DKK_name
                                   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD0KK, stdNoPIDsPions]
                                 )

        self.selDstPiPi = makeDstar2D0Pi( dst2DPiPi_name
                                   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD0PiPi, stdNoPIDsPions]
                                 )

        self.selDstRSSS = makeDstar2D0Pi( dst2DRS_name+'SS'
                                   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2KpiSS, stdNoPIDsPions]
                                 )

        self.selDstKKSS = makeDstar2D0Pi( dst2DKK_name+'SS'
                                   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD0KKSS, stdNoPIDsPions]
                                 )

        self.selDstPiPiSS = makeDstar2D0Pi( dst2DPiPi_name+'SS'
                                   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD0PiPiSS, stdNoPIDsPions]
                                 )

        self.selDPartial = makeDPartial( dPartial_name
                                   , config
                                   , '[D- -> K- pi- pi+]cc'
                                   , inputSel = [stdNoPIDsPions, stdNoPIDsKaons]
                                 )

        self.selDstarPartial = makeDstarPartial( dstarPartial_name
                                   , config
                                   , '[D*(2010)+ -> D- pi+]cc'
                                   , inputSel = [self.selDPartial, stdNoPIDsPions]
                                 )

        self.selPseudoPsi = makePseudoPsi( pseudoPsi_name
                                   , config
                                   , '[psi(3770) -> D0 D*(2010)+]cc'
                                   , inputSel = [self.selD2Kpi, self.selDstarPartial]
                                 )
        # Untagged lines
        self.d2kpi_line = StrippingLine(d2kpi_name+"Line",
                                        prescale = config['UntaggedCFLinePrescale'],
                                        postscale = config['UntaggedCFLinePostscale'],
                                        selection = self.selD2Kpi
                                       )

        self.d2kk_line = StrippingLine(d2kk_name+"Line",
                                        prescale = config['UntaggedSCSLinePrescale'],
                                        postscale = config['UntaggedSCSLinePostscale'],
                                        selection = self.selD0KKsgl
                                       )

        self.d2pipi_line = StrippingLine(d2pipi_name+"Line",
                                        prescale = config['UntaggedSCSLinePrescale'],
                                        postscale = config['UntaggedSCSLinePostscale'],
                                        selection = self.selD0PiPisgl
                                       )

        # Untagged TIS lines
        self.d2kpiTIS_line = StrippingLine(d2kpi_name+"TISLine",
                                        prescale = config['UntaggedTISLinePrescale'],
                                        postscale = config['UntaggedTISLinePostscale'],
                                        selection = self.selD2KpiTIS
                                       )

        self.d2kkTIS_line = StrippingLine(d2kk_name+"TISLine",
                                        prescale = config['UntaggedTISLinePrescale'],
                                        postscale = config['UntaggedTISLinePostscale'],
                                        selection = self.selD0KKTIS
                                       )

        self.d2pipiTIS_line = StrippingLine(d2pipi_name+"TISLine",
                                        prescale = config['UntaggedTISLinePrescale'],
                                        postscale = config['UntaggedTISLinePostscale'],
                                        selection = self.selD0PiPiTIS
                                       )

        # Tagged lines
        self.dstRS_line = StrippingLine(dst2DRS_name+"Line",
                                        prescale = config['TaggedRSLinePrescale'],
                                        postscale = config['TaggedRSLinePostscale'],
                                        selection = self.selDstRS
                                       )

        self.dstWS_line = StrippingLine(dst2DWS_name+"Line",
                                        prescale = config['TaggedWSLinePrescale'],
                                        postscale = config['TaggedWSLinePostscale'],
                                        selection = self.selDstWS
                                       )

        self.dstKK_line = StrippingLine(dst2DKK_name+"Line",
                                        prescale = config['TaggedSCSLinePrescale'],
                                        postscale = config['TaggedSCSLinePostscale'],
                                        selection = self.selDstKK
                                       )

        self.dstPiPi_line = StrippingLine(dst2DPiPi_name+"Line",
                                        prescale = config['TaggedSCSLinePrescale'],
                                        postscale = config['TaggedSCSLinePostscale'],
                                        selection = self.selDstPiPi
                                       )

        # Tagged same sign lines
        self.dstRSSS_line = StrippingLine(dst2DRS_name+"SSLine",
                                        prescale = config['TaggedRSLinePrescale'],
                                        postscale = config['TaggedRSLinePostscale'],
                                        selection = self.selDstRSSS
                                       )

        self.dstKKSS_line = StrippingLine(dst2DKK_name+"SSLine",
                                        prescale = config['TaggedSCSLinePrescale'],
                                        postscale = config['TaggedSCSLinePostscale'],
                                        selection = self.selDstKKSS
                                       )

        self.dstPiPiSS_line = StrippingLine(dst2DPiPi_name+"SSLine",
                                        prescale = config['TaggedSCSLinePrescale'],
                                        postscale = config['TaggedSCSLinePostscale'],
                                        selection = self.selDstPiPiSS
                                       )

        # Pseudo Psi line
        self.pseudoPsi_line = StrippingLine(pseudoPsi_name+"Line",
                                        prescale = 1.,
                                        postscale = 1.,
                                        selection = self.selPseudoPsi
                                       )

        # register lines
        if config['RunSameSign']:
          self.registerLine(self.dstRSSS_line)
          self.registerLine(self.dstKKSS_line)
          self.registerLine(self.dstPiPiSS_line)

        if config['RunTISLines']:
          self.registerLine(self.d2kpiTIS_line)
          self.registerLine(self.d2kkTIS_line)
          self.registerLine(self.d2pipiTIS_line)

        if config['RunDefault']:
          self.registerLine(self.d2kpi_line)
          if not config['UntaggedKpiOnly']:
            self.registerLine(self.d2kk_line)
            self.registerLine(self.d2pipi_line)

            self.registerLine(self.dstRS_line)
            self.registerLine(self.dstWS_line)
            self.registerLine(self.dstKK_line)
            self.registerLine(self.dstPiPi_line)
            if config['AddPartialD']:
              self.registerLine(self.pseudoPsi_line)

def makeD2hhAsymm(name,
             config,
             KPIDK_string,
             PiPIDK_string,
             Mass_low_string,
             Mass_high_string,
             CombPIDK_string,
             DecayDescriptor,
             inputSel,
             useTOS,
             Hlt1TOS,
             Hlt2TOS
            ) :
    """
    Create and return a D0 -> hh' Selection object.
    Arguments:
    name        : name of the Selection.
    config      : dictionary of cut values.
    ..._string  : cut implementation for PIDK cuts.
    DecayDescriptor: DecayDescriptor.
    inputSel    : input selections
    """

    def makeTISTOS( name, _input, _hlttos ) :
            from Configurables import TisTosParticleTagger
            _tisTosFilter = TisTosParticleTagger( name + "Tagger" )
            _tisTosFilter.TisTosSpecs = _hlttos
            return Selection( name
                              , Algorithm = _tisTosFilter
                              , RequiredSelections = [ _input ]
                              )        

    _Kcuts1  = "~ISMUON & (PT > %(DaugPtMin)s* MeV) & (MIPCHI2DV(PRIMARY) > %(DaugIPChi2)s)" % locals()['config']
    _KcutsPIDK  = KPIDK_string % locals()['config']
    _Kcuts2  = " & (ISLONG) & (P > %(DaugP)s* MeV) & (TRCHI2DOF < %(DaugTrkChi2)s)" % locals()['config']
    _Kcuts = _Kcuts1 + _KcutsPIDK + _Kcuts2
    _Picuts1 = "~ISMUON & (PT > %(DaugPtMin)s* MeV) & (MIPCHI2DV(PRIMARY) > %(DaugIPChi2)s)" % locals()['config']
    _PicutsPIDK  = PiPIDK_string % locals()['config']
    _Picuts2 = " & (ISLONG) & (P > %(DaugP)s* MeV) & (TRCHI2DOF < %(DaugTrkChi2)s)" % locals()['config']
    _Picuts = _Picuts1 + _PicutsPIDK + _Picuts2
    _dauCuts = { 'K+': _Kcuts, 'pi+': _Picuts }

    _massLow  = Mass_low_string % locals()['config']
    _massHigh  = Mass_high_string % locals()['config']
    _combCuts1 = "(APT > %(D0Pt)s* MeV)" \
                "& (AHASCHILD( PT > %(DaugPtMax)s* MeV ) )" \
                "& (ADOCA(1,2)< %(D0DOCA)s* mm)" \
                "& (AP > %(D0P)s* MeV)" % locals()['config']
    _combCutsPIDK = CombPIDK_string % locals()['config']
    _combCuts = _combCuts1 + _combCutsPIDK + _massLow + _massHigh

    _motherCuts = "(VFASPF(VCHI2PDOF) < %(D0VtxChi2Ndof)s)" \
                  "& (BPVVDCHI2 > %(D0FDChi2)s)" \
                  "& (BPVLTIME() > %(D0Tau)s)" \
                  "& (BPVDIRA > %(D0BPVDira)s)" % locals()['config']

    _D0 = CombineParticles( DecayDescriptor = DecayDescriptor,
                            MotherCut = _motherCuts,
                            CombinationCut = _combCuts,
                            DaughtersCuts = _dauCuts)

    _sel = Selection ( name+'Sel',
                       Algorithm = _D0,
                       RequiredSelections = inputSel )

    if not useTOS:
      return _sel

    _selD2hhHlt1TOS = makeTISTOS( name + "D2hhHlt1TOS"
                                    , _sel
                                    , Hlt1TOS
                                            )
    _selD2hhHlt2TOS = makeTISTOS( name + "D2hhHlt2TOS"
                                            , _selD2hhHlt1TOS
                                            , Hlt2TOS
                                            )
       
    return _selD2hhHlt2TOS

def makeDstar2D0Pi( name
                    , config
                    , DecayDescriptor
                    , inputSel
                  ) :

    """
    Create and return a D* -> D0 pi Selection object.
    Arguments:
    name        : name of the Selection.
    config      : dictionary of cut values.
    DecayDescriptor: DecayDescriptor.
    inputSel    : input selections
    """

    daugCuts = "(TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" % locals()['config']
    combCuts = "((AM - AM1) < %(Dstar_AMDiff_MAX)s* MeV)" % locals()['config']
    dstarCuts = "(VFASPF(VCHI2/VDOF) < %(Dstar_VCHI2VDOF_MAX)s)" \
                "& ((M - M1) < %(Dstar_MDiff_MAX)s* MeV)" % locals()['config']

    _Dstar = CombineParticles( DecayDescriptor = DecayDescriptor
                             , DaughtersCuts = { "pi+" : daugCuts }
                             , CombinationCut = combCuts
                             , MotherCut = dstarCuts
                             )

    return Selection( name+'Sel',
                      Algorithm = _Dstar,
                      RequiredSelections = inputSel
                    )

def makeDPartial( name
                    , config
                    , DecayDescriptor
                    , inputSel
                  ) :

    """
    Create and return a D- -> K- pi- pi+ Selection object.
    Arguments:
    name        : name of the Selection.
    config      : dictionary of cut values.
    DecayDescriptor: DecayDescriptor.
    inputSel    : input selections
    """

    _Kcuts1  = "~ISMUON & (PT > %(DaugPtLoose)s* MeV) & (MIPCHI2DV(PRIMARY) > %(DaugIPChi2Loose)s)" % locals()['config']
    _KcutsPIDK  = " & (PIDK > %(HighPIDK)s)" % locals()['config']
    _Kcuts2  = " & (ISLONG) & (P > %(DaugPLoose)s* MeV) & (TRCHI2DOF < %(DaugTrkChi2Loose)s)" % locals()['config']
    _Kcuts = _Kcuts1 + _KcutsPIDK + _Kcuts2
    _Picuts1 = "~ISMUON & (PT > %(DaugPtMin)s* MeV) & (MIPCHI2DV(PRIMARY) > %(DaugIPChi2)s)" % locals()['config']
    _PicutsPIDK  = " & (PIDK < %(LowPIDK)s)" % locals()['config']
    _Picuts2 = " & (ISLONG) & (P > %(DaugP)s* MeV) & (TRCHI2DOF < %(DaugTrkChi2)s)" % locals()['config']
    _Picuts = _Picuts1 + _PicutsPIDK + _Picuts2
    _dauCuts = { 'K+': _Kcuts, 'pi+': _Picuts }
    #_Kcuts1  = "~ISMUON & (PT > 500* MeV) & (MIPCHI2DV(PRIMARY) > 4)"
    #_KcutsPIDK  = " & (PIDK > 5)"
    #_Kcuts2  = " & (ISLONG) & (P > 5000* MeV) & (TRCHI2DOF < 5)"
    #_Kcuts = _Kcuts1 + _KcutsPIDK + _Kcuts2
    #_Picuts1 = "~ISMUON & (PT > 500* MeV) & (MIPCHI2DV(PRIMARY) > 4)"
    #_PicutsPIDK  = " & (PIDK < 0)"
    #_Picuts2 = " & (ISLONG) & (P > 5000* MeV) & (TRCHI2DOF < 5)"
    #_Picuts = _Picuts1 + _PicutsPIDK + _Picuts2
    #_dauCuts = { 'K+': _Kcuts, 'pi+': _Picuts }

    _combCuts = "(APT > %(D0PtLoose)s* MeV)" \
                "& (AP > %(D0P)s* MeV)" % locals()['config']

    _motherCuts = "(VFASPF(VCHI2PDOF) < %(D0VtxChi2Ndof)s)" \
                  "& (BPVVDCHI2 > %(D0FDChi2)s)" % locals()['config']


    _Dminus = CombineParticles( DecayDescriptor = DecayDescriptor
                             , DaughtersCuts = _dauCuts
                             , CombinationCut = _combCuts
                             , MotherCut = _motherCuts
                             )

    return Selection( name+'Sel',
                      Algorithm = _Dminus,
                      RequiredSelections = inputSel
                    )

def makeDstarPartial( name
                    , config
                    , DecayDescriptor
                    , inputSel
                  ) :

    """
    Create and return a D*+ -> D- pi+ Selection object.
    Arguments:
    name        : name of the Selection.
    config      : dictionary of cut values.
    DecayDescriptor: DecayDescriptor.
    inputSel    : input selections
    """

    daugCuts = "(TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" % locals()['config']
    combCuts = "((AM - AM1) < %(Dstar_AMDiff_MAX)s* MeV)" % locals()['config']
    dstarCuts = "(VFASPF(VCHI2/VDOF) < %(Dstar_VCHI2VDOF_MAX)s)" \
                "& ((M - M1) < %(Dstar_MDiff_MAX)s* MeV)" % locals()['config']

    _Dstar = CombineParticles( DecayDescriptor = DecayDescriptor
                             , DaughtersCuts = { "pi+" : daugCuts }
                             , CombinationCut = combCuts
                             , MotherCut = dstarCuts
                             )

    return Selection( name+'Sel',
                      Algorithm = _Dstar,
                      RequiredSelections = inputSel
                    )

def makePseudoPsi( name
                    , config
                    , DecayDescriptor
                    , inputSel
                  ) :

    """
    Create and return a D* -> D0 pi Selection object.
    Arguments:
    name        : name of the Selection.
    config      : dictionary of cut values.
    DecayDescriptor: DecayDescriptor.
    inputSel    : input selections
    """

    _daugCuts = "(PT> %(D0PtLoose)s*MeV)" % locals()['config']
    _combCuts = "(APT> %(D0PtLoose)s*MeV)" % locals()['config']

    _Psi = CombineParticles( DecayDescriptor = DecayDescriptor
                             , DaughtersCuts = { "D0": _daugCuts }
                             , CombinationCut = _combCuts
                             , MotherCut = "(VFASPF(VCHI2PDOF) < 10000)"
                             )

    return Selection( name+'Sel',
                      Algorithm = _Psi,
                      RequiredSelections = inputSel
                    )

