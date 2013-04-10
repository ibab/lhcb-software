__author__ = ['Paolo Gandini']
__date__ = '10/04/2013'
__version__ = '$Revision: 5.0 $'

#####################################################################################
# Partially reconstructed selections for Kaon and Proton tracking efficiency studies
# Hadronic modes only with high multiplicity
# Lb phase space restricted to Lc** (test of efficiency)
#####################################################################################

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles, FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseKaons, StdLooseMuons, StdLooseProtons

name = 'confB2DHHHForBXX'

__all__ = ('B2DHHHForBXXConf',
           'makeB2Da1',
           'TOSFilter',
           'confdict')

confdict =  {
     "PreScale"      : 1.0   # adimensional
    ,"GEC_nLongTrk"  : 250   # adimensional
    ,"TRCHI2"        : 3.0   # adimensional
    ,"GHOSTPROB_MAX" : 0.35  # adimensional
    ,"KaonPIDK"      : 4.0   # adimensional
    ,"PionPIDK"      : 2.0   # adimensional
    ,"dau_PT_MIN"    : 200   # MeV -> was 400
    ,"dau_MIPCHI2DV" :   5   # adimensional
    ,"D_MASS_MIN"    : 650   # MeV                 
    ,"D_MASS_MAX"    : 1400  # MeV                 
    ,"D_PT_MIN"      : 1500  # MeV                 
    ,"D_AMAXDOCA"    : 0.5   # mm                  
    ,"D_DOCACHI2_MAX":   6   # adimensional        
    ,"D_VFASPF"      :   4   # adimensional
    ,"D_BPVVDCHI2"   : 100   # adimensional
    ,"Lc_MASS_MIN"   :  700  # MeV
    ,"Lc_MASS_MAX"   : 1370  # MeV
    ,"Lc_PT_MIN"     : 1500  # MeV
    ,"Lc_AMAXDOCA"   : 0.5   # mm
    ,"Lc_DOCACHI2_MAX":  6   # adimensional
    ,"Lc_VFASPF"     :   4   # adimensional    
    ,"Lc_BPVVDCHI2"  : 100   # adimensional
    ,"A1_MASS_MIN"   : 400   # MeV
    ,"A1_MASS_MAX"   : 3400  # MeV
    ,"A1_PT_MIN"     : 1500  # MeV
    ,"A1_AMAXDOCA"   : 0.5   # mm              
    ,"A1_VFASPF"     :   4   # adimensional    
    ,"A1_BPVVDCHI2"  : 100   # adimensional    
    ,"B_MASS_MIN"    : 3600  # MeV
    ,"B_MASS_MAX"    : 4950  # MeV
    ,"B_PT_MIN"      : 1500  # MeV
    ,"B_DOCACHI2_MAX": 50    # adimensional     
    ,"B_VFASPF"      : 10    # adimensional
    ,"B_BPVVDCHI2"   : 100   # adimensional
    ,"B_DZ"          : 0.0   # mm
    ,"B_D_deltamass_MIN" : 2500 #MeV              
    ,"B_D_deltamass_MAX" : 4550 #MeV
    ,"PiPi_MASS_MAX"                     :  400  #MeV            (500)
    ,"PiPi_DOCACHI2_MAX"                 :   15  # adimensional  (15)
    ,"PiPi_SUMPT_MIN"                    :  200  #MeV            (600)
    ,"PiPi_CHI2NDF"                      :    4  # adimensional  (3)
    ,"Lb_MASS_MIN"                       : 2800  # MeV               
    ,"Lb_MASS_MAX"                       : 4800  # MeV               
    ,"Lb_PT_MIN"                         : 1500  # MeV
    ,"Lb_DOCACHI2_MAX"                   : 50    # adimensional
    ,"Lb_VFASPF"                         : 10    # adimensional
    ,"Lb_BPVVDCHI2"                      : 100   # adimensional
    ,"Lb_DZ"                             : 0.0   # mm
    ,"Lb_Lc_deltamass_MIN"               : 1900  #MeV                         
    ,"Lb_Lc_deltamass_MAX"               : 4150  #MeV             
    ,"LcStar_MASS_MIN"                   :    0  #
    ,"LcStar_MASS_MAX"                   :  700  #MeV            (700)
    }

     
