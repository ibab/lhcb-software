#
__author__ = ['Francesco Dettori', 'Walter Bonivento']
__date__ = '2010/04/23'
__version__ = '$Revision: 1.2 $'

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
    __slots__ = { 'PrescalepipiBox'     : 1.
                  , 'PrescalepipiD0SB'  : 1.
                  , 'PrescalepipiDMSB'  : 1.
                  , 'PrescalepipiSignal'  : 1.
                  , 'PrescalemumuBox'     : 1.
                  , 'PrescalemumuD0SB'  : 1.
                  , 'PrescalemumuDMSB'  : 1.
                  , 'PrescalemumuSignal'  : 1.
                  , 'PrescaleKpiBox'    : 1.
                  , 'PrescalemueBox'    : 1.
                  , 'PrescaleeeBox'    : 1.
                  , 'PrescalepimuBox'    : 1.
                  , 'PrescaleKmuBox'    : 1.
                  ,'DMassWin'           : 150.       # MeV
                  ,'doca'               : 7.0        # mm
                  ,'XminPT'             : 750.       # MeV
                  ,'XmaxPT'             : 750.       # MeV
                  ,'XminP'              : 3000.      # MeV
                  ,'XminIPCHI2'         : 1          # adimensional
                  ,'DDira'              : 0.999     # adimensional
                  ,'PiMinPT'            : 110.       # MeV
                  ,'PiMaxIPCHI2'        : 98.         # adimensional
                  ,'D0MinPT'            : 1600.      # MeV
                  ,'D0MinD'             : 0.         # mm
                  ,'D0MaxIPCHI2'        : 24.       # mm
                  ,'DstMassWin'         : 160.       # MeV
                  ,'DstD0DMWin'         : 10.        # MeV
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
        d0comb_combcut =       "(AMAXDOCA('')< %(doca)s *mm) & (ADAMASS('D0')< %(DMassWin)s *MeV) & (AMAXCHILD(PT)>%(XmaxPT)s *MeV)"
        d0comb_combcut_tight = "(AMAXDOCA('')< %(doca)s *mm) & (ADAMASS('D0')< %(DMassWin)s *MeV) & (AMAXCHILD(PT)>%(XmaxPT)s *MeV)" 

        d0comb_childcut = "(PT> %(XminPT)s *MeV) & (P>%(XminP)s *MeV) & (BPVIPCHI2()> %(XminIPCHI2)s)" 
        d0comb_d0cut = "(BPVDIRA> %(DDira)s)"
        name = "D02"+xplus+xminus
        tbline = CombineParticles( name )
        inputLoc = {
             "pi" : "StdNoPIDsPions"
            ,"mu" : "StdNoPIDsMuons"
            ,"K" : "StdNoPIDsKaons"
            ,"e" : "StdNoPIDsElectrons"
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
        dstcomb_dstcut       =  "(abs(M-MAXTREE('D0'==ABSID,M)-145.42) < %(DstD0DMWin)s )"
        dstcomb_combcut =  "(ADAMASS('D*(2010)+')<%(DstMassWin)s * MeV)"
        dstcomb_picut = "(PT> %(PiMinPT)s * MeV) &  (BPVIPCHI2() < %(PiMaxIPCHI2)s)"
        dstcomb_d0cut = "(PT> %(D0MinPT)s * MeV) & (BPVVD > %(D0MinD)s ) & (BPVIPCHI2() < %(D0MaxIPCHI2)s )"
        
        dstar = CombineParticles( "combine" )
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
        dstar_box = dstar.clone("Dst2PiD02"+combname+"D0PiComb" )
        dstar_box.InputLocations = [ "StdNoPIDsPions", "StdNoPIDsUpPions", "D02"+combname ]
        seq_box =  bindMembers("seq_"+combname+"_box", algos = [ xxComb, dstar_box])
        pres = "Prescale"+combname+"Box"
        line_box = StrippingLine("Dst2PiD02"+combname+"Box", algos = [ seq_box ], prescale = self.getProps()[ pres ])
        return line_box
    
    
    
    def lines ( self ) :
        from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from CommonParticles.StdNoPIDsMuons import StdNoPIDsMuons
        from CommonParticles.StdNoPIDsKaons import StdNoPIDsKaons
        from CommonParticles.StdNoPIDsElectrons import StdNoPIDsElectrons
        
       
        line_pipi_box = self.baseLine("pi", "pi")
        line_mumu_box = self.baseLine("mu", "mu")
        line_Kpi_box  = self.baseLine("K",  "pi")
        line_ee_box   = self.baseLine("e",   "e")
        line_mue_box  = self.baseLine("mu",  "e")
        line_pimu_box = self.baseLine("pi", "mu")
        line_Kmu_box  = self.baseLine("K",  "mu")
        
        
        
        lines = [line_pipi_box, line_mumu_box,line_pimu_box, line_Kpi_box, line_mue_box, line_ee_box, line_Kmu_box]
        
        return lines
    

#    def __apply_configuration__(self) :
#        self.lines()




