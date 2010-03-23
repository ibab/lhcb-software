#############################################################
##
##  Compare exisiting condDB value to new measurement
##
##  Author: T. Blake
##
#############################################################

from ROOT import TCanvas, TH1F, TH2F
import os, sys

from Gaudi.Configuration import *
from Configurables import DDDBConf, CondDB, CondDBAccessSvc

DDDBConf(DataType = "2009")
CondDB()

from rootStyle import applyRootStyle 
applyRootStyle()

from pyHPDXMLParsingUtils import *

import GaudiPython
app = GaudiPython.AppMgr()
det = app.detSvc()


def usage( argv ):
    print 'Usage: python ' + argv[0] + ' <xml 0> ... <xml n>'
    sys.exit(2)
    
def hpdPositionFromCondDB( hpd ):
    """
    Get the HPD local image offset in mm from the CondDB 
    """    
    sensorpath = 'HPD:' + str(hpd) + '/SiSensor:' + str(hpd)
    sisensor = None 
    if ( hpd < 98 ):
        sisensor = det['Structure/LHCb/BeforeMagnetRegion/Rich1/HPDPanel0/'+sensorpath]
    elif ( hpd < 196 ):
        sisensor = det['Structure/LHCb/BeforeMagnetRegion/Rich1/HPDPanel1/'+sensorpath] 
    elif ( hpd < 340 ):
        sisensor = det['Structure/LHCb/AfterMagnetRegion/Rich2/HPDPanel0/'+sensorpath] 
    else:
        sisensor = det['Structure/LHCb/AfterMagnetRegion/Rich2/HPDPanel1/'+sensorpath] 
    if None == sisensor:
        return (False,0,0)
    zero = GaudiPython.gbl.Math.XYZPoint()
    offset = sisensor.geometry().ownMatrix()*zero
    return (True, -offset.x(), -offset.y())


def drawImageDeltaPos( xmlfilelist ):
    histx0 = TH1F("hist_x0","hist_x0",41,-2.05,2.05)
    histx0.Sumw2()
    histx0.SetXTitle('x-shift [mm]')
    histy0 = TH1F("hist_y0","hist_y0",41,-2.05,2.05)
    histy0.Sumw2()
    histy0.SetXTitle('y-shift [mm]')
    scatterx0 = TH2F("scatter_x0","",41,-2.05,2.05,41,-2.05,2.05)
    scatterx0.SetXTitle('x-shift [mm]')
    scatterx0.SetYTitle('x-shift CondDB [mm]')
    scattery0 = TH2F("scatter_y0","",41,-2.05,2.05,41,-2.05,2.05)
    scattery0.SetXTitle('y-shift [mm]')
    scattery0.SetYTitle('y-shift CondDB [mm]')
    counterdict = countersFromXMLFileList( xmlfilelist )
    for hpd in range(0,484):
        (isvalid,x0,y0) = hpdLocalOffset( hpd, counterdict )
        (isvalidDB,condx0,condy0) = hpdPositionFromCondDB(hpd)
        if isvalid and isvalidDB:
            histx0.Fill(x0-condx0)
            histy0.Fill(y0-condy0)
            scatterx0.Fill(x0,condx0)
            scattery0.Fill(y0,condy0)
    canDelta = TCanvas('canDelta')
    canDelta.Divide(2)
    canDelta.cd(1)
    histx0.Draw()
    canDelta.cd(2)
    histy0.Draw()
    canDelta.Update()
    canScat = TCanvas('canScatter')
    canScat.Divide(2)
    canScat.cd(1)
    scatterx0.Draw('scat')
    canScat.cd(2)
    scattery0.Draw('scat')
    canScat.Update()
    rep = ''
    while not rep in [ 'q', 'Q' ]:
        rep = raw_input( 'enter "q" to quit: ' )
        if 1 < len(rep):
            rep = rep[0]


if len(sys.argv) < 2:
    usage(sys.argv)
else:
    xmllist = []
    for i in range(1,len(sys.argv)):
        xmlfile = sys.argv[i]
        if not os.path.exists( xmlfile ):
            sys.exit(2)
        xmllist += [ xmlfile ]
    drawImageDeltaPos( xmllist )
