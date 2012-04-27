# $Id: StrippingBs2JpsiPhiPrescaledAndDetatched.py,v 1.1 2010-06-30 12:53:17 jpalac Exp $
__author__ = ['Marco Gersabeck']
__date__ = '25/04/2012'
__version__ = '$Revision: 1.1 $'

'''
This file contains stripping lines for D*-tagged D0-->hhpi0 decays
'''
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsKaons, StdAllNoPIDsPions, StdTightKaons, StdTightPions, StdLooseResolvedPi0, StdLooseMergedPi0

default_config = { 'DaugPtMin': 800.,
           'DaugPtMax': 1250.,
           'DaugPtLoose': 500.,
           'DaugP': 5000.,
           'DaugPLoose': 3000.,
           'DaugIPChi2Loose': 9.,
           'DaugIPChi2': 25.,
           'DaugTrkChi2': 3.,
           'DaugTrkChi2Loose': 4.,
           'HighPIDK': 5.,
           'LowPIDK': 0.,
           'D0Pt': 1800.,
           'D0PtLoose': 1500.,
           'D0MassWindowCentre': 1865.,
           'D0MassWindowWidth': 100.,
           'D0KPiMassWindowWidthLow':  -100.,
           'D0KPiMassWindowWidthHigh': 100.,
           'D0PiPiMassWindowWidthLow':  -75.,
           'D0PiPiMassWindowWidthHigh': 200.,
           'D0KKMassWindowWidthLow': -100.,
           'D0KKMassWindowWidthHigh': 200.,
           'D0P': 5000.,
           'D0VtxChi2Ndof': 10.,
           'D0FDChi2': 40.,
           'D0BPVDira': 0.9999,
           'D0DOCA': 0.07,
           'KstPt': 1800.,
           'KstP': 5000.,
           'KstVtxChi2Ndof': 10.,
           'KstFDChi2': 40.,
           'KstDOCA': 0.07,
           'KstMassWindowCentre': 1865.,
           'KstKPiMassWindowWidthHigh': 50.,
           'Daug_TRCHI2DOF_MAX': 3.,
           'Dstar_AMDiff_MAX': 185.,
           'Dstar_VCHI2VDOF_MAX': 100.,
           'Dstar_MDiff_MAX': 180.,
           'Pi0MinPT_R':1000.,
           'Pi0MinPT_M':2000.,
           'ResPi0MinGamCL': 0.2,
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
	   'RunSameSign': True,
	   'RunDefault': True,
	   'UseTOSFilter': True,
	   'Hlt2TOS': { 'Hlt2CharmHadD02HHXDst_hhXDecision%TOS' : 0, 'Hlt2CharmHadD02HHXDst_hhXWideMassDecision%TOS' : 0, 'Hlt2Global%TIS' : 0 },

         }

