'''
B->DDphi, D= D0, D+, Ds+, D*0, D*+ 

Author: M. Martinelli
Notes: Excited states are included by placing the line in FULL.DST
'''
__author__ = ['Maurizio Martinelli']

defaultName = 'B2DDphi'

# Import Packages
from StrippingConf.StrippingLine import StrippingLine
from PhysSelPython.Wrappers import Selection , DataOnDemand
from StrippingUtils.Utils import LineBuilder
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from Configurables import FilterDesktop, CombineParticles, DaVinci__N4BodyDecays

# Default configuration dictionary
default_config = {
    'NAME' : 'B2DDphi',
    'BUILDERTYPE' : 'B2DDphiConf',
    'CONFIG' : {
      # Lines
    'FULLDST': 'D0D0',
    # Cuts made on all bachelor tracks
    'TR_CHI2DOF_MAX' :3,
    'TR_PT_MIN': 100, #MeV
    'TR_P_MIN': 1000, #MeV
    'TR_MIPCHI2DV_MIN': 4,
    'TR_GHP_MAX': 0.4,
    # Cuts made on all KK's
    'KK_MASS_MAX': 2300, #MeV
    'KK_MASS_MAX_D0D0': 2300, #MeV
    'KK_MASS_MAX_DD': 2300, #MeV
    'KK_MASS_MAX_DsDs': 2200, #MeV
    'KK_MASS_MAX_DstDst': 2050, #MeV
    'KK_MASS_MAX_DstDp': 2150, #MeV
    'KK_VCHI2NDOF': 9, 
    # Cuts made on all D0's
    'D0_MASS_WIN': 65, #MeV
    'D0_PT': 1500., #MeV
    'D0_VCHI2NDOF': 10, 
    # Cuts made on all D+'s
    'Dp_MASS_WIN': 65, #MeV
    'Dp_PT': 1500., #MeV
    'Dp_VCHI2NDOF': 10,
    # Cuts made on all Ds+'s
    'Ds_MASS_WIN': 65, #MeV
    'Ds_PT': 1500., #MeV
    'Ds_VCHI2NDOF': 10,
    # Cuts made on all D*+'s
    'Dst_MASS_WIN': 50, #MeV
    'Dst_PT': 1500., #MeV
    'Dst_VCHI2NDOF': 15,
    # Cuts made on all B's
    'B_MASS_MAX': 6000, #MeV
    'B_MASS_MIN': 4500, #MeV
    'B_MASS_MIN_MDST': 4800, #MeV
#    'B_DOCA': 0.3, #mm
    'B_DOCACHI2': 16, # 4-body decay
    'B_DIRA': 0.99, 
    'B_VCHI2NDOF': 15,
    'B_BPVLTIME': 0.2, #ps
    # Cuts made on PID for specific particles
    'K_ProbNNk_MIN': 0.1,
    'PI_ProbNNpi_MIN': 0.1
    }, 
    'STREAMS' : {
     'Bhadron': [
     'StrippingB2DDphi_B2DstDpPhiLine',
     'StrippingB2DDphi_B2DDPhiLine',
     'StrippingB2DDphi_B2DsDsPhiLine',
     'StrippingB2DDphi_B2DstDstPhiLine', 
    ],
    'BhadronCompleteEvent':[
     'StrippingB2DDphi_B2D0D0PhiLine',  
     ]
    },
    'WGs' : [ 'B2OC' ]    
}


