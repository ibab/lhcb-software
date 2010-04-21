# $Id: StrippingB0q2DplusMuX.py,v 1.1 2010-04-21 12:10:42 rlambert Exp $

__author__ = [ 'Rob LAmbert' ]
__date__ = '2010-04-20'
__version = '$Revision: 1.1 $'

# Many selections are put in here,
# A) MC09:   the full MC09 offline selection,
# B) Presel: the MC09-tuned preselection, for early data only!
# C) Tuned:  The data-tuned stripping selection, to be written
#
# A and B are not tunable, since one is the tightest and the other the loosest, they should only ever be prescaled
#
# C will be tunable in between A and B, but isn't written yet.
#
# All lines look basically like this:
#
#   1) Make a D(s)
#   2) Choose a Muon
#   3) Make the B(s)
#

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine, StrippingMember

class StrippingB0q2DplusMuXClass(object) :
    """
    Stripping seelctions, initially optimised on MC09 data
    
    """
    SelType="MC09" #MC09 or Presel (configure/return one line)
    KnownLines=["MC09","Presel"] #list of all known lines
    
    def Mu(self):
        """
        the bachelor muon selection
        """
        MuCut="Error"
        
        #####################         MC09 or EarlyData   ##################################
        if self.SelType=="MC09":
            MuCut = "((ISMUON) & (HASMUON) & (ISLONG) & (PT >  600.*MeV) & (MIPCHI2DV(PRIMARY) > 2.0) & (TRCHI2DOF<2) & (MIPDV(PRIMARY) > 0.01) & (PIDmu-PIDpi > 0.0) & (PIDmu-PIDK > -10.0))"
        #####################         Presel        ##################################
        elif self.SelType=="Presel":
            MuCut = "((ISMUON) & (HASMUON) & (ISLONG) & (PT >  500.*MeV) & (MIPCHI2DV(PRIMARY) > 2.0) & (TRCHI2DOF<9) & (MIPDV(PRIMARY) > 0.01) & (PIDmu-PIDpi > -5.0) & (PIDmu-PIDK > -10.0))"
        else:
            raise TypeError, 'SelType ' + self.SelType + ' not known'
        
        ###################   Create selection  ###################################
        MuForB0q = FilterDesktop(self.SelType+"MuForB0q")
        MuForB0q.Code = MuCut
        MyStdMuons = DataOnDemand('DODStdLooseMuons', Location = 'StdLooseMuons')
        SelMuForB0q = Selection("SelMuForB0q2DplusMuX"+self.SelType, Algorithm=MuForB0q, RequiredSelections = [MyStdMuons])
        
        return SelMuForB0q
    
    def Dplus(self):
        """
        Dplus for the selection
        Don't want to use the standard, because the decay descriptor is sub-optimal
        Here [D_s- -> K+ K- pi-]cc
        Which can be assosciated in this selection to:
        [(Meson & Charm & Strange) ==> K+ K- pi- {pi0} ]
        or
        [(Meson & Charm & Down) ==> K+ K- pi- {pi0} ]
        
        """
        KCut="Error"
        PiCut="Error"
        DCombCut="Error"
        DMothCut="Error"
        #####################         MC09        ##################################
        if self.SelType=="MC09":
            KCut= "(P > 2.0*GeV) & (MIPDV(PRIMARY) > 0.03) & (MIPCHI2DV(PRIMARY) > 4) & (TRCHI2DOF < 3) & (PIDK-PIDpi >  7) & (PIDK-PIDmu  > 10)  & (PIDK-PIDp  > -10) & (PT>400*MeV)"
            PiCut="(P > 2.0*GeV) & (MIPDV(PRIMARY) > 0.01) & (MIPCHI2DV(PRIMARY) > 7) & (TRCHI2DOF < 9) & (PIDpi-PIDK > -5) & (PIDpi-PIDmu > -2)  & (PT>300*MeV)"
            DCombCut="(ADAMASS('D_s-')<210*MeV) & (APT>1200*MeV)"
            DMothCut="("\
                      "(VFASPF(VPCHI2) > 0.015) "\
                      "& (VFASPF(VCHI2/VDOF) < 16.0) "\
                      "& (M > 1768.*MeV) & (M < 2068.*MeV) "\
                      "& (PT > 1500.*MeV) "\
                      "& (BPVDIRA > 0.99) "\
                      "& (BPVVDZ  > 1.0*mm) "\
                      "& (BPVVDCHI2 > 144) "\
                      ")"
        #####################         Presel        ##################################
        elif self.SelType=="Presel":
            KCut= "(P > 2.0*GeV) & (MIPDV(PRIMARY) > 0.03) & (MIPCHI2DV(PRIMARY) > 4) & (TRCHI2DOF <  9) & (PIDK-PIDpi >  -5)  & (PT>300*MeV)"
            PiCut="(P > 2.0*GeV) & (MIPDV(PRIMARY) > 0.01) & (MIPCHI2DV(PRIMARY) > 4) & (TRCHI2DOF < 16) & (PIDpi-PIDmu > -5)  & (PT>300*MeV)"
            DCombCut="(ADAMASS('D_s-')<200*MeV) & (APT>1000*MeV)"
            DMothCut="("\
                      "(VFASPF(VPCHI2) > 0.00025) "\
                      "& (VFASPF(VCHI2/VDOF) < 16.0) "\
                      "& (M > 1768.*MeV) & (M < 2068.*MeV) "\
                      "& (PT > 1000.*MeV) "\
                      "& (BPVDIRA > 0.99) "\
                      "& (BPVVDZ  > 1.0*mm) "\
                      "& (BPVVDCHI2 > 49) "\
                      ")"
        
        else:
            raise TypeError, 'SelType ' + self.SelType + ' not known'
        
        ###################   Create selection  ###################################
        
        Dplus2KKpiForB0q = CombineParticles(self.SelType+"Dplus2KKpiForB0q")
        #don't think I need this bit, automatically added by selections
        #Dplus2KKpiForB0q.InputLocations = [ "StdLoosePions", "StdLooseKaons" ]
        Dplus2KKpiForB0q.DecayDescriptor = "[D_s- -> K+ K- pi-]cc" 
        Dplus2KKpiForB0q.DaughtersCuts = {
            "K+"  : KCut,
            "pi+" : PiCut
            } 
        Dplus2KKpiForB0q.CombinationCut = DCombCut
        Dplus2KKpiForB0q.MotherCut = DMothCut
        
        MyStdLooseKaons = DataOnDemand('DODStdLooseDs2KKpi',
                                    Location = 'StdLooseKaons')
        MyStdLoosePions = DataOnDemand('DODStdLooseDs2KKpi',
                                    Location = 'StdLoosePions')
        
        SelDplus2KKpiForB0q = Selection("SelDplus2KKpiForB0q"+self.SelType, Algorithm=Dplus2KKpiForB0q, RequiredSelections = [MyStdLooseKaons,MyStdLoosePions])
        
        return SelDplus2KKpiForB0q
    
    def B0q(self):
        """
        B0q selection:
        [B_s0 -> D_s- mu+]cc, [B_s0 -> D_s+ mu+]cc
        But really this can be associated to anything of the form:
        [ (Meson & Beauty & Strange) => l+ Nu ( (Meson & Charm & Strange) ==> K+ K- pi- {pi0} ) {pi0} ]CC
        or
        [ (Meson & Beauty & Down) => l+ Nu ( (Meson & Charm & Down) ==> K+ K- pi- {pi0} ) {pi0} ]CC
        """
        BCut ="Error"
        BCombCut ="Error"

        #####################         MC09        ##################################
        if self.SelType=="" or self.SelType=="MC09":
            BCut = "("\
                   "(M < 6300.*MeV) & (M > 2200.*MeV) & (VFASPF(VCHI2/VDOF)<16) & "\
                   "(VFASPF(VPCHI2)>0.015) & "\
                   "((MINTREE(('D_s+'==ABSID),VFASPF(VZ))-VFASPF(VZ))>0*mm) "\
                   "& (BPVDIRA>0.998) "\
                   "& (BPVVDCHI2>1) "\
                   ")"
            
            BCombCut    = "((AM > 2000.*MeV))"
        
        #####################         Presel        ##################################
        elif self.SelType=="Presel":
            
            BCut = "("\
                   "(M < 6300.*MeV) & (M > 2200.*MeV) & (VFASPF(VPCHI2)>0.00025) & (VFASPF(VCHI2/VDOF)<16) & "\
                   "((MINTREE(('D_s+'==ABSID),VFASPF(VZ))-VFASPF(VZ))>0*mm) "\
                   "& (BPVDIRA>0.997) & (BPVVDCHI2>1) "\
                   ")"
            
            BCombCut    = "((AM > 2000.*MeV))"
               
        else:
            raise TypeError, 'SelType ' + self.SelType + ' not known'

        ###################   Create selection  ###################################
        CombB0q2DplusMuX = CombineParticles(self.SelType+'CombB0q2DplusMuX')
        
        CombB0q2DplusMuX.DecayDescriptors = ["[B_s0 -> D_s- mu+]cc", "[B_s0 -> D_s+ mu+]cc"] #includes wrong charges
        
        #don't think I need this bit.. automatically added by selections
        #B0q2DplusMuX.InputLocations = ["Dplus2KKPiForB0q", "MuForB0q"]
        #CombB0q2DplusMuX.InputLocations= ['Phys/SelDs2KKPi', 'Phys/SelBachelorMuForBs2DsMuX']
        
        if BCombCut is not None: CombB0q2DplusMuX.CombinationCut = BCombCut
        
        CombB0q2DplusMuX.MotherCut      = BCut
        
        SelB0q2DplusMuX = Selection("SelB0q2DplusMuX"+self.SelType, Algorithm=CombB0q2DplusMuX,
                                RequiredSelections = [self.Dplus(), self.Mu()])
        
        from PhysSelPython.Wrappers import SelectionSequence
        SeqB0q2DplusMuX = SelectionSequence("SeqB0q2DplusMuX"+self.SelType, TopSelection = SelB0q2DplusMuX)
        
        
        return SeqB0q2DplusMuX
    
    def Lines(self):
        '''Return all known lines'''
        allLines=[]
        for line in self.Lines:
            self.SelType=line
            line = StrippingLine("B0q2DplusMuX"+self.SelType
                                 , prescale = 1.
                                 , algos = [ self.B0q() ]
                                 )
            allLines.append(line)
        return allLines
    
