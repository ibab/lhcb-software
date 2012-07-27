__author__ = ['Conor Fitzpatrick']
__date__ = '26/06/2012'
__version__ = '$Revision: 1.0 $'

'''
This file contains stripping lines using the Hlt1MBNoBiasDecision
D02Kpi taken from StrippingD2hh.py
D2Kpipi taken from StrippingD2hhh_conf.py
Phi, Jpsi lines modified from CommonParticles selections. 
'''
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles, FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseKaons as Kaons 
from StandardParticles import StdLoosePions as Pions 
from StandardParticles import StdLooseMuons as Muons

default_config = { 'D02Kpi_DaugPtMin': 800.,
           'D02Kpi_DaugPtMax': 1500.,
           'D02Kpi_DaugP': 5000.,
           'D02Kpi_DaugIPChi2': 9.,
           'D02Kpi_DaugTrkChi2': 3.,
           'D02Kpi_HighPIDK': 5.,
           'D02Kpi_LowPIDK': 0.,
           'D02Kpi_D0Pt': 2000.,
           'D02Kpi_D0MassWindowCentre': 1865.,
           'D02Kpi_D0KPiMassWindowWidthLow':  -100.,
           'D02Kpi_D0KPiMassWindowWidthHigh': 200.,
           'D02Kpi_D0P': 5000.,
           'D02Kpi_D0VtxChi2Ndof': 10.,
           'D02Kpi_D0FDChi2': 40.,
           'D02Kpi_D0BPVDira': 0.9999,
           'D02Kpi_D0DOCA': 0.07,
           'D02Kpi_Prescale':0.,
           'D02Kpi_Postscale':1.,
           
	   'D2KPP_DaughterPT':250.,
	   'D2KPP_DaughterP':2000.,
	   'D2KPP_DaughterIPChi2':4.,
	   'D2KPP_Daughter2IPChi2':10.,
	   'D2KPP_KPIDK':7.,
	   'D2KPP_piPIDK':3.,
	   'D2KPP_PTSum':2800.,
	   'D2KPP_DPt':1000.,
	   'D2KPP_DIPChi2':15.,
	   'D2KPP_DdcaFDChi2':100.,
	   'D2KPP_DDIRA':0.,
	   'D2KPP_DVXChi2NDOF':10.,
	   'D2KPP_MinMassPosFit':1800.,
	   'D2KPP_MaxMassPosFit':2040.,
           'D2KPP_Prescale':0.,
           'D2KPP_Postscale':1.,


	'Phi2KK_DaugPtMin':500.,
	'Phi2KK_PtMin':2000.,
	'Phi2KK_PIDK':5.,
	'Phi2KK_DaugP':0.,
	'Phi2KK_DaugTrkChi2':5.,
	'Phi2KK_VFASPFVCHI2':10.,
	'Phi2KK_CombAM':1050.,
	'Phi2KK_ADOCACHI2CUT':30.,
	'Phi2KK_MIPCHI2':9.,
	'Phi2KK_Prescale':0.2,
	'Phi2KK_Postscale':1.,
		
	'Jpsi2MM_DaugPtMin':0.,
	'Jpsi2MM_DaugP':0.,
	'Jpsi2MM_DaugTrkChi2':10.,
	'Jpsi2MM_VFASPFVCHI2':25.,
	'Jpsi2MM_ADAMASS':100.,
	'Jpsi2MM_ADOCACHI2CUT':30.,
	'Jpsi2MM_Prescale':1.,
	'Jpsi2MM_Postscale':1.,

	'HLT':"HLT_PASS_RE('Hlt1MBNoBiasDecision')",
#	'HLT':"HLT_PASS_RE('Hlt1*.*')",
           
}
class TriggerValidationConf(LineBuilder) :
    '''
    Produces 4 lines:
    Phi->KK
    D->Kpi
    D->Kpipi
    
    Usage:
    from StrippingSelections import StrippingTriggerValidation
    confTriggerValidation = StrippingTriggerValidation.TriggerValidationConf("TriggerValidation",StrippingTriggerValidation.default_config)
    stream.appendLines( confTriggerValidation.lines() )
    '''
    __configuration_keys__ = ('D02Kpi_DaugPtMin',
                              'D02Kpi_DaugPtMax',
                              'D02Kpi_DaugP',
                              'D02Kpi_DaugIPChi2',
                              'D02Kpi_DaugTrkChi2',
                              'D02Kpi_HighPIDK',
                              'D02Kpi_LowPIDK',
                              'D02Kpi_D0Pt',
                              'D02Kpi_D0MassWindowCentre',
                              'D02Kpi_D0KPiMassWindowWidthLow',
                              'D02Kpi_D0KPiMassWindowWidthHigh',
                              'D02Kpi_D0P',
                              'D02Kpi_D0VtxChi2Ndof',
                              'D02Kpi_D0FDChi2',
                              'D02Kpi_D0BPVDira',
                              'D02Kpi_D0DOCA',
                              'D02Kpi_Prescale',
			      'D02Kpi_Postscale',
			      
			      'D2KPP_DaughterPT',
                              'D2KPP_DaughterP',
                              'D2KPP_DaughterIPChi2',
                              'D2KPP_Daughter2IPChi2',
                              'D2KPP_KPIDK',
                              'D2KPP_piPIDK',
                              'D2KPP_PTSum',
                              'D2KPP_DPt',
                              'D2KPP_DIPChi2',
                              'D2KPP_DdcaFDChi2',
                              'D2KPP_DDIRA',
                              'D2KPP_DVXChi2NDOF',
                              'D2KPP_MinMassPosFit',
                              'D2KPP_MaxMassPosFit',
                              'D2KPP_Prescale',
                              'D2KPP_Postscale',


				'Phi2KK_DaugPtMin',
				'Phi2KK_PtMin',
				'Phi2KK_MIPCHI2',
				'Phi2KK_PIDK',
				'Phi2KK_DaugP',
				'Phi2KK_DaugTrkChi2',
				'Phi2KK_VFASPFVCHI2',
				'Phi2KK_CombAM',
				'Phi2KK_ADOCACHI2CUT',
				'Phi2KK_Prescale',
				'Phi2KK_Postscale',
				
			        'Jpsi2MM_DaugPtMin',
				'Jpsi2MM_DaugP',
				'Jpsi2MM_DaugTrkChi2',
				'Jpsi2MM_VFASPFVCHI2',
				'Jpsi2MM_ADAMASS',
				'Jpsi2MM_ADOCACHI2CUT',
				'Jpsi2MM_Prescale',
				'Jpsi2MM_Postscale',
				'HLT'
                              )


    def __init__(self, name, config) :

	LineBuilder.__init__(self, name, config)


	d02kpi_name = name+'D02KPi'
        self.selD02Kpi = makeD02Kpi(d02kpi_name, config)
        self.d02kpi_line = StrippingLine(d02kpi_name+"Line",
                                        prescale = config['D02Kpi_Prescale'],
                                        postscale = config['D02Kpi_Postscale'],
                                        selection = self.selD02Kpi,
					HLT=config['HLT']

                                       )
	self.registerLine(self.d02kpi_line)
        
        d2kpipi_name=name+'D2KPiPi'
        self.selD2Kpipi = makeD2Kpipi(d2kpipi_name, config)
        self.d2Kpipi_line = StrippingLine(d2kpipi_name+"Line",
		                        prescale=config['D2KPP_Prescale'],
					postscale=config['D2KPP_Postscale'],
					selection=self.selD2Kpipi,
					HLT=config['HLT']
					)
	self.registerLine(self.d2Kpipi_line)
	


	jpsi2mm_name=name+'Jpsi2MM'
	self.selJpsi2MM=makeJpsi2MM(jpsi2mm_name,config)
	self.jpsi2MM_line=StrippingLine(jpsi2mm_name+"Line",
				        prescale=config['Jpsi2MM_Prescale'],
				        postscale=config['Jpsi2MM_Postscale'],
					selection=self.selJpsi2MM,
					HLT=config['HLT']
					)
	self.registerLine(self.jpsi2MM_line)
	
	phi2kk_name=name+'Phi2KK'
	self.selPhi2KK=makePhi2KK(phi2kk_name,config)
	self.phi2KK_line = StrippingLine(phi2kk_name+"Line",
					prescale=config['Phi2KK_Prescale'],
					postscale=config['Phi2KK_Postscale'],
					selection=self.selPhi2KK,
					HLT=config['HLT']
					)
	self.registerLine(self.phi2KK_line)				

