#################################################
#		 EM physics study and validation		#
#		with EMGaussMoni and BremVeloCheck		#
#												#
#		Jobs are executed with this script		#
#		     Peter Griffith 21.11.14			#
#################################################

#####################################################################################


import os
from tempfile import mkstemp
from shutil import move
from os import remove, close
import re
import sys
from analysis import *
import time 
startTime = time.time()

#from analysis import dedxAnalysis, bremAnalysis

def replaceFile(fPath, pattern, sub):
	tpath=fPath+"_tmp"
	with open(fPath) as infile, open(tpath,"w") as outfile:
		for i,line in enumerate(infile):
			if pattern in line:
				outfile.write("\t"+sub+"\n")
			else:
				outfile.write(line)
	move(tpath, fPath)


def replaceVar(var,val):
	fPath = 'EMstudy_config.py'
	pattern = var+" = "
	sub = pattern+str(val)
	replaceFile(fPath,pattern,sub)


def replaceStr(var,val):
	fPath = 'EMstudy_config.py'
	pattern = var+" = "
	sub = pattern+"'"+str(val)+"'"
	replaceFile(fPath,pattern,sub)


def runJob(pgunID,emPL,pgunE):
	#set required values in options files for scanning
	replaceVar('pgunID',pgunID)
	replaceStr('emPL',emPL)
	replaceVar('pgunE',pgunE)
	#run job
	os.system('gaudirun.py EMstudy_config.py EMstudy_opts.py Gauss-Job-EMstudy.py')


#####################################################################################

#import the configuration
from EMstudy_config import config
opts=config()
testType = opts['testType']


if(opts['pgunE']=='scan'):
	energies=opts['eScan_list']
else:
	energies = [opts['pgunE']]

if(opts['emPL']=='scan'):
	plists = opts['plScan_list']
else:
	plists = [opts['emPL']]

if(opts['pgunID']=='scan'):
	particles = opts['partScan_list']
else:
	particles = [opts['pgunID']]


print opts



if (opts['runGenerator']):
	for pgunID in particles:
		for emPL in plists:
			for pgunE in energies:
				runJob(pgunID,emPL,pgunE)

if (opts['doAnalysis']):
	if(testType=='brem' or testType == 'both'):
		bremAnalysis(particles,plists,energies,opts)
	if(testType =='dedx' or testType == 'both'):
		dedxAnalysis(particles,plists,energies,opts)

endTime = time.time()

print("##########################################################")
print("#                      FINISHED.                         #")							 
print("##########################################################")
print("RUNTIME = "+str(endTime-startTime)+" seconds")

