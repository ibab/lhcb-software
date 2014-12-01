#################################################
#		 EM physics study and validation		#
#		with EMGaussMoni and BremVeloCheck		#
#												#
#	    	Configure run options here			#
#		     Peter Griffith 21.11.14			#
#################################################

def config():

#################################################################
# Choose whether to run Bremsstrahlung test, dE/dx test or both #
#################################################################
	#testType = 'brem'	# to run Brem tool only  
	testType ='dedx'	# to run dedx monitoring algorithm only
	#testType = 'brem'	# to run both

#####################################################
# Choose whether to run on r/phi velo or velo pixel #
#####################################################
	#veloType = 'vp' 	# for velo pixel
	veloType = 'velo' # for r/phi velo. 

#Peter Griffith 21.11.14: EMGaussMoni is being updated to work with VP.
#Currently job will not crAsh, but will not produce required dEdx output

############################################## 
# Set particle gun and simulation options	 #
# Setting pgunID, pgunE or emPL to 'scan'    #
# will cause program to loop over the values #
# speficied in the scan lists below          #
##############################################
	pgunID = 'scan'
						# Particle to run on. [11,13,211] for electron, muon, pion
	pgunE = 'scan'
						# energy of particles if running a single job
	emPL = 'scan'

						# specify G4 EM physics lists to use in simulation
						# [NoCuts, Opt1, Opt2, Opt3, LHCb]

	nEvts = 1000		# number of particles to generate
	dRays = False		# choose whether to simulate delta rays
	

####################################################
# Specify energies, physics lists and particles to # 
# scan if any scan booleans above are true. If so, #
# energy/list/particle variables above are ignored #
# and the corresponding list is used instead       #
####################################################
	eScan_list=[0.1, 0.2, 0.4, 1., 5., 10., 16.8, 50., 100., 120., 168.]
	plScan_list = ['NoCuts','Opt1','Opt2','Opt3','LHCb']
#	plScan_list = ['NoCuts']
	partScan_list = [11,13,211]

########################
# Output configuration #
########################
	saveSim = False 		# if true, the .sim file from the job will be saved. 
							# EmGaussMoni can be be run on it in GaussRead mode using
							# emMoni_GaussRead.py

	runGenerator = False		# if true, the jobs will be submitted to generate the histos files
	doAnalysis = True			# if true, the outputs will be analysed to produce plots and numbers


	return {
			'testType':			testType, 
			'veloType':			veloType, 
			'pgunID':			pgunID,
			'pgunE':			pgunE, 
			'nEvts':			nEvts, 
			'dRays':			dRays,
			'emPL':				emPL, 
			'saveSim':			saveSim,
			'eScan_list':		eScan_list,
			'plScan_list':		plScan_list,
			'partScan_list':	partScan_list,
			'runGenerator':		runGenerator,
			'doAnalysis':		doAnalysis
			}