def makeD02Kpi(name,config) :
        """
        Create and return a D0 -> Kpi Selection object.
        Arguments:
        name        : name of the Selection.
        config      : dictionary of cut values.
        """
     

        _Kcuts1  = "~ISMUON & (PT > %(D02Kpi_DaugPtMin)s* MeV) & (MIPCHI2DV(PRIMARY) > %(D02Kpi_DaugIPChi2)s)" % locals()['config']
        _KcutsPIDK  = " & (PIDK > %(D02Kpi_HighPIDK)s)" % locals()['config']


        _Kcuts2  = " & (ISLONG) & (P > %(D02Kpi_DaugP)s* MeV) & (TRCHI2DOF < %(D02Kpi_DaugTrkChi2)s)" % locals()['config']
        _Kcuts = _Kcuts1 + _KcutsPIDK + _Kcuts2
        _Picuts1 = "~ISMUON & (PT > %(D02Kpi_DaugPtMin)s* MeV) & (MIPCHI2DV(PRIMARY) > %(D02Kpi_DaugIPChi2)s)" % locals()['config']
        _PicutsPIDK  = " & (PIDK < %(D02Kpi_LowPIDK)s)" % locals()['config']
        _Picuts2 = " & (ISLONG) & (P > %(D02Kpi_DaugP)s* MeV) & (TRCHI2DOF < %(D02Kpi_DaugTrkChi2)s)" % locals()['config']
        _Picuts = _Picuts1 + _PicutsPIDK + _Picuts2
        _dauCuts = { 'K+': _Kcuts, 'pi+': _Picuts }

        _massLow  = "& (DAMASS(%(D02Kpi_D0MassWindowCentre)s* MeV) > %(D02Kpi_D0KPiMassWindowWidthLow)s* MeV)" % locals()['config']
        _massHigh  = "& (DAMASS(%(D02Kpi_D0MassWindowCentre)s* MeV) < %(D02Kpi_D0KPiMassWindowWidthHigh)s* MeV)" % locals()['config']
        _combCuts1 = "(APT > %(D02Kpi_D0Pt)s* MeV)" \
                    "& (AHASCHILD( PT > %(D02Kpi_DaugPtMax)s* MeV ) )" \
                    "& (ADOCA(1,2)< %(D02Kpi_D0DOCA)s* mm)" \
                    "& (AP > %(D02Kpi_D0P)s* MeV)" % locals()['config']
        _combCutsPIDK = '' % locals()['config']
        _combCuts = _combCuts1 + _combCutsPIDK + _massLow + _massHigh
    
        _motherCuts = "(VFASPF(VCHI2PDOF) < %(D02Kpi_D0VtxChi2Ndof)s)" \
                      "& (BPVVDCHI2 > %(D02Kpi_D0FDChi2)s)" \
                      "& (BPVDIRA > %(D02Kpi_D0BPVDira)s)" % locals()['config']
    
        _D0 = CombineParticles( DecayDescriptor = '[D0 -> K- pi+]cc',
                                MotherCut = _motherCuts,
                                CombinationCut = _combCuts,
                                DaughtersCuts = _dauCuts)
        _sel = Selection ( name+'Sel',
                           Algorithm = _D0,
                         RequiredSelections = [Kaons,Pions])
    
        return _sel

