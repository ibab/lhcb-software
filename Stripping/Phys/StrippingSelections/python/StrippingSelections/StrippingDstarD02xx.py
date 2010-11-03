#
__author__ = ['Francesco Dettori', 'Walter Bonivento']
__date__ = '2010/04/23'
__version__ = '$Revision: 1.5 $'

'''
  Stripping for D*(2010)+ -> pi+ (D0->xx) selection:
            where xx = {mu mu, pi pi, pi mu, k mu, ... }
            
'''
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingDstarD02xxConf(LHCbConfigurableUser):
    """
    Definition of  D*(2010)+ -> pi+ (D0->xx) stripping
    """
    __slots__ = {   'PrescalepipiBox'     : 1.
                  , 'PrescalemumuBox'     : 1.
                  , 'PrescaleKpiBox'    : 1.
                  , 'PrescaleemuBox'    : 1.
                  , 'PrescaleeKBox'    : 1.
                  , 'PrescaleepiBox'    : 1.
                  , 'PrescalepimuBox'    : 1.
                  , 'PrescaleKmuBox'    : 1.
                  , 'Prescalepipi_untagged_Box'     : 0.2
                  , 'Prescalemumu_untagged_Box'     : 1.
                  , 'PrescaleKpi_untagged_Box'    : 0.2
                  , 'Prescalepimu_untagged_Box'    : 1.
                  , 'PrescaleKmu_untagged_Box'    : 0.2  
                  ,'DMassWin'           : 70.       # MeV
                  ,'DMassWinMuMu'       : 300       #MeV
                  ,'doca'               : 0.1        # mm
                  ,'XminPT'             : 750.       # MeV
                  ,'XmaxPT'             : 1100.      # MeV
                  ,'XminP'              : 4000.      # MeV
                  ,'XTrackChi2'         : 5.        # adimensional
                  ,'XTrackChi2Pi'         : 7.        # adimensional
                  ,'XminIPChi2'         : 3        # adimensional
                  ,'XmaxIPChi2'         : 8        # adimensional
                  ,'DMinFlightChi2'    :  20.
                  ,'DDira'              : 0.9997     # adimensional
                  ,'D0MinPT'            : 1800.      # MeV
                  ,'DMaxIPChi2'        :15.
                  ,'DVChi2'            :10. 
                  ,'PiMinPT'            : 110.       # MeV
                  ,'PiMaxIPCHI2'        : 10.         # adimensional
                  ,'DstMassWin'         : 110.       # MeV
                  ,'DstD0DMWin'         : 10.        # MeV
                  ,'DstD0DMWinMuMu'      : 30.        # MeV  
                  ,'RequireHlt'         : 1          # 
                  ,'prefix'         : '' 
                  }


    
    def getProps(self) :
        """
        Copied from StrippingBu2LLKConf.
        Originally from HltLinesConfigurableUser.
        May someday be absorbed into the base class definition.
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
    
    
    ####### Template for combine particles for D0 -> x+ y-  ######
    def combinetwobody(self, xplus, xminus) :
        from Configurables import CombineParticles
        d0comb_combcut =       "(AMAXDOCA('')< %(doca)s *mm) & (ADAMASS('D0')< %(DMassWin)s *MeV) & (AMAXCHILD(PT)>%(XmaxPT)s *MeV) & (APT> %(D0MinPT)s)"

        d0comb_childcut = "(PT> %(XminPT)s *MeV) & (P>%(XminP)s *MeV) & (TRCHI2DOF<%(XTrackChi2)s) & (MIPCHI2DV(PRIMARY)> %(XminIPChi2)s) " 
        d0comb_d0cut = "(BPVDIRA> %(DDira)s) & (INGENERATION( (MIPCHI2DV(PRIMARY)>%(XmaxIPChi2)s),1 ) ) & (BPVVDCHI2> %(DMinFlightChi2)s) & (MIPCHI2DV(PRIMARY)< %(DMaxIPChi2)s) & (VFASPF(VCHI2/VDOF)< %(DVChi2)s)"
        name = "D02"+xplus+xminus
        tbline = CombineParticles( self.getProp('prefix')+name )
        inputLoc = {
             "pi" : "StdNoPIDsPions"
            ,"mu" : "StdLooseMuons"
            ,"K" : "StdLooseKaons"
            ,"e" : "StdLooseElectrons"
            }
        if xplus != xminus :
            decays = ["D0 -> "+ xplus + "+ " + xminus + "- ", "D0 -> "+ xplus + "- " + xminus + "+ " ]
            tbline.DecayDescriptors =  decays
            tbline.DaughtersCuts = { xplus+"+" : d0comb_childcut % self.getProps()
                                     , xminus+"-" : d0comb_childcut % self.getProps() }
            tbline.InputLocations = [inputLoc[xplus], inputLoc[xminus] ]
        else:
            decay = "D0 -> "+ xplus + "+ " + xminus + "- "
            tbline.DecayDescriptor =  decay
            tbline.DaughtersCuts = { xplus+"+" : d0comb_childcut % self.getProps() }
            tbline.InputLocations = [ inputLoc[xplus] ]
        tbline.MotherCut = d0comb_d0cut  % self.getProps()
        tbline.CombinationCut = d0comb_combcut % self.getProps()
        return tbline
    
    ####### Template for combine particles for D* -> D0 pi
    def combineDstar(self) :
        from Configurables import CombineParticles
        dstcomb_dstcut       =  "(abs(M-MAXTREE('D0'==ABSID,M)-145.42) < %(DstD0DMWin)s ) & (VFASPF(VCHI2/VDOF)< %(DVChi2)s)"
        dstcomb_combcut =  "(ADAMASS('D*(2010)+')<%(DstMassWin)s * MeV)"
        dstcomb_picut = "(PT> %(PiMinPT)s * MeV) &  ( MIPCHI2DV(PRIMARY)< %(PiMaxIPCHI2)s) & (TRCHI2DOF<%(XTrackChi2Pi)s) "
        dstcomb_d0cut = "PT>0"
        
        dstar = CombineParticles( self.getProp('prefix')+"combine" )
        #dstar.DecayDescriptors = ['[D*(2010)+ -> D0 pi+]cc']
        dstar.DecayDescriptors = ['D*(2010)+ -> D0 pi+', 'D*(2010)- -> D0 pi-']
        dstar.DaughtersCuts = {    "pi+" : dstcomb_picut % self.getProps() ,
                                   "D0"    : dstcomb_d0cut % self.getProps()
                                   }
        dstar.CombinationCut = dstcomb_combcut % self.getProps()
        dstar.MotherCut =  dstcomb_dstcut % self.getProps()
        return dstar

    ###### Full line
    def baseLine(self, xplus, xminus) :
        from StrippingConf.StrippingLine import bindMembers
        from StrippingConf.StrippingLine import StrippingLine
        xxComb = self.combinetwobody(xplus, xminus)
        dstar = self.combineDstar()
        combname = xplus+xminus
        dstar_box = dstar.clone(self.getProp('prefix')+"Dst2PiD02"+combname+"D0PiComb" )
        dstar_box.InputLocations = [ "StdNoPIDsPions", "StdNoPIDsUpPions", self.getProp('prefix')+"D02"+combname ]
        seq_box =  bindMembers(self.getProp('prefix')+"seq_"+combname+"_box", algos = [ xxComb, dstar_box])
        pres = "Prescale"+combname+"Box"
        # Capitalize particle names to match Hlt2 D*->pi D0-> xx lines
        Xplus  = xplus[0].upper() + xplus[1:]    
        Xminus = xminus[0].upper() + xminus[1:]
        hltname = "Hlt2Dst2PiD02"+Xplus+Xminus+"*Decision"  # * matches Signal, Sidebands and Box lines
        line_box = StrippingLine(self.getProp('prefix')+"Dst2PiD02"+combname+"Box",
                                 HLT = "HLT_PASS_RE('"+hltname+"')",
                                 algos = [ seq_box ], prescale = self.getProps()[ pres ])

        return line_box
    
    def baseLine_untagged(self, xplus, xminus) :
        from StrippingConf.StrippingLine import bindMembers
        from StrippingConf.StrippingLine import StrippingLine
        xxComb = self.combinetwobody(xplus, xminus)
#        dstar = self.combineDstar()
        combname = xplus+xminus
#        dstar_box = dstar.clone(self.getProp('prefix')+"Dst2PiD02"+combname+"D0PiComb" )
#        dstar_box.InputLocations = [ "StdNoPIDsPions", "StdNoPIDsUpPions", self.getProp('prefix')+"D02"+combname ]
        seq_box =  bindMembers(self.getProp('prefix')+"seq_"+combname+"_untagged__box", algos = [ xxComb])
        pres = "Prescale"+combname+"_untagged_Box"
        # Capitalize particle names to match Hlt2 D*->pi D0-> xx lines
        Xplus  = xplus[0].upper() + xplus[1:]    
        Xminus = xminus[0].upper() + xminus[1:]
        hltname = "Hlt2Dst2PiD02"+Xplus+Xminus+"*Decision"  # * matches Signal, Sidebands and Box lines
        line_untagged_box = StrippingLine(self.getProp('prefix')+"Dst2PiD02"+combname+"_untagged_Box",
                                 HLT = "HLT_PASS_RE('"+hltname+"')",
                                 algos = [ seq_box ], prescale = self.getProps()[ pres ])

        return line_untagged_box    
    
    def lines ( self ) :
        from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from CommonParticles.StdNoPIDsMuons import StdNoPIDsMuons
        from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons
        from CommonParticles.StdNoPIDsElectrons import StdNoPIDsElectrons
        
       
        line_pipi_box = self.baseLine("pi", "pi")
        line_mumu_box = self.baseLine("mu", "mu")
        line_Kpi_box  = self.baseLine("K",  "pi")
        line_eK_box   = self.baseLine("e",   "K")
        line_epi_box   = self.baseLine("e",   "pi")
        line_mue_box  = self.baseLine("e",  "mu")
        line_pimu_box = self.baseLine("pi", "mu")
        line_Kmu_box  = self.baseLine("K",  "mu")
        line_mumu_untagged_box  = self.baseLine_untagged("mu",  "mu")
        line_Kmu_untagged_box  = self.baseLine_untagged("K",  "mu")
        line_pipi_untagged_box  = self.baseLine_untagged("pi",  "pi")
        line_Kpi_untagged_box  = self.baseLine_untagged("K",  "pi")
        line_pimu_untagged_box = self.baseLine_untagged("pi", "mu")
        
        
        lines = [line_pipi_box, line_mumu_box,line_pimu_box, line_Kpi_box, line_mue_box, line_eK_box,line_epi_box, line_Kmu_box,line_mumu_untagged_box ,line_Kmu_untagged_box, line_pipi_untagged_box,line_Kpi_untagged_box,line_pimu_untagged_box]
        
        return lines
    

#    def __apply_configuration__(self) :
#        self.lines()




