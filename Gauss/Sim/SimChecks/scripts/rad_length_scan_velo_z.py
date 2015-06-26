#!/usr/bin/env python
#################################################################################
## This option file helps you save time by running in sequence                 ##
## the radiation lenght tool with particle fired from different positions      ##
## along the Z axis. Only the plane after the velo is activated.               ##
## You can run this simply by "python rad_length_scan.py"                      ##
## Twiki at: https://twiki.cern.ch/twiki/bin/view/LHCb/RadLengthStudies        ##
##                                                                             ##
##  @author : L.Pescatore                                                      ##
##  @date   : last modified on 2015-06-26                                      ##
#################################################################################

import sys
import os
from tempfile import NamedTemporaryFile

pwd = os.environ['PWD']
outputpath = pwd
out = 'Rad.root'
zmin = 0.    #in mm minimum 0.01
zmax = 1.    #in mm minimum 0.01
step = 0.5   #in mm minimum 0.01

###########################################

base = os.environ["SIMCHECKSROOT"] + "/options/RadLength/"

pguntmp = '''from Gaudi.Configuration import *
from Configurables import ParticleGun
from GaudiKernel.SystemOfUnits import *
from Configurables import MomentumRange

ParticleGun = ParticleGun("ParticleGun")
ParticleGun.EventType = 53210205

from Configurables import MaterialEval
ParticleGun.addTool(MaterialEval, name="MaterialEval")
ParticleGun.ParticleGunTool = "MaterialEval"

from Configurables import FlatNParticles
ParticleGun.addTool(FlatNParticles, name="FlatNParticles")
ParticleGun.NumberOfParticlesTool = "FlatNParticles"
ParticleGun.FlatNParticles.MinNParticles = 1
ParticleGun.FlatNParticles.MaxNParticles = 1
ParticleGun.MaterialEval.PdgCode = 14
ParticleGun.MaterialEval.ModP = 50 * GeV
ParticleGun.MaterialEval.EtaPhi = True
ParticleGun.MaterialEval.Xorig = {x:.2f}*mm
ParticleGun.MaterialEval.Yorig = {y:.2f}*mm
ParticleGun.MaterialEval.Zorig = {z:.2f}*mm
ParticleGun.MaterialEval.ZPlane = 19.89*m

NTupleSvc().Output = ["FILE2 DATAFILE='Rad_{x}_{y}_{z}.root' TYP='ROOT' OPT='NEW'"]
'''

os.system("mkdir -p plots")
cmd = "gaudirun.py {base}/Gauss-Job.py {base}/RadLengthAna_VELO.py ".format(base=base)

s = 100.
for zz in [ x/s for x in range(int(s*zmin),int(s*(zmax+step)),int(s*step)) ] :
	with NamedTemporaryFile(suffix=".py") as tmp :
		tmp.write(pguntmp.format(x=0.0,y=0.0,z=zz))
		tmp.flush()
		
		os.system(cmd+tmp.name)

output=outputpath+'/'+out
merge_command = 'hadd -f {output} {pwd}/Rad*.root'.format(output=output, pwd=pwd)
os.system(merge_command)


