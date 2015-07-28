
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection, MergedSelection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *
#from Configurables import SubstitutePID

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class Bc2DDBuilder(object):
    '''Makes all Bc->DD decays for the Beauty2Charm module.'''

    def __init__(self,d,dst,config):
        
        self.config = config
        self.d      = d
        self.dst    = dst
        self.lines  = []

        self.d0modes = { }
        self.dpmodes = { }
        self.dstarpmodes = { }
        self.dstar0modes = { }

        # Build the merged D selections
        self._createD0Modes()
        self._createDpModes()
        self._createDstarPlusModes()
        self._createDstar0Modes()

        # Define the Bc lines
        self._makeBc2DD0()
        self._makeBc2DstarD0()
        self._makeBc2DstarDstar0()

    def _buildSel(self,dmodes,name,map):
        map[name] = MergedSelection("BC2DD-"+name+"-Beauty2Charm",RequiredSelections=dmodes)

    def _createD0Modes(self):

        # ( D->hh )
        name = "D02HH"
        #dmodes = self.d.hh
        dmodes = self.d.hh_pid
        # With Upstream tracks as well
        #dmodes += self.d.hh_up
        self._buildSel(dmodes,name,self.d0modes)

        # ( D->hhhh )
        name = "D02HHHH"
        #dmodes = self.d.hhhh
        dmodes = self.d.hhhh_pid
        # With Upstream tracks as well
        #dmodes += self.d.hhhh_up
        self._buildSel(dmodes,name,self.d0modes)

        # ( D->Kshh )
        name = "D02KSHH"
        #dmodes = self.d.kshh_ll  + self.d.kshh_dd
        dmodes = self.d.kshh_ll_pid  + self.d.kshh_dd_pid
        # With Upstream tracks as well
        #dmodes += self.d.kshh_ll_up + self.d.kshh_dd_up
        self._buildSel(dmodes,name,self.d0modes)

        # ( D->hhpi0 )
        name = "D02HHPI0"
        #dmodes = self.d.pi0hh_merged  + self.d.pi0hh_resolved
        dmodes = self.d.pi0hh_merged_pid  + self.d.pi0hh_resolved_pid
        # With Upstream tracks as well
        #dmodes += self.d.pi0hh_merged_up + self.d.pi0hh_resolved_up
        self._buildSel(dmodes,name,self.d0modes)

        ## # ( D->hhhhpi0 )
        ## name = "D02HHHHPI0"
        ## dmodes = self.d.pi0hhhh_merged + self.d.pi0hhhh_resolved
        ## # With Upstream tracks as well
        ## #dmodes += self.d.pi0hhhh_merged_up + self.d.pi0hhhh_resolved_up
        ## self._buildSel(dmodes,name,self.d0modes)
         
        # ( D0->Kshhpi0 )
        name = "D02KSHHPI0"
        #dmodes  = self.d.kspi0hh_ll_merged + self.d.kspi0hh_ll_resolved
        #dmodes += self.d.kspi0hh_dd_merged + self.d.kspi0hh_dd_resolved
        dmodes  = self.d.kspi0hh_ll_merged_pid + self.d.kspi0hh_ll_resolved_pid
        dmodes += self.d.kspi0hh_dd_merged_pid + self.d.kspi0hh_dd_resolved_pid
        # With Upstream
        #dmodes += self.d.kspi0hh_ll_merged_up + self.d.kspi0hh_ll_resolved_up
        #dmodes += self.d.kspi0hh_dd_merged_up + self.d.kspi0hh_dd_resolved_up
        self._buildSel(dmodes,name,self.d0modes)

    def _createDpModes(self):
        
        # ( D->hhh )
        name = "D2HHH"
        #dmodes = self.d.hhh
        dmodes = self.d.hhh_pid
        # With Upstream tracks as well
        #dmodes += self.d.hhh_up
        self._buildSel(dmodes,name,self.dpmodes)

        # ( D->Ksh )
        name = "D2KSH"
        #dmodes = self.d.ksh_ll   + self.d.ksh_dd
        dmodes = self.d.ksh_ll_pid   + self.d.ksh_dd_pid
        # With Upstream tracks as well
        #dmodes += self.d.ksh_ll_up + self.d.ksh_dd_up
        self._buildSel(dmodes,name,self.dpmodes)

        # ( D->Kshhh )
        name = "D2KSHHH"
        #dmodes = self.d.kshhh_ll + self.d.kshhh_dd
        dmodes = self.d.kshhh_ll_pid + self.d.kshhh_dd_pid
        # With Upstream tracks as well
        #dmodes += self.d.kshhh_ll_up + self.d.kshhh_dd_up
        self._buildSel(dmodes,name,self.dpmodes)

        # ( D->hhhpi0 )
        name = "D2HHHPI0"
        #dmodes = self.d.pi0hhh_merged + self.d.pi0hhh_resolved
        dmodes = self.d.pi0hhh_merged_pid + self.d.pi0hhh_resolved_pid 
        # With Upstream tracks as well
        #dmodes += self.d.pi0hhh_merged_up + self.d.pi0hhh_resolved_up
        self._buildSel(dmodes,name,self.dpmodes)

        # ( D->Kshpi0 )
        name = "D2KSHPI0"
        dmodes  = self.d.kspi0h_ll_merged + self.d.kspi0h_ll_resolved
        dmodes += self.d.kspi0h_dd_merged + self.d.kspi0h_dd_resolved
        # With Upstream
        #dmodes += self.d.kspi0h_ll_merged_up + self.d.kspi0h_ll_resolved_up
        #dmodes += self.d.kspi0h_dd_merged_up + self.d.kspi0h_dd_resolved_up
        self._buildSel(dmodes,name,self.dpmodes)

        # ( D -> K*+hh )
        name = "D2KstHH"
        dmodes = self.d.kstarhh
        self._buildSel(dmodes,name,self.dpmodes)

        # ( D -> K*+K*0 )
        name = "D2KstKst0"
        dmodes = self.d.kstarkstar0
        self._buildSel(dmodes,name,self.dpmodes)

        # ( D -> K*+Ks )
        name = "D2KstKS"
        dmodes = self.d.kstarks_ll + self.d.kstarks_dd
        self._buildSel(dmodes,name,self.dpmodes)
  
    def _createDstarPlusModes(self):

        # D*+ modes ( D0 -> hh )
        name = "Dst2D0PID02HH"
        #dmodes = self.dst.d0pi
        dmodes = self.dst.d0pi_pid
        #dmodes += self.dst.d0pi_dup + self.dst.d0pi_hup + self.dst.d0pi_dup_hup
        self._buildSel(dmodes,name,self.dstarpmodes)

        # D*+ modes ( D0 -> hhhh )
        name = "Dst2D0PID02HHHH"
        #dmodes = self.dst.d0pi_hhhh
        dmodes = self.dst.d0pi_hhhh_pid
        #dmodes += self.dst.d0pi_hhhh_dup + self.dst.d0pi_hhhh_hup + self.dst.d0pi_hhhh_dup_hup
        self._buildSel(dmodes,name,self.dstarpmodes)

        # D*+ modes ( D0 -> Kshh )
        name = "Dst2D0PID02KSHH"
        #dmodes = self.dst.d0pi_kshh_ll + self.dst.d0pi_kshh_dd
        dmodes = self.dst.d0pi_kshh_ll_pid + self.dst.d0pi_kshh_dd_pid
        #dmodes += self.dst.d0pi_kshh_ll_dup + self.dst.d0pi_kshh_dd_dup
        #dmodes += self.dst.d0pi_kshh_ll_hup + self.dst.d0pi_kshh_dd_hup
        self._buildSel(dmodes,name,self.dstarpmodes)

        # D*+ modes ( D0 -> KsPi0hh )
        name = "Dst2D0PID02KSPI0HH"
        #dmodes  = self.dst.d0pi_kspi0hh_ll_merged + self.dst.d0pi_kspi0hh_dd_merged
        #dmodes += self.dst.d0pi_kspi0hh_ll_resolved + self.dst.d0pi_kspi0hh_dd_resolved
        dmodes  = self.dst.d0pi_kspi0hh_ll_merged_pid + self.dst.d0pi_kspi0hh_dd_merged_pid
        dmodes += self.dst.d0pi_kspi0hh_ll_resolved_pid + self.dst.d0pi_kspi0hh_dd_resolved_pid
        #dmodes += self.dst.d0pi_kspi0hh_ll_merged_dup + self.dst.d0pi_kspi0hh_dd_merged_dup
        #dmodes += self.dst.d0pi_kspi0hh_ll_resolved_dup + self.dst.d0pi_kspi0hh_dd_resolved_dup
        #dmodes += self.dst.d0pi_kspi0hh_ll_merged_hup + self.dst.d0pi_kspi0hh_dd_merged_hup
        #dmodes += self.dst.d0pi_kspi0hh_ll_resolved_hup + self.dst.d0pi_kspi0hh_dd_resolved_hup
        self._buildSel(dmodes,name,self.dstarpmodes)

        # Ds*+ pi0 modes ( Ds+ -> hhh )
        name = "Dst2DPI0D2HHH"
        #dmodes = self.dst.dpi0_merged + self.dst.dpi0_resolved
        dmodes = self.dst.dpi0_merged_pid + self.dst.dpi0_resolved_pid
        #dmodes += self.dst.dpi0_merged_dup + self.dst.dpi0_resolved_dup
        self._buildSel(dmodes,name,self.dstarpmodes)
        
        # Ds*+ pi0 modes ( Ds+ -> Ksh )
        name = "Dst2DPI0D2KSH"
        dmodes  = self.dst.dpi0_merged_ksh_ll + self.dst.dpi0_resolved_ksh_ll
        dmodes += self.dst.dpi0_merged_ksh_dd + self.dst.dpi0_resolved_ksh_dd
        #dmodes += self.dst.dpi0_merged_ksh_ll_dup + self.dst.dpi0_resolved_ksh_ll_dup
        #dmodes += self.dst.dpi0_merged_ksh_dd_dup + self.dst.dpi0_resolved_ksh_dd_dup
        self._buildSel(dmodes,name,self.dstarpmodes)

        # Ds*+ pi0 modes ( Ds+ -> Kshhh )
        name = "Dst2DPI0D2KSHHH"
        #dmodes  = self.dst.dpi0_merged_kshhh_ll + self.dst.dpi0_resolved_kshhh_ll
        #dmodes += self.dst.dpi0_merged_kshhh_dd + self.dst.dpi0_resolved_kshhh_dd
        dmodes  = self.dst.dpi0_merged_kshhh_ll_pid + self.dst.dpi0_resolved_kshhh_ll_pid
        dmodes += self.dst.dpi0_merged_kshhh_dd_pid + self.dst.dpi0_resolved_kshhh_dd_pid
        #dmodes += self.dst.dpi0_merged_kshhh_ll_dup + self.dst.dpi0_resolved_kshhh_ll_dup
        #dmodes += self.dst.dpi0_merged_kshhh_dd_dup + self.dst.dpi0_resolved_kshhh_dd_dup
        self._buildSel(dmodes,name,self.dstarpmodes)

        # Ds*+ pi0 modes ( Ds+ -> KsPi0h )
        name = "Dst2DPI0D2KSPI0H"
        dmodes  = self.dst.dpi0_merged_kspi0h_ll + self.dst.dpi0_merged_kspi0h_dd
        dmodes += self.dst.dpi0_resolved_kspi0h_ll + self.dst.dpi0_resolved_kspi0h_dd
        #dmodes += self.dst.dpi0_merged_kspi0h_ll_dup + self.dst.dpi0_merged_kspi0h_dd_dup
        #dmodes += self.dst.dpi0_resolved_kspi0h_ll_dup + self.dst.dpi0_resolved_kspi0h_dd_dup
        self._buildSel(dmodes,name,self.dstarpmodes)

        # Ds*+ gamma modes ( Ds+ -> hhh )
        name = "DStar2DGammaD2HHH"
        #dmodes = self.dst.dgamma_hhh
        dmodes = self.dst.dgamma_hhh_pid
        #dmodes += self.dst.dgamma_hhh_dup
        self._buildSel(dmodes,name,self.dstarpmodes)

        # Ds*+ gamma modes ( Ds+ -> Ksh )
        name = "Dst2DGammaD2KSH"
        dmodes = self.dst.dgamma_ksh_ll + self.dst.dgamma_ksh_dd
        #dmodes += self.dst.dgamma_ksh_ll_dup + self.dst.dgamma_ksh_dd_dup
        self._buildSel(dmodes,name,self.dstarpmodes)

        # Ds*+ gamma modes ( Ds+ -> Kshhh )
        name = "Dst2DGammaD2KSHHH"
        #dmodes = self.dst.dgamma_kshhh_ll + self.dst.dgamma_kshhh_dd
        dmodes = self.dst.dgamma_kshhh_ll_pid + self.dst.dgamma_kshhh_dd_pid
        #dmodes += self.dst.dgamma_kshhh_ll_dup + self.dst.dgamma_kshhh_dd_dup
        self._buildSel(dmodes,name,self.dstarpmodes)

        # Ds*+ gamma modes ( Ds+ -> KsPi0h )
        name = "Dst2DGammaD2KSPI0H"
        dmodes = self.dst.dgamma_kspi0h_ll + self.dst.dgamma_kspi0h_dd
        #dmodes += self.dst.dgamma_kspi0h_ll_dup + self.dst.dgamma_kspi0h_dd_dup
        self._buildSel(dmodes,name,self.dstarpmodes)


    def _createDstar0Modes(self):

        # D0* pi0 ( D0 -> hh )
        name = "Dst02D0PI0D02HH"
        #dmodes  = self.dst.d0pi0_merged      + self.dst.d0pi0_resolved
        dmodes  = self.dst.d0pi0_merged_pid      + self.dst.d0pi0_resolved_pid
        #dmodes += self.dst.d0pi0_merged_dup  + self.dst.d0pi0_resolved_dup
        self._buildSel(dmodes,name,self.dstar0modes)

        # D0* pi0 ( D0 -> hhhh )
        name = "Dst02D0PI0D02HHHH"
        #dmodes = self.dst.d0pi0_hhhh_merged + self.dst.d0pi0_hhhh_resolved
        dmodes = self.dst.d0pi0_hhhh_merged_pid + self.dst.d0pi0_hhhh_resolved_pid
        #dmodes += self.dst.d0pi0_hhhh_merged_dup + self.dst.d0pi0_hhhh_resolved_dup
        self._buildSel(dmodes,name,self.dstar0modes)

        # D0* pi0 ( D0 -> Kshh )
        name = "Dst02D0PI0D02KSHH"
        #dmodes = self.dst.d0pi0_kshh_ll     + self.dst.d0pi0_kshh_dd
        dmodes = self.dst.d0pi0_kshh_ll_pid     + self.dst.d0pi0_kshh_dd_pid
        #dmodes += self.dst.d0pi0_kshh_ll_dup + self.dst.d0pi0_kshh_dd_dup
        self._buildSel(dmodes,name,self.dstar0modes)

         # D0* pi0 ( D0 -> Kshh )
        name = "Dst02D0GammaD02KSHH"
        #dmodes = self.dst.d0gamma_kshh_ll   + self.dst.d0gamma_kshh_dd
        dmodes = self.dst.d0gamma_kshh_ll_pid   + self.dst.d0gamma_kshh_dd_pid
        #dmodes += self.dst.d0gamma_kshh_ll_dup + self.dst.d0gamma_kshh_dd_dup
        self._buildSel(dmodes,name,self.dstar0modes)


    def _createLines(self,name,modes0,modes1,descr,prescale=1.0):

        # Loop over all combinations and build the lines
        for d0name,d0mode in modes0.iteritems():
            for d1name,d1mode in modes1.iteritems():
                tag = d0name + d1name
                #print "'Stripping" + name + tag + "Beauty2CharmLine',"
                decays = { name : descr }
                inputs = { name : [ d0mode, d1mode ] }
                bc2dd = makeB2XSels( decays, tag, inputs, self.config )
                self.lines.append( ProtoLine(bc2dd,prescale) )

       
    def _makeBc2DD0(self):
        '''Makes Bc+ -> D+ D0 + c.c.'''
        self._createLines( 'Bc2DD0', self.dpmodes, self.d0modes, 
                           ["B_c+ -> D+ D0","B_c- -> D- D0"] )
        

    def _makeBc2DstarD0(self):
        '''Makes Bc+ -> D*+ D0 + c.c.'''
        self._createLines( "Bc2DstD0", self.dstarpmodes, self.d0modes,
                           ["B_c+ -> D*(2010)+ D0",
                            "B_c- -> D*(2010)- D0",
                            "B_c+ -> D*_s+ D0",
                            "B_c- -> D*_s- D0"] )

    def _makeBc2DstarDstar0(self):
        '''Makes Bc+ -> D*+ D*0 + c.c.'''
        self._createLines( "Bc2DstDst0", self.dstarpmodes, self.dstar0modes,
                           ["B_c+ -> D*(2010)+ D*(2007)0",
                            "B_c- -> D*(2010)- D*(2007)0",
                            "B_c+ -> D*_s+ D*(2007)0",
                            "B_c- -> D*_s- D*(2007)0"] )
