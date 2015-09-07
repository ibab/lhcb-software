#!/usr/bin/env python
# $Id: StrippingD2hhh_FTcalib.py,v 0.0 2015-09-05 12:53:17 mdorigo Exp $
'''
Module for construction of D->HHH for FT calibrations (asymmetries in SSK and SSpi)
based on StrippingD2hhh_conf.py from Erica Polycarpo and Alberto Reis

Provides functions to build D2KKP, D2KPP, selections using StdLooseDplus
Provides class D2hhhFTCalibConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - D2hhhFTCalibConf
   - makeStdD2hhhFT
'''

__author__ = ['Mirco Dorigo']
__date__ = '05/09/2015'
__version__ = '$Revision: 0.0 $'

__all__ = ('D2hhhFTCalibConf',
           'makeStdD2hhhFT')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import ( StdNoPIDsPions,
                                StdTightKaons ) 

name = "D2hhhFTCalib"
# Cuts to be implemented vy the Stripping team in Stripping Settings
# Valid for Stripping 21 (27/8)
# 

default_config = {
    'NAME'        :  'D2hhhFTCalib',
    'WGs'         :  ['ALL'],
    'BUILDERTYPE' : 'D2hhhFTCalibConf',
    'STREAMS' : [ 'CharmCompleteEvent' ],
    'CONFIG'      : {
    'DaughterPT'            : 250.,
    'DaughterP'             : 2000.,
    'DaughterIPChi2'        : 4.,
    'Daughter2IPChi2'       : 10.,
    'D2KKKDaughterIPChi2'   : 4.,
    'D2KKKDaughter2IPChi2'  : 4.,
    'KPIDK'                 : 7.,
    'piPIDK'                : 3.,
    'PTSum'                 : 3200.,
    'DOCAMax'               : 0.5,
    'DOCAChi2'              : 50.,
    'DPt'                   : 2000.,
    'DIPChi2'               : 1500.,
    'DdcaFDChi2'            : 150.,
    'DPVLTIME'              : 0.2,
    'DDIRA'                 : 0.99995,
    'DVXChi2NDOF'           : 6.,
    'MinMassPosFit'         : 1920.,
    'MaxMassPosFit'         : 2040.,
    'MaxTracksInEvent'      : 500,
    'D2KKPLinePrescale'     : 0.2,
    'D2KKPLinePostscale'    : 1.0 ,
      }
    }

