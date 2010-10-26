from ROOT import *
from array import array

############################################
# Computinug the efficiency in a box region
############################################

def getEffRegion(LayerSumHit, x1=32, x2=42, y1=0, y2=20):

    nentries = 0
    success = 0

    #print "***************************"
    for x in range(x1,x2):
    
        for y in range(y1, y2):
            
            binnr = LayerSumHit.GetBin(x,y)
            effxBin = LayerSumHit.GetBinContent(binnr)
            nentryxBin =LayerSumHit.GetBinEntries(binnr) 
            #print "bin eff: %s  entries:  %s"%(effxBin, nentryxBin)
            success += effxBin*nentryxBin
            nentries += nentryxBin


    eff = success/nentries
    error = TMath.sqrt(eff*(1-eff)/nentries)
    #error2 = TMath.sqrt(success/(success*success)+nentries/(nentries*nentries))*eff
        
    #print "eff: %s +- %s"%(eff, error)
    #print " ---- compare errors %s    %s"%(error, error2)

    return {"eff":eff, "error":error}


#############################################
#   Same but symmetric in y: 
#############################################
def getEffRegionSymmY(LayerSumHit, x1=32, x2=42, y1=0, y2=20):

    nentries = 0
    success = 0

    for x in range(x1,x2):
        for y in range(y1, y2):
            binnr = LayerSumHit.GetBin(x,y)
            effxBin = LayerSumHit.GetBinContent(binnr)
            nentryxBin =LayerSumHit.GetBinEntries(binnr) 
            success += effxBin*nentryxBin
            nentries += nentryxBin


    for x in range(x1, x2):
        for y in range(100-y2, 100-y1):
            binnr = LayerSumHit.GetBin(x,y)
            effxBin = LayerSumHit.GetBinContent(binnr)
            nentryxBin =LayerSumHit.GetBinEntries(binnr) 
            success += effxBin*nentryxBin
            nentries += nentryxBin

    eff = success/nentries
    error = TMath.sqrt(eff*(1-eff)/nentries)
    #error2 = TMath.sqrt(success/(success*success)+nentries/(nentries*nentries))*eff
    print "eff: %s +- %s"%(eff, error)

    return {"eff":eff, "error":error}

########################################################
###   Calculate efficiency per layer  
########################################################

