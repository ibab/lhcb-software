"""
Stripping lines for B/Bs decays to chi_c0(1P) X or eta_c(1S) X.
All decay modes contain 6 charged hadrons in the final states:
1. B_s0 -> chi_c0(1P) (->K+ K- pi+ pi-) phi
2. B_s0 -> eta_c(1S) (->K+ K- pi+ pi-) phi
3. B_s0 -> chi_c0(1P) (->pi+ pi- pi+ pi-) phi
4. B_s0 -> chi_c0(1P) (->K+ K- pi+ pi-) K*(892)0
5. B_s0 -> eta_c(1S) (->K+ K- pi+ pi-) K*(892)0
"""



__author__ = ['Katarzyna Senderowska']
__date__ = '27/02/2011'
__version__ = '$Revision: 1.1 $'



from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseKaons, StdTightPions, StdTightKaons



__all__ = (
    'B2CharmoniumX_6HAllLinesConf',
    'makeBs2ChicPhi_Chic2PiPiPiPi',
    'makeBs2EtacPhi_Etac2KKPiPi',
    'makeBs2ChicPhi_Chic2KKPiPi',
    'makeB2EtacKst_Etac2KKPiPi',
    'makeB2ChicKst_Chic2KKPiPi',
    'confdict'
    )

confdict = {
    'Prescale'               : 1.0 ,
    'Postscale'              : 1.0 ,
    'TRCHI2'                 : 3,
    'DOCA'                   : 0.1,
    #K parameters
    'K_PT'                   : 750., 
    'K_IPCHI2'               : 4, 
    'K_PIDK'                 : -3.,
    #pi parameters
    'pi_PT'                  : 750.,
    'pi_IPCHI2'              : 4.,   
    'pi_PIDK'                : 3.,   
    #'max' daughters
    'PT_maxetacdaughter'     : 1400., 
    'PT_maxchicdaughter'     : 1300.,
    'IPCHI2_maxdaughter'     : 9.,
    #phi parameters
    'phi_DIRA'               : 0.95, 
    'phi_VDZ'                : 0., 
    'phi_PT'                 : 1900.,
    'phi_IPCHI2'             : 6.,
    'phi_VCHI2'              : 7.,   
    #Kst parameters
    'Kst_DIRA'               : 0.93, 
    'Kst_VDZ'                : 0., 
    'Kst_PT'                 : 1600., 
    'Kst_IPCHI2'             : 4.,
    'Kst_VCHI2'              : 12.,
    #etac parameters
    'etac_DIRA'              : 0.9,
    'etac_VDZ'               : 0.,    
    'etac_IPCHI2'            : 4.,
    'etac_VCHI2'             : 9.,
    'etac_sumPT'             : 4000, 
    #chic/etac parameters
    'chic_DIRA'              : 0.9,
    'chic_VDZ'               : 0.,    
    'chic_IPCHI2'            : 4.,
    'chic_VCHI2'             : 9.,
    'chic_sumPT'             : 3600, 
    #Bs parameters
    'Bs_VDZ'                 : 0.,
    'Bs_DIRA'                : 0.9,
    'Bs_VCHI2'               : 25.
    }



