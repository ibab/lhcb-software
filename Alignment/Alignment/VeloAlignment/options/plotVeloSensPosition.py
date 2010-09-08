#
# Draw the position in the global frame of the point (xpos,ypos,0) in local frame for each sensor
# (plots are saved in SensXZPosX...pdf and  SensYZPosX...pdf)
# The position is also written in txt files (RightX..txt and LeftY...txt)
# To compare the sensor position obtained with 2 files one can use the script
# $VELOALIGNMENTROOT/scripts/make_pos_comp_2align.py
#
# to run python plotVeloSensPosition.py
# (the parameters should changed by hand in this file, look for 'User Options')
#
import GaudiPython
import GaudiPython.TupleUtils as TupleUtils
from GaudiPython import gbl

from ROOT import TH1, TH2, TCanvas, Math, TH2F, TLegend

import os
import sys

from ROOT import gROOT, gRandom, gStyle
from ROOT import TCanvas, TF1, TH1F, TH2F
BRUNELOPTS = os.environ[ 'ESCHEROPTS' ]

# User Options
OPTIONS = 'Options_with_align1.py' # options file
label='_align1' # label for pdt and txt files
xpos = 0  # xposition in local frame
ypos = 0  # yposition in local frame

appMgr = GaudiPython.AppMgr( outputlevel = 3, joboptions = OPTIONS )

EVT = appMgr.evtSvc()

SEL=appMgr.evtSel()

det = appMgr.detSvc()

# get some math-stuff
Math = GaudiPython.gbl.ROOT.Math

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

gStyle.SetLabelSize(0.07,"xyz");
gStyle.SetTitleSize(0.07,"xyz");
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

histoRightR = TH2F( "histoRightR", "Right R Sensor origin in global frame",1000, -0.1, 0.1,1000, -0.1, 0.1 )
histoRightPhi = TH2F( "histoRightPhi", "Right Phi Sensor origin in global frame",1000, -0.1, 0.1,1000, -0.1, 0.1 )
histoRightRPhi = TH2F( "histoRightRPhi", "Right R-Phi Sensor origin in global frame",1000, -0.1, 0.1,1000, -0.1, 0.1 )
histoLeftR = TH2F( "histoLeftR", "Left R Sensor origin in global frame",1000, -0.1, 0.1,1000, -0.1, 0.1 )
histoLeftPhi = TH2F( "histoLeftPhi", "Left Phi Sensor origin in global frame",1000, -0.1, 0.1,1000, -0.1, 0.1 )
histoLeftRPhi = TH2F( "histoLeftRPhi", "Left R-Phi Sensor origin in global frame",1000, -0.1, 0.1,1000, -0.1, 0.1 )

histoRightXZ = TH2F( "histoRighXZ", "Right Sensor origin in global frame XZ",1200, -300, 800,1000,-0.45, 0.25 )
histoRightYZ = TH2F( "histoRightYZ", "Right Sensor origini in global frame YZ",1200, -300, 800,1000,-0.45, 0.25 )
histoLeftXZ = TH2F( "histoLeftXZ", "Left Sensor origin in global frame XZ",1200, -300, 800,1000,-0.45, 0.25 )
histoLeftYZ = TH2F( "histoLeftYZ", "Left Sensor origini in global frame YZ",1200, -300, 800,1000,-0.45, 0.25 )

first_R = '/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/Module00/RPhiPair00/Detector-00'
r_sensor = det[ first_R ]

base_left = '/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/Module'
base_right = '/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/Module'
rphi = '/RPhiPair'
det00 = '/Detector-00'
det01 = '/Detector-01'


name ='RightX'+str(xpos)+'-Y'+str(ypos)+label+'.txt'
nameL ='LeftX'+str(xpos)+'-Y'+str(ypos)+label+'.txt'
writefile =open(name,'w')
writefileL =open(nameL,'w')

