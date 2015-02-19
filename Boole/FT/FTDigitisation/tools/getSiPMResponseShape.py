#!/bin/env/python

################################################
# Script to create the SiPM Response function
#  from a raw testbeam data file
# by Jacco de Vries [03-11-2014]
###############################################

from ROOT import TH1F, TCanvas, TGraph, TLine, TSpline3, gStyle 
from array import array

gStyle.SetOptStat(0)


def getCSV(filename) :
  f = open(filename,"r")
  # get array of lines of words
  arr = []
  for line in f :
    arr += [ line.split() ]
  # string -> float
  for i, line in enumerate(arr) :
    if(i>0) :
      for j, word in enumerate(line) :
        arr[i][j] = float( word )
  return arr


arr1 = getCSV("Integrator_BX0_10pe.csv")
arr2 = getCSV("Integrator_BX2_10pe.csv")

nsvalue = 214e-3
Tprev  = [ arr1[i][1] for i in range(len(arr1))[1:] ]
Tcurr  = [ arr1[i][3] for i in range(len(arr1))[1:] ]
Tnext  = [ arr2[i][1] for i in range(len(arr2))[1:] ]
Tnxt2  = [ arr2[i][3] for i in range(len(arr2))[1:] ]

Tprev.reverse()
Tcurr.reverse()
Tnext.reverse()
Tnxt2.reverse()

le = len(Tcurr) 
le25ns = int(25. / nsvalue) 

# calculate time-axis points, include shift of spills
xPrev = [ i*nsvalue for i in range( -le25ns, le-le25ns ) ]
xCurr = [ i*nsvalue for i in range(  0, le ) ]
xNext = [ i*nsvalue for i in range(  le25ns, le25ns + le ) ]
xNxt2 = [ i*nsvalue for i in range(  2*le25ns, 2*le25ns + le) ]

grPrev = TGraph(len(xPrev), array("d", xPrev), array("d",Tprev) )
grCurr = TGraph(len(xCurr), array("d", xCurr), array("d",Tcurr) )
grNext = TGraph(len(xNext), array("d", xNext), array("d",Tnext) )
grNxt2 = TGraph(len(xNxt2), array("d", xNxt2), array("d",Tnxt2) )
for i, gr in enumerate([grPrev,grCurr,grNext,grNxt2]) :
  gr.SetMarkerColor(i+6)
  gr.SetLineColor(i+6)
  gr.SetMarkerStyle(5)
  gr.SetMarkerSize(0.5)

c1 = TCanvas("c1","c1",700,500)
h1 = TH1F("h1","pulse shape; time [ns]; value",100,-30,100)
h1.SetMinimum(0.9)
h1.SetMaximum(1.1)
h1.DrawCopy("")

grPrev.Draw("PL")
grCurr.Draw("PL")
grNext.Draw("PL")
grNxt2.Draw("PL")

lines = [TLine(-25,0.9,-25,1.1), TLine( 0,0.9, 0,1.1),
         TLine( 25,0.9, 25,1.1), TLine(50,0.9,50,1.1),
         TLine( 75,0.9, 75,1.1)]
for line in lines: 
  line.Draw("same")

c1.Modified()
c1.Update()


# split up into (non-)overlapping parts
T1 = Tprev[ : le25ns]
T21 = [ Tprev[le25ns + i] for i in range(le-le25ns) ]
T22 = [ Tcurr[i]          for i in range(le-le25ns) ]
T3 = Tcurr[le-le25ns : le25ns]
T41 = [ Tcurr[le25ns + i] for i in range(le-le25ns) ]
T42 = [ Tnext[i]          for i in range(le-le25ns) ]
T5 = Tnext[le-le25ns : le25ns]
T61 = [ Tnext[le25ns + i] for i in range(le-le25ns) ]
T62 = [ Tnxt2[i]          for i in range(le-le25ns) ]
T7 = Tnxt2[le-le25ns : ]

