#!/usr/bin/env python
# $Id: StrippingD2hhh_conf.py,v 1.1 2011-02-14 12:53:17 polye Exp $
'''
Module for construction of D->HHH based on 
 Bs->JpsiPhi pre-scaled and detatched stripping Selections and StrippingLines
 from Greig and Juan and D->HHH selections cuts from Hamish and Mat.
Provides functions to build D2KKP, D2KPP, D2PPP, D2KPPos selections using 
 StdLooseDplus and D2KKK and D2hhh inclusive using CombineParticles.
Provides class D2hhhConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - D2hhhConf
   - makeStdD2hhh
   - makeD2KKK
   - makeDs2KKPos
   - makeD2hhhInc
'''

__author__ = ['Erica Polycarpo','Alberto Reis']
__date__ = '14/02/2011'
__version__ = '$Revision: 1.1 $'

__all__ = ('D2hhhConf',
           'makeStdD2hhh', 'makeD2KKK', 'makeD2HHHInc', 'makeDs2KKPos' )

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import ( StdNoPIDsPions,
                                StdTightKaons ) 

name = "D2hhh"
# Cuts to be implemented vy the Stripping team in Stripping Settings
# Valid for Stripping 21 (27/8)
# 

default_config = {
    'NAME'        :  'D2hhhLines',
    'WGs'         :  ['Charm'],
    'BUILDERTYPE' : 'D2hhhConf',
    'STREAMS' : [ 'Charm' ],
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
    'MinMassPosFit'         : 1800.,
    'MaxMassPosFit'         : 2040.,
    'D2KPPMaxMassPosFit'    : 1940.,
    'Ds2KKPosMinMassPosFit' : 1900.,
    'D2HHHIncMinMassPosFit' : 1100.,
    'D2HHHIncMaxMassPosFit' : 2070.,
    'MaxTracksInEvent'      : 500,
    'D2KKPLinePrescale'     : 1.0,
    'D2KKPLinePostscale'    : 1.0 ,
    'D2KPPLinePrescale'     : 0.5,
    'D2KPPLinePostscale'    : 1.0,
    'D2PPPLinePrescale'     : 0.5,
    'D2PPPLinePostscale'    : 1.0,
    'D2KKKLinePrescale'     : 1.0,
    'D2KKKLinePostscale'    : 1.0,
    'Ds2KKPosLinePrescale'  : 1.0,
    'Ds2KKPosLinePostscale' : 1.0,
    'D2KPPosLinePrescale'   : 1.0,
    'D2KPPosLinePostscale'  : 1.0,
    'D2HHHIncLinePrescale'  : 0.03,
    'D2HHHIncLinePostscale' : 1.0,
      }
    }

