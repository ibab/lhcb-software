
"""
Module for construction of D0->KSKS taged and untaged (stripping Selections and StrippingLines.
Provides functions to build, KS->LL (from StdLooseKsLL), KS->DD (from StdLooseKsDD), D0->KSLL KSLL (D0LL), D0->KSLL KSDD (D0LD) and D0->KSDD KSDD (D0DD) selections.
Provides class D02KSKSConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
Under usage of the KS-cut variable
nu'_2 = log((piplus_BPVIPCHI2*piminus_BPVIPCHI2)/(KS0_BPVIPCHI2^2 + KS0_DOCAMAX^2))
Exported symbols (use python help!):
    - D02KSKSConf
"""

__author__ = ['Markward Britsch','Marianna Fontana', 'Michael Sokoloff']
__date__ = '15/05/2014'
__version__ = 'Stripping23'
__all__ = 'D02KSKSConf'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdAllNoPIDsPions as Pions
#from StandardParticles import StdNoPIDsPions as Pions
#from StandardParticles import StdLoosePions as Pions
from StandardParticles import StdNoPIDsDownPions as DownPions
from StandardParticles import StdNoPIDsUpPions as UpPions
#from StandardParticles import StdLooseKaons as Kaons


default_config = {
    'NAME'          : 'D02KSKS',
    'WGs'           : ['Charm'],
    'BUILDERTYPE'   : 'D02KSKSConf',
    'CONFIG'        : {'D0_MassWindowBeforeFit'          : 150.0, # D0 mass window cut in MeV before vertex fit
                       'D0_MassWindow'                   : 100.0, # D0 mass window cut in MeV 
                       'D0_DOCA_LL'                      : 1.0, # DOCA cut in mm for D0->KSLL KSLL
                       'D0_DOCA_DD'                      : 4.0, # DOCA cut in mm for D0->KSLL KSDD and D0->KSDD KSDD
                       'KS_LL_nu2prime'                  : 0.0, # nu'_2 = log((piplus_BPVIPCHI2*piminus_BPVIPCHI2)/(KS0_BPVIPCHI2^2 + KS0_DOCAMAX^2)), preselection cut for KsLL
                       'KS_LL_signedFLchi2'              : 50.0, # KsLL siged flight length chi2 preselection cut
                       'KS_LL_TRGHOSTPROB'		    : 1.0,		# Track ghost probability KSLL
                       'KS_DD_nu2prime'                  : 0.0, # nu'_2, preselection cut for KsLL
                       'KS_DD_signedFLchi2'              : 50.0, # KsDD siged flight length chi2 preselection cut
                       'KS_DD_TRGHOSTPROB'		    : 1.0,		# Track ghost probability KSDD
                       
                       'D0_vertexChi2_LL'                : 20.0, # D0LL reduced vertex chi2 cut
                       'D0_IPchi2_LL'                    : 50.0, # D0LL IP chi2 cut
                       'D0_KS_signedFLchi2_LL'           : 200.0, # D0LL signed flight length of the Ks
                       'D0_signedFLchi2_LL'              : -1.0, # D0LL signed flight length chi2 cut
                       
                       'D0_vertexChi2_LD'                : 20.0, # D0LD reduced vertex chi2 cut
                       'D0_IPchi2_LD'                    : 10.0, # D0LD IP chi2 cut
                       'D0_KSLL_signedFLchi2_LD'         : 500.0, # D0LD signed flight length of the KsLL cut
                       'D0_signedFLchi2_LD'              : 1.0, # D0LD signed flight length chi2 cut
                       
                       'D0_vertexChi2_DD'                : 20.0, # D0DD reduced vertex chi2 cut
                       'D0_IPchi2_DD'                    : 10.0, # D0DD IP chi2 cut
                       'D0_KS_nu2prime'                  : 4.0, # D0DD KS nu'_2 cut
                       'D0_lnPt_DD'                      : 21.0, # D0DD ln(D0_PT*KS0_PT*KS00_PT)
                       
                       'slowPi_IPchi2_LL'                : 40.0, # slow pi IP chi2 cut for LL
                       'slowPi_IPchi2_LD'                : 40.0, # slow pi IP chi2 cut for LD
                       'slowPi_IPchi2_DD'                : 40.0, # slow pi IP chi2 cut for DD
	
                       'slowPi_LLL_TRGHOSTPROB'	    : 1.0,  # Track ghost probability slow pi LL
                       'slowPi_LDL_TRGHOSTPROB'	    : 1.0,  # Track ghost probability slow pi LD
                       'slowPi_DDL_TRGHOSTPROB'	    : 1.0,  # Track ghost probability slow pi DD
                       
                       'slowPi_LLU_TRGHOSTPROB'	    : 1.0,  # Track ghost probability slow pi LL
                       'slowPi_LDU_TRGHOSTPROB'	    : 1.0,  # Track ghost probability slow pi LD
                       'slowPi_DDU_TRGHOSTPROB'	    : 1.0,  # Track ghost probability slow pi DD
	
                       'Dst_mass_LL'                     : 200, # mass window on D* cut LL in MeV
                       'Dst_mass_LD'                     : 200, # mass window on D* cut LD in MeV
                       'Dst_mass_DD'                     : 200, # mass window on D* cut DD in MeV
                       'Dst_vertexChi2_LL'               : 40.0, # Dst reduces vertex chi2 cut LL
                       'Dst_vertexChi2_LD'               : 40.0, # Dst reduces vertex chi2 cut LD
                       'Dst_vertexChi2_DD'               : 40.0, # Dst reduces vertex chi2 cut DD

                       'Dst_massDiffLow_LL'              : 135, # m_D* - m_D0 low mass difference cut in MeV
                       'Dst_massDiffHigh_LL'             : 160, # m_D* - m_D0 high mass difference cut in MeV
                       'Dst_massDiffLow_LD'              : 135, # m_D* - m_D0 low mass difference cut in MeV
                       'Dst_massDiffHigh_LD'             : 160, # m_D* - m_D0 high mass difference cut in MeV
                       'Dst_massDiffLow_DD'              : 135, # m_D* - m_D0 low mass difference cut in MeV
                       'Dst_massDiffHigh_DD'             : 160, # m_D* - m_D0 high mass difference cut in MeV
                  
                       'PrescaleLL'                        : 1.0,
                       'PostscaleLL'                       : 1.0,
                       'PrescaleLD'                        : 1.0,
                       'PostscaleLD'                       : 1.0,
                       'PrescaleDD'                        : 1.0,
                       'PostscaleDD'                       : 1.0,
                       
                       'DstPrescaleLL'                        : 1.0,
                       'DstPostscaleLL'                       : 1.0,
                       'DstPrescaleLD'                        : 1.0,
                       'DstPostscaleLD'                       : 1.0,
                       'DstPrescaleDD'                        : 1.0,
                       'DstPostscaleDD'                       : 1.0
                       
                       },
    'STREAMS'      : ['Charm']
    }