# calculate the average shift in the overlap
avg21 = sum(T21) / float(len(T21))
avg22 = sum(T22) / float(len(T22))
shift2 = avg21 - avg22
avg41 = sum(T41) / float(len(T41))
avg42 = sum(T42) / float(len(T42))
shift4 = avg41 - avg42
avg61 = sum(T61) / float(len(T61))
avg62 = sum(T62) / float(len(T62))
shift6 = avg61 - avg62

# make the shifted arrays (using the mean in overlap regions)
shift = -T1[0]
P1 = [ T1[i]                           + shift  for i in range(len(T1 )) ]
P2 = [ ((T22[i] + shift2) + T21[i])/2. + shift  for i in range(len(T22)) ]
shift += shift2
P3 = [ T3[i]                           + shift  for i in range(len(T3 )) ]
P4 = [ ((T42[i] + shift4) + T41[i])/2. + shift  for i in range(len(T42)) ]
shift += shift4
P5 = [ T5[i]                           + shift  for i in range(len(T5 )) ]
P6 = [ ((T62[i] + shift6) + T61[i])/2. + shift  for i in range(len(T62)) ]
shift += shift6
P7 = [ T7[i]                           + shift  for i in range(len(T7 )) ]
pulseShape = P1 + P2 + P3 + P4 + P5 + P6 + P7



# reverse the shape
for i in range(len(pulseShape)) :
  pulseShape[i] = -pulseShape[i]

# normalize
maxY = max(pulseShape)
for i in range(len(pulseShape)) : pulseShape[i] /= maxY


# make the TGraph object
xlist = [i * nsvalue - 25. for i in range(len(pulseShape)) ] 
gr = TGraph(len(xlist), array("d",xlist), array("d",pulseShape) )

# smooth the function
smoothF = 10
pulseShapeSmooth = [ sum(pulseShape[i*smoothF:i*smoothF+smoothF])/smoothF for i in range(len(pulseShape)/smoothF) ]
xlistSmooth      = [ sum(     xlist[i*smoothF:i*smoothF+smoothF])/smoothF for i in range(len(     xlist)/smoothF) ]
grSmooth = TGraph(len(xlistSmooth), array("d",xlistSmooth), array("d",pulseShapeSmooth) )





c2 = TCanvas("c2","c2",700,500)
h1.SetMinimum(-0.2)
h1.SetMaximum( 1.1)
h1.DrawCopy("")

line = TLine(-30, 0, 100, 0)
line.SetLineColor(4)
line.Draw("same")

gr.Draw("same")
spline = TSpline3("spline", grSmooth)
spline.SetLineColor(2)
spline.Draw("same")

c2.Modified()
c2.Update()







## LOAD the old shape

Tprev = [  3.22188688e-05,   3.22145863e-05,   3.22145863e-05,   3.22145863e-05,
           3.22145863e-05,   3.22145863e-05,   3.22145863e-05,   3.22145863e-05,
           3.22145863e-05,   3.22145863e-05,   3.22145863e-05,   3.22145863e-05,
           3.22145863e-05,   3.22145863e-05,   3.22145863e-05,   3.22145863e-05,
           3.22145863e-05,   3.22145863e-05,   3.22145863e-05,   3.22145863e-05,
           3.22145863e-05,   3.22145863e-05,   3.22145863e-05,   3.22145863e-05,
           3.22145863e-05,   3.22145863e-05]
Tsig =[  2.88048286e-02,   2.76166647e-02,   2.84853231e-02,   2.86509678e-02,
         2.87964850e-02,   2.90245415e-02,   2.92078012e-02,   2.93951976e-02,
         2.95711670e-02,   2.96709382e-02,   2.97279647e-02,   2.96909134e-02,
         2.94095934e-02,   2.89146629e-02,   2.80610778e-02,   2.67112946e-02,
         2.47312816e-02,   2.20768793e-02,   1.86479251e-02,   1.44897527e-02,
         9.82891757e-03,   5.23039367e-03,   1.56929340e-03,  -2.17283810e-04,
         -1.82921036e-05,   6.66654783e-05]