def makeD2Kpipi(name, config) : 
	"""
	Create and return a D -> Kpipi Selection object.
	Arguments:
	name        : name of the Selection.
        config      : dictionary of cut values.
        """
        _Daughtercuts_pi = """ 
                       (MINTREE('pi+'==ABSID, PT) > %(D2KPP_DaughterPT)s ) &
                       (MINTREE('pi+'==ABSID, P ) > %(D2KPP_DaughterP)s) &
                       (MINTREE('pi+'==ABSID, MIPCHI2DV(PRIMARY)) > %(D2KPP_DaughterIPChi2)s ) 
                       """ %locals()['config']
        _Daughtercuts_K = """
                      (MINTREE('K-'==ABSID, PT) > %(D2KPP_DaughterPT)s ) &
                      (MINTREE('K-'==ABSID, P ) > %(D2KPP_DaughterP)s) & 
                      (MINTREE('K-'==ABSID, MIPCHI2DV(PRIMARY)) > %(D2KPP_DaughterIPChi2)s ) 
                      """ %locals()['config']
        _Combcuts_HHH =""" 
                   (SUMTREE( ISBASIC , PT ) > %(D2KPP_PTSum)s*MeV) &
                   (2 <= NINGENERATION((MIPCHI2DV(PRIMARY) > %(D2KPP_Daughter2IPChi2)s ) , 1)) 
                   """ % locals()['config']

        _Mothercuts_HHH = """ 
                      (PT > %(D2KPP_DPt)s) & (VFASPF(VCHI2/VDOF) < %(D2KPP_DVXChi2NDOF)s) & 
                      (BPVDIRA > %(D2KPP_DDIRA)s) & (BPVIPCHI2() < %(D2KPP_DIPChi2)s) & 
                      (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(D2KPP_DdcaFDChi2)s)
                      """ % locals()['config']
        _cutsMassPosFit = " (in_range ( %(D2KPP_MinMassPosFit)s ,  M  , %(D2KPP_MaxMassPosFit)s )) " % locals()['config']
        _DaughterCuts = _Daughtercuts_pi
        if config['D2KPP_piPIDK'] != None :
             _DaughterCuts += " & (MAXTREE('pi+'==ABSID, PIDK-PIDpi) < %(D2KPP_piPIDK)s) " % locals()['config']

        if config['D2KPP_KPIDK'] != None :
             _DaughterCuts += " & (MINTREE('K-'==ABSID, PIDK-PIDpi) > %(D2KPP_KPIDK)s )" %locals()['config']
             _DaughterCuts += " & "+_Daughtercuts_K


        _code = '('+_DaughterCuts+'&'+_Combcuts_HHH+'&'+_Mothercuts_HHH+'&'+_cutsMassPosFit+')'

        _filterHHH = FilterDesktop( Code = _code )

        stdD2KPP = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi/Particles")
        _sel = Selection ( name+'Sel',
                       Algorithm = _filterHHH,
                       RequiredSelections = [stdD2KPP])
	return _sel