class D02KSKSConf(LineBuilder) :
    """
    Builder of D0->KSKS stripping Selection and StrippingLine.
    Constructs D0 -> KS KS Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> bs2ksksConf = D02KSKSConf('D02KSKSTest',config)
    >>> bs2ksksLines = bs2ksksConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:

    selKS2LL               : KS -> Long Long Selection object

    selKS2DD              : KS -> Downstream Downstream Selection object

    selD02KSLLKSLL            : D0 -> KS(LL) KS(LL) Selection object
    ll_line                : StrippingLine made out of selD02KSLLKSLL
    lines                  : List of lines, [ll_line]

    selD02KSLLKSDD           : D0 -> KS(LL) KS(DD) Selection object
    ld_line                : StrippingLine made out of selD02KSLLKSDD
    lines                  : List of lines, [ld_line]

    selD02KSDDKSDD            : D0 -> KS(DD) KS(DD) Selection object
    dd_line                : StrippingLine made out of selD02KSDDKSDD
    lines                  : List of lines, [dd_line]

    selDst2D0LL_L          : D*+ -> pi+(L) (D0 -> KS(LL) KS(LL)) Selection object
    Dst_ll_l_line              : StrippingLine made out of selD02KSLLKSLL
    lines                  : List of lines, [Dst_ll_l_line]

    selDst2D0LD_L             : D*+ -> pi+(L) (D0 -> KS(LL) KS(DD)) Selection object
    Dst_ld_l_line              : StrippingLine made out of selD02KSLLKSDD
    lines                  : List of lines, [Dst_ld_l_line]

    selDst2D0DD_L            : D*+ -> pi+(L) (D0 -> KS(DD) KS(DD)) Selection object
    Dst_dd_l_line              : StrippingLine made out of selD02KSDDKSDD
    lines                  : List of lines, [Dst_dd_l_line]

    selDst2D0LL_U          : D*+ -> pi+(U) (D0 -> KS(LL) KS(LL)) Selection object
    Dst_ll_u_line              : StrippingLine made out of selD02KSLLKSLL
    lines                  : List of lines, [Dst_ll_u_line]

    selDst2D0LD_U             : D*+ -> pi+(U) (D0 -> KS(LL) KS(DD)) Selection object
    Dst_ld_u_line              : StrippingLine made out of selD02KSLLKSDD
    lines                  : List of lines, [Dst_ld_u_line]

    selDst2D0DD_U            : D*+ -> pi+(U) (D0 -> KS(DD) KS(DD)) Selection object
    Dst_dd_u_line              : StrippingLine made out of selD02KSDDKSDD
    lines                  : List of lines, [Dst_dd_u_line]

    Exports as class data member:
    D02KSKSConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('D0_MassWindowBeforeFit',
                              'D0_MassWindow',
                              'D0_DOCA_LL',
                              'D0_DOCA_DD',
                              'KS_LL_nu2prime',
                              'KS_LL_signedFLchi2',
			      'KS_LL_TRGHOSTPROB',
                              'KS_DD_nu2prime',
                              'KS_DD_signedFLchi2',
			      'KS_DD_TRGHOSTPROB',
                              'D0_vertexChi2_LL',
                              'D0_IPchi2_LL',
                              'D0_KS_signedFLchi2_LL', # signed flight length of the Ks
                              'D0_signedFLchi2_LL',
                              'D0_vertexChi2_LD',
                              'D0_IPchi2_LD',
                              'D0_KSLL_signedFLchi2_LD', # signed flight length of the KsLL
                              'D0_signedFLchi2_LD',
                              'D0_vertexChi2_DD',
                              'D0_IPchi2_DD',
                              'D0_KS_nu2prime', # nu'_2
                              'D0_lnPt_DD', # ln(D0_PT*KS0_PT*KS00_PT)
                              'slowPi_IPchi2_LL', # slow pi IP chi2 cut for LL
                              'slowPi_IPchi2_LD', # slow pi IP chi2 cut for LD
                              'slowPi_IPchi2_DD', # slow pi IP chi2 cut for DD
			      'slowPi_LLL_TRGHOSTPROB', 
      			      'slowPi_LDL_TRGHOSTPROB',
      			      'slowPi_DDL_TRGHOSTPROB',
      			      'slowPi_LLU_TRGHOSTPROB', 
      			      'slowPi_LDU_TRGHOSTPROB',
      			      'slowPi_DDU_TRGHOSTPROB',  
                              'Dst_mass_LL', # mass window on D* cut LL
                              'Dst_mass_LD', # mass window on D* cut LD
                              'Dst_mass_DD', # mass window on D* cut DD
                              'Dst_vertexChi2_LL', # Dst reduces vertex chi2 cut LL
                              'Dst_vertexChi2_LD', # Dst reduces vertex chi2 cut LD
                              'Dst_vertexChi2_DD', # Dst reduces vertex chi2 cut DD
                              'Dst_massDiffLow_LL',
                              'Dst_massDiffHigh_LL',
                              'Dst_massDiffLow_LD',
                              'Dst_massDiffHigh_LD',
                              'Dst_massDiffLow_DD',
                              'Dst_massDiffHigh_DD',
                              'PrescaleLL',
                              'PostscaleLL',
                              'PrescaleLD',
                              'PostscaleLD',
                              'PrescaleDD',
                              'PostscaleDD',
                              'DstPrescaleLL',
                              'DstPostscaleLL',
                              'DstPrescaleLD',
                              'DstPostscaleLD',
                              'DstPrescaleDD',
                              'DstPostscaleDD'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        ll_name = name+'LL'
        dd_name = name+'DD'
        ld_name = name+'LD'
        ll_l_name = name+'LL_L'
        ll_u_name = name+'LL_U'
        dd_l_name = name+'DD_L'
        dd_u_name = name+'DD_U'
        ld_l_name = name+'LD_L'
        ld_u_name = name+'LD_U'

        #GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
        #    'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.pions = Pions
        self.down_pions = DownPions
        self.up_pions = UpPions
# no Kaons anymore        self.kaons = Kaons

#        self.hadrons = MergedSelection("HadronsFor" + name,
#                                       RequiredSelections = [ self.pions, self.kaons ] )

        # no merged selection required , because Kaon+/- are no longer considered

# D0 lineKs creation:
#        self.makeKS2DD( 'KSfor'+dd_name, config )
        self.makeKS2LL( 'KSfor'+ll_name, config )
        self.makeKS2DD( 'KSfor'+dd_name, config )


# D0 lines:
#        self.makeD02KSDDhh( dd_name, config )
        self.makeD02KSLLKSLL( ll_name, config )


        self.ll_line = StrippingLine(ll_name+"Line",
            prescale = config['PrescaleLL'],
            postscale = config['PostscaleLL'],
            selection =  self.selD02KSLLKSLL#,
            #FILTER = GECCode
            )

#        self.registerLine(self.dd_line)
        self.registerLine(self.ll_line)

        self.makeD02KSLLKSDD( ld_name, config )


        self.ld_line = StrippingLine(ld_name+"Line",
            prescale = config['PrescaleLD'],
            postscale = config['PostscaleLD'],
            selection =  self.selD02KSLLKSDD#,
            #FILTER = GECCode
            )

#        self.registerLine(self.dd_line)
        self.registerLine(self.ld_line)

        self.makeD02KSDDKSDD( dd_name, config )


        self.dd_line = StrippingLine(dd_name+"Line",
            prescale = config['PrescaleDD'],
            postscale = config['PostscaleDD'],
            selection =  self.selD02KSDDKSDD#,
            #FILTER = GECCode
            )

#        self.registerLine(self.dd_line)
        self.registerLine(self.dd_line)

# Dst lines with LONG slow pion:
        self.makeDst2D0LL_L( 'Dst'+ll_l_name, config )
        self.Dst_ll_l_line = StrippingLine('Dst'+ll_l_name+"Line",
            prescale = config['DstPrescaleLL'],
            postscale = config['DstPostscaleLL'],
            selection =  self.selDst2D0LL_L#,
            #FILTER = GECCode
            )
        self.registerLine(self.Dst_ll_l_line)

        self.makeDst2D0LD_L( 'Dst'+ld_l_name, config )
        self.Dst_ld_l_line = StrippingLine('Dst'+ld_l_name+"Line",
            prescale = config['DstPrescaleLD'],
            postscale = config['DstPostscaleLD'],
            selection =  self.selDst2D0LD_L#,
            #FILTER = GECCode
            )
        self.registerLine(self.Dst_ld_l_line)

        self.makeDst2D0DD_L( 'Dst'+dd_l_name, config )
        self.Dst_dd_l_line = StrippingLine('Dst'+dd_l_name+"Line",
            prescale = config['DstPrescaleDD'],
            postscale = config['DstPostscaleDD'],
            selection =  self.selDst2D0DD_L#,
            #FILTER = GECCode
            )
        self.registerLine(self.Dst_dd_l_line)

# Dst lines with UP slow pion:
        self.makeDst2D0LL_U( 'Dst'+ll_u_name, config )
        self.Dst_ll_u_line = StrippingLine('Dst'+ll_u_name+"Line",
            prescale = config['DstPrescaleLL'],
            postscale = config['DstPostscaleLL'],
            selection =  self.selDst2D0LL_U#,
            #FILTER = GECCode
            )
        self.registerLine(self.Dst_ll_u_line)

        self.makeDst2D0LD_U( 'Dst'+ld_u_name, config )
        self.Dst_ld_u_line = StrippingLine('Dst'+ld_u_name+"Line",
            prescale = config['DstPrescaleLD'],
            postscale = config['DstPostscaleLD'],
            selection =  self.selDst2D0LD_U#,
            #FILTER = GECCode
            )
        self.registerLine(self.Dst_ld_u_line)

        self.makeDst2D0DD_U( 'Dst'+dd_u_name, config )
        self.Dst_dd_u_line = StrippingLine('Dst'+dd_u_name+"Line",
            prescale = config['DstPrescaleDD'],
            postscale = config['DstPostscaleDD'],
            selection =  self.selDst2D0DD_U#,
            #FILTER = GECCode
            )
        self.registerLine(self.Dst_dd_u_line)


    def makeKS2LL( self, name, config ) :
#        # define all the cuts
#
#        #1st daughters cuts 
#
#
#        # before vretex fit cuts 
#
##        _DaughterCuts = {"pi-" : _cut_prob_KNDL,
##                         "pi+" : _cut_prob_KPDL}
##        _DaughterCuts = {"pi+" : "(P>2*GeV)"}
#
#        _CombinationCuts = "(ACUTDOCA(1.0*mm,'')) & (ADAMASS('KS0')<50*MeV)"
#
#
#
##        _trkChi2Cut1 = "(CHILDCUT((TRCHI2DOF<%s),1))" % config['Trk_Chi2']
##        _trkChi2Cut2 = "(CHILDCUT((TRCHI2DOF<%s),2))" % config['Trk_Chi2']
#        _MotherCuts = "PT>-100"
#
#        ########## EndCuts definition
#
#
#
#        # make the Ks from StdAllNoPIDsPions
#        _Ks = CombineParticles( DecayDescriptor = "KS0 -> pi+ pi-" ,
#                                #DaughtersCuts = _DaughterCuts,
#                                CombinationCut = _CombinationCuts,
#                                MotherCut = _MotherCuts)
#
#
        ## get the KS's to filter
        _stdKSLL = DataOnDemand( Location = "Phys/StdLooseKsLL/Particles" )
        #_allCuts = "(DOCAMAX < 1.0*mm)  & (ADMASS('KS0')<50*MeV)"
        #_doca_cut = "(DOCAMAX < %s*mm)" % config['KS_LL_DOCA']
        #_mass_cut = "(ADMASS('KS0')<%s*MeV)" % config['KS_LL_MASS']
        _nu2prime_cut = "(log((CHILD(BPVIPCHI2(),1)*CHILD(BPVIPCHI2(),2))/(BPVIPCHI2()*BPVIPCHI2() + DOCAMAX*DOCAMAX)) > %s)" % config['KS_LL_nu2prime'] # 0
        _signedFLchi2_cut = "(BPVLTSIGNCHI2() > %s)" % config['KS_LL_signedFLchi2'] # 50
        _trghost_cut1 = "(CHILD(TRGHOSTPROB, 1) <=%s )" % config['KS_LL_TRGHOSTPROB']
        _trghost_cut2 = "(CHILD(TRGHOSTPROB, 2) <=%s )" % config['KS_LL_TRGHOSTPROB']
	
        _allCuts = _nu2prime_cut + "&" + _signedFLchi2_cut + "&" + _trghost_cut1 + "&" + _trghost_cut2
        
        ## make the filter
        _filterKSLL = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        #self.selKS2LL = Selection( "selKS2LL_" + name, Algorithm = _Ks, RequiredSelections = [ self.pions  ] )
        #self.selKS2LL = Selection( "selKS2LL_" + name, Algorithm = _Ks, RequiredSelections = [ _selPions]  )
        self.selKS2LL = Selection( "selKS2LL_" + name, Algorithm = _filterKSLL, RequiredSelections = [_stdKSLL] )


    def makeKS2DD( self, name, config ) :

        # define all the cuts

        #1st daughters cuts 


        # before vretex fit cuts 


#        #_DaughterCuts = {"pi+" : "(P>2*GeV) & (MIPDV(PRIMARY)<100.*mm) & (PT>200*MeV)"}
#        _DaughterCuts = {}
#        
#        _CombinationCuts = "(ACUTDOCA(4.0*mm,'')) & (ADAMASS('KS0')<100*MeV)"
#
#
#
##        _trkChi2Cut1 = "(CHILDCUT((TRCHI2DOF<%s),1))" % config['Trk_Chi2']
##        _trkChi2Cut2 = "(CHILDCUT((TRCHI2DOF<%s),2))" % config['Trk_Chi2']
#        _MotherCuts = "PT>-100"
#
#        ########## EndCuts definition
#
#
#
#        # make the Ks from StdAllNoPIDsPions
#        _Ks = CombineParticles( DecayDescriptor = "KS0 -> pi+ pi-" ,
#                                DaughtersCuts = _DaughterCuts,
#                                CombinationCut = _CombinationCuts,
#                                MotherCut = _MotherCuts)
#
        # get the KS's to filter
        _stdKSDD = DataOnDemand( Location = "Phys/StdLooseKsDD/Particles" )
#        _allCuts = "(DOCAMAX<4.0*mm) & (ADMASS('KS0')<100*MeV)" 
#        _doca_cut = "(DOCAMAX < %s*mm)" % config['KS_DD_DOCA']
#        _mass_cut = "(ADMASS('KS0')<%s*MeV)" % config['KS_DD_MASS']
#        _allCuts = _doca_cut + "&" + _mass_cut
        _nu2prime_cut = "(log((CHILD(BPVIPCHI2(),1)*CHILD(BPVIPCHI2(),2))/(BPVIPCHI2()*BPVIPCHI2() + DOCAMAX*DOCAMAX)) > %s)" % config['KS_DD_nu2prime'] # 0
        _signedFLchi2_cut = "(BPVLTSIGNCHI2() > %s)" % config['KS_DD_signedFLchi2'] # 50
        _trghost_cut1 = "(CHILD(TRGHOSTPROB, 1) <=%s )" % config['KS_DD_TRGHOSTPROB']
        _trghost_cut2 = "(CHILD(TRGHOSTPROB, 2) <=%s )" % config['KS_DD_TRGHOSTPROB']
	
        _allCuts = _nu2prime_cut + "&" + _signedFLchi2_cut + "&" + _trghost_cut1 + "&" + _trghost_cut2

        # make the filter
        _filterKSDD = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        #self.selKS2DD = Selection( "selKS2DD_" + name, Algorithm = _Ks, RequiredSelections = [ self.down_pions  ] )
        self.selKS2DD = Selection( "selKS2DD_" + name, Algorithm = _filterKSDD, RequiredSelections = [_stdKSDD] )


    def makeD02KSDDKSDD( self, name, config ) :
        """
        Create and store a D0 -> KS(DD) KS(DD) Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """
	# The following lines were produced automatically; preambuloProbDef should be put into the preambulo of CombineParticles

        _daughterCuts = "(log((CHILD(BPVIPCHI2(),1)*CHILD(BPVIPCHI2(),2))/(BPVIPCHI2()*BPVIPCHI2() + DOCAMAX*DOCAMAX)) > %s)" % config['D0_KS_nu2prime'] # 4 # nu'_2

        # before D vertex fit 
        _doca_cut = "(ACUTDOCA(%s*mm,''))" % config['D0_DOCA_DD']
        _massBefore_cut = "(ADAMASS('D0') < %s*MeV)" % config['D0_MassWindowBeforeFit'] # 150
        _combCuts = _doca_cut + "&" + _massBefore_cut
        #_combCuts = "(ACUTDOCA(4.0*mm,'')) & (ADAMASS('D0') < 600*MeV)"

        _vertexChi2_cut = "(VFASPF(VCHI2/VDOF) < %s)" % config['D0_vertexChi2_DD'] # 20
        _IPchi2_cut = "(BPVIPCHI2() < %s)" % config['D0_IPchi2_DD'] # 10
        _lnPt_cut = "(log(PT*CHILD(PT,1)*CHILD(PT,2)) > %s) " % config['D0_lnPt_DD'] # 21
        _mass_cut = "(ADMASS('D0') < %s*MeV)" % config['D0_MassWindow'] # 100
        
        _motherCuts = _vertexChi2_cut + "&" + _IPchi2_cut + "&" + _lnPt_cut + "&" + _mass_cut
        
        _D = CombineParticles()
        _D.DecayDescriptors = [ "D0 -> KS0 KS0" ]
        
        _D.CombinationCut = _combCuts
        _D.DaughtersCuts = { "KS0" : _daughterCuts }
        #_D.DaughtersCuts = _daughterCuts
        _D.MotherCut = _motherCuts

#        _Preambulo = preambuloDef_prob_BCu
#    print 'B combCuts: ',_combCuts
#    print 'B motherCuts: ',_motherCuts
#        print 'prescale: ',prescale

#        _D.Preambulo        = _Preambulo
        self.selD02KSDDKSDD = Selection (name, Algorithm = _D, RequiredSelections = [ self.selKS2DD ])
        
    def makeD02KSLLKSDD( self, name, config ) :
        """
        Create and store a D0 -> KS(LL) KS(DD) Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """
        
        KsKsLD_mother_cut = "INTREE((ABSID=='pi+') & (ISLONG)) & INTREE((ABSID=='pi+') & (ISDOWN))"

        _daughterCuts = "(INTREE((ABSID=='pi+') & (ISDOWN)) | (BPVLTSIGNCHI2() > %s))" % config['D0_KSLL_signedFLchi2_LD'] # 500, signed flight length of the KsLL
        
        # before D vertex fit 
        _doca_cut = "(ACUTDOCA(%s*mm,''))" % config['D0_DOCA_DD']
        _massBefore_cut = "(ADAMASS('D0') < %s*MeV)" % config['D0_MassWindowBeforeFit']
        _combCuts = _doca_cut + "&" + _massBefore_cut
        #_combCuts = "(ACUTDOCA(4.0*mm,'')) & (ADAMASS('D0') < 600*MeV)"
        
        _vertexChi2_cut = "(VFASPF(VCHI2/VDOF) < %s)" % config['D0_vertexChi2_LD'] # 20
        _IPchi2_cut = "(BPVIPCHI2() < %s)" % config['D0_IPchi2_LD'] # 10
        _signedFLchi2_cut = "(BPVLTSIGNCHI2() > %s)" % config['D0_signedFLchi2_LD'] # 1.0
        _mass_cut = "(ADMASS('D0') < %s*MeV)" % config['D0_MassWindow'] # 100

        _motherCuts = KsKsLD_mother_cut + " & " + _vertexChi2_cut + " & " + _IPchi2_cut + " & " + _signedFLchi2_cut + " & " + _mass_cut
        
        _D = CombineParticles()
        _D.DecayDescriptors = [ "D0 -> KS0 KS0" ]
        
        #_D.DaughtersCuts = _daughterCuts
        _D.DaughtersCuts = { "KS0" : _daughterCuts }
        _D.CombinationCut = _combCuts #+  " & " + ? #_cut_prob_BCC
        _D.MotherCut = _motherCuts

#        _Preambulo = preambuloDef_prob_BCC + preambuloDef_prob_BCu
#    print 'B combCuts: ',_combCuts
#    print 'B motherCuts: ',_motherCuts
#        print 'prescale: ',prescale

#        _B.Preambulo        = _Preambulo
        self.selD02KSLLKSDD = Selection (name, Algorithm = _D, RequiredSelections = [ self.selKS2LL, self.selKS2DD ])

    def makeD02KSLLKSLL( self, name, config ) :
        """
        Create and store a B -> KS(LL) KS(LL) Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        
        _daughterCuts = "(BPVLTSIGNCHI2() > %s)" % config['D0_KS_signedFLchi2_LL'] # 200, signed flight length of the Ks
        
        # before D vertex fit
        _doca_cut = "(ACUTDOCA(%s*mm,''))" % config['D0_DOCA_LL']
        _massBefore_cut = "(ADAMASS('D0') < %s*MeV)" % config['D0_MassWindowBeforeFit']
        _combCuts = _doca_cut + "&" + _massBefore_cut
        ##_combCuts = "(ACUTDOCA(1.0*mm,'')) & (ADAMASS('D0') < 600*MeV)"
        
        _vertexChi2_cut = "(VFASPF(VCHI2/VDOF) < %s)" % config['D0_vertexChi2_LL'] # 20
        _IPchi2_cut = "(BPVIPCHI2() < %s)" % config['D0_IPchi2_LL'] # 50
        _signedFLchi2_cut = "(BPVLTSIGNCHI2() > %s)" % config['D0_signedFLchi2_LL'] # -1.0
        _mass_cut = "(ADMASS('D0') < %s*MeV)" % config['D0_MassWindow'] # 100

        _motherCuts =  _vertexChi2_cut + " & " + _IPchi2_cut + " & " + _signedFLchi2_cut  + " & " + _mass_cut # _cut_prob_BCu
        
        _D = CombineParticles()
        _D.DecayDescriptors = [ "D0 -> KS0 KS0" ]
        
        _D.DaughtersCuts = { "KS0" : _daughterCuts }
        #_D.DaughtersCuts = _daughterCuts
        _D.CombinationCut = _combCuts #+  " & " + ? # _cut_prob_BCC
        _D.MotherCut = _motherCuts

        self.selD02KSLLKSLL = Selection (name, Algorithm = _D, RequiredSelections = [ self.selKS2LL ] )

    def makeDst2D0LL_L( self, name, config ) :
        """
        Create and store a Dst+ -> pi+ (D0 -> KS(LL) KS(LL)) Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        #_daughterCuts = "(BPVIPCHI2() < %s)" % config['D0_IPchi2_LL'] # 40
        _daughterCuts = "(TRGHOSTPROB <=%s )" % config['slowPi_LLL_TRGHOSTPROB']
        
        _vertexChi2_cut = "(VFASPF(VCHI2/VDOF) < %s)" % config['Dst_vertexChi2_LL'] # 40

        _combCuts  = "(ADAMASS('D*(2010)+') < %s*MeV)" % config['Dst_mass_LL'] # 200
        _massDiffLow_cut = "(switch(CHILD(ABSID, 2) == 211, MM - CHILD(MM,1), MM - CHILD(MM,2)) > %s*MeV)" % config['Dst_massDiffLow_LL'] # 135
        _massDiffHigh_cut = "(switch(CHILD(ABSID, 2) == 211, MM - CHILD(MM,1), MM - CHILD(MM,2)) < %s*MeV)" % config['Dst_massDiffHigh_LL'] # 160
        #_motherCuts =  _vertexChi2_cut + " & " + _massDiffLow_cut + " & " + _massDiffHigh_cut
        _motherCuts =  _massDiffLow_cut + " & " + _massDiffHigh_cut
        
        _D = CombineParticles()
        #_D.DecayDescriptors = [ "[D*(2010)+ -> (D0-> (KS0 -> pi+ pi-) (KS0 -> pi+ pi-)) pi+]cc" ]
        #_D.DecayDescriptors = [ "[D*(2010)+ -> D0 pi+]cc" ]
        _D.DecayDescriptors = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
        
        _D.DaughtersCuts = { "pi+" : _daughterCuts }
        _D.CombinationCut = _combCuts #+  " & " + ? # _cut_prob_BCC
        _D.MotherCut = _motherCuts

        self.selDst2D0LL_L = Selection (name, Algorithm = _D, RequiredSelections = [ self.pions, self.selD02KSLLKSLL ] )

    def makeDst2D0LL_U( self, name, config ) :
        """
        Create and store a Dst+ -> pi+ (D0 -> KS(LL) KS(LL)) Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        #_daughterCuts = "(BPVIPCHI2() < %s)" % config['D0_IPchi2_LL'] # 40
        _daughterCuts = "(TRGHOSTPROB <=%s )" % config['slowPi_LLU_TRGHOSTPROB']

        _vertexChi2_cut = "(VFASPF(VCHI2/VDOF) < %s)" % config['Dst_vertexChi2_LL'] # 40

        _combCuts  = "(ADAMASS('D*(2010)+') < %s*MeV)" % config['Dst_mass_LL'] # 200
        _massDiffLow_cut = "(switch(CHILD(ABSID, 2) == 211, MM - CHILD(MM,1), MM - CHILD(MM,2)) > %s*MeV)" % config['Dst_massDiffLow_LL'] # 135
        _massDiffHigh_cut = "(switch(CHILD(ABSID, 2) == 211, MM - CHILD(MM,1), MM - CHILD(MM,2)) < %s*MeV)" % config['Dst_massDiffHigh_LL'] # 160
        #_motherCuts =  _vertexChi2_cut + " & " + _massDiffLow_cut + " & " + _massDiffHigh_cut
        _motherCuts =  _massDiffLow_cut + " & " + _massDiffHigh_cut
        
        _D = CombineParticles()
        #_D.DecayDescriptors = [ "[D*(2010)+ -> (D0-> (KS0 -> pi+ pi-) (KS0 -> pi+ pi-)) pi+]cc" ]
        #_D.DecayDescriptors = [ "[D*(2010)+ -> D0 pi+]cc" ]
        _D.DecayDescriptors = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
        
        _D.DaughtersCuts = { "pi+" : _daughterCuts }
        _D.CombinationCut = _combCuts #+  " & " + ? # _cut_prob_BCC
        _D.MotherCut = _motherCuts

        self.selDst2D0LL_U = Selection (name, Algorithm = _D, RequiredSelections = [ self.up_pions, self.selD02KSLLKSLL ] )

    def makeDst2D0LD_L( self, name, config ) :
        """
        Create and store a Dst+ -> pi+ (D0 -> KS(LL) KS(DD)) Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        #_daughterCuts = "(BPVIPCHI2() < %s)" % config['D0_IPchi2_LD'] # 40
        _daughterCuts = "(TRGHOSTPROB <=%s )" % config['slowPi_LDL_TRGHOSTPROB']
        
        _vertexChi2_cut = "(VFASPF(VCHI2/VDOF) < %s)" % config['Dst_vertexChi2_LD'] # 40

        _combCuts  = "(ADAMASS('D*(2010)+') < %s*MeV)" % config['Dst_mass_LD'] # 200
        _massDiffLow_cut = "(switch(CHILD(ABSID, 2) == 211, MM - CHILD(MM,1), MM - CHILD(MM,2)) > %s*MeV)" % config['Dst_massDiffLow_LD'] # 135
        _massDiffHigh_cut = "(switch(CHILD(ABSID, 2) == 211, MM - CHILD(MM,1), MM - CHILD(MM,2)) < %s*MeV)" % config['Dst_massDiffHigh_LD'] # 160
        #_motherCuts =  _vertexChi2_cut + " & " + _massDiffLow_cut + " & " + _massDiffHigh_cut
        _motherCuts =  _massDiffLow_cut + " & " + _massDiffHigh_cut
        
        _D = CombineParticles()
        #_D.DecayDescriptors = [ "[D*(2010)+ -> (D0-> (KS0 -> pi+ pi-) (KS0 -> pi+ pi-)) pi+]cc" ]
        #_D.DecayDescriptors = [ "[D*(2010)+ -> D0 pi+]cc" ]
        _D.DecayDescriptors = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
        
        _D.DaughtersCuts = { "pi+" : _daughterCuts }
        _D.CombinationCut = _combCuts #+  " & " + ? # _cut_prob_BCC
        _D.MotherCut = _motherCuts

        self.selDst2D0LD_L = Selection (name, Algorithm = _D, RequiredSelections = [ self.pions, self.selD02KSLLKSDD ] )

    def makeDst2D0LD_U( self, name, config ) :
        """
        Create and store a Dst+ -> pi+ (D0 -> KS(LL) KS(DD)) Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        #_daughterCuts = "(BPVIPCHI2() < %s)" % config['D0_IPchi2_LD'] # 40
        _daughterCuts = "(TRGHOSTPROB <=%s )" % config['slowPi_LDU_TRGHOSTPROB']

        
        _vertexChi2_cut = "(VFASPF(VCHI2/VDOF) < %s)" % config['Dst_vertexChi2_LD'] # 40

        _combCuts  = "(ADAMASS('D*(2010)+') < %s*MeV)" % config['Dst_mass_LD'] # 200
        _massDiffLow_cut = "(switch(CHILD(ABSID, 2) == 211, MM - CHILD(MM,1), MM - CHILD(MM,2)) > %s*MeV)" % config['Dst_massDiffLow_LD'] # 135
        _massDiffHigh_cut = "(switch(CHILD(ABSID, 2) == 211, MM - CHILD(MM,1), MM - CHILD(MM,2)) < %s*MeV)" % config['Dst_massDiffHigh_LD'] # 160
        #_motherCuts =  _vertexChi2_cut + " & " + _massDiffLow_cut + " & " + _massDiffHigh_cut
        _motherCuts =  _massDiffLow_cut + " & " + _massDiffHigh_cut
        
        _D = CombineParticles()
        #_D.DecayDescriptors = [ "[D*(2010)+ -> (D0-> (KS0 -> pi+ pi-) (KS0 -> pi+ pi-)) pi+]cc" ]
        #_D.DecayDescriptors = [ "[D*(2010)+ -> D0 pi+]cc" ]
        _D.DecayDescriptors = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
        
        _D.DaughtersCuts = { "pi+" : _daughterCuts }
        _D.CombinationCut = _combCuts #+  " & " + ? # _cut_prob_BCC
        _D.MotherCut = _motherCuts

        self.selDst2D0LD_U = Selection (name, Algorithm = _D, RequiredSelections = [ self.up_pions, self.selD02KSLLKSDD ] )

    def makeDst2D0DD_L( self, name, config ) :
        """
        Create and store a Dst+ -> pi+ (D0 -> KS(DD) KS(DD)) Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        #_daughterCuts = "(BPVIPCHI2() < %s)" % config['D0_IPchi2_DD'] # 40
        _daughterCuts = "(TRGHOSTPROB <=%s )" % config['slowPi_DDL_TRGHOSTPROB']
        
        _vertexChi2_cut = "(VFASPF(VCHI2/VDOF) < %s)" % config['Dst_vertexChi2_DD'] # 40

        _combCuts  = "(ADAMASS('D*(2010)+') < %s*MeV)" % config['Dst_mass_DD'] # 200
        _massDiffLow_cut = "(switch(CHILD(ABSID, 2) == 211, MM - CHILD(MM,1), MM - CHILD(MM,2)) > %s*MeV)" % config['Dst_massDiffLow_DD'] # 135
        _massDiffHigh_cut = "(switch(CHILD(ABSID, 2) == 211, MM - CHILD(MM,1), MM - CHILD(MM,2)) < %s*MeV)" % config['Dst_massDiffHigh_DD'] # 160
        #_motherCuts =  _vertexChi2_cut + " & " + _massDiffLow_cut + " & " + _massDiffHigh_cut
        _motherCuts =  _massDiffLow_cut + " & " + _massDiffHigh_cut
        
        _D = CombineParticles()
        #_D.DecayDescriptors = [ "[D*(2010)+ -> (D0-> (KS0 -> pi+ pi-) (KS0 -> pi+ pi-)) pi+]cc" ]
        #_D.DecayDescriptors = [ "[D*(2010)+ -> D0 pi+]cc" ]
        _D.DecayDescriptors = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
        
        _D.DaughtersCuts = { "pi+" : _daughterCuts }
        _D.CombinationCut = _combCuts #+  " & " + ? # _cut_prob_BCC
        _D.MotherCut = _motherCuts

        self.selDst2D0DD_L = Selection (name, Algorithm = _D, RequiredSelections = [ self.pions, self.selD02KSDDKSDD ] )

    def makeDst2D0DD_U( self, name, config ) :
        """
        Create and store a Dst+ -> pi+ (D0 -> KS(DD) KS(DD)) Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        #_daughterCuts = "(BPVIPCHI2() < %s)" % config['D0_IPchi2_DD'] # 40
        _daughterCuts = "(TRGHOSTPROB <=%s )" % config['slowPi_DDU_TRGHOSTPROB']
        
        _vertexChi2_cut = "(VFASPF(VCHI2/VDOF) < %s)" % config['Dst_vertexChi2_DD'] # 40

        _combCuts  = "(ADAMASS('D*(2010)+') < %s*MeV)" % config['Dst_mass_DD'] # 200
        _massDiffLow_cut = "(switch(CHILD(ABSID, 2) == 211, MM - CHILD(MM,1), MM - CHILD(MM,2)) > %s*MeV)" % config['Dst_massDiffLow_DD'] # 135
        _massDiffHigh_cut = "(switch(CHILD(ABSID, 2) == 211, MM - CHILD(MM,1), MM - CHILD(MM,2)) < %s*MeV)" % config['Dst_massDiffHigh_DD'] # 160
        #_motherCuts =  _vertexChi2_cut + " & " + _massDiffLow_cut + " & " + _massDiffHigh_cut
        _motherCuts =  _massDiffLow_cut + " & " + _massDiffHigh_cut
        
        _D = CombineParticles()
        #_D.DecayDescriptors = [ "[D*(2010)+ -> (D0-> (KS0 -> pi+ pi-) (KS0 -> pi+ pi-)) pi+]cc" ]
        #_D.DecayDescriptors = [ "[D*(2010)+ -> D0 pi+]cc" ]
        _D.DecayDescriptors = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
        
        _D.DaughtersCuts = { "pi+" : _daughterCuts }
        _D.CombinationCut = _combCuts #+  " & " + ? # _cut_prob_BCC
        _D.MotherCut = _motherCuts

        self.selDst2D0DD_U = Selection (name, Algorithm = _D, RequiredSelections = [ self.up_pions, self.selD02KSDDKSDD ] )
