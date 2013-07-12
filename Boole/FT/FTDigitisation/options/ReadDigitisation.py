#! /usr/bin/env python
from ROOT import gROOT, TFile, TCanvas, TH1D,kBlue,kRed,gStyle, TLine, TF1,TLatex

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
        
    def DepositPlots(self):
        #Rstyle = TStyle(ROOT.TStyle)
        gStyle.SetOptStat(0)
        gStyle.SetPalette(1)
        #gROOT.SetStyle("Rstyle")
        for files in self.ListOfFiles:
            tfile = TFile.Open(files)
            OutFileMantisse = files[:files.find('-histos')]

            FibreAttenuationMap = tfile.Get('MCFTDepositCreator/FibreAttenuationMap')
            RadiationAttMap     = tfile.Get('MCFTDepositCreator/RadiationAttMap')
            ReflectionContributionMap = tfile.Get('MCFTDepositCreator/ReflectionContributionMap')
            ReflectionAttenuationMap = tfile.Get('MCFTDepositCreator/ReflectionAttenuationMap')
            FinalAttenuationMap = tfile.Get('MCFTDepositCreator/FinalAttenuationMap')

            HitEntryPosition      = tfile.Get('MCFTDepositCreator/HitEntryPosition')
            EnergyOfHit           = tfile.Get('MCFTDepositCreator/EnergyOfHit')
            EnergyOfHitZOOM       = tfile.Get('MCFTDepositCreator/EnergyOfHitZOOM')

            LayerStereoAngle      = tfile.Get('MCFTDepositCreator/LayerStereoAngle')
            LayerHoleRadius       = tfile.Get('MCFTDepositCreator/LayerHoleRadius')
            LayerHalfSizeY        = tfile.Get('MCFTDepositCreator/LayerHalfSizeY')
            CheckNbChannel        = tfile.Get('MCFTDepositCreator/CheckNbChannel')

            FibreLengh            = tfile.Get('MCFTDepositCreator/FibreLengh')
            FibreFraction         = tfile.Get('MCFTDepositCreator/FibreFraction')
            AttenuationFactor     = tfile.Get('MCFTDepositCreator/AttenuationFactor')
            EnergyDepositedInCell = tfile.Get('MCFTDepositCreator/EnergyDepositedInCell')
            EnergyRecordedInCell  = tfile.Get('MCFTDepositCreator/EnergyRecordedInCell')


            Depositcvs = TCanvas("Depositcvs","Depositcvs")
            Extension = self.OutFileType
            
            # 2D plots
            FibreAttenuationMap.Draw("colz")
            self.DrawQuarterEdge()
            #for Extension in self.OutFileType : 
            Depositcvs.SaveAs(OutFileMantisse+"_FibreAttenuationMap"+Extension)

            RadiationAttMap.Draw("colz")
            self.DrawQuarterEdge()
            #for Extension in self.OutFileType : 
            Depositcvs.SaveAs(OutFileMantisse+"_RadiationAttMap"+Extension)

            ReflectionContributionMap.Draw("colz")
            self.DrawQuarterEdge()
            #for Extension in self.OutFileType : 
            Depositcvs.SaveAs(OutFileMantisse+"_ReflectionContributionMap"+Extension)

            ReflectionAttenuationMap.Draw("colz")
            self.DrawQuarterEdge()
            #for Extension in self.OutFileType : 
            Depositcvs.SaveAs(OutFileMantisse+"ReflectionAttenuationMap"+Extension)
            
            FinalAttenuationMap.Draw("colz")
            self.DrawQuarterEdge()
            #for Extension in self.OutFileType : 
            Depositcvs.SaveAs(OutFileMantisse+"_FinalAttenuationMap"+Extension)

            HitEntryPosition.Draw("colz")
            #for Extension in self.OutFileType : 
            Depositcvs.SaveAs(OutFileMantisse+"_HitEntryPosition"+Extension)

            # 1D plots linear scale
           # LayerHalfSizeY.SetMarkerStyle(8)
           # LayerHalfSizeY.SetMarkerColor(kBlue)
           # LayerHalfSizeY.Draw("P")
            #for Extension in self.OutFileType : 
            Depositcvs.SaveAs(OutFileMantisse+"_LayerHalfSizeY"+Extension)

            if(LayerStereoAngle):
                LayerStereoAngle.SetMarkerStyle(8)
                LayerStereoAngle.SetMarkerColor(kBlue)
                LayerStereoAngle.Draw("P")
                #for Extension in self.OutFileType : 
                Depositcvs.SaveAs(OutFileMantisse+"_LayerStereoAngle"+Extension)
            else:
                print "!!!!! BEWARE : LayerStereoAngle NOT FOUND"

            if(LayerHoleRadius):
                LayerHoleRadius.SetMarkerStyle(8)
                LayerHoleRadius.SetMarkerColor(kBlue)
                LayerHoleRadius.Draw("P")
                #for Extension in self.OutFileType : 
                Depositcvs.SaveAs(OutFileMantisse+"_LayerHoleRadius"+Extension)
            else:
                print "!!!!! BEWARE : LayerHoleRadius NOT FOUND"
            
            # Empty !!!!
            if(FibreLengh):
                FibreLengh.SetMarkerStyle(8)
                FibreLengh.SetMarkerColor(kBlue)
                FibreLengh.Draw("P")
                #for Extension in self.OutFileType : 
                Depositcvs.SaveAs(OutFileMantisse+"_FibreLengh"+Extension)
            else:
                print "!!!!! BEWARE : FibreLengh NOT FOUND"                
            # Plots in linear and log scales
            for i in range(2):
                if i==1:
                    Depositcvs.SetLogy()
                    Extension = "_LOG"+self.OutFileType
                    
                EnergyOfHit.SetMarkerStyle(8)
                EnergyOfHit.SetMarkerColor(kBlue)
                EnergyOfHit.Draw("P")
                Depositcvs.SaveAs(OutFileMantisse+"_EnergyOfHit"+Extension)

                EnergyOfHitZOOM.SetMarkerStyle(8)
                EnergyOfHitZOOM.SetMarkerColor(kBlue)
                EnergyOfHitZOOM.Draw("P")
                Depositcvs.SaveAs(OutFileMantisse+"_EnergyOfHitZOOM"+Extension)

                if(CheckNbChannel):
                    CheckNbChannel.SetMarkerStyle(8)
                    CheckNbChannel.SetMarkerColor(kBlue)
                    CheckNbChannel.Draw("P")
                    Depositcvs.SaveAs(OutFileMantisse+"_CheckNbChannel"+Extension)
                else:
                    print "!!!!! BEWARE : CheckNbChannel NOT FOUND"       

                FibreFraction.SetMarkerStyle(8)
                FibreFraction.SetMarkerColor(kBlue)
                FibreFraction.Draw("P")
                Depositcvs.SaveAs(OutFileMantisse+"_FibreFraction"+Extension)
                
                AttenuationFactor.SetMarkerStyle(8)
                AttenuationFactor.SetMarkerColor(kBlue)
                AttenuationFactor.Draw("LP")
                Depositcvs.SaveAs(OutFileMantisse+"_AttenuationFactor"+Extension)
                
                EnergyDepositedInCell.SetMarkerStyle(8)
                EnergyDepositedInCell.SetMarkerColor(kBlue)
                EnergyDepositedInCell.Draw("LP")
                Depositcvs.SaveAs(OutFileMantisse+"_EnergyDepositedInCell"+Extension)
                
                EnergyRecordedInCell.SetMarkerStyle(8)
                EnergyRecordedInCell.SetMarkerColor(kBlue)
                EnergyRecordedInCell.Draw("LP")
                Depositcvs.SaveAs(OutFileMantisse+"_EnergyRecordedInCell"+Extension)
            tfile.Close()
                 

    def DigitPlots(self):
        gStyle.SetOptStat(0)
        gStyle.SetPalette(1)
        gStyle.SetFuncWidth(3)
        
        for files in self.ListOfFiles:
            tfile = TFile.Open(files)
            OutFileMantisse = files[:files.find('-histos')]

            
