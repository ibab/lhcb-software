#!/usr/bin/env python
#################################################################################
## This option file helps you save time by running in sequence                 ##
## the radiation lenght tool with particle fired from different positions      ##
## along the Z axis. Only the plane after the velo is activated.               ##
## You can run this simply by "python rad_length_scan.py"                      ##
## Twiki at: https://twiki.cern.ch/twiki/bin/view/LHCb/RadLengthStudies        ##
##                                                                             ##
##  @author : L.Pescatore                                                      ##
##  @date   : last modified on 2015-06-29                                      ##
#################################################################################

import sys
import os
from tempfile import NamedTemporaryFile
from ROOT import *

pwd = os.environ['PWD']
outputpath = pwd
out = 'Rad.root'
zmin = 0.    #in mm minimum 0.01
zmax = 100.    #in mm minimum 0.01
step = 10.   #in mm minimum 0.01
nevts = 2000

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

from Configurables import GiGa, GiGaStepActionSequence, RadLengthColl
giga = GiGa()
giga.addTool( GiGaStepActionSequence("StepSeq") , name = "StepSeq" )
giga.StepSeq.addTool( RadLengthColl )
giga.StepSeq.RadLengthColl.orig_x = {x:.2f}
giga.StepSeq.RadLengthColl.orig_y = {y:.2f}
giga.StepSeq.RadLengthColl.orig_z = {z:.2f}


NTupleSvc().Output = ["FILE2 DATAFILE='Rad_{x}_{y}_{z}.root' TYP='ROOT' OPT='NEW'"]

from Gauss.Configuration import *
LHCbApp().EvtMax = {nevts}
'''


############# Run tool and reate tuple

os.system("mkdir -p plots")
cmd = "gaudirun.py {base}/Gauss-Job.py {base}/RadLengthAna_VELO.py ".format(base=base)

s = 100.
z_to_scan = [ x/s for x in range(int(s*zmin),int(s*(zmax+step)),int(s*step)) ]

print "Starting scan! Points: ", z_to_scan

for zz in z_to_scan :
	with NamedTemporaryFile(suffix=".py") as tmp :
		tmp.write(pguntmp.format(x=0.0,y=0.0,z=zz,nevts=nevts))
		tmp.flush()
		
		os.system(cmd+tmp.name)

output=outputpath+'/'+out
merge_command = 'hadd -f {output} {pwd}/Rad_*.root'.format(output=output, pwd=pwd)
os.system(merge_command)

### Make output plot

f = TFile(out)
t = f.Get("RadLengthColl/tree")
hout = TGraphErrors()
print "z\tAvg\tErr"
for i,zz in enumerate(z_to_scan) :
	t.Draw("cumradlgh>>h","(TMath::Abs(origz - %f) < 1e-5)" % zz)
	h = gPad.GetPrimitive("h")
	avg     = h.GetMean()
	avg_err = h.GetMeanError()
	print '{0:.3f}\t{1:.5f}\t{2:.5f}'.format(zz,avg,avg_err)
	hout.SetPoint(i,zz,avg)
	hout.SetPointError(i,0.,avg_err)

c = TCanvas()
hout.GetXaxis().SetTitle("z [mm]")
hout.GetYaxis().SetTitle("Average rad. length")
hout.SetTitle("Average rad. length in VELO")
hout.SetMarkerStyle(20)
hout.SetMarkerColor(1)
hout.SetMarkerSize(1.)
hout.Draw("AP")
#hout.SetMinimum(0.)
c.Print("Avg_rad_length_vs_z.pdf")