def makeplotslayer(histolist,title,lowz,highz):

    # make an array of the thresholds
    x = array("f", [800.0, 1000.0, 1200.0, 1250.0, 1300.0, 1350.0, 1400.0, 1450.0])
    xerror = array("f", [0.0]*8)
    
    #These values are the number of bins in the entire layer!
    x1 = 1
    x2 = 73
    y1 = 1
    y2 = 99

    listY =[]
    listYError = []

    print " " 
    print "*** Make threshold curve for region.    x: %s-%s  y: %s-%s"%(x1,x2,y1,y2)
    for ih in histolist:
        ih.SetXTitle("X (mm)")
        ih.SetYTitle("Y (mm)")
        ih.SetZTitle("Eff.")
       #dictEff = getEffRegionSymmY(ih, x1=25, x2=39, y1=0, y2=20)
        dictEff = getEffRegion(ih, x1, x2, y1, y2)

        listY.append(dictEff['eff'])
        listYError.append(dictEff['error'])

    min_eff = 0.0
    max_eff = 1.0

    y = array('f', listY)
    yerror = array('f', listYError)

    graph = TGraphErrors(len(histolist), x, y, xerror, yerror)
    graph.SetTitle("Threshold Curve (2 par)")
    graph.SetMarkerStyle(20)
    graph.SetMarkerColor(4)
    graph.GetXaxis().SetTitle("Threshold (mV)")
    graph.GetYaxis().SetTitle("OT Hit efficiency")
    graph.SetMaximum(1.3)

    print 'listwithefficiencies =', y
    
    graph_copy = TGraphErrors(len(histolist), x, y, xerror, yerror)
    graph_copy.SetTitle("Threshold Curve (4 par)")
    graph_copy.SetMarkerStyle(20)
    graph_copy.SetMarkerColor(4)
    graph_copy.GetXaxis().SetTitle("Threshold (mV)")
    graph_copy.GetYaxis().SetTitle("OT Hit efficiency")
    graph_copy.SetMaximum(1.3)

    # ------------------------------------
    # 2d-Histogram with half-eff. points:
    # ------------------------------------
    offsetx=1
    offsety=1
    xmin=0+offsetx
    xmax=histolist[0].GetNbinsX()-offsetx
    ymin=0+offsety
    ymax=histolist[0].GetNbinsY()-offsety
    print "*** Make 2d histo with half.eff.points. x: %s-%s  y: %s-%s"%(xmin, xmax, ymin, ymax)
    m_2DHalfEffPoint = TH2D( "halfeffpoint", "Half Eff.Point (mV)",\
                             74, -3157.0, 3157.0, 100, -2800,2800)
    m_2DHalfEffPoint.SetXTitle("X (mm)")
    m_2DHalfEffPoint.SetYTitle("Y (mm)")
    m_2DHalfEffPoint.SetZTitle("Thr (mV)")
    m_2DHalfEffWidth = TH2D( "halfeffwidth", "Half Eff.Width (mV)",\
                             74, -3157.0, 3157.0, 100, -2800,2800)
    m_2DHalfEffWidth.SetXTitle("X (mm)")
    m_2DHalfEffWidth.SetYTitle("Y (mm)")
    m_2DHalfEffChi2 = TH2D( "halfeffchi2", "Chi2/ndof of S-curve Fit",\
                             74, -3157.0, 3157.0, 100, -2800,2800)
    m_2DHalfEffChi2.SetXTitle("X (mm)")
    m_2DHalfEffChi2.SetYTitle("Y (mm)")
    m_2DHalfEffPlateau = TH2D( "halfeffplateau", "Eff plateau of S-curve Fit",\
                             74, -3157.0, 3157.0, 100, -2800,2800)
    m_2DHalfEffPlateau.SetXTitle("X (mm)")
    m_2DHalfEffPlateau.SetYTitle("Y (mm)")
    m_2DHalfEffTail = TH2D( "halfefftail", "Tail of S-curve Fit",\
                             74, -3157.0, 3157.0, 100, -2800,2800)
    m_2DHalfEffTail.SetXTitle("X (mm)")
    m_2DHalfEffTail.SetYTitle("Y (mm)")

    ic = 0

    for ix in range(xmin,xmax):
        for iy in range(ymin, ymax):
            listY =[]
            listYError = []
            nentries_allruns=0
            for j in xrange(0, len(histolist)):
                binnr = histolist[j].GetBin(ix,iy)
                eff = histolist[j].GetBinContent(binnr)
                nentries =histolist[j].GetBinEntries(binnr) 
                nentries_allruns =+ nentries
                error = 0.
                if nentries > 0:
                    error=TMath.sqrt(eff*(1-eff)/nentries)

                listY.append(eff)
                listYError.append(error)
                #if eff > 0.45 and eff < 0.55: print ix, iy, binnr, x[i], eff

            y = array('f', listY)
            yerror = array('f', listYError)
            graphtmp = TGraphErrors(len(histolist), x, y, xerror, yerror)
            f2.SetParameter(0,1350.) 
            f2.SetParameter(1,  75.) 
            f2.SetParameter(2,0.9999) 
            f2.SetParameter(3,0.0001) 
            f2.SetParLimits(2, -0.01,1.01) 
            f2.SetParLimits(3, -0.01,0.1) 
            graphtmp.Fit("f2","Q0")
            halfeffpoint   = 0.
            halfeffwidth   = 0.
            halfeffplateau = 0.
            halfefftail    = 0.
            halfeffchi2    = 0.
            halfeffchi2ndof= 0.
            halfeffndof    = f2.GetNDF()
            if nentries_allruns > 4 and halfeffndof > 0:
                halfeffpoint   = f2.GetParameter(0)
                halfeffwidth   = f2.GetParameter(1)
                if f2.GetNumberFreeParameters() > 2:
                    halfeffplateau = f2.GetParameter(2)
                    halfefftail    = f2.GetParameter(3)
                halfeffchi2    = f2.GetChisquare()        
                halfeffchi2ndof=halfeffchi2/halfeffndof
            #print ic, ix, iy, x[1], y[1], x[12], y[12], halfeffpoint
            #print ix, iy, halfeffpoint, halfeffndof
            m_2DHalfEffPoint.SetBinContent(ix,iy,halfeffpoint)
            m_2DHalfEffWidth.SetBinContent(ix,iy,halfeffwidth)
            m_2DHalfEffChi2.SetBinContent(ix,iy,halfeffchi2ndof)
            m_2DHalfEffPlateau.SetBinContent(ix,iy,halfeffplateau)
            m_2DHalfEffTail.SetBinContent(ix,iy,halfefftail)

    print "*** Draw canvas"
    canvas = TCanvas(title,title,10,10,1100,600)
    canvas.Divide(3,3)

    canvas.cd(1)
    graph.Draw("AP")
    graph.Fit("f1","Q")
    #myline=TLine(x1,y1,x2,y2)
    myline1=TLine(1350.,0.,1350.,0.5)
    myline2=TLine(750.,0.5,1350.,0.5)
    myline1.Draw("same")
    myline2.Draw("same")

    canvas.cd(2)
    graph_copy.Draw("AP")
    f2.SetParameter(0,1330.) 
    f2.SetParameter(1,  80.) 
    f2.SetParameter(2, 0.999) 
    f2.SetParameter(3, 0.001) 
    graph_copy.Fit("f2","Q")
    myline1=TLine(1350.,0.,1350.,0.5)
    myline2=TLine(750.,0.5,1350.,0.5)
    myline1.Draw("same")
    myline2.Draw("same")

    canvas.cd(3)
    histolist[5].GetZaxis().SetRangeUser(min_eff, max_eff)
    histolist[5].Draw("colz")

    canvas.cd(4)
    m_2DHalfEffPoint.GetZaxis().SetRangeUser(lowz, highz)
    m_2DHalfEffPoint.Draw("colz")

    canvas.cd(5)
    m_2DHalfEffWidth.GetZaxis().SetRangeUser(40., 90.)
    m_2DHalfEffWidth.Draw("colz")

    canvas.cd(6)
    m_2DHalfEffChi2.GetZaxis().SetRangeUser(0., 10.)
    m_2DHalfEffChi2.Draw("colz")

    canvas.cd(7)
    m_2DHalfEffPlateau.GetZaxis().SetRangeUser(0.8, 1.2)
    m_2DHalfEffPlateau.Draw("colz")

    canvas.cd(8)
    m_2DHalfEffTail.GetZaxis().SetRangeUser(0.0, 0.02)
    m_2DHalfEffTail.Draw("colz")

    #raw_input('Press Enter to Quit')

    canvas.Print('pics/'+title+'.eps')
        
    return 

