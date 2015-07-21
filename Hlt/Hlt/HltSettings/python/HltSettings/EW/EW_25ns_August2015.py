from GaudiKernel.SystemOfUnits import GeV, MeV, mm

class EW_25ns_August2015( object ) :
	"""
	Threshold settings for Hlt2 Electroweak (EW) lines

	WARNING: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

	@author K. Dreimanis, k.dreimanis@liverpool.ac.uk
	@date 2015-07-21
	"""

	__all__ = ( 'ActiveHlt2Lines' )

	def ActiveHlt2Lines( self ) :
	"""
	Returns the list of ACTIVE lines
	"""

	lines = [
		# DiMuon
		'Hlt2EWDiMuonZ',
		'Hlt2EWDiMuonDY1',
		'Hlt2EWDiMuonDY2',
		'Hlt2EWDiMuonDY3',
		'Hlt2EWDiMuonDY4',
		# Single Muon
		'Hlt2EWSingleMuonLowPt',
		'Hlt2EWSingleMuonHighPt',
		'Hlt2EWSingleMuonVHighPt',
		# DiElectron
		'Hlt2EWDiElectronDY',
		'Hlt2EWDiElectronHighMass',
		# Single Electron
		'Hlt2EWSingleElectronLowPt',
		'Hlt2EWSingleElectronHighPt',
		'Hlt2EWSingleElectronVHighPt',
		# Single Tau
		'Hlt2EWSingleTauHighPt2Prong',
		'Hlt2EWSingleTauHighPt3Prong'
		]

	return lines

	def Thresholds( self ) :
		"""
		Returns a dictionary of cuts
		"""
		d = {}
		from Hlt2Lines.EW.Lines import EWLines

		d.update( { EWLines : {
			'Prescale' : {'Hlt2EWDiMuonDY1'             :  0.02,
				      'Hlt2EWDiMuonDY2'             :   0.3,
				      'Hlt2EWSingleMuonLowPt'       : 0.002,
				      'Hlt2EWSingleMuonHighPt'      :   0.1,
				      'Hlt2EWSingleElectronLowPt'   : 0.001,
				      'Hlt2EWSingleElectronHighPt'  :  0.01,
				      'Hlt2EWSingleTauHighPt2Prong' :   0.2,
				      'Hlt2EWSingleTauHighPt3Prong' :   0.5},
			# DiMuon
			'DiMuonZ'               : {'MinMass'           : 40000 * MeV,
						   'Pt'                :     0 * MeV},
			
			'DiMuonDY1'             : {'MinMass'           : 2500 * MeV,
						   'MinExcMass'        : 3000 * MeV,
						   'MaxExcMass'        : 3200 * MeV,
						   'MuPt'              :  800 * MeV,
						   'Pt'                :    0 * MeV,
						   'TkChi2'            :   10},
			
			'DiMuonDY2'             : {'MinMass'           : 5000 * MeV,
						   'MuPt'              : 1000 * MeV,
						   'Pt'                :    0 * MeV,
						   'TkChi2'            :   10},
			
			'DiMuonDY3'             : {'MinMass'           : 10000 * MeV,
						   'MuPt'              :     0 * MeV,
						   'TkChi2'            :    10},
			
			'DiMuonDY4'             : {'MinMass'           : 20000 * MeV,
						   'MuPt'              :     0 * Mev,
						   'TkChi2'            :    10},

			# Single Muon
			'SingleMuonLowPt'       : {'Pt'                : 4800 * MeV,
						   'TkChi2'            :   10},
			
			'SingleMuonHighPt'      : {'Pt'                : 10000 * MeV},
			
			'SingleMuonVHighPt'     : {'Pt'                : 12500 * MeV},

			# DiElectron
			'DiElectronDY'          : {'L0Req'             : "L0_CHANNEL('Electron')",
						   'Hlt1Req'           : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
						   'MinMass'           : 10000 * MeV,
						   'MaxMass'           : 1e+10 * MeV,
						   'VtxChi2'           :    25,
						   'Pt'                :  -999 * MeV,
						   'ElecPt'            :  1000 * MeV,
						   'ElecPIDe'          :   1.5,
						   'ElecTkChi2'        :    10,
						   'PrsMin'            :    50,
						   'EcalMin'           :   0.1,
						   'HcalMax'           :  0.05},
			
			'DiElectronHighMass'    : {'L0Req'             : "L0_CHANNEL('Electron')",
						   'Hlt1Req'           : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
						   'MinMass'           : 20000 * MeV,
						   'VtxChi2'           :    25,
						   'TkChi2'            :    10,
						   'Pt'                :  -999 * MeV,
						   'ElecPt'            : 10000 * MeV,
						   'PrsMin'            :    50,
						   'EcalMin'           :   0.1,
						   'HcalMax'           :  0.05},
			
			# Single Electron
			'SingleElectronLowPt'   : {'L0Req'             : "L0_CHANNEL('Electron')",
						   'Hlt1Req'           : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
						   'Pt'                :  4800 * MeV,
						   'PIDe'              :     4,
						   'TkChi2'            :     5,
						   'PrsMin'            :    50,
						   'EcalMin'           :   0.1,
						   'HcalMax'           :  0.05},
			
			'SingleElectronHighPt'  : {'L0Req'             : "L0_CHANNEL('Electron')",
						   'Hlt1Req'           : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
						   'Pt'                : 10000 * MeV,
						   'PrsMin'            :    50,
						   'EcalMin'           :   0.1,
						   'HcalMax'           :  0.05,
						   'TkChi2'            :    20},

			'SingleElectronVHighPt' : {'L0Req'             : "L0_CHANNEL('Electron')",
						   'Hlt1Req'           : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
						   'Pt'                : 15000 * MeV,
						   'PrsMin'            :    50,
						   'EcalMin'           :   0.1,
						   'HcalMax'           :  0.05,
						   'TkChi2'            :    20},
			# Single Tau
			'TauTrkFilter'          : {'trk_PT'            :     2 * GeV,
						   'trk_TRCHI2DOF_MAX' :     3},

			'TauRhoCombiner'        : {'RHO_M_MIN'         :     0 * GeV,
						   'RHO_M_MAX'         :     2 * GeV,
						   'RHO_PT_MIN'        :     0 * GeV,
						   'PI_PT_MIN'         :     2 * GeV},

			'Tau3PiCombiner'        : {'sumPT'             :     8 * GeV,
						   'PT'                :     8 * GeV,
						   'childPT'           :     2 * GeV,
						   'maxchildPT'        :     5 * GeV,
						   'FDCHI2_MIN'        :    10,
						   'FDT_MIN'           :   0.5,
						   'VCHI2_NDOF_MAX'    :    20,
						   'DOCA_MAX'          :   0.1,
						   'CORRM_MIN'         :   1.2 * GeV,
						   'CORRM_MAX'         :   2.0 * GeV},

			'TauPiPi0Combiner'      : {'sumPT'             :     8 * GeV,
						   'PT'                :     8 * GeV,
						   'childPT'           :     5 * GeV,
						   'maxchildPT'        :     5 * GeV,
						   'M_MIN'             :     0 * GeV,
						   'M_MAX'             :   1.5 * GeV},

			'SingleTauHighPt2Prong' : {'PT'                :    15 * GeV},

			'SingleTauHighPt3Prong' : {'PT'                :    12 * GeV}

			} } )

	return d

