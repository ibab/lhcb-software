"""
Module for construction of Bs/Bd->Jpsi KS K pi stripping Selections and StrippingLines.
Provides functions to build KS->DD, KS->LL, Bs selections.
Provides class Bs2JpsiKSKpi, which constructs the Selections and StrippingLines
given a configuration dictionary.
Selections based on previous version of the line by Jussara Miranda.
Exported symbols (use python help!):
   - B2JpsiKShhConf
"""

__author__ = ['Matthew M Reid']
__date__ = '31/01/2012'
__version__ = 'Stripping20'
__all__ = 'B2JpsiKShhConf'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdLoosePions as Pions

################################################################################################################################################
# Define the dictionary of cuts
################################################################################################################################################
default_config = {'Trk_Chi2'                 : 4.0,
                  'Jpsi_MassWindow'          : 80.0, # 80 by default is the StdMassConstrainedJpsi2MuMu mass window
                  'KS_DD_MassWindow'         : 30.0,
                  'KS_DD_VtxChi2'            : 18.0,
                  'KS_DD_DLS'                : 5.0,
                  'KS_LL_MassWindow'         : 30.0,
                  'KS_LL_VtxChi2'            : 20.0,
                  'KS_LL_DLS'                : 5.0,
                  'Bs_Mlow'                  : 520.0,
                  'Bs_Mhigh'                 : 750.0,
                  'BsDaug_DD_maxDocaChi2'    : 5.0,
                  'BsDaug_LL_maxDocaChi2'    : 5.0,
                  'Bs_PT'                    : 600.0,
                  'Bs_VtxChi2'               : 9.0,
                  'Bs_Dira'                  : 0.99992,
                  'Bs_LL_IPChi2PV'           : 75.0,
                  'Bs_DD_IPChi2PV'           : 75.0,
                  'Prescale'                 : 1.0,
                  'Prescale_SameSign'        : 1.0,
                  'Postscale'                : 1.0
                  }

################################################################################################################################################
# Selection Class
################################################################################################################################################
class B2JpsiKShhConf(LineBuilder) :
    """
    Builder of B -> JpsiKKpi stripping Selection and StrippingLine.
    Constructs B -> Jpsi KS K pi  Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> b2jpiskshhConf = B2JpsiKShhConf('B2JpsiKShh',config)
    >>> b2jpsikskpiLines = b2jpsikshhConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selJpsi                     : Jpsi -> mu mu Selection object
    selKS2DD                    : KS -> Down Down Selection object
    selKS2LL                    : KS -> Long Long Selection object
    selB2JpsiKSDDhh             : B -> Jpsi KS(DD) h h Selection object
    selB2JpsiKSLLhh             : B -> Jpsi KS(LL) h h Selection object
    selB2JpsiKSDDhh_SameSign    : B -> Jpsi KS(DD) h+(-) h+(-) Selection object
    selB2JpsiKSLLhh_SameSign    : B -> Jpsi KS(LL) h+(-) h+(-) Selection object
    dd_line                     : StrippingLine made out of selB2JpsiKSDDhh
    ll_line                     : StrippingLine made out of selB2JpsiKSLLhh
    dd_line_same                : StrippingLine made out of selB2JpsiKSDDhh_SameSign
    ll_line_same                : StrippingLine made out of selB2JpsiKSLLhh_SameSign
    lines                       : List of lines, [dd_line, ll_line, dd_line_same, ll_line_same]

    Exports as class data member:
    B2KShhConf.__configuration_keys__ : List of required configuration parameters.
    """
    __configuration_keys__ = ('Trk_Chi2'
                            , 'Jpsi_MassWindow'
                            , 'KS_DD_MassWindow'
                            , 'KS_DD_VtxChi2'
                            , 'KS_DD_DLS'
                            , 'KS_LL_MassWindow'
                            , 'KS_LL_VtxChi2'
                            , 'KS_LL_DLS'
                            , 'Bs_Mlow'
                            , 'Bs_Mhigh'
                            , 'BsDaug_DD_maxDocaChi2'
                            , 'BsDaug_LL_maxDocaChi2'
                            , 'Bs_PT'
                            , 'Bs_VtxChi2'
                            , 'Bs_Dira'
                            , 'Bs_LL_IPChi2PV'
                            , 'Bs_DD_IPChi2PV'
                            , 'Prescale'
                            , 'Prescale_SameSign'
                            , 'Postscale')

