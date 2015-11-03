"""
Stripping options for (pre-)selecting D*+ - > D0 (h h gamma) pi+

Authors: Maurizio Martinelli
"""

########################################################################
__author__ = ['Maurizio Martinelli']
__date__ = '15/05/2015'
__version__ = '$Revision: 0.1 $'

__all__ = ('DstarD2XGammaLines',
           'default_config')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from StandardParticles                     import StdAllNoPIDsPions, StdNoPIDsPions, StdNoPIDsKaons
from StandardParticles                     import StdLooseAllPhotons, StdAllLooseGammaLL, StdAllLooseGammaDD

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig

from Configurables import FilterDesktop

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm

default_config = {
    'NAME'        : 'DstarD2XGamma',
    'WGs'         : ['Charm'],
    'BUILDERTYPE' : 'DstarD2XGammaLines',
    'CONFIG'      : { 'PrescaleDstarD2PiPiGamma': 1,
                      'PrescaleDstarD2KPiGamma' : 1,
                      'PrescaleDstarD2KKGamma'  : 1,
                      'PrescaleDstarD2PiPiGamma_CNVLL': 1,
                      'PrescaleDstarD2KPiGamma_CNVLL' : 1,
                      'PrescaleDstarD2KKGamma_CNVLL'  : 1,
                      'PrescaleDstarD2PiPiGamma_CNVDD': 1,
                      'PrescaleDstarD2KPiGamma_CNVDD' : 1,
                      'PrescaleDstarD2KKGamma_CNVDD'  : 1,
                      # Gamma
                      'photonPT'                : 1.8 * GeV,
                      'MaxMass_CNV_LL'          : 100 * MeV,
                      'MaxVCHI2_CNV_LL'         : 9,
                      'MinPT_CNV_LL'            : 1000 * MeV,
                      'MaxMass_CNV_DD'          : 100 * MeV,
                      'MaxVCHI2_CNV_DD'         : 9,
                      'MinPT_CNV_DD'            : 1000 * MeV,
                      # X -> HH
                      'TrChi2'                  : 3,
                      'TrGhostProb'             : 0.5,
                      'MinTrkPT'                : 250 * MeV,
                      'MinTrkIPChi2'            : 3,
                      'HighPIDK'                : 0,
                      'LowPIDK'                 : 0,
                      'CombMassLow_HH'          : 0 * MeV,
                      'CombMassHigh_HH'         : 1820 * MeV,
                      'MinCombPT'               : 1.5 * GeV,
                      'MaxADOCACHI2'            : 10.0,
                      'MinVDCHI2_HHComb'        : 1000.0,
                      'MassLow_HH'              : 0 * MeV,
                      'MassHigh_HH'             : 1810 * MeV,
                      'MaxVCHI2NDOF_HH'         : 16.0,
                      'MinVDCHI2_HH'            : 1000.0,
                      # D0 -> X Gamma
                      'CombMassLow'             : 1665 * MeV,
                      'CombMassHigh'            : 2075 * MeV,
                      'MassLow'                 : 1685 * MeV,
                      'MassHigh'                : 2055 * MeV,
                      'MinPT'                   : 1.8 * GeV,
                      'MaxVCHI2NDOF'            : 12.0,
                      'MinBPVDIRA'              : 0.9998,
                      'MinBPVTAU'               : 0.1 * picosecond,
                      # D* -> D0 pi+
                      'Daug_TRCHI2DOF_MAX'      : 3,
                      'Dstar_AMDiff_MAX'        : 180 * MeV,
                      'Dstar_VCHI2VDOF_MAX'     : 15.0,
                      'Dstar_MDiff_MAX'         : 160 * MeV,
                      # HLT filters, only process events firing triggers matching the RegEx
                      'Hlt1Filter': None,
                      'Hlt2Filter': None,
                    },
    'STREAMS'     : ['CharmCompleteEvent']
    }

