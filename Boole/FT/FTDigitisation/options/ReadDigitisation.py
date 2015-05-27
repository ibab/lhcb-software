#! /usr/bin/env python
#python ReadDigitisation.py -i 2BitsADC_Initial-100ev-histos.root
__author__ = 'Eric Cogneras'

import os,sys,getopt,time
from ROOT import gROOT, TFile, TCanvas, TH1D,kBlue, kRed, gStyle, TLine, TF1, TLatex,TLegend,THStack

class Read:
    def __init__(self, files= ["MiniBias_v20_nu25_EC-histos.root"], outtype = ".gif"):
        self.ListOfFiles = files
        self.OutFileType = outtype
        self.LogScale = False
        self.XMax = 3210 # in mm
        self.YMax = 2418 # in mm
        self.lineY = TLine(0,self.YMax,self.XMax,self.YMax)
        self.lineX = TLine(self.XMax,0,self.XMax,self.YMax)
        self.lineX.SetLineColor(1)
        self.lineX.SetLineWidth(4)
        self.lineY.SetLineColor(1)
        self.lineY.SetLineWidth(4)
        print self.ListOfFiles,self.OutFileType

    def DrawQuarterEdge(self):
        self.lineX.Draw()
        self.lineY.Draw()
        
    def CreateDir(self,DirChain):
        Dirnames = DirChain.split('/')
        Dirname = ""
        for i in Dirnames:
            Dirname = os.path.join(Dirname, i)
            if os.path.exists("./"+Dirname):
                print "Directory ",Dirname, "  exist : do nothing"
            else :
                print "Directory ",Dirname, " do not exist : create it"
                os.mkdir("./"+Dirname)
            
    def Draw1DHisto(self,list,cvs,mantisse):
        FileMantisse = mantisse.split('/')[0]
        RangeSupEdge = 1
        if(self.LogScale): RangeSupEdge = 2
        for i in range(RangeSupEdge):
            if i==1:
                cvs.SetLogy()
                ext = "_LOG"+self.OutFileType
            else:
                cvs.SetLogy(0)
                ext = self.OutFileType
                
            for h in list :
                h.SetMarkerStyle(8)
                h.SetMarkerColor(kBlue)
                h.Draw("P")
                cvs.SaveAs(mantisse+"/"+FileMantisse+"_"+h.GetName()+ext)
                
    def Draw1DHistoBar(self,list,cvs,mantisse):
        FileMantisse = mantisse.split('/')[0]
        RangeSupEdge = 1
        if(self.LogScale): RangeSupEdge = 2
        for i in range(RangeSupEdge):
            if i==1:
                cvs.SetLogy()
                ext = "_LOG"+self.OutFileType
            else:
                cvs.SetLogy(0)
                ext = self.OutFileType
                
            for h in list :
                h.SetFillColor(kBlue)
                h.Draw("bar2")
                cvs.SaveAs(mantisse+"/"+FileMantisse+"_"+h.GetName()+ext)               

    def Draw2DHisto(self,list,cvs,mantisse, opt=""):
        print "___________ Draw2DHisto"
        #gStyle.SetOptStat(0)
        FileMantisse = mantisse.split('/')[0]
        for h in list :
            print h
            #h.SetMarkerStyle(8)
            #h.SetMarkerColor(kBlue)
            h.Draw("COLORZ")
            for Options in  opt.split(" ") :
                print Options
                if (Options == "QE") :
                    self.DrawQuarterEdge()
            cvs.SaveAs(mantisse+"/"+FileMantisse+"_"+h.GetName()+self.OutFileType)
            
    def DrawADCGainProfile(self,list,cvs,mantisse, opt=""):
        FileMantisse = mantisse.split('/')[0]
        cvs.SetLogy(0)
        for h in list :
            hProf = h.ProfileX()
            h.Draw("colz")
            hProf.SetLineWidth(5)
            hProf.SetLineColor(7)
            hProf.Draw("SAME")
            linFit = TF1("linFit", "pol1", 0, 0.3)
            hProf.Fit(linFit,"","SAME", 0, 0.3)
            comment = TLatex(0.5,.2,"Slope = "+str("%.0f" % linFit.GetParameter(1))+" MeV^{-1}")
            comment.SetTextColor(kRed)
            comment.SetNDC(1)
            comment.Draw()
            cvs.SaveAs(mantisse+"/"+FileMantisse+"_"+h.GetName()+self.OutFileType)

    def DrawResolutionSingleGaussianFit(self,list,cvs,mantisse, opt=""):
        FileMantisse = mantisse.split('/')[0]
        for h in list :
            h.SetFillColor(38)
            h.SetMarkerStyle(8)
            sgngaus = TF1("sgngaus", "gaus", h.GetMean()-2*h.GetRMS(),h.GetMean()+2*h.GetRMS())
            h.Fit(sgngaus,"","", sgngaus.GetXmin(), sgngaus.GetXmax())
            comment = TLatex(.6,.6,"#sigma = "+str("%.3f" % sgngaus.GetParameter(2))+" mm")                
            comment.SetTextColor(kRed)
            comment.SetNDC(1)

            RangeSupEdge = 1
            if(self.LogScale): RangeSupEdge = 2
            for i in range(RangeSupEdge):
                if i==1:
                    cvs.SetLogy()
                    ext = "_LOG"+self.OutFileType
                else:
                    cvs.SetLogy(0)
                    ext = self.OutFileType
                h.Draw("bar2")
                sgngaus.Draw("SAME")
                comment.Draw()         
                cvs.SaveAs(mantisse+"/"+FileMantisse+"_"+h.GetName()+"SingleGaussFit"+ext)

    def DrawResolutionDoubleGaussianFit(self,list,cvs,mantisse, opt=""):
        FileMantisse = mantisse.split('/')[0]
        for h in list :
            h.SetFillColor(38)
            #h.Draw("bar2")
            doublgaus = TF1("doublgaus", "gaus(0)+gaus(3)", h.GetXaxis().GetXmin(), h.GetXaxis().GetXmax() )   
            sgngaus= TF1("sgngaus", "gaus", doublgaus.GetXmin(), doublgaus.GetXmax() )
            bckgaus = TF1("bckgaus", "gaus", doublgaus.GetXmin(), doublgaus.GetXmax() )
            h.Fit(sgngaus,"","", sgngaus.GetXmin(),sgngaus.GetXmax())
            doublgaus.SetParameters(sgngaus.GetParameter(0),sgngaus.GetParameter(1),sgngaus.GetParameter(2),sgngaus.GetParameter(0)/100.,sgngaus.GetParameter(1),sgngaus.GetParameter(2)*2)
            doublgaus.SetParLimits(4,(doublgaus.GetXmin()-(doublgaus.GetXmax()-doublgaus.GetXmin())/2),(doublgaus.GetXmin()+(doublgaus.GetXmax()-doublgaus.GetXmin())/2))
            h.Fit(doublgaus,"","", doublgaus.GetXmin(),doublgaus.GetXmax())
            sgngaus.SetParameters(doublgaus.GetParameter(0),doublgaus.GetParameter(1),doublgaus.GetParameter(2))
            bckgaus.SetParameters(doublgaus.GetParameter(3),doublgaus.GetParameter(4),doublgaus.GetParameter(5))
            bckgaus.SetLineStyle(7)
            bckgaus.SetLineColor(1)
 
            sgngaus.SetLineStyle(7)
            sgngaus.SetLineColor(4)

            comment = TLatex(.6,.6,"#sigma = "+str("%.3f" % doublgaus.GetParameter(2))+" mm")                
            comment.SetTextColor(kBlue)
            comment.SetNDC(1)

            RangeSupEdge = 1
            if(self.LogScale): RangeSupEdge = 2
            for i in range(RangeSupEdge):
                if i==1:
                    cvs.SetLogy()
                    ext = "_LOG"+self.OutFileType
                else:
                    cvs.SetLogy(0)
                    ext = self.OutFileType
                h.Draw("bar2")
                bckgaus.Draw("SAME")
                sgngaus.Draw("SAME")
                comment.Draw()    
                cvs.SaveAs(mantisse+"/"+FileMantisse+"_"+h.GetName()+"DoubleGaussFit"+ext)


    def DepositPlots(self):
        #Rstyle = TStyle(ROOT.TStyle)
        #gStyle.SetOptStat(0)
        gStyle.SetPalette(1)
        RootDirectory = 'MCFTDepositCreator'
        EdgePlotList = ["FibreAttenuationMap", "RadiationAttMap", 
                        "ReflectionContributionMap", "ReflectionAttenuationMap", 
                        "FinalAttenuationMap", "FinalReflectedAttenuationMap"]
        #gROOT.SetStyle("Rstyle")
        for files in self.ListOfFiles:
            tfile = TFile.Open(files)
            OutFileMantisse = files[:files.find('-histos')]+"/"+RootDirectory+"_"+time.strftime('%y-%m-%d_%H-%M',time.localtime())
            self.CreateDir(OutFileMantisse)
            tdir = tfile.GetDirectory(RootDirectory)
            histo1D = []
            histo2D = []     
            histo2DQuarterEdge = []
            DepositCvs= TCanvas("Depositcvs","Depositcvs")
            

            dirlist = tdir.GetListOfKeys()
            iter = dirlist.MakeIterator()
            key = iter.Next()
            td = None
            while key:
                if key.GetClassName() == 'TH1D':
                    td = key.ReadObj()
                    #objName = td.GetName()
                    #print "found TH1D", objName
                    histo1D.append(td) 
                if key.GetClassName() == 'TH2D':
                    td = key.ReadObj()
                    #objName = td.GetName()
                    if td.GetName() in EdgePlotList :
                        histo2DQuarterEdge.append(td)
                    else :
                    	histo2D.append(td)
                key = iter.Next()

            self.Draw2DHisto(histo2D,DepositCvs,OutFileMantisse)

            self.Draw2DHisto(histo2DQuarterEdge,DepositCvs,OutFileMantisse,"QE")

            self.Draw1DHistoBar(histo1D,DepositCvs,OutFileMantisse)

            
    def DigitPlots(self):
        RootDirectory = 'MCFTDigitCreator'
        for files in self.ListOfFiles:
            tfile = TFile.Open(files)
            OutFileMantisse = files[:files.find('-histos')]+"/"+RootDirectory+"_"+time.strftime('%y-%m-%d_%H-%M',time.localtime())
            self.CreateDir(OutFileMantisse)
            tdir = tfile.GetDirectory(RootDirectory)
            histo1D = []
            histo2D = []     
            histoADCvsEnergy = []            
            DigitCvs= TCanvas("Digitcvs","Digitcvs")
            

            dirlist = tdir.GetListOfKeys()
            iter = dirlist.MakeIterator()
            key = iter.Next()
            td = None
            while key:
                if key.GetClassName() == 'TH1D':
                    td = key.ReadObj()
                    #objName = td.GetName()
                    #print "found TH1D", objName
                    histo1D.append(td) 
                if key.GetClassName() == 'TH2D':
                    td = key.ReadObj()
                    #objName = td.GetName()
                    #print "found TH2D", objName
                    if (td.GetName().find('ADCGain') >= 0):
                        histoADCvsEnergy.append(td)
                    else :
                        histo2D.append(td)
                key = iter.Next()

            self.Draw2DHisto(histo2D,DigitCvs,OutFileMantisse)

            self.Draw1DHistoBar(histo1D,DigitCvs,OutFileMantisse)

            self.DrawADCGainProfile(histoADCvsEnergy,DigitCvs,OutFileMantisse)
            
    def ClusterPlots(self):
        RootDirectory = 'FTClusterCreator'
        for files in self.ListOfFiles:
            tfile = TFile.Open(files)
            OutFileMantisse = files[:files.find('-histos')]+"/"+RootDirectory+"_"+time.strftime('%y-%m-%d_%H-%M',time.localtime())
            self.CreateDir(OutFileMantisse)
            tdir = tfile.GetDirectory(RootDirectory)
            histo1D = []
            histoResolution = []
            histo2D = []                
            Clustercvs= TCanvas("Clustercvs","Clustercvs")
            

            dirlist = tdir.GetListOfKeys()
            iter = dirlist.MakeIterator()
            key = iter.Next()
            td = None
            while key:
                if key.GetClassName() == 'TH1D':
                    td = key.ReadObj()
                    #objName = td.GetName()
                    #print "found TH1D", objName
                    if (td.GetName().find('Cluster_x_position_resolution') >= 0 ): 
                        histoResolution.append(td) 
                    else : 
                        histo1D.append(td) 
                if key.GetClassName() == 'TH2D':
                    td = key.ReadObj()
                    #objName = td.GetName()
                    #print "found TH2D", objName
                    histo2D.append(td)
                key = iter.Next()

            self.Draw2DHisto(histo2D,Clustercvs,OutFileMantisse)

            self.Draw1DHistoBar(histo1D,Clustercvs,OutFileMantisse)

            self.DrawResolutionSingleGaussianFit(histoResolution,Clustercvs,OutFileMantisse)

            self.DrawResolutionDoubleGaussianFit(histoResolution,Clustercvs,OutFileMantisse)                      

    def DepositMoniPlots(self):
        RootDirectory = 'MCFTDepositMonitor'
        for files in self.ListOfFiles:
            tfile = TFile.Open(files)
            OutFileMantisse = files[:files.find('-histos')]+"/"+RootDirectory+"_"+time.strftime('%y-%m-%d_%H-%M',time.localtime())
            self.CreateDir(OutFileMantisse)
            tdir = tfile.GetDirectory(RootDirectory)
            histo1D = []
            histoResolution = []
            histo2D = []                
            DepositMonicvs= TCanvas("DepositMonicvs","DepositMonicvs")
            

            dirlist = tdir.GetListOfKeys()
            iter = dirlist.MakeIterator()
            key = iter.Next()
            td = None
            while key:
                if key.GetClassName() == 'TH1D':
                    td = key.ReadObj()
                    histo1D.append(td) 
                if key.GetClassName() == 'TH2D':
                    td = key.ReadObj()
                    histo2D.append(td)
                key = iter.Next()
            self.Draw2DHisto(histo2D,DepositMonicvs,OutFileMantisse)

            self.Draw1DHistoBar(histo1D,DepositMonicvs,OutFileMantisse)
            
    def ClusterMoniPlots(self):
        #gStyle.SetOptStat(11111)
        RootDirectory = 'FTClusterMonitor'
        for files in self.ListOfFiles:
            tfile = TFile.Open(files)
            OutFileMantisse = files[:files.find('-histos')]+"/"+RootDirectory+"_"+time.strftime('%y-%m-%d_%H-%M',time.localtime())
            self.CreateDir(OutFileMantisse)
            tdir = tfile.GetDirectory(RootDirectory)
            histo1D = []
            histoResolution = []
            histo2D = []                
            ClusterMonicvs= TCanvas("ClusterMonicvs","ClusterMonicvs")
            

            dirlist = tdir.GetListOfKeys()
            iter = dirlist.MakeIterator()
            key = iter.Next()
            td = None
            while key:
                if key.GetClassName() == 'TH1D':
                    td = key.ReadObj()
                    if ((td.GetName().find('Cluster_x_position_resolution') >= 0 ) and (td.GetName().find('Cluster_x_position_resolution_1Ch') < 0 )) : 
                        histoResolution.append(td) 
                    else : 
                        histo1D.append(td) 
                if key.GetClassName() == 'TH2D':
                    td = key.ReadObj()
                    histo2D.append(td)
                key = iter.Next()
            self.Draw2DHisto(histo2D,ClusterMonicvs,OutFileMantisse)

            self.Draw1DHistoBar(histo1D,ClusterMonicvs,OutFileMantisse)

            self.DrawResolutionSingleGaussianFit(histoResolution,ClusterMonicvs,OutFileMantisse)

            self.DrawResolutionDoubleGaussianFit(histoResolution,ClusterMonicvs,OutFileMantisse)

            
    def ClusterSizeOneTrouble(self):
        RootDirectory = 'FTClusterMonitor'
        for files in self.ListOfFiles:
            tfile = TFile.Open(files)
            OutFileMantisse = files[:files.find('-histos')]+"/"+RootDirectory+"_"+time.strftime('%y-%m-%d_%H-%M',time.localtime())
            self.CreateDir(OutFileMantisse)
            tdir = tfile.GetDirectory(RootDirectory)
            for type in {"x_position", "Channel"} : 
                All = tdir.Get("Cluster_"+type+"_resolution_1Ch")
                Cell0 = tdir.Get("Cluster_"+type+"_resolution_1Ch_Cell0")
                Cell127 = tdir.Get("Cluster_"+type+"_resolution_1Ch_Cell127")
                Cell63 = tdir.Get("Cluster_"+type+"_resolution_1Ch_Cell63")
                Cell64 = tdir.Get("Cluster_"+type+"_resolution_1Ch_Cell64")
                CellNeighbour = tdir.Get("Cluster_"+type+"_resolution_1Ch_WithNeighbouringCell")

                All.SetLineColor(4)
                Cell0.SetLineColor(2)
                Cell127.SetLineColor(7)
                Cell63.SetLineColor(6)     
                Cell64.SetLineColor(40)         
                CellNeighbour.SetLineColor(28)
                # Standard drawing
                ClusterMoniSizeOneTroublecvs= TCanvas("ClusterSizeOneTroublecvs","ClusterSizeOneTrouble")
                ClusterMoniSizeOneTroublecvs.SetGrid()
                ClusterMoniSizeOneTroublecvs.cd()
                All.Draw("H")
                Cell0.Draw("HSAME")
                Cell127.Draw("HSAME")
                Cell63.Draw("HSAME")
                Cell64.Draw("HSAME")
                CellNeighbour.Draw("HSAME")
                Leg = TLegend(0.11,0.7,0.4,0.89)
                Leg.AddEntry(All,"All Cells","l")
                Leg.AddEntry(Cell63,"Cell 63","l")
                Leg.AddEntry(Cell64,"Cell 64","l")
                Leg.AddEntry(Cell0,"Cell 0","l")
                Leg.AddEntry(Cell127,"Cell 127","l")
                Leg.AddEntry(CellNeighbour,"Cells with left/right neighbour","l")
                Leg.Draw()
                ClusterMoniSizeOneTroublecvs.SaveAs(OutFileMantisse+"/"+(OutFileMantisse.split('/')[0])+"_"+All.GetName()+self.OutFileType)
                
                # Stack drawing
                Cell0.SetFillColor(2)
                Cell127.SetFillColor(7)
                Cell63.SetFillColor(6)
                Cell64.SetFillColor(40)
                CellNeighbour.SetFillColor(28)                  
                HistoStack = THStack("HistoStack","Stacked 1D histograms")
                HistoStack.Add(Cell63)
                HistoStack.Add(Cell64)
                HistoStack.Add(Cell0)
                HistoStack.Add(Cell127)
                HistoStack.Add(CellNeighbour)

                ClusterMoniSizeOneTroubleStackcvs= TCanvas("ClusterSizeOneTroubleStackcvs","ClusterSizeOneTroubleStack")
                ClusterMoniSizeOneTroubleStackcvs.SetGrid()
                ClusterMoniSizeOneTroubleStackcvs.cd()
                HistoStack.Draw("H")
                HistoStack.GetXaxis().SetTitle(All.GetXaxis().GetTitle());
                HistoStack.GetYaxis().SetTitle(All.GetYaxis().GetTitle());
                #ClusterMoniSizeOneTroubleStackcvs.Update()
                All.Draw("HSAME")
                LegStack = TLegend(0.11,0.7,0.4,0.89)
                LegStack.AddEntry(All,"All Cells","l")
                LegStack.AddEntry(Cell63,"Cell 63","f")
                LegStack.AddEntry(Cell64,"Cell 64","f")
                LegStack.AddEntry(Cell0,"Cell 0","l=f")
                LegStack.AddEntry(Cell127,"Cell 127","f")
                LegStack.AddEntry(CellNeighbour,"Cells with left/right neighbour","f")                
                LegStack.Draw()
                ClusterMoniSizeOneTroubleStackcvs.SaveAs(OutFileMantisse+"/"+(OutFileMantisse.split('/')[0])+"_"+All.GetName()+"_Stack"+self.OutFileType)

            # Details on cells with neighbours
            Cell1to4 = tdir.Get("Cluster_Channel_resolution_1Ch_Cell1to4")       
            Cell59to62 = tdir.Get("Cluster_Channel_resolution_1Ch_Cell59to62") 
            Cell65to68 = tdir.Get("Cluster_Channel_resolution_1Ch_Cell65to68")  
            Cell122to126 = tdir.Get("Cluster_Channel_resolution_1Ch_Cell122to126")
            CellAllOthers = tdir.Get("Cluster_Channel_resolution_1Ch_AllOtherNeighbors")           
            Cell1to4.SetLineColor(2)
            Cell1to4.SetFillColor(2)            
            Cell59to62.SetLineColor(3)
            Cell59to62.SetFillColor(3)            
            Cell65to68.SetLineColor(4)
            Cell65to68.SetFillColor(4)
            Cell122to126.SetLineColor(5)
            Cell122to126.SetFillColor(5)
            CellAllOthers.SetLineColor(6)
            CellAllOthers.SetFillColor(6)
            NeighbourStack = THStack("NeighbourStack","Stacked 1D histograms")
            NeighbourStack.Add(Cell1to4)
            NeighbourStack.Add(Cell59to62)
            NeighbourStack.Add(Cell65to68)
            NeighbourStack.Add(Cell122to126)
            NeighbourStack.Add(CellAllOthers)

            ClusterMoniSizeOneTroubleNeighbourhoodcvs= TCanvas("ClusterSizeOneTroubleNeighbourhoodcvs","ClusterSizeOneTroubleNeighbourhood")
            ClusterMoniSizeOneTroubleNeighbourhoodcvs.SetGrid()
            ClusterMoniSizeOneTroubleNeighbourhoodcvs.cd()
            NeighbourStack.Draw("nostack")
            NeighbourStack.GetXaxis().SetTitle(Cell1to4.GetXaxis().GetTitle());
            NeighbourStack.GetYaxis().SetTitle(Cell1to4.GetYaxis().GetTitle());
            CellNeighbour.Draw("SAME")
            ClusterMoniSizeOneTroubleNeighbourhoodcvs.SaveAs(OutFileMantisse+"/"+(OutFileMantisse.split('/')[0])+"_NeighboringStudy_Stack"+self.OutFileType)

            # Details on cells with/without Cluster size=4 neighbours
            #CellWithCl4Neighbour = tdir.Get("Cluster_Channel_resolution_1Ch_Cell1to4")
            CellWithCl4Neighbour = tdir.Get("Cluster_Channel_resolution_1Ch_withSize4neigh")
            #CellWithoutCl4Neighbour = tdir.Get("Cluster_Channel_resolution_1Ch_Cell1to4")
            CellWithoutCl4Neighbour = tdir.Get("Cluster_Channel_resolution_1Ch_withoutSize4neigh")
            
            CellWithCl4Neighbour.SetLineColor(2)
            CellWithCl4Neighbour.SetFillColor(2)
            CellWithoutCl4Neighbour.SetLineColor(3)
            CellWithoutCl4Neighbour.SetFillColor(3)           
            Clus4NeighbourStack = THStack("Clus4NeighbourStack","St histograms")
            Clus4NeighbourStack.Add(CellWithCl4Neighbour)
            Clus4NeighbourStack.Add(CellWithoutCl4Neighbour)
            Clus4TroubleNeighbourhoodcvs= TCanvas("Clus4TroubleNeighbourhoodcvs","Clus4TroubleNeighbourhoodcvs")
            Clus4TroubleNeighbourhoodcvs.SetGrid()
            Clus4TroubleNeighbourhoodcvs.cd()
            Clus4NeighbourStack.Draw("H")
            Clus4NeighbourStack.GetXaxis().SetTitle(CellWithCl4Neighbour.GetXaxis().GetTitle());
            Clus4NeighbourStack.GetYaxis().SetTitle(CellWithCl4Neighbour.GetYaxis().GetTitle())
            All.Draw("HSAME")
            LegClus4Size = TLegend(0.11,0.7,0.4,0.89)
            LegClus4Size.AddEntry(All,"All Cells","l")
            LegClus4Size.AddEntry(CellWithCl4Neighbour,"With size 4 neighbour ","f")
            LegClus4Size.AddEntry(CellWithoutCl4Neighbour,"Without size 4 neighbour ","f")
            LegClus4Size.Draw()
            Clus4TroubleNeighbourhoodcvs.SaveAs(OutFileMantisse+"/"+(OutFileMantisse.split('/')[0])+"_CluS4NeighboringStudy_Stack"+self.OutFileType)

            
def main(argv):
    input = "2BitsADC_2Bits-100ev-histos.root"
    print "Main"
    print input
    
    try:                                
        opts, args = getopt.getopt(argv, "hi:", ["help", "input="])
    except getopt.GetoptError as err:
        print(err)
        print "Error"
        usage()                         
        sys.exit(2)              
    for opt, arg in opts:
        print opt, arg
        if opt in ("-h", "--help"):      
            usage()                     
            sys.exit()                                 
        elif opt in ("-i", "--input"): 
            input = arg               

    print input
    
    t = Read([ input ])
    t.DepositPlots()
    t.DigitPlots()
    t.ClusterPlots()
    t.DepositMoniPlots()
    t.ClusterMoniPlots()
    t.ClusterSizeOneTrouble()
    raw_input("Enter any key to close")
    
if __name__ == "__main__":
    main(sys.argv[1:])
