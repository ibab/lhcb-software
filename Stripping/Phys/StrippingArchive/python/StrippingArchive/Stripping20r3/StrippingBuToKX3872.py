# $Id: StrippingBu2KX3872.py,v 1.2 2010-08-26 13:16:50 nmangiaf Exp $

__author__ = ['Jeremy Dickens']
__date__ = '21/02/2011'
__version__ = '$Revision: 1.2 $'

'''
Implements 3 lines: B+ -> K+ X3872, B+ -> K+ Psi(2S) and a looser B+ -> K+ JPsi pi+ pi+ 
'''

## Note this is just for testing the stripping on 25/02/2011
## Cuts may well change before the stripping is launched

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from StrippingUtils.Utils import LineBuilder

Stripping_BuToKX3872_TestDictonary = {
    'Prescale_BuToKX3872'    : 1.0,
    'Postscale_BuToKX3872'   : 1.0,

    'Prescale_BuToKX3872Loose'    : 1.0,
    'Postscale_BuToKX3872Loose'   : 1.0,

    'Prescale_BuToKPsi2S'    : 1.0,
    'Postscale_BuToKPsi2S'   : 1.0,

    'Prescale_BuToKPsi2SLoose'    : 1.0,
    'Postscale_BuToKPsi2SLoose'   : 1.0,

    # B cuts
    'Bu_Comb_MassWindow'         : 450.0,
    'Bu_Comb_MassWindowLoose'    : 500.0,
    'Bu_MassWindow'              : 400.0,
    'Bu_MassWindowLoose'         : 400.0,
    'Bu_VertexCHI2'              :   4.0,
    'Bu_VertexCHI2Loose'         :   5.0,
    'Bu_IPCHI2'                  :  15.0,
    'Bu_IPCHI2Loose'             :  20.0,
    'Bu_FlightCHI2'              :  40.0,
    'Bu_FlightCHI2Loose'         :  30.0,
    'Bu_DIRA'                    :   0.9995,
    'Bu_DIRALoose'               :   0.9995,
    
    # X3872 / Psi(2S) cuts
    'X3872_Comb_MassWindow'      : 180.0,
    'X3872_Comb_MassWindowLoose' : 220.0,
    'X3872_MassWindow'           : 150.0,
    'X3872_MassWindowLoose'      : 190.0,
    'X3872_VertexCHI2'           :   8.0,
    'X3872_VertexCHI2Loose'      :  10.0,

    # Track cuts
    'Track_CHI2nDOF'       : 4.0,

    # Kaon cuts
    'Kaon_MinIPCHI2'       : 6.0,
    'Kaon_MinIPCHI2Loose'  : 5.0,
    'Kaon_PT'              : 200.0,
    'Kaon_PTLoose'         : 150.0,

    # Pion cuts
    'Pion_MinIPCHI2'       : 6.0,
    'Pion_MinIPCHI2Loose'  : 5.0,

    # JPsi cuts
    'JPsi_MassWindow'       :  70.0,
    'JPsi_MassWindowLoose'  :  70.0,
    'JPsi_VertexCHI2'       :  10.0,
    'JPsi_VertexCHI2Loose'  :  10.0,

    # Muon cuts
    'Muon_MinIPCHI2'        :   3.0,
    'Muon_MinIPCHI2Loose'   :   2.0,
    'Muon_PT'               : 500.0,
    'Muon_IsMuon'           : True
    }