class B2DHHHForBXXLinesConf(LineBuilder):


    __configuration_keys__ = (
         "PreScale"
        ,"GEC_nLongTrk"
        ,"TRCHI2"
        ,"GHOSTPROB_MAX"
        ,"KaonPIDK"     
        ,"PionPIDK"
        ,"dau_PT_MIN"
        ,"dau_MIPCHI2DV"
        ,"D_MASS_MIN"
        ,"D_MASS_MAX"
        ,"D_PT_MIN"
        ,"D_AMAXDOCA"
        ,"D_DOCACHI2_MAX"
        ,"D_VFASPF"
        ,"D_BPVVDCHI2"
        ,"Lc_MASS_MIN"
        ,"Lc_MASS_MAX"
        ,"Lc_PT_MIN"
        ,"Lc_AMAXDOCA"
        ,"Lc_DOCACHI2_MAX"
        ,"Lc_VFASPF"
        ,"Lc_BPVVDCHI2"
        ,"A1_MASS_MIN"
        ,"A1_MASS_MAX"
        ,"A1_PT_MIN"
        ,"A1_AMAXDOCA"
        ,"A1_VFASPF"
        ,"A1_BPVVDCHI2"
        ,"B_MASS_MIN"    
        ,"B_MASS_MAX"    
        ,"B_PT_MIN"
        ,"B_DOCACHI2_MAX"
        ,"B_VFASPF"      
        ,"B_BPVVDCHI2" 
        ,"B_DZ"         
        ,"B_D_deltamass_MIN"
        ,"B_D_deltamass_MAX"
        ,"PiPi_MASS_MAX"
        ,"PiPi_DOCACHI2_MAX"
        ,"PiPi_SUMPT_MIN"
        ,"PiPi_CHI2NDF"
        ,"Lb_MASS_MIN"
        ,"Lb_MASS_MAX"
        ,"Lb_PT_MIN"
        ,"Lb_DOCACHI2_MAX"
        ,"Lb_VFASPF"
        ,"Lb_BPVVDCHI2"
        ,"Lb_DZ"
        ,"Lb_Lc_deltamass_MIN"
        ,"Lb_Lc_deltamass_MAX"
        ,"LcStar_MASS_MIN"                   
        ,"LcStar_MASS_MAX"                    
        )


    __confdict__={}


    def __init__(self, name, config) :
        
        LineBuilder.__init__(self, name, config)
        self.__confdict__=config        

        
        #########
        self.dauPions = Selection( "dauPifor" + name,
                                   Algorithm = FilterDesktop(Code = "(TRGHOSTPROB < %(GHOSTPROB_MAX)s) & (PIDK < %(PionPIDK)s) & (MIPCHI2DV(PRIMARY)> %(dau_MIPCHI2DV)s) & (TRCHI2DOF < %(TRCHI2)s) & (PT > %(dau_PT_MIN)s *MeV)" %config ),
                                   RequiredSelections = [StdLoosePions])
        
        self.dauKaons = Selection( "dauKfor" + name,
                                   Algorithm = FilterDesktop(Code = "(TRGHOSTPROB < %(GHOSTPROB_MAX)s) & (PIDK > %(KaonPIDK)s) & (MIPCHI2DV(PRIMARY)> %(dau_MIPCHI2DV)s) & (TRCHI2DOF < %(TRCHI2)s) & (PT > %(dau_PT_MIN)s *MeV)" %config ),
                                   RequiredSelections = [StdLooseKaons])


        #########
        self.comb_D2PiPi = CombineParticles(DecayDescriptors = ['[D- -> pi- pi-]cc'],
                                            CombinationCut   = "(AM+10 > %(D_MASS_MIN)s *MeV) & (AM-10 < %(D_MASS_MAX)s *MeV) & (APT+20 > %(D_PT_MIN)s *MeV) & (AMAXDOCA('') < %(D_AMAXDOCA)s *mm) & (ADOCACHI2CUT(%(D_DOCACHI2_MAX)s,''))" %config,
                                            MotherCut = "(M > %(D_MASS_MIN)s *MeV) & (M < %(D_MASS_MAX)s *MeV) & (PT > %(D_PT_MIN)s *MeV) & (BPVDIRA>0.99) & (VFASPF(VCHI2/VDOF)< %(D_VFASPF)s) & (BPVVDCHI2>%(D_BPVVDCHI2)s)" %config
                                            )
        self.seld2pi = Selection("D2PiPifor"+name,
                                 Algorithm = self.comb_D2PiPi,
                                 RequiredSelections = [self.dauPions])


        #########       
        self.comb_Lc2Kpi = CombineParticles(DecayDescriptors = ['[Lambda_c+ -> K- pi+]cc'],
                                            CombinationCut   = "(AM+10 > %(Lc_MASS_MIN)s *MeV) & (AM-10 < %(Lc_MASS_MAX)s *MeV) & (APT+20 > %(Lc_PT_MIN)s *MeV) & (AMAXDOCA('') < %(Lc_AMAXDOCA)s *mm) & (ADOCACHI2CUT(%(Lc_DOCACHI2_MAX)s,''))" %config,
                                            MotherCut = "(M > %(Lc_MASS_MIN)s *MeV) & (M < %(Lc_MASS_MAX)s *MeV) & (PT > %(Lc_PT_MIN)s *MeV) & (BPVDIRA>0.99) & (VFASPF(VCHI2/VDOF)< %(Lc_VFASPF)s) & (BPVVDCHI2>%(Lc_BPVVDCHI2)s)" %config
                                            )
        self.selLc2Kpi = Selection("LcToKpifor"+name,
                                   Algorithm = self.comb_Lc2Kpi,
                                   RequiredSelections = [self.dauKaons , self.dauPions])


        #########
        self.comb_a2PiPiPi = CombineParticles(DecayDescriptors = ['[a_1(1260)+ -> pi+ pi- pi+]cc'],
                                              CombinationCut = "(AM+10 > %(A1_MASS_MIN)s *MeV) & (AM-10 < %(A1_MASS_MAX)s *MeV) & (APT+20 > %(A1_PT_MIN)s *MeV) & (AMAXDOCA('') < %(A1_AMAXDOCA)s *mm) & (ADOCACHI2CUT(%(D_DOCACHI2_MAX)s,''))" %config,
                                              MotherCut = "(M > %(A1_MASS_MIN)s *MeV) & (M < %(A1_MASS_MAX)s *MeV) & (PT > %(A1_PT_MIN)s *MeV) & (VFASPF(VCHI2/VDOF)< %(A1_VFASPF)s) & (BPVVDCHI2>%(A1_BPVVDCHI2)s)" %config
                                              )
        self.sela3pi = Selection("A2PiPiPifor" + name,                                              
                                 Algorithm = self.comb_a2PiPiPi,
                                 RequiredSelections = [self.dauPions])


        #########
        self.comb_B2Dhhh = CombineParticles(DecayDescriptor = '[B0 -> D- a_1(1260)+]cc',
                                            CombinationCut = "(AM+10 > %(B_MASS_MIN)s *MeV) & (AM-10 < %(B_MASS_MAX)s *MeV) & (AM-AM1+10 > %(B_D_deltamass_MIN)s *MeV) & (AM-AM1-10 < %(B_D_deltamass_MAX)s *MeV) & (APT+20 > %(B_PT_MIN)s *MeV) & (ADOCACHI2CUT(%(B_DOCACHI2_MAX)s,''))"%config, 
                                            MotherCut = "(M > %(B_MASS_MIN)s *MeV) & (M < %(B_MASS_MAX)s *MeV) & (PT > %(B_PT_MIN)s *MeV) & (BPVDIRA> 0.999) & (VFASPF(VCHI2/VDOF) < %(B_VFASPF)s) & (BPVVDCHI2>%(B_BPVVDCHI2)s) & (MINTREE(((ABSID=='D+')|(ABSID=='D0')|(ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > %(B_DZ)s *mm )"%config
                                            )
        self.selB2Dhhh = Selection("B2Dhhh_DPiPi_for"+ name,
                                   Algorithm = self.comb_B2Dhhh,
                                   RequiredSelections = [self.seld2pi, self.sela3pi])


        #########
        self.diPiPi = PiPiMaker("PiPi_For" + name,config,[self.dauPions],['rho(770)0 -> pi+ pi-'])


        self.comb_Lb2Lchhh = CombineParticles(DecayDescriptor = '[Lambda_b0 -> Lambda_c+ rho(770)0 pi-]cc',
                                              CombinationCut = "(AM+10 > %(Lb_MASS_MIN)s *MeV) & (AM-10 < %(Lb_MASS_MAX)s *MeV) & (AM-AM1+10 > %(Lb_Lc_deltamass_MIN)s *MeV) & (AM-AM1-10 < %(Lb_Lc_deltamass_MAX)s *MeV) & (AM12-AM1+10 > %(LcStar_MASS_MIN)s *MeV) & (AM12-AM1-10 < %(LcStar_MASS_MAX)s *MeV) & (APT+20 > %(Lb_PT_MIN)s *MeV) & (ADOCACHI2CUT(%(Lb_DOCACHI2_MAX)s,''))"%config,
                                              MotherCut = "(M > %(Lb_MASS_MIN)s *MeV) & (M < %(Lb_MASS_MAX)s *MeV) & (PT > %(Lb_PT_MIN)s *MeV) & (BPVDIRA> 0.999) & (VFASPF(VCHI2/VDOF) < %(Lb_VFASPF)s ) & (BPVVDCHI2>%(Lb_BPVVDCHI2)s) & (MINTREE(((ABSID=='D+')|(ABSID=='D0')|(ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > %(Lb_DZ)s *mm )"%config
                                              )
        self.selLb2Lchhh = Selection("Lb2Lchhh_LcKpi_for"+ name,
                                     Algorithm = self.comb_Lb2Lchhh,
                                     RequiredSelections = [self.selLc2Kpi, self.diPiPi, self.dauPions])

                                            
        #########  
        self.selB2Dhhh_tos = TOSFilter("B2Dhhh_DPiPiTOS_for" + name, self.selB2Dhhh)
        self.selLb2Lchhh_tos = TOSFilter("Lb2Lchhh_LcKpiTOS_for" + name, self.selLb2Lchhh)
        #########   

        GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" %config ,
                 "Preambulo": ["from LoKiTracks.decorators import *"]}



        #########
        self.B2DhhhLine = StrippingLine('B2DHHHwD2PiPi' + name + 'Line', prescale = config['PreScale'], FILTER=GECs, selection = self.selB2Dhhh_tos )
        self.Lb2LchhhLine = StrippingLine('Lb2LcHHHwLc2KPi' + name + 'Line', prescale = config['PreScale'], FILTER=GECs, selection = self.selLb2Lchhh_tos )

        self.registerLine(self.B2DhhhLine)
        self.registerLine(self.Lb2LchhhLine)                        

                                                                                                         



