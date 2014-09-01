#!/usr/bin/env python
'''
Module for construction of Ds->KKPi based on 
 StrippingD2hhh_conf.py stripping Selections and StrippingLines.
Provides functions to build Ds2KKPi selections using StdVeryTightDsplus2KKPi.
Provides class Ds2KKpiConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - Ds2KKpiConf
   - makeStdDs2KKpi
'''

__author__ = ['Marco Pappagallo']
__date__ = '26/08/2014'
__version__ = '$Revision: 1.0 $'

__all__ = ('Ds2KKpiConf',
           'makeStdDs2KKpi',
           'default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

## Test:  Yes
default_config = {
    'NAME'        : 'FullDSTDs2KKpi',
    'BUILDERTYPE' : 'Ds2KKpiConf' ,
    'CONFIG' : {
                'Daughter2IPChi2'           :   10.,
                'PTSum'                     : 2800.,
                'DsPt'                      : 1000.,
                'DsIPChi2'                  :    9.,
                'DsVXChi2NDOF'              :   10.,
                'MinMassPhi'                : 1010.,
                'MaxMassPhi'                : 1030.,
                'MinMassKstar'              :    0., # 836
                'MaxMassKstar'              :    0., # 956
                'piPIDK'                    :    3.,
                'KPIDK'                     :    7.,
                'DsDIRA'                    :    0., 
                'MinCosHelAngleKstar'       :    2., # 0.4
                'MaxTracksInEvent'          :  500,
                'Ds2KKPLinePrescale'        :    1.0,
                'Ds2KKPLinePostscale'       :    1.0
                },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }


class Ds2KKpiConf(LineBuilder) :
    """
    Builder of Ds ->KKPi stripping Selection and StrippingLine.
    Constructs Ds ->KKPI Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> D2HHHConf = Ds2KKpiConf('D2HHHtest',config)
    >>> D2hhhLines = D2HHHConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selKKPi            :  get StdVeryTightDsplus2KKPi
    Ds2KKPi_line       :  StrippingLine made out of selKKPi 
    lines              :  List of lines [Ds2KKPi_line]

    Exports as class data member:
    Ds2KKpiConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('Daughter2IPChi2',
                              'PTSum',
                              'DsPt',
                              'DsIPChi2',
                              'DsVXChi2NDOF',
                              'MinMassPhi'  ,
                              'MaxMassPhi'  ,
                              'MinMassKstar',
                              'MaxMassKstar',
                              'piPIDK'      ,
                              'KPIDK'       ,
                              'DsDIRA'      ,
                              'MinCosHelAngleKstar',
                              'MaxTracksInEvent',
                              'Ds2KKPLinePrescale',
                              'Ds2KKPLinePostscale'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        self.selKKPi = DataOnDemand(Location = "Phys/StdVeryTightDsplus2KKPi/Particles")

        self.selDs2KKPi = makeStdDs2KKpi(name,  
                                         KKPiSel = self.selKKPi, 
                                         Daughter2IPChi2 = config['Daughter2IPChi2'],
                                         PTSum = config['PTSum'],
                                         DsIPChi2= config['DsIPChi2'],
                                         DsPt = config['DsPt'],
                                         DsVXChi2NDOF = config['DsVXChi2NDOF'],
                                         MinMassPhi   = config['MinMassPhi'],
                                         MaxMassPhi   = config['MaxMassPhi'],
                                         MinMassKstar = config['MinMassKstar'],
                                         MaxMassKstar = config['MaxMassKstar'],
                                         piPIDK       = config['piPIDK'],
                                         KPIDK        = config['KPIDK'],
                                         DsDIRA       = config['DsDIRA'],
                                         MinCosHelAngleKstar = config['MinCosHelAngleKstar']
                                         )

        self.filterGE = globalEventCutFilter(name + 'GECFilter', 
                                    MaxTrSIZE = config['MaxTracksInEvent'])

        self.Ds2KKPi_line = StrippingLine(name+"_Line",
                                        prescale = config['Ds2KKPLinePrescale'],
                                        postscale = config['Ds2KKPLinePostscale'],
                                        selection = self.selDs2KKPi,
                                        FILTER = self.filterGE,
                                        )

        self.registerLine(self.Ds2KKPi_line)

def makeStdDs2KKpi(name,
                   KKPiSel,
                   Daughter2IPChi2, 
                   PTSum,
                   DsIPChi2,
                   DsPt,
                   DsVXChi2NDOF,
                   MinMassPhi,
                   MaxMassPhi,
                   MinMassKstar,
                   MaxMassKstar,
                   piPIDK,
                   KPIDK,
                   DsDIRA,
                   MinCosHelAngleKstar
                   ):
    """
    Create and return a Ds -> KKPi Selection object.
    Arguments:
    name            : name of the Selection.
    KKPiSel         : Standard Specific Ds2KKPi selection    
    Daughter2IPChi2 : Minimum IPChi2 required to at least 2 daughters 
    PTSum           : Minimum sum of daughters momenta
    DsIPChi2        : Maximum IPChi2 of the Ds
    DsPt            : Minimum Ds Momentum
    DsVXChi2NDOF    : Maximum Chi2 of the Ds Vertex
    MinMassPhi      : Lower bound for phi mass
    MaxMassPhi      : Upper bound for phi mass   
    MinMassKstar    : Lower bound for K*(892) mass
    MaxMassKstar    : Upper bound for K*(892) mass
    piPIDK          : Kaon - pion DLL for pions
    KPIDK           : Kaon - pion DLL for kaons
    DsDIRA          : Cosine angle between Ds momentum and FD-direction
    MinCosHelAngleKstar: Cosine of the helicity angle in the K*(892) rest frame
    """

    _Combcuts =""" 
               (SUMTREE( ISBASIC , PT ) > %(PTSum)s*MeV) &
               (2 <= NINGENERATION((MIPCHI2DV(PRIMARY) > %(Daughter2IPChi2)s ) , 1)) 
               """ % locals()
                    
    _Mothercuts = """
                  (PT > %(DsPt)s) & (VFASPF(VCHI2/VDOF) < %(DsVXChi2NDOF)s) &
                  (BPVDIRA > %(DsDIRA)s) & (BPVIPCHI2() < %(DsIPChi2)s)
                  """ % locals()

    _DaughterCuts = "(MAXTREE('pi+'==ABSID, PIDK-PIDpi) < %(piPIDK)s) & (MINTREE('K-'==ABSID, PIDK-PIDpi) > %(KPIDK)s ) " % locals()

    # M12 --> phi(1020)
    # M13 --> K*(892)0

    _cutsDP_phi = "(in_range ( %(MinMassPhi)s* MeV , M12 , %(MaxMassPhi)s* MeV ))" % locals()
    _cutsDP_Kst = "(in_range ( %(MinMassKstar)s* MeV , M13 , %(MaxMassKstar)s* MeV ) & (abs(COSPOL('[D_s+ -> ^K- K+ pi+]CC', '[D_s+ -> K- K+ ^pi+]CC', False)) > %(MinCosHelAngleKstar)s))"  % locals()
    _cutsDP = '('+_cutsDP_phi + '|' +_cutsDP_Kst + ')'

    _code = '('+_DaughterCuts+'&'+_cutsDP+'&'+_Combcuts+'&'+_Mothercuts+')'
 
    _filter = FilterDesktop( Code = _code ) 
   
    return Selection ( name,
                       Algorithm = _filter,
                       RequiredSelections = [KKPiSel])

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

