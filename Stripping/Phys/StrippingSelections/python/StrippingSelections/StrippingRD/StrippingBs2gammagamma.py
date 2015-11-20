'''
Bs -> gamma gamma stripping selection.

Provides class StrippingBs2gammagammaConf with methods to return stripping line objects.

Exports the following stripping lines as instance data members:
- Bs2gammagamma_LLLine      : main line with one conversion (LL)
- Bs2gammagamma_DDLine      : main line with one conversion (DD)
- Bs2gammagamma_DoubleLine  : line with 2 conversions (DD or LL)
- Bs2gammagamma_NoConvLine  : line with 0 conversions
- Bs2gammagamma_NoConvWideLine  : wide line with 0 conversions
'''

__author__  = [ 'Sean Benson' ]
__date__    = '2014/07/07'
__version__    = '1.0'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop#, CombineParticles
from Configurables import CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from CommonParticles import StdAllLooseGammaConversion
from CommonParticles import StdLooseAllPhotons

from Configurables import ( DiElectronMaker, ProtoParticleCALOFilter,
                            ParticleTransporter, BremAdder )

default_config = {
    'NAME'        : 'Bs2GammaGamma',
    'WGs'         : ['RD'],
    'BUILDERTYPE' : 'StrippingBs2gammagammaConf',
    'CONFIG'      : { 'gammaPT'             : 1000    # MeV/c
                     ,'gammaP'              : 6000   # MeV/c
                     ,'gammaCL'             : 0.3     # adimensional
                     ,'gammaConvPT'         : 2000    # MeV/c
                     ,'gammaConvMDD'        : 60     # MeV/cc
                     ,'gammaConvIPCHI'      : 0     # adimensional
                     ,'gammaNonePT'         : 1100    # MeV/c
                     ,'gammaNoneP'          : 6000   # MeV/c
                     ,'gammaNoneCL'         : 0.3    # adimensional
                     ,'NoConvHCAL2ECAL'     : 0.1   # adimensional
                     ,'LLProbNNe'           : 0.5    # adimensional
                     ,'DDProbNNe'           : 0.3    # adimensional
                     ,'ConvGhostLL'         : 0.3    # adimensional
                     ,'ConvGhostDD'         : 0.3    # adimensional
                     ,'BsPT'                : 2000    # MeV/c
                     ,'BsVertexCHI2pDOF'    : 20      # adimensional
                     ,'BsLowMass'           : 4300    # MeV/cc
                     ,'BsLowMassDD'         : 4400    # MeV/cc
                     ,'BsNonePT'            : 2000    # MeV/c
                     ,'BsLowMassDouble'     : 4000    # MeV/cc
                     ,'BsLowMassNone'       : 4800    # MeV/cc
                     ,'BsHighMass'          : 6500    # MeV/cc
                     ,'BsHighMassDD'        : 6700    # MeV/cc
                     ,'BsHighMassNone'      : 6300    # MeV/cc
                     ,'BsHighMassDouble'    : 7000    # MeV/cc
                     ,'HLT2Double'          : "HLT_PASS_RE('Hlt2RadiativeB2GammaGammaDoubleDecision')"
                     ,'HLT2LL'              : "HLT_PASS_RE('Hlt2RadiativeB2GammaGammaLLDecision')"
                     ,'HLT2DD'              : "HLT_PASS_RE('Hlt2RadiativeB2GammaGammaDDDecision')"
                     ,'HLT1None'            : "HLT_PASS_RE('Hlt1.*GammaGammaDecision')"
                     ,'HLT2None'            : "HLT_PASS_RE('Hlt2.*GammaGammaDecision')"
                    },
    'STREAMS'     : ['Leptonic']
    }


