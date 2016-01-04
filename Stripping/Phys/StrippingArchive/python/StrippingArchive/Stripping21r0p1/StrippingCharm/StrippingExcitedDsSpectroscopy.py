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
__date__ = '09/09/2015'
__version__ = '$Revision: 1.0 $'

__all__ = ('DsstGammaConf',
           'makeDs2KKpi',
           'makeDsg',
           'makeDsgg',
           'default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

## Test:  Yes
default_config = {
    'NAME'        : 'ExcitedDsSpectroscopy',
    'BUILDERTYPE' : 'ExcitedDsSpectroscopyConf' ,
    'CONFIG' : {
                'Daughter2IPChi2'           :   10.,
                'PTSum'                     : 2800.,
                'DsPt'                      : 1000.,
                'DsIPChi2'                  :    9.,
                'DsVXChi2NDOF'              :   10.,
                'MinMassPhi'                : 1010.,
                'MaxMassPhi'                : 1030.,
                'MinMassKstar'              :  836., # 836
                'MaxMassKstar'              :  956., # 956
                'piPIDK'                    :    3.,
                'KPIDK'                     :    7.,
                'DsDIRA'                    :    0., 
                'MinCosHelAngleKstar'       :    0.4, # 0.4
                'MaxTracksInEvent'          :  500,
                'DsstGammaLinePrescale'     :    1.0,
                'DsstGammaLinePostscale'    :    1.0,
                'DsGammaLinePrescale'       :    1.0,
                'DsGammaLinePostscale'      :    1.0,
                'GammaPT'                   :  400.,
                'GammaCL'                   :    0.1,
                'DeltaMassDsstar'           :  450.,
                'DeltaMassDsg'              : 1300.,
                'Gamma2PT'                  :    0.,
                'Gamma2CL'                  :    0.1,
                'DeltaMassDsgg'             :  1300.,
                'MaxMassDsst'               :  2300.
                },
    
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


class ExcitedDsSpectroscopyConf(LineBuilder) :
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
                              'DsstGammaLinePrescale',
                              'DsstGammaLinePostscale',
                              'DsGammaLinePrescale',
                              'DsGammaLinePostscale',
                              'GammaPT',
                              'GammaCL',
                              'DeltaMassDsstar',
                              'DeltaMassDsg',
                              'Gamma2PT',
                              'Gamma2CL',
                              'DeltaMassDsgg',
                              'MaxMassDsst'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        self.selKKPi = DataOnDemand(Location = "Phys/StdVeryTightDsplus2KKPi/Particles")

        self.selDs2KKPi = makeStdDs2KKpi('DsFor%s' % name,
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

#        //self.selPhoton = makePhoton('GammaFor%s' % name)

        self.selDsg  = makeDsg('DsstarFor%s' % name,
                               DsSel = self.selDs2KKPi,
                               GammaPT = config['GammaPT'],
                               GammaCL = config['GammaCL'],
                               DeltaMassDsg = config['DeltaMassDsg']
                               )

        self.selDsg_vetoed = makeDsg_vetoed('Pi0VetoDsgFor%s' % name,
                                            DsgSel = self.selDsg
                                            )
        

        self.selDsgg = makeDsgg('DsggFor%s' % name,
                                DsstSel = self.selDsg_vetoed,
                                Gamma2PT = config['Gamma2PT'],
                                Gamma2CL = config['Gamma2CL'],
                                DeltaMassDsgg = config['DeltaMassDsgg'],
                                MaxMassDsst = config['MaxMassDsst']
                                )

        self.selDsgg_vetoed = makeDsgg_vetoed('Pi0VetoDsggFor%s' % name,
                                              DsggSel = self.selDsgg
                                              )

        self.filterGE = globalEventCutFilter(name + 'GECFilter', 
                                    MaxTrSIZE = config['MaxTracksInEvent'])


        self.Dsg_line = StrippingLine("DsGammaFor"+name+"Line",
                                      prescale = config['DsGammaLinePrescale'],
                                      postscale = config['DsGammaLinePostscale'],
                                      selection = self.selDsg_vetoed,
                                      #selection = self.selDs2KKPi,
                                      #selection = self.selDsgg_vetoed,
                                      FILTER = self.filterGE,
                                      )
        
        self.registerLine(self.Dsg_line)
                
        
        self.Dsgg_line = StrippingLine("DsstGammaFor"+name+"Line",
                                       prescale = config['DsstGammaLinePrescale'],
                                       postscale = config['DsstGammaLinePostscale'],
                                       #selection = self.selDsgg,
                                       #selection = self.selDs2KKPi,
                                       selection = self.selDsgg_vetoed,
                                       FILTER = self.filterGE,
                                       )

        self.registerLine(self.Dsgg_line)
        
def makeDsgg_vetoed(name,
                    DsggSel
                    ):

    from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger
    
    _g = Pi0Veto__Tagger(
        MassWindow     = 25,
        MassChi2       = -1           ,
        ExtraInfoIndex = 25041     ## unique !
        )
    
    return Selection(name, Algorithm=_g, RequiredSelections=[DsggSel])
                        

def makeDsg_vetoed(name,
                   DsgSel
                   ):
    
    from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger
    
    _g = Pi0Veto__Tagger(
        MassWindow     = 25,
        MassChi2       = -1           ,
        ExtraInfoIndex = 25040     ## unique !
        )
    
    return Selection(name, Algorithm=_g, RequiredSelections=[DsgSel])

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


def makeDsg(name,
            DsSel,
            GammaPT,
            GammaCL,
            DeltaMassDsg
            ):
    
    _daughterPhotonCut   = "(PT > %(GammaPT)s) & (CL > %(GammaCL)s)" % locals()
    _combinationCut = "(AM - AM1) < %(DeltaMassDsg)s*MeV" % locals() 
    _motherCut      = "ALL"

    _Dsg = CombineParticles(DecayDescriptor = "[D*_s+ -> D_s+ gamma]cc", 
                            DaughtersCuts = {"gamma"  : _daughterPhotonCut
                                             },
                            CombinationCut = _combinationCut,
                            MotherCut = _motherCut)
    
    _stdPhoton = DataOnDemand(Location="Phys/StdLooseAllPhotons/Particles")
    
    return Selection(name, Algorithm=_Dsg, RequiredSelections=[DsSel, _stdPhoton])

def makeDsgg(name,
             DsstSel,
             Gamma2PT,
             Gamma2CL,
             DeltaMassDsgg,
             MaxMassDsst
             ):

    _daughterDsstCut     = "M < %(MaxMassDsst)s*MeV" % locals()
    _daughterPhotonCut   = "(PT > %(Gamma2PT)s) & (CL > %(Gamma2CL)s)" % locals()
    _combinationCut = "(AM - AM1) < %(DeltaMassDsgg)s*MeV" % locals()
    _motherCut      = "ALL"
    
    _Dsgg = CombineParticles(DecayDescriptor = "[D_s1(2536)+ -> D*_s+ gamma]cc",
                             DaughtersCuts = {"gamma"  : _daughterPhotonCut,
                                              "D*_s+"  : _daughterDsstCut
                                              },
                             CombinationCut = _combinationCut,
                             MotherCut = _motherCut)
    
    _stdPhoton = DataOnDemand(Location="Phys/StdLooseAllPhotons/Particles")
    
    return Selection(name, Algorithm=_Dsgg, RequiredSelections=[DsstSel, _stdPhoton])

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

