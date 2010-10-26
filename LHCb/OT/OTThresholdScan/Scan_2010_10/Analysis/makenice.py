from ROOT import *
from array import array

from function import *

colors = [0,11,5,7,3,6,2,4,12,1]

colours = array('i',colors)

gROOT.SetStyle("Plain")
gStyle.SetPalette(10,colours)
gStyle.SetOptFit(0111)
#gStyle.SetOptFitW(0.3)

gStyle.SetOptStat(0)
gStyle.SetStatX(0.9)
gStyle.SetStatY(0.9)
gStyle.SetStatW(0.2)
gStyle.SetStatH(0.1)
gStyle.SetStatFont(12)

gStyle.SetTitleFont(12,"poep")
gStyle.SetTitleFont(12,"X") 
gStyle.SetTitleFont(12,"Y") 
gStyle.SetLabelFont(12,"X")
gStyle.SetLabelFont(12,"Y")
gStyle.SetTitleSize(0.05,"X")
gStyle.SetTitleSize(0.05,"Y")
gStyle.SetLabelSize(0.045,"X")
gStyle.SetLabelSize(0.045,"Y")
gStyle.SetTitleOffset(0.9,"X")
gStyle.SetTitleOffset(0.9,"Y")
gStyle.SetLabelOffset( 0.005,"X")
gStyle.SetLabelOffset( 0.005,"Y")
gStyle.SetCanvasBorderSize(2)
gStyle.SetLineWidth(1)
                       
#########################
# opening the files
#########################

nsteps = 100

tfile = TFile("mergedhistos.root")

for j in range(0,nsteps+1):
        vars()['h'+str(j)] = tfile.Get('HitEfficiency/threshold_'+str(j))

# make the list of histos per layer:

nlayers = 12
nstepsperlayer = 8
stepoffset = 1

for i in range(0,nlayers):
    startvalue = nstepsperlayer*i+stepoffset
    print 'offset threshold per layer =', startvalue
    vars()['histos'+str(i)] = []
    for j in range(0,nstepsperlayer):
        vars()['histos'+str(i)].append(vars()['h'+str(startvalue+j)])

#overwrite histos0
histos0 = []
for i in range(2,9):
    vars()['h'+str(i)] = tfile.Get('HitEfficiency/threshold_'+str(i))
    histos0.append(vars()['h'+str(i)])

#######################################
#define the S-curves (error functions)
#######################################

f1 = TF1("f1","0.5*(1.-TMath::Erf((x-[0])/(TMath::Sqrt(2.)*[1])))",750.,1550.) 
f1.SetParName(0,"Half.Eff.Point (mV)")
f1.SetParName(1,"Width (mV)")
f1.SetParLimits(0,1000.,1600.) 
f1.SetParLimits(1, 20.,  150.) 
f1.SetParameter(0,1327.) 
f1.SetParameter(1, 85.) 

f2 = TF1("f2","0.5*([2]-[3])*(1.-TMath::Erf((x-[0])/(TMath::Sqrt(2.)*[1])))",750.,1550.) 
f2.SetParName(0,"Half.Eff.Point (mV)")
f2.SetParName(1,"Width (mV)")
f2.SetParName(2,"Plateau")
f2.SetParName(3,"Tail")
f2.SetParLimits(0,1000.,1600.) 
f2.SetParLimits(1, 20.,  150.) 
f2.SetParameter(0,1327.) 
f2.SetParameter(1, 85.) 
f2.SetParameter(2, 1.) 
f2.SetParameter(3, 0.)

specialmakeplotslayer0(histos0,'Layer0',1280,1420)

#makeplotslayer(histos1,'Layer1',1280,1420)
#makeplotslayer(histos2,'Layer2',1280,1420)
#makeplotslayer(histos3,'Layer3',1280,1420)
#makeplotslayer(histos4,'Layer4',1280,1420)
#makeplotslayer(histos5,'Layer5',1280,1420)
#makeplotslayer(histos6,'Layer6',1280,1420)
#makeplotslayer(histos7,'Layer7',1280,1420)
#makeplotslayer(histos8,'Layer8',1280,1420)
#makeplotslayer(histos9,'Layer9',1280,1420)
#makeplotslayer(histos10,'Layer10',1280,1420)
#makeplotslayer(histos11,'Layer11',1280,1420)