class B2CharmoniumX_6HAllLinesConf(LineBuilder):

    
        __configuration_keys__ = (
        'Prescale',
        'Postscale',
	'TRCHI2',
	'DOCA',
        #K parameters
        'K_PT',
        'K_IPCHI2',
	'K_PIDK',
	#pi parameters
        'pi_PT',    
        'pi_IPCHI2',
	'pi_PIDK',
	#'max' daughters
	'PT_maxetacdaughter',
	'PT_maxchicdaughter', 
	'IPCHI2_maxdaughter',
        #phi parameters
	'phi_DIRA',
        'phi_VDZ',        
        'phi_PT',
        'phi_IPCHI2',
        'phi_VCHI2',
	#Kst parameters
	'Kst_DIRA',
        'Kst_VDZ',        
        'Kst_PT',
        'Kst_IPCHI2',
        'Kst_VCHI2',
	#etac parameters
	'etac_DIRA',
        'etac_VDZ', 
        'etac_IPCHI2',
        'etac_VCHI2',
	'etac_sumPT',
	#chic/etac parameters
	'chic_DIRA',
        'chic_VDZ', 
        'chic_IPCHI2',
        'chic_VCHI2',
	'chic_sumPT',
        #Bs parameters
        'Bs_VDZ',
        'Bs_DIRA',
        'Bs_VCHI2'
        )



        __confdict__={}

        
        
        def __init__(self, name, config) :
            
            LineBuilder.__init__(self, name, config)
            self.__confdict__=config

		
            self.selKaon = Selection( "KaonFor" + name,
                                      Algorithm = self._KaonFilter(),
                                      RequiredSelections = [StdTightKaons]
				      )

            
            self.selPion = Selection( "PionFor" + name,
                                      Algorithm = self._PionFilter(),
                                      RequiredSelections = [StdTightPions]
				      )


            self.selPhi2KK = Selection( "Phi2KKFor" + name,
                                        Algorithm = self._Phi2KKFilter(),
                                        RequiredSelections = [ DataOnDemand(Location="Phys/StdLoosePhi2KK/Particles") ]
					)


	    self.selKst2KPi = Selection( "Kst2KPiFor" + name,
					 Algorithm = self._Kst2KPiFilter(),
					 RequiredSelections = [self.selKaon, self.selPion]
					 )
      

            self.selChic2KKPiPi = Selection( "Chic2KKPiPiFor" + name,
					     Algorithm = self._Chic2KKPiPiFilter(),
					     RequiredSelections = [self.selKaon, self.selPion]
					     ) 


	    self.selEtac2KKPiPi = Selection( "Etac2KKPiPiFor" + name,
					     Algorithm = self._Etac2KKPiPiFilter(),
					     RequiredSelections = [self.selKaon, self.selPion]
					     )


	    self.selChic2PiPiPiPi = Selection( "Chic2PiPiPiPiFor" + name,
					       Algorithm = self._Chic2PiPiPiPiFilter(),
					       RequiredSelections = [self.selPion]
					       )
	    

	    self.selBs2ChicPhi_Chic2PiPiPiPi = makeBs2ChicPhi_Chic2PiPiPiPi('Bs2ChicPhi_Chic2PiPiPiPi' + name,
									    DecayDescriptors = [ 'B_s0 -> chi_c0(1P) phi(1020)' ],
									    PhiSel = self.selPhi2KK, 
									    ChicSel = self.selChic2PiPiPiPi,
									    Bs_VCHI2 = config['Bs_VCHI2'],
									    Bs_DIRA = config['Bs_DIRA'],
									    Bs_VDZ = config['Bs_VDZ']
									    )

	    
	    self.selBs2EtacPhi_Etac2KKPiPi = makeBs2EtacPhi_Etac2KKPiPi('Bs2EtacPhi_Etac2KKPiPi' + name,
									DecayDescriptors = [ 'B_s0 -> eta_c(1S) phi(1020)' ],
									PhiSel = self.selPhi2KK, 
									EtacSel = self.selEtac2KKPiPi,
									Bs_VCHI2 = config['Bs_VCHI2'],
									Bs_DIRA = config['Bs_DIRA'],
									Bs_VDZ = config['Bs_VDZ']
									)
	    
	    
            self.selBs2ChicPhi_Chic2KKPiPi = makeBs2ChicPhi_Chic2KKPiPi('Bs2ChicPhi_Chic2KKPiPi' + name,
                                                                        DecayDescriptors = [ 'B_s0 -> chi_c0(1P) phi(1020)' ],
                                                                        PhiSel = self.selPhi2KK, 
                                                                        ChicSel = self.selChic2KKPiPi,
                                                                        Bs_VCHI2 = config['Bs_VCHI2'],
                                                                        Bs_DIRA = config['Bs_DIRA'],
                                                                        Bs_VDZ = config['Bs_VDZ']
                                                                        )

	
	    self.selB2EtacKst_Etac2KKPiPi  = makeB2EtacKst_Etac2KKPiPi('Bs2EtacKst_Etac2KKPiPi' + name,
								       DecayDescriptors = [ 'B0 -> eta_c(1S) K*(892)0' ],
								       KstSel = self.selKst2KPi, 
								       EtacSel = self.selEtac2KKPiPi,
								       Bs_VCHI2 = config['Bs_VCHI2'],
								       Bs_DIRA = config['Bs_DIRA'],
								       Bs_VDZ = config['Bs_VDZ']
								       )


	    self.selB2ChicKst_Chic2KKPiPi = makeB2ChicKst_Chic2KKPiPi('B2ChicKst_Chic2KKPiPi' + name,
								      DecayDescriptors = [ 'B0 -> chi_c0(1P) K*(892)0' ],
								      KstSel = self.selKst2KPi, 
								      ChicSel = self.selChic2KKPiPi,
								      Bs_VCHI2 = config['Bs_VCHI2'],
								      Bs_DIRA = config['Bs_DIRA'],
								      Bs_VDZ = config['Bs_VDZ']
								      )


	    self.Bs2ChicPhi_Chic2PiPiPiPi = StrippingLine('Bs2ChicPhi_Chic2PiPiPiPi' + name + 'Line'
							  , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
							  , prescale = config['Prescale']
							  , selection = self.selBs2ChicPhi_Chic2PiPiPiPi
							  )



	    self.Bs2EtacPhi_Etac2KKPiPi = StrippingLine('Bs2EtacPhi_Etac2KKPiPi' + name + 'Line'
							, HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
							, prescale = config['Prescale']
							, selection = self.selBs2EtacPhi_Etac2KKPiPi
							)


	    self.Bs2ChicPhi_Chic2KKPiPi = StrippingLine('Bs2ChicPhi_Chic2KKPiPi' + name + 'Line'
                                                        , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
                                                        , prescale = config['Prescale']
                                                        , selection = self.selBs2ChicPhi_Chic2KKPiPi
                                                        )


	    self.B2EtacKst_Etac2KKPiPi = StrippingLine('B2EtacKst_Etac2KKPiPi' + name + 'Line'
						       , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
						       , prescale = config['Prescale']
						       , selection = self.selB2EtacKst_Etac2KKPiPi
						       )


	    self.B2ChicKst_Chic2KKPiPi = StrippingLine('B2ChicKst_Chic2KKPiPi' + name + 'Line'
					   , HLT  = "HLT_PASS_RE('Hlt2.*Topo.*Decision')"
					   , prescale = config['Prescale']
					   , selection = self.selB2ChicKst_Chic2KKPiPi
					   )
	    

	    self.registerLine(self.Bs2ChicPhi_Chic2PiPiPiPi)	
	    self.registerLine(self.Bs2EtacPhi_Etac2KKPiPi)
	    self.registerLine(self.Bs2ChicPhi_Chic2KKPiPi)
	    self.registerLine(self.B2EtacKst_Etac2KKPiPi)
	    self.registerLine(self.B2ChicKst_Chic2KKPiPi)
            
            
            
            
        def _PionFilter( self ):
		_code = "(TRCHI2DOF < %(TRCHI2)s) & (PT > %(pi_PT)s *MeV)"\
			"& (MIPCHI2DV(PRIMARY) > %(pi_IPCHI2)s) &  (PIDK < %(pi_PIDK)s)" % self.__confdict__
		_pi = FilterDesktop( Code = _code )
		return _pi

        
        def _KaonFilter( self ):
		_code = "  (TRCHI2DOF < %(TRCHI2)s) & (PT > %(K_PT)s *MeV)"\
			"& (MIPCHI2DV(PRIMARY) > %(K_IPCHI2)s) &  (PIDK > %(K_PIDK)s)" % self.__confdict__
		_k = FilterDesktop( Code = _code )
		return _k


	def _Phi2KKFilter( self ):
                _code = "(MINTREE(ABSID=='K+',PIDK) > -3) & (ADMASS('phi(1020)') < 30.*MeV) & (PT> %(phi_PT)s) & (MIPCHI2DV(PRIMARY) > %(phi_IPCHI2)s) & (VFASPF(VCHI2) < %(phi_VCHI2)s) & (BPVDIRA > %(phi_DIRA)s)" % self.__confdict__
		_phi2kk = FilterDesktop(Code = _code)
		return _phi2kk


	def _Kst2KPiFilter( self ):
		_decayDescriptor = '[K*(892)0 -> K+ pi-]cc'
		_combinationCut = "(ADAMASS('K*(892)0') < 100 *MeV) & (APT > %(Kst_PT)s)" % self.__confdict__
		_motherCut = "(VFASPF(VZ) > %(Kst_VDZ)s *mm) & (BPVDIRA > %(Kst_DIRA)s) & (MIPCHI2DV(PRIMARY) > %(Kst_IPCHI2)s) & (VFASPF(VCHI2) < %(Kst_VCHI2)s)" % self.__confdict__
		_kst2kpi = CombineParticles( DecayDescriptor = _decayDescriptor,
					    CombinationCut = _combinationCut,
					    MotherCut = _motherCut)                            
		return _kst2kpi


        def _Chic2KKPiPiFilter( self ):
		_decayDescriptors = [ 'chi_c0(1P) -> K+ K- pi+ pi-' ]
		_combinationCut = "(ADAMASS('chi_c0(1P)') < 60 *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(chic_sumPT)s *MeV) & (AMAXDOCA('') < %(DOCA)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),PT)) > %(PT_maxchicdaughter)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),MIPCHI2DV(PRIMARY))) > %(IPCHI2_maxdaughter)s)" % self.__confdict__
		_motherCut = "(BPVDIRA> %(chic_DIRA)s) & (MIPCHI2DV(PRIMARY) > %(chic_IPCHI2)s) & (VFASPF(VCHI2/VDOF) < %(chic_VCHI2)s)"  % self.__confdict__
		_chic2kkpipi = CombineParticles( DecayDescriptors = _decayDescriptors,
						 CombinationCut = _combinationCut,
						 MotherCut = _motherCut)                             
		return _chic2kkpipi


	def _Etac2KKPiPiFilter( self ):
		_decayDescriptors = [ 'eta_c(1S) -> K+ K- pi+ pi-'  ]
		_combinationCut = "(ADAMASS('eta_c(1S)') < 90 *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(etac_sumPT)s *MeV) & (AMAXDOCA('') < %(DOCA)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),PT)) > %(PT_maxetacdaughter)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),MIPCHI2DV(PRIMARY))) > %(IPCHI2_maxdaughter)s)" % self.__confdict__
		_motherCut = "(BPVDIRA> %(etac_DIRA)s) & (MIPCHI2DV(PRIMARY) > %(etac_IPCHI2)s) & (VFASPF(VCHI2/VDOF) < %(etac_VCHI2)s)"  % self.__confdict__
		_etac2kkpipi = CombineParticles( DecayDescriptors = _decayDescriptors,
						 CombinationCut = _combinationCut,
						 MotherCut = _motherCut)                             
		return _etac2kkpipi


        def _Chic2PiPiPiPiFilter( self ):
		_decayDescriptors = [ 'chi_c0(1P) -> pi+ pi- pi+ pi-' ]
		_combinationCut = "(ADAMASS('chi_c0(1P)') < 60 *MeV) & (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) > %(chic_sumPT)s *MeV) & (AMAXDOCA('') < %(DOCA)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),PT)) > %(PT_maxchicdaughter)s) & (AMAXCHILD(MAXTREE(((ABSID=='pi+') | (ABSID=='K+')),MIPCHI2DV(PRIMARY))) > %(IPCHI2_maxdaughter)s)" % self.__confdict__
		_motherCut = "(BPVDIRA> %(chic_DIRA)s) & (MIPCHI2DV(PRIMARY) > %(chic_IPCHI2)s) & (VFASPF(VCHI2/VDOF) < %(chic_VCHI2)s)"  % self.__confdict__
		_chic2pipipipi = CombineParticles( DecayDescriptors = _decayDescriptors,
						   CombinationCut = _combinationCut,
						   MotherCut = _motherCut)                             
		return _chic2pipipipi
        