#            ChanNoise_nb                  = tfile.Get('MCFTDigitCreator/ChanNoise_nb')
#            ChanNoise_layer               = tfile.Get('MCFTDigitCreator/ChanNoise_layer')
#            ChanNoise_module              = tfile.Get('MCFTDigitCreator/ChanNoise_module')
#            ChanNoise_fibermat            = tfile.Get('MCFTDigitCreator/ChanNoise_fibermat')
#            ChanNoise_sipm                = tfile.Get('MCFTDigitCreator/ChanNoise_sipm')            
#            ChanNoise_channel             = tfile.Get('MCFTDigitCreator/ChanNoise_channel')
            FiredChannelID                = tfile.Get('MCFTDigitCreator/FiredChannelID')
            HitPerChannel                 = tfile.Get('MCFTDigitCreator/HitPerChannel')
            EnergyPerHitPerChannel        = tfile.Get('MCFTDigitCreator/EnergyPerHitPerChannel')
            EnergyPerHitPerChannelZOOM    = tfile.Get('MCFTDigitCreator/EnergyPerHitPerChannelZOOM')
            EnergyPerHitPerChannelBIGZOOM = tfile.Get('MCFTDigitCreator/EnergyPerHitPerChannelBIGZOOM')
            EnergyPerChannel              = tfile.Get('MCFTDigitCreator/EnergyPerChannel')
            EnergyPerChannelZOOM          = tfile.Get('MCFTDigitCreator/EnergyPerChannelZOOM')
            EnergyPerChannelBIGZOOM       = tfile.Get('MCFTDigitCreator/EnergyPerChannelBIGZOOM')
            ADCPerChannel                 = tfile.Get('MCFTDigitCreator/ADCPerChannel')
            ADCGain                       = tfile.Get('MCFTDigitCreator/ADCGain')
            ADCGainZOOM                   = tfile.Get('MCFTDigitCreator/ADCGainZOOM')           
            ADCGainBIGZOOM                = tfile.Get('MCFTDigitCreator/ADCGainBIGZOOM')
            Digitcvs= TCanvas("Digitcvs","Digitcvs")
            Extension = self.OutFileType

            # 2D plots
            ADCGain_PRX=ADCGain.ProfileX()
            ADCGain.Draw("colz")
            ADCGain_PRX.SetLineWidth(5)
            ADCGain_PRX.SetLineColor(7)
            ADCGain_PRX.Draw("SAME")
            FitFct = TF1("FitFct", "pol1", 0, 0.3)
            ADCGain_PRX.Fit(FitFct,"","SAME", 0, 0.3)
            Comment = TLatex(0.3,10,"Slope = "+str(FitFct.GetParameter(1)))
            Comment.SetTextColor(kRed)
            Comment.Draw()
            Digitcvs.SaveAs(OutFileMantisse+"_ADCGain"+Extension)

            # 1D plots linear scale
