
# Compare the alignment Detectors.xml constants of compare2 with each file i nthe listFile
# Plot the value of the constants in global frame and the differences versus z position of the modules.
# the plots are saved in SensConstDiff_wrt....ps
#
# to run
# python CompareDetectorXml.py
# (the parameters should changed by hand in this file, look for 'User Options')
#

import os
from os.path import join, getsize
from ROOT import TFile, TProfile, TF1, TMath, TH1F, TCanvas,TPostScript,TGraphErrors
from ROOT import gDirectory
from math import fabs
from os   import path
import sys
from ROOT import *
from ROOT import gROOT, gRandom, gStyle
from ROOT import TCanvas, TF1, TH1F, TH2F, TGLabel

pl01 = TLatex()
pl01.SetTextSize(0.06) 
pl01.SetTextColor(16)
gROOT.Reset()
gROOT.SetStyle("Pub");
gStyle.SetPadColor(10)
gStyle.SetGridColor(16)
gStyle.SetFrameFillColor(10)
gStyle.SetFillColor(10)
gStyle.SetTitleFillColor(10)
gStyle.SetTitleBorderSize(0)
gStyle.SetCanvasColor(10)
gStyle.SetStatColor(10);

gStyle.SetLabelSize(0.08,"xyz");
gStyle.SetTitleSize(0.08,"xyz");
gStyle.SetFrameLineWidth(1);
gStyle.SetHistLineWidth(1);
gStyle.SetNdivisions(510, "x");
gStyle.SetNdivisions(508, "y");
gStyle.SetNdivisions(8, "z");
gStyle.SetTickLength(0.03, "x");
gStyle.SetTickLength(0.03, "y");
gStyle.SetTickLength(0.03, "z");
gStyle.SetFrameBorderSize(1);
gStyle.SetLegendBorderSize(1);
gStyle.SetCanvasBorderSize(1);
gStyle.SetPadBorderSize(1);
gStyle.SetStatBorderSize(1);
gStyle.SetTitleBorderSize(1);
gStyle.SetLabelOffset(0.005, "x");
gStyle.SetLabelOffset(0.005, "y");
gStyle.SetLabelOffset(0.005, "z");
gStyle.SetBarWidth(0.1);
gStyle.SetLineWidth(1);
gStyle.SetTitleOffset(1, "x");
gStyle.SetTitleOffset(0.8, "y");
gStyle.SetTitleOffset(1, "z");
gStyle.SetOptStat(0)


# User Options
listFiles=["/afs/cern.ch/user/s/siborghi/cmtuser/Brunel_v37r1/Alignment/Escher/options/RunKalmSens61/Iter9/xml/Conditions/Velo/Alignment/Detectors.xml"]
compare2="Metrology"
fnamein2="/afs/cern.ch/user/s/siborghi/cmtuser/Alignment_v4r6/Alignment/Escher/scripts/AlignXml/DetectorsMetrology.xml"

psfile="SensConstDiff_wrt"+compare2+".ps"
ps = TPostScript(psfile,112);
c1 = TCanvas( 'c1', '', 10, 10, 1200, 900 )
c2 = TCanvas( 'c2', '', 10, 10, 1200, 900 )
c1.Divide(1,3)
c2.Divide(2,3)