########################################################
###   Calculate efficiency SPECIAL LAYER0  
########################################################

def specialmakeplotslayer0(histolist,title,lowz,highz):

    # make an array of the thresholds
    x = array("f", [1000.0, 1200.0, 1250.0, 1300.0, 1350.0, 1400.0, 1450.0])
    xerror = array("f", [0.0]*7)
    
    #These values are the number of bins in the entire layer!
    x1 = 1
    x2 = 73
    y1 = 1
    y2 = 99

    listY =[]
    listYError = []

    print " " 
    print "*** Make threshold curve for region.    x: %s-%s  y: %s-%s"%(x1,x2,y1,y2)
    for ih in histolist:
        ih.SetXTitle("X (mm)")
        ih.SetYTitle("Y (mm)")
        ih.SetZTitle("Eff.")
       #dictEff = getEffRegionSymmY(ih, x1=25, x2=39, y1=0, y2=20)
        dictEff = getEffRegion(ih, x1, x2, y1, y2)

        listY.append(dictEff['eff'])
        listYError.append(dictEff['error'])

    min_eff = 0.0
    max_eff = 1.0

    y = array('f', listY)
    yerror = array('f', listYError)

    graph = TGraphErrors(len(histolist), x, y, xerror, yerror)
    graph.SetTitle("Threshold Curve (2 par)")
    graph.SetMarkerStyle(20)
    graph.SetMarkerColor(4)
    graph.GetXaxis().SetTitle("Threshold (mV)")
    graph.GetYaxis().SetTitle("OT Hit efficiency")
    graph.SetMaximum(1.3)

    print 'listwithefficiencies =', y
    
    graph_copy = TGraphErrors(len(histolist), x, y, xerror, yerror)
    graph_copy.SetTitle("Threshold Curve (4 par)")
    graph_copy.SetMarkerStyle(20)
    graph_copy.SetMarkerColor(4)
    graph_copy.GetXaxis().SetTitle("Threshold (mV)")
    graph_copy.GetYaxis().SetTitle("OT Hit efficiency")
    graph_copy.SetMaximum(1.3)

    # ------------------------------------
    # 2d-Histogram with half-eff. points:
    # ------------------------------------
    offsetx=1
    offsety=1
    xmin=0+offsetx
    xmax=histolist[0].GetNbinsX()-offsetx
    ymin=0+offsety
    ymax=histolist[0].GetNbinsY()-offsety
    print "*** Make 2d histo with half.eff.points. x: %s-%s  y: %s-%s"%(xmin, xmax, ymin, ymax)
    m_2DHalfEffPoint = TH2D( "halfeffpoint", "Half Eff.Point (mV)",\
                             74, -3157.0, 3157.0, 100, -2800,2800)
    m_2DHalfEffPoint.SetXTitle("X (mm)")
    m_2DHalfEffPoint.SetYTitle("Y (mm)")
    m_2DHalfEffPoint.SetZTitle("Thr (mV)")
    m_2DHalfEffWidth = TH2D( "halfeffwidth", "Half Eff.Width (mV)",\
                             74, -3157.0, 3157.0, 100, -2800,2800)
    m_2DHalfEffWidth.SetXTitle("X (mm)")
    m_2DHalfEffWidth.SetYTitle("Y (mm)")
    m_2DHalfEffChi2 = TH2D( "halfeffchi2", "Chi2/ndof of S-curve Fit",\
                             74, -3157.0, 3157.0, 100, -2800,2800)
    m_2DHalfEffChi2.SetXTitle("X (mm)")
    m_2DHalfEffChi2.SetYTitle("Y (mm)")
    m_2DHalfEffPlateau = TH2D( "halfeffplateau", "Eff plateau of S-curve Fit",\
                             74, -3157.0, 3157.0, 100, -2800,2800)
    m_2DHalfEffPlateau.SetXTitle("X (mm)")
    m_2DHalfEffPlateau.SetYTitle("Y (mm)")
    m_2DHalfEffTail = TH2D( "halfefftail", "Tail of S-curve Fit",\
                             74, -3157.0, 3157.0, 100, -2800,2800)
    m_2DHalfEffTail.SetXTitle("X (mm)")
    m_2DHalfEffTail.SetYTitle("Y (mm)")

    ic = 0

    for ix in range(xmin,xmax):
        for iy in range(ymin, ymax):
            listY =[]
            listYError = []
            nentries_allruns=0
            for j in xrange(0, len(histolist)):
                binnr = histolist[j].GetBin(ix,iy)
                eff = histolist[j].GetBinContent(binnr)
                nentries =histolist[j].GetBinEntries(binnr) 
                nentries_allruns =+ nentries
                error = 0.
                if nentries > 0:
                    error=TMath.sqrt(eff*(1-eff)/nentries)

                listY.append(eff)
                listYError.append(error)
                #if eff > 0.45 and eff < 0.55: print ix, iy, binnr, x[i], eff

            y = array('f', listY)
            yerror = array('f', listYError)
            graphtmp = TGraphErrors(len(histolist), x, y, xerror, yerror)
            f2.SetParameter(0,1350.) 
            f2.SetParameter(1,  75.) 
            f2.SetParameter(2,0.9999) 
            f2.SetParameter(3,0.0001) 
            f2.SetParLimits(2, -0.01,1.01) 
            f2.SetParLimits(3, -0.01,0.1) 
            graphtmp.Fit("f2","Q0")
            halfeffpoint   = 0.
            halfeffwidth   = 0.
            halfeffplateau = 0.
            halfefftail    = 0.
            halfeffchi2    = 0.
            halfeffchi2ndof= 0.
            halfeffndof    = f2.GetNDF()
            if nentries_allruns > 4 and halfeffndof > 0:
                halfeffpoint   = f2.GetParameter(0)
                halfeffwidth   = f2.GetParameter(1)
                if f2.GetNumberFreeParameters() > 2:
                    halfeffplateau = f2.GetParameter(2)
                    halfefftail    = f2.GetParameter(3)
                halfeffchi2    = f2.GetChisquare()        
                halfeffchi2ndof=halfeffchi2/halfeffndof
            #print ic, ix, iy, x[1], y[1], x[12], y[12], halfeffpoint
            #print ix, iy, halfeffpoint, halfeffndof
            m_2DHalfEffPoint.SetBinContent(ix,iy,halfeffpoint)
            m_2DHalfEffWidth.SetBinContent(ix,iy,halfeffwidth)
            m_2DHalfEffChi2.SetBinContent(ix,iy,halfeffchi2ndof)
            m_2DHalfEffPlateau.SetBinContent(ix,iy,halfeffplateau)
            m_2DHalfEffTail.SetBinContent(ix,iy,halfefftail)

    print "*** Draw canvas"
    canvas = TCanvas(title,title,10,10,1100,600)
    canvas.Divide(3,3)

    canvas.cd(1)
    graph.Draw("AP")
    graph.Fit("f1","Q")
    #myline=TLine(x1,y1,x2,y2)
    myline1=TLine(1350.,0.,1350.,0.5)
    myline2=TLine(750.,0.5,1350.,0.5)
    myline1.Draw("same")
    myline2.Draw("same")

    canvas.cd(2)
    graph_copy.Draw("AP")
    f2.SetParameter(0,1330.) 
    f2.SetParameter(1,  80.) 
    f2.SetParameter(2, 0.999) 
    f2.SetParameter(3, 0.001) 
    graph_copy.Fit("f2","Q")
    myline1=TLine(1350.,0.,1350.,0.5)
    myline2=TLine(750.,0.5,1350.,0.5)
    myline1.Draw("same")
    myline2.Draw("same")

    canvas.cd(3)
    histolist[4].GetZaxis().SetRangeUser(min_eff, max_eff)
    histolist[4].Draw("colz")

    canvas.cd(4)
    m_2DHalfEffPoint.GetZaxis().SetRangeUser(lowz, highz)
    m_2DHalfEffPoint.Draw("colz")

    canvas.cd(5)
    m_2DHalfEffWidth.GetZaxis().SetRangeUser(40., 90.)
    m_2DHalfEffWidth.Draw("colz")

    canvas.cd(6)
    m_2DHalfEffChi2.GetZaxis().SetRangeUser(0., 10.)
    m_2DHalfEffChi2.Draw("colz")

    canvas.cd(7)
    m_2DHalfEffPlateau.GetZaxis().SetRangeUser(0.8, 1.2)
    m_2DHalfEffPlateau.Draw("colz")

    canvas.cd(8)
    m_2DHalfEffTail.GetZaxis().SetRangeUser(0.0, 0.02)
    m_2DHalfEffTail.Draw("colz")

    #raw_input('Press Enter to Quit')

    canvas.Print('pics/'+title+'.eps')

    #Save histos to root file

    fnew = TFile("eff-Aug2010-L0.root", "recreate")
    m_2DHalfEffPoint.Write()
    m_2DHalfEffWidth.Write()
    m_2DHalfEffChi2.Write()
    m_2DHalfEffPlateau.Write()
    m_2DHalfEffTail.Write()
    fnew.Close()    
    return 
