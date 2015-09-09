"""
Stripping options for (pre-)selecting D(s)+ -> h h pi KS

Authors: Maurizio Martinelli
"""

########################################################################
__author__ = ['Maurizio Martinelli']
__date__ = '15/05/2015'
__version__ = '$Revision: 0.1 $'

__all__ = ('D2HHHKsLines',
           'default_config')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from Configurables                         import DaVinci__N4BodyDecays
from StandardParticles                     import StdNoPIDsPions, StdNoPIDsKaons
from StandardParticles                     import StdLooseKsDD, StdLooseKsLL

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig
from Configurables               import SubstitutePID, FilterDesktop

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm


default_config = {
    'NAME'        : 'D2HHHKs',
    'WGs'         : ['Charm'],
    'BUILDERTYPE' : 'D2HHHKsLines',
    'CONFIG'      : { 'PrescaleD2PiPiPiKsDD'    : 1,
                      'PrescaleD2KPiPiKsDD'     : 1,
                      'PrescaleD2KKPiKsDD'      : 1,
                      'PrescaleD2PiPiPiKsLL'    : 1,
                      'PrescaleD2KPiPiKsLL'     : 1,
                      'PrescaleD2KKPiKsLL'      : 1,
                      # KS (DD)
                      'MinKsPT_DD'              : 300 * MeV,
                      'MaxKsVCHI2NDOF_DD'       : 20.0,
                      'MinDz_DD'                : 250. * mm,
                      'MaxDz_DD'                : 9999 * mm,
                      'MinKsIpChi2_DD'          : 3,
                      'KSCutDIRA_DD'            : 0.999,
                      'KSCutMass_DD'            : 50 * MeV,
                      'KSCutFDChi2_DD'          : 5,
                      # KS (LL)
                      'MinKsPT_LL'              : 300 * MeV,
                      'MaxKsVCHI2NDOF_LL'       : 20.0,
                      'MinDz_LL'                : 0 * mm,
                      'MaxDz_LL'                : 9999 * mm,
                      'MinKsIpChi2_LL'          : 3,
                      'KSCutDIRA_LL'            : 0.999,
                      'KSCutMass_LL'            : 35 * MeV,
                      'KSCutFDChi2_LL'          : 5,
                      # D(s)+ -> HHHKS
                      'TrChi2'                  : 3,
                      'TrGhostProb'             : 0.5,
                      'MinTrkPT'                : 250 * MeV,
                      'MinTrkIPChi2'            : 3,
                      'HighPIDK'                : 0,
                      'LowPIDK'                 : 0,
                      'MaxADOCACHI2'            : 10.0,
                      'CombMassLow'             : 1750 * MeV,
                      'CombMassHigh'            : 2100 * MeV,
                      'MinCombPT'               : 1.5,
                      'MaxVCHI2NDOF'            : 12.0,
                      'MinBPVDIRA'              : 0.9998,
                      'MinBPVTAU'               : 0.1 * picosecond,
                      'MassLow'                 : 1790 * MeV,
                      'MassHigh'                : 2050 * MeV,
                      # HLT filters, only process events firing triggers matching the RegEx
                      'Hlt1Filter': None,
                      'Hlt2Filter': None,
                    },
    'STREAMS'     : ['Charm']
    }