# Configure the LinBuilder
class B2DDphiConf(LineBuilder):
    __configuration_keys__ = (
        # Lines
        'FULLDST',
        # Cuts made on all bachelor tracks
        'TR_CHI2DOF_MAX',
        'TR_PT_MIN',
        'TR_P_MIN',
        'TR_MIPCHI2DV_MIN',
        'TR_GHP_MAX',
        # Cuts made on all KK's
        'KK_MASS_MAX',
        'KK_MASS_MAX_D0D0',
        'KK_MASS_MAX_DD',
        'KK_MASS_MAX_DsDs',
        'KK_MASS_MAX_DstDp',
        'KK_MASS_MAX_DstDst',
        'KK_VCHI2NDOF',
        # Cuts made on all D0's
        'D0_MASS_WIN',
        'D0_PT',
        'D0_VCHI2NDOF',
        # Cuts made on all D+'s
        'Dp_MASS_WIN',
        'Dp_PT',
        'Dp_VCHI2NDOF',
        # Cuts made on all Ds+'s
        'Ds_MASS_WIN',
        'Ds_PT',
        'Ds_VCHI2NDOF',
        # Cuts made on all D*+'s
        'Dst_MASS_WIN',
        'Dst_PT',
        'Dst_VCHI2NDOF',
        # Cuts made on all B's
        'B_MASS_MAX',
        'B_MASS_MIN',
        'B_MASS_MIN_MDST',
        'B_DOCACHI2',
        'B_DIRA',
        'B_VCHI2NDOF',
        'B_BPVLTIME',
        # Cuts made on PID for specific particles
        'K_ProbNNk_MIN',
        'PI_ProbNNpi_MIN'
        )

    def __init__(self, moduleName, config):
        LineBuilder.__init__(self, moduleName, config)

        # Lines
        FullDstLines = config['FULLDST'].split(',')

        # Select D candidates from composite common particles
        self.selD = {}
        DSelections = {'D0': {'name':'D02KPi',
                              'pdgname': 'D0',
                              'location': 'StdLooseD02KPi'},
                        'Dp': {'name':'D2KPiPi',
                              'pdgname': 'D+',
                              'location': 'StdLooseDplus2KPiPi'},
                        'Ds': {'name': 'Ds2KKPi',
                              'pdgname': 'D_s+',
                              'location': 'StdLooseDsplus2KKPi'},
                        'Dst': {'name': 'Dst2D0Pi_D02KPi',
                              'pdgname': 'D*(2010)+',
                              'location': 'StdLooseDstarWithD02KPi'} }
        for name, args in DSelections.iteritems():
            self.selD.update({name: self.makeDCandidate(args['name']+'For'+moduleName,
                                                        args['pdgname'],
                                                        args['location'],
                                                        config['TR_PT_MIN'],
                                                        config['TR_P_MIN'],
                                                        config['TR_MIPCHI2DV_MIN'],
                                                        config['TR_GHP_MAX'],
                                                        config['K_ProbNNk_MIN'],
                                                        config['PI_ProbNNpi_MIN'],
                                                        config[name+'_MASS_WIN'],
                                                        config[name+'_PT'],
                                                        config[name+'_VCHI2NDOF'])})
            
        # Select kaons from StdLoose list
        self.selK  = self.makeKaons(   'KFor' + moduleName, 
                                       config['TR_PT_MIN'],
                                       config['TR_P_MIN'],
                                       config['TR_MIPCHI2DV_MIN'],
                                       config['TR_GHP_MAX'],
                                       config['K_ProbNNk_MIN'] )

        # Create B candidates
        self.selB = {}
        BCandidateHelper = {'D0D0': {'decayDescriptor': "B_s0 -> K+ K- D0 D~0",
                                     'selections': ['D0']},
                            'DstDp': {'decayDescriptor': "[B_s0 -> K+ K- D*(2010)+ D-]cc",
                                     'selections': ['Dst', 'Dp']},
                            'DD': {'decayDescriptor': "B_s0 -> K+ K- D+ D-",
                                     'selections': ['Dp']},
                            'DsDs': {'decayDescriptor': "B_s0 -> K+ K- D_s+ D_s-",
                                     'selections': ['Ds']},
                            'DstDst': {'decayDescriptor': "B_s0 -> K+ K- D*(2010)+ D*(2010)-",
                                     'selections': ['Dst']} }
        for name, args in BCandidateHelper.iteritems():
            self.selB.update({name: self.makeBs2DDPhi('B2'+name+'Phi'+ moduleName,
                                                    args['decayDescriptor'],
                                                    Selections = [ self.selK]+[ self.selD[i] for i in args['selections']],
                                                    KKMassMax = config['KK_MASS_MAX_'+name],
                                                    BMassMax = config['B_MASS_MAX'],
                                                    BMassMin = config['B_MASS_MIN' if name in FullDstLines else 'B_MASS_MIN_MDST'],
                                                    BVCHI2 = config['B_VCHI2NDOF'],
                                                    BDOCACHI2 = config['B_DOCACHI2'],
                                                    BDIRA = config['B_DIRA'],
                                                    BPVLTIME = config['B_BPVLTIME']) })
            
        # Create the stripping lines
        for name, selections in self.selB.iteritems():
            line = StrippingLine(moduleName+"_B2"+name+"PhiLine",
                                             prescale = 1,
                                             postscale = 1,
                                             algos = [ selections ]
                                             )
            self.registerLine(line)

    def makeKaons( self, name,
                   KaonPT,
                   KaonP,
                   KaonIPCHI2,
                   KaonGPMax,
                   KaonProbNNk ):

        """
        Create and return a refined list of Kaons
        Starts from DataOnDemand 'Phys/StdLooseKaons'.
        Arguments:
        name             : name of the Selection.
        KaonPT           : Minimum transverse momentum of K (MeV).
        KaonIPCHI2       : Minimum impact parameter chi2 of K.
        KaonProbNNk      : Minimum ProbNNk of K.
        """
        
        _params = locals()
        _code = " ( PROBNNk > %(KaonProbNNk)s)"    \
            "& ( PT > %(KaonPT)s *MeV)"  \
            "& ( P > %(KaonP)s *MeV)" \
            "& ( TRGHP < %(KaonGPMax)s)"\
            "& ( MIPCHI2DV(PRIMARY) > %(KaonIPCHI2)s)" % _params
        StdLooseKaons = DataOnDemand(Location = "Phys/StdAllLooseANNKaons/Particles")

        _kFilter = FilterDesktop("KFilterForBs2DDPhi",
                                 Code = _code)

        return Selection (name,
                          Algorithm = _kFilter,
                          RequiredSelections = [StdLooseKaons])

    
    def makeDCandidate( self, name,
                        PDGName,
                        ContainerName,
                        TrackPT,
                        TrackP,
                        TrackIPCHI2,
                        TrackGHP,
                        KaonProbNNk,
                        PionProbNNpi,
                        DMassWin,
                        DPT,
                        DVCHI2):
        
        """
        Create and return a D candidate Selection object.
        Starts from DataOnDemand 'Phys/StdLooseD02KPi'.
        Arguments:
        name             : name of the Selection.
        TrackPT          : Minimum transverse momentum of K,pi (MeV).
        TrackIPCHI2      : Minimum impact parameter chi2 of K,pi.
        TrackGHP         : Maximum ghost probability of K,pi.
        KaonProbNNk      : Minimum ProbNNK of K.
        PionProbNNpi     : Minimum ProbNNpi of pi.
        DPT             : Minimum transverse momentum of D (MeV).
        DMassWin        : D invariant mass window around PDG mass value (MeV).
        DVCHI2          : Maximum D vertex chi2 (per degree of freedom?)
        """
        
        _params = locals()
        _code = " (INTREE(('K+'==ABSID) & (PROBNNk > %(KaonProbNNk)s) & (PT>%(TrackPT)s *MeV) & (MIPCHI2DV(PRIMARY)> %(TrackIPCHI2)s) & (TRGHP < %(TrackGHP)s)))" \
            "& (INTREE(('pi+'==ABSID) & (PROBNNpi > %(PionProbNNpi)s) & (PT>%(TrackPT)s *MeV) & (MIPCHI2DV(PRIMARY)> %(TrackIPCHI2)s) & (TRGHP < %(TrackGHP)s)))" \
            "& (ADMASS('%(PDGName)s') < %(DMassWin)s *MeV)" \
            "& (PT > %(DPT)s *MeV)" \
            "& (VFASPF(VCHI2/VDOF) < %(DVCHI2)s)" % _params
        DCandidatesList = DataOnDemand(Location = "Phys/"+ContainerName+"/Particles")
        _dFilter = FilterDesktop(name+"FilterForBs2DDPhi",
                                  Code = _code)

        return Selection (name,
                          Algorithm = _dFilter,
                          RequiredSelections = [DCandidatesList])
    
    def makeBs2DDPhi(self, name,
                     DecayDesc,
                     Selections,
                     KKMassMax,
                     BMassMax,
                     BMassMin,
                     BVCHI2,
                     BDOCACHI2,
                     BDIRA,
                     BPVLTIME):
        """
        Create and return a Bs -> D+ (KPiPi) D- (KPiPi) Phi (KK) Selection object.
        Arguments:
        name        : name of the Selection.
        Ksel        : Kaons Selection object.
        D0sel       : D+ -> K-pi+pi+ Selection object.
        KKMassMax   : KK invariant mass maximum (MeV).
        BMassMax    : Bs invariant mass maximum (MeV).
        BMassMin    : Bs invariant mass minimum (MeV).
        BVCHI2      : Maximum Bs vertex chi2 per degree of freedom.
        BDOCACHI2   : Maximum DOCACHI2 for tracks used in B vertex.
        BPVLTIME    : Minimum Bs lifetime
        """ 
        
        _motherCuts = "(VFASPF(VCHI2/VDOF) < %(BVCHI2)s) & (BPVDIRA > %(BDIRA)s) & (BPVLTIME() > %(BPVLTIME)s*ps)"% locals()
        _combinationCut = "(AM > %(BMassMin)s *MeV) & (AM < %(BMassMax)s *MeV)" \
            " & (ACHI2DOCA(1,4)< %(BDOCACHI2)s) & (ACHI2DOCA(2,4)< %(BDOCACHI2)s) & (ACHI2DOCA(3,4)< %(BDOCACHI2)s)" % locals() 
        _combination12Cut = "(AM < %(KKMassMax)s *MeV) & (ACHI2DOCA(1,2) < %(BDOCACHI2)s )"  % locals()
        _combination123Cut = "(ACHI2DOCA(1,3) < %(BDOCACHI2)s ) & (ACHI2DOCA(2,3) < %(BDOCACHI2)s )"  % locals()
        
        _Bs = DaVinci__N4BodyDecays("_"+name)
        _Bs.DecayDescriptor = DecayDesc
        _Bs.DaughtersCuts = { }
        _Bs.Combination12Cut = _combination12Cut
        _Bs.Combination123Cut = _combination123Cut
        _Bs.CombinationCut = _combinationCut
        _Bs.MotherCut = _motherCuts
        
        return Selection ( name,
                           Algorithm = _Bs,
                           RequiredSelections = Selections)

