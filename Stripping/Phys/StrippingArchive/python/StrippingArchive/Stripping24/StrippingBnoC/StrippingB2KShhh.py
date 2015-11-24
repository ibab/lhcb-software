'''
Module for construction of B->KShhh
   KSpipipi
   KSKpipi
   KSKKpi
   KSKKK
'''
########################################################################

__author__ = ['Maurizio Martinelli']
__date__ = '3/11/2015'
__version__ = '$Revision: 1.0 $'

__all__ = ('B2KShhhConf',
           'default_config'
           )

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles, DaVinci__N4BodyDecays
from StandardParticles                     import StdNoPIDsPions, StdNoPIDsKaons # ipchi2 cut
from StandardParticles                     import StdLooseKsDD, StdLooseKsLL

from PhysSelPython.Wrappers      import Selection, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig

from Configurables import FilterDesktop

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm


default_config = {
    'NAME'        : 'B2KShhh',
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'B2KSHHHLines',
    'CONFIG'      : { 'PrescaleB2PiPiPiKSLL' : 1,
                      'PrescaleB2KPiPiKSLL'  : 1,
                      'PrescaleB2KKPiKSLL'   : 1,
                      'PrescaleB2KKKKSLL'    : 1,
                      'PrescaleB2PiPiPiKSDD' : 1,
                      'PrescaleB2KPiPiKSDD'  : 1,
                      'PrescaleB2KKPiKSDD'   : 1,
                      'PrescaleB2KKKKSDD'    : 1,
                      # KS (DD)
                      'MinDz_DD'             : 0 * mm,
                      'MaxDz_DD'             : 9999 * mm,
                      'KSCutDIRA_DD'         : 0.999,
                      'KSCutMass_DD'         : 50 * MeV,
                      'KSCutFDChi2_DD'       : 5,
                      # KS (LL)
                      'MinDz_LL'             : 0 * mm,
                      'MaxDz_LL'             : 9999 * mm,
                      'KSCutDIRA_LL'         : 0.999,
                      'KSCutMass_LL'         : 35 * MeV,
                      'KSCutFDChi2_LL'       : 5,
                      # B -> KSHHH
                      'TrChi2'               : 4,
                      'TrGhostProb'          : 0.4,
                      'MinTrkPT'             : 500 * MeV,
                      'MinTrkP'              : 1.5 * GeV,
                      'MinTrkIPChi2'         : 4,
                      'HighPIDK'             : 0,
                      'LowPIDK'              : 0,
                      'MaxADOCACHI2'         : 10.0,
                      'CombMassLow'          : 4500 * MeV,
                      'CombMassHigh'         : 7200 * MeV,
                      'MinCombPT'            : 1.5 * GeV,
                      'MinVPT'               : 1.5 * GeV,
                      'MaxVCHI2NDOF'         : 12.0,
                      'MinBPVDIRA'           : 0.99995,
                      'MinBPVTAU'            : 0.1 * picosecond,
                      'MassLow'              : 4700 * MeV,
                      'MassHigh'             : 7000 * MeV,
                      # HLT filters, only process events firing triggers matching the RegEx
                      # 2012 Triggers
                      #'Hlt1Filter'          : 'Hlt1TrackAllL0Decision',
                      #'Hlt2Filter'          : 'Hlt2Topo[234]Body.*Decision',
                      # 2015 Triggers
                      'Hlt1Filter'           : 'Hlt1(Two)?TrackMVADecision',
                      'Hlt2Filter'           : 'Hlt2Topo[234]BodyDecision',
                    },
    'STREAMS'     : ['Bhadron']
    }