class StrippingBu2KX3872Conf(LineBuilder):
    """
    Configuration object for B+ -> K X(3872) lines
    """
    __configuration_keys__ = ( 
    'Prescale_BuToKX3872',
    'Postscale_BuToKX3872',

    'Prescale_BuToKX3872Loose',
    'Postscale_BuToKX3872Loose',

    'Prescale_BuToKPsi2S',
    'Postscale_BuToKPsi2S',

    'Prescale_BuToKPsi2SLoose',
    'Postscale_BuToKPsi2SLoose', 

    # B cuts
    'Bu_Comb_MassWindow', 
    'Bu_Comb_MassWindowLoose',
    'Bu_MassWindow',
    'Bu_MassWindowLoose',
    'Bu_VertexCHI2',  
    'Bu_VertexCHI2Loose',
    'Bu_IPCHI2', 
    'Bu_IPCHI2Loose',     
    'Bu_FlightCHI2',  
    'Bu_FlightCHI2Loose',
    'Bu_DIRA',     
    'Bu_DIRALoose', 
    
    # X3872 / Psi(2S) cuts
    'X3872_Comb_MassWindow',     
    'X3872_Comb_MassWindowLoose',
    'X3872_MassWindow',          
    'X3872_MassWindowLoose',     
    'X3872_VertexCHI2',          
    'X3872_VertexCHI2Loose',     

    # Track cuts
    'Track_CHI2nDOF',      

    # Kaon cuts
    'Kaon_MinIPCHI2',     
    'Kaon_MinIPCHI2Loose',
    'Kaon_PT',     
    'Kaon_PTLoose',

    # Pion cuts
    'Pion_MinIPCHI2',      
    'Pion_MinIPCHI2Loose',

    # JPsi cuts
    'JPsi_MassWindow',     
    'JPsi_MassWindowLoose',
    'JPsi_VertexCHI2',     
    'JPsi_VertexCHI2Loose',

    # Muon cuts
    'Muon_MinIPCHI2',      
    'Muon_MinIPCHI2Loose', 
    'Muon_PT',             
    'Muon_IsMuon'         
    )

    def __init__(self, name, config):
        '''The constructor of the configuration class.
        Requires a configuration dictionary, config, which must provide all the settings for cuts which are not hard coded
        '''

        LineBuilder.__init__(self, name, config)

        self.name = name

        self.BuToKX3872LineName = self.name + "_BuToKX3872"
        self.BuToKPsi2SLineName = self.name + "_BuToKPsi2S"
        self.BuToKX3872LooseLineName = self.name + "_BuToKX3872Loose"
        self.BuToKPsi2SLooseLineName = self.name + "_BuToKPsi2SLoose"
        #self.__PVOutputLocation__ = "Rec/Vertex/"+name+"_ModPrimary"
        ##self.__PVOutputLocation__ = "Rec/Vertex/Primary"

        ############################
        ## Define the cut strings ##
        ############################

        ###############
        #   Bu cuts  ##
        ###############
        
        self.BuCombCut = "(ADAMASS('B+') < %(Bu_Comb_MassWindow)s * MeV)" %config
        self.BuCombLooseCut = "(ADAMASS('B+') < %(Bu_Comb_MassWindowLoose)s * MeV)" %config

        self.BuCut = "(ADMASS('B+') < %(Bu_MassWindow)s * MeV) & (VFASPF(VCHI2/VDOF) < %(Bu_VertexCHI2)s) & (BPVIPCHI2() < %(Bu_IPCHI2)s) & (BPVDIRA> %(Bu_DIRA)s) & (BPVVDCHI2 > %(Bu_FlightCHI2)s)" %config
        self.BuLooseCut = "(ADMASS('B+') < %(Bu_MassWindowLoose)s * MeV) & (VFASPF(VCHI2/VDOF) < %(Bu_VertexCHI2Loose)s) & (BPVIPCHI2() < %(Bu_IPCHI2Loose)s) & (BPVDIRA> %(Bu_DIRALoose)s) & (BPVVDCHI2 > %(Bu_FlightCHI2Loose)s)" %config

        ##############################
        ##  X3872  and Psi(2S) cuts ##
        ##############################
        
        self.X3872CombCut = "(ADAMASS('X_1(3872)') < %(X3872_Comb_MassWindow)s * MeV)" %config
        self.X3872CombLooseCut = "(ADAMASS('X_1(3872)') < %(X3872_Comb_MassWindowLoose)s * MeV)" %config

        self.Psi2SCombCut = "(ADAMASS('psi(2S)') < %(X3872_Comb_MassWindow)s * MeV)" %config
        self.Psi2SCombLooseCut = "(ADAMASS('psi(2S)') < %(X3872_Comb_MassWindowLoose)s * MeV)" %config

        ResonanceVertexCut = "(VFASPF(VCHI2/VDOF) < %(X3872_VertexCHI2)s)" %config
        ResonanceVertexLooseCut = "(VFASPF(VCHI2/VDOF) < %(X3872_VertexCHI2Loose)s)" %config
        
        self.X3872Cut = "(ADMASS('X_1(3872)') < %(X3872_MassWindow)s * MeV) & " %config + ResonanceVertexCut
        self.X3872LooseCut = "(ADMASS('X_1(3872)') < %(X3872_MassWindowLoose)s * MeV) & " %config + ResonanceVertexLooseCut

        self.Psi2SCut = "(ADMASS('psi(2S)') < %(X3872_MassWindow)s * MeV) & " %config + ResonanceVertexCut
        self.Psi2SLooseCut = "(ADMASS('psi(2S)') < %(X3872_MassWindowLoose)s * MeV) & " %config + ResonanceVertexLooseCut

        ######################
        ##    Track cuts    ##
        ######################

        TrackCut = "(TRCHI2DOF < %(Track_CHI2nDOF)s)" %config

        self.KaonCut = TrackCut + " & (PT > %(Kaon_PT)s * MeV) & (MIPCHI2DV(PRIMARY) > %(Kaon_MinIPCHI2)s)" %config
        self.KaonLooseCut = TrackCut + "  & (PT > %(Kaon_PTLoose)s * MeV) & (MIPCHI2DV(PRIMARY) > %(Kaon_MinIPCHI2Loose)s)" %config

        self.PionCut = TrackCut + " & (MIPCHI2DV(PRIMARY) > %(Pion_MinIPCHI2)s)" %config
        self.PionLooseCut = TrackCut + " & (MIPCHI2DV(PRIMARY) > %(Pion_MinIPCHI2Loose)s)" %config

        MuonCut = TrackCut + " & (MIPCHI2DV(PRIMARY) > %(Muon_MinIPCHI2)s) & (PT > %(Muon_PT)s * MeV)" %config
        if(config["Muon_IsMuon"]):
            MuonCut += " & (ISMUON)"
            
        MuonLooseCut = TrackCut + " & (MIPCHI2DV(PRIMARY) > %(Muon_MinIPCHI2Loose)s) & (PT > %(Muon_PT)s * MeV)" %config
        if(config["Muon_IsMuon"]):
            MuonLooseCut += " & (ISMUON)"

        MuonPVCut = TrackCut + " & (PT > %(Muon_PT)s * MeV)" %config
        if(config["Muon_IsMuon"]):
            MuonPVCut += " & (ISMUON)"

        ##################
        ##   Rho cuts   ##
        ##################

        self.RhoCuts = "(2 == NINTREE((ABSID=='pi+') & " + self.PionCut + "))"
        
        ######################
        ##    J/psi cuts    ##
        ######################

        JPsiCut = "(ADMASS('J/psi(1S)') < %(JPsi_MassWindow)s * MeV) & (VFASPF(VCHI2/VDOF) < %(JPsi_VertexCHI2)s)" %config
        JPsiLooseCut = "(ADMASS('J/psi(1S)') < %(JPsi_MassWindowLoose)s * MeV) & (VFASPF(VCHI2/VDOF) < %(JPsi_VertexCHI2Loose)s)" %config

        self.JPsiCuts = JPsiCut + " & (2 == NINTREE((ABSID=='mu-') & " + MuonCut + "))"
        self.JPsiLooseCuts = JPsiLooseCut + " & (2 == NINTREE((ABSID=='mu-') & " + MuonLooseCut + "))"

        #########################
        ## Make the selections ##
        #########################

        ## loose selections
        Sel_JPsiLoose = self.__FilterSelectionJPsi__(self.name + "Loose", self.JPsiLooseCuts)
        Sel_RhoLoose = self.__CreateSelectionRho__(self.name + "Loose", "", "ALL", self.PionCut)

        Sel_X3872Loose = self.__CreateSelectionX3872__(self.BuToKX3872LooseLineName, self.X3872CombLooseCut, self.X3872LooseCut, [Sel_JPsiLoose, Sel_RhoLoose])
        Sel_Psi2SLoose = self.__CreateSelectionX3872__(self.BuToKPsi2SLooseLineName, self.Psi2SCombLooseCut, self.Psi2SLooseCut, [Sel_JPsiLoose, Sel_RhoLoose])

        Sel_KaonLoose = self.__FilterKaon__(self.name + "Loose", self.KaonLooseCut)

        Sel_BuX3872KLoose = self.__CreateSelectionBu__(self.BuToKX3872LooseLineName, self.BuCombLooseCut, self.BuLooseCut, [Sel_X3872Loose, Sel_KaonLoose])
        Sel_BuPsi2SKLoose = self.__CreateSelectionBu__(self.BuToKPsi2SLooseLineName, self.BuCombLooseCut, self.BuLooseCut, [Sel_Psi2SLoose, Sel_KaonLoose])

        ## tight selections
        Sel_JPsi = self.__FilterSelectionJPsi__(self.name, self.JPsiCuts)
        Sel_Rho = self.__FilterSelectionRho__(self.name, self.RhoCuts, [Sel_RhoLoose])

        Sel_X3872 = self.__CreateSelectionX3872__(self.BuToKX3872LineName, self.X3872CombCut, self.X3872Cut, [Sel_JPsi, Sel_Rho])
        Sel_Psi2S = self.__CreateSelectionX3872__(self.BuToKPsi2SLineName, self.Psi2SCombCut, self.Psi2SCut, [Sel_JPsi, Sel_Rho])

        Sel_Kaon = self.__FilterKaon__(self.name, self.KaonCut)

        Sel_BuX3872K = self.__CreateSelectionBu__(self.BuToKX3872LineName, self.BuCombCut, self.BuCut, [Sel_X3872, Sel_Kaon])
        Sel_BuPsi2SK = self.__CreateSelectionBu__(self.BuToKPsi2SLineName, self.BuCombCut, self.BuCut, [Sel_Psi2S, Sel_Kaon])
    
        ###################################
        ## Construct the stripping lines ##
        ###################################

        from StrippingConf.StrippingLine import StrippingLine

        ## Define PV filter
        #MyFilterPVs = {'Code' : "VSOURCE('Rec/Vertex/Primary') >> (NTRACKS > 6) >> RV_SINKTES('"+self.__PVOutputLocation__+"') >> ~VEMPTY", 'Preambulo' : ["from LoKiPhys.decorators import *"]}

        ## --- B+ -> X3872 K+ loose line ---
        Line_BuToX3872Loose_Name = self.BuToKX3872LooseLineName + "Line"
        Line_BuToX3872Loose = StrippingLine( Line_BuToX3872Loose_Name,
                                             prescale = config['Prescale_BuToKX3872Loose'],
                                             postscale = config['Postscale_BuToKX3872Loose'],
                                             #FILTER = MyFilterPVs,
                                             selection = Sel_BuX3872KLoose)
        self.registerLine(Line_BuToX3872Loose)

        ## --- B+ -> Psi2S K+ loose line ---
        Line_BuToPsi2SLoose_Name = self.BuToKPsi2SLooseLineName + "Line"
        Line_BuToPsi2SLoose = StrippingLine( Line_BuToPsi2SLoose_Name,
                                             prescale = config['Prescale_BuToKPsi2SLoose'],
                                             postscale = config['Postscale_BuToKPsi2SLoose'],
                                             selection = Sel_BuPsi2SKLoose)
        self.registerLine(Line_BuToPsi2SLoose)

        ## --- B+ -> X3872 K+ line ---
        Line_BuToX3872_Name = self.BuToKX3872LineName + "Line"
        Line_BuToX3872 = StrippingLine( Line_BuToX3872_Name,
                                        prescale = config['Prescale_BuToKX3872'],
                                        postscale = config['Postscale_BuToKX3872'],
                                        selection = Sel_BuX3872K)
        self.registerLine(Line_BuToX3872)

        ## --- B+ -> Psi2S K+ line ---
        Line_BuToPsi2S_Name = self.BuToKPsi2SLineName + "Line"
        Line_BuToPsi2S = StrippingLine( Line_BuToPsi2S_Name,
                                        prescale = config['Prescale_BuToKPsi2S'],
                                        postscale = config['Postscale_BuToKPsi2S'],
                                        selection = Sel_BuPsi2SK)
        self.registerLine(Line_BuToPsi2S)

        #self.printCuts()
        
    def printCuts(self):

        '''Print the compiled cut values'''
        
        print " "
        print "-------------------------------------------"
        print "-- B+ -> K X3872 etc stripping line cuts --"
        print "-------------------------------------------"
        print " "
        print "  --> B+ -> K X3872 line"
        print "  --> Bu cut:                ", self.BuCut
        print "  --> Bu combination cut:    ", self.BuCombCut
        print "  --> X3872 cut:             ", self.X3872Cut
        print "  --> X3872 combination cut: ", self.X3872CombCut
        print "  --> JPsi cuts:             ", self.JPsiCuts
        print "  --> Pion cut:              ", self.PionCut
        print "  --> Kaon cut:              ", self.KaonCut

        print " "
        print "  --> B+ -> K X3872 loose line"
        print "  --> Bu cut:                ", self.BuLooseCut
        print "  --> Bu combination cut:    ", self.BuCombLooseCut
        print "  --> X3872 cut:             ", self.X3872LooseCut
        print "  --> X3872 combination cut: ", self.X3872CombLooseCut
        print "  --> JPsi cuts:             ", self.JPsiLooseCuts
        print "  --> Pion cut:              ", self.PionLooseCut
        print "  --> Kaon cut:              ", self.KaonLooseCut

        print " "
        print "  --> B+ -> K Psi(2S) line"
        print "  --> Bu cut:                ", self.BuCut
        print "  --> Bu combination cut:    ", self.BuCombCut
        print "  --> X3872 cut:             ", self.Psi2SCut
        print "  --> X3872 combination cut: ", self.Psi2SCombCut
        print "  --> JPsi cuts:             ", self.JPsiCuts
        print "  --> Pion cut:              ", self.PionCut
        print "  --> Kaon cut:              ", self.KaonCut

        print " "
        print "  --> B+ -> K Psi(2S) loose line"
        print "  --> Bu cut:                ", self.BuLooseCut
        print "  --> Bu combination cut:    ", self.BuCombLooseCut
        print "  --> X3872 cut:             ", self.Psi2SLooseCut
        print "  --> X3872 combination cut: ", self.Psi2SCombLooseCut
        print "  --> JPsi cuts:             ", self.JPsiLooseCuts
        print "  --> Pion cut:              ", self.PionLooseCut
        print "  --> Kaon cut:              ", self.KaonLooseCut

        return True

    ##########################################
    ## Create selections for StrippingLines ##
    ##########################################

    #################
    ## Filter Kaon ##
    #################

    def __FilterKaon__(self, lName, KaonCut):
        '''
        Kaon filter for Bu -> K X3872 (from StdLooseKaons)
        '''

        from StandardParticles import StdLooseKaons
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        FilterKaon = FilterDesktop()
        #FilterKaon.InputPrimaryVertices = self.__PVOutputLocation__
        FilterKaon.Code = KaonCut

        from PhysSelPython.Wrappers import Selection        
        SelKaon = Selection("SelFilter_" + lName + "_Kaon", Algorithm = FilterKaon,
                            RequiredSelections = [StdLooseKaons])

        return SelKaon

    #########################
    ## Create rho -> pi pi ##
    #########################

    def __CreateSelectionRho__(self, lName, RhoCombCut, RhoCut, PionCut):
        '''
        rho(770)0 -> pi+ pi- selection (from StdAllLoosePions)
        '''

        from StandardParticles import StdAllLoosePions

        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        CombineRho = CombineParticles()
        #CombineRho.InputPrimaryVertices = self.__PVOutputLocation__
        
        CombineRho.DecayDescriptor = "rho(770)0 -> pi+ pi-"

        if(len(RhoCombCut) > 0):
            CombineRho.CombinationCut = RhoCombCut
        
        CombineRho.MotherCut = RhoCut
        CombineRho.DaughtersCuts = { "pi+" : PionCut }

        from PhysSelPython.Wrappers import Selection
        SelRho = Selection("SelBuild_" + lName + "_Rho", Algorithm = CombineRho,
                           RequiredSelections = [StdAllLoosePions])

        return SelRho

    def __FilterSelectionRho__(self, lName, RhoCuts, InputSelections):
        '''
        Filter already built rho (eg from the loose line)
        '''

        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        FilterRho = FilterDesktop()
        #FilterRho.InputPrimaryVertices = self.__PVOutputLocation__
        FilterRho.Code = RhoCuts

        from PhysSelPython.Wrappers import Selection        
        SelRho = Selection("SelFilter_" + lName + "_Rho", Algorithm = FilterRho,
                           RequiredSelections = InputSelections)
        
        return SelRho

    ##########################
    ## Filter Jpsi -> mu mu ##
    ##########################

    def __FilterSelectionJPsi__(self, lName, JPsiCuts):
        '''
        J/psi(1S) -> mu+ mu- filter (from StdLooseJpsi2MuMu)
        '''

        from PhysSelPython.Wrappers import DataOnDemand
        StdJPsi = DataOnDemand(Location = "Phys/StdLooseJpsi2MuMu/Particles")

        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        FilterJPsi = FilterDesktop()
        #FilterJPsi.InputPrimaryVertices = self.__PVOutputLocation__
        FilterJPsi.Code = JPsiCuts

        from PhysSelPython.Wrappers import Selection        
        SelJPsi = Selection("SelFilter_" + lName + "_JPsi", Algorithm = FilterJPsi,
                            RequiredSelections = [StdJPsi])
        
        return SelJPsi

    ##############################
    ## Create X3872 -> JPsi rho ##
    ##############################

    def __CreateSelectionX3872__(self, lName, CombCut, MotherCut, InputSelections):
        '''
        X3872 -> J/psi(1S) rho(770)0: note this can be used for the psi(2S) as well - just use different cuts!
        '''

        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        CombineX3872 = CombineParticles()
        #CombineX3872.InputPrimaryVertices = self.__PVOutputLocation__

        CombineX3872.DecayDescriptor = "X_1(3872) -> J/psi(1S) rho(770)0"

        CombineX3872.CombinationCut = CombCut
        CombineX3872.MotherCut = MotherCut

        from PhysSelPython.Wrappers import Selection
        SelX3872 = Selection("Sel_" + lName + "_X3872", Algorithm = CombineX3872,
                             RequiredSelections = InputSelections)
        
        return SelX3872

    ###########################
    ## Create B+ -> X3872 K+ ##
    ###########################

    def __CreateSelectionBu__(self, lName, CombCut, MotherCut, InputSelections):
        '''
        B+ -> K+ X3872 Selection
        '''

        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        CombineBu = CombineParticles()
        #CombineBu.InputPrimaryVertices = self.__PVOutputLocation__
        
        CombineBu.DecayDescriptors = ["B+ -> X_1(3872) K+", "B- -> X_1(3872) K-"]

        CombineBu.CombinationCut = CombCut
        CombineBu.MotherCut = MotherCut

        from PhysSelPython.Wrappers import Selection
        SelBu = Selection("Sel_" + lName + "_Bu", Algorithm = CombineBu,
                          RequiredSelections = InputSelections)
        
        return SelBu

