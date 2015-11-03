"""
Stripping options for (pre-)selecting D*+ - > D0 (pi0 h h KS) pi+

Authors: Maurizio Martinelli
"""

########################################################################
__author__ = ['Maurizio Martinelli']
__date__ = '22/05/2015'
__version__ = '$Revision: 1.0 $'

__all__ = ('DstarD2KSHHPi0Lines',
           'default_config')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles, DaVinci__N4BodyDecays
from StandardParticles                     import StdAllNoPIDsPions, StdNoPIDsPions, StdNoPIDsKaons, StdLooseResolvedPi0, StdLooseMergedPi0, StdLooseKsDD, StdLooseKsLL

from PhysSelPython.Wrappers      import Selection, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig

from Configurables import FilterDesktop

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm


default_config = {
    'NAME'        : 'DstarD2KSHHPi0',
    'WGs'         : ['Charm'],
    'BUILDERTYPE' : 'DstarD2KSHHPi0Lines',
    'CONFIG'      : { 'PrescaleDstarD2PiPiPi0KSLL' : 1,
                      'PrescaleDstarD2KPiPi0KSLL'  : 1,
                      'PrescaleDstarD2KKPi0KSLL'   : 1,
                      'PrescaleDstarD2PiPiPi0KSDD' : 1,
                      'PrescaleDstarD2KPiPi0KSDD'  : 1,
                      'PrescaleDstarD2KKPi0KSDD'   : 1,
                      # Pi0
                      'Pi0MassWin'              : 16 * MeV,
                      'Pi0PtMin'                : 1000 * MeV,
                      'Pi0PMin'                 : 2500 * MeV,
                      'PhotonCL'                : 0.25,
                      # KS (DD)
                      'MinDz_DD'                : 0 * mm,
                      'MaxDz_DD'                : 9999 * mm,
                      'KSCutDIRA_DD'            : 0.999,
                      'KSCutMass_DD'            : 50 * MeV,
                      'KSCutFDChi2_DD'          : 5,
                      # KS (LL)
                      'MinDz_LL'                : 0 * mm,
                      'MaxDz_LL'                : 9999 * mm,
                      'KSCutDIRA_LL'            : 0.999,
                      'KSCutMass_LL'            : 35 * MeV,
                      'KSCutFDChi2_LL'          : 5,
                      # D0 -> HHHH
                      'TrChi2'                  : 3,
                      'TrGhostProb'             : 0.5,
                      'MinTrkPT'                : 250 * MeV,
                      'MinTrkIPChi2'            : 3,
                      'HighPIDK'                : 0,
                      'LowPIDK'                 : 0,
                      'MaxADOCACHI2'            : 10.0,
                      'CombMassLow'             : 1700 * MeV,
                      'CombMassHigh'            : 2100 * MeV,
                      'MinCombPT'               : 0.0,
                      'MaxVCHI2NDOF'            : 12.0,
                      'MinBPVDIRA'              : 0.9998,
                      'MinBPVTAU'               : 0.1 * picosecond,
                      'MassLow'                 : 1790 * MeV,
                      'MassHigh'                : 1940 * MeV,
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

class DstarD2KSHHPi0Lines( LineBuilder ) :
    """Class defining the D*+ -> (D0 -> pi0 h h KS) pi+ stripping lines"""

    __configuration_keys__ = default_config['CONFIG'].keys()

    def __init__( self,name,config ) :

        LineBuilder.__init__(self, name, config)

        # Lists of Pi0, Gammas
        self.selPi0Resolved = Selection( "SelPi0Resolvedfor" + name,
                                         Algorithm = self.Pi0ResolvedFilter("Pi0Resolvedfor"+name, config),
                                         RequiredSelections = [StdLooseResolvedPi0])
        self.selPi0Merged = Selection( "SelPi0Mergedfor" + name,
                                       Algorithm = self.Pi0MergedFilter("Pi0Mergedfor"+name, config),
                                       RequiredSelections = [StdLooseMergedPi0])
        self.AllPi0s = MergedSelection("SelAllPi0sFor"+name,
                                       RequiredSelections = [ self.selPi0Resolved,self.selPi0Merged])

        self.KS = {'DD': Selection( "SelKSDDFor"+name,
                                    Algorithm = self.KSFilter('KSDDFor'+name,
                                                              config['MinDz_DD'],
                                                              config['MaxDz_DD'],
                                                              config['KSCutDIRA_DD'],
                                                              config['KSCutMass_DD'],
                                                              config['KSCutFDChi2_DD']),
                                    RequiredSelections = [StdLooseKsDD]),
                   'LL': Selection( "SelKSLLFor"+name,
                                    Algorithm = self.KSFilter('KSLLFor'+name,
                                                              config['MinDz_LL'],
                                                              config['MaxDz_LL'],
                                                              config['KSCutDIRA_LL'],
                                                              config['KSCutMass_LL'],
                                                              config['KSCutFDChi2_LL']),
                                    RequiredSelections = [StdLooseKsLL]) }

        self.D2HHHH, self.Dst2D0pi, self.lineDstarD2HHHH = {}, {}, {}

        decays = { 'PiPiPi0KS'  : ['D0 -> pi+ pi- KS0 pi0'],
                   'KPiPi0KS'   : ['D0 -> K- pi+ KS0 pi0', 'D0 -> K+ pi- KS0 pi0'],
                   'KKPi0KS'    : ['D0 -> K+ K- KS0 pi0'],
                     }

        for decay, decayDescriptors in decays.iteritems():
            # make the various stripping selections
            DstarD2KSHHPi0Name   = name + "DstarD2" + decay
            D2KSHHPi0Name   = name + "D2" + decay

            am34 = (134.97 + 497.614) * MeV
            am4 = 134.97 * MeV
            inputs = [ StdNoPIDsPions, self.AllPi0s ] if decays == 'PiPiPi0KS' else [ StdNoPIDsPions, StdNoPIDsKaons, self.AllPi0s ]

            # use both LL and DD KS
            for ksName, ksCands in self.KS.iteritems():
                # Create the D0 candidate
                self.D2HHHH[decay+ksName] = self.makeD2HHHH(D2KSHHPi0Name+ksName,
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
                                                            config['MassLow'],
                                                            config['MassHigh'],
                                                            config['MaxVCHI2NDOF'],
                                                            config['MinBPVDIRA'],
                                                            config['MinBPVTAU'],
                                                            inputs+[ksCands]
                                                            )

                # Create the D* candidate
                self.Dst2D0pi[decay+ksName] = self.makeDstar2D0Pi(DstarD2KSHHPi0Name+ksName,
                                                                  config,
                                                                  ['D*(2010)+ -> D0 pi+', 'D*(2010)- -> D0 pi-'],
                                                                  inputSel = [self.D2HHHH[decay+ksName],    StdAllNoPIDsPions] )

                # Create the stripping line
                self.lineDstarD2HHHH[decay+ksName] = StrippingLine(DstarD2KSHHPi0Name+ksName+"Line",
                                                                   prescale  = config['PrescaleDstarD2'+decay+ksName],
                                                                   selection = self.Dst2D0pi[decay+ksName],
                                                                   HLT1 = config['Hlt1Filter'],
                                                                   HLT2 = config['Hlt2Filter'] )

                # Register the line
                self.registerLine(self.lineDstarD2HHHH[decay+ksName])
        # end loop on decay modes

    def Pi0ResolvedFilter( self, _name, _config):
        _code = ( "  ( ADMASS('pi0') <  %(Pi0MassWin)s )" \
                  "& ( PT> %(Pi0PtMin)s ) " \
                  "& ( P> %(Pi0PMin)s )" \
                  "& ( CHILD(CL,1)> %(PhotonCL)s) " \
                  "& ( CHILD(CL,2)> %(PhotonCL)s) " % _config )
        _pil = FilterDesktop( name = _name, Code = _code )
        return _pil

    def Pi0MergedFilter( self, _name, _config):
        _code = "(PT> %(Pi0PtMin)s *MeV) & (P> %(Pi0PMin)s *MeV)" % _config
        _pil = FilterDesktop( name = _name, Code = _code )
        return _pil

    def KSFilter(self, _name,
                 minDz, maxDz,
                 KSCutDIRA, KSCutMass, KSCutFDChi2 ) :
        _code = " (BPVVDZ > %(minDz)s ) " \
                "&(BPVVDZ < %(maxDz)s ) " \
                "&(BPVDIRA > %(KSCutDIRA)s ) " \
                "&(ADMASS('KS0') < %(KSCutMass)s ) " \
                "&(BPVVDCHI2> %(KSCutFDChi2)s )" % locals()
        _KsFilter = FilterDesktop(name = _name, Code = _code)
        return _KsFilter

    def makeD2HHHH( self, name, decayDescriptors,
                   trChi2,trGhostProb,minPT,minIPChi2,
                   highPIDK, lowPIDK,
                   am34, am4, amMin, amMax,
                   maxDocaChi2, minCombPT,
                   vmMin, vmMax, maxVChi, minbpvDira, minLT,
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
          minPT           : minimum 4-body combination PT
          maxDocaChi2     : maximum 2-tracks DocaChi2
          mMin            : minimum vertex mass
          mMax            : maximum vertex mass
          maxVChi         : maximum vertex chi2
          minbpvDira      : minimum DIRA wrt best PV
          minLT           : minimum lifetime wrt best PV
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
                              "&( (APT1+APT2+APT3+APT4) > %(minPT)s )" \
                              "&( ACHI2DOCA(1,4) < %(maxDocaChi2)s ) " \
                              "&( ACHI2DOCA(2,4) < %(maxDocaChi2)s ) " \
                              "&( ACHI2DOCA(3,4) < %(maxDocaChi2)s ) " %locals() )
        _mother_cuts = (" (in_range( %(vmMin)s, M, %(vmMax)s )) " \
                        "&(VFASPF(VCHI2PDOF) < %(maxVChi)s)" \
                        "&(BPVDIRA > %(minbpvDira)s )" \
                        "&(BPVLTIME() > %(minLT)s )" %locals() )

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