################################################################################################################################################
# Initialise class and build lines
################################################################################################################################################
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        dd_name = name+'DD'
        ll_name = name+'LL'

        # Same sign names
        dd_name_same = name+'DD'+'SameSign'
        ll_name_same = name+'LL'+'SameSign'
        #GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
         #          'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.pions = Pions

        self.hadrons = MergedSelection("HadronsFor" + name,
                                       RequiredSelections = [ self.pions ] )
        # make the Jpsi
        self.makeJpsi2MuMu( 'Jpsi2MuMu', config )

        # Make the KS0s
        self.makeKS2DD( 'KSfor'+dd_name, config )
        self.makeKS2LL( 'KSfor'+ll_name, config )


        #To get the Same Sign lines you MUST have the string 'SameSign' in the name.
        self.makeB2JpsiKSDDhh( dd_name_same, config )
        self.makeB2JpsiKSLLhh( ll_name_same, config )

        self.makeB2JpsiKSDDhh( dd_name, config )
        self.makeB2JpsiKSLLhh( ll_name, config )

        self.dd_line = StrippingLine(dd_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection = self.selB2JpsiKSDDhh,
                                     #FILTER = GECCode
                                     )

        self.ll_line = StrippingLine(ll_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection =  self.selB2JpsiKSLLhh
                                     #FILTER = GECCode
                                     )

        self.dd_line_same = StrippingLine(dd_name_same+"Line",
                                     prescale = config['Prescale_SameSign'],
                                     postscale = config['Postscale'],
                                     selection = self.selB2JpsiKSDDhh_SameSign
                                     #HLT = "( HLT_PASS_RE('Hlt1TrackMuonDecision') & HLT_PASS_RE('Hlt1SingleMuonHighPTDecision') & HLT_PASS_RE('Hlt1DiMuonHighMassDecision') & HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2TopoMu2BodyBBDTDecision') & HLT_PASS_RE('Hlt2DiMuonJPsiHighPTDecision') & HLT_PASS_RE('Hlt2DiMuonDetachedDecision') & HLT_PASS_RE('Hlt2DiMuonJPsiDecision') )"
                                     #FILTER = GECCode
                                     )

        self.ll_line_same = StrippingLine(ll_name_same+"Line",
                                     prescale = config['Prescale_SameSign'],
                                     postscale = config['Postscale'],
                                     selection =  self.selB2JpsiKSLLhh_SameSign
                                     #HLT = "( HLT_PASS_RE('Hlt1TrackMuonDecision') & HLT_PASS_RE('Hlt1SingleMuonHighPTDecision') & HLT_PASS_RE('Hlt1DiMuonHighMassDecision') & HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2TopoMu2BodyBBDTDecision') & HLT_PASS_RE('Hlt2DiMuonJPsiHighPTDecision') & HLT_PASS_RE('Hlt2DiMuonDetachedDecision') & HLT_PASS_RE('Hlt2DiMuonJPsiDecision') )"
                                     #FILTER = GECCode
                                     )
        self.registerLine(self.dd_line)
        self.registerLine(self.ll_line)

        self.registerLine(self.dd_line_same)
        self.registerLine(self.ll_line_same)

