# $Id: Hlt2B2JpsiXLines.py,v 1.25 2010-09-09 15:20:09 graven Exp $

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
class Hlt2B2JpsiXLinesConf(HltLinesConfigurableUser) :

    __slots__ = {'BsLifetimeCut': 0.15 # ps
                 ,'Prescale'    : { 'Hlt2Bs2JpsiPhiPrescaled' : 1.
                                   ,'Hlt2Bs2JpsiPhiDetached' : 1.
				   ,'Hlt2Bs2JpsiPhiWide' : 0.05
                                   ,'Hlt2Bs2JpsiPhiSignal' : 1.
                                   ,'Hlt2Bs2JpsieePhiUnbiasedPT' : 0.01
                                   ,'Hlt2Bs2JpsieePhiSignal' : 1.
                                   ,'Hlt2TFBs2JpsieePhiUnbiasedPT' : 0.01
                                   ,'Hlt2TFBs2JpsieePhiSignal' : 1.
				   ,'Hlt2Bd2JpsiKstarWide' : 0.05
                                   ,'Hlt2Bd2JpsiKstarSignal' : 1.
       				   ,'Hlt2Bu2JpsiKWide' : 0.05
                                   ,'Hlt2Bu2JpsiKSignal' : 1.
	                           ,'Hlt2Bd2JpsiMuMuKsLLBiased' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsLLUnbiased' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsDDBiased' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsDDUnbiased' : 1.
                                   ,'Hlt2B2JpsiX_MuMu' : 1.
                                   ,'Hlt2Bc2JpsiMuX'       : 1.  # In case too high rates, may be scaled
                                   ,'Hlt2Bc2JpsiMuXSignal' : 1.  # signal
                                   ,'Hlt2Bc2JpsiH'         : 1.
                                   ,'Hlt2Bc2JpsiHDetached' : 1.
                                   ,'Hlt2Bc2JpsiHPrescaled': 0.5  # may be scaled 
                                   ,'Hlt2TFBc2JpsiMuX'       : 1.  # In case too high rates, may be scaled
                                   ,'Hlt2TFBc2JpsiMuXSignal' : 1.  # signal
                                   ,'Hlt2TFBc2JpsiH'         : 1.
                                   ,'Hlt2TFBc2JpsiHDetached' : 1.
                                   ,'Hlt2TFBc2JpsiHPrescaled': 0.5  # may be scaled                                   
                                    }
                 ,'Postscale'   : { 'Hlt2Bs2JpsiPhiPrescaled' : 0.1
                                   ,'Hlt2Bs2JpsiPhiDetached' : 1.
                                   ,'Hlt2Bs2JpsiPhiWide' : 1.
                                   ,'Hlt2Bs2JpsiPhiSignal' : 1.
                                   ,'Hlt2Bs2JpsieePhiUnbiasedPT' : 1.
                                   ,'Hlt2Bs2JpsieePhiSignal' : 1.
                                   ,'Hlt2TFBs2JpsieePhiUnbiasedPT' : 1.
                                   ,'Hlt2TFBs2JpsieePhiSignal' : 1.
				   ,'Hlt2Bd2JpsiKstarWide' : 1.
                                   ,'Hlt2Bd2JpsiKstarSignal' : 1.
                  		   ,'Hlt2Bu2JpsiKWide' : 1.
                                   ,'Hlt2Bu2JpsiKSignal' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsLLBiased' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsLLUnbiased' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsDDBiased' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsDDUnbiased' : 1.
                                   ,'Hlt2B2JpsiX_MuMu' : 1.
                                   ,'Hlt2Bc2JpsiMuX'       : 1.  # In case too high rates, may be scaled
                                   ,'Hlt2Bc2JpsiMuXSignal' : 1.  # signal
                                   ,'Hlt2Bc2JpsiH'         : 1.  
                                   ,'Hlt2Bc2JpsiHDetached' : 1.
                                   ,'Hlt2Bc2JpsiHPrescaled': 1.  #  may be scaled
                                   ,'Hlt2TFBc2JpsiMuX'       : 1.  # In case too high rates, may be scaled
                                   ,'Hlt2TFBc2JpsiMuXSignal' : 1.  # signal
                                   ,'Hlt2TFBc2JpsiH'         : 1.
                                   ,'Hlt2TFBc2JpsiHDetached' : 1.  
                                   ,'Hlt2TFBc2JpsiHPrescaled': 1.  # may be scaled 
                                    }
                 ,'HltANNSvcID'  : {
                                    'Bs2JpsieePhiUnbiasedPT'   : 50390
                                   ,'Bs2JpsieePhiSignal'       : 50391
                                   ,'TFBs2JpsieePhiUnbiasedPT' : 50395
                                   ,'TFBs2JpsieePhiSignal'     : 50396
                                   ,'Bc2JpsiMuX'           : 54010
                                   ,'Bc2JpsiMuXSignal'     : 54011
                                   ,'Bc2JpsiH'             : 54020
                                   ,'Bc2JpsiHPrescaled'    : 54030
                                   ,'Bc2JpsiHDetached'     : 54031
                                   ,'TFBc2JpsiMuX'           : 54110
                                   ,'TFBc2JpsiMuXSignal'     : 54111
                                   ,'TFBc2JpsiH'             : 54120
                                   ,'TFBc2JpsiHPrescaled'    : 54130
                                   ,'TFBc2JpsiHDetached'     : 54131  
                                    }
                 
                 
                 # Bc->Jpsi(MuMu)MuX
                 , 'Bc2JpsiMuX_BcUpperMass'         : 7000.  # MeV, Upper limit for partial rec.
                 , 'Bc2JpsiMuX_BcLowerMass'         : 3000.  # MeV, Lower limit for partial rec.
                 , 'Bc2JpsiMuX_BcVtxCHI2'           :   25.  # adimentional
                 , 'Bc2JpsiMuX_BcPT'                :    0.  # MeV, May incrase up to 5000 MeV if needed
                 , 'Bc2JpsiMuX_JpsiMassWindow'      :  300.  # MeV, about 10 sigma, may decread to 250
                 , 'Bc2JpsiMuX_JpsiTightMassWindow' :  120.  # MeV, at least 3 sigma (30 MeV for Bc)
                 , 'Bc2JpsiMuX_JpsiVtxCHI2'         :   25.  # adimentional
                 , 'Bc2JpsiMuX_MuonJpsiPT'          :  800.  # MeV
                 , 'Bc2JpsiMuX_MuonBcPT'            : 1200.  # MeV
                 , 'Bc2JpsiMuX_TrackCHI2DOF'        :   25.  # adimentional
                 # Bc->Jpsi(MuMu)H
                 , 'Bc2JpsiH_BcMassWindow'        :  400.   # MeV, Mass window for Bc 
                 , 'Bc2JpsiH_BcVtxCHI2'           :   25.   # adimentional
                 , 'Bc2JpsiH_BcPT'                :    0.   # MeV, May incrase up to 5000 MeV if needed
                 , 'Bc2JpsiH_JpsiMassWindow'      :  120.   # MeV, at least 3 sigma (30 MeV for Bc)
                 , 'Bc2JpsiH_JpsiVtxCHI2'         :   25.   # adimentional
                 , 'Bc2JpsiH_MuonJpsiPT'          :  800.   # MeV 
                 , 'Bc2JpsiH_PionPT'              : 1200.   # MeV
                 # Bc->Jpsi(MuMu)H Prescaled and Detached (as Bc2JpsiHPD)
                 , 'Bc2JpsiHPD_BcMassWindow'        :  400.   # MeV, Mass window for Bc 
                 , 'Bc2JpsiHPD_BcVtxCHI2'           :   25.   # adimentional
                 , 'Bc2JpsiHPD_BcPT'                :    0.   # MeV, May incrase up to 5000 MeV if needed
                 , 'Bc2JpsiHPD_JpsiMassWindow'      :  120.   # MeV, at least 3 sigma (30 MeV for Bc)
                 , 'Bc2JpsiHPD_JpsiVtxCHI2'         :   25.   # adimentional
                 , 'Bc2JpsiHPD_MuonJpsiPT'          :  800.   # MeV 
                 , 'Bc2JpsiHPD_PionPT'              : 1200.   # MeV
                 , 'Bc2JpsiHPD_PionIPCHI2'          :    4.   # adimentional
                 , 'Bc2JpsiHPD_JpsiIPCHI2'          :   -1.   # adimentional, choose one  
                 , 'Bc2JpsiHPD_MuonIPCHI2'          :   -1.   # adimentional, choose one 
                 , 'Bc2JpsiHPD_BcIPCHI2'            :   25.   # adimentional

                 # Bs->Jpsi(ee)Phi
                 , 'Bs2JpsieePhi_L0Req'               :  "L0_CHANNEL('Electron')"
                 , 'Bs2JpsieePhi_Hlt1Req'             :  "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')" 
                 
                 , 'Bs2JpsieePhi_ElectronPT'          :  500.     # MeV
                 , 'Bs2JpsieePhi_ElectronMIPCHI2'     :    2.25   # adimensional
                 , 'Bs2JpsieePhi_JpsiVertexCHI2pDOF'  :   25.     # adimensional
                 , 'Bs2JpsieePhi_JpsiMassMin'         : 2500.     # MeV
                 , 'Bs2JpsieePhi_JpsiMassMax'         : 3300.     # MeV
                 , 'Bs2JpsieePhi_KaonMIPCHI2'         :    2.25   # adimensional
                 , 'Bs2JpsieePhi_PhiPT'               :  900.     # MeV
                 , 'Bs2JpsieePhi_PhiVertexCHI2pDOF'   :   25.     # adimensional
                 , 'Bs2JpsieePhi_PhiMassMin'          :  990.     # MeV
                 , 'Bs2JpsieePhi_PhiMassMax'          : 1050.     # MeV
                 , 'Bs2JpsieePhi_BsDIRA'              :    0.99   # adimensional
                 , 'Bs2JpsieePhi_BsVertexCHI2pDOF'    :   25.     # adimensional
                 , 'Bs2JpsieePhi_BsPseudoMassMin'     : 3800.     # MeV
                 , 'Bs2JpsieePhi_BsPseudoMassMax'     : 6800.     # MeV

                 
                 # TrackFitted Bc->Jpsi(MuMu)MuX
                 , 'TFBc2JpsiMuX_BcUpperMass'         : 6400.  # MeV, Upper limit for partial rec.
                 , 'TFBc2JpsiMuX_BcLowerMass'         : 3200.  # MeV, Lower limit for partial rec.
                 , 'TFBc2JpsiMuX_BcVtxCHI2'           :   25.  # adimentional
                 , 'TFBc2JpsiMuX_BcPT'                : 5000.  # MeV, May incrase up to 5000 MeV if needed
                 , 'TFBc2JpsiMuX_JpsiMassWindow'      :  220.  # MeV, about 10 sigma, may decread to 200
                 , 'TFBc2JpsiMuX_JpsiTightMassWindow' :  100.  # MeV, at least 3 sigma (30 MeV for Bc)
                 , 'TFBc2JpsiMuX_JpsiVtxCHI2'         :   20.  # adimentional
                 , 'TFBc2JpsiMuX_MuonJpsiPT'          : 1200.  # MeV
                 , 'TFBc2JpsiMuX_MuonBcPT'            : 2000.  # MeV
                 , 'TFBc2JpsiMuX_TrackCHI2DOF'        :   25.  # adimentional
                 # TrackFitted Bc->Jpsi(MuMu)H
                 , 'TFBc2JpsiH_BcMassWindow'        :  400.   # MeV, Mass window for Bc 
                 , 'TFBc2JpsiH_BcVtxCHI2'           :   25.   # adimentional
                 , 'TFBc2JpsiH_BcPT'                :    0.   # MeV, May incrase up to 5000 MeV if needed
                 , 'TFBc2JpsiH_JpsiMassWindow'      :  100.   # MeV, at least 3 sigma (30 MeV for Bc)
                 , 'TFBc2JpsiH_JpsiVtxCHI2'         :   25.   # adimentional
                 , 'TFBc2JpsiH_MuonJpsiPT'          :  800.   # MeV 
                 , 'TFBc2JpsiH_PionPT'              : 1200.   # MeV
                 # TrackFitted Bc->Jpsi(MuMu)H Prescaled and Detached (as TFBc2JpsiHPD)
                 , 'TFBc2JpsiHPD_BcMassWindow'        :  400.   # MeV, Mass window for Bc 
                 , 'TFBc2JpsiHPD_BcVtxCHI2'           :   25.   # adimentional
                 , 'TFBc2JpsiHPD_BcPT'                :    0.   # MeV, May incrase up to 5000 MeV if needed
                 , 'TFBc2JpsiHPD_JpsiMassWindow'      :  100.   # MeV, at least 3 sigma (30 MeV for Bc)
                 , 'TFBc2JpsiHPD_JpsiVtxCHI2'         :   25.   # adimentional
                 , 'TFBc2JpsiHPD_MuonJpsiPT'          :  800.   # MeV 
                 , 'TFBc2JpsiHPD_PionPT'              : 1200.   # MeV
                 , 'TFBc2JpsiHPD_PionIPCHI2'          :    4.   # adimentional
                 , 'TFBc2JpsiHPD_JpsiIPCHI2'          :   -1.   # adimentional, choose one  
                 , 'TFBc2JpsiHPD_MuonIPCHI2'          :   -1.   # adimentional, choose one 
                 , 'TFBc2JpsiHPD_BcIPCHI2'            :   25.   # adimentional

                 # TrackFitted Bs->Jpsi(ee)Phi
                 , 'TFBs2JpsieePhi_L0Req'               :  "L0_CHANNEL('Electron')"
                 , 'TFBs2JpsieePhi_Hlt1Req'             :  "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')" 

                 , 'TFBs2JpsieePhi_ElectronPT'          :  500.      # MeV
                 , 'TFBs2JpsieePhi_ElectronMIPCHI2'     :    2.25    # adimensional
                 , 'TFBs2JpsieePhi_JpsiVertexCHI2pDOF'  :   25.      # adimensional
                 , 'TFBs2JpsieePhi_JpsiMassMin'         : 2500.      # MeV
                 , 'TFBs2JpsieePhi_JpsiMassMax'         : 3300.      # MeV
                 , 'TFBs2JpsieePhi_KaonMIPCHI2'         :    2.25    # adimensional
                 , 'TFBs2JpsieePhi_PhiPT'               :  900.      # MeV
                 , 'TFBs2JpsieePhi_PhiVertexCHI2pDOF'   :   25.      # adimensional
                 , 'TFBs2JpsieePhi_PhiMassMin'          :  990.      # MeV
                 , 'TFBs2JpsieePhi_PhiMassMax'          : 1050.      # MeV
                 , 'TFBs2JpsieePhi_BsDIRA'              :    0.99    # adimensional
                 , 'TFBs2JpsieePhi_BsVertexCHI2pDOF'    :   25.      # adimensional
                 , 'TFBs2JpsieePhi_BsPseudoMassMin'     : 3800.      # MeV
                 , 'TFBs2JpsieePhi_BsPseudoMassMax'     : 6800.      # MeV                 
                 }
    
    def __apply_configuration__(self) :
	self.__makeHlt2Bs2JpsiPhiPrescaledAndDetachedLines()
        self.__makeHlt2Bs2JpsiPhiLines()
        self.__makeHlt2Bs2JpsieePhiLines()
        self.__makeHlt2TFBs2JpsieePhiLines()
        self.__makeHlt2Bd2JpsiKstarLines()
        self.__makeHlt2Bu2JpsiKLines()
    	self.__makeHlt2Bd2JpsiMuMuKsLLBiasedLines()
    	self.__makeHlt2Bd2JpsiMuMuKsLLUnbiasedLines()
    	self.__makeHlt2Bd2JpsiMuMuKsDDBiasedLines()
    	self.__makeHlt2Bd2JpsiMuMuKsDDUnbiasedLines()	
	self.__makeHlt2B2JpsiX_MuMuLines()
        self.__makeHlt2Bc2JpsiMuXLines()
        self.__makeHlt2Bc2JpsiHLines()
        self.__makeHlt2Bc2JpsiHPrescaledAndDetachedLines()
        self.__makeHlt2TFBc2JpsiMuXLines()
        self.__makeHlt2TFBc2JpsiHLines()
        self.__makeHlt2TFBc2JpsiHPrescaledAndDetachedLines()
        

    def __makeHlt2Bs2JpsiPhiPrescaledAndDetachedLines(self):
        '''
        Prescaled and detached selection for Bs -> J/psi Phi
        
        @author Greig Cowan
        @author Tristan du Pree
        @date 2009-05-19
        '''
    
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.DiMuon import Jpsi2MuMu
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons
	from HltTracking.HltPVs import PV3D
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiPrescaledDecision" : 50375 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiDetachedDecision" : 50376 } )

        # Make the phi
        phiCombine = Hlt2Member( CombineParticles
                                 , "PhiCombine"
                                 , DecayDescriptor = "phi(1020) -> K+ K-"
                                 , Inputs = [NoCutsKaons]
                                 , MotherCut = "(M<1100*MeV) & (PT>500*MeV) & (VFASPF(VCHI2PDOF)<25)"
                                 )
        
        # Make the Bs
        BsCuts = "(ADMASS('B_s0')<300*MeV) & (BPVLTFITCHI2()<36) & (VFASPF(VCHI2)<100)"
        BsCombine = Hlt2Member( CombineParticles
                                , "BsCombine"
                                , DecayDescriptor = "[B_s0 -> J/psi(1S) phi(1020)]cc"
                                , Inputs  = [Jpsi2MuMu, phiCombine]
                                , MotherCut = BsCuts
                                )
       
        line = Hlt2Line('Bs2JpsiPhiPrescaled'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [PV3D(), Jpsi2MuMu, NoCutsKaons, phiCombine, BsCombine]
                        )

        # Now do the detached 
        # Note: we should _share_ the J/psi and phi between these two lines!!
        line.clone('Bs2JpsiPhiDetached'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , algos = [PV3D(), Jpsi2MuMu, NoCutsKaons, phiCombine, BsCombine]
                   , BsCombine = {"MotherCut": BsCuts + "& (BPVLTIME()>%(BsLifetimeCut)s*ps)"%self.getProps() }
                   )


    def __makeHlt2Bs2JpsiPhiLines(self):
        '''
        Prescaled Bs -> J/psi Phi selection

        @author Geraldine Conti
        @author Adlene Hicheur
        @date 2009-07-19
        '''
    
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons
        from Hlt2SharedParticles.DiMuon import HighPtJpsi2MuMu
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiWideDecision" : 50370 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiSignalDecision" : 50371 } )
    
        # Make the phi
        PhiCombine = Hlt2Member( CombineParticles
                                 , "PhiCombine"
                                 , DecayDescriptor = "phi(1020) -> K+ K-"
                                 , Inputs = [NoCutsKaons]
                                 , CombinationCut = "(AM<1045*MeV) & (APT>1000*MeV)"
                                 , MotherCut = "(VFASPF(VCHI2PDOF)<25)"
                                 )

        # Make the Bs
        BsCombine = Hlt2Member( CombineParticles
                                , "BsCombine"
                                , DecayDescriptor = "[B_s0 -> J/psi(1S) phi(1020)]cc"
                                , Inputs  = [HighPtJpsi2MuMu, PhiCombine]
                                , CombinationCut = "(ADAMASS('B_s0')<300*MeV)"
                                , MotherCut = "(VFASPF(VCHI2PDOF)<6)"
                                )
        
        line = Hlt2Line('Bs2JpsiPhiWide'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [HighPtJpsi2MuMu, NoCutsKaons, PhiCombine, BsCombine]
                        )
        
        # Now do the Signal selection
        line.clone('Bs2JpsiPhiSignal'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , BsCombine = {"CombinationCut" :  "(ADAMASS('B_s0')<100*MeV)"}
                   )
        


    def __makeHlt2Bs2JpsieePhiLines(self):
        '''
        Prescaled Bs -> J/psi(ee) Phi selection
        
        @author Jibo HE
        @author Stephane TJampens
        @author Artur Ukleja
        @author Adlene Hicheur
        @date 2010-05-14
        '''
                         
        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.BasicParticles import Electrons
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons
        from HltTracking.HltPVs import PV3D
        from Configurables import CombineParticles, FilterDesktop
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsieePhiUnbiasedPTDecision" : self.getProp('HltANNSvcID')['Bs2JpsieePhiUnbiasedPT'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsieePhiSignalDecision" : self.getProp('HltANNSvcID')['Bs2JpsieePhiSignal'] } )


        """
        #------------------------
        # L0 & Hlt1 Requirements
        #------------------------ 
        """
        L0Req  = self.getProp("Bs2JpsieePhi_L0Req")
        Hlt1Req = self.getProp("Bs2JpsieePhi_Hlt1Req")
        
        if not L0Req:
            L0Req = None
            
        if not Hlt1Req:
            Hlt1Req= None
            
        
        # Electron Cuts
        ElCuts = "(PT > %(Bs2JpsieePhi_ElectronPT)s *MeV)" % self.getProps()

        # Jpsi Cuts
        JpsiCuts = "(VFASPF(VCHI2PDOF) < %(Bs2JpsieePhi_JpsiVertexCHI2pDOF)s ) & (in_range(%(Bs2JpsieePhi_JpsiMassMin)s *MeV, MM, %(Bs2JpsieePhi_JpsiMassMax)s *MeV))" % self.getProps()

        # Make the Jpsiee
        JpsiCombine = Hlt2Member( CombineParticles
                                  , "JpsiCombine"
                                  , DecayDescriptor = "J/psi(1S) -> e+ e-"
                                  , Inputs = [Electrons]
                                  , DaughtersCuts = {"e+" : ElCuts }
                                  , MotherCut = JpsiCuts
                                  , InputPrimaryVertices = "None"
                                  , UseP2PVRelations = False
                                  )
        
        # Phi Cuts
        PhiCuts = "(VFASPF(VCHI2PDOF) < %(Bs2JpsieePhi_PhiVertexCHI2pDOF)s ) & (PT > %(Bs2JpsieePhi_PhiPT)s *MeV) & ( in_range( %(Bs2JpsieePhi_PhiMassMin)s *MeV, MM, %(Bs2JpsieePhi_PhiMassMax)s *MeV) )" % self.getProps()
        
        # Make the phi
        PhiCombine = Hlt2Member( CombineParticles
                                 , "PhiCombine"
                                 , DecayDescriptor = "phi(1020) -> K+ K-"
                                 , Inputs = [NoCutsKaons]
                                 , MotherCut = PhiCuts
                                 , InputPrimaryVertices = "None"
                                 , UseP2PVRelations = False
                                 )
        
        # Bs Cuts
        BsCuts = "(VFASPF(VCHI2PDOF) < %(Bs2JpsieePhi_BsVertexCHI2pDOF)s ) & ( in_range( %(Bs2JpsieePhi_BsPseudoMassMin)s *MeV, M-M1+3096.916*MeV, %(Bs2JpsieePhi_BsPseudoMassMax)s *MeV) )"  % self.getProps()

        # Make the Bs
        BsCombine = Hlt2Member( CombineParticles
                                , "BsCombine"
                                , DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)"
                                , Inputs  = [JpsiCombine, PhiCombine]
                                , MotherCut = BsCuts
                                , InputPrimaryVertices = "None"
                                , UseP2PVRelations = False                                
                                )
        
        Bs2JpsieePhiUnbiased = bindMembers( "Bs2JpsieePhi", [ Electrons, JpsiCombine, NoCutsKaons, PhiCombine, BsCombine ] )
        
        
        # Final Wide Selection
        line = Hlt2Line('Bs2JpsieePhiUnbiasedPT'
                        , L0DU = L0Req
                        , HLT  = Hlt1Req
                        , prescale = self.prescale
                        , algos = [Electrons, JpsiCombine, NoCutsKaons, PhiCombine, BsCombine]
                        , postscale = self.postscale
                        )

        # Lifetime biased Electron and Kaon Cuts
        AddElectronCuts = "( MINTREE('e+'==ABSID,MIPCHI2DV(PRIMARY)) > %(Bs2JpsieePhi_ElectronMIPCHI2)s )" % self.getProps()
        AddKaonCuts     = "( MINTREE('K+'==ABSID,MIPCHI2DV(PRIMARY)) > %(Bs2JpsieePhi_KaonMIPCHI2)s )" % self.getProps()
        AddBsCuts       = "( BPVDIRA > %(Bs2JpsieePhi_BsDIRA)s )" % self.getProps()
        
        # Now do the Signal selection, lifetime biased
        FilterBs2JpsieePhi  = Hlt2Member( FilterDesktop # type
                                          , "FilterBs2JpsieePhi" 
                                          , Code = AddElectronCuts + " & " + AddKaonCuts + " & " + AddBsCuts
                                          , Inputs = [ Bs2JpsieePhiUnbiased ]
                                          )

        line.clone('Bs2JpsieePhiSignal'
                   , prescale = self.prescale
                   , L0DU = L0Req
                   , HLT  = Hlt1Req
                   , algos = [ PV3D(), Bs2JpsieePhiUnbiased, FilterBs2JpsieePhi ]
                   , postscale = self.postscale
                   )    




    def __makeHlt2TFBs2JpsieePhiLines(self):
        '''
        Prescaled TrackFitted Bs -> J/psi(ee) Phi selection
        
        @author Jibo HE
        @author Stephane TJampens
        @author Artur Ukleja
        @author Adlene Hicheur
        @date 2010-05-14
        '''
                         
        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedElectrons
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons
        from HltTracking.HltPVs import PV3D
        from Configurables import CombineParticles, FilterDesktop
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TFBs2JpsieePhiUnbiasedPTDecision" : self.getProp('HltANNSvcID')['TFBs2JpsieePhiUnbiasedPT'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TFBs2JpsieePhiSignalDecision" : self.getProp('HltANNSvcID')['TFBs2JpsieePhiSignal'] } )

        """
        #------------------------
        # L0 & Hlt1 Requirements
        #------------------------ 
        """
        L0Req   = self.getProp("TFBs2JpsieePhi_L0Req")
        Hlt1Req = self.getProp("TFBs2JpsieePhi_Hlt1Req")
        
        if not L0Req: L0Req = None
        if not Hlt1Req: Hlt1Req= None

        # Electron Cuts
        ElCuts = "(PT > %(TFBs2JpsieePhi_ElectronPT)s *MeV)" % self.getProps()

        # Jpsi Cuts
        JpsiCuts = "(VFASPF(VCHI2PDOF) < %(TFBs2JpsieePhi_JpsiVertexCHI2pDOF)s ) & (in_range(%(TFBs2JpsieePhi_JpsiMassMin)s *MeV, MM, %(TFBs2JpsieePhi_JpsiMassMax)s *MeV))" % self.getProps()

        # Make the Jpsiee
        JpsiCombine = Hlt2Member( CombineParticles
                                  , "JpsiCombine"
                                  , DecayDescriptor = "J/psi(1S) -> e+ e-"
                                  , Inputs = [BiKalmanFittedElectrons]
                                  , DaughtersCuts = {"e+" : ElCuts }
                                  , MotherCut = JpsiCuts
                                  , InputPrimaryVertices = "None"
                                  , UseP2PVRelations = False
                                  )

        # Phi Cuts
        PhiCuts = "(VFASPF(VCHI2PDOF) < %(TFBs2JpsieePhi_PhiVertexCHI2pDOF)s ) & (PT > %(TFBs2JpsieePhi_PhiPT)s *MeV) & (in_range(%(TFBs2JpsieePhi_PhiMassMin)s *MeV, MM, %(TFBs2JpsieePhi_PhiMassMax)s *MeV))" % self.getProps()

        # Make the phi
        PhiCombine = Hlt2Member( CombineParticles
                                 , "PhiCombine"
                                 , DecayDescriptor = "phi(1020) -> K+ K-"
                                 , Inputs = [BiKalmanFittedKaons]
                                 , MotherCut = PhiCuts
                                 , InputPrimaryVertices = "None"
                                 , UseP2PVRelations = False
                                 )
        # Bs Cuts
        BsCuts = "(VFASPF(VCHI2PDOF) < %(TFBs2JpsieePhi_BsVertexCHI2pDOF)s ) & (in_range(%(TFBs2JpsieePhi_BsPseudoMassMin)s *MeV, M-M1+3096.916*MeV, %(TFBs2JpsieePhi_BsPseudoMassMax)s *MeV))"  % self.getProps()
        
        # Make the Bs
        BsCombine = Hlt2Member( CombineParticles
                                , "BsCombine"
                                , DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)"
                                , Inputs  = [JpsiCombine, PhiCombine]
                                , MotherCut = BsCuts
                                , InputPrimaryVertices = "None"
                                , UseP2PVRelations = False
                                )

        TFBs2JpsieePhiUnbiased = bindMembers( "TFBs2JpsieePhi", [BiKalmanFittedElectrons, JpsiCombine, BiKalmanFittedKaons, PhiCombine, BsCombine] ) 
        
        # Final Wide Selection
        line = Hlt2Line('TFBs2JpsieePhiUnbiasedPT'
                        , prescale = self.prescale
                        , L0DU = L0Req
                        , HLT  = Hlt1Req
                        , algos = [BiKalmanFittedElectrons, JpsiCombine, BiKalmanFittedKaons, PhiCombine, BsCombine]
                        , postscale = self.postscale
                        )
        
        
        # Lifetime biased Electron and Kaon Cuts
        AddElectronCuts = "( MINTREE('e+'==ABSID,MIPCHI2DV(PRIMARY)) > %(TFBs2JpsieePhi_ElectronMIPCHI2)s )" % self.getProps()
        AddKaonCuts     = "( MINTREE('K+'==ABSID,MIPCHI2DV(PRIMARY)) > %(TFBs2JpsieePhi_KaonMIPCHI2)s )" % self.getProps()
        AddBsCuts       = "( BPVDIRA > %(TFBs2JpsieePhi_BsDIRA)s )" % self.getProps()        
        
        # Now do the Signal selection, lifetime biased
        FilterTFBs2JpsieePhi  = Hlt2Member( FilterDesktop # type
                                            , "FilterTFBs2JpsieePhi" 
                                            , Code = AddElectronCuts + " & " + AddKaonCuts + " & " + AddBsCuts
                                            , Inputs = [ TFBs2JpsieePhiUnbiased ]
                                            )
        
        line.clone('TFBs2JpsieePhiSignal'
                   , prescale = self.prescale
                   , L0DU = L0Req
                   , HLT  = Hlt1Req
                   , algos = [ PV3D(), TFBs2JpsieePhiUnbiased, FilterTFBs2JpsieePhi ]
                   , postscale = self.postscale
                   )    





    def __makeHlt2Bd2JpsiKstarLines(self):
        '''
        Prescaled Bd -> J/psi K* selection 
        
        @author Geraldine Conti
        @author Adlene Hicheur
        @date 2009-07-19
        '''
        
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons, NoCutsPions
        from Hlt2SharedParticles.DiMuon import HighPtJpsi2MuMu
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiKstarWideDecision" : 50360 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiKstarSignalDecision" : 50361 } )
        
        # Make the Kstar
        KstarCombine = Hlt2Member( CombineParticles
                                   , "KstarCombine"
                                   , DecayDescriptor = "K*(892)0 -> K+ pi-"
                                   , Inputs = [NoCutsKaons, NoCutsPions]
                                   , CombinationCut = "(ADAMASS('K*(892)0')<120*MeV) & (APT>1000*MeV)"
                                   , MotherCut = "(VFASPF(VCHI2PDOF)<25)"
                                   )
        
        # Make the Bd
        BdCombine = Hlt2Member( CombineParticles
                                , "BdCombine"
                                , DecayDescriptor = "B0 -> J/psi(1S) K*(892)0"
                                , Inputs  = [HighPtJpsi2MuMu, KstarCombine]
                                , CombinationCut = "(ADAMASS('B0')<300*MeV)"
                                , MotherCut = "(VFASPF(VCHI2PDOF)<6) & (PT>2000*MeV)"
                                )
        
        line = Hlt2Line('Bd2JpsiKstarWide'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [HighPtJpsi2MuMu, NoCutsKaons, NoCutsPions, KstarCombine, BdCombine]
                        )
        
        # Now do the Signal selection
        line.clone('Bd2JpsiKstarSignal'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , BdCombine = {"CombinationCut" :  "(ADAMASS('B0')<100*MeV)"}
                   )
        
    def __makeHlt2Bu2JpsiKLines(self):
        '''
        Prescaled Bu -> J/psi K selection 
        
        @author Geraldine Conti
        @author Adlene Hicheur
        @date 2009-07-19
        '''
        
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons
        from Hlt2SharedParticles.DiMuon import HighPtJpsi2MuMu
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2JpsiKWideDecision" : 50380 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2JpsiKSignalDecision" : 50381 } )
        
        # Make the Bu
        BuCombine = Hlt2Member( CombineParticles
                                , "BuCombine"
                                , DecayDescriptor = "[B+ -> J/psi(1S) K+]cc"
                                , Inputs  = [HighPtJpsi2MuMu, NoCutsKaons]
                                , CombinationCut = "(ADAMASS('B+')<300*MeV)"
                                , MotherCut = "(VFASPF(VCHI2PDOF)<6)"
                                , DaughtersCuts = {"K+" : "(PT>1300*MeV)"}
                                )
        
        line = Hlt2Line('Bu2JpsiKWide'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [HighPtJpsi2MuMu, NoCutsKaons, BuCombine]
                        )
    
        # Now do the Signal selection
        line.clone('Bu2JpsiKSignal'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , BuCombine = {"CombinationCut" :  "(ADAMASS('B+')<100*MeV)"}
                   )

    def __makeHlt2Bd2JpsiMuMuKsLLBiasedLines(self):
        '''
	@author S Amato
	@author C Gobel
	@author F Rodrigues
	'''
	from HltLine.HltLine import Hlt2Line, Hlt2Member
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.DiMuon import Jpsi2MuMu
        from Hlt2SharedParticles.Ks import KsLLTF
        from Configurables import CombineParticles, FilterDesktop
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiMuMuKsLLBiasedDecision" : 50320 } )
        
        filterJpsi = Hlt2Member( FilterDesktop # type
                                 , "FilterJpsi"      # name -- to be bound to the line name: Hlt2LineNameFilter
                                 #,Code = "(MM>3037*MeV) & (MM<3157*MeV) & (MINTREE('mu-'==ABSID,PT)>0.5*GeV) & (BPVVDCHI2 > 4) "
                                 , Code = "(MM>3037*MeV) & (MM<3157*MeV) & (BPVVDCHI2>7)  "
                                 , Inputs = [ Jpsi2MuMu ]
                                 )

        filterKS = Hlt2Member( FilterDesktop # type
                               , "FilterKS"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code = "VFASPF(VCHI2PDOF)<16 "
                               , Inputs = [ KsLLTF ]
                               )

        combineB = Hlt2Member( CombineParticles # type
                               , "CombineB"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , DecayDescriptor = "B0 -> J/psi(1S) KS0"
                               , CombinationCut = "(ADAMASS('B0')<300*MeV)"
                               , MotherCut = "(VFASPF(VCHI2PDOF)<55) & (BPVIPCHI2()< 50) "
                               , Inputs = [filterJpsi,filterKS ]
                               )

        line = Hlt2Line('Bd2JpsiMuMuKsLLBiased'
		        , prescale = self.prescale
                   	, postscale = self.postscale	
                        , algos = [  PV3D(),  Jpsi2MuMu, filterJpsi
                                       , KsLLTF,      filterKS
                                       , combineB
                                       ]
                        )


    def __makeHlt2Bd2JpsiMuMuKsLLUnbiasedLines(self):
        '''
        @author S Amato
        @author C Gobel
        @author F Rodrigues
        '''
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.DiMuon import Jpsi2MuMu
        from Hlt2SharedParticles.Ks import KsLLTF
        from Configurables import CombineParticles, FilterDesktop
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiMuMuKsLLUnbiasedDecision" : 50340 } )
        
        filterJpsi = Hlt2Member( FilterDesktop # type
                                 , "FilterJpsi"      # name -- to be bound to the line name: Hlt2LineNameFilter
                                 , Code = "(MM>3037*MeV) & (MM<3157*MeV) & (MINTREE('mu-'==ABSID,PT)>0.8*GeV)"
                                 , Inputs = [ Jpsi2MuMu ]
                                 )
        
        filterKS = Hlt2Member( FilterDesktop # type
                               , "FilterKS"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code = "(PT > 1.0*GeV)"
                               , Inputs = [ KsLLTF ]
                               )
        
        combineB = Hlt2Member( CombineParticles # type
                               , "CombineB"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , DecayDescriptor = "B0 -> J/psi(1S) KS0"
                               , CombinationCut = "(ADAMASS('B0')<300*MeV)"
                               , MotherCut = "(PT>0.3*GeV) & (VFASPF(VCHI2PDOF)<25)"
                               , Inputs = [filterJpsi,filterKS ]
                               )
        
        line = Hlt2Line('Bd2JpsiMuMuKsLLUnbiased'
                        , prescale = self.prescale
                   	, postscale = self.postscale
			,  algos = [   Jpsi2MuMu , filterJpsi
                                       , KsLLTF,       filterKS
                                       , combineB
                                       ]
                        )

    def __makeHlt2Bd2JpsiMuMuKsDDBiasedLines(self):
        '''
        @author S Amato
        @author C Gobel
        @author F Rodrigues
        '''
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.DiMuon import Jpsi2MuMu
        from Hlt2SharedParticles.Ks import KsDD
        from Configurables import GaudiSequencer,  FilterDesktop
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiMuMuKsDDBiasedDecision" : 50330 } )

        filterJpsi = Hlt2Member( FilterDesktop # type
                                 , "FilterJpsi"      # name -- to be bound to the line name: Hlt2LineNameFilter
                                 , Code = "(MM>3037*MeV) & (MM<3157*MeV) & (BPVVDCHI2>7)  "
                                 , Inputs = [ Jpsi2MuMu ]
                                 )

        combineB = Hlt2Member( CombineParticles # type
                               , "CombineB"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , DecayDescriptor = "B0 -> J/psi(1S) KS0"
                               , CombinationCut = "(ADAMASS('B0')<300*MeV)"
                               , MotherCut = "(VFASPF(VCHI2PDOF)<25) & (BPVIPCHI2()< 45)"
                               , Inputs = [filterJpsi,KsDD ]
                               )
        line = Hlt2Line('Bd2JpsiMuMuKsDDBiased'
                        , prescale = self.prescale
                   	, postscale = self.postscale
                        ,  algos = [   PV3D(), Jpsi2MuMu , filterJpsi
                                       , KsDD
                                       , combineB
                                       ]
                        )

    def __makeHlt2Bd2JpsiMuMuKsDDUnbiasedLines(self):
        '''
        @author S Amato
        @author C Gobel
        @author F Rodrigues
        '''
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.DiMuon import Jpsi2MuMu
        from Hlt2SharedParticles.Ks import KsDD
        from Configurables import GaudiSequencer, FilterDesktop
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiMuMuKsDDUnbiasedDecision" : 50350 } )

        filterJpsi = Hlt2Member( FilterDesktop # type
                                 , "FilterJpsi"      # name -- to be bound to the line name: Hlt2LineNameFilter
                                 , Code = "(MM>3037*MeV) & (MM<3157*MeV) & (MINTREE('mu-'==ABSID,PT)>0.8*GeV)"
                                 , Inputs = [ Jpsi2MuMu ]
                                 )

        filterKS = Hlt2Member( FilterDesktop # type
                               , "FilterKS"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code = "(PT > 0.8*GeV)"
                               , Inputs = [KsDD ]
                               )

        combineB = Hlt2Member( CombineParticles # type
                               , "CombineB"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , DecayDescriptor = "B0 -> J/psi(1S) KS0"
                               , CombinationCut = "(ADAMASS('B0')<300*MeV)"
                               , MotherCut = "(PT>0.3*GeV) & (VFASPF(VCHI2PDOF)<20) & (BPVIPCHI2()< 30)"
                               , Inputs = [filterJpsi,filterKS ]
                               )

        line = Hlt2Line('Bd2JpsiMuMuKsDDUnbiased'
                   	, prescale = self.prescale
                   	, postscale = self.postscale
                        ,  algos = [   PV3D(), Jpsi2MuMu, filterJpsi
                                       , KsDD, filterKS
                                       , combineB
                                       ]
                        )

    def __makeHlt2B2JpsiX_MuMuLines(self):
        '''
        @author P Koppenburg
        '''
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.DiMuon import DiMuon
        from Configurables import FilterDesktop
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2JpsiX_MuMuDecision" : 50050 } )

        filter = Hlt2Member( FilterDesktop # type
                             , "Filter"      # name -- to be bound to the line name: Hlt2LineNameFilter
                             , Code = "(ADMASS('J/psi(1S)')<50*MeV) & (MINTREE('mu-'==ABSID,PT)>1.5*GeV)"
                             , Inputs = [ DiMuon ]
                             )
        # @todo Add some IP cuts                   

        Hlt2Line('B2JpsiX_MuMu' 
		   	, prescale = self.prescale
                   	, postscale = self.postscale
		   	, algos = [ DiMuon , filter ]
			)

        

    def __makeHlt2Bc2JpsiMuXLines(self):
        '''
        Bc->Jpsi(MuMu)MuX
        @author Jibo.He@cern.ch
        '''

        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop, CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bc2JpsiMuXDecision"       : self.getProp('HltANNSvcID')['Bc2JpsiMuX'] } )  # Full box, may be prescaled
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bc2JpsiMuXSignalDecision" : self.getProp('HltANNSvcID')['Bc2JpsiMuXSignal'] } )  # Signal box

        #Jpsi Cut
        JpsiCut = "(MINTREE('mu+'==ABSID,PT) > %(Bc2JpsiMuX_MuonJpsiPT)s *MeV) & (ADMASS('J/psi(1S)') < %(Bc2JpsiMuX_JpsiMassWindow)s *MeV) & (VFASPF(VCHI2PDOF) < %(Bc2JpsiMuX_JpsiVtxCHI2)s)" % self.getProps()

        # MuBc Cut
        MuonBcCut = "(PT > %(Bc2JpsiMuX_MuonBcPT)s *MeV)" % self.getProps()

        # Comb cut
        combCut = "(%(Bc2JpsiMuX_BcLowerMass)s *MeV < AM) & (AM < %(Bc2JpsiMuX_BcUpperMass)s *MeV)" % self.getProps()

        # Bc Cut
        BcCut = "(VFASPF(VCHI2PDOF)< %(Bc2JpsiMuX_BcVtxCHI2)s ) & (PT > %(Bc2JpsiMuX_BcPT)s *MeV)" % self.getProps()

        #Jpsi Tight Cut for signal
        JpsiTightCut = "(INTREE((ID=='J/psi(1S)') & (ADMASS('J/psi(1S)') < %(Bc2JpsiMuX_JpsiTightMassWindow)s *MeV)))" % self.getProps()

        #---------------------------
        # J/psi -> Mu Mu
        #---------------------------
        from Hlt2SharedParticles.DiMuon import DiMuon

        FilterJpsi4Bc2JpsiMuX = Hlt2Member( FilterDesktop # type
                                            , "FilterJpsi4Bc2JpsiMuX" 
                                            , Code = JpsiCut
                                            , Inputs = [ DiMuon ]
                                            , InputPrimaryVertices = "None"
                                            , UseP2PVRelations = False
                                            )

        #---------------------------
        # Bc -> J/psi(MuMu) Mu X
        #---------------------------
        from Hlt2SharedParticles.BasicParticles import Muons

        CombineBc2JpsiMuX = Hlt2Member( CombineParticles
                                        , "CombineBc2JpsiMuX"
                                        , DecayDescriptor = "[ B_c+ -> J/psi(1S) mu+ ]cc"
                                        , DaughtersCuts = { "mu+" : MuonBcCut }
                                        , CombinationCut = combCut
                                        , MotherCut = BcCut
                                        , Inputs = [ FilterJpsi4Bc2JpsiMuX, Muons ]
                                        , InputPrimaryVertices = "None"
                                        , UseP2PVRelations = False
                                        )
        
        Bc2JpsiMuXAll = bindMembers( "Bc2JpsiMuX", [ DiMuon, FilterJpsi4Bc2JpsiMuX, Muons, CombineBc2JpsiMuX ] )

        line_A = Hlt2Line('Bc2JpsiMuX'
                          , prescale = self.prescale
                          , algos = [ Bc2JpsiMuXAll ]
                          , postscale = self.postscale
                          )
        
        #---------------------------
        # Bc2JpsiMuXSignal 
        #---------------------------
        FilterBc2JpsiMuXSignal = Hlt2Member( FilterDesktop 
                                             , "FilterBc2JpsiMuXSignal" 
                                             , Code = JpsiTightCut
                                             , Inputs = [ Bc2JpsiMuXAll ]
                                             , InputPrimaryVertices = "None"
                                             , UseP2PVRelations = False
                                             )

        
        line_S = Hlt2Line('Bc2JpsiMuXSignal'
                          , prescale = self.prescale
                          , algos = [ Bc2JpsiMuXAll
                                      , FilterBc2JpsiMuXSignal ]
                          , postscale = self.postscale
                          ) 

        
    def __makeHlt2Bc2JpsiHLines(self):
        '''
        Bc->Jpsi(MuMu)H
        @author Jibo.He@cern.ch
        '''

        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop, CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bc2JpsiHDecision" : self.getProp('HltANNSvcID')['Bc2JpsiH'] } )   # High Pt

        # Jpsi Cut
        # Which is faster, INTREE or CHILDCUT?
        JpsiCut = "(MINTREE('mu+'==ABSID,PT) > %(Bc2JpsiH_MuonJpsiPT)s *MeV) & (ADMASS('J/psi(1S)') < %(Bc2JpsiH_JpsiMassWindow)s *MeV) & (VFASPF(VCHI2PDOF) < %(Bc2JpsiH_JpsiVtxCHI2)s)" % self.getProps()
                
        # MuBc Cut
        PionCut = "(PT > %(Bc2JpsiH_PionPT)s *MeV)" % self.getProps()
        
        # Comb cut
        combCut = "(ADAMASS('B_c+') < %(Bc2JpsiH_BcMassWindow)s *MeV)" % self.getProps()
        
        # Bc Cut
        BcCut = "(VFASPF(VCHI2PDOF)< %(Bc2JpsiH_BcVtxCHI2)s ) & (PT > %(Bc2JpsiH_BcPT)s *MeV)" % self.getProps()


        #---------------------------
        # J/psi -> Mu Mu
        #---------------------------
        from Hlt2SharedParticles.DiMuon import DiMuon

        FilterJpsi4Bc2JpsiH = Hlt2Member( FilterDesktop # type
                                          , "FilterJpsi4Bc2JpsiH"      # name -- to be bound to the line name: Hlt2LineNameFilter
                                          , Code = JpsiCut
                                          , Inputs = [ DiMuon ]
                                          , InputPrimaryVertices = "None"
                                          , UseP2PVRelations = False
                                          )
        
        #---------------------------
        # Bc -> J/psi(MuMu) H
        #---------------------------
        from Hlt2SharedParticles.BasicParticles import NoCutsPions
        
        CombineBc2JpsiH = Hlt2Member( CombineParticles
                                      , "CombineBc2JpsiH"
                                      , DecayDescriptor = "[ B_c+ -> J/psi(1S) pi+ ]cc"
                                      , DaughtersCuts = { "pi+" : PionCut }
                                      , CombinationCut = combCut
                                      , MotherCut = BcCut
                                      , Inputs = [ FilterJpsi4Bc2JpsiH, NoCutsPions ]
                                      , InputPrimaryVertices = "None"
                                      , UseP2PVRelations = False
                                      )
        
        line = Hlt2Line('Bc2JpsiH'
                        , prescale = self.prescale
                        , algos = [ DiMuon
                                    , FilterJpsi4Bc2JpsiH
                                    , NoCutsPions
                                    , CombineBc2JpsiH ]
                        , postscale = self.postscale
                        )

        
    def __makeHlt2Bc2JpsiHPrescaledAndDetachedLines(self):
        '''
        Bc->Jpsi(MuMu)H Prescaled and Detached
        @author Jibo.He@cern.ch
        '''

        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop, CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bc2JpsiHPrescaledDecision" : self.getProp('HltANNSvcID')['Bc2JpsiHPrescaled'] } ) 
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bc2JpsiHDetachedDecision"  : self.getProp('HltANNSvcID')['Bc2JpsiHDetached'] } ) 

        # Loose cuts
        #---------------------------
        #Jpsi Cut
        JpsiPDCut = "(MINTREE('mu+'==ABSID,PT) > %(Bc2JpsiHPD_MuonJpsiPT)s *MeV) & (ADMASS('J/psi(1S)') < %(Bc2JpsiHPD_JpsiMassWindow)s *MeV) & (VFASPF(VCHI2PDOF) < %(Bc2JpsiHPD_JpsiVtxCHI2)s)" % self.getProps()
        
        
        # Pion Bc Cut
        PionPDCut = "(PT > %(Bc2JpsiHPD_PionPT)s *MeV)" % self.getProps()
        
        # Comb cut
        combPDCut = "(ADAMASS('B_c+') < %(Bc2JpsiHPD_BcMassWindow)s *MeV)" % self.getProps()
        
        # Bc Cut
        BcPDCut = "(VFASPF(VCHI2PDOF)< %(Bc2JpsiHPD_BcVtxCHI2)s ) & (PT > %(Bc2JpsiHPD_BcPT)s *MeV)" % self.getProps()

        # Lifetime biased cut
        LifetimeBiasedCut = "(MINTREE('pi+'==ABSID,BPVIPCHI2()) > %(Bc2JpsiHPD_PionIPCHI2)s) & (MINTREE('mu+'==ABSID,BPVIPCHI2()) > %(Bc2JpsiHPD_MuonIPCHI2)s) & (MINTREE('J/psi(1S)'==ABSID,BPVIPCHI2()) > %(Bc2JpsiHPD_JpsiIPCHI2)s) & (BPVIPCHI2() < %(Bc2JpsiHPD_BcIPCHI2)s)" % self.getProps()
        

        #---------------------------
        # J/psi -> Mu Mu
        #---------------------------
        from Hlt2SharedParticles.DiMuon import DiMuon

        FilterJpsi4Bc2JpsiHPD = Hlt2Member( FilterDesktop # type
                                            , "FilterJpsi4Bc2JpsiHPD" 
                                            , Code = JpsiPDCut
                                            , Inputs = [ DiMuon ]
                                            , InputPrimaryVertices = "None"
                                            , UseP2PVRelations = False
                                            )
        
        #---------------------------
        # Bc -> J/psi(MuMu) H
        #---------------------------
        from Hlt2SharedParticles.BasicParticles import NoCutsPions
        
        CombineBc2JpsiHPD = Hlt2Member( CombineParticles
                                        , "CombineBc2JpsiHPD"
                                        , DecayDescriptor = "[ B_c+ -> J/psi(1S) pi+ ]cc"
                                        , DaughtersCuts = { "pi+" : PionPDCut }
                                        , CombinationCut = combPDCut
                                        , MotherCut = BcPDCut
                                        , Inputs = [ FilterJpsi4Bc2JpsiHPD, NoCutsPions ]
                                        , InputPrimaryVertices = "None"
                                        , UseP2PVRelations = False
                                        )

        Bc2JpsiHPDLoose = bindMembers( "Bc2JpsiHPrescaledAndDetached", [ DiMuon, FilterJpsi4Bc2JpsiHPD, NoCutsPions, CombineBc2JpsiHPD ] )
        
        line_Prescaled = Hlt2Line('Bc2JpsiHPrescaled'
                                  , prescale = self.prescale
                                  , algos = [ DiMuon
                                              , FilterJpsi4Bc2JpsiHPD
                                              , NoCutsPions
                                              , CombineBc2JpsiHPD ]
                                  , postscale = self.postscale
                                  )


        #---------------------------
        # Bc2JpsiHDetached
        #---------------------------
        from HltTracking.HltPVs import PV3D
        
        FilterBc2JpsiHPD = Hlt2Member( FilterDesktop # type
                                       , "FilterBc2JpsiHPD" 
                                       , Code = LifetimeBiasedCut
                                       , Inputs = [ Bc2JpsiHPDLoose ]
                                       )

        
        line_Detached = Hlt2Line('Bc2JpsiHDetached'
                                 , prescale = self.prescale
                                 , algos = [ PV3D()
                                           , Bc2JpsiHPDLoose
                                           , FilterBc2JpsiHPD ]
                                 , postscale = self.postscale
                                 )        




    def __makeHlt2TFBc2JpsiMuXLines(self):
        '''
        Track Fitted Bc->Jpsi(MuMu)MuX
        @author Jibo.He@cern.ch
        '''

        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop, CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TFBc2JpsiMuXDecision"       : self.getProp('HltANNSvcID')['TFBc2JpsiMuX'] } )  # Full box, may be prescaled
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TFBc2JpsiMuXSignalDecision" : self.getProp('HltANNSvcID')['TFBc2JpsiMuXSignal'] } )  # Signal box

        #Jpsi Cut
        JpsiCut = "(MINTREE('mu+'==ABSID,PT) > %(TFBc2JpsiMuX_MuonJpsiPT)s *MeV) & (ADMASS('J/psi(1S)') < %(TFBc2JpsiMuX_JpsiMassWindow)s *MeV) & (VFASPF(VCHI2PDOF) < %(TFBc2JpsiMuX_JpsiVtxCHI2)s)" % self.getProps()

        # MuBc Cut
        MuonBcCut = "(PT > %(TFBc2JpsiMuX_MuonBcPT)s *MeV)" % self.getProps()
        
        # Comb cut
        combCut = "(%(TFBc2JpsiMuX_BcLowerMass)s *MeV < AM) & (AM < %(TFBc2JpsiMuX_BcUpperMass)s *MeV)" % self.getProps()

        # Bc Cut
        BcCut = "(VFASPF(VCHI2PDOF)< %(TFBc2JpsiMuX_BcVtxCHI2)s ) & (PT > %(TFBc2JpsiMuX_BcPT)s *MeV)" % self.getProps()

        #Jpsi Tight Cut for signal
        JpsiTightCut = "(INTREE((ID=='J/psi(1S)') & (ADMASS('J/psi(1S)') < %(TFBc2JpsiMuX_JpsiTightMassWindow)s *MeV)))" % self.getProps()

        #---------------------------
        # J/psi -> Mu Mu
        #---------------------------
        from Hlt2SharedParticles.TrackFittedDiMuon import TrackFittedDiMuon
        
        FilterJpsi4TFBc2JpsiMuX = Hlt2Member( FilterDesktop # type
                                              , "FilterJpsi4TFBc2JpsiMuX" 
                                              , Code = JpsiCut
                                              , Inputs = [ TrackFittedDiMuon ]
                                              , InputPrimaryVertices = "None"
                                              , UseP2PVRelations = False                                             
                                              )
        
        #---------------------------
        # Bc -> J/psi(MuMu) Mu X
        #---------------------------
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons

        CombineTFBc2JpsiMuX = Hlt2Member( CombineParticles
                                          , "CombineTFBc2JpsiMuX"
                                          , DecayDescriptor = "[ B_c+ -> J/psi(1S) mu+ ]cc"
                                          , DaughtersCuts = { "mu+" : MuonBcCut }
                                          , CombinationCut = combCut
                                          , MotherCut = BcCut
                                          , Inputs = [ FilterJpsi4TFBc2JpsiMuX, BiKalmanFittedMuons ]
                                          , InputPrimaryVertices = "None"
                                          , UseP2PVRelations = False
                                          )
        
        TFBc2JpsiMuXAll = bindMembers( "TFBc2JpsiMuX", [ TrackFittedDiMuon, FilterJpsi4TFBc2JpsiMuX, BiKalmanFittedMuons, CombineTFBc2JpsiMuX ] )

        line_A = Hlt2Line('TFBc2JpsiMuX'
                          , prescale = self.prescale
                          , algos = [ TFBc2JpsiMuXAll ]
                          , postscale = self.postscale
                          )
        
        #---------------------------
        # TFBc2JpsiMuXSignal 
        #---------------------------
        FilterTFBc2JpsiMuXSignal = Hlt2Member( FilterDesktop 
                                               , "FilterTFBc2JpsiMuXSignal" 
                                               , Code = JpsiTightCut
                                               , Inputs = [ TFBc2JpsiMuXAll ]
                                               , InputPrimaryVertices = "None"
                                               , UseP2PVRelations = False
                                               )
        
        
        line_S = Hlt2Line('TFBc2JpsiMuXSignal'
                          , prescale = self.prescale
                          , algos = [ TFBc2JpsiMuXAll
                                      , FilterTFBc2JpsiMuXSignal ]
                          , postscale = self.postscale
                          ) 

        
    def __makeHlt2TFBc2JpsiHLines(self):
        '''
        Track Fitted Bc->Jpsi(MuMu)H
        @author Jibo.He@cern.ch
        '''

        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop, CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TFBc2JpsiHDecision"  : self.getProp('HltANNSvcID')['TFBc2JpsiH'] } )   # High Pt

        # Jpsi Cut
        # Which is faster, INTREE or CHILDCUT?
        JpsiCut = "(MINTREE('mu+'==ABSID,PT) > %(TFBc2JpsiH_MuonJpsiPT)s *MeV) & (ADMASS('J/psi(1S)') < %(TFBc2JpsiH_JpsiMassWindow)s *MeV) & (VFASPF(VCHI2PDOF) < %(TFBc2JpsiH_JpsiVtxCHI2)s)" % self.getProps()
                
        # MuBc Cut
        PionCut = "(PT > %(TFBc2JpsiH_PionPT)s *MeV)" % self.getProps()
        
        # Comb cut
        combCut = "(ADAMASS('B_c+') < %(TFBc2JpsiH_BcMassWindow)s *MeV)" % self.getProps()
        
        # Bc Cut
        BcCut = "(VFASPF(VCHI2PDOF)< %(TFBc2JpsiH_BcVtxCHI2)s ) & (PT > %(TFBc2JpsiH_BcPT)s *MeV)" % self.getProps()


        #---------------------------
        # J/psi -> Mu Mu
        #---------------------------
        from Hlt2SharedParticles.TrackFittedDiMuon import TrackFittedDiMuon
        
        FilterJpsi4TFBc2JpsiH = Hlt2Member( FilterDesktop # type
                                            , "FilterJpsi4TFBc2JpsiH" 
                                            , Code = JpsiCut
                                            , Inputs = [ TrackFittedDiMuon ]
                                            , InputPrimaryVertices = "None"
                                            , UseP2PVRelations = False
                                            )
        
        #---------------------------
        # Bc -> J/psi(MuMu) H
        #---------------------------
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions
        
        CombineTFBc2JpsiH = Hlt2Member( CombineParticles
                                        , "CombineTFBc2JpsiH"
                                        , DecayDescriptor = "[ B_c+ -> J/psi(1S) pi+ ]cc"
                                        , DaughtersCuts = { "pi+" : PionCut }
                                        , CombinationCut = combCut
                                        , MotherCut = BcCut
                                        , Inputs = [ FilterJpsi4TFBc2JpsiH, BiKalmanFittedPions ]
                                        , InputPrimaryVertices = "None"
                                        , UseP2PVRelations = False
                                        )
        
        line = Hlt2Line('TFBc2JpsiH'
                        , prescale = self.prescale
                        , algos = [ TrackFittedDiMuon
                                    , FilterJpsi4TFBc2JpsiH
                                    , BiKalmanFittedPions
                                    , CombineTFBc2JpsiH ]
                        , postscale = self.postscale
                        )

        
    def __makeHlt2TFBc2JpsiHPrescaledAndDetachedLines(self):
        '''
        Track Fitted Bc->Jpsi(MuMu)H Prescaled and Detached
        @author Jibo.He@cern.ch
        '''

        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop, CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TFBc2JpsiHPrescaledDecision" : self.getProp('HltANNSvcID')['TFBc2JpsiHPrescaled'] } ) 
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TFBc2JpsiHDetachedDecision"  : self.getProp('HltANNSvcID')['TFBc2JpsiHDetached'] } ) 

        # Loose cuts
        #---------------------------
        #Jpsi Cut
        JpsiPDCut = "(MINTREE('mu+'==ABSID,PT) > %(TFBc2JpsiHPD_MuonJpsiPT)s *MeV) & (ADMASS('J/psi(1S)') < %(TFBc2JpsiHPD_JpsiMassWindow)s *MeV) & (VFASPF(VCHI2PDOF) < %(TFBc2JpsiHPD_JpsiVtxCHI2)s)" % self.getProps()
        
        
        # Pion Bc Cut
        PionPDCut = "(PT > %(TFBc2JpsiHPD_PionPT)s *MeV)" % self.getProps()
        
        # Comb cut
        combPDCut = "(ADAMASS('B_c+') < %(TFBc2JpsiHPD_BcMassWindow)s *MeV)" % self.getProps()
        
        # Bc Cut
        BcPDCut = "(VFASPF(VCHI2PDOF)< %(TFBc2JpsiHPD_BcVtxCHI2)s ) & (PT > %(TFBc2JpsiHPD_BcPT)s *MeV)" % self.getProps()

        # Lifetime biased cut
        LifetimeBiasedCut = "(MINTREE('pi+'==ABSID,BPVIPCHI2()) > %(TFBc2JpsiHPD_PionIPCHI2)s) & (MINTREE('mu+'==ABSID,BPVIPCHI2()) > %(TFBc2JpsiHPD_MuonIPCHI2)s) & (MINTREE('J/psi(1S)'==ABSID,BPVIPCHI2()) > %(TFBc2JpsiHPD_JpsiIPCHI2)s) & (BPVIPCHI2() < %(TFBc2JpsiHPD_BcIPCHI2)s)" % self.getProps()
        

        #---------------------------
        # J/psi -> Mu Mu
        #---------------------------
        from Hlt2SharedParticles.TrackFittedDiMuon import TrackFittedDiMuon

        FilterJpsi4TFBc2JpsiHPD = Hlt2Member( FilterDesktop # type
                                              , "FilterJpsi4TFBc2JpsiHPD" 
                                              , Code = JpsiPDCut
                                              , Inputs = [ TrackFittedDiMuon ]
                                              , InputPrimaryVertices = "None"
                                              , UseP2PVRelations = False
                                              )
        
        #---------------------------
        # Bc -> J/psi(MuMu) H
        #---------------------------
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions
        
        CombineTFBc2JpsiHPD = Hlt2Member( CombineParticles
                                          , "CombineTFBc2JpsiHPD"
                                          , DecayDescriptor = "[ B_c+ -> J/psi(1S) pi+ ]cc"
                                          , DaughtersCuts = { "pi+" : PionPDCut }
                                          , CombinationCut = combPDCut
                                          , MotherCut = BcPDCut
                                          , Inputs = [ FilterJpsi4TFBc2JpsiHPD, BiKalmanFittedPions ]
                                          , InputPrimaryVertices = "None"
                                          , UseP2PVRelations = False
                                          )

        TFBc2JpsiHPDLoose = bindMembers( "TFBc2JpsiHPrescaledAndDetached", [ TrackFittedDiMuon, FilterJpsi4TFBc2JpsiHPD, BiKalmanFittedPions, CombineTFBc2JpsiHPD ] )
        
        line_Prescaled = Hlt2Line('TFBc2JpsiHPrescaled'
                                  , prescale = self.prescale
                                  , algos = [ TrackFittedDiMuon
                                              , FilterJpsi4TFBc2JpsiHPD
                                              , BiKalmanFittedPions
                                              , CombineTFBc2JpsiHPD ]
                                  , postscale = self.postscale
                                  )

        #---------------------------
        # TFBc2JpsiHDetached
        #---------------------------
        from HltTracking.HltPVs import PV3D
        
        FilterTFBc2JpsiHPD = Hlt2Member( FilterDesktop # type
                                         , "FilterTFBc2JpsiHPD" 
                                         , Code = LifetimeBiasedCut
                                         , Inputs = [ TFBc2JpsiHPDLoose ]
                                         )
        
        
        line_Detached = Hlt2Line('TFBc2JpsiHDetached'
                                 , prescale = self.prescale
                                 , algos = [ PV3D(), TFBc2JpsiHPDLoose , FilterTFBc2JpsiHPD ]
                                 , postscale = self.postscale
                                 )        
        