def makeBs2ChicPhi_Chic2PiPiPiPi(name,
				 DecayDescriptors,
				 PhiSel,
				 ChicSel,
				 Bs_VCHI2,
				 Bs_DIRA,
				 Bs_VDZ):
	_combinationCut = "(ADAMASS('B_s0') < 500 *MeV)"
	_motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
	
	_B = CombineParticles(DecayDescriptors = DecayDescriptors,
			      CombinationCut = _combinationCut,
			      MotherCut = _motherCut)
    
	return Selection (name,
			  Algorithm = _B,
			  RequiredSelections = [PhiSel, ChicSel])


def makeBs2EtacPhi_Etac2KKPiPi(name,
		   DecayDescriptors,
		   PhiSel,
		   EtacSel,
		   Bs_VCHI2,
		   Bs_DIRA,
		   Bs_VDZ):
	_combinationCut = "(ADAMASS('B_s0') < 500 *MeV)"
	_motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()

	_B = CombineParticles(DecayDescriptors = DecayDescriptors,
			      CombinationCut = _combinationCut,
			      MotherCut = _motherCut)
    
	return Selection (name,
			  Algorithm = _B,
			  RequiredSelections = [PhiSel, EtacSel])


def makeBs2ChicPhi_Chic2KKPiPi(name,
			       DecayDescriptors,
			       PhiSel,
			       ChicSel,
			       Bs_VCHI2,
			       Bs_DIRA,
			       Bs_VDZ):
	_combinationCut = "(ADAMASS('B_s0') < 500 *MeV)"
	_motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()

	_B = CombineParticles(DecayDescriptors = DecayDescriptors,
			      CombinationCut = _combinationCut,
			      MotherCut = _motherCut)
    
	return Selection (name,
			  Algorithm = _B,
			  RequiredSelections = [PhiSel, ChicSel])


