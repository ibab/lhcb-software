"""
Stripping options for (pre-)selecting D*+ - > D0 (pi h h h) pi+

Authors: Maurizio Martinelli, Maxime Schubiger
"""

########################################################################
__author__ = ['Maurizio Martinelli', 'Maxime Schubiger']
__date__ = '07/05/2015'
__version__ = '$Revision: 1.0 $'

__all__ = ('DstarD2HHHHLines',
           'default_config')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles, DaVinci__N4BodyDecays
from StandardParticles                     import StdAllNoPIDsPions, StdNoPIDsPions, StdNoPIDsKaons

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm


default_config = {
    'NAME'        : 'DstarD2HHHH',
    'WGs'         : ['Charm'],
    'BUILDERTYPE' : 'DstarD2HHHHLines',
    'CONFIG'      : { 'PrescaleDstarD2PiPiPiPi' : 1,
                      'PrescaleDstarD2KPiPiPi'  : 1,
                      'PrescaleDstarD2KKPiPi'   : 1,
                      'PrescaleDstarD2KKKPi'    : 1,
                      # D0 -> HHHH
                      'TrChi2'                  : 3,
                      'TrGhostProb'             : 0.5,
                      'MinTrkPT'                : 250 * MeV,
                      'MinTrkIPChi2'            : 3,
                      'HighPIDK'                : 0,
                      'LowPIDK'                 : 0,
                      'MaxADOCACHI2'            : 10.0,
                      'CombMassLow'             : 1730 * MeV,
                      'CombMassHigh'            : 2000 * MeV,
                      'MinCombPT'               : 1500 * MeV,
                      'MinCombP'                : 20000 * MeV,
                      'MaxVCHI2NDOF'            : 12.0,
                      'MinBPVDIRA'              : 0.9998,
                      'MinBPVTAU'               : 0.1 * picosecond,
                      'MassLow'                 : 1790 * MeV,
                      'MassHigh'                : 1940 * MeV,
                      'MinDPT'                  : 1600 * MeV,
                      'MinDP'                   : 25000 * MeV,
                      # D* -> D0 pi+
                      'Daug_TRCHI2DOF_MAX'      : 3,
                      'Dstar_AMDiff_MAX'        : 190 * MeV,
                      'Dstar_VCHI2VDOF_MAX'     : 15.0,
                      'Dstar_MDiff_MAX'         : 170 * MeV,
                      # HLT filters, only process events firing triggers matching the RegEx
                      'Hlt1Filter': None,
                      'Hlt2Filter': None,
                    },
    'STREAMS'     : ['Charm']
    }