def makePhi2KK(name, config) :
	
        _Kcuts1  = "~ISMUON & (PT > %(Phi2KK_DaugPtMin)s* MeV)" % locals()['config']
        _KcutsPIDK  = " & (PIDK > %(Phi2KK_PIDK)s)" % locals()['config']
        _Kcuts2  = " & (P > %(Phi2KK_DaugP)s* MeV) & (TRCHI2DOF < %(Phi2KK_DaugTrkChi2)s) & (MIPCHI2DV(PRIMARY)> %(Phi2KK_MIPCHI2)s)" % locals()['config']
        _dauCuts = _Kcuts1 + _KcutsPIDK + _Kcuts2
	_motherCuts = "(P > %(Phi2KK_PtMin)s) & (VFASPF(VCHI2) < %(Phi2KK_VFASPFVCHI2)s)" % locals()['config']
	_combCuts = "(AM < %(Phi2KK_CombAM)s* MeV) & (ADOCACHI2CUT(%(Phi2KK_ADOCACHI2CUT)s, ''))" % locals()['config']
        _Phi = CombineParticles( DecayDescriptor = 'phi(1020) -> K+ K-',
                                MotherCut = _motherCuts,
                                CombinationCut = _combCuts,
                                DaughtersCuts = {"K+" : _dauCuts})
	
        _sel= Selection ( name+'Sel',
                           Algorithm = _Phi,
                         RequiredSelections = [Kaons] )
	return _sel
    
def makeJpsi2MM(name, config) : 
        _MuCuts="(PT > %(Jpsi2MM_DaugPtMin)s* MeV) & (P > %(Jpsi2MM_DaugP)s* MeV) & (TRCHI2DOF < %(Jpsi2MM_DaugTrkChi2)s)" % locals()['config']
	_combCuts = "(ADAMASS('J/psi(1S)') < %(Jpsi2MM_ADAMASS)s*MeV) & (ADOCACHI2CUT(%(Jpsi2MM_ADOCACHI2CUT)s,''))"  % locals()['config']
        _motherCuts = "(VFASPF(VCHI2) < %(Jpsi2MM_VFASPFVCHI2)s)"  % locals()['config']
	_Jpsi = CombineParticles(DecayDescriptor = 'J/psi(1S) -> mu+ mu-',
		MotherCut = _motherCuts,
		CombinationCut = _combCuts,
		DaughtersCuts = {"mu+" : _MuCuts}
		)
	_sel = Selection(name+'Sel',
		Algorithm = _Jpsi,
		RequiredSelections = [Muons] ) 
	return _sel
		