class StrippingBs2gammagammaConf(LineBuilder):


    __configuration_keys__ = (
          'gammaPT'                        # MeV/c
        , 'gammaP'                         # MeV/c
        , 'gammaCL'                        # adimensional
        , 'gammaConvPT'                    # MeV/c
        , 'gammaConvMDD'                   # MeV/cc
        , 'gammaConvIPCHI'                 # adimensional
        , 'gammaNonePT'                    # MeV/c
        , 'gammaNoneP'                     # MeV/c
        , 'gammaNoneCL'                    # adimensional
        , 'NoConvHCAL2ECAL'                # adimensional
        , 'LLProbNNe'                      #
        , 'DDProbNNe'                      #
        , 'ConvGhostLL'                    #
        , 'ConvGhostDD'                    #
        , 'BsPT'                           # MeV/c
        , 'BsVertexCHI2pDOF'               # adimensional
        , 'BsLowMass'                      # MeV/cc
        , 'BsLowMassDD'                    # MeV/cc
        , 'BsNonePT'                       # MeV/c
        , 'BsLowMassDouble'                # MeV/cc
        , 'BsLowMassNone'                  # MeV/cc
        , 'BsHighMass'                     # MeV/cc
        , 'BsHighMassDD'                   # MeV/cc
        , 'BsHighMassDouble'               # MeV/cc
        , 'BsHighMassNone'                 # MeV/cc
        , 'HLT2Double'
        , 'HLT2LL'
        , 'HLT2DD'
        , 'HLT1None'
        , 'HLT2None'
    )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self.L0cut = "L0_CHANNEL_RE('Electron') | L0_CHANNEL_RE('Photon')"

        fltrCode_LL = "(MAXTREE(ISBASIC,TRGHOSTPROB)<%(ConvGhostLL)s) & (PT>(%(gammaConvPT)s-200.0)*MeV) & (MIPCHI2DV(PRIMARY)>%(gammaConvIPCHI)s) & (MAXTREE(ISBASIC,PPINFO( LHCb.ProtoParticle.ProbNNe,-1 ))>%(LLProbNNe)s)" % config
        self._trkFilter_LL = FilterDesktop( Code = fltrCode_LL )
        fltrCode_DD = "(MAXTREE(ISBASIC,TRGHOSTPROB)<%(ConvGhostDD)s) & (M<%(gammaConvMDD)s*MeV) & (PT>%(gammaConvPT)s*MeV) & (MIPCHI2DV(PRIMARY)>(2.0/3.0)*%(gammaConvIPCHI)s) & (MAXTREE(ISBASIC,PPINFO( LHCb.ProtoParticle.ProbNNe,-1 ))>%(DDProbNNe)s)" % config
        self._trkFilter_DD = FilterDesktop( Code = fltrCode_DD )
        fltrCode_nonConv = "(PT>%(gammaPT)s*MeV) & (CL>%(gammaCL)s) & (MAXTREE(ISBASIC,PPINFO( LHCb.ProtoParticle.CaloNeutralHcal2Ecal,1000 ))<%(NoConvHCAL2ECAL)s)" % config
        self._trkFilterNonConv = FilterDesktop( Code = fltrCode_nonConv )
        #
        self.convPhotons_LL = DataOnDemand(Location='Phys/StdAllLooseGammaLL/Particles')
        self.convPhotons_DD = DataOnDemand(Location='Phys/StdAllLooseGammaDD/Particles')
        stdPhotons     = DataOnDemand(Location='Phys/StdLooseAllPhotons/Particles')
        #
                # Clean up the converted photons to reduce the timing
        self.convPhotons_LL_clean = Selection( 'PhotonConvFilterLL' + name, Algorithm = self._trkFilter_LL, RequiredSelections = [self.convPhotons_LL])
        self.convPhotons_DD_clean = Selection( 'PhotonConvFilterDD' + name, Algorithm = self._trkFilter_DD, RequiredSelections = [self.convPhotons_DD])
        # Clean up the non-converted photons to reduce the timing
        self.stdPhotons_clean = Selection( 'PhotonFilter' + name, Algorithm = self._trkFilterNonConv, RequiredSelections = [stdPhotons])

                # Make sure our photons and electrons are TOS at L0
                # photon
                #self.stdPhotons_L0 =  MergedSelection("mergedSelphotonL0", RequiredSelections = [
                #    self.TOSFilter("SelgammaL0PhotonTOS", [self.stdPhotons_clean], "L0PhotonDecision"),
                #    self.TOSFilter("SelgammaL0ElectronTOS", [self.stdPhotons_clean], "L0ElectronDecision")] )
                # conv LL
                #self.convPhotons_LL_clean_L0 = MergedSelection("mergedSelLongLongL0", RequiredSelections = [
                #    self.TOSFilter("SelConvLLL0PhotonTOS", [self.convPhotons_LL_clean],"L0PhotonDecision"),
                #    self.TOSFilter("SelConvLLL0ElectronTOS", [self.convPhotons_LL_clean],"L0ElectronDecision")] )
                # conv DD
                #self.convPhotons_DD_clean_L0 = MergedSelection("mergedSelDownDownL0", RequiredSelections = [
                #    self.TOSFilter("SelConvDDL0PhotonTOS", [self.convPhotons_DD_clean],"L0PhotonDecision"),
                #    self.TOSFilter("SelConvDDL0ElectronTOS", [self.convPhotons_DD_clean],"L0ElectronDecision")] )

        self.Bs2gammagammaLLLine     = self._Bs2gammagammaLL_X_Line( name, config)
        self.registerLine( self.Bs2gammagammaLLLine )
        self.Bs2gammagammaDDLine     = self._Bs2gammagammaDD_X_Line( name, config)
        self.registerLine( self.Bs2gammagammaDDLine )
        self.Bs2gammagammaDoubleLine     = self._Bs2gammagammaDouble_X_Line( name, config)
        self.registerLine( self.Bs2gammagammaDoubleLine )
        self.Bs2gammagammaNoneLine     = self._Bs2gammagammaNone_X_Line( name, config, False)
        self.Bs2gammagammaNoneWideLine     = self._Bs2gammagammaNone_X_Line( name+"Wide", config, True)
        self.registerLine( self.Bs2gammagammaNoneLine )
        self.registerLine( self.Bs2gammagammaNoneWideLine )


    def _Bs2gammagammaLL_X_Line( self, name, config) :
        BsGG_DC_LL = "(P>(%(gammaP)s-2000.0)*MeV)" % config
        BsGG_CC_LL = "(in_range(%(BsLowMass)s*MeV, AM, %(BsHighMass)s*MeV))" % config
        BsGG_MC_LL = "(PT>%(BsPT)s*MeV) & (INTREE( (ID=='gamma') & (ISBASIC) )) & (INTREE( HASTRACK & ISLONG ))" % config


        _Bs2gammagamma_LL = CombineParticles(name = "CombineParticles_BsGG_LL",
                DecayDescriptor = "B_s0 -> gamma gamma"
                , DaughtersCuts  = {'gamma' : BsGG_DC_LL}
                , CombinationCut = BsGG_CC_LL
                , MotherCut      = BsGG_MC_LL)
        _Bs2gammagamma_LL.ParticleCombiners.update({ '' : 'MomentumCombiner:PUBLIC'})

        Bs2gammagamma_LL = Selection(
                name+"_LL",
                Algorithm = _Bs2gammagamma_LL,
                RequiredSelections = [self.convPhotons_LL_clean,self.stdPhotons_clean])

        return StrippingLine(name+"_LLLine"
                     , prescale = 1
                     , postscale = 1
                     #, selection = self.TOSFilter(name+"_LLTOSLine",[Bs2gammagamma_LL],"L0(Photon|Electron)Decision")
                     , selection = self.TOSFilter(name+"_LLTOSLine",[Bs2gammagamma_LL],"Hlt2RadiativeB2GammaGammaLLDecision") # HLT2 lines require L0 anyway
                     , L0DU = self.L0cut
                     , HLT2 = config["HLT2LL"]
                     , RequiredRawEvents = ["Calo"],MDSTFlag = True
                     , EnableFlavourTagging = False
                     )
    def _Bs2gammagammaDD_X_Line( self, name, config) :
        BsGG_DC_DD = "(P>%(gammaP)s*MeV)" % config
        BsGG_CC_DD = "(in_range(%(BsLowMassDD)s*MeV, AM, %(BsHighMassDD)s*MeV))" % config
        BsGG_MC_DD = "(PT>%(BsPT)s*MeV) & (INTREE( (ID=='gamma') & (ISBASIC) )) & (INTREE( HASTRACK & ISDOWN ))" % config


        _Bs2gammagamma_DD = CombineParticles(name = "CombineParticles_BsGG_DD",
                DecayDescriptor = "B_s0 -> gamma gamma"
                , DaughtersCuts  = {'gamma' : BsGG_DC_DD}
                , CombinationCut = BsGG_CC_DD
                , MotherCut      = BsGG_MC_DD)
        _Bs2gammagamma_DD.ParticleCombiners.update({ '' : 'MomentumCombiner:PUBLIC'})

        Bs2gammagamma_DD = Selection(
                name+"_DD",
                Algorithm = _Bs2gammagamma_DD,
                RequiredSelections = [self.convPhotons_DD_clean,self.stdPhotons_clean])

        return StrippingLine(name+"_DDLine"
                , prescale = 1
                , postscale = 1
                #, selection = self.TOSFilter(name+"_DDTOSLine",[Bs2gammagamma_DD],"L0(Photon|Electron)Decision")
                , selection = self.TOSFilter(name+"_DDTOSLine",[Bs2gammagamma_DD],"Hlt2RadiativeB2GammaGammaDDDecision") # HLT2 lines require L0 anyway
                , L0DU = self.L0cut
                , HLT2 = config["HLT2DD"]
                , RequiredRawEvents = ["Calo"],MDSTFlag = True
                , EnableFlavourTagging = False)
    def _Bs2gammagammaDouble_X_Line( self, name, config) :
        BsGG_DC_double = "(PT>0.5*%(gammaConvPT)s*MeV) & (P>0.5*%(gammaP)s*MeV)" % config
        BsGG_CC_double = "(in_range(%(BsLowMassDouble)s*MeV, AM, %(BsHighMassDouble)s*MeV))" % config
        BsGG_MC_double = "(VFASPF(VCHI2/VDOF)<%(BsVertexCHI2pDOF)s)" % config


        _Bs2gammagamma_double = CombineParticles(name = "CombineParticles_BsGG_double",
                DecayDescriptor = "B_s0 -> gamma gamma"
                , DaughtersCuts  = {'gamma' : BsGG_DC_double}
                , CombinationCut = BsGG_CC_double
                , MotherCut      = BsGG_MC_double)
        _Bs2gammagamma_double.ParticleCombiners.update( { "" : "OfflineVertexFitter:PUBLIC"} )

        allConv = MergedSelection("MergedConversions",RequiredSelections=[self.convPhotons_LL,self.convPhotons_DD])
        Bs2gammagamma_double = Selection(
                name+"_double",
                Algorithm = _Bs2gammagamma_double,
                RequiredSelections = [allConv])

        return StrippingLine(name+"_doubleLine"
                , prescale = 1
                , postscale = 1
                #, selection = self.TOSFilter(name+"_doubleTOSLine",[Bs2gammagamma_double],"L0(Photon|Electron)Decision")
                , selection = self.TOSFilter(name+"_doubleTOSLine",[Bs2gammagamma_double],"Hlt2RadiativeB2GammaGammaDoubleDecision") # HLT2 lines require L0 anyway
                , L0DU = self.L0cut
                , HLT2 = config["HLT2Double"]
                , RequiredRawEvents = ["Calo"],MDSTFlag = True
                , EnableFlavourTagging = False)
    def _Bs2gammagammaNone_X_Line( self, name, config, wide) :

        BsGG_DC_none = "(PT>%(gammaNonePT)s*MeV) & (P>%(gammaNoneP)s*MeV) & (CL>%(gammaNoneCL)s)" % config
        if wide == True:
            BsGG_CC_none = "(in_range( ( %(BsLowMassNone)s - 500.0 )*MeV, AM, ( %(BsHighMassNone)s + 500.0 )*MeV) )" % config
            BsGG_MC_none = "(PT>%(BsNonePT)s*MeV) & (in_range( ( %(BsLowMassNone)s - 500.0 )*MeV, M, ( %(BsHighMassNone)s + 500.0 )*MeV) )" % config
        else:
            BsGG_CC_none = "(in_range(%(BsLowMassNone)s*MeV, AM, %(BsHighMassNone)s*MeV))" % config
            BsGG_MC_none = "(PT>%(BsNonePT)s*MeV) & (in_range(%(BsLowMassNone)s*MeV, M, %(BsHighMassNone)s*MeV))" % config

        if wide == True:
            scaleWide = 0.2
        else:
            scaleWide = 1.0

        _Bs2gammagamma_none = CombineParticles(name = "CombineParticles_BsGG_none",
                DecayDescriptor = "B_s0 -> gamma gamma"
                , DaughtersCuts  = {'gamma' : BsGG_DC_none}
                , CombinationCut = BsGG_CC_none
                , MotherCut      = BsGG_MC_none)
        _Bs2gammagamma_none.ParticleCombiners.update({ '' : 'MomentumCombiner:PUBLIC'})

        Bs2gammagamma_none = Selection(
                name+"_none",
                Algorithm = _Bs2gammagamma_none,
                RequiredSelections = [self.stdPhotons_clean])

        return StrippingLine(name+"_NoConvLine"
                , prescale = scaleWide
                , postscale = 1
                , HLT1 = config["HLT1None"]
                , HLT2 = config["HLT2None"]
                #, selection = self.TOSFilter(name+"_NoConvTOSLine",[Bs2gammagamma_none],"L0(Photon|Electron)Decision")
                , selection = self.TOSFilter(name+"_NoConvTOSLine",[Bs2gammagamma_none],"Hlt2RadiativeB2GammaGammaDecision") # HLT2 lines require L0 anyway
                                , L0DU = self.L0cut
                                , RequiredRawEvents = ["Calo"],MDSTFlag = True
                , EnableFlavourTagging = False)

    def TOSFilter( self, name = None, sel = None, trigger = None ):
        from Configurables import TisTosParticleTagger
        _filter = TisTosParticleTagger(name+"_TriggerTos")
        _filter.TisTosSpecs = { trigger+"%TOS" : 0 }
        from PhysSelPython.Wrappers import Selection
        _sel = Selection("Sel" + name + "_TriggerTos", RequiredSelections = sel, Algorithm = _filter )
        return _sel
    def mergedTOS(self, name = None, sel = None, trigger1 = None, trigger2 = None):
        return MergedSelection(name, RequiredSelections = [
            self.TOSFilter(name+trigger1+"_subsel", [sel], trigger1),
            self.TOSFilter(name+trigger2+"_subsel", [sel], trigger2)] )

