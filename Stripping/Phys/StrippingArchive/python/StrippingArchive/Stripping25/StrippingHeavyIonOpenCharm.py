"""
Stripping lines for selection of
    [D0 -> K- pi+]cc
    [D*(2010)+ -> (D0 -> K- pi+) pi+]cc

    [D+ -> K- pi+ pi+]cc
    [D+ -> K- K+ pi+]cc
    [D_s+ -> pi+ (phi(1020) -> K- K+)]cc
    [Lambda_c+ -> p+ K- pi+]cc
for open charm cross section measurement.
!!! based on those implemented by [Alex Pearce]
"""

__author__ = ['Yanxi ZHANG']

__all__ = (
    'default_config',
    'HeavyIonOpenCharmConf'
)


from GaudiKernel.SystemOfUnits import MeV, GeV, mrad, mm
from GaudiConfUtils.ConfigurableGenerators import CombineParticles, FilterDesktop
#from Configurables import CombineParticles, FilterDesktop
from StandardParticles import (
    StdAllNoPIDsKaons,
    StdAllNoPIDsPions,
    StdAllNoPIDsProtons,
)
from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine

default_config = {
    'NAME': 'HeavyIonOpenCharm',
    'WGs': ['ALL'],
    'BUILDERTYPE': 'HeavyIonOpenCharmConf',
    'STREAMS': ['ALL'],
    'CONFIG': {
        "odin": ["NoBeam","Beam1","Beam2","BeamCrossing"], #"( ODIN_BXTYP == LHCb.ODIN.Beam1) | (ODIN_BXTYP == LHCb.ODIN.Beam2) | (ODIN_BXTYP == LHCb.ODIN.BeamCrossing) | (ODIN_BXTYP == LHCb.ODIN.NoBeam)",        ##[ 'NoBeam', 'BeamCrossing','Beam1','Beam2']
        'Track_CHI2'         : 3,         # Minimum track chi2
        #
        #####################
        ###  D0 -> K- pi+   #
        #####################
        'D0_Daug_All_PT_MIN'         : 250.0*MeV,         # Minimum transverse momentum all D0 daughters must satisfy before fit
        'D0_Daug_AllA_PT_MIN'         : 500.0*MeV,         # Minimum transverse momentum at least one D0 daughters must satisfy after fit
        'D0_Daug_P_MIN'              : 3.0*GeV,           # Minimum D0 daughter momentum
        'D0_Daug_P_MAX'              : 1.E6*GeV,          # Maximum D0 daughter momentum,       'do it offline"
        'D0_Daug_ETA_MIN'            : 0.0,               # Minimum D0 daughter pseudorapidity, 'do it offline'
        'D0_Daug_ETA_MAX'            : 10.0,              # Maximum D0 daughter pseudorapidity, 'do it offline'
        'D0_K_PIDK_MIN'              : 5.0,               # Minimum Dpm daughter kaon DLLK       'in trigger of pAr hlt2'
        'D0_Pi_PIDK_MAX'             : 5.0,              # Maximum Dpm daughter pion DLLK,       'in trigger of pAr hlt2'
        'D0_Comb_ADOCAMAX_MAX'       : 0.5,               # DOCA
        'D0_Daug_All_BPVIPCHI2_MIN'  : 4.0,               # Minimum best primary vertex IP chi^2 all D0 daughters must satisfy
        'D0_VCHI2VDOF_MAX'           : 10.0,              # Maximum Dpm vertex chi^2 per vertex fit DoF 
        'D0_acosBPVDIRA_MAX'         : 35.0*mrad,         # Maximum angle between D0 momentum and D0 direction of flight, '10 at HLT2 of pAr'

        'D0_PVDispCut'               : '((BPVVDCHI2 > 9.0)|(BPVLTIME() > 0.100*picosecond))', # Primary vertex displacement requirement
        'D0_ADAMASS_WIN'             : 100.0*MeV,         # D0 mass window around the nominal D0 mass before the vertex fit, '90 MeV at HLT2 of pAr'

        'D0_Prescale':  1.0,
        'D0_Postscale': 1.0, 

        'D0_Hlt1Filter': "HLT_PASS_RE('Hlt1SMOG.*Decision')",   #Hlt1SMOGD0KPiDecision ?
        'D0_Hlt2Filter': "HLT_PASS_RE('Hlt2SMOGD02KPiDecision')",


        ##################################
        ###  D*+ -> (D0 -> K- pi+) pi+   #
        ##################################
        'Dst_AMDiff_MAX'        : 160.0*MeV,         # Maximum delta mass value m(D*{0,+}) - m(D0)
        'Dst_VCHI2VDOF_MAX'     : 10.0,              # Maximum D*+ vertex chi^2 per vertex DoF (_not_ applied to D*0)

        'Dst_Prescale'     : 1.0, 
        'Dst_Postscale'    : 1.0,

        'Dst_Hlt1Filter': "HLT_PASS_RE('Hlt1SMOG.*Decision')",   #Hlt1SMOGD0KPiDecision ?
        'Dst_Hlt2Filter': "HLT_PASS_RE('Hlt2SMOGD02KPiDecision')",


	######################################
	# D0 -> K pi No PV line, to estimate PV efficiency for open charm analysis 
	######################################
        'NoPVD0_Daug_All_PT_MIN'         : 800.0*MeV,         # Minimum transverse momentum all D0 daughters must satisfy
        'NoPVD0_Daug_P_MIN'              : 5.0*GeV,           # Minimum D0 daughter momentum
        'NoPVD0_Daug_P_MAX'              : 200.*GeV,          # Maximum D0 daughter momentum,       'do it offline"
        'NoPVD0_Daug_ETA_MIN'            : 2.0,               # Minimum D0 daughter pseudorapidity, 'do it offline'
        'NoPVD0_Daug_ETA_MAX'            : 5.0,              # Maximum D0 daughter pseudorapidity, 'do it offline'
        'NoPVD0_K_PIDK_MIN'              : 5.0,               # Minimum Dpm daughter kaon DLLK       'in trigger of pAr hlt2'
        'NoPVD0_Pi_PIDK_MAX'             : 5.0,              # Maximum Dpm daughter pion DLLK,       'in trigger of pAr hlt2'
        'NoPVD0_VCHI2VDOF_MAX'           : 6.0,              # Maximum Dpm vertex chi^2 per vertex fit DoF 
        'NoPVD0_Comb_ADOCAMAX_MAX'       : 0.5,               # DOCA

        'NoPVD0_ADAMASS_WIN'             : 100.0*MeV,         # D0 mass window around the nominal D0 mass before the vertex fit, '90 MeV at HLT2 of pAr'
	    'NoPVD0_PT'                      : 1000.0*MeV,
	    'NoPVD0_VTX_Xmin'                      : -5.0*mm,
	    'NoPVD0_VTX_Xmax'                      : +5.0*mm,
	    'NoPVD0_VTX_Ymin'                      : -5.0*mm,
	    'NoPVD0_VTX_Ymax'                      : +5.0*mm,
	    'NoPVD0_VTX_Zmin'                      : -1000.*mm,
	    'NoPVD0_VTX_Zmax'                      : +1000.*mm,
	    'NoPVD0_odin'                      : ["NoBeam","Beam1","Beam2","BeamCrossing"],

        'NoPVD0_Prescale':  0.2,
        'NoPVD0_Postscale': 1.0, 

        'NoPVD0_Hlt1Filter': "HLT_PASS_RE('Hlt1SMOG.*Decision')",
        'NoPVD0_Hlt2Filter': "HLT_PASS_RE('Hlt2SMOGD02KPiDecision')",

        ########################################
        ### D+ -> K- pi+ pi+, Ds+ -> K- K+ pi+, Lambda_c+ -> p+ K- pi+ #
        ########################################
        'D_Daug_All_PT_MIN'             : 200.0*MeV,         # Minimum transverse momentum all D daughters must satisfy
        'D_Daug_2of3_PT_MIN'            : 400.0*MeV, # Minimum transverse momentum at least 2 D daughters must satisfy
        'D_Daug_1of3_PT_MIN'            : 1000.0*MeV, # Minimum transverse momentum at least 1 D daughter must satisfy
        'D_Daug_All_BPVIPCHI2_MIN'      : 2.0,               # Minimum best primary vertex IP chi^2 all D daughters must satisfy
        'D_Daug_2of3_BPVIPCHI2_MIN'     : 4.0, # Minimum best PV IP chi^2 at least 2 D daughters must satisfy
        'D_Daug_1of3_BPVIPCHI2_MIN'     : 9.0, # Minimum best PV IP chi^2 at least 1 D daughter must satisfy
        'D_Daug_P_MIN'                  : 3.0*GeV,           # Minimum D daughter momentum
        'D_Daug_P_MAX'                  : 1.E6*GeV,          # Maximum D daughter momentum,       'do it offline"
        'D_Daug_ETA_MIN'                : 0.0,               # Minimum D daughter pseudorapidity, 'do it offline'
        'D_Daug_ETA_MAX'                : 10.0,              # Maximum D daughter pseudorapidity, 'do it offline'
        'D_K_PIDK_MIN'                  : 5.0,               # Minimum D daughter kaon DLLK       'in trigger of pAr hlt2'
        'D_Pi_PIDK_MAX'                 : 5.0,              # Maximum D daughter pion DLLK,       'in trigger of pAr hlt2'
        'D_P_PIDp_MIN'                  : 5.0,              # Maximum D (Lc) daughter proton DLLp,       'in trigger of pAr hlt2'
        'D_P_PIDpPIDK_MIN'              : 5.0,              # Maximum D (Lc) daughter proton DLLp-DLLK,       'in trigger of pAr hlt2'
        'D_Comb_ADOCAMAX_MAX'           : 0.5,         # Maximum distance of closest approach of D daughters
        #'D_Comb_ADOCAMAX_MAX'          : 0.5*mm,         # Maximum distance of closest approach of D daughters
        'D_VCHI2VDOF_MAX'               : 25.0,              # Maximum Dpm vertex chi^2 per vertex fit DoF
        'D_acosBPVDIRA_MAX'             : 35.0*mrad,         # Maximum angle between Dpm momentum and Dpm direction of flight

        'Dp_PVDispCut'            : '((BPVVDCHI2 > 9.0)|(BPVLTIME() > 0.1*picosecond))', # Primary vertex displacement requirement
        'Dp_ADAMASS_WIN'          : 100.0*MeV,         # D+ mass window around the nominal D+ mass before the vertex fit, '90 MeV at HLT2 of pAr'

        'Dp_Prescale': 1.0,
        'Dp_Postscale': 1.0, 

        'Dp_Hlt1Filter': "HLT_PASS_RE('Hlt1SMOG.*Decision')",   #Hlt1SMOGDpmKKPiDecision ?
        'Dp_Hlt2Filter': "HLT_PASS_RE('Hlt2SMOGDpm2KPiPiDecision')",
       

        'Ds_PVDispCut'            : '((BPVVDCHI2 > 9.0)|(BPVLTIME() > 0.1*picosecond))', # Primary vertex displacement requirement
        'Ds_ADAMASS_WIN'          : 100.0*MeV,         # Ds+ mass window around the nominal Ds+ mass before the vertex fit, '90 MeV at HLT2 of pAr'

        'Ds_Prescale': 1.0,
        'Ds_Postscale': 1.0, 

        # HLT filters, only process events firing triggers matching the RegEx
        'Ds_Hlt1Filter': "HLT_PASS_RE('Hlt1SMOG.*Decision')",   #Hlt1SMOGDpmKPiDecision ?
        'Ds_Hlt2Filter': "HLT_PASS_RE('Hlt2SMOGDs2KKPiDecision')",


        'Lc_PVDispCut'            : '((BPVVDCHI2 > 9.0)|(BPVLTIME() > 0.1*picosecond))', # Primary vertex displacement requirement
        'Lc_ADAMASS_WIN'          : 100.0*MeV,         # Lc+ mass window around the nominal Lc+ mass before the vertex fit, '90 MeV at HLT2 of pAr'

        'Lc_Prescale': 1.0,
        'Lc_Postscale': 1.0, 

        # HLT filters, only process events firing triggers matching the RegEx
        'Lc_Hlt1Filter': "HLT_PASS_RE('Hlt1SMOG.*Decision')",   #Hlt1SMOGDpmKPiDecision ?
        'Lc_Hlt2Filter': "HLT_PASS_RE('Hlt2SMOGLc2KPPiDecision')",
    }
}


class HeavyIonOpenCharmConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    # Allowed configuration keys
    __configuration_keys__ = default_config['CONFIG'].keys()

    # Decay descriptors
    decays = {
          "D0"      :["[D0 -> K- pi+]cc"],
          "Dst"     :["[D*(2010)+ -> D0 pi+]cc"],
          "Dp"      :["[D+ -> K- pi+ pi+]cc"],
          "Ds"      :["[D_s+ -> K+ K- pi+]cc"],
          "Lc"      :["[Lambda_c+ -> p+ K- pi+]cc"],
    }

    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)


        # P: p+/p-
        # K: K+/K-
        # H: pi+/pi-, (hadron)
        D02HH_name = '{0}D02HH'.format(name)
        NoPVD02HH_name = '{0}NoPVD02HHBE'.format(name)
        Dst2D0Pi_name = '{0}Dst2D0Pi'.format(name)
        Dp2KHH_name = '{0}Dp2KHH'.format(name)
        Ds2KKH_name = '{0}Ds2KKH'.format(name)
        Lc2PKH_name = '{0}Lc2PKH'.format(name)

        self.inPions = StdAllNoPIDsPions
        self.inKaons = StdAllNoPIDsKaons
        self.inProtons = StdAllNoPIDsProtons

        self.selD02HH = self.makeD02HH(
            D02HH_name,
            inputSel=[self.inKaons, self.inPions],
            decDescriptors=self.decays["D0"]
        )

        self.selNoPVD02HH = self.makeNoPVD02HH(
            NoPVD02HH_name,
            inputSel=[self.inKaons, self.inPions],
            decDescriptors=self.decays["D0"]
        )

        self.selDst2D0Pi = self.makeDst2D0Pi(
            name            = Dst2D0Pi_name,
            inputSel        =[self.selD02HH, self.inPions],
            decDescriptors  =self.decays["Dst"]
        )

        self.selDp2KHH = self.makeThreeProng(
            name            = Dp2KHH_name,
            inputSel        =[self.inKaons, self.inPions],
            decDescriptors  =self.decays["Dp"],
            particle        = "D+"
        )
        self.selDs2KKH = self.makeThreeProng(
            name            =Ds2KKH_name,
            inputSel        =[self.inKaons, self.inPions],
            decDescriptors  =self.decays["Dp"],
            particle        = "D_s+"
        )
        self.selLc2PKH = self.makeThreeProng(
            name            =Lc2PKH_name,
            inputSel        =[self.inKaons, self.inPions, self.inProtons],
            decDescriptors  =self.decays["Lc"],
            particle        = "Lambda_c+"
        )



        #odin = []
        #for odin_type in config['odin']:
        #    if odin_type not in ["NoBeam","Beam1","Beam2","BeamCrossing"]:
        #        continue
        #    odin.append( "(ODIN_BXTYP == LHCb.ODIN.%s)"%(odin_type,) )
        #odin = "|".join(odin)
        odin = "|".join( ["(ODIN_BXTYP == LHCb.ODIN.%s)"%(odin_type,) for odin_type in self.config['odin'] if odin_type in ["NoBeam","Beam1","Beam2","BeamCrossing"]])
        #"( ODIN_BXTYP == LHCb.ODIN.Beam1) | (ODIN_BXTYP == LHCb.ODIN.Beam2) | (ODIN_BXTYP == LHCb.ODIN.BeamCrossing) | (ODIN_BXTYP == LHCb.ODIN.NoBeam)",        ##[ 'NoBeam', 'BeamCrossing','Beam1','Beam2']

        self.line_D02HH = self.make_line(
            name='{0}Line'.format(D02HH_name),
            prescale  =self.config['D0_Prescale'],
            postscale =self.config['D0_Postscale'],
            selection =self.selD02HH,
            HLT1      =self.config['D0_Hlt1Filter'],
            HLT2      =self.config['D0_Hlt2Filter'],
            ODIN      =odin
        )

        self.line_NoPVD02HH = self.make_line(
            name='{0}Line'.format(NoPVD02HH_name),
            prescale  =self.config['NoPVD0_Prescale'],
            postscale =self.config['NoPVD0_Postscale'],
            selection =self.selNoPVD02HH,
	        checkPV   = False,
            HLT1      =self.config['NoPVD0_Hlt1Filter'],
            HLT2      =self.config['NoPVD0_Hlt2Filter'],
            #ODIN      = odin
            ODIN      = ("|".join( ["(ODIN_BXTYP == LHCb.ODIN.%s)"%(odin_type,) for odin_type in self.config['NoPVD0_odin'] if odin_type in ["NoBeam","Beam1","Beam2"]]))
            #ODIN      = ("|".join( ["(ODIN_BXTYP == LHCb.ODIN.%s)"%(odin_type,) for odin_type in self.config['NoPVD0_odin'] if odin_type in ["NoBeam","Beam1","Beam2","BeamCrossing"]]))
        )

        #""", not work because trigger works only on Beam1
        self.line_NoPVD02HH = self.make_line(
            name=('{0}Line'.format(NoPVD02HH_name)).replace("BE","BB"),
            prescale  =self.config['NoPVD0_Prescale']*0.1,
            postscale =self.config['NoPVD0_Postscale'],
            selection =self.selNoPVD02HH,
	        checkPV   = False,
            HLT1      =self.config['NoPVD0_Hlt1Filter'],
            HLT2      =self.config['NoPVD0_Hlt2Filter'],
            #ODIN      = odin
            ODIN      = ("|".join( ["(ODIN_BXTYP == LHCb.ODIN.%s)"%(odin_type,) for odin_type in ["BeamCrossing"]]))
        )
        #"""


        self.line_Dst2D0Pi= self.make_line(
            name='{0}Line'.format(Dst2D0Pi_name),
            prescale   =self.config['Dst_Prescale'],
            postscale  =self.config['Dst_Postscale'],
            selection  =self.selDst2D0Pi,
            HLT1       =self.config['Dst_Hlt1Filter'],
            HLT2       =self.config['Dst_Hlt2Filter'],
            ODIN       =odin
        )

        self.line_Dp2KHH = self.make_line(
            name='{0}Line'.format(Dp2KHH_name),
            prescale   =self.config['Dp_Prescale'],
            postscale  =self.config['Dp_Postscale'],
            selection  =self.selDp2KHH,
            HLT1       =self.config['Dp_Hlt1Filter'],
            HLT2       =self.config['Dp_Hlt2Filter'],
            ODIN       =odin
        )

        self.line_Ds2KKH = self.make_line(
            name='{0}Line'.format(Ds2KKH_name),
            prescale   =self.config['Ds_Prescale'],
            postscale  =self.config['Ds_Postscale'],
            selection  =self.selDs2KKH,
            HLT1       =self.config['Ds_Hlt1Filter'],
            HLT2       =self.config['Ds_Hlt2Filter'],
            ODIN       =odin
        )

        self.line_Lc2PKH = self.make_line(
            name='{0}Line'.format(Lc2PKH_name),
            prescale   =self.config['Lc_Prescale'],
            postscale  =self.config['Lc_Postscale'],
            selection  =self.selLc2PKH,
            HLT1       =self.config['Lc_Hlt1Filter'],
            HLT2       =self.config['Lc_Hlt2Filter'],
            ODIN       =odin
        )


    def make_line(self, name, selection, prescale, postscale, **kwargs):
        """Create the stripping line defined by the selection.

        Keyword arguments:
        name -- Base name for the Line
        selection -- Selection instance
        prescale -- Fraction of candidates to randomly drop before stripping
        postscale -- Fraction of candidates to randomly drop after stripping
        **kwargs -- Keyword arguments passed to StrippingLine constructor
        """
        # Only create the line with positive pre- and postscales
        # You can disable each line by setting either to a negative value
        if prescale > 0 and postscale > 0:
            line = StrippingLine(
                name,
                selection=selection,
                prescale=prescale,
                postscale=postscale,
                **kwargs
            )
            self.registerLine(line)
            return line
        else:
            return False

    def makeD02HH(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a D0 -> h- h+ decay.

        Keyword arguments:
        name -- Name to give the Selection instance
        inputSel -- List of inputs passed to Selection.RequiredSelections
        decDescriptors -- List of decay descriptors for CombineParticles
        """
        lclPreambulo = [
            'from math import cos'
        ]

        daugCuts = (
            '(PT > {0[D0_Daug_All_PT_MIN]})'
            '&(TRCHI2DOF<{0[Track_CHI2]})'
            '& (BPVIPCHI2() > {0[D0_Daug_All_BPVIPCHI2_MIN]})'
        ).format(self.config)

        pidFiducialCuts = (
            '(in_range({0[D0_Daug_P_MIN]}, P, {0[D0_Daug_P_MAX]}))'
            '& (in_range({0[D0_Daug_ETA_MIN]}, ETA, {0[D0_Daug_ETA_MAX]}))'
        ).format(self.config)

        kaonPIDCuts = (
            pidFiducialCuts +
            '& (PIDK-PIDpi > {0[D0_K_PIDK_MIN]})'
        ).format(self.config)

        pionPIDCuts = (
            pidFiducialCuts +
            '& (PIDK-PIDpi < {0[D0_Pi_PIDK_MAX]})'
        ).format(self.config)

        combCuts = "(ADAMASS('D0') < {0[D0_ADAMASS_WIN]}) & (AMINDOCA('')<{0[D0_Comb_ADOCAMAX_MAX]}) & (((APT1 > {0[D0_Daug_AllA_PT_MIN]}))|((APT2 > {0[D0_Daug_AllA_PT_MIN]})))".format(self.config)

        d0Cuts = (
            '(VFASPF(VCHI2/VDOF) < {0[D0_VCHI2VDOF_MAX]})'
            '& ({0[D0_PVDispCut]})'
            '& (BPVDIRA > cos({0[D0_acosBPVDIRA_MAX]}))'
        ).format(self.config)

        _D0 = CombineParticles(
            #name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            Preambulo=lclPreambulo,
            DaughtersCuts={
                'pi+': '{0} & {1}'.format(daugCuts, pionPIDCuts),
                'K+': '{0} & {1}'.format(daugCuts, kaonPIDCuts)
            },
            CombinationCut=combCuts,
            MotherCut=d0Cuts
        )

        return Selection(name, Algorithm=_D0, RequiredSelections=inputSel)




    def makeNoPVD02HH(self, name, inputSel, decDescriptors):

        lclPreambulo = [
            'from math import cos'
        ]
        daugCuts = ( '(PT > {0[NoPVD0_Daug_All_PT_MIN]}) & (TRCHI2DOF<2.)').format(self.config)

        pidFiducialCuts = (
            '(in_range({0[NoPVD0_Daug_P_MIN]}, P, {0[NoPVD0_Daug_P_MAX]}))'
            '& (in_range({0[NoPVD0_Daug_ETA_MIN]}, ETA, {0[NoPVD0_Daug_ETA_MAX]}))'
        ).format(self.config)

        kaonPIDCuts = (
            pidFiducialCuts +
            '& (PIDK-PIDpi > {0[NoPVD0_K_PIDK_MIN]})'
        ).format(self.config)

        pionPIDCuts = (
            pidFiducialCuts +
            '& (PIDK-PIDpi < {0[NoPVD0_Pi_PIDK_MAX]})'
        ).format(self.config)

        combCuts = "(ADAMASS('D0') < {0[NoPVD0_ADAMASS_WIN]}) & (AMINDOCA('')<{0[NoPVD0_Comb_ADOCAMAX_MAX]})".format(self.config)

        d0Cuts = ( '(VFASPF(VCHI2/VDOF) < {0[D0_VCHI2VDOF_MAX]})'
                '&(PT>{0[NoPVD0_PT]})'
                '&(VFASPF(VX)>{0[NoPVD0_VTX_Xmin]})&(VFASPF(VX)<{0[NoPVD0_VTX_Xmax]})'
                '&(VFASPF(VY)>{0[NoPVD0_VTX_Ymin]})&(VFASPF(VY)<{0[NoPVD0_VTX_Ymax]})'
                '&(VFASPF(VZ)>{0[NoPVD0_VTX_Zmin]})&(VFASPF(VZ)<{0[NoPVD0_VTX_Zmax]})'
                ).format(self.config)

        _D0 = CombineParticles(
            #name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            Preambulo=lclPreambulo,
            DaughtersCuts={
                'pi+': '{0} & {1}'.format(daugCuts, pionPIDCuts),
                'K+': '{0} & {1}'.format(daugCuts, kaonPIDCuts)
            },
            CombinationCut=combCuts,
            MotherCut=d0Cuts
        )

        return Selection(name, Algorithm=_D0, RequiredSelections=inputSel)


    def makeDst2D0Pi(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a D*+ -> D0 pi+ decay.

        Keyword arguments:
        name -- Name to give the Selection instance
        inputSel -- List of inputs passed to Selection.RequiredSelections
        decDescriptors -- List of decay descriptors for CombineParticles
        """
        combCuts = '((AM - AM1) < {0[Dst_AMDiff_MAX]})'.format(self.config)

        daugCuts = ( '(TRCHI2DOF<{0[Track_CHI2]})').format(self.config)
        dstarCuts = '(VFASPF(VCHI2/VDOF) < {0[Dst_VCHI2VDOF_MAX]})'.format( self.config)

        _Dst = CombineParticles(
            #name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            CombinationCut=combCuts,
            MotherCut=dstarCuts,
            DaughtersCuts={ 'pi+': daugCuts }
        )

        return Selection(name, Algorithm=_Dst, RequiredSelections=inputSel)
    
    def makeThreeProng(self, name, inputSel, decDescriptors,particle):
        """Return a Selection instance for a D+/Ds+/Lc+ -> h+ h+ h- decay.

        Keyword arguments:
        name -- Name to give the Selection instance
        inputSel -- List of inputs passed to Selection.RequiredSelections
        decDescriptors -- List of decay descriptors for CombineParticles
        """
        #from the string to guess the particle nick name
        _part = {
              "D+":"Dp",
              "D-":"Dp",
              "D_s+":"Ds",
              "D_s-":"Ds",
              "Lambda_c+":"Lc",
              "Lambda_c~-":"Lc",
              }
        nickname =  _part[particle]
        #if particle not in _part.keys:
        #   print "particle name required!!"

        lclPreambulo = [
            'from math import cos'
        ]

        daugCuts = (
            '(PT > {0[D_Daug_All_PT_MIN]})'
            '&(TRCHI2DOF<{0[Track_CHI2]})'
            '& (BPVIPCHI2() > {0[D_Daug_All_BPVIPCHI2_MIN]})'
        ).format(self.config)
        pidFiducialCuts = (
            '(in_range({0[D_Daug_P_MIN]}, P, {0[D_Daug_P_MAX]}))'
            '& (in_range({0[D_Daug_ETA_MIN]}, ETA, {0[D_Daug_ETA_MAX]}))'
        ).format(self.config)
        pionPIDCuts = (
            pidFiducialCuts +
            '& (PIDK-PIDpi < {0[D_Pi_PIDK_MAX]})'
        ).format(self.config)
        kaonPIDCuts = (
            pidFiducialCuts +
            '& (PIDK-PIDpi > {0[D_K_PIDK_MIN]})'
        ).format(self.config)
        protonPIDCuts = (
            pidFiducialCuts +
            '& (PIDp-PIDpi > {0[D_P_PIDp_MIN]})'
            '& (PIDp-PIDK > {0[D_P_PIDpPIDK_MIN]})'
        ).format(self.config)

        _dauCuts = {
              "Dp":{
                 "pi+": '{0} & {1}'.format(daugCuts, pionPIDCuts),
                 "K+" : '{0} & {1}'.format(daugCuts, kaonPIDCuts)
              },
              "Ds":{
                 "pi+": '{0} & {1}'.format(daugCuts, pionPIDCuts),
                 "K+" : '{0} & {1}'.format(daugCuts, kaonPIDCuts)
              },
              "Lc":{
                 "pi+": '{0} & {1}'.format(daugCuts, pionPIDCuts),
                 "K+" : '{0} & {1}'.format(daugCuts, kaonPIDCuts),
                 "p+" : '{0} & {1}'.format(daugCuts, protonPIDCuts)
              }
        }
       

        # Add 10 MeV either side of the ADAMASS checks to account for the
        # poorer mass resolution before the vertex fit
        combCuts = (
            '(AMAXCHILD(PT) > {0[D_Daug_1of3_PT_MIN]})'
            '& (AMAXCHILD(BPVIPCHI2()) > {0[D_Daug_1of3_BPVIPCHI2_MIN]})'
            '& (ANUM(PT > {0[D_Daug_2of3_PT_MIN]}) >= 2)'
            '& (ANUM(BPVIPCHI2() > {0[D_Daug_2of3_BPVIPCHI2_MIN]}) >= 2)'
            "& (ACUTDOCA({0[D_Comb_ADOCAMAX_MAX]}, ''))"
        ).format(self.config)

        combCuts = combCuts +  ("&(ADAMASS('{0}') < {1})").format(particle,self.config[ nickname+"_ADAMASS_WIN" ])
        dCuts = (
            '(VFASPF(VCHI2/VDOF) < {0[D_VCHI2VDOF_MAX]})'
            '& (BPVDIRA > cos({0[D_acosBPVDIRA_MAX]}))'
        ).format(self.config)
        dCuts = dCuts + ('& ({0})').format(self.config[ nickname+"_PVDispCut" ])

        _dplus = CombineParticles(
            #name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            Preambulo=lclPreambulo,
            CombinationCut=combCuts,
            MotherCut=dCuts,
            DaughtersCuts=_dauCuts[nickname]
        )

        return Selection(name, Algorithm=_dplus, RequiredSelections=inputSel)