def makeB2EtacKst_Etac2KKPiPi(name,
		  DecayDescriptors,
		  KstSel,
		  EtacSel,
		  Bs_VCHI2,
		  Bs_DIRA,
		  Bs_VDZ):
	_combinationCut = "(ADAMASS('B0') < 500 *MeV)"
	_motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()

	_B = CombineParticles(DecayDescriptors = DecayDescriptors,
			      CombinationCut = _combinationCut,
			      MotherCut = _motherCut)
	
	return Selection (name,
			  Algorithm = _B,
			  RequiredSelections = [KstSel, EtacSel])


def makeB2ChicKst_Chic2KKPiPi(name,
		  DecayDescriptors,
		  KstSel,
		  ChicSel,
		  Bs_VCHI2,
		  Bs_DIRA,
		  Bs_VDZ):
	_combinationCut = "(ADAMASS('B0') < 500 *MeV)"
	_motherCut = "(VFASPF(VZ) > %(Bs_VDZ)s *mm) & (BPVDIRA> %(Bs_DIRA)s) & (VFASPF(VCHI2/VDOF) < %(Bs_VCHI2)s)"  % locals()
	
	_B = CombineParticles(DecayDescriptors = DecayDescriptors,
			      CombinationCut = _combinationCut,
			      MotherCut = _motherCut)
	
	return Selection (name,
			  Algorithm = _B,
			  RequiredSelections = [KstSel, ChicSel])