class D2hhhConf(LineBuilder) :
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
                              'D2KPPMaxMassPosFit',
                              'Ds2KKPosMinMassPosFit',
                              'D2HHHIncMinMassPosFit',
                              'D2HHHIncMaxMassPosFit',
                              'MaxTracksInEvent',
                              'D2KKPLinePrescale',
                              'D2KKPLinePostscale',
                              'D2KPPLinePrescale',
                              'D2KPPLinePostscale',
                              'D2PPPLinePrescale',
                              'D2PPPLinePostscale',
                              'D2KKKLinePrescale',
                              'D2KKKLinePostscale',
                              'Ds2KKPosLinePrescale',
                              'Ds2KKPosLinePostscale',
                              'D2KPPosLinePrescale',
                              'D2KPPosLinePostscale',
                              'D2HHHIncLinePrescale',
                              'D2HHHIncLinePostscale',
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        D2KKP_name = name+'_KKP'
        D2KPP_name = name+'_KPP'
        D2PPP_name = name+'_PPP'
        D2KPPos_name = name+'_KPPos'
        D2KKK_name = name+'_KKK'
        Ds2KKPos_name = name+'_KKPos'
        D2HHHInc_name = name+'_HHHInc'

        self.selKKP = DataOnDemand(Location = "Phys/StdLooseDplus2KKPi/Particles")
        self.selKPP = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi/Particles")
        self.selPPP = DataOnDemand(Location = "Phys/StdTightDplus2PiPiPi/Particles")
        self.selKPPos = DataOnDemand(Location = "Phys/StdTightDplus2KPiPiOppSignPi/Particles")
        self.selKKK = DataOnDemand(Location = "Phys/StdLooseDplus2KKK/Particles")
        self.selKKPos = DataOnDemand(Location = "Phys/StdLooseDsplus2KKPiOppSign/Particles")

        self.selD2KKP = makeStdD2hhh(D2KKP_name,  
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
        self.selD2KPP = makeStdD2hhh(D2KPP_name,  
                                     hhhSel = self.selKPP, 
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
				     MaxMassPosFit= config['D2KPPMaxMassPosFit'],
                                     KPIDK = config['KPIDK'],
                                     piPIDK = config['piPIDK']
			             )
        self.selD2PPP = makeStdD2hhh(D2PPP_name,  
                                     hhhSel = self.selPPP, 
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
                                     piPIDK = config['piPIDK']
                                     )
        self.selD2KPPos = makeStdD2hhh(D2KPPos_name,  
                                     hhhSel = self.selKPPos, 
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
        self.selD2KKK = makeStdD2hhh(D2KKK_name,
                                     hhhSel = self.selKKK,
                                     DaughterPT= config['DaughterPT'],
                                     DaughterP = config['DaughterP'],
                                     DaughterIPChi2 = config['D2KKKDaughterIPChi2'],
                                     Daughter2IPChi2 = config['D2KKKDaughter2IPChi2'],
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
                                     KPIDK = config['KPIDK']
                                     )
        self.selDs2KKPos = makeStdD2hhh(Ds2KKPos_name,
                                     hhhSel = self.selKKPos,
                                     DaughterPT= config['DaughterPT'],
                                     DaughterP = config['DaughterP'],
                                     DaughterIPChi2 = config['D2KKKDaughterIPChi2'],
                                     Daughter2IPChi2 = config['D2KKKDaughter2IPChi2'],
                                     DaughterDOCAChi2 = config['DOCAChi2'],
                                     PTSum = config['PTSum'],
                                     DDIRA= config['DDIRA'],
				     DIPChi2= config['DIPChi2'],
				     DdcaFDChi2= config['DdcaFDChi2'],
				     DPVLTIME = config['DPVLTIME'],
                                     DPt = config['DPt'],
                                     DVXChi2NDOF = config['DVXChi2NDOF'],
				     MinMassPosFit= config['Ds2KKPosMinMassPosFit'],
				     MaxMassPosFit= config['MaxMassPosFit'],
                                     KPIDK = config['KPIDK'],
                                     piPIDK = config['piPIDK']
                                     )
        self.selD2HHHInc = makeD2HHHInc(D2HHHInc_name,  
                                     DaughterPT= config['DaughterPT'],
                                     DaughterP = config['DaughterP'],
                                     DaughterIPChi2 = config['DaughterIPChi2'],
                                     Daughter2IPChi2 = config['Daughter2IPChi2'],
                                     PTSum = config['PTSum'],
                                     DaughterDOCA = config['DOCAMax'],
                                     DaughterDOCAChi2 = config['DOCAChi2'],
				     DDIRA= config['DDIRA'],
				     DIPChi2= config['DIPChi2'],
				     DdcaFDChi2= config['DdcaFDChi2'],
				     DPVLTIME = config['DPVLTIME'],
                                     DPt = config['DPt'],
                                     DVXChi2NDOF = config['DVXChi2NDOF'],
				     MinMassPosFit= config['D2HHHIncMinMassPosFit'],
				     MaxMassPosFit= config['D2HHHIncMaxMassPosFit']
                                     )
        self.filterGE = globalEventCutFilter(name + 'GECFilter', 
                                    MaxTrSIZE = config['MaxTracksInEvent'])

        self.D2KKP_line = StrippingLine(D2KKP_name+"Line",
                                        prescale = config['D2KKPLinePrescale'],
                                        postscale = config['D2KKPLinePostscale'],
                                        selection = self.selD2KKP,
                                        FILTER = self.filterGE
                                        )
        self.D2KPP_line = StrippingLine(D2KPP_name+"Line",
                                        prescale = config['D2KPPLinePrescale'],
                                        postscale = config['D2KPPLinePostscale'],
                                        selection = self.selD2KPP,
                                        FILTER = self.filterGE
                                        )
        self.D2PPP_line = StrippingLine(D2PPP_name+"Line",
                                        prescale = config['D2PPPLinePrescale'],
                                        postscale = config['D2PPPLinePostscale'],
                                        selection = self.selD2PPP,
                                        FILTER = self.filterGE
                                        )
        self.D2KPPos_line = StrippingLine(D2KPPos_name+"Line",
                                        prescale = config['D2KPPosLinePrescale'],
                                        postscale = config['D2KPPosLinePostscale'],
                                        selection = self.selD2KPPos,
                                        FILTER = self.filterGE
                                        )
        self.D2KKK_line = StrippingLine(D2KKK_name+"Line",
                                        prescale = config['D2KKKLinePrescale'],
                                        postscale = config['D2KKKLinePostscale'],
                                        selection = self.selD2KKK,
                                        FILTER = self.filterGE
                                        )
        self.Ds2KKPos_line = StrippingLine(Ds2KKPos_name+"Line",
                                        prescale = config['Ds2KKPosLinePrescale'],
                                        postscale = config['Ds2KKPosLinePostscale'],
                                        selection = self.selDs2KKPos,
                                        FILTER = self.filterGE
                                        )
        self.D2HHHInc_line = StrippingLine(D2HHHInc_name+"Line",
                                        prescale = config['D2HHHIncLinePrescale'],
                                        postscale = config['D2HHHIncLinePostscale'],
                                        selection = self.selD2HHHInc,
                                        FILTER = self.filterGE
                                        )

        self.registerLine(self.D2KKP_line)
        self.registerLine(self.D2KPP_line)
        self.registerLine(self.D2PPP_line)
        self.registerLine(self.D2KPPos_line)
        self.registerLine(self.D2KKK_line)
        self.registerLine(self.Ds2KKPos_line)
        self.registerLine(self.D2HHHInc_line)


def makeStdD2hhh(name,
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


def makeD2HHHInc(name,
              DaughterPT,
              DaughterP,
              DaughterIPChi2,
              Daughter2IPChi2,
              DaughterDOCA,
              DaughterDOCAChi2,
              PTSum,
              DDIRA,
              DIPChi2,
              DdcaFDChi2,
              DPVLTIME,
              DPt,
              DVXChi2NDOF,
              MinMassPosFit,
              MaxMassPosFit
              ):
    """
    Create and return a D -> HHHInclusive selection 
    Arguments:
    name           : name of the Selection.
    pionSel        : Pion Selection (from CommonParticles) 
    DaughterPT     : Minimum PT among daughters
    DaughterP      : Minimum P among daughters
    DaughterIPChi2 : Minimum IPChi2 among daughters
    Daughter2IPChi2: Minimum IPChi2 required to at least 2 daughters 
    DaughterDOCA   : Maximum distance of closest approach between 2 daughters 
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
    """

    _DaughterCuts = "((MIPCHI2DV(PRIMARY)) > %(DaughterIPChi2)s ) & (P > %(DaughterP)s*MeV) & (PT > %(DaughterPT)s*MeV) " % locals() 
                      
    _Combcuts_HHH = "(ADOCACHI2CUT(%(DaughterDOCAChi2)s,'')) & (ADOCACUT(%(DaughterDOCA)s*mm,'')) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(PTSum)s*MeV) & (ANUM(MIPCHI2DV(PRIMARY) > %(Daughter2IPChi2)s ) >= 2) " % locals()
                   
    _Mothercuts_HHH = """ 
                      (PT > %(DPt)s) & (VFASPF(VCHI2/VDOF) < %(DVXChi2NDOF)s) & 
                      (BPVDIRA > %(DDIRA)s) & (BPVIPCHI2() < %(DIPChi2)s) & 
                      (VFASPF(VMINVDCHI2DV(PRIMARY)) > %(DdcaFDChi2)s) & (BPVLTIME() > %(DPVLTIME)s*picosecond)
                      """ % locals()
    _cutsMassPosFit = " (in_range ( %(MinMassPosFit)s ,  M  , %(MaxMassPosFit)s )) " % locals()
                   
    _combHHH = CombineParticles()
    _combHHH.DecayDescriptor = '[D+ -> pi- pi+ pi+]cc' 
    _combHHH.DaughtersCuts = { "pi+" : '(' + _DaughterCuts + ')' } 
    _combHHH.CombinationCut = '(' + _Combcuts_HHH + ')' 
    _combHHH.MotherCut = '(' + _Mothercuts_HHH + ' & ' + _cutsMassPosFit  + ')' 


    return Selection ( name,
                       Algorithm = _combHHH,
                       RequiredSelections = [StdNoPIDsPions])


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