for i in range(21):
  names = []
  number_right = '%02d' % ( 2 * i + 1)
  name_right00 = base_right + number_right + rphi + number_right + det00
  name_right01 = base_right + number_right + rphi + number_right + det01
  names.append( name_right00 )
  names.append( name_right01 )
  print  ' NAME ='+ name_right00 + ' NAME ='+ name_right01
  pointR = pointPhi = GaudiPython.gbl.ROOT.Math.XYZPoint()
  #pointR = pointPhi = Math.XYZPoint()
  
  averagexRight=0.
  averageyRight=0.
  for name in names:
    print 'name='+ name
    sensor = det[ name ]
    print 'sens='
    if sensor.type() == r_sensor.type():
      print 'sensor is R type'
      point = GaudiPython.gbl.ROOT.Math.XYZPoint()
      #point = Gaudi.Math.XYZPoint()
      print 'SetXYZ'
      point.SetXYZ( xpos,ypos,0 )
      print 'SetXYZ'
      pointR = globalPoint = sensor.geometry().toGlobal( point )
      print 'Global point'
      histoRightR.Fill( globalPoint.X(), globalPoint.Y() )
      averagexRight=averagexRight+globalPoint.X()
      averageyRight=averageyRight+globalPoint.Y()
      print 'i='+str(i)
      writefile.write(str(i));
      writefile.write(' ');
      writefile.write(str(1));
      writefile.write(' ');
      writefile.write(str(globalPoint.X()));
      writefile.write(' ');
      writefile.write(str(globalPoint.Y()));
      writefile.write(' ');
      writefile.write(str(globalPoint.Z()));
      writefile.write('\n');
      histoRightXZ.Fill( pointR.Z(), pointR.X() )
      histoRightYZ.Fill( pointR.Z(), pointR.Y() )
                  
    if sensor.type() != r_sensor.type():
      print 'sensor is Phi type'
      point = GaudiPython.gbl.ROOT.Math.XYZPoint()
      point.SetXYZ( xpos,ypos,0 )
      pointPhi = globalPoint = sensor.geometry().toGlobal( point )
      histoRightPhi.Fill( globalPoint.X(), globalPoint.Y() )
      averagexRight=averagexRight+globalPoint.X()
      averageyRight=averageyRight+globalPoint.Y()
      writefile.write(str(i));
      writefile.write(' ');
      writefile.write(str(0));
      writefile.write(' ');
      writefile.write(str(globalPoint.X()));
      writefile.write(' ');
      writefile.write(str(globalPoint.Y()));
      writefile.write(' ');
      writefile.write(str(globalPoint.Z()));
      writefile.write('\n');
      histoRightXZ.Fill( pointPhi.Z(), pointPhi.X() )
      histoRightYZ.Fill( pointPhi.Z(), pointPhi.Y() )
  histoRightRPhi.Fill( pointR.X() - pointPhi.X(), pointR.Y() - pointPhi.Y() )

  averagexRight=averagexRight/42.
  averageyRight=averageyRight/42.
  print "Right x=%d y=%d" % (averagexRight,averageyRight)
  names = []
  number_left = '%02d' % ( 2 * i )
  name_left00 = base_left + number_left + rphi + number_left + det00
  name_left01 = base_left + number_left + rphi + number_left + det01
  names.append( name_left00 )
  names.append( name_left01 )
  pointR = pointPhi = GaudiPython.gbl.ROOT.Math.XYZPoint()
  for name in names:
    print name
    sensor = det[ name ]
    if sensor.type() == r_sensor.type():
      point = GaudiPython.gbl.ROOT.Math.XYZPoint()
      point.SetXYZ( 0, 0, 0 )
      pointR = globalPoint = sensor.geometry().toGlobal( point )
      histoLeftR.Fill( globalPoint.X(), globalPoint.Y() )
      writefileL.write(str(i));
      writefileL.write(' ');
      writefileL.write(str(1));
      writefileL.write(' ');
      writefileL.write(str(globalPoint.X()));
      writefileL.write(' ');
      writefileL.write(str(globalPoint.Y()));
      writefileL.write(' ');
      writefileL.write(str(globalPoint.Z()));
      writefileL.write('\n');
      histoLeftXZ.Fill( pointR.Z(), pointR.X() )
      histoLeftYZ.Fill( pointR.Z(), pointR.Y() )
    if sensor.type() != r_sensor.type():
      point = GaudiPython.gbl.ROOT.Math.XYZPoint()
      point.SetXYZ( 0, 0, 0 )
      pointPhi = globalPoint = sensor.geometry().toGlobal( point )
      print globalPoint.X()
      print globalPoint.Y()
      histoLeftPhi.Fill( globalPoint.X(), globalPoint.Y() )
      writefileL.write(str(i));
      writefileL.write(' ');
      writefileL.write(str(0));
      writefileL.write(' ');
      writefileL.write(str(globalPoint.X()));
      writefileL.write(' ');
      writefileL.write(str(globalPoint.Y()));
      writefileL.write(' ');
      writefileL.write(str(globalPoint.Z()));
      writefileL.write('\n');
      histoLeftXZ.Fill( pointPhi.Z(), pointPhi.X() )
      histoLeftYZ.Fill( pointPhi.Z(), pointPhi.Y() )
  histoLeftRPhi.Fill( pointR.X() - pointPhi.X(), pointR.Y() - pointPhi.Y() )