################################################################################################################################################
# Define functions for the selection
################################################################################################################################################
    def makeKS2DD( self, name, config ) :
        """
        Cuts:
            - KS_DD_MassWindow : mass acceptance of KSDD - have a realistic KSDD
            - KS_DD_VtxChi2    : maximum vertex chi2 of the KS daughters - good fitting vertex
            - KS_DD_DLS        : uses the best primary vertex from the PhysDesktop to calculate the decay length significance
        """
         # define all the cuts
        _massCut = "(ADMASS('KS0')<%s*MeV)"     % config['KS_DD_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2PDOF)<%s)"     % config['KS_DD_VtxChi2']
        _dlsig   = "(BPVDLS>%s)"                % config['KS_DD_DLS']

        _allCuts = _massCut+'&'+_vtxCut+'&'+_dlsig

        # get the KS's to filter
        _stdKSDD = DataOnDemand( Location = "Phys/StdLooseKsDD/Particles" )

        # make the filter
        _filterKSDD = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selKS2DD = Selection( name, Algorithm = _filterKSDD, RequiredSelections = [_stdKSDD] )

################################################################################################################################################
    def makeKS2LL( self, name, config ) :
        """
        Cuts:
            - KS_LL_MassWindow : mass acceptance of KSLL - have a realistic KSLL
            - KS_LL_VtxChi2    : maximum vertex chi2 of the KL daughters - good fitting vertex
            - KS_LL_DLS        : uses the best primary vertex from the PhysDesktop to calculate the decay length significance
            - Trk_Chi2         : maximum track chi2 per dof - only good KLL tracks accteped generally fly out of detector
        """
        # define all the cuts
        _massCut    = "(ADMASS('KS0')<%s*MeV)"          % config['KS_LL_MassWindow']
        _vtxCut     = "(VFASPF(VCHI2PDOF)<%s)"          % config['KS_LL_VtxChi2']
        _dlsig      = "(BPVDLS>%s)"                     % config['KS_LL_DLS']
        _trkChi2Cut1 = "(CHILDCUT((TRCHI2DOF<%s),1))"   % config['Trk_Chi2']
        _trkChi2Cut2 = "(CHILDCUT((TRCHI2DOF<%s),2))"   % config['Trk_Chi2']

        _allCuts = _massCut+'&'+_trkChi2Cut1+'&'+_trkChi2Cut2+'&'+_vtxCut+'&'+_dlsig

        # get the KS's to filter
        _stdKSLL = DataOnDemand( Location = "Phys/StdLooseKsLL/Particles" )

        # make the filter
        _filterKSLL = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selKS2LL = Selection( name, Algorithm = _filterKSLL, RequiredSelections = [_stdKSLL] )

################################################################################################################################################
    def makeJpsi2MuMu( self, name, config):
        """
            Cuts:
            - Jpsi_MassWindow   : mass acceptance of Jpsi
        """
        # define all the cuts
        _massCut = "(ADMASS('J/psi(1S)')<%s*MeV)" % config['Jpsi_MassWindow']

        _allCuts = _massCut

        # AutomaticData is for data on the DST
        Jpsi_Data = DataOnDemand( Location = "Phys/StdMassConstrainedJpsi2MuMu/Particles" )

        # Filter the Jpsi for tighter cut
        _Jpsi_Filter = FilterDesktop( Code = _allCuts)

        # make a Selection
        self.selJpsi = Selection(name , Algorithm = _Jpsi_Filter, RequiredSelections = [ Jpsi_Data ])

################################################################################################################################################
    def makeB2JpsiKSDDhh( self, name, config ) :
        """
            Create and store a B -> Jpsi KS(DD) h h, or a B -> Jpsi KS(DD) h+(-) h+(-) Same Sign Selection Object.
            Arguments:
            name                    : name of the Selection.
            config                  : config dictionary

            Combination Cuts:
            - Bs_Mlow               : lower mass limit - cut on lower mass side band
            - Bs_Mhigh              : upper mass limit - cut on upper mass side band
            - BsDaug_DD_maxDocaChi2 : chi2 between all possible pairs of particles is less than cut

            Mother Cuts:
            - Bs_VtxChi2            : applies chi2 per ndof cut to the vertex of the Bs
            - Bs_Dira               : minimum cosine of the angle between the momentum of the particle and the direction of flight from the best PV to the decay vertex.
            - Bs_PT                 : minimum value of transverse momentum.
            - Bs_DD_IPChi2PV        : significance of the B IP with respect to the best PV.
            """
        # Define Combination cuts, LoKi array functors as applited to array of daughters (begin with A)
        _massCutLow     = "(AM>(5336-%s)*MeV)"               % config['Bs_Mlow']
        _massCutHigh    = "(AM<(5336+%s)*MeV)"               % config['Bs_Mhigh']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['BsDaug_DD_maxDocaChi2']

        _combCuts = _massCutLow+"&"+_massCutHigh+"&"+_maxDocaChi2Cut

        # Define Mother cuts
        _vtxChi2Cut = "(VFASPF(VCHI2PDOF)<%s)"         % config['Bs_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['Bs_Dira']
        _ptCut      = "(PT>%s*MeV)"                    % config['Bs_PT']
        _ipChi2PV   = "(BPVIPCHI2()<%s)"               % config['Bs_DD_IPChi2PV']

        _motherCuts = _vtxChi2Cut+'&'+_diraCut+'&'+_ptCut+'&'+_ipChi2PV

        _B = CombineParticles()
        #_B.DaughtersCuts = { "K+" : "(TRCHI2DOF<%s)"% config['Trk_Chi2'], "K+" : " (MIPCHI2DV(PRIMARY)> %s)"% config['Daug_Chi2AFPV'], "pi+" : "(TRCHI2DOF<%s)"% config['Trk_Chi2'], "pi+" : " (MIPCHI2DV(PRIMARY)> %s)"% config['Daug_Chi2AFPV']}
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        if "SameSign" in name:
            _B.DecayDescriptors = [ "B_s0 -> J/psi(1S) KS0 pi+ pi+", "B_s0 -> J/psi(1S) KS0 pi- pi-" ]
            self.selB2JpsiKSDDhh_SameSign = Selection (name, Algorithm = _B, RequiredSelections = [ self.selJpsi, self.selKS2DD, self.hadrons ])

        else:
            _B.DecayDescriptors = [ "B_s0 -> J/psi(1S) KS0 pi+ pi-" ]
            self.selB2JpsiKSDDhh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selJpsi, self.selKS2DD, self.hadrons ])