class B2KSHHHLines( LineBuilder ) :
    """Class defining the B -> h h h KS stripping lines"""

    __configuration_keys__ = default_config['CONFIG'].keys()

    def __init__( self,name,config ) :

        LineBuilder.__init__(self, name, config)

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

        self.B2KSHHH, self.lineB2KSHHH = {}, {}

        decays = { 'PiPiPiKS'   : ['[B+ -> pi+ pi- pi+ KS0]cc'],
                   'KPiPiKS'    : ['[B+ -> pi+ K- pi+ KS0]cc', '[B+ -> pi+ pi- K+ KS0]cc'],
                   'KKPiKS'     : ['[B+ -> pi+ K- K+ KS0]cc', '[B+ -> K+ pi- K+ KS0]cc'],
                   'KKKKS'      : ['[B+ -> K+ K- K+ KS0]cc'],
                     }

        for decay, decayDescriptors in decays.iteritems():
            # make the various stripping selections
            B2KSHHHName   = name + "_" + decay.replace('KS','_')

            amKSPi = (134.97 + 497.611) * MeV
            amKSK = (497.614 + 497.611 ) * MeV
            amKS = 497.611 * MeV
            am34 = amKSPi if decay == 'PiPiPiKS' else amKSK
            inputs = [ StdNoPIDsPions ] if decay == 'PiPiPiKS' else [ StdNoPIDsPions, StdNoPIDsKaons ]

            # use both LL and DD KS
            for ksName, ksCands in self.KS.iteritems():
                # Create the B candidate
                self.B2KSHHH[decay+ksName] = self.makeB2KSHHH(B2KSHHHName+ksName,
                                                            decayDescriptors,
                                                            config['TrChi2'],
                                                            config['TrGhostProb'],
                                                            config['MinTrkPT'],
                                                            config['MinTrkP'],
                                                            config['MinTrkIPChi2'],
                                                            config['HighPIDK'],
                                                            config['LowPIDK'],
                                                            am34,
                                                            amKS,
                                                            config['CombMassLow'],
                                                            config['CombMassHigh'],
                                                            config['MaxADOCACHI2'],
                                                            config['MinCombPT'],
                                                            config['MassLow'],
                                                            config['MassHigh'],
                                                            config['MinVPT'],
                                                            config['MaxVCHI2NDOF'],
                                                            config['MinBPVDIRA'],
                                                            config['MinBPVTAU'],
                                                            inputs+[ksCands]
                                                            )

                # Create the stripping line
                Hlt1Filter = {'Code' : "HLT_PASS_RE('%s')" % config['Hlt1Filter'],
                              'Preambulo' : ["from LoKiCore.functions import *"]} if config['Hlt1Filter'] != None else None

                Hlt2Filter = {'Code' : "HLT_PASS_RE('%s')" % config['Hlt2Filter'],
                              'Preambulo' : ["from LoKiCore.functions import *"]} if config['Hlt2Filter'] != None else None

                RelInfo = self.getRelInfo()
                self.lineB2KSHHH[decay+ksName] = StrippingLine(B2KSHHHName+ksName+"Line",
                                                            prescale  = config['PrescaleB2'+decay+ksName],
                                                            selection = self.B2KSHHH[decay+ksName],
                                                            HLT1 = Hlt1Filter,
                                                            HLT2 = Hlt2Filter,
                                                            RelatedInfoTools = RelInfo )

                # Register the line
                self.registerLine(self.lineB2KSHHH[decay+ksName])
        # end loop on decay modes

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

    def makeB2KSHHH( self, name, decayDescriptors,
                   trChi2,trGhostProb,minPT,minP,minIPChi2,
                   highPIDK, lowPIDK,
                   am34, am4, amMin, amMax,
                   maxDocaChi2, minCombPT,
                   vmMin, vmMax, minVPT,  maxVChi, minbpvDira, minLT,
                   inputSel = [ StdNoPIDsPions, StdNoPIDsKaons ]) :
        """
          Create and return a B -> KSHHH (H=K,pi) Selection object.
          Arguments:
          name            : name of the Selection.
          DecayDescriptor : DecayDescriptor.
          trChi2          : maximum tracks chi2
          trGhostProb     : maximum tracks ghost probability
          minPT           : minimum tracks PT
          minP           : minimum tracks P
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
          minVPT          : minimum vertex PT
          maxVChi         : maximum vertex chi2
          minbpvDira      : minimum DIRA wrt best PV
          minLT           : minimum lifetime wrt best PV
          inputSel        : input selections
        """

        _daughters_cuts = " (TRGHOSTPROB < %(trGhostProb)s)" \
                          "&(TRCHI2DOF < %(trChi2)s)" \
                          "&(PT > %(minPT)s)" \
                          "&(P > %(minP)s)" \
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
                        "&(PT > %(minVPT)s)" \
                        "&(VFASPF(VCHI2PDOF) < %(maxVChi)s)" \
                        "&(BPVDIRA > %(minbpvDira)s )" \
                        "&(BPVLTIME() > %(minLT)s )" %locals() )

        CombineB2KSHHH = DaVinci__N4BodyDecays(DecayDescriptors = decayDescriptors,
                                              DaughtersCuts = { "pi+" : _daughters_cuts+_pidPi, "K+" : _daughters_cuts+_pidK },
                                              Combination12Cut = _c12_cuts, Combination123Cut = _c123_cuts,
                                              CombinationCut = _combination_cuts,
                                              MotherCut = _mother_cuts)

        return Selection(name,
                         Algorithm = CombineB2KSHHH,
                         RequiredSelections = inputSel )

    def getRelInfo(self):
        relInfo = []
        for coneAngle in [0.8,1.0,1.3,1.7]:
            relInfo += [
            { "Type"         : "RelInfoConeVariables",
              "ConeAngle"    : coneAngle,
              "Variables"    : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
              "DaughterLocations" : { "[B+ -> ^X+ X- X+ KS0]CC" : 'P2ConeVar%s_1' % str(coneAngle).replace('.',''),
                                      "[B+ -> X+ ^X- X+ KS0]CC" : 'P2ConeVar%s_2' % str(coneAngle).replace('.',''),
                                      "[B+ -> X+ X- ^X+ KS0]CC" : 'P2ConeVar%s_3' % str(coneAngle).replace('.','') } }
                        ]
        relInfo += [ { "Type" : "RelInfoVertexIsolation", "Location": "VertexIsoInfo" } ]
        return relInfo


########################################################################
