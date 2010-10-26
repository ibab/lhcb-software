from ROOT import *
from array import array

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

tfile_jul = TFile("eff-Jul2010-L0.root")
tfile_aug = TFile("eff-Aug2010-L0.root")

halfeff_jul = tfile_jul.Get('halfeffpoint')
halfeff_aug = tfile_aug.Get('halfeffpoint')
h_subtr=TH2D( "h_subtr", "H_subtr",74, -3157.0, 3157.0, 100, -2800,2800)
h_subtr.Add(halfeff_aug,halfeff_jul,-1.)

# Make empty bins large and negative for plotting purposes...
xmax=halfeff_jul.GetNbinsX()
ymax=halfeff_jul.GetNbinsY()
for ix in range(0,xmax):
    for iy in range(0, ymax):
        binnr = h_subtr.GetBin(ix,iy)
        halfeff_shift = h_subtr.GetBinContent(binnr)
        if halfeff_shift == 0.:
            h_subtr.SetBinContent(binnr, -9999.)

# ------------------------------------
# Draw Canvas!
# ------------------------------------
print "*** Draw canvas"
canvas1 = TCanvas("c1", "c1",10,10,300,500)
canvas1.Divide(1, 3)

canvas1.cd(1)
halfeff_jul.SetTitle('Half.Eff.Point (mV) - T1L0 - Jul 2010')
halfeff_jul.Draw("colz")

canvas1.cd(2)
halfeff_aug.SetTitle('Half.Eff.Point (mV) - T1L0 - Aug 2010')
halfeff_aug.Draw("colz")

canvas1.cd(3)
h_subtr.SetTitle('Shift Aug - Jul')
h_subtr.GetZaxis().SetRangeUser(-50, 50)
h_subtr.Draw("colz")

canvas1.Print("subtr_T1L0_JulAug2010.pdf")
