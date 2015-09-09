"""
Stripping options for (pre-)selecting D(s)+ -> h h pi pi0

Authors: Maurizio Martinelli
"""

########################################################################
__author__ = ['Maurizio Martinelli']
__date__ = '16/05/2015'
__version__ = '$Revision: 0.1 $'

__all__ = ('D2HHHPi0Lines',
           'default_config')

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from Configurables                         import DaVinci__N4BodyDecays
from StandardParticles                     import StdNoPIDsPions, StdNoPIDsKaons
from StandardParticles                     import StdLooseResolvedPi0, StdLooseMergedPi0

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig
from Configurables               import SubstitutePID, FilterDesktop

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm


default_config = {
    'NAME'        : 'D2HHHPi0',
    'WGs'         : ['Charm'],
    'BUILDERTYPE' : 'D2HHHPi0Lines',
    'CONFIG'      : { 'PrescaleD2PiPiPiPi0R'    : 1,
                      'PrescaleD2KPiPiPi0R'     : 1,
                      'PrescaleD2KKPiPi0R'      : 1,
                      'PrescaleD2PiPiPiPi0M'    : 1,
                      'PrescaleD2KPiPiPi0M'     : 1,
                      'PrescaleD2KKPiPi0M'      : 1,
                      # Pi0 (R)
                      'MinMass_Pi0R'            : 120,
                      'MaxMass_Pi0R'            : 150,
                      'MinPT_Pi0R'              : 1500 * MeV,
                      # Pi0 (M)
                      'MinPT_Pi0M'              : 2000 * MeV,
                      # D(s)+ -> HHHPi0
                      'TrChi2'                  : 3,
                      'TrGhostProb'             : 0.5,
                      'MinTrkPT'                : 300 * MeV,
                      'MinTrkIPChi2'            : 3,
                      'HighPIDK'                : 0,
                      'LowPIDK'                 : 0,
                      'MaxADOCACHI2'            : 10.0,
                      'CombMassLow'             : 1750 * MeV,
                      'CombMassHigh'            : 2100 * MeV,
                      'MinCombPT'               : 3.0 * GeV,
                      'MaxVCHI2NDOF'            : 12.0,
                      'MinBPVDIRA'              : 0.9998,
                      'MinBPVTAU'               : 0.1 * picosecond,
                      'MassLow'                 : 1790 * MeV,
                      'MassHigh'                : 2060 * MeV,
                      # HLT filters, only process events firing triggers matching the RegEx
                      'Hlt1Filter': None,
                      'Hlt2Filter': None,
                    },
    'STREAMS'     : ['Charm']
    }

class D2HHHPi0Lines( LineBuilder ) :
    """Class defining the D(s)+ -> h h pi pi0 stripping lines"""
    
    __configuration_keys__ = default_config['CONFIG'].keys()
    
    def __init__( self,name,config ) :        
        
        LineBuilder.__init__(self, name, config)
        
        self.D2HHHPi0, self.lineD2HHHPi0 = {}, {}
        
        decays = { 'PiPiPiPi0'  : ['[D+ -> pi+ pi- pi+ pi0]cc'],
                   'KPiPiPi0'   : ['[D+ -> K- pi+ pi+ pi0]cc', '[D+ -> K+ pi- pi+ pi0]cc'],
                   'KKPiPi0'    : ['[D+ -> K+ K- pi+ pi0]cc', '[D+ -> K+ K+ pi- pi0]cc'],
                     }

        Piz = {'R': self.makePiz('Pi0RFor'+name,
                                [StdLooseResolvedPi0],
                                config['MinPT_Pi0R'],
                                config['MinMass_Pi0R'],
                                config['MaxMass_Pi0R']),
              'M': self.makePiz('Pi0MFor'+name,
                                [StdLooseMergedPi0],
                                config['MinPT_Pi0M']) }

        for decay, decayDescriptors in decays.iteritems():
            # make the various stripping selections
            D2HHHPi0Name   = name + "D2" + decay
        
            am34 = (139.57 + 134.9766) * MeV
            am4 = 134.9766 * MeV
            inputs = [ StdNoPIDsPions ] if decays == 'PiPiPiPi0' else [ StdNoPIDsPions, StdNoPIDsKaons ]
        
            # use both LL and DD KS
            for pizName, pizCands in Piz.iteritems():
            
                # Create the D+ candidate
                self.D2HHHPi0[decay+pizName] = self.makeD2HHHPi0(D2HHHPi0Name+pizName,
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
                                                              inputs+[pizCands]
                                                              )
        
                # Create the stripping line
                self.lineD2HHHPi0[decay+pizName] = StrippingLine(D2HHHPi0Name+pizName+"Line",
                                                               prescale  = config['PrescaleD2'+decay+pizName],
                                                               selection = self.D2HHHPi0[decay+pizName],
                                                               HLT1 = config['Hlt1Filter'],
                                                               HLT2 = config['Hlt2Filter'] )
        
                # Register the line
                self.registerLine(self.lineD2HHHPi0[decay+pizName])
            # end loop on piz types
        # end loop on decay modes

    def makePiz(self, name, inputSel,
                minPT, mMin=0, mMax=0) :
        """
            Given a set of input candidates, refine the list to create good Pi0 candidates
            Used for both Merged and Resolved Pi0.
        """
        _code = " (PT > %(minPT)s ) " % locals()
        if mMin != 0 or mMax != 0:
            _code = ( "( in_range( %(mMin)s* MeV, M, %(mMax)s )) & "  % locals() )+ _code
        _PizFilter = FilterDesktop(name = 'PizFilter_'+name,
                                   Code = _code)
        return Selection (name,
                          Algorithm = _PizFilter,
                          RequiredSelections = inputSel)

    def makeD2HHHPi0( self, name, decayDescriptors,
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
                              "&( (APT1+APT2+APT3+APT4) > %(minPT)s )" %locals() )
                              #"&( ACHI2DOCA(1,4) < %(maxDocaChi2)s ) " \ #removed in s23c since it has no effect on pi0 (ACHI2DOCA is always set to 0)
                              #"&( ACHI2DOCA(2,4) < %(maxDocaChi2)s ) " \
                              #"&( ACHI2DOCA(3,4) < %(maxDocaChi2)s ) " %locals() )
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