class DstarD2hhpi0Conf(LineBuilder) :

    '''
    Produces 4 tagged D->hhpi0 lines:
    tagged, D*->D0pi with:
    - D->Kpipi0 RS
    - D->Kpipi0 WS
    - D->KKpi0
    - D->pipipi0

    Usage:
    from StrippingSelections import StrippingDstarD2hhpi0
    confDstarD2hhpi0 = StrippingDstarD2hhpi0.DstarD2hhpi0Conf("DstarD2hhpi0",StrippingDstarD2hhpi0.default_config)
    stream.appendLines( confDstarD2hhpi0.lines() )
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
	     		      'D0FDChi2',
	     		      'D0BPVDira',
	     		      'D0DOCA',
                              'Daug_TRCHI2DOF_MAX',
                              'Dstar_AMDiff_MAX',
                              'Dstar_VCHI2VDOF_MAX',
                              'Dstar_MDiff_MAX',
                              'KstPt',
                              'KstP',
                              'KstVtxChi2Ndof',
                              'KstFDChi2',
                              'KstDOCA',
                              'KstMassWindowCentre',
                              'KstKPiMassWindowWidthHigh',
                              'Pi0MinPT_R',
                              'Pi0MinPT_M',
                              'ResPi0MinGamCL',
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
			      'RunSameSign',
			      'RunDefault',
	                      'UseTOSFilter',
	                      'Hlt2TOS',
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        myKaons = StdTightKaons
        myPions = StdTightPions
        allPions = StdAllNoPIDsPions

        d2kpi_name = name+'PromptD2KPiPi0'
        d0RS_name =  name+'PromptD0RSPi0'
        d0WS_name = name+'PromptD0WSPi0'
        d2KK_name = name+'PromptD2KKPi0'
        d2pipi_name = name+'PromptD2PiPiPi0'
        dst2Dpipi_name = name+'PromptDst2D2PiPiPi0'
        dst2DKK_name = name+'PromptDst2D2KKPi0'
        dst2DRS_name = name+'PromptDst2D2RSPi0'
        dst2DWS_name = name+'PromptDst2D2WSPi0'

        # D0 -> hh' selections

        self.selD2WSR = makeD2hhAsymm(d0WS_name+'R',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'K+ pi-',
			         inputSel = [myPions, myKaons],
				 useResolved = True,
				 TOSFilter = 'TOS'
			        )
        self.selD2WSM = makeD2hhAsymm(d0WS_name+'M',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'K+ pi-',
			         inputSel = [myPions, myKaons],
				 useResolved = False,
				 TOSFilter = 'TOS'
			        )
        self.selD2KpiR = makeD2hhAsymm(d2kpi_name+'R',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'K- pi+',
			         inputSel = [myPions, myKaons],
				 useResolved = True,
				 TOSFilter = 'TOS'
			        )
        self.selD2KpiM = makeD2hhAsymm(d2kpi_name+'M',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'K- pi+',
			         inputSel = [myPions, myKaons],
				 useResolved = False,
				 TOSFilter = 'TOS'
			        )
        self.selD2KpiRSS = makeD2hhAsymm(d2kpi_name+'RSS',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'K- pi-',
			         inputSel = [myPions, myKaons],
				 useResolved = True,
				 TOSFilter = 'NONE'
			        )
        self.selD2KpiMSS = makeD2hhAsymm(d2kpi_name+'MSS',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'K- pi-',
			         inputSel = [myPions, myKaons],
				 useResolved = False,
				 TOSFilter = 'NONE'
			        )
        self.selD2KKR = makeD2hhAsymm(d2KK_name+'R',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'K- K+',
			         inputSel = [myKaons],
				 useResolved = True,
				 TOSFilter = 'TOS'
			        )
        self.selD2KKM = makeD2hhAsymm(d2KK_name+'M',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'K- K+',
			         inputSel = [myKaons],
				 useResolved = False,
				 TOSFilter = 'TOS'
			        )
        self.selD2KKRSS = makeD2hhAsymm(d2KK_name+'RSS',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'K- K-',
			         inputSel = [myKaons],
				 useResolved = True,
				 TOSFilter = 'NONE'
			        )
        self.selD2KKMSS = makeD2hhAsymm(d2KK_name+'MSS',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'K- K-',
			         inputSel = [myKaons],
				 useResolved = False,
				 TOSFilter = 'NONE'
			        )
        self.selD2pipiR = makeD2hhAsymm(d2pipi_name+'R',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'pi- pi+',
			         inputSel = [myPions],
				 useResolved = True,
				 TOSFilter = 'TOS'
			        )
        self.selD2pipiM = makeD2hhAsymm(d2pipi_name+'M',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'pi- pi+',
			         inputSel = [myPions],
				 useResolved = False,
				 TOSFilter = 'TOS'
			        )
        self.selD2pipiRSS = makeD2hhAsymm(d2pipi_name+'RSS',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'pi- pi-',
			         inputSel = [myPions],
				 useResolved = True,
				 TOSFilter = 'NONE'
			        )
        self.selD2pipiMSS = makeD2hhAsymm(d2pipi_name+'MSS',  
			         config,
 				 KPIDK_string = ' & (PIDK > %(HighPIDK)s)',
				 PiPIDK_string = ' & (PIDK < %(LowPIDK)s)',
				 Mass_low_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) > %(D0KPiMassWindowWidthLow)s* MeV)',
				 Mass_high_string = '& (DAMASS(%(D0MassWindowCentre)s* MeV) < %(D0KPiMassWindowWidthHigh)s* MeV)',
				 Kst_Mass_high_string = '& (DAMASS(%(KstMassWindowCentre)s* MeV) < %(KstKPiMassWindowWidthHigh)s* MeV)',
				 CombPIDK_string = '',
				 DecayDescriptor = 'pi- pi-',
			         inputSel = [myPions],
				 useResolved = False,
				 TOSFilter = 'NONE'
			        )
        # Dstar -> D0 pi selections
	self.selDstRSR = makeDstar2D0Pi( dst2DRS_name+'R'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2KpiR, allPions]
                                 )

	self.selDstRSM = makeDstar2D0Pi( dst2DRS_name+'M'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2KpiM, allPions]
                                 )

	self.selDstWSR = makeDstar2D0Pi( dst2DWS_name+'R'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2WSR, allPions]
                                 )

	self.selDstWSM = makeDstar2D0Pi( dst2DWS_name+'M'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2WSM, allPions]
                                 )

	self.selDstRSRSS = makeDstar2D0Pi( dst2DRS_name+'RSS'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2KpiRSS, allPions]
                                 )

	self.selDstRSMSS = makeDstar2D0Pi( dst2DRS_name+'MSS'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2KpiMSS, allPions]
                                 )

	self.selDstKKR = makeDstar2D0Pi( dst2DKK_name+'R'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2KKR, allPions]
                                 )

	self.selDstKKM = makeDstar2D0Pi( dst2DKK_name+'M'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2KKM, allPions]
                                 )

	self.selDstKKRSS = makeDstar2D0Pi( dst2DKK_name+'RSS'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2KKRSS, allPions]
                                 )

	self.selDstKKMSS = makeDstar2D0Pi( dst2DKK_name+'MSS'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2KKMSS, allPions]
                                 )

	self.selDstpipiR = makeDstar2D0Pi( dst2Dpipi_name+'R'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2pipiR, allPions]
                                 )

	self.selDstpipiM = makeDstar2D0Pi( dst2Dpipi_name+'M'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2pipiM, allPions]
                                 )

	self.selDstpipiRSS = makeDstar2D0Pi( dst2Dpipi_name+'RSS'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2pipiRSS, allPions]
                                 )

	self.selDstpipiMSS = makeDstar2D0Pi( dst2Dpipi_name+'MSS'
				   , config
                                   , '[D*(2010)+ -> D0 pi+]cc'
                                   , inputSel = [self.selD2pipiMSS, allPions]
                                 )

        # Tagged lines
        self.dstRSR_line = StrippingLine(dst2DRS_name+"RLine",
                                        prescale = config['TaggedRSLinePrescale'],
                                        postscale = config['TaggedRSLinePostscale'],
                                        selection = self.selDstRSR
                                       )

        self.dstRSM_line = StrippingLine(dst2DRS_name+"MLine",
                                        prescale = config['TaggedRSLinePrescale'],
                                        postscale = config['TaggedRSLinePostscale'],
                                        selection = self.selDstRSM
                                       )

        self.dstWSR_line = StrippingLine(dst2DWS_name+"RLine",
                                        prescale = config['TaggedWSLinePrescale'],
                                        postscale = config['TaggedWSLinePostscale'],
                                        selection = self.selDstWSR
                                       )

        self.dstWSM_line = StrippingLine(dst2DWS_name+"MLine",
                                        prescale = config['TaggedWSLinePrescale'],
                                        postscale = config['TaggedWSLinePostscale'],
                                        selection = self.selDstWSM
                                       )

        self.dstRSRSS_line = StrippingLine(dst2DRS_name+"RSSLine",
                                        prescale = config['TaggedRSLinePrescale'],
                                        postscale = config['TaggedRSLinePostscale'],
                                        selection = self.selDstRSRSS
                                       )

        self.dstRSMSS_line = StrippingLine(dst2DRS_name+"MSSLine",
                                        prescale = config['TaggedRSLinePrescale'],
                                        postscale = config['TaggedRSLinePostscale'],
                                        selection = self.selDstRSMSS
                                       )

        self.dstKKR_line = StrippingLine(dst2DKK_name+"RLine",
                                        prescale = config['TaggedSCSLinePrescale'],
                                        postscale = config['TaggedSCSLinePostscale'],
                                        selection = self.selDstKKR
                                       )

        self.dstKKM_line = StrippingLine(dst2DKK_name+"MLine",
                                        prescale = config['TaggedSCSLinePrescale'],
                                        postscale = config['TaggedSCSLinePostscale'],
                                        selection = self.selDstKKM
                                       )

        self.dstKKRSS_line = StrippingLine(dst2DKK_name+"RSSLine",
                                        prescale = config['TaggedSCSLinePrescale'],
                                        postscale = config['TaggedSCSLinePostscale'],
                                        selection = self.selDstKKRSS
                                       )

        self.dstKKMSS_line = StrippingLine(dst2DKK_name+"MSSLine",
                                        prescale = config['TaggedSCSLinePrescale'],
                                        postscale = config['TaggedSCSLinePostscale'],
                                        selection = self.selDstKKMSS
                                       )

        self.dstpipiR_line = StrippingLine(dst2Dpipi_name+"RLine",
                                        prescale = config['TaggedSCSLinePrescale'],
                                        postscale = config['TaggedSCSLinePostscale'],
                                        selection = self.selDstpipiR
                                       )

        self.dstpipiM_line = StrippingLine(dst2Dpipi_name+"MLine",
                                        prescale = config['TaggedSCSLinePrescale'],
                                        postscale = config['TaggedSCSLinePostscale'],
                                        selection = self.selDstpipiM
                                       )

        self.dstpipiRSS_line = StrippingLine(dst2Dpipi_name+"RSSLine",
                                        prescale = config['TaggedSCSLinePrescale'],
                                        postscale = config['TaggedSCSLinePostscale'],
                                        selection = self.selDstpipiRSS
                                       )

        self.dstpipiMSS_line = StrippingLine(dst2Dpipi_name+"MSSLine",
                                        prescale = config['TaggedSCSLinePrescale'],
                                        postscale = config['TaggedSCSLinePostscale'],
                                        selection = self.selDstpipiMSS
                                       )

        # register lines
	if config['RunSameSign']:
	  self.registerLine(self.dstRSRSS_line)
	  self.registerLine(self.dstRSMSS_line)
	  self.registerLine(self.dstKKRSS_line)
	  self.registerLine(self.dstKKMSS_line)
	  self.registerLine(self.dstpipiRSS_line)
	  self.registerLine(self.dstpipiMSS_line)

	if config['RunDefault']:
          self.registerLine(self.dstRSR_line)
          self.registerLine(self.dstRSM_line)
          self.registerLine(self.dstWSR_line)
          self.registerLine(self.dstWSM_line)
          self.registerLine(self.dstKKR_line)
          self.registerLine(self.dstKKM_line)
          self.registerLine(self.dstpipiR_line)
          self.registerLine(self.dstpipiM_line)

def makeD2hhAsymm(name,
             config,
	     KPIDK_string,
	     PiPIDK_string,
	     Mass_low_string,
	     Mass_high_string,
	     Kst_Mass_high_string,
	     CombPIDK_string,
	     DecayDescriptor,
             inputSel,
	     useResolved,
	     TOSFilter
            ) :
    """
    Create and return a D0 -> hh' Selection object.
    Arguments:
    name        : name of the Selection.
    config      : dictionary of cut values.
    ..._string  : cut implementation for PIDK cuts.
    DecayDescriptor: DecayDescriptor.
    inputSel    : input selections
    useResolved : True = use resolved pi0, False = use merged pi0
    """

    def makeTISTOS( name, _input, _hlttos ) :
            from Configurables import TisTosParticleTagger
            _tisTosFilter = TisTosParticleTagger( name + "Tagger" )
            _tisTosFilter.TisTosSpecs = _hlttos
            return Selection( name
                              , Algorithm = _tisTosFilter
                              , RequiredSelections = [ _input ]
                              )        

    _Kcuts1  = "~ISMUON & (PT > %(DaugPtMin)s* MeV) & (MIPCHI2DV(PRIMARY) > %(DaugIPChi2Loose)s)" % locals()['config']
    _KcutsPIDK  = KPIDK_string % locals()['config']
    _Kcuts2  = " & (ISLONG) & (P > %(DaugP)s* MeV) & (TRCHI2DOF < %(DaugTrkChi2)s)" % locals()['config']
    _Kcuts = _Kcuts1 + _KcutsPIDK + _Kcuts2
    _Picuts1 = "~ISMUON & (PT > %(DaugPtMin)s* MeV) & (MIPCHI2DV(PRIMARY) > %(DaugIPChi2Loose)s)" % locals()['config']
    _PicutsPIDK  = PiPIDK_string % locals()['config']
    _Picuts2 = " & (ISLONG) & (P > %(DaugP)s* MeV) & (TRCHI2DOF < %(DaugTrkChi2)s)" % locals()['config']
    _Picuts = _Picuts1 + _PicutsPIDK + _Picuts2
    _dauCuts = { 'K+': _Kcuts, 'pi+': _Picuts }

    _KstmassHigh  = Kst_Mass_high_string % locals()['config']
    _combCutsKst1 = "(APT > %(KstPt)s* MeV)" \
		"& (AHASCHILD( PT > %(DaugPtMax)s* MeV ) )" \
		"& (AHASCHILD( MIPCHI2DV(PRIMARY) > %(DaugIPChi2)s ) )" \
    		"& (ADOCA(1,2)< %(KstDOCA)s* mm)" \
                "& (AP > %(KstP)s* MeV)" % locals()['config']
    _combCutsKstPIDK = CombPIDK_string % locals()['config']
    _combCutsKst = _combCutsKst1 + _combCutsKstPIDK + _KstmassHigh

    _motherCutsKst = "(VFASPF(VCHI2PDOF) < %(KstVtxChi2Ndof)s)" \
                  "& (BPVVDCHI2 > %(KstFDChi2)s)" % locals()['config']

    _decay = '[K*(892)0 -> '+DecayDescriptor+']cc'
    _Kstar = CombineParticles( DecayDescriptor = _decay,
                               MotherCut = _motherCutsKst,
                               CombinationCut = _combCutsKst,
                               DaughtersCuts = _dauCuts)

    _selKstar = Selection( name+'KstarSel',
                           Algorithm = _Kstar,
			   RequiredSelections = inputSel )

    if useResolved:
      _Pi0Cuts = "(PT>%(Pi0MinPT_R)s *MeV) & (CHILD(CL,1)>%(ResPi0MinGamCL)s) & (CHILD(CL,2)>%(ResPi0MinGamCL)s)" %locals()['config']
    else:  
      _Pi0Cuts = "(PT>%(Pi0MinPT_M)s *MeV)" %locals()['config']

    _dauCuts = { 'pi0': _Pi0Cuts }

    _massLow  = Mass_low_string % locals()['config']
    _massHigh  = Mass_high_string % locals()['config']
    _combCuts1 = "(APT > %(D0Pt)s* MeV)" \
                "& (AP > %(D0P)s* MeV)" % locals()['config']
    _combCuts = _combCuts1 + _massLow + _massHigh

    _motherCuts = "(VFASPF(VCHI2PDOF) < %(D0VtxChi2Ndof)s)" \
                  "& (BPVVDCHI2 > %(D0FDChi2)s)" \
                  "& (BPVDIRA > %(D0BPVDira)s)" % locals()['config']

    _D0 = CombineParticles( DecayDescriptor = '[D0 -> K*(892)0 pi0]cc',
                            MotherCut = _motherCuts,
                            CombinationCut = _combCuts,
                            DaughtersCuts = _dauCuts)


    if useResolved:
      _myPi0 = StdLooseResolvedPi0
    else:  
      _myPi0 = StdLooseMergedPi0

    _sel = Selection ( name+'Sel',
                       Algorithm = _D0,
                       RequiredSelections = [_selKstar,_myPi0])

    _useTOS = config['UseTOSFilter']
    if not _useTOS:
      return _sel

    if 'TOS' == TOSFilter:
      _Hlt2TOS = config['Hlt2TOS']
    elif 'NONE' == TOSFilter:
      return _sel
      
    _selD2hhHlt2TOS = makeTISTOS( name + "D2hhHlt2TOS"
                                            , _sel
                                            , _Hlt2TOS
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