class DstarD2XGammaLines( LineBuilder ) :
    """Class defining the D*+ -> (D0 -> (X->h h) gamma) pi+ stripping lines"""

    __configuration_keys__ = default_config['CONFIG'].keys()

    def __init__( self,name,config ) :

        LineBuilder.__init__(self, name, config)

        # Select photons
        self.selPhotons = {'': self.makePhoton('GammaFor'+name,
                                                config['photonPT']),
                           '_CNVLL': self.makePhotonConv('CNVGammaLLFor'+name,
                                                        config['MaxMass_CNV_LL'],
                                                        config['MaxVCHI2_CNV_LL'],
                                                        config['MinPT_CNV_LL'],
                                                        [StdAllLooseGammaLL]),
                           '_CNVDD': self.makePhotonConv('CNVGammaDDFor'+name,
                                                        config['MaxMass_CNV_DD'],
                                                        config['MaxVCHI2_CNV_DD'],
                                                        config['MinPT_CNV_DD'],
                                                        [StdAllLooseGammaDD]) }

        self.HHForD2XGamma, self.D2XGamma, self.Dst2D0pi, self.lineDstarD2XGamma = {}, {}, {}, {}

        decays = { 'PiPiGamma'  : {'HHdesc': ['rho(770)0 -> pi+ pi-'], 'D0desc': ['D0 -> rho(770)0 gamma'] },
                   'KPiGamma'   : {'HHdesc': ['K*(892)0 -> K- pi+', 'K*(892)0 -> K+ pi-'], 'D0desc': ['D0 -> K*(892)0 gamma'] },
                   'KKGamma'    : {'HHdesc': ['phi(1020) -> K+ K-'], 'D0desc': ['D0 -> phi(1020) gamma'] },
                     }

        for decay, decayDescriptors in decays.iteritems():
            # make the various stripping selections
            DstarD2XGammaName   = name + "DstarD2" + decay
            D2XGammaName   = name + "D2" + decay
            HHCombName   = name + "HHForD2" + decay

            inputs = [ StdNoPIDsPions] if decays == 'PiPiGamma' else [ StdNoPIDsPions, StdNoPIDsKaons ]

            # Create the 2-body candidate
            self.HHForD2XGamma[decay] = self.makeHH(HHCombName,
                                                     decayDescriptors['HHdesc'],
                                                     config['TrChi2'],
                                                     config['TrGhostProb'],
                                                     config['MinTrkPT'],
                                                     config['MinTrkIPChi2'],
                                                     config['HighPIDK'],
                                                     config['LowPIDK'],
                                                     config['CombMassLow_HH'], # FIXME!!!!
                                                     config['CombMassHigh_HH'],
                                                     config['MinCombPT'],
                                                     config['MaxADOCACHI2'],
                                                     config['MinVDCHI2_HHComb'],
                                                     config['MassLow_HH'],
                                                     config['MassHigh_HH'],
                                                     config['MaxVCHI2NDOF_HH'],
                                                     config['MinVDCHI2_HH'],
                                                     inputs
                                                     )

            for phName, photons in self.selPhotons.iteritems():

                # Create the D0 candidate
                self.D2XGamma[decay+phName] = self.makeD2XGamma(D2XGammaName+phName,
                                                                decayDescriptors['D0desc'],
                                                                config['CombMassLow'],
                                                                config['CombMassHigh'],
                                                                config['MassLow'],
                                                                config['MassHigh'],
                                                                config['MinPT'],
                                                                config['MaxVCHI2NDOF'],
                                                                config['MinBPVDIRA'],
                                                                config['MinBPVTAU'],
                                                                inputSel = [self.HHForD2XGamma[decay], photons]
                                                                )

                # Create the D* candidate
                self.Dst2D0pi[decay+phName] = self.makeDstar2D0Pi(DstarD2XGammaName+phName,
                                                                  config,
                                                                  ['D*(2010)+ -> D0 pi+', 'D*(2010)- -> D0 pi-'],
                                                                  inputSel = [self.D2XGamma[decay+phName], StdAllNoPIDsPions] )

                # Create the stripping line
                self.lineDstarD2XGamma[decay+phName] = StrippingLine(DstarD2XGammaName+phName+"Line",
                                                                     prescale  = config['PrescaleDstarD2'+decay+phName],
                                                                     selection = self.Dst2D0pi[decay+phName],
                                                                     HLT1 = config['Hlt1Filter'],
                                                                     HLT2 = config['Hlt2Filter'] )

                # Register the line
                self.registerLine(self.lineDstarD2XGamma[decay+phName])
        # end loop on decay modes

    def makePhoton(self, name, photonPT):
        """Create photon Selection object starting from DataOnDemand 'Phys/StdLooseAllPhotons'.

        @arg name: name of the Selection.
        @arg photonPT: PT of the photon

        @return: Selection object

        """
        # Prepare selection
        _code = "(PT> %(photonPT)s*MeV)" % locals()
        _gammaFilter = FilterDesktop(name = "GammaFilter_"+name, Code=_code)
        _stdGamma = StdLooseAllPhotons
        return Selection(name, Algorithm=_gammaFilter, RequiredSelections=[_stdGamma])

    def makePhotonConv(self, name,
                       mMax, maxVChi, minPT,
                       inputSel):
        """
            Create photon selection objects starting from converted photons list
                (StdAllLooseGammaLL, StdAllLooseGammaDD)
        """
        _code = " ( MM < %(mMax)s * MeV ) " \
                "&( HASVERTEX ) " \
                "&( VFASPF(VCHI2/VDOF)<%(maxVChi)s ) " \
                "&( PT >  %(minPT)s)" % locals()

        _filter = FilterDesktop(name = "ConvertedGammaFilter_"+name,
                                Code = _code)

        return Selection(name,
                         Algorithm = _filter,
                         RequiredSelections = inputSel )

        _DDConvSel = FilterDesktop(Code = "(MM < 100*MeV) & (HASVERTEX) & (VFASPF(VCHI2/VDOF)<9) & (PT >  %(B2XGGammaCNVPTMin)s)" % self.__confdict__
)

        self.ConvLLPhoton = Selection( 'ConvLLPhoton' + self.name,
                                       Algorithm = _LLConvSel,
                                       RequiredSelections = [StdAllLooseGammaLL])
        self.ConvDDPhoton = Selection( 'ConvDDPhoton' + self.name,
                                       Algorithm = _DDConvSel,
                                       RequiredSelections = [StdAllLooseGammaDD])

    def makeHH(self, name, decayDescriptors,
               trChi2,trGhostProb,minTrkPT,minIPChi2,
               highPIDK, lowPIDK,
               amMin, amMax, minPT, maxDocaChi2, minVDChi2comb,
               vmMin, vmMax, maxVChi, minVDChi2, inputSel) :
        """
          Create and return a X -> HH (H=K,pi) Selection object.
          Arguments:
          name             : name of the Selection.
          DecayDescriptors : DecayDescriptors.
          trChi2           : maximum tracks chi2
          trGhostProb      : maximum tracks ghost probability
          minPT            : minimum tracks PT
          minIPChi2        : minimum tracks IPChi2
          highPIDK         : maximum PIDk for pions
          lowPIDK          : minimum PIDk for kaons
          amMin            : minimum combination mass
          amMax            : maximum combination mass
          minPT            : minimum combination PT
          maxDocaChi2      : maximum 2-tracks DocaChi2
          minVDChi2comb    : minimum vertex distance chi2
          vmMax            : maximum vertex mass
          maxVChi          : maximum vertex chi2
          minVDChi2        : minimum vertex distance chi2
          inputSel         : input selections
        """

        _daughters_cuts = " (TRGHOSTPROB < %(trGhostProb)s)" \
                          "&(TRCHI2DOF < %(trChi2)s)" \
                          "&(PT > %(minTrkPT)s)" \
                          "&(MIPCHI2DV(PRIMARY) > %(minIPChi2)s )" %locals()
        _pidPi = "&(PIDK < %(highPIDK)s)" %locals()
        _pidK  = "&(PIDK > %(lowPIDK)s)" %locals()

        _combination_cuts =  (" (in_range( %(amMin)s, AM, %(amMax)s ) ) " \
                              "&( (APT1+APT2) > %(minPT)s )"  \
                              "&( ACHI2DOCA(1,2) < %(maxDocaChi2)s )" %locals() )

        _mother_cuts = (" (in_range( %(vmMin)s, M, %(vmMax)s )) " \
                        "&(VFASPF(VCHI2PDOF) < %(maxVChi)s)" \
                        "&( BPVVDCHI2 > %(minVDChi2)s )" %locals() )

        CombineX2HH = CombineParticles(DecayDescriptors = decayDescriptors,
                                       DaughtersCuts = { "pi+" : _daughters_cuts+_pidPi, "K+" : _daughters_cuts+_pidK },
                                       CombinationCut = _combination_cuts,
                                       MotherCut = _mother_cuts)

        return Selection(name,
                         Algorithm = CombineX2HH,
                         RequiredSelections = inputSel )


    def makeD2XGamma( self, name, decayDescriptors,
                      amMin, amMax,
                      vmMin, vmMax, minPT, maxVChi, minbpvDira, minLT,
                      inputSel ) :
        """
          Create and return a D0 -> X Gamma (X=resonance) Selection object.
          Arguments:
          name            : name of the Selection.
          DecayDescriptors: DecayDescriptors.
          amMin           : minimum combination mass
          amMax           : maximum combination mass
          vmMin           : minimum vertex mass
          vmMax           : maximum vertex mass
          minPT           : minimum vertex PT
          maxVChi         : maximum vertex chi2
          minLT           : minimum lifetime wrt best PV
          inputSel        : input selections
        """

        _combination_cuts =  (" (in_range( %(amMin)s, AM, %(amMax)s )) " %locals() )
        _mother_cuts = (" (in_range( %(vmMin)s* MeV, M, %(vmMax)s )) " \
                        "&(PT > %(minPT)s)" \
                        "&(VFASPF(VCHI2PDOF) < %(maxVChi)s)" \
                        "&(BPVDIRA > %(minbpvDira)s )" \
                        "&(BPVLTIME() > %(minLT)s )" %locals() )

        CombineD2XGamma = CombineParticles(DecayDescriptors = decayDescriptors,
                                           CombinationCut = _combination_cuts,
                                           MotherCut = _mother_cuts)

        return Selection(name,
                         Algorithm = CombineD2XGamma,
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