#########
 
def TOSFilter( name = None, sel = None ):
    from Configurables import TisTosParticleTagger
    _filter = TisTosParticleTagger(name+"_TriggerTos")
    #_filter.TisTosSpecs = { "Hlt1Global%TOS" : 0, "Hlt2Global%TOS" : 0 }
    _filter.TisTosSpecs = { "Hlt1.*Decision%TOS" : 0, "Hlt2.*Decision%TOS" : 0 }
    _filter.NoRegex = True
    _sel = Selection("Sel"+name+"_TriggerTos",
                     RequiredSelections = [sel],
                     Algorithm = _filter )
    return _sel
                                                                                                                       
#########


#########
def PiPiMaker(_combName,config,_RequiredSelections,_decayDescriptors):
    comb = CombineParticles(DecayDescriptors = _decayDescriptors,
                            CombinationCut = "(AM-10 < %(PiPi_MASS_MAX)s*MeV)"\
                            "& (ACUTDOCACHI2(%(PiPi_DOCACHI2_MAX)s,''))"\
                            "& (ACHILD(PT,1) + ACHILD(PT,2) > %(PiPi_SUMPT_MIN)s *MeV)" % config,
                            MotherCut = "(VFASPF(VCHI2/VDOF)< %(PiPi_CHI2NDF)s)" % config)
    sel = Selection("SelPiPi"+_combName,
                    Algorithm = comb,
                    RequiredSelections = _RequiredSelections)
    return sel

#########
        
