#! /usr/bin/env python
#python ReadDigitisation.py -i 2BitsADC_Initial-100ev-histos.root
__author__ = 'Eric Cogneras'

import os,sys,getopt,time
from ROOT import gROOT, TFile, TCanvas, TH1D,kBlue, kRed, gStyle, TLine, TF1, TLatex

class Read:
    def __init__(self, files= ["MiniBias_v20_nu25_EC-histos.root"], outtype = ".gif"):
        self.ListOfFiles = files
        self.OutFileType = outtype
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
        for i in range(2):
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
        for i in range(2):
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
            comment = TLatex(.6,.6,"#sigma = "+str("%.0f" % round(sgngaus.GetParameter(2)))+" #mum")                
            comment.SetTextColor(kRed)
            comment.SetNDC(1)

            for i in range(2):
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

            comment = TLatex(.6,.6,"#sigma = "+str("%.0f" % round(doublgaus.GetParameter(2)))+" #mum")                
            comment.SetTextColor(kBlue)
            comment.SetNDC(1)

            for i in range(2):
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
        gStyle.SetOptStat(0)
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
    #t.DigitPlots()
    #t.ClusterPlots()
    
if __name__ == "__main__":
    main(sys.argv[1:])
