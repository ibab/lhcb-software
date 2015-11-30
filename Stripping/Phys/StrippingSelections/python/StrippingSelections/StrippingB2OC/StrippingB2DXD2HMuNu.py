__author__ = ['Wenbin Qian']
__date__ = '09/11/2015'
__version__ = '$Revision: 0.01 $'

from Gaudi.Configuration import *
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, DaVinci__N4BodyDecays, DaVinci__N3BodyDecays
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StandardParticles import StdLooseMuons, StdLoosePions, StdLooseKaons
from GaudiKernel.SystemOfUnits import MeV, GeV, cm, mm

__all__ = ('B2DXD2HMuNuBuilder',
           'default_config')

default_config = {
    'B2DXD2HMuNu' :{
       'WGs'         : ['B2OC'],
       'BUILDERTYPE' : 'B2DXD2HMuNuBuilder',
       'CONFIG'      :{
          "PT_MIN"          :  200,  #MeV
          "P_MIN"           :  3000, #MeV
          "MIPCHI2DV_MIN"   :  4.,
          "TRGHP_MAX"       :  0.4,
          "MuonPIDK"            : 0.    ,#adimensional
          "MuonPIDmu"           : 3.    ,#adimensional
          "MuonPIDp"            : 0.    ,#adimensional
          "KaonPIDK"            : 5.     ,#adimensional
          "KaonPIDmu"           : 5.     ,#adimensional
          "KaonPIDp"            : 5.     ,#adimensional
          "PionPIDK"            : 0.     ,#adimensional
          "PionPIDmu"           : 0.     ,#adimensional
          "PionPIDp"            : 0.     ,#adimensional
          "DMass_MAX"           : 1800, #MeV
          "Cos_MAX"             : 0.99999,
          "SUMPT_MIN"           : 1500, #MeV
          "ADOCA_MAX"           : 0.2, #mm
          "Plane_MAX"           : 0.02, #mm
          "VCHI2DOF_MAX"        : 10,
          "BPVVDCHI2_MIN"       : 36,
          "HH_MAX"              : 3800, #MeV
          "BMass_MAX"           : 5200, #MeV
          "GEC_nLongTrk"        : 160.   #adimensional
       },
       'STREAMS'     : {
          'B2OC': [
             'StrippingB2D0PiD02PiMuNuLine',
             'StrippingB2D0PiD02KMuNuLine',
             'StrippingB2D0PiD02KPiPiMuNuLine',
             'StrippingB2D0KD02PiMuNuLine',
             'StrippingB2D0KD02KMuNuLine',
             'StrippingB2D0KD02KPiPiMuNuLine',
             'StrippingB2D0KPiPiD02PiMuNuLine',
             'StrippingB2D0KPiPiD02KMuNuLine',
             'StrippingB2D0KPiPiD02KPiPiMuNuLine',
             'StrippingB02D0KPiD02PiMuNuLine',
             'StrippingB02D0KPiD02KMuNuLine',
             'StrippingB02D0KPiD02KPiPiMuNuLine',
             'StrippingB2D0PiD02PiMuNuSUPLine',
             'StrippingB2D0PiD02KMuNuSUPLine',
             'StrippingB2D0PiD02KPiPiMuNuSUPLine',
             'StrippingB2D0KD02PiMuNuSUPLine',
             'StrippingB2D0KD02KMuNuSUPLine',
             'StrippingB2D0KD02KPiPiMuNuSUPLine',
             'StrippingB2D0KPiPiD02PiMuNuSUPLine',
             'StrippingB2D0KPiPiD02KMuNuSUPLine',
             'StrippingB2D0KPiPiD02KPiPiMuNuSUPLine',
             'StrippingB02D0KPiD02PiMuNuSUPLine',
             'StrippingB02D0KPiD02KMuNuSUPLine',
             'StrippingB02D0KPiD02KPiPiMuNuSUPLine',
             'StrippingB2D0PiD02PiMuNuWSLine',
             'StrippingB2D0PiD02KMuNuWSLine',
             'StrippingB2D0PiD02KPiPiMuNuWSLine',
             'StrippingB2D0KD02PiMuNuWSLine',
             'StrippingB2D0KD02KMuNuWSLine',
             'StrippingB2D0KD02KPiPiMuNuWSLine',
             'StrippingB2D0KPiPiD02PiMuNuWSLine',
             'StrippingB2D0KPiPiD02KMuNuWSLine',
             'StrippingB2D0KPiPiD02KPiPiMuNuWSLine',
             'StrippingB02D0KPiD02PiMuNuWSLine',
             'StrippingB02D0KPiD02KMuNuWSLine',
             'StrippingB02D0KPiD02KPiPiMuNuWSLine',
             'StrippingB2D0PiD02PiMuNuWSSUPLine',
             'StrippingB2D0PiD02KMuNuWSSUPLine',
             'StrippingB2D0PiD02KPiPiMuNuWSSUPLine',
             'StrippingB2D0KD02PiMuNuWSSUPLine',
             'StrippingB2D0KD02KMuNuWSSUPLine',
             'StrippingB2D0KD02KPiPiMuNuWSSUPLine',
             'StrippingB2D0KPiPiD02PiMuNuWSSUPLine',
             'StrippingB2D0KPiPiD02KMuNuWSSUPLine',
             'StrippingB2D0KPiPiD02KPiPiMuNuWSSUPLine',
             'StrippingB02D0KPiD02PiMuNuWSSUPLine',
             'StrippingB02D0KPiD02KMuNuWSSUPLine',
             'StrippingB02D0KPiD02KPiPiMuNuWSSUPLine'
             ]
       }
    }
}

            
class B2DXD2HMuNuBuilder(LineBuilder):
            
    __configuration_keys__ = (
         "PT_MIN",
         "P_MIN",
         "MIPCHI2DV_MIN",
         "TRGHP_MAX",
         "MuonPIDK",
         "MuonPIDmu",
         "MuonPIDp",
         "KaonPIDK",
         "KaonPIDmu",
         "KaonPIDp",
         "PionPIDK",
         "PionPIDmu",
         "PionPIDp",
         "DMass_MAX",
         "Cos_MAX",
         "SUMPT_MIN",
         "ADOCA_MAX",
         "Plane_MAX",
         "VCHI2DOF_MAX",
         "BPVVDCHI2_MIN",
         "HH_MAX",
         "BMass_MAX",
         "GEC_nLongTrk"
         )
    
    def __init__(self,name,config): 
        LineBuilder.__init__(self, name, config)
        self._config=config
               
        self.GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" % config,
                      "Preambulo": ["from LoKiTracks.decorators import *"]}
        
        self._muon = self._muonFilter() 
        self._pion = self._pionFilter()
        self._kaon = self._kaonFilter()

        self._d2pimunu = self._D2HMuNuBuilder("D02PiMuNu",["[D0 -> pi- mu+]cc"],[self._muon,self._pion])
        self._d2kmunu = self._D2HMuNuBuilder("D02KMuNu",["[D0 -> K- mu+]cc"],[self._muon,self._kaon])
        self._d2kpipimunu = self._D2KPiPiMuNuBuilder("D02KPiPiMuNu",["[D0 -> K- pi+ pi- mu+]cc"],[self._kaon,self._pion,self._muon])
        self._d2pimunuws = self._D2HMuNuBuilder("D02PiMuNuWS",["[D0 -> pi- mu-]cc"],[self._muon,self._pion])
        self._d2kmunuws = self._D2HMuNuBuilder("D02KMuNuWS",["[D0 -> K- mu-]cc"],[self._muon,self._kaon])
        self._d2kpipimunuws = self._D2KPiPiMuNuBuilder("D02KPiPiMuNuWS",["[D0 -> K- pi+ pi- mu-]cc"],[self._kaon,self._pion,self._muon])
        self._hh = self._HHBuilder()
        self._hhh = self._HHHBuilder()
               
        self.registerLine(self._makeBLine("B2D0PiD02PiMuNu",["[B- -> D0 pi-]cc"],[self._d2pimunu,self._pion],1.0))
        self.registerLine(self._makeBLine("B2D0PiD02KMuNu",["[B- -> D0 pi-]cc"],[self._d2kmunu,self._pion],1.0))
        self.registerLine(self._makeBLine("B2D0PiD02KPiPiMuNu",["[B- -> D0 pi-]cc"],[self._d2kpipimunu,self._pion],1.0))
        self.registerLine(self._makeBLine("B2D0KD02PiMuNu",["[B- -> D0 K-]cc"],[self._d2pimunu,self._kaon],1.0))
        self.registerLine(self._makeBLine("B2D0KD02KMuNu",["[B- -> D0 K-]cc"],[self._d2kmunu,self._kaon],1.0))
        self.registerLine(self._makeBLine("B2D0KD02KPiPiMuNu",["[B- -> D0 K-]cc"],[self._d2kpipimunu,self._kaon],1.0))
        self.registerLine(self._makeBLine("B2D0KPiPiD02PiMuNu",["[B- -> D0 K_1(1270)-]cc"],[self._d2pimunu,self._hhh],1.0))
        self.registerLine(self._makeBLine("B2D0KPiPiD02KMuNu",["[B- -> D0 K_1(1270)-]cc"],[self._d2kmunu,self._hhh],1.0))
        self.registerLine(self._makeBLine("B2D0KPiPiD02KPiPiMuNu",["[B- -> D0 K_1(1270)-]cc"],[self._d2kpipimunu,self._hhh],1.0))
        self.registerLine(self._makeBLine("B02D0KPiD02PiMuNu",["[B0 -> D~0 K*(892)0]cc"],[self._d2pimunu,self._hh],1.0))
        self.registerLine(self._makeBLine("B02D0KPiD02KMuNu",["[B0 -> D~0 K*(892)0]cc"],[self._d2kmunu,self._hh],1.0))
        self.registerLine(self._makeBLine("B02D0KPiD02KPiPiMuNu",["[B0 -> D~0 K*(892)0]cc"],[self._d2kpipimunu,self._hh],1.0))

        self.registerLine(self._makeBLine("B2D0PiD02PiMuNuSUP",["[B+ -> D0 pi+]cc"],[self._d2pimunu,self._pion],1.0))
        self.registerLine(self._makeBLine("B2D0PiD02KMuNuSUP",["[B+ -> D0 pi+]cc"],[self._d2kmunu,self._pion],1.0))
        self.registerLine(self._makeBLine("B2D0PiD02KPiPiMuNuSUP",["[B+ -> D0 pi+]cc"],[self._d2kpipimunu,self._pion],1.0))
        self.registerLine(self._makeBLine("B2D0KD02PiMuNuSUP",["[B+ -> D0 K+]cc"],[self._d2pimunu,self._kaon],1.0))
        self.registerLine(self._makeBLine("B2D0KD02KMuNuSUP",["[B+ -> D0 K+]cc"],[self._d2kmunu,self._kaon],1.0))
        self.registerLine(self._makeBLine("B2D0KD02KPiPiMuNuSUP",["[B+ -> D0 K+]cc"],[self._d2kpipimunu,self._kaon],1.0))
        self.registerLine(self._makeBLine("B2D0KPiPiD02PiMuNuSUP",["[B+ -> D0 K_1(1270)+]cc"],[self._d2pimunu,self._hhh],1.0))
        self.registerLine(self._makeBLine("B2D0KPiPiD02KMuNuSUP",["[B+ -> D0 K_1(1270)+]cc"],[self._d2kmunu,self._hhh],1.0))
        self.registerLine(self._makeBLine("B2D0KPiPiD02KPiPiMuNuSUP",["[B+ -> D0 K_1(1270)+]cc"],[self._d2kpipimunu,self._hhh],1.0))
        self.registerLine(self._makeBLine("B02D0KPiD02PiMuNuSUP",["[B0 -> D0 K*(892)0]cc"],[self._d2pimunu,self._hh],1.0))
        self.registerLine(self._makeBLine("B02D0KPiD02KMuNuSUP",["[B0 -> D0 K*(892)0]cc"],[self._d2kmunu,self._hh],1.0))
        self.registerLine(self._makeBLine("B02D0KPiD02KPiPiMuNuSUP",["[B0 -> D0 K*(892)0]cc"],[self._d2kpipimunu,self._hh],1.0))

        self.registerLine(self._makeBLine("B2D0PiD02PiMuNuWS",["[B- -> D0 pi-]cc"],[self._d2pimunuws,self._pion],0.1))
        self.registerLine(self._makeBLine("B2D0PiD02KMuNuWS",["[B- -> D0 pi-]cc"],[self._d2kmunuws,self._pion],0.1))
        self.registerLine(self._makeBLine("B2D0PiD02KPiPiMuNuWS",["[B- -> D0 pi-]cc"],[self._d2kpipimunuws,self._pion],0.1))
        self.registerLine(self._makeBLine("B2D0KD02PiMuNuWS",["[B- -> D0 K-]cc"],[self._d2pimunuws,self._kaon],0.1))
        self.registerLine(self._makeBLine("B2D0KD02KMuNuWS",["[B- -> D0 K-]cc"],[self._d2kmunuws,self._kaon],0.1))
        self.registerLine(self._makeBLine("B2D0KD02KPiPiMuNuWS",["[B- -> D0 K-]cc"],[self._d2kpipimunuws,self._kaon],0.1))
        self.registerLine(self._makeBLine("B2D0KPiPiD02PiMuNuWS",["[B- -> D0 K_1(1270)-]cc"],[self._d2pimunuws,self._hhh],0.1))
        self.registerLine(self._makeBLine("B2D0KPiPiD02KMuNuWS",["[B- -> D0 K_1(1270)-]cc"],[self._d2kmunuws,self._hhh],0.1))
        self.registerLine(self._makeBLine("B2D0KPiPiD02KPiPiMuNuWS",["[B- -> D0 K_1(1270)-]cc"],[self._d2kpipimunuws,self._hhh],0.1))
        self.registerLine(self._makeBLine("B02D0KPiD02PiMuNuWS",["[B0 -> D~0 K*(892)0]cc"],[self._d2pimunuws,self._hh],0.1))
        self.registerLine(self._makeBLine("B02D0KPiD02KMuNuWS",["[B0 -> D~0 K*(892)0]cc"],[self._d2kmunuws,self._hh],0.1))
        self.registerLine(self._makeBLine("B02D0KPiD02KPiPiMuNuWS",["[B0 -> D~0 K*(892)0]cc"],[self._d2kpipimunuws,self._hh],0.1))

        self.registerLine(self._makeBLine("B2D0PiD02PiMuNuWSSUP",["[B+ -> D0 pi+]cc"],[self._d2pimunuws,self._pion],0.1))
        self.registerLine(self._makeBLine("B2D0PiD02KMuNuWSSUP",["[B+ -> D0 pi+]cc"],[self._d2kmunuws,self._pion],0.1))
        self.registerLine(self._makeBLine("B2D0PiD02KPiPiMuNuWSSUP",["[B+ -> D0 pi+]cc"],[self._d2kpipimunuws,self._pion],0.1))
        self.registerLine(self._makeBLine("B2D0KD02PiMuNuWSSUP",["[B+ -> D0 K+]cc"],[self._d2pimunuws,self._kaon],0.1))
        self.registerLine(self._makeBLine("B2D0KD02KMuNuWSSUP",["[B+ -> D0 K+]cc"],[self._d2kmunuws,self._kaon],0.1))
        self.registerLine(self._makeBLine("B2D0KD02KPiPiMuNuWSSUP",["[B+ -> D0 K+]cc"],[self._d2kpipimunuws,self._kaon],0.1))
        self.registerLine(self._makeBLine("B2D0KPiPiD02PiMuNuWSSUP",["[B+ -> D0 K_1(1270)+]cc"],[self._d2pimunuws,self._hhh],0.1))
        self.registerLine(self._makeBLine("B2D0KPiPiD02KMuNuWSSUP",["[B+ -> D0 K_1(1270)+]cc"],[self._d2kmunuws,self._hhh],0.1))
        self.registerLine(self._makeBLine("B2D0KPiPiD02KPiPiMuNuWSSUP",["[B+ -> D0 K_1(1270)+]cc"],[self._d2kpipimunuws,self._hhh],0.1))
        self.registerLine(self._makeBLine("B02D0KPiD02PiMuNuWSSUP",["[B0 -> D0 K*(892)0]cc"],[self._d2pimunuws,self._hh],0.1))
        self.registerLine(self._makeBLine("B02D0KPiD02KMuNuWSSUP",["[B0 -> D0 K*(892)0]cc"],[self._d2kmunuws,self._hh],0.1))
        self.registerLine(self._makeBLine("B02D0KPiD02KPiPiMuNuWSSUP",["[B0 -> D0 K*(892)0]cc"],[self._d2kpipimunuws,self._hh],0.1))


    def _NominalMuSelection( self ):
        return " (PT> %(PT_MIN)s* MeV)"\
               "& (P> %(P_MIN)s* MeV)"\
               "& (TRGHOSTPROB < %(TRGHP_MAX)s)"\
               "& (PIDmu-PIDpi> %(MuonPIDmu)s )"\
               "& (PIDmu-PIDp> %(MuonPIDp)s )"\
               "& (PIDmu-PIDK> %(MuonPIDK)s )"\
               "& (MIPCHI2DV(PRIMARY)> %(MIPCHI2DV_MIN)s )"
        
    def _NominalKSelection( self ):
        return " (PT> %(PT_MIN)s *MeV)"\
               "& (P> %(P_MIN)s* MeV)"\
               "& (TRGHOSTPROB < %(TRGHP_MAX)s)"\
               "& (PIDK-PIDpi> %(KaonPIDK)s )  "\
               "& (PIDK-PIDp> %(KaonPIDp)s )   "\
               "& (PIDK-PIDmu> %(KaonPIDmu)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(MIPCHI2DV_MIN)s )"
        
    def _NominalPiSelection( self ):
        return " (PT> %(PT_MIN)s *MeV)"\
               "& (P> %(P_MIN)s* MeV)"\
               "& (TRGHOSTPROB < %(TRGHP_MAX)s)"\
               "& (PIDK-PIDpi< %(PionPIDK)s ) "\
               "& (PIDp < %(PionPIDp)s )      "\
               "& (PIDmu < %(PionPIDmu)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(MIPCHI2DV_MIN)s )"

    def _muonFilter( self ):
        _mu = FilterDesktop( Code = self._NominalMuSelection() % self._config )
        return Selection("Mu_for"+self._name,Algorithm=_mu,RequiredSelections = [StdLooseMuons])        

    def _pionFilter( self ):
        _pi = FilterDesktop( Code = self._NominalPiSelection() % self._config )
        return Selection("Pi_for"+self._name,Algorithm=_pi,RequiredSelections = [StdLoosePions])
       
    def _kaonFilter( self ):
        _ka = FilterDesktop( Code = self._NominalKSelection() % self._config )
        return Selection("K_for"+self._name,Algorithm=_ka,RequiredSelections = [StdLooseKaons])

    def _D2HMuNuBuilder( self,_name,_decay,inputs ):
        _d = CombineParticles(
            DecayDescriptors = _decay,
            CombinationCut = "(ASUM(PT)>%(SUMPT_MIN)s*MeV) "\
            "& (ADOCA(1,2)<%(ADOCA_MAX)s*mm) "\
            "& (AM<%(DMass_MAX)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(VCHI2DOF_MAX)s )"\
            "& (cos<%(Cos_MAX)s)" \
#            "& (MIPCHI2DV(PRIMARY)> %(MIPCHI2DV_MIN)s )"\
            "& (BPVVDCHI2 >%(BPVVDCHI2_MIN)s)"% self._config,
            )
        _d.Preambulo = ["from LoKiPhys.decorators import *",
                         "px1 = CHILD(PX,1)",
                         "py1 = CHILD(PY,1)",
                         "pz1 = CHILD(PZ,1)",
                         "px2 = CHILD(PX,2)",
                         "py2 = CHILD(PY,2)",
                         "pz2 = CHILD(PZ,2)",
                         "cos = (px1*px2+py1*py2+pz1*pz2)/(sqrt(px1*px1+py1*py1+pz1*pz1)*sqrt(px2*px2+py2*py2+pz2*pz2))",
                        ]
        return Selection(_name+"_for"+self._name,Algorithm=_d,RequiredSelections=inputs)
    
    def _D2KPiPiMuNuBuilder( self,_name,_decay,inputs ):
        _d = DaVinci__N4BodyDecays(
            DecayDescriptors =  _decay,
            Combination12Cut =  "  (ADOCA(1,2)<%(ADOCA_MAX)s*mm) "%self._config,
            Combination123Cut = "  (ADOCA(1,3)<%(ADOCA_MAX)s*mm) " \
            "& (ADOCA(2,3)<%(ADOCA_MAX)s*mm) "%self._config,
            CombinationCut = "  (ADOCA(1,4)<%(ADOCA_MAX)s*mm) " \
            "& (ADOCA(2,4)<%(ADOCA_MAX)s*mm) " \
            "& (ADOCA(3,4)<%(ADOCA_MAX)s*mm) " \
            "& (ASUM(PT)>%(SUMPT_MIN)s*MeV) "\
            "& (AM<%(DMass_MAX)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(VCHI2DOF_MAX)s )"\
 #           "& (MIPCHI2DV(PRIMARY)> %(MIPCHI2DV_MIN)s )"\
            "& (BPVVDCHI2 >%(BPVVDCHI2_MIN)s)"% self._config,
            )
        return Selection(_name+"_for"+self._name,Algorithm=_d,RequiredSelections=inputs)

    def _HHBuilder(self):
        _hh = CombineParticles(
            DecayDescriptors =  ["[K*(892)0 -> K+ pi-]cc"],
            CombinationCut = "(ASUM(PT)>%(SUMPT_MIN)s*MeV) "\
            "& (ADOCA(1,2)<%(ADOCA_MAX)s*mm) "\
            "& (AM<%(HH_MAX)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(VCHI2DOF_MAX)s )"\
  #          "& (MIPCHI2DV(PRIMARY)> %(MIPCHI2DV_MIN)s )"\
            "& (BPVVDCHI2 >%(BPVVDCHI2_MIN)s)"% self._config,
            )
        return Selection("HH_for"+self._name,Algorithm=_hh,RequiredSelections=[self._kaon,self._pion])

    def _HHHBuilder(self):
        _hhh = DaVinci__N3BodyDecays(
            DecayDescriptors =  ["[K_1(1270)+ -> K+ pi- pi+]cc"],
            Combination12Cut =  "  (ADOCA(1,2)<%(ADOCA_MAX)s*mm) "%self._config,
            CombinationCut = "  (ADOCA(1,3)<%(ADOCA_MAX)s*mm) " \
            "& (ADOCA(2,3)<%(ADOCA_MAX)s*mm) "\
            "& (ASUM(PT)>%(SUMPT_MIN)s*MeV) "\
            "& (AM<%(HH_MAX)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(VCHI2DOF_MAX)s )"\
   #         "& (MIPCHI2DV(PRIMARY)> %(MIPCHI2DV_MIN)s )"\
            "& (BPVVDCHI2 >%(BPVVDCHI2_MIN)s)"% self._config,
            )
        return Selection("HHH_for"+self._name,Algorithm=_hhh,RequiredSelections=[self._kaon,self._pion])
    
    def _makeBLine(self,_name,_decay,inputs, pre):
        _b = CombineParticles(
            DecayDescriptors = _decay,
            CombinationCut = "(AM<%(BMass_MAX)s *MeV)" %self._config,
            MotherCut = "(MDNDOCA(5.279,0) < %(Plane_MAX)s*mm) & (lim_up<0) & (lim_lo>0)"%self._config, 
            )
        _b.Preambulo = ["from LoKiPhys.decorators import *",
                        "m_B = 5279.29 ",
                        "m_D = 1864.84 ",
                        "m_3 = CHILD(M,2)",
                        "e3 = (m_B*m_B - m_D*m_D - m_3*m_3)/(2.*m_D)",
                        "p3 = sqrt(e3*e3 - m_3*m_3)",
                        "m_all = M",
                        "m_n   = CHILD(M,1)",
                        "lim_up = (m_all*m_all - m_B*m_B) - (1.+e3/m_D -p3/m_D)*(m_n*m_n - m_D*m_D) ",
                        "lim_lo = (m_all*m_all - m_B*m_B) - (1.+e3/m_D + p3/m_D)*(m_n*m_n - m_D*m_D) ",	                
	               ]
        _bsel = Selection("Sel"+_name,Algorithm = _b,RequiredSelections = inputs)

        hlt2 = "(HLT_PASS_RE('Hlt2Topo.*Decision') |"\
               " HLT_PASS_RE('Hlt2.*SingleMuon.*Decision'))"
        Line = StrippingLine(_name+'Line',
               prescale = pre,
               checkPV = True,
               FILTER=self.GECs,
               HLT2 = hlt2,
               selection = _bsel) 
        
        return Line
                                                    

    