counter=0
for fnamein in listFiles:
    counter+=1
    vlabel='_'+str(counter)
    inputname=compare2+vlabel

    grltx =TGraph()
    grlty =TGraph()
    grltz =TGraph()
    grlrx =TGraph()
    grlry =TGraph()
    grlrz =TGraph()
    grrtx =TGraph()
    grrty =TGraph()
    grrtz =TGraph()
    grrrx =TGraph()
    grrry =TGraph()
    grrrz =TGraph()

    gr1ltx =TGraph()
    gr1lty =TGraph()
    gr1ltz =TGraph()
    gr1lrx =TGraph()
    gr1lry =TGraph()
    gr1lrz =TGraph()
    gr1rtx =TGraph()
    gr1rty =TGraph()
    gr1rtz =TGraph()
    gr1rrx =TGraph()
    gr1rry =TGraph()
    gr1rrz =TGraph()

    vd1transxr =[]
    vd1transxl =[]
    vd1transyr =[]
    vd1transyl =[]
    vd1transzr =[]
    vd1transzl =[]
    vd1rotxr =[]
    vd1rotxl =[]
    vd1rotyr =[]
    vd1rotyl =[]
    vd1rotzr =[]
    vd1rotzl =[]

    gr2ltx =TGraph()
    gr2lty =TGraph()
    gr2ltz =TGraph()
    gr2lrx =TGraph()
    gr2lry =TGraph()
    gr2lrz =TGraph()
    gr2rtx =TGraph()
    gr2rty =TGraph()
    gr2rtz =TGraph()
    gr2rrx =TGraph()
    gr2rry =TGraph()
    gr2rrz =TGraph()

    vd2transxr =[]
    vd2transxl =[]
    vd2transyr =[]
    vd2transyl =[]
    vd2transzr =[]
    vd2transzl =[]
    vd2rotxr =[]
    vd2rotxl =[]
    vd2rotyr =[]
    vd2rotyl =[]
    vd2rotzr =[]
    vd2rotzl =[]

    vmodzr=(-159.0,-130.0,-98.0,-70.0,-39.0,-11.0,22.0,50.0,82.0,109.0,140.0,170.0,202.0,230.0,250.0,289.0,451.0,600.0,652.0,700.0,751.0)
    vmodzl=(-175.0,-144.0,-115.0,-84.0,-55.0,-23.0,2.0,35.0,64.0,94.0,124.0,155.0,185.0,216.0,244.0,276.0,434.0,587.0,634.0,685.0,735.0)
    vmodz = []
    il=0
    ir=0
    for i in range(0,42):
        #print i
        if (i%2==0):
            vmodz.append(vmodzl[il])
            il+=1
        else:
            vmodz.append(vmodzr[ir])
            ir+=1
        vd1transxr.append(0.)
        vd1transyr.append(0.)
        vd1transzr.append(0.)
        vd1rotxr.append(0.)
        vd1rotyr.append(0.)
        vd1rotzr.append(0.)
        vd1transxl.append(0.)
        vd1transyl.append(0.)
        vd1transzl.append(0.)
        vd1rotxl.append(0.)
        vd1rotyl.append(0.) 
        vd1rotzl.append(0.)
        vd2transxr.append(0.)
        vd2transyr.append(0.)
        vd2transzr.append(0.)
        vd2rotxr.append(0.)
        vd2rotyr.append(0.)
        vd2rotzr.append(0.)
        vd2transxl.append(0.)
        vd2transyl.append(0.)
        vd2transzl.append(0.)
        vd2rotxl.append(0.)
        vd2rotyl.append(0.)
        vd2rotzl.append(0.)


    fin=open(fnamein,'r')
    for line in fin.readlines():
        if ( (line.find('Detector')>0) and (line.find('PU')<0) ):
            tx =[0,0]
            ty =[0,0]
            tz =[0,0]
            rx =[0,0]
            ry =[0,0]
            rz =[0,0]
            #Determine module number and sensor 00 or 01
            split1     = line.split('Detector')
            split2     = split1[1].split('-')
            nmod  = int(split2[0])
            split3     = split2[1].split('"')
            nsens = int(split3[0])
            #print nmod
            #print nsens
            #extract translation and rotation
            split4     = split1[1].split('>')
            split5     = split4[2].split('<')
            splitTrans = split5[0].split()
            #print splitTrans
            tx[nsens] = float(splitTrans[0])
            ty[nsens] = float(splitTrans[1])
            tz[nsens] = float(splitTrans[2])
            split6     = split4[4].split('<')
            splitRot   = split6[0].split()
            rx[nsens] = float(splitRot[0])
            ry[nsens] = float(splitRot[1])
            rz[nsens] = float(splitRot[2])
            #print splitRot
            if ((abs(tx[0])>1e-6) or (abs(ty[0])>1e-6) or (abs(tz[0])>1e-6) or (abs(rx[0])>1e-6) or (abs(ry[0])>1e-6) or (abs(rz[0])>1e-6)):
                transx = tx[0]*1000.
                transy = ty[0]*1000.
                transz = tz[0]*1000.
                rotx   = rx[0]*1000000.
                roty   = ry[0]*1000000.
                rotz   = rz[0]*1000000.
                #Modules pari (even) are on left side
                if (nmod%2==0):
                    #sign correction to be in global frame
                    sign=-1
                    if (nmod%4==0):
                        sign=1
                    n=gr1ltx.GetN()
                    gr1ltx.SetPoint(n,vmodz[nmod],transx)
                    n=gr1lty.GetN()
                    gr1lty.SetPoint(n,vmodz[nmod],sign * transy)
                    n=gr1ltz.GetN()
                    gr1ltz.SetPoint(n,vmodz[nmod],sign * transz)
                    n=gr1lrx.GetN()
                    gr1lrx.SetPoint(n,vmodz[nmod],rotx)
                    n=gr1lry.GetN()
                    gr1lry.SetPoint(n,vmodz[nmod],sign * roty)
                    n=gr1lrz.GetN()
                    gr1lrz.SetPoint(n,vmodz[nmod],sign * rotz)
                    vd1transxl[nmod] = transx
                    vd1rotxl[nmod] = rotx
                    stringa=str(nmod)+" "+str(sign)
                    #print stringa
                    #sign=1
                    vd1transyl[nmod] = sign * transy
                    vd1transzl[nmod] = sign * transz
                    vd1rotyl[nmod] = sign * roty
                    vd1rotzl[nmod] = sign * rotz
                else:
                    sign=-1
                    if ((nmod+1)%4==0):
                        sign=1
                    n=gr1rtx.GetN()
                    gr1rtx.SetPoint(n,vmodz[nmod],-transx)
                    n=gr1rty.GetN()
                    gr1rty.SetPoint(n,vmodz[nmod],sign * transy)
                    n=gr1rtz.GetN()
                    gr1rtz.SetPoint(n,vmodz[nmod],-sign * transz)
                    n=gr1rrx.GetN()
                    gr1rrx.SetPoint(n,vmodz[nmod],-rotx)
                    n=gr1rry.GetN()
                    gr1rry.SetPoint(n,vmodz[nmod],sign * roty)
                    n=gr1rrz.GetN()
                    gr1rrz.SetPoint(n,vmodz[nmod],-sign * rotz)
                    vd1transxr[nmod] = -transx
                    vd1rotxr[nmod] = -rotx
                    #sign correction to be in global frame
                    stringa=str(nmod)+" "+str(sign)
                    #print stringa
                    #sign=1
                    vd1transyr[nmod] = sign * transy
                    vd1transzr[nmod] = -sign * transz
                    vd1rotyr[nmod] = sign * roty
                    vd1rotzr[nmod] = -sign * rotz

            if ((abs(tx[1]>1e-6)) or (abs(ty[1])>1e-6) or (abs(tz[1])>1e-6) or (abs(rx[1])>1e-6) or (abs(ry[1])>1e-6) or (abs(rz[1])>1e-6)):
                transx = tx[1]*1000.
                transy = ty[1]*1000.
                transz = tz[1]*1000.
                rotx   = rx[1]*1000000.
                roty   = ry[1]*1000000.
                rotz   = rz[1]*1000000.
                #Modules pari (even) are on left side
                if (nmod%2==0):
                   #sign correction to be in global frame
                    sign=-1
                    if (nmod%4==0):
                        sign=1
                    n=gr1ltx.GetN()
                    gr1ltx.SetPoint(n,vmodz[nmod],transx)
                    n=gr1lty.GetN()
                    gr1lty.SetPoint(n,vmodz[nmod],sign * transy)
                    n=gr1ltz.GetN()
                    gr1ltz.SetPoint(n,vmodz[nmod],sign * transz)
                    n=gr1lrx.GetN()
                    gr1lrx.SetPoint(n,vmodz[nmod],rotx)
                    n=gr1lry.GetN()
                    gr1lry.SetPoint(n,vmodz[nmod],sign * roty)
                    n=gr1lrz.GetN()
                    gr1lrz.SetPoint(n,vmodz[nmod],sign * rotz)
                    vd1transxl[nmod] = transx
                    vd1rotxl[nmod] = rotx
                    stringa=str(nmod)+" "+str(sign)
                    #print stringa
                    #sign=1
                    vd1transyl[nmod] = sign * transy
                    vd1transzl[nmod] = sign * transz
                    vd1rotyl[nmod] = sign * roty
                    vd1rotzl[nmod] = sign * rotz
                else:
                    #print nmod
                    #print nsens
                    #sign correction to be in global frame
                    sign=-1
                    if ((nmod+1)%4==0):
                        sign=1
                    n=gr1rtx.GetN()
                    gr1rtx.SetPoint(n,vmodz[nmod],-transx)
                    n=gr1rty.GetN()
                    gr1rty.SetPoint(n,vmodz[nmod],sign * transy)
                    n=gr1rtz.GetN()
                    gr1rtz.SetPoint(n,vmodz[nmod],-sign * transz)
                    n=gr1rrx.GetN()
                    gr1rrx.SetPoint(n,vmodz[nmod],-rotx)
                    n=gr1rry.GetN()
                    gr1rry.SetPoint(n,vmodz[nmod],sign * roty)
                    n=gr1rrz.GetN()
                    gr1rrz.SetPoint(n,vmodz[nmod],-sign * rotz)
                    vd1transxr[nmod] = -transx
                    vd1rotxr[nmod] = -rotx
                    stringa=str(nmod)+" "+str(sign)
                    #print stringa
                    #sign=1
                    vd1transyr[nmod] = sign * transy
                    vd1transzr[nmod] = -sign * transz
                    vd1rotyr[nmod] = sign * roty
                    vd1rotzr[nmod] = -sign * rotz


    fin=open(fnamein2,'r')
    for line in fin.readlines():
        if ( (line.find('Detector')>0) and (line.find('PU')<0) ):
            tx =[0,0]
            ty =[0,0]
            tz =[0,0]
            rx =[0,0]
            ry =[0,0]
            rz =[0,0]
            #Determine module number and sensor 00 or 01
            split1     = line.split('Detector')
            split2     = split1[1].split('-')
            nmod  = int(split2[0])
            split3     = split2[1].split('"')
            nsens = int(split3[0])
            #print nmod
            #print nsens
            #extract translation and rotation
            split4     = split1[1].split('>')
            split5     = split4[2].split('<')
            splitTrans = split5[0].split()
            #print splitTrans
            tx[nsens] = float(splitTrans[0])
            ty[nsens] = float(splitTrans[1])
            tz[nsens] = float(splitTrans[2])
            split6     = split4[4].split('<')
            splitRot   = split6[0].split()
            rx[nsens] = float(splitRot[0])
            ry[nsens] = float(splitRot[1])
            rz[nsens] = float(splitRot[2])
            #print splitRot
            if ((abs(tx[0])>1e-6) or (abs(ty[0])>1e-6) or (abs(tz[0])>1e-6) or (abs(rx[0])>1e-6) or (abs(ry[0])>1e-6) or (abs(rz[0])>1e-6)):
                transx = tx[0]*1000.
                transy = ty[0]*1000.
                transz = tz[0]*1000.
                rotx   = rx[0]*1000000.
                roty   = ry[0]*1000000.
                rotz   = rz[0]*1000000.
                #Modules pari (even) are on left side
                if (nmod%2==0):
                    #sign correction to be in global frame
                    sign=-1
                    if (nmod%4==0):
                        sign=1
                    n=gr2ltx.GetN()
                    gr2ltx.SetPoint(n,vmodz[nmod],transx)
                    n=gr2lty.GetN()
                    gr2lty.SetPoint(n,vmodz[nmod],sign * transy)
                    n=gr2ltz.GetN()
                    gr2ltz.SetPoint(n,vmodz[nmod],sign * transz)
                    n=gr2lrx.GetN()
                    gr2lrx.SetPoint(n,vmodz[nmod],rotx)
                    n=gr2lry.GetN()
                    gr2lry.SetPoint(n,vmodz[nmod],sign * roty)
                    n=gr2lrz.GetN()
                    gr2lrz.SetPoint(n,vmodz[nmod],sign * rotz)
                    vd2transxl[nmod] = transx
                    vd2rotxl[nmod] = rotx

                    vd2transyl[nmod] = sign * transy
                    vd2transzl[nmod] = sign * transz
                    vd2rotyl[nmod] = sign * roty
                    vd2rotzl[nmod] = sign * rotz
                else:
                    #sign correction to be in global frame
                    sign=-1
                    if ((nmod+1)%4==0):
                        sign=1
                    n=gr2rtx.GetN()
                    gr2rtx.SetPoint(n,vmodz[nmod],-transx)
                    n=gr2rty.GetN()
                    gr2rty.SetPoint(n,vmodz[nmod],sign * transy)
                    n=gr2rtz.GetN()
                    gr2rtz.SetPoint(n,vmodz[nmod],-sign * transz)
                    n=gr2rrx.GetN()
                    gr2rrx.SetPoint(n,vmodz[nmod],-rotx)
                    n=gr2rry.GetN()
                    gr2rry.SetPoint(n,vmodz[nmod],sign * roty)
                    n=gr2rrz.GetN()
                    gr2rrz.SetPoint(n,vmodz[nmod],-sign * rotz)
                    vd2transxr[nmod] = -transx
                    vd2rotxr[nmod] = -rotx
                    vd2transyr[nmod] = sign * transy
                    vd2transzr[nmod] = -sign * transz
                    vd2rotyr[nmod] = sign * roty
                    vd2rotzr[nmod] = -sign * rotz

            if ((abs(tx[1])>1e-6) or (abs(ty[1])>1e-6) or (abs(tz[1])>1e-6) or (abs(rx[1])>1e-6) or (abs(ry[1])>1e-6) or (abs(rz[1])>1e-6)):
                transx = tx[1]*1000.
                transy = ty[1]*1000.
                transz = tz[1]*1000.
                rotx   = rx[1]*1000000.
                roty   = ry[1]*1000000.
                rotz   = rz[1]*1000000.
                #Modules pari (even) are on left side
                if (nmod%2==0):
                    #sign correction to be in global frame
                    sign=-1
                    if (nmod%4==0):
                        sign=1
                    n=gr2ltx.GetN()
                    gr2ltx.SetPoint(n,vmodz[nmod],transx)
                    n=gr2lty.GetN()
                    gr2lty.SetPoint(n,vmodz[nmod],sign * transy)
                    n=gr2ltz.GetN()
                    gr2ltz.SetPoint(n,vmodz[nmod],sign * transz)
                    n=gr2lrx.GetN()
                    gr2lrx.SetPoint(n,vmodz[nmod],rotx)
                    n=gr2lry.GetN()
                    gr2lry.SetPoint(n,vmodz[nmod],sign * roty)
                    n=gr2lrz.GetN()
                    gr2lrz.SetPoint(n,vmodz[nmod],sign * rotz)
                    vd2transxl[nmod] = transx
                    vd2rotxl[nmod] = rotx
                    vd2transyl[nmod] = sign * transy
                    vd2transzl[nmod] = sign * transz
                    vd2rotyl[nmod] = sign * roty
                    vd2rotzl[nmod] = sign * rotz
                else:
                    #sign correction to be in global frame
                    sign=-1
                    if ((nmod+1)%4==0):
                        sign=1
                    n=gr2rtx.GetN()
                    gr2rtx.SetPoint(n,vmodz[nmod], -transx)
                    n=gr2rty.GetN()
                    gr2rty.SetPoint(n,vmodz[nmod],sign * transy)
                    n=gr2rtz.GetN()
                    gr2rtz.SetPoint(n,vmodz[nmod],-sign * transz)
                    n=gr2rrx.GetN()
                    gr2rrx.SetPoint(n,vmodz[nmod], -rotx)
                    n=gr2rry.GetN()
                    gr2rry.SetPoint(n,vmodz[nmod],sign * roty)
                    n=gr2rrz.GetN()
                    gr2rrz.SetPoint(n,vmodz[nmod],-sign * rotz)
                    vd2transxr[nmod] = -transx
                    vd2rotxr[nmod] = -rotx
                    vd2transyr[nmod] = sign * transy
                    vd2transzr[nmod] = -sign * transz
                    vd2rotyr[nmod] = sign * roty
                    vd2rotzr[nmod] = -sign * rotz

    for i in range(0,42):
        if (i%2==0):
            n=grltx.GetN()
            grltx.SetPoint(n,vmodz[i], vd1transxl[i]-vd2transxl[i])
            n=grlty.GetN()
            grlty.SetPoint(n,vmodz[i], vd1transyl[i]-vd2transyl[i])
            n=grltz.GetN()
            grltz.SetPoint(n,vmodz[i], vd1transzl[i]-vd2transzl[i])
            n=grlrx.GetN()
            grlrx.SetPoint(n,vmodz[i], vd1rotxl[i]-vd2rotxl[i])
            n=grlry.GetN()
            grlry.SetPoint(n,vmodz[i], vd1rotyl[i]-vd2rotyl[i])
            n=grlrz.GetN()
            grlrz.SetPoint(n,vmodz[i], vd1rotzl[i]-vd2rotzl[i])
        else:
            n=grrtx.GetN()
            grrtx.SetPoint(n,vmodz[i], vd1transxr[i]-vd2transxr[i])
            n=grrty.GetN()
            grrty.SetPoint(n,vmodz[i], vd1transyr[i]-vd2transyr[i])
            n=grrtz.GetN()
            grrtz.SetPoint(n,vmodz[i], vd1transzr[i]-vd2transzr[i])
            n=grrrx.GetN()
            grrrx.SetPoint(n,vmodz[i], vd1rotxr[i]-vd2rotxr[i])
            n=grrry.GetN()
            grrry.SetPoint(n,vmodz[i], vd1rotyr[i]-vd2rotyr[i])
            n=grrrz.GetN()
            grrrz.SetPoint(n,vmodz[i], vd1rotzr[i]-vd2rotzr[i])

    c1.cd(1)
    c1_1.SetGrid();
    gr1rtx.GetXaxis().SetTitle("Mod Z [mm]");
    gr1rtx.GetYaxis().SetTitle("Tx [#mum]");
    gr1rtx.SetMinimum(-50.)
    gr1rtx.SetMaximum(50.)
    gr1rtx.GetXaxis().SetRangeUser(-180.,760.);
    gr1rtx.SetMarkerStyle(4);
    gr1rtx.SetMarkerSize(0.5);
    gr1rtx.SetMarkerColor(4);
    gr1rtx.SetLineColor(4);
    gr1rtx.Draw('ap')
    c1.cd(2)
    c1_2.SetGrid();
    gr1rty.GetXaxis().SetTitle("Mod Z [mm]");
    gr1rty.GetYaxis().SetTitle("Ty [#mum]");
    gr1rty.SetMinimum(-55.)
    gr1rty.SetMaximum(55.)
    gr1rty.GetXaxis().SetRangeUser(-180.,760.);
    gr1rty.SetMarkerStyle(4);
    gr1rty.SetMarkerSize(0.5);
    gr1rty.SetMarkerColor(4);
    gr1rty.SetLineColor(4);
    gr1rty.Draw('ap')
    c1.cd(3)
    c1_3.SetGrid();
    gr1rtz.GetXaxis().SetTitle("Mod Z [mm]");
    gr1rtz.GetYaxis().SetTitle("Tz [#mum]");
    gr1rtz.SetMinimum(-500.)
    gr1rtz.SetMaximum(500.)
    gr1rtz.GetXaxis().SetRangeUser(-180.,760.);
    gr1rtz.SetMarkerStyle(4);
    gr1rtz.SetMarkerSize(0.5);
    gr1rtz.SetMarkerColor(4);
    gr1rtz.SetLineColor(4);
    gr1rtz.Draw('ap')

    c1.cd(1)
    c1_1.SetGrid();
    gr1rrx.GetXaxis().SetTitle("Mod Z [mm]");
    gr1rrx.GetYaxis().SetTitle("Rx [#murad]");
    gr1rrx.SetMinimum(-500.)
    gr1rrx.SetMaximum(500.)
    gr1rrx.GetXaxis().SetRangeUser(-180.,760.);
    gr1rrx.SetMarkerStyle(4);
    gr1rrx.SetMarkerSize(0.5);
    gr1rrx.SetMarkerColor(4);
    gr1rrx.SetLineColor(4);
    gr1rrx.Draw('ap')
    c1.cd(2)
    c1_2.SetGrid();
    gr1rry.GetXaxis().SetTitle("Mod Z [mm]");
    gr1rry.GetYaxis().SetTitle("Ry [#murad]");
    gr1rry.SetMinimum(-6000.)
    gr1rry.SetMaximum(6000.)
    gr1rry.GetXaxis().SetRangeUser(-180.,760.);
    gr1rry.SetMarkerStyle(4);
    gr1rry.SetMarkerSize(0.5);
    gr1rry.SetMarkerColor(4);
    gr1rry.SetLineColor(4);
    gr1rry.Draw('ap')
    c1.cd(3)
    c1_3.SetGrid();
    gr1rrz.GetXaxis().SetTitle("Mod Z [mm]");
    gr1rrz.GetYaxis().SetTitle("Rz [#murad]");
    gr1rrz.SetMinimum(-1000.)
    gr1rrz.SetMaximum(1000.)
    gr1rrz.GetXaxis().SetRangeUser(-180.,760.);
    gr1rrz.SetMarkerStyle(4);
    gr1rrz.SetMarkerSize(0.5);
    gr1rrz.SetMarkerColor(4);
    gr1rrz.SetLineColor(4);
    gr1rrz.Draw('ap')


    c1.cd(1)
    c1_1.SetGrid();
    gr2rtx.GetXaxis().SetTitle("Mod Z [mm]");
    gr2rtx.GetYaxis().SetTitle("Tx [#mum]");
    gr2rtx.SetMinimum(-50.)
    gr2rtx.SetMaximum(50.)
    gr2rtx.GetXaxis().SetRangeUser(-180.,760.);
    gr2rtx.SetMarkerStyle(26);
    gr2rtx.SetMarkerSize(0.5);
    gr2rtx.SetMarkerColor(2);
    gr2rtx.SetLineColor(2);
    gr2rtx.Draw('ap')
    c1.cd(2)
    c1_2.SetGrid();
    gr2rty.GetXaxis().SetTitle("Mod Z [mm]");
    gr2rty.GetYaxis().SetTitle("Ty [#mum]");
    gr2rty.SetMinimum(-55.)
    gr2rty.SetMaximum(55.)
    gr2rty.GetXaxis().SetRangeUser(-180.,760.);
    gr2rty.SetMarkerStyle(26);
    gr2rty.SetMarkerSize(0.5);
    gr2rty.SetMarkerColor(2);
    gr2rty.SetLineColor(2);
    gr2rty.Draw('ap')
    c1.cd(3)
    c1_3.SetGrid();
    gr2rtz.GetXaxis().SetTitle("Mod Z [mm]");
    gr2rtz.GetYaxis().SetTitle("Tz [#mum]");
    gr2rtz.SetMinimum(-500.)
    gr2rtz.SetMaximum(500.)
    gr2rtz.GetXaxis().SetRangeUser(-180.,760.);
    gr2rtz.SetMarkerStyle(26);
    gr2rtz.SetMarkerSize(0.5);
    gr2rtz.SetMarkerColor(2);
    gr2rtz.SetLineColor(2);
    gr2rtz.Draw('ap')
    c1.cd(1)
    c1_1.SetGrid();
    gr2rrx.GetXaxis().SetTitle("Mod Z [mm]");
    gr2rrx.GetYaxis().SetTitle("Rx [#murad]");
    gr2rrx.SetMinimum(-500.)
    gr2rrx.SetMaximum(500.)
    gr2rrx.GetXaxis().SetRangeUser(-180.,760.);
    gr2rrx.SetMarkerStyle(26);
    gr2rrx.SetMarkerSize(0.5);
    gr2rrx.SetMarkerColor(2);
    gr2rrx.SetLineColor(2);
    gr2rrx.Draw('ap')
    c1.cd(2)
    c1_2.SetGrid();
    gr2rry.GetXaxis().SetTitle("Mod Z [mm]");
    gr2rry.GetYaxis().SetTitle("Ry [#murad]");
    gr2rry.SetMinimum(-6000.)
    gr2rry.SetMaximum(6000.)
    gr2rry.GetXaxis().SetRangeUser(-180.,760.);
    gr2rry.SetMarkerStyle(26);
    gr2rry.SetMarkerSize(0.5);
    gr2rry.SetMarkerColor(2);
    gr2rry.SetLineColor(2);
    gr2rry.Draw('ap')
    c1.cd(3)
    c1_3.SetGrid();
    gr2rrz.GetXaxis().SetTitle("Mod Z [mm]");
    gr2rrz.GetYaxis().SetTitle("Rz [#murad]");
    gr2rrz.SetMinimum(-1000.)
    gr2rrz.SetMaximum(1000.)
    gr2rrz.GetXaxis().SetRangeUser(-180.,760.);
    gr2rrz.SetMarkerStyle(26);
    gr2rrz.SetMarkerSize(0.5);
    gr2rrz.SetMarkerColor(2);
    gr2rrz.SetLineColor(2);
    gr2rrz.Draw('ap')




    c1_1.SetGrid();
    gr1ltx.GetXaxis().SetTitle("Mod Z [mm]");
    gr1ltx.GetYaxis().SetTitle("Tx [#mum]");
    gr1ltx.SetMinimum(-50.)
    gr1ltx.SetMaximum(50.)
    gr1ltx.GetXaxis().SetRangeUser(-180.,760.);
    gr1ltx.SetMarkerStyle(4);
    gr1ltx.SetMarkerSize(0.5);
    gr1ltx.SetMarkerColor(4);
    gr1ltx.SetLineColor(4);
    gr1ltx.Draw('ap')
    c1.cd(2)
    c1_2.SetGrid();
    gr1lty.GetXaxis().SetTitle("Mod Z [mm]");
    gr1lty.GetYaxis().SetTitle("Ty [#mum]");
    gr1lty.SetMinimum(-55.)
    gr1lty.SetMaximum(55.)
    gr1lty.GetXaxis().SetRangeUser(-180.,760.);
    gr1lty.SetMarkerStyle(4);
    gr1lty.SetMarkerSize(0.5);
    gr1lty.SetMarkerColor(4);
    gr1lty.SetLineColor(4);
    gr1lty.Draw('ap')
    c1.cd(3)
    c1_3.SetGrid();
    gr1ltz.GetXaxis().SetTitle("Mod Z [mm]");
    gr1ltz.GetYaxis().SetTitle("Tz [#mum]");
    gr1ltz.SetMinimum(-500.)
    gr1ltz.SetMaximum(500.)
    gr1ltz.GetXaxis().SetRangeUser(-180.,760.);
    gr1ltz.SetMarkerStyle(4);
    gr1ltz.SetMarkerSize(0.5);
    gr1ltz.SetMarkerColor(4);
    gr1ltz.SetLineColor(4);
    gr1ltz.Draw('ap')
    c1.cd(1)
    c1_1.SetGrid();
    gr1lrx.GetXaxis().SetTitle("Mod Z [mm]");
    gr1lrx.GetYaxis().SetTitle("Rx [#murad]");
    gr1lrx.SetMinimum(-500.)
    gr1lrx.SetMaximum(500.)
    gr1lrx.GetXaxis().SetRangeUser(-180.,760.);
    gr1lrx.SetMarkerStyle(4);
    gr1lrx.SetMarkerSize(0.5);
    gr1lrx.SetMarkerColor(4);
    gr1lrx.SetLineColor(4);
    gr1lrx.Draw('ap')
    c1.cd(2)
    c1_2.SetGrid();
    gr1lry.GetXaxis().SetTitle("Mod Z [mm]");
    gr1lry.GetYaxis().SetTitle("Ry [#murad]");
    gr1lry.SetMinimum(-2000.)
    gr1lry.SetMaximum(2000.)
    gr1lry.GetXaxis().SetRangeUser(-180.,760.);
    gr1lry.SetMarkerStyle(4);
    gr1lry.SetMarkerSize(0.5);
    gr1lry.SetMarkerColor(4);
    gr1lry.SetLineColor(4);
    gr1lry.Draw('ap')
    c1.cd(3)
    c1_3.SetGrid();
    gr1lrz.GetXaxis().SetTitle("Mod Z [mm]");
    gr1lrz.GetYaxis().SetTitle("Rz [#murad]");
    gr1lrz.SetMinimum(-1000.)
    gr1lrz.SetMaximum(1000.)
    gr1lrz.GetXaxis().SetRangeUser(-180.,760.);
    gr1lrz.SetMarkerStyle(4);
    gr1lrz.SetMarkerSize(0.5);
    gr1lrz.SetMarkerColor(4);
    gr1lrz.SetLineColor(4);
    gr1lrz.Draw('ap')


    c1.cd(1)
    c1_1.SetGrid();
    gr2ltx.GetXaxis().SetTitle("Mod Z [mm]");
    gr2ltx.GetYaxis().SetTitle("Tx [#mum]");
    gr2ltx.SetMinimum(-50.)
    gr2ltx.SetMaximum(50.)
    gr2ltx.GetXaxis().SetRangeUser(-180.,760.);
    gr2ltx.SetMarkerStyle(26);
    gr2ltx.SetMarkerSize(0.5);
    gr2ltx.SetMarkerColor(2);
    gr2ltx.SetLineColor(2);
    gr2ltx.Draw('ap')
    c1.cd(2)
    c1_2.SetGrid();
    gr2lty.GetXaxis().SetTitle("Mod Z [mm]");
    gr2lty.GetYaxis().SetTitle("Ty [#mum]");
    gr2lty.SetMinimum(-55.)
    gr2lty.SetMaximum(55.)
    gr2lty.GetXaxis().SetRangeUser(-180.,760.);
    gr2lty.SetMarkerStyle(26);
    gr2lty.SetMarkerSize(0.5);
    gr2lty.SetMarkerColor(2);
    gr2lty.SetLineColor(2);
    gr2lty.Draw('ap')
    c1.cd(3)
    c1_3.SetGrid();
    gr2ltz.GetXaxis().SetTitle("Mod Z [mm]");
    gr2ltz.GetYaxis().SetTitle("Tz [#mum]");
    gr2ltz.SetMinimum(-500.)
    gr2ltz.SetMaximum(500.)
    gr2ltz.GetXaxis().SetRangeUser(-180.,760.);
    gr2ltz.SetMarkerStyle(26);
    gr2ltz.SetMarkerSize(0.5);
    gr2ltz.SetMarkerColor(2);
    gr2ltz.SetLineColor(2);
    gr2ltz.Draw('ap')
    c1.cd(1)
    c1_1.SetGrid();
    gr2lrx.GetXaxis().SetTitle("Mod Z [mm]");
    gr2lrx.GetYaxis().SetTitle("Rx [#murad]");
    gr2lrx.SetMinimum(-500.)
    gr2lrx.SetMaximum(500.)
    gr2lrx.GetXaxis().SetRangeUser(-180.,760.);
    gr2lrx.SetMarkerStyle(26);
    gr2lrx.SetMarkerSize(0.5);
    gr2lrx.SetMarkerColor(2);
    gr2lrx.SetLineColor(2);
    gr2lrx.Draw('ap')
    c1.cd(2)
    c1_2.SetGrid();
    gr2lry.GetXaxis().SetTitle("Mod Z [mm]");
    gr2lry.GetYaxis().SetTitle("Ry [#murad]");
    gr2lry.SetMinimum(-2000.)
    gr2lry.SetMaximum(2000.)
    gr2lry.GetXaxis().SetRangeUser(-180.,760.);
    gr2lry.SetMarkerStyle(26);
    gr2lry.SetMarkerSize(0.5);
    gr2lry.SetMarkerColor(2);
    gr2lry.SetLineColor(2);
    gr2lry.Draw('ap')
    c1.cd(3)
    c1_3.SetGrid();
    gr2lrz.GetXaxis().SetTitle("Mod Z [mm]");
    gr2lrz.GetYaxis().SetTitle("Rz [#murad]");
    gr2lrz.SetMinimum(-1000.)
    gr2lrz.SetMaximum(1000.)
    gr2lrz.GetXaxis().SetRangeUser(-180.,760.);
    gr2lrz.SetMarkerStyle(26);
    gr2lrz.SetMarkerSize(0.5);
    gr2lrz.SetMarkerColor(2);
    gr2lrz.SetLineColor(2);
    gr2lrz.Draw('ap')



    graphRight="Phi Sensors in Right side           --  Align "+inputname
    graphLeft="Phi Sensors in Left side"

    c2.cd(1)
    if (counter==1):
        ps.NewPage();
    c2_1.SetGrid();
    gr1ltx.SetTitle(graphLeft)
    gr1ltx.Draw('ap')
    gr2ltx.Draw('p')    
    c2.cd(3)
    c2_3.SetGrid();
    gr1lty.Draw('ap')     
    gr2lty.Draw('p')     
    c2.cd(5)
    c2_5.SetGrid();
    gr1ltz.Draw('ap')
    gr2ltz.Draw('p')
    #pdffile="Trans12SampleL"+inputname+"_sens.pdf"
    #c2.SaveAs(pdffile)
    c2.cd(2)
    c2_2.SetGrid();
    gr1rtx.SetTitle(graphRight)
    gr1rtx.Draw('ap')
    gr2rtx.Draw('p')
    leg =TLegend(0.7,0.8,1.0,1);
    alignvers="Align v "+vlabel
    leg.AddEntry(gr1rtx,alignvers,"p");
    leg.AddEntry(gr2rtx,compare2,"p");
    leg.Draw()
    c2.cd(4)
    c2_4.SetGrid();
    gr1rty.Draw('ap')     
    gr2rty.Draw('p')     
    c2.cd(6)
    c2_6.SetGrid();
    gr1rtz.Draw('ap')
    gr2rtz.Draw('p')
    #pdffile="Trans12SampleR"+inputname+"_sens.pdf"
    #c2.SaveAs(pdffile)
    c2.Update()
    ps.NewPage()

    c2.cd(1)
    c2_1.SetGrid();
    gr1lrx.SetTitle(graphLeft)
    gr1lrx.Draw('ap')
    gr2lrx.Draw('p')
    c2.cd(3)
    c2_3.SetGrid();
    gr1lry.Draw('ap')     
    gr2lry.Draw('p')     
    c2.cd(5)
    c2_5.SetGrid();
    gr1lrz.Draw('ap')
    gr2lrz.Draw('p')
    #pdffile="Rot12SampleL"+inputname+"_sens.pdf"
    #c2.SaveAs(pdffile)
    c2.cd(2)
    c2_2.SetGrid();
    gr1rrx.SetTitle(graphRight)
    gr1rrx.Draw('ap')
    gr2rrx.Draw('p')
    leg =TLegend(0.7,0.8,1.0,1);
    alignvers="Align v "+vlabel
    leg.AddEntry(gr1rrx,alignvers,"p");
    leg.AddEntry(gr2rrx,compare2,"p");
    leg.Draw()
    c2.cd(4)
    c2_4.SetGrid();
    gr1rry.Draw('ap')     
    gr2rry.Draw('p')     
    c2.cd(6)
    c2_6.SetGrid();
    gr1rrz.Draw('ap')
    gr2rrz.Draw('p')
    #pdffile="Rot12SampleR"+inputname+"_sens.pdf"
    #c2.SaveAs(pdffile)
    c2.Update()
    ps.NewPage()


    #DIFFERENCE
    c2.cd(1)
    c2_1.SetGrid();
    grltx.SetTitle(graphLeft)
    grltx.GetXaxis().SetTitle("Mod Z [mm]");
    grltx.GetYaxis().SetTitle("#DeltaTx [#mum]");
    grltx.SetMinimum(-45.)
    grltx.SetMaximum(45.)
    grltx.GetXaxis().SetRangeUser(-180.,760.);
    grltx.SetMarkerStyle(8);
    grltx.SetMarkerSize(0.5);
    grltx.SetMarkerColor(1);
    grltx.SetLineColor(1);
    grltx.Draw('ap')
    c2.cd(3)
    c2_3.SetGrid();
    grlty.GetXaxis().SetTitle("Mod Z [mm]");
    grlty.GetYaxis().SetTitle("#DeltaTy [#mum]");
    grlty.SetMinimum(-45.)
    grlty.SetMaximum(45.)
    grlty.GetXaxis().SetRangeUser(-180.,760.);
    grlty.SetMarkerStyle(8);
    grlty.SetMarkerSize(0.5);
    grlty.SetMarkerColor(1);
    grlty.SetLineColor(1);
    grlty.Draw('ap')
    c2.cd(5)
    c2_5.SetGrid();
    grltz.GetXaxis().SetTitle("Mod Z [mm]");
    grltz.GetYaxis().SetTitle("#DeltaTz [#mum]");
    grltz.SetMinimum(-500.)
    grltz.SetMaximum(500.)
    grltz.GetXaxis().SetRangeUser(-180.,760.);
    grltz.SetMarkerStyle(8);
    grltz.SetMarkerSize(0.5);
    grltz.SetMarkerColor(1);
    grltz.SetLineColor(1);
    grltz.Draw('ap')
    #pdffile="TransSampleL"+inputname+"_sens.pdf"
    #c2.SaveAs(pdffile)
    c2.cd(2)
    c2_2.SetGrid();
    grrtx.SetTitle(graphRight)
    grrtx.GetXaxis().SetTitle("Mod Z [mm]");
    grrtx.GetYaxis().SetTitle("#DeltaTx [#mum]");
    grrtx.SetMinimum(-45.)
    grrtx.SetMaximum(45.)
    grrtx.GetXaxis().SetRangeUser(-180.,760.);
    grrtx.SetMarkerStyle(8);
    grrtx.SetMarkerSize(0.5);
    grrtx.SetMarkerColor(1);
    grrtx.SetLineColor(1);
    grrtx.Draw('ap')
    c2.cd(4)
    c2_4.SetGrid();
    grrty.GetXaxis().SetTitle("Mod Z [mm]");
    grrty.GetYaxis().SetTitle("#DeltaTy [#mum]");
    grrty.SetMinimum(-45.)
    grrty.SetMaximum(45.)
    grrty.GetXaxis().SetRangeUser(-180.,760.);
    grrty.SetMarkerStyle(8);
    grrty.SetMarkerSize(0.5);
    grrty.SetMarkerColor(1);
    grrty.SetLineColor(1);
    grrty.Draw('ap')
    c2.cd(6)
    c2_6.SetGrid();
    grrtz.GetXaxis().SetTitle("Mod Z [mm]");
    grrtz.GetYaxis().SetTitle("#DeltaTz [#mum]");
    grrtz.SetMinimum(-500.)
    grrtz.SetMaximum(500.)
    grrtz.GetXaxis().SetRangeUser(-180.,760.);
    grrtz.SetMarkerStyle(8);
    grrtz.SetMarkerSize(0.5);
    grrtz.SetMarkerColor(1);
    grrtz.SetLineColor(1);
    grrtz.Draw('ap')
    #pdffile="TransSampleR"+inputname+"_sens.pdf"
    #c2.SaveAs(pdffile)
    c2.Update()
    ps.NewPage()

    c2.cd(1)
    c2_1.SetGrid();
    grlrx.SetTitle(graphLeft)
    grlrx.GetXaxis().SetTitle("Mod Z [mm]");
    grlrx.GetYaxis().SetTitle("#DeltaRx [#murad]");
    grlrx.SetMinimum(-100.)
    grlrx.SetMaximum(100.)
    grlrx.GetXaxis().SetRangeUser(-180.,760.);
    grlrx.SetMarkerStyle(8);
    grlrx.SetMarkerSize(0.5);
    grlrx.SetMarkerColor(1);
    grlrx.SetLineColor(1);
    grlrx.Draw('ap')
    c2.cd(3)
    c2_3.SetGrid();
    grlry.GetXaxis().SetTitle("Mod Z [mm]");
    grlry.GetYaxis().SetTitle("#DeltaRy [#murad]");
    grlry.SetMinimum(-2000.)
    grlry.SetMaximum(2000.)
    grlry.GetXaxis().SetRangeUser(-180.,760.);
    grlry.SetMarkerStyle(8);
    grlry.SetMarkerSize(0.5);
    grlry.SetMarkerColor(1);
    grlry.SetLineColor(1);
    grlry.Draw('ap')
    c2.cd(5)
    c2_5.SetGrid();
    grlrz.GetXaxis().SetTitle("Mod Z [mm]");
    grlrz.GetYaxis().SetTitle("#DeltaRz [#murad]");
    grlrz.SetMinimum(-1005.)
    grlrz.SetMaximum(1005.)
    grlrz.GetXaxis().SetRangeUser(-180.,760.);
    grlrz.SetMarkerStyle(8);
    grlrz.SetMarkerSize(0.5);
    grlrz.SetMarkerColor(1);
    grlrz.SetLineColor(1);
    grlrz.Draw('ap')
    #pdffile="RotSampleL"+inputname+"_sens.pdf"
    #c2.SaveAs(pdffile)
    c2.cd(2)
    c2_2.SetGrid();
    grrrx.SetTitle(graphRight)
    grrrx.GetXaxis().SetTitle("Mod Z [mm]");
    grrrx.GetYaxis().SetTitle("#DeltaRx [#murad]");
    grrrx.SetMinimum(-100.)
    grrrx.SetMaximum(100.)
    grrrx.GetXaxis().SetRangeUser(-180.,760.);
    grrrx.SetMarkerStyle(8);
    grrrx.SetMarkerSize(0.5);
    grrrx.SetMarkerColor(1);
    grrrx.SetLineColor(1);
    grrrx.Draw('ap')
    c2.cd(4)
    c2_4.SetGrid();
    grrry.GetXaxis().SetTitle("Mod Z [mm]");
    grrry.GetYaxis().SetTitle("#DeltaRy [#murad]");
    grrry.SetMinimum(-2000.)
    grrry.SetMaximum(2000.)
    grrry.GetXaxis().SetRangeUser(-180.,760.);
    grrry.SetMarkerStyle(8);
    grrry.SetMarkerSize(0.5);
    grrry.SetMarkerColor(1);
    grrry.SetLineColor(1);
    grrry.Draw('ap')
    c2.cd(6)
    c2_6.SetGrid();
    grrrz.GetXaxis().SetTitle("Mod Z [mm]");
    grrrz.GetYaxis().SetTitle("#DeltaRz [#murad]");
    grrrz.SetMinimum(-1005.)
    grrrz.SetMaximum(1005.)
    grrrz.GetXaxis().SetRangeUser(-180.,760.);
    grrrz.SetMarkerStyle(8);
    grrrz.SetMarkerSize(0.5);
    grrrz.SetMarkerColor(1);
    grrrz.SetLineColor(1);
    grrrz.Draw('ap')
    #pdffile="RotSampleR"+inputname+"_sens.pdf"
    #c2.SaveAs(pdffile)
    c2.Update()


ps.Close()