Tnext = \
  [-0.00136548, -0.00089388, -0.0012617,  -0.00137867, -0.00145512, -0.00160822,
   -0.00173438, -0.00187304, -0.00201678, -0.00213902, -0.0022567,  -0.00234672,
   -0.0023399, -0.00226133, -0.00202669, -0.00154593, -0.00067318,  0.00060853,
   0.00250173,  0.00515046,  0.00876429,  0.01327495,  0.01883291,  0.02372474,
   0.02788753,  0.0281931 ]

Tnext2 = \
  [-0.00034101, -0.00025578, -0.00030324, -0.00031672, -0.00032477, -0.00034131,
   -0.00035811, -0.00037248, -0.00038796, -0.00040745, -0.00042273, -0.00044454,
   -0.00046555, -0.00048982, -0.00051403, -0.00054482, -0.00056937, -0.00060258,
   -0.00064924, -0.00068913, -0.00070694, -0.00073142, -0.00099868, -0.00083826,
   -0.00119224, -0.00104176]


Tprev.reverse()
Tsig.reverse()
Tnext.reverse()
Tnext2.reverse()

xSig = [i for i in range(26) ]
xPrev = [i for i in range(-25,1) ]
xNext = [i for i in range(25,51) ]
xNext2 = [i for i in range(50,76) ]

grSig = TGraph(len(xSig), array("d",xSig), array("d",Tsig) )
grPrev = TGraph(len(xPrev), array("d",xPrev), array("d",Tprev) )
grNext = TGraph(len(xNext), array("d",xNext), array("d",Tnext) )
grNext2 = TGraph(len(xNext2), array("d",xNext2), array("d",Tnext2) )

pulseShapeOld = [Tprev[i]-Tprev[0] for i in range(26) ]
pulseShapeOld.extend([Tsig[i]-Tsig[0]+pulseShapeOld[25] for i in range(1,26)])
pulseShapeOld.extend([Tnext[i]-Tnext[0]+pulseShapeOld[50] for i in range(1,26)])
pulseShapeOld.extend([Tnext2[i]-Tnext2[0]+pulseShapeOld[75] for i in range(1,26)])


c3 = TCanvas("c3","c3",700,500)
h1.DrawCopy()
x = [i for i in range(-25,76) ]

maxY = max(pulseShapeOld)
for i in range(len(pulseShapeOld)) : pulseShapeOld[i] /= maxY

gr = TGraph(len(x), array("d",x), array("d",pulseShapeOld) )
gr.Draw("C")





# Draw the new shape on top of the old one

maxOld = pulseShapeOld.index(max(pulseShapeOld))
maxNew = pulseShapeSmooth.index(max(pulseShapeSmooth))
tshift = xlistSmooth[maxNew] - x[maxOld]
tshift -= 4  # manual shift, to make the integrals overlap by eye
print tshift
for i in range(len(xlistSmooth)) :
  xlistSmooth[i] -= tshift

grSmooth2 = TGraph(len(xlistSmooth), array("d",xlistSmooth), array("d",pulseShapeSmooth) )
spline2 = TSpline3("spline2", grSmooth2)
spline2.SetLineColor(2)
spline2.Draw("same")

c3.Modified()
c3.Update()



# print vector for SiPMResponse.cpp
print "SiPM Integrator pulse height for SiPMResponse.cpp:"
for i,entry in enumerate(pulseShapeSmooth):
  if(i%5==0) : print ""
  print "(" + str(entry) + ")",
print ""
print "SiPM Integrator pulse times for SiPMResponse.cpp:"
for i,entry in enumerate(xlistSmooth):
  if(i%5==0) : print ""
  print "(" + str(entry) + ")",
print ""





# Write
#c1.SaveAs("c1.png")
#c2.SaveAs("c2.png")
#c3.SaveAs("c3.png")