class DstarD2HHHHLines( LineBuilder ) :
    """Class defining the D*+ -> (D0 -> pi h h h) pi+ stripping lines"""

    __configuration_keys__ = default_config['CONFIG'].keys()

    def __init__( self,name,config ) :

        LineBuilder.__init__(self, name, config)

        self.D2HHHH, self.Dst2D0pi, self.lineDstarD2HHHH = {}, {}, {}

        decays = { 'PiPiPiPi'  : ['D0 -> pi+ pi- pi+ pi-'],
                   'KPiPiPi'   : ['D0 -> K- pi+ pi+ pi-', 'D0 -> K+ pi- pi+ pi-'],
                   'KKPiPi'    : ['D0 -> K+ K- pi+ pi-'],
                   'KKKPi'     : ['D0 -> K- K+ K- pi+', 'D0 -> K+ K+ K- pi-']
                     }

        for decay, decayDescriptors in decays.iteritems():
            # make the various stripping selections
            DstarD2HHHHName   = name + "DstarD2" + decay
            D2HHHHName   = name + "D2" + decay

            am34 = (139.57 + (493.677 if decay == 'KKKPi' else 139.57) ) * MeV
            am4 = 139.57 * MeV
            inputs = [ StdNoPIDsPions] if decays == 'PiPiPiPi' else [ StdNoPIDsPions, StdNoPIDsKaons ]

            # Create the D0 candidate
            self.D2HHHH[decay] = self.makeD2HHHH(D2HHHHName,
                                            decayDescriptors,
                                            config['TrChi2'],
                                            config['TrGhostProb'],
                                            config['MinTrkPT'],
                                            config['MinTrkIPChi2'],
                                            config['HighPIDK'],
                                            config['LowPIDK'],
                                            am34,
                                            am4,
                                            config['CombMassLow'],
                                            config['CombMassHigh'],
                                            config['MaxADOCACHI2'],
                                            config['MinCombPT'],
                                            config['MinCombP'],
                                            config['MassLow'],
                                            config['MassHigh'],
                                            config['MaxVCHI2NDOF'],
                                            config['MinBPVDIRA'],
                                            config['MinBPVTAU'],
                                            config['MinDPT'],
                                            config['MinDP'],
                                            inputs
                                            )

            # Create the D* candidate
            self.Dst2D0pi[decay] = self.makeDstar2D0Pi(DstarD2HHHHName,
                                                  config,
                                                  ['D*(2010)+ -> D0 pi+', 'D*(2010)- -> D0 pi-'],
                                                  inputSel = [self.D2HHHH[decay], StdAllNoPIDsPions] )

            # Create the stripping line
            self.lineDstarD2HHHH[decay] = StrippingLine(DstarD2HHHHName+"Line",
                                                        prescale  = config['PrescaleDstarD2'+decay],
                                                        selection = self.Dst2D0pi[decay],
                                                        HLT1 = config['Hlt1Filter'],
                                                        HLT2 = config['Hlt2Filter'] )

            # Register the line
            self.registerLine(self.lineDstarD2HHHH[decay])
        # end loop on decay modes



    def makeD2HHHH( self, name, decayDescriptors,
                   trChi2,trGhostProb,minPT,minIPChi2,
                   highPIDK, lowPIDK,
                   am34, am4, amMin, amMax,
                   maxDocaChi2, minCombPT, minCombP,
                   vmMin, vmMax, maxVChi, minbpvDira, minLT, minDPT, minDP,
                   inputSel = [ StdNoPIDsPions, StdNoPIDsKaons ]) :
        """
          Create and return a D0 -> HHHH (H=K,pi) Selection object.
          Arguments:
          name            : name of the Selection.
          DecayDescriptor : DecayDescriptor.
          trChi2          : maximum tracks chi2
          trGhostProb     : maximum tracks ghost probability
          minPT           : minimum tracks PT
          minIPChi2       : minimum tracks IPChi2
          highPIDK        : maximum PIDk for pions
          lowPIDK         : minimum PIDk for kaons
          am34            : phase space limit on 2-body combinations mass
          am4             : phase space limit on 3-body combinations mass
          amMin           : minimum 4-body combination mass
          amMax           : maximum 4-body combination mass
          minCombPT       : minimum 4-body combination PT
          minCombP        : minimum 4-body combination P
          maxDocaChi2     : maximum 2-tracks DocaChi2
          mMin            : minimum vertex mass
          mMax            : maximum vertex mass
          maxVChi         : maximum vertex chi2
          minbpvDira      : minimum DIRA wrt best PV
          minLT           : minimum lifetime wrt best PV
          minDPT          : minimum D PT
          minDP           : minimum D P
          inputSel        : input selections
        """

        _daughters_cuts = " (TRGHOSTPROB < %(trGhostProb)s)" \
                          "&(TRCHI2DOF < %(trChi2)s)" \
                          "&(PT > %(minPT)s)" \
                          "&(MIPCHI2DV(PRIMARY) > %(minIPChi2)s )" %locals()
        _pidPi = "&(PIDK < %(highPIDK)s)" %locals()
        _pidK  = "&(PIDK > %(lowPIDK)s)" %locals()


        _c12_cuts = (" ( AM < (%(amMax)s - %(am34)s) ) " \
                     "&( ACHI2DOCA(1,2) < %(maxDocaChi2)s ) " %locals() )
        _c123_cuts =(" ( AM < (%(amMax)s - %(am4)s) ) " \
                     "&( ACHI2DOCA(1,3) < %(maxDocaChi2)s ) " \
                     "&( ACHI2DOCA(2,3) < %(maxDocaChi2)s ) " %locals() )
        _combination_cuts =  (" (in_range( %(amMin)s, AM, %(amMax)s )) " \
                              "&( (APT1+APT2+APT3+APT4) > %(minCombPT)s )" \
                              "&( AP > %(minCombP)s )" \
                              "&( ACHI2DOCA(1,4) < %(maxDocaChi2)s ) " \
                              "&( ACHI2DOCA(2,4) < %(maxDocaChi2)s ) " \
                              "&( ACHI2DOCA(3,4) < %(maxDocaChi2)s ) " %locals() )
        _mother_cuts = (" (in_range( %(vmMin)s, M, %(vmMax)s )) " \
                        "&(VFASPF(VCHI2PDOF) < %(maxVChi)s)" \
                        "&(BPVDIRA > %(minbpvDira)s )" \
                        "&(BPVLTIME() > %(minLT)s )" \
                        "&(PT > %(minDPT)s )" \
                        "&(P  > %(minDP)s )" %locals() )

        CombineD2HHHH = DaVinci__N4BodyDecays(DecayDescriptors = decayDescriptors,
                                              DaughtersCuts = { "pi+" : _daughters_cuts+_pidPi, "K+" : _daughters_cuts+_pidK },
                                              Combination12Cut = _c12_cuts, Combination123Cut = _c123_cuts,
                                              CombinationCut = _combination_cuts,
                                              MotherCut = _mother_cuts)

        return Selection(name,
                         Algorithm = CombineD2HHHH,
                         RequiredSelections = inputSel )


    def makeDstar2D0Pi(self, name,
                       config,
                       DecayDescriptors,
                       inputSel) :
        """
          Create and return a D* -> D0 pi Selection object.
          Arguments:
          name        : name of the Selection.
          config      : dictionary of cut values.
          DecayDescriptor: DecayDescriptor.
          inputSel    : input selections
        """
        daugCuts = "(TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" % locals()['config']
        combCuts = "((AM - AM1) < %(Dstar_AMDiff_MAX)s)" % locals()['config']
        dstarCuts = " (VFASPF(VCHI2/VDOF) < %(Dstar_VCHI2VDOF_MAX)s)" \
                    "&((M - M1) < %(Dstar_MDiff_MAX)s)" % locals()['config']

        _Dstar = CombineParticles(DecayDescriptors = DecayDescriptors,
                                  DaughtersCuts = { "pi+" : daugCuts },
                                  CombinationCut = combCuts,
                                  MotherCut = dstarCuts )

        return Selection(name+'Sel',
                         Algorithm = _Dstar,
                         RequiredSelections = inputSel
                         )


########################################################################