class D2hhhFTCalibConf(LineBuilder) :
    """
    Builder of D->hhh stripping Selection and StrippingLine.
    Constructs D ->hhh Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> D2HHHConf = D2hhhConf('D2HHHtest',config)
    >>> D2hhhLines = D2HHHConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selKKP             :  get StdLooseDplus2KKP 
    selKPP             :  get StdLooseDplus2KPP 
    selPPP             :  get StdTightDplus2PPP 
    selKPPos           :  get StdLooseDplus2KPPi (DCS)
    selTightKaon       :  get StdTightKaons
    selNoPIDsPion      :  get StdNoPIDsPion 
    D2KKP_line         :  StrippingLine made out of selKKP 
    D2KPP_line         :  StrippingLine made out of selKPP 
    D2PPP_line         :  StrippingLine made out of selPPP 
    D2KPPos_line       :  StrippingLine made out of selKPPos
    D2KKK_line         :  StrippingLine made out of 3K combinations
    Ds2KKPos_line      :  StrippingLine made out of 3K combinations
    D2HHHInc_line      :  StrippingLine made out of 3PI combinations with large mass window
    lines              :  List of lines [D2KKP_line, D2KPP_line, D2PPP_line, D2KPPos_line, D2KKK_line, D2HHHInc_line]

    Exports as class data member:
    D2HHHConf.__configuration_keys__ : List of required configuration parameters.
					RelatedInfoTools = [
					  {
					    'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
					    'RecursionLevel' : 1,
					    'Locations' : {self.selD2KKP : 'P2CVD2KKP',}
					    },
					  ],
    """

    __configuration_keys__ = ('DaughterPT',                       
                              'DaughterP',
                              'DaughterIPChi2',
                              'Daughter2IPChi2',
                              'D2KKKDaughterIPChi2',
                              'D2KKKDaughter2IPChi2',
                              'KPIDK',
                              'piPIDK',
                              'PTSum',
                              'DOCAMax',
                              'DOCAChi2',
                              'DPt',
                              'DIPChi2',
                              'DdcaFDChi2',
                              'DPVLTIME',
                              'DDIRA',
                              'DVXChi2NDOF',
                              'MinMassPosFit',
                              'MaxMassPosFit',
                              'MaxTracksInEvent',
                              'D2KKPLinePrescale',
                              'D2KKPLinePostscale',
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        D2KKP_name = name+'_KKP'

        self.selKKP = DataOnDemand(Location = "Phys/StdLooseDplus2KKPi/Particles")

        self.selD2KKP = makeStdD2hhhFT(D2KKP_name,  
                                       hhhSel = self.selKKP, 
                                       DaughterPT= config['DaughterPT'],
                                       DaughterP = config['DaughterP'],
                                       DaughterIPChi2 = config['DaughterIPChi2'],
                                       Daughter2IPChi2 = config['Daughter2IPChi2'],
                                       DaughterDOCAChi2 = config['DOCAChi2'],
                                       PTSum = config['PTSum'],
                                       DDIRA= config['DDIRA'],
                                       DIPChi2= config['DIPChi2'],
                                       DdcaFDChi2= config['DdcaFDChi2'],
                                       DPVLTIME = config['DPVLTIME'],
                                       DPt = config['DPt'],
                                       DVXChi2NDOF = config['DVXChi2NDOF'],
                                       MinMassPosFit= config['MinMassPosFit'],
                                       MaxMassPosFit= config['MaxMassPosFit'],
                                       KPIDK = config['KPIDK'],
                                       piPIDK = config['piPIDK']
                                       )
        self.filterGE = globalEventCutFilter(name + 'GECFilter', 
                                    MaxTrSIZE = config['MaxTracksInEvent'])

        self.D2KKP_line = StrippingLine(D2KKP_name+"Line",
                                        prescale = config['D2KKPLinePrescale'],
                                        postscale = config['D2KKPLinePostscale'],
                                        selection = self.selD2KKP,
                                        FILTER = self.filterGE
                                        )

        self.registerLine(self.D2KKP_line)


def makeStdD2hhhFT(name,
                 hhhSel,
		 DaughterPT,
		 DaughterP,
		 DaughterIPChi2,
		 Daughter2IPChi2, 
		 DaughterDOCAChi2, 
		 PTSum,
		 DDIRA,
		 DIPChi2,
		 DdcaFDChi2,
		 DPVLTIME,
		 DPt,
		 DVXChi2NDOF,
		 MinMassPosFit,
		 MaxMassPosFit,
		 KPIDK=None,
		 piPIDK=None
                 ):
    """
    Create and return a D -> HHH Selection object.
    Arguments:
    name           : name of the Selection.
    hhhSel         : Standard Specific D2HHH selection    
    DaughterPT     : Minimum PT among daughters
    DaughterIPChi2 : Minimum IPChi2 among daughters
    Daughter2IPChi2: Minimum IPChi2 required to at least 2 daughters 
    DaughterP      : Minimum P among daughters
    DaughterDOCAChi2:Maximum distance of closest approach between 2 daughters in error units 
    PTSum          : Minimum sum of daughters momenta
    DDIRA          : Minimum opening angle between sum_p and FD-direction
    DIPChi2        : Maximum IPChi2 of the D
    DdcaFDChi2     : Minimum distance from SV to any PV
    DPVLTIME       : Minimum decay time
    DPt            : Minimum D Momentum
    DVXChi2NDOF    : Maximum Chi2 of the D Vertex
    MinMassPosFit  : Minimum value of HHH invariant mass (MeV)
    MaxMassPosFit  : Maximum value of HHH invariant mass (MeV).
    KPIDK=None     : Minimum Kaon - pion DLL for kaons
    piPIDK=None    : Maximum Kaon - pion DLL for pions
    """
    _Daughtercuts_pi = """
                       (MINTREE('pi+'==ABSID, PT) > %(DaughterPT)s*MeV ) &
                       (MINTREE('pi+'==ABSID, P ) > %(DaughterP)s*MeV) &
                       (MINTREE('pi+'==ABSID, MIPCHI2DV(PRIMARY)) > %(DaughterIPChi2)s ) 
                       """ %locals()
    _Daughtercuts_K = """
                      (MINTREE('K-'==ABSID, PT) > %(DaughterPT)s*MeV ) &
                      (MINTREE('K-'==ABSID, P ) > %(DaughterP)s*MeV) & 
                      (MINTREE('K-'==ABSID, MIPCHI2DV(PRIMARY)) > %(DaughterIPChi2)s ) 
                      """ %locals() 
    _Combcuts_HHH =""" 
                   ((SUMTREE( ISBASIC , PT ) > %(PTSum)s*MeV) &
                   (2 <= NINGENERATION((MIPCHI2DV(PRIMARY) > %(Daughter2IPChi2)s ) , 1))) 
                   """ % locals()
                    
    _Mothercuts_HHH = """ 
                      (PT > %(DPt)s) & (VFASPF(VCHI2/VDOF) < %(DVXChi2NDOF)s) & 
                      (BPVDIRA > %(DDIRA)s) & (BPVIPCHI2() < %(DIPChi2)s) & 
                      (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DdcaFDChi2)s) & (BPVLTIME() > %(DPVLTIME)s*picosecond)
                      & (DOCACHI2MAX < %(DaughterDOCAChi2)s)  
                      """ % locals()
    _cutsMassPosFit = " (in_range ( %(MinMassPosFit)s ,  M  , %(MaxMassPosFit)s )) " % locals()
    _DaughterCuts = _Daughtercuts_pi
    
    if piPIDK != None :
       _DaughterCuts += " & (MAXTREE('pi+'==ABSID, PIDK-PIDpi) < %(piPIDK)s) " % locals() 
   
    if KPIDK != None :
       _DaughterCuts += " & (MINTREE('K-'==ABSID, PIDK-PIDpi) > %(KPIDK)s )" %locals()
       _DaughterCuts += " & "+_Daughtercuts_K   
  
    _code = '('+_DaughterCuts+'&'+_Combcuts_HHH+'&'+_Mothercuts_HHH+'&'+_cutsMassPosFit+')'
 
    _filterHHH = FilterDesktop( Code = _code ) 
   
    return Selection ( name,
                       Algorithm = _filterHHH,
                       RequiredSelections = [hhhSel])



def globalEventCutFilter(name, 
                         MaxTrSIZE = None 
                         ) :
#  
  if MaxTrSIZE == None : return None
  
  from Configurables import LoKi__VoidFilter as VoidFilter
  from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
  modules = CoreFactory('CoreFactory').Modules
  for i in ['LoKiTracks.decorators']:
     if i not in modules : modules.append(i)
  
  _code = "(recSummary(LHCb.RecSummary.nTracks, 'Rec/Track/Best') < %(MaxTrSIZE)s )" % locals()
  
  
  return _code          

