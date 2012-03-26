"""
Module for construction of Bs->KSKS stripping Selections and StrippingLines.
Provides functions to build, KS->LL, Downstream tracks, KS->LL not implemented, Bs selections (Bd to be implemented).
Provides class Bs2KSKSConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
Exported symbols (use python help!):
   - Bs2KSKSConf
"""

__author__ = ['Florin MACIUC']
__date__ = '13/03/2012'
__version__ = 'Stripping17'
__all__ = 'Bs2KSKSConf'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdAllNoPIDsPions as Pions
#from StandardParticles import StdLooseKaons as Kaons


default_config = {'Trk_Chi2'                : 10.0,

                  'KS_LL_MassWindowP'        : 50.0, # before vertex fit ADAMASS  Ks2PiPi.CombinationCut = "( ADAMASS('KS0') < 50.*MeV )"
                  'KS_LL_MassWindow'        : 30.0,
                  'KS_LL_VtxChi2'           : 100.0, # vertex fit of KS
                  'KS_LL_FDChi2'            : 80.0, # BPVVDCHI2 or the sqrt(pow((KS0_ENDVERTEX_X-KS0_OWNPV_X)/KS0_ENDVERTEX_XERR,2.)+pow((KS0_ENDVERTEX_Z-KS0_OWNPV_Z)/KS0_ENDVERTEX_ZERR,2.)+pow((KS0_ENDVERTEX_Y-KS0_OWNPV_Y)/KS0_ENDVERTEX_YERR,2.))>10&&sqrt(pow((KS00_ENDVERTEX_X-KS00_OWNPV_X)/KS00_ENDVERTEX_XERR,2.)+pow((KS00_ENDVERTEX_Z-KS00_OWNPV_Z)/KS00_ENDVERTEX_ZERR,2.)+pow((KS00_ENDVERTEX_Y-KS00_OWNPV_Y)/KS00_ENDVERTEX_YERR,2.))>10. cut used 

#####################################
#
#                   KS  Dauthers cuts 
#                  
######################################                  

                  'pi_L_MIPDV'              : 20.0,#'(MIPDV(PRIMARY)<20*mm)',
                  'pi_L_Pmin'              : 2000.0, # momentum cut for daughters of pions  MeV

                  'KS_LL_CombMPI'              : 30,   #( sqrt( CHILD( MIPCHI2DV(PRIMARY) , 1 ) * CHILD( MIPCHI2DV(PRIMARY) , 1 ) +  CHILD( MIPCHI2DV(PRIMARY) , 2 ) * CHILD( MIPCHI2DV(PRIMARY ) , 2 ) ) > 30 )

                  'KS_LL_DOCA'                  :0.5,           # (DOCA(1,2) <0.5*mm)
                  'KS_LL_DOCA_ERR'              : 0.3,    
                  'KS_LL_nu'                    : (-2.0),       # nu = log(IP_pi+ * IP_pi- / IP_Ks)

                  'KS_LL_pT'                    : 600, # PT_pi+ + PT_pi- >600 MeV
##################################################
                  'B_MassWindowP'                  : 150.0, # B mass window of 100 MeV : ADAMASS('B_s0') < 50.*MeV 
                  'B_DOCA'                      : 2.5, # 2.5 mm DOCA cut for the two Kaons
                  'B_IPCHI2'                       : 10, # B IP separation wrt PV
                  'B_IP'                           : 0.4, # mm IP(B)<0.4 mm
                  'B_VtxChi2'                        : 20, # B_Vertex_Chi2 / NdoF < 20
                                                        # no ghost cut because no precedent in Stripping
                  'B_BPVVDCHI2'                    : 43.5,  # BPVVDCHI2 of B - separation wrt B - sqrt(B_s0_CHI2_OWNPV)>6.6

                  'GEC_MaxTracks'           : 250, # 
                  'Prescale'                : 1.0,
                  'Postscale'               : 1.0
                  }