c1 = TCanvas( "c1", "Sensor origins", 10, 10, 900, 600 )
c1.Divide(2,2)
#c1.Divide(3,2)
c1.cd(1)
histoRightR.Draw("box")
c1.cd(2)
histoRightPhi.Draw("box")
c1.cd(3)
#histoRightRPhi.Draw("box")
#c1.cd(4)
histoLeftR.Draw("box")
c1.cd(4)
#c1.cd(5)
histoLeftPhi.Draw("box")
#c1.cd(6)
#histoLeftRPhi.Draw("box")
#c1.SaveAs( "alignment_check.root" )


histoLeftXZ.SetMarkerColor(4);
histoLeftXZ.SetLineColor(4);
histoLeftXZ.SetMarkerStyle(25);
histoLeftXZ.SetMarkerSize(0.5);

histoLeftYZ.SetMarkerColor(4);
histoLeftYZ.SetLineColor(4);
histoLeftYZ.SetMarkerStyle(25);
histoLeftYZ.SetMarkerSize(0.5);

histoRightXZ.SetMarkerColor(2);
histoRightXZ.SetLineColor(2);
histoRightXZ.SetMarkerStyle(25);
histoRightXZ.SetMarkerSize(0.5);

histoRightYZ.SetMarkerColor(2);
histoRightYZ.SetLineColor(2);
histoRightYZ.SetMarkerStyle(25);
histoRightYZ.SetMarkerSize(0.5);

histoLeftXZ.SetTitle("Sensor origin in LHCb global frame")
histoLeftXZ.GetYaxis().SetTitle("x position (mm)");
histoLeftXZ.GetXaxis().SetTitle("z position (mm)");
histoLeftYZ.SetTitle("Sensor origin in LHCb global frame")
histoLeftYZ.GetYaxis().SetTitle("y position (mm)");
histoLeftYZ.GetXaxis().SetTitle("z position (mm)");

leg = TLegend(0.82,0.8,0.95,0.95);
leg.AddEntry(histoLeftXZ,"A side","p");
leg.AddEntry(histoRightXZ,"C side","p");


c11 = TCanvas( "c1", "Sensor origins", 10, 10, 900, 600 )
#c11.Divide(1,2)
c11.SetGrid();
#c11.SetGrid();
#c11.cd(1)
histoLeftXZ.Draw()
histoRightXZ.Draw("same")
leg.Draw();
filexpdf='SensXZPosX'+str(xpos)+'-Y'+str(ypos)+label+'.pdf'
c11.SaveAs(filexpdf)

#c11.cd(2)
histoLeftYZ.Draw()
histoRightYZ.Draw("same")
leg.Draw(); 
fileypdf='SensYZPosX'+str(xpos)+'-Y'+str(ypos)+label+'.pdf'
c11.SaveAs(fileypdf)


appMgr.exit()