################################################################################################################################################
    def makeB2JpsiKSLLhh( self, name, config ) :
        """
            Create and store a B -> Jpsi KS(LL) h h, or a B -> Jpsi KS(LL) h+(-) h+(-) Same Sign Selection Object.
            Arguments:
            name                    : name of the Selection.
            config                  : config dictionary

            Combination Cuts:
            - Bs_Mlow               : lower mass limit - cut on lower mass side band
            - Bs_Mhigh              : upper mass limit - cut on upper mass side band
            - BsDaug_LL_maxDocaChi2 : chi2 between all possible pairs of particles is less than cut

            Mother Cuts:
            - Bs_VtxChi2            : applies chi2 per ndof cut to the vertex of the Bs
            - Bs_Dira               : minimum cosine of the angle between the momentum of the particle and the direction of flight from the best PV to the decay vertex.
            - Bs_PT                 : minimum value of transverse momentum.
            - Bs_LL_IPChi2PV        : significance of the B IP with respect to the best PV.
        """
        # Define Combination cuts, LoKi array functors as applited to array of daughters (begin with A)
        _massCutLow     = "(AM>(5336-%s)*MeV)"               % config['Bs_Mlow']
        _massCutHigh    = "(AM<(5336+%s)*MeV)"               % config['Bs_Mhigh']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['BsDaug_LL_maxDocaChi2']

        _combCuts = _massCutLow+"&"+_massCutHigh+"&"+_maxDocaChi2Cut

        # Define Mother cuts
        _vtxChi2Cut = "(VFASPF(VCHI2PDOF)<%s)"             % config['Bs_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                       % config['Bs_Dira']
        _ptCut      = "(PT>%s*MeV)"                        % config['Bs_PT']
        _ipChi2PV   = "(BPVIPCHI2()<%s)"                   % config['Bs_LL_IPChi2PV']

        _motherCuts = _vtxChi2Cut+'&'+_diraCut+'&'+_ptCut+'&'+_ipChi2PV

        _B = CombineParticles()
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        if "SameSign" in name:
             _B.DecayDescriptors = [ "B_s0 -> J/psi(1S) KS0 pi+ pi+", "B_s0 -> J/psi(1S) KS0 pi- pi-" ]
             self.selB2JpsiKSLLhh_SameSign = Selection (name, Algorithm = _B, RequiredSelections = [ self.selJpsi, self.selKS2LL, self.hadrons ] )

        else:
            _B.DecayDescriptors = [ "B_s0 -> J/psi(1S) KS0 pi+ pi-" ]
            self.selB2JpsiKSLLhh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selJpsi, self.selKS2LL, self.hadrons ] )