class Bs2KSKSConf(LineBuilder) :
    """
    Builder of B->KSKS stripping Selection and StrippingLine.
    Constructs B -> KS KS Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> bs2ksksConf = Bs2KSKSConf('Bs2KSKSTest',config)
    >>> bs2ksksLines = bs2ksksConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:

    selKS2LL               : KS -> Long Long Selection object

    selBs2KSLLKSLL            : B -> KS(LL) KS(LL) Selection object
    ll_line                : StrippingLine made out of selBs2KSLLKSLL
    lines                  : List of lines, [ll_line]

    Exports as class data member:
    Bs2KSKSConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('Trk_Chi2',
                              'KS_LL_MassWindowP',                              
                              'KS_LL_MassWindow',
                              'KS_LL_VtxChi2',
                              'KS_LL_FDChi2',
                              
                              'pi_L_MIPDV',
                              'pi_L_Pmin',
                              'KS_LL_CombMPI',
                              'KS_LL_DOCA',
                              'KS_LL_DOCA_ERR',
                              'KS_LL_nu',
                              'KS_LL_pT',

                              'B_MassWindowP',
                              'B_DOCA',
                              'B_IPCHI2',
                              'B_IP', 
                              'B_VtxChi2',
                              'B_BPVVDCHI2',
                              

                              'GEC_MaxTracks',
                              'Prescale',
                              'Postscale'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

#        dd_name = name+'DD'
        ll_name = name+'LL'

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.pions = Pions
# no Kaons anymore        self.kaons = Kaons

#        self.hadrons = MergedSelection("HadronsFor" + name,
#                                       RequiredSelections = [ self.pions, self.kaons ] )

        # no merged selection required , because Kaon+/- are no longer considered

#        self.makeKS2DD( 'KSfor'+dd_name, config )
        self.makeKS2LL( 'KSfor'+ll_name, config )

#        self.makeBs2KSDDhh( dd_name, config )
        self.makeBs2KSLLKSLL( ll_name, config )


        self.ll_line = StrippingLine(ll_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection =  self.selBs2KSLLKSLL,
                                     FILTER = GECCode
                                     )

#        self.registerLine(self.dd_line)
        self.registerLine(self.ll_line)


    def makeKS2LL( self, name, config ) :


        # define all the cuts
        
        #1st daughters cuts 
        _piPCut1 = "(P>%s*MeV)" % config['pi_L_Pmin']
        _piMIPCut1 = "(MIPDV(PRIMARY)<%s*mm)" % config['pi_L_MIPDV']
        _pipCuts = _piPCut1 + '&' + _piMIPCut1

        _piPCut2 = "(P>%s*MeV)" % config['pi_L_Pmin']
        _piMIPCut2 = "(MIPDV(PRIMARY)<%s*mm)" % config['pi_L_MIPDV']       
        _pimCuts = _piPCut2 + '&' + _piMIPCut2 


        # before vretex fit cuts 
        _massCut2   = "(ADAMASS('KS0')<%s*MeV)" % config['KS_LL_MassWindowP']
        _docacut    = "(ACUTDOCA(%s*mm,''))" %   config['KS_LL_DOCA']
    
        
        _CombinationCuts = _massCut2 + '&' + _docacut
     
    
        # after vertex fit cuts    
        _massCut    = "(ADMASS('KS0')<%s*MeV)" % config['KS_LL_MassWindow']
        _vtxCut     = "(VFASPF(VCHI2)<%s)"     % config['KS_LL_VtxChi2']
        _combMIP    = "( sqrt( CHILD( MIPCHI2DV(PRIMARY) , 1 ) * CHILD( MIPCHI2DV(PRIMARY) , 1 ) +  CHILD( MIPCHI2DV(PRIMARY) , 2 ) * CHILD( MIPCHI2DV(PRIMARY ) , 2 ) )>%s )" % config['KS_LL_CombMPI']
        _docachi2cut = "(DOCA(1,2)/(sqrt( DOCACHI2(1,2) ))<%s*mm)" %   config['KS_LL_DOCA_ERR']  
        _nuCut       = "(log( CHILD( MIPDV(PRIMARY) , 1 )* CHILD( MIPDV(PRIMARY) , 2 )/ MIPDV( PRIMARY) ) > %s )" % config['KS_LL_nu']
        _ptCut       = "((CHILD ( PT , 1 )+CHILD ( PT , 2 ))>%s*MeV)" % config['KS_LL_pT']
        _fdCut       = "(BPVVDCHI2>%s)"         % config['KS_LL_FDChi2']
                                              
        
#        _trkChi2Cut1 = "(CHILDCUT((TRCHI2DOF<%s),1))" % config['Trk_Chi2']
#        _trkChi2Cut2 = "(CHILDCUT((TRCHI2DOF<%s),2))" % config['Trk_Chi2']
        _MotherCuts = _massCut+'&'+_vtxCut+'&'+ _combMIP  + '&' + _docachi2cut+ '&' + _nuCut + '&' + _ptCut + '&' +_fdCut 
        
        ########## EndCuts definition



        print 'KS pipCuts: ',   _pipCuts
        print 'KS pimCuts: ',  _pimCuts
        print 'KS CombinationCuts: ', _CombinationCuts
        print 'KS MotherCuts: ', _MotherCuts
        
        # make the Ks from StdAllNoPIDsPions
        _Ks = CombineParticles( DecayDescriptor = "KS0 -> pi+ pi-" ,
                                DaughtersCuts = { "pi+": _pipCuts,  "pi-": _pimCuts },
                                CombinationCut = _CombinationCuts,
                                MotherCut = _MotherCuts)
        
        
#  Ks2PiPi.DaughtersCuts =  { "pi+" : "(P>2*GeV) & (MIPDV(PRIMARY)<20.*mm)",
#                           "pi-" : "(P>2*GeV) & (MIPDV(PRIMARY)<20.*mm)" }
      


        # get the KS's to filter
#        _stdKSLL = DataOnDemand( Location = "Phys/StdLooseKsLL/Particles" )

        # make the filter
#        _filterKSLL = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selKS2LL = Selection( "selKS2LL_" + name, Algorithm = _Ks, RequiredSelections = [ self.pions  ] )


    def makeBs2KSLLKSLL( self, name, config ) :
        """
        Create and store a B -> KS(LL) KS(LL) Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """
        # before B vertex fit 
        _BamassCut  =  "(ADAMASS('B_s0')<%s*MeV)"     % config['B_MassWindowP']
        _Bdoca = "ACUTDOCA(%s*mm,'')"   % config['B_DOCA']
        _combCuts =_BamassCut + '&' + _Bdoca

        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['B_IPCHI2']
        _Bip =  "(MIPDV(PRIMARY)<%s)"                  % config['B_IP']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['B_VtxChi2']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['B_BPVVDCHI2']

        _motherCuts = _ipChi2Cut + '&' + _Bip + '&' + _vtxChi2Cut + '&' + _fdChi2Cut

        _B = CombineParticles()
        _B.DecayDescriptors = [ "B_s0 -> KS0 KS0" ]
 
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts


        print 'B combCuts: ',_combCuts
        print 'B motherCuts: ',_motherCuts

        self.selBs2KSLLKSLL = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKS2LL ])