#            ChanNoise_nb.SetMarkerStyle(8)
#            ChanNoise_nb.SetMarkerColor(kBlue)
#            ChanNoise_nb.Draw("P")
#            Digitcvs.SaveAs(OutFileMantisse+"_ChanNoise_nb"+Extension)

#            ChanNoise_layer.SetMarkerStyle(8)
#            ChanNoise_layer.SetMarkerColor(kBlue)
#            ChanNoise_layer.Draw("P")
#            Digitcvs.SaveAs(OutFileMantisse+"_ChanNoise_layer"+Extension)

#            ChanNoise_module.SetMarkerStyle(8)
#            ChanNoise_module.SetMarkerColor(kBlue)
#            ChanNoise_module.Draw("P")
#            Digitcvs.SaveAs(OutFileMantisse+"_ChanNoise_module"+Extension)

#            ChanNoise_fibermat.SetMarkerStyle(8)
#            ChanNoise_fibermat.SetMarkerColor(kBlue)
#            ChanNoise_fibermat.Draw("P")
#            Digitcvs.SaveAs(OutFileMantisse+"_ChanNoise_fibermat"+Extension)

#            ChanNoise_sipm.SetMarkerStyle(8)
#            ChanNoise_sipm.SetMarkerColor(kBlue)
#            ChanNoise_sipm.Draw("P")
#            Digitcvs.SaveAs(OutFileMantisse+"_ChanNoise_sipm"+Extension)