class D2HHHKsLines( LineBuilder ) :
    """Class defining the D(s)+ -> h h pi KS stripping lines"""
    
    __configuration_keys__ = default_config['CONFIG'].keys()
    
    def __init__( self,name,config ) :        
        
        LineBuilder.__init__(self, name, config)
        
        self.D2HHHKs, self.lineD2HHHKs = {}, {}
        
        decays = { 'PiPiPiKs'  : ['[D+ -> pi+ pi- pi+ KS0]cc'],
                   'KPiPiKs'   : ['[D+ -> K- pi+ pi+ KS0]cc', '[D+ -> K+ pi- pi+ KS0]cc'],
                   'KKPiKs'    : ['[D+ -> K+ K- pi+ KS0]cc', '[D+ -> K+ K+ pi- KS0]cc'],
                     }

        KS = {'DD': self.makeKS('KSDDFor'+name,
                                config['MinKsPT_DD'],
                                config['MaxKsVCHI2NDOF_DD'],
                                config['MinDz_DD'],
                                config['MaxDz_DD'],
                                config['MinKsIpChi2_DD'],
                                config['KSCutDIRA_DD'],
                                config['KSCutMass_DD'],
                                config['KSCutFDChi2_DD'],
                                [StdLooseKsDD]),
              'LL': self.makeKS('KSLLFor'+name,
                                config['MinKsPT_LL'],
                                config['MaxKsVCHI2NDOF_LL'],
                                config['MinDz_LL'],
                                config['MaxDz_LL'],
                                config['MinKsIpChi2_LL'],
                                config['KSCutDIRA_LL'],
                                config['KSCutMass_LL'],
                                config['KSCutFDChi2_LL'],
                                [StdLooseKsLL]) }

        for decay, decayDescriptors in decays.iteritems():
            # make the various stripping selections
            D2HHHKsName   = name + "D2" + decay
        
            am34 = (139.57 + 493.614) * MeV
            am4 = 493.614 * MeV
            inputs = [ StdNoPIDsPions ] if decays == 'PiPiPiKs' else [ StdNoPIDsPions, StdNoPIDsKaons ]
        
            # use both LL and DD KS
            for ksName, ksCands in KS.iteritems():
            
                # Create the D+ candidate
                self.D2HHHKs[decay+ksName] = self.makeD2HHHKs(D2HHHKsName+ksName,
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
        
                # Create the stripping line
                self.lineD2HHHKs[decay+ksName] = StrippingLine(D2HHHKsName+ksName+"Line",
                                                               prescale  = config['PrescaleD2'+decay+ksName],
                                                               selection = self.D2HHHKs[decay+ksName],
                                                               HLT1 = config['Hlt1Filter'],
                                                               HLT2 = config['Hlt2Filter'] )
        
                # Register the line
                self.registerLine(self.lineD2HHHKs[decay+ksName])
            # end loop on ks types
        # end loop on decay modes

    def makeKS(self, name,
               KSPT, KSVtxChi2,
               minDz, maxDz, minIPChi2,
               KSCutDIRA, KSCutMass, KSCutFDChi2,
               inputSel) :
        """
            Given a set of input candidates, refine the list to create good KS candidates
            Used for both long and downstream tracks.
        """
        _code = " (PT > %(KSPT)s)" \
                "&(VFASPF(VCHI2PDOF) < %(KSVtxChi2)s)" \
                "&(BPVVDZ > %(minDz)s ) " \
                "&(BPVVDZ < %(maxDz)s ) " \
                "&(BPVDIRA > %(KSCutDIRA)s ) " \
                "&(MIPCHI2DV(PRIMARY) > %(minIPChi2)s) "\
                "&(ADMASS('KS0') < %(KSCutMass)s ) " \
                "&(BPVVDCHI2> %(KSCutFDChi2)s )" % locals()
        _KsFilter = FilterDesktop(name = 'KSFilter_'+name,
                                  Code = _code)
        return Selection (name,
                          Algorithm = _KsFilter,
                          RequiredSelections = inputSel)

    def makeD2HHHKs( self, name, decayDescriptors,
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
        _mother_cuts = (" (in_range( %(vmMin)s* MeV, M, %(vmMax)s )) " \
                        "&(VFASPF(VCHI2PDOF) < %(maxVChi)s)" \
                        "&(BPVDIRA > %(minbpvDira)s )" \
                        "&(BPVLTIME() > %(minLT)s )" %locals() )

        CombineD2HHHKs = DaVinci__N4BodyDecays(DecayDescriptors = decayDescriptors,
                                              DaughtersCuts = { "pi+" : _daughters_cuts+_pidPi, "K+" : _daughters_cuts+_pidK },
                                              Combination12Cut = _c12_cuts, Combination123Cut = _c123_cuts,
                                              CombinationCut = _combination_cuts,
                                              MotherCut = _mother_cuts)
    
        return Selection(name,
                         Algorithm = CombineD2HHHKs,
                         RequiredSelections = inputSel )


########################################################################  