#            ChanNoise_channel.SetMarkerStyle(8)
#            ChanNoise_channel.SetMarkerColor(kBlue)
#            ChanNoise_channel.Draw("P")
#            Digitcvs.SaveAs(OutFileMantisse+"_ChanNoise_channel"+Extension)
            
            FiredChannelID.SetMarkerStyle(8)
            FiredChannelID.SetMarkerColor(kBlue)
            FiredChannelID.Draw("P")
            Digitcvs.SaveAs(OutFileMantisse+"_FiredChannelID"+Extension)

            
            ADCPerChannel.SetMarkerStyle(8)
            ADCPerChannel.SetMarkerColor(kBlue)
            ADCPerChannel.Draw("P")
            Digitcvs.SaveAs(OutFileMantisse+"_ADCPerChannel"+Extension)

            # Plots in linear and log scales
            for i in range(2):
                if i==1:
                    Digitcvs.SetLogy()
                    Extension = "_LOG"+self.OutFileType
                    
                HitPerChannel.SetMarkerStyle(8)
                HitPerChannel.SetMarkerColor(kBlue)
                HitPerChannel.Draw("P")
                Digitcvs.SaveAs(OutFileMantisse+"_HitPerChannel"+Extension)
            
                EnergyPerHitPerChannel.SetMarkerStyle(8)
                EnergyPerHitPerChannel.SetMarkerColor(kBlue)
                EnergyPerHitPerChannel.Draw("P")
                Digitcvs.SaveAs(OutFileMantisse+"_EnergyPerHitPerChannel"+Extension)

                EnergyPerHitPerChannelZOOM.SetMarkerStyle(8)
                EnergyPerHitPerChannelZOOM.SetMarkerColor(kBlue)
                EnergyPerHitPerChannelZOOM.Draw("P")
                Digitcvs.SaveAs(OutFileMantisse+"_EnergyPerHitPerChannelZOOM"+Extension)

                EnergyPerHitPerChannelBIGZOOM.SetMarkerStyle(8)
                EnergyPerHitPerChannelBIGZOOM.SetMarkerColor(kBlue)
                EnergyPerHitPerChannelBIGZOOM.Draw("P")
                Digitcvs.SaveAs(OutFileMantisse+"_EnergyPerHitPerChannelBIGZOOM"+Extension)

                EnergyPerChannel.SetMarkerStyle(8)
                EnergyPerChannel.SetMarkerColor(kBlue)
                EnergyPerChannel.Draw("P")
                Digitcvs.SaveAs(OutFileMantisse+"_EnergyPerChannel"+Extension)

                EnergyPerChannelZOOM.SetMarkerStyle(8)
                EnergyPerChannelZOOM.SetMarkerColor(kBlue)
                EnergyPerChannelZOOM.Draw("P")
                Digitcvs.SaveAs(OutFileMantisse+"_EnergyPerChannelZOOM"+Extension)
                                                                                 
                EnergyPerChannelBIGZOOM.SetMarkerStyle(8)
                EnergyPerChannelBIGZOOM.SetMarkerColor(kBlue)
                EnergyPerChannelBIGZOOM.Draw("P")
                Digitcvs.SaveAs(OutFileMantisse+"_EnergyPerChannelBIGZOOM"+Extension)
                #DigADCCount.SetMarkerStyle(8)
                #DigADCCount.Draw("P")
                #Digitcvs.SaveAs(OutFileMantisse+"_DigADCCount"+Extension)
        
                #DigADCCountvsEnergy.SetMarkerStyle(8)
                #DigADCCountvsEnergy.Draw("SURF2")
                #Digitcvs.SaveAs(OutFileMantisse+"_DigADCCountvsEnergy"+Extension)

        

    def ClusterPlots(self):
        for files in self.ListOfFiles:
            tfile = TFile.Open(files)
            OutFileMantisse = files[:files.find('-histos')]
            

            ClusChannelID         = tfile.Get('FTClusterCreator/ClusChannelID')
            ClusSiPMID            = tfile.Get('FTClusterCreator/ClusSiPMID')
            ClusFraction          = tfile.Get('FTClusterCreator/ClusFraction')
            ClusSize              = tfile.Get('FTClusterCreator/ClusSize')
            ClusCharge            = tfile.Get('FTClusterCreator/ClusCharge')
            ClusChargeZoom        = tfile.Get('FTClusterCreator/ClusChargeZOOM')
            ClusChargeVsSize      = tfile.Get('FTClusterCreator/ClusChargevsSize') #2D
            ClusterPerSiPM        = tfile.Get('FTClusterCreator/ClusterPerSiPM')
            KeptHitEntryPosition  = tfile.Get('FTClusterCreator/KeptHitEntryPosition') #2D
            KeptHitEnergy         = tfile.Get('FTClusterCreator/KeptHitEnergy')
            KeptHitPDGId          = tfile.Get('FTClusterCreator/KeptHitPDGId')
            LostHitEntryPosition  = tfile.Get('FTClusterCreator/LostHitEntryPosition') #2D
            LostHitEnergy         = tfile.Get('FTClusterCreator/LostHitEnergy')
            LostHitPDGId          = tfile.Get('FTClusterCreator/LostHitPDGId')          
   
            Clustercvs= TCanvas("Clustercvs","Clustercvs")
            Extension = self.OutFileType

            ClusChargeVsSize.SetMarkerStyle(8)
            ClusChargeVsSize.SetMarkerColor(kBlue)
            ClusChargeVsSize.Draw("COLORZ")
            Clustercvs.SaveAs(OutFileMantisse+"_ClusChargeVsSize"+Extension)

            ClusterSize64 = files[:files.find('-histos')]
            for i in range(2):
                if i==1:
                    Clustercvs.SetLogy()
                    Extension = "_LOG"+self.OutFileType

                ClusChannelID.SetMarkerStyle(8)
                ClusChannelID.SetMarkerColor(kBlue)
                ClusChannelID.Draw("P")
                Clustercvs.SaveAs(OutFileMantisse+"_ClusChannelID"+Extension)

                ClusSiPMID.SetMarkerStyle(8)
                ClusSiPMID.SetMarkerColor(kBlue)
                ClusSiPMID.Draw("P")
                Clustercvs.SaveAs(OutFileMantisse+"_ClusSiPMID"+Extension)
            
                ClusFraction.SetMarkerStyle(8)
                ClusFraction.SetMarkerColor(kBlue)
                ClusFraction.Draw("P")
                Clustercvs.SaveAs(OutFileMantisse+"_ClusFraction"+Extension)
                
                ClusSize.SetMarkerStyle(8)
                ClusSize.SetMarkerColor(kBlue)
                ClusSize.Draw("P")
                Clustercvs.SaveAs(OutFileMantisse+"_ClusSize"+Extension)
            
                ClusCharge.SetMarkerStyle(8)
                ClusCharge.SetMarkerColor(kBlue)
                ClusCharge.Draw("P")
                Clustercvs.SaveAs(OutFileMantisse+"_ClusCharge"+Extension)
                
                ClusChargeZoom.SetMarkerStyle(8)
                ClusChargeZoom.SetMarkerColor(kBlue)
                ClusChargeZoom.Draw("P")
                Clustercvs.SaveAs(OutFileMantisse+"_ClusChargeZoom"+Extension)

                ClusterPerSiPM.SetMarkerStyle(8)
                ClusterPerSiPM.SetMarkerColor(kBlue)
                ClusterPerSiPM.Draw("P")
                Clustercvs.SaveAs(OutFileMantisse+"_ClusterPerSiPM"+Extension)
        
def main():
    #t = Read()
   # t = Read(["Bs_mumu_v6_nu325_EC-histos.root","Bs_mumu_v6_nu65_EC-histos.root","MiniBias_v6_nu325_EC-histos.root","MiniBias_v6_nu65_EC-histos.root"],".eps")
    t = Read(["MiniBias_v20_nu25_EC-histos.root"])
    #t = Read(["Bs_mumu_v6_nu65_EC-histos.root"])
    #t = Read(["Bs_mumu_v6_nu325_EC-histos.root","Bs_mumu_v6_nu65_EC-histos.root","MiniBias_v6_nu325_EC-histos.root","MiniBias_v6_nu65_EC-histos.root"])
    #t = Read( ["MiniBias_v6_nu325_EC-histos.root"] )
    t.DepositPlots()
    t.DigitPlots()
    t.ClusterPlots()
    
if __name__ == "__main__":
    main()
