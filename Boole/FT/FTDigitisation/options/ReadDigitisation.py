#! /usr/bin/env python
from ROOT import gROOT, TFile, TCanvas, TH1D,kBlue,kRed,gStyle, TLine, TF1,TLatex

class Read:
    def __init__(self, files=  ["Bs_mumu_v6_nu325_EC-histos.root"], outtype = ".gif"):
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
            Depositcvs.SaveAs(OutFileMantisse+"_FibreAttenuationMap"+Extension)

            RadiationAttMap.Draw("colz")
            self.DrawQuarterEdge()
            Depositcvs.SaveAs(OutFileMantisse+"_RadiationAttMap"+Extension)

            ReflectionContributionMap.Draw("colz")
            self.DrawQuarterEdge()
            Depositcvs.SaveAs(OutFileMantisse+"_ReflectionContributionMap"+Extension)

            ReflectionAttenuationMap.Draw("colz")
            self.DrawQuarterEdge()
            Depositcvs.SaveAs(OutFileMantisse+"ReflectionAttenuationMap"+Extension)
            
            FinalAttenuationMap.Draw("colz")
            self.DrawQuarterEdge()
            Depositcvs.SaveAs(OutFileMantisse+"_FinalAttenuationMap"+Extension)

            HitEntryPosition.Draw("colz")
            Depositcvs.SaveAs(OutFileMantisse+"_HitEntryPosition"+Extension)

            # 1D plots linear scale
            LayerHalfSizeY.SetMarkerStyle(8)
            LayerHalfSizeY.SetMarkerColor(kBlue)
            LayerHalfSizeY.Draw("P")
            Depositcvs.SaveAs(OutFileMantisse+"_LayerHalfSizeY"+Extension)

            LayerStereoAngle.SetMarkerStyle(8)
            LayerStereoAngle.SetMarkerColor(kBlue)
            LayerStereoAngle.Draw("P")
            Depositcvs.SaveAs(OutFileMantisse+"_LayerStereoAngle"+Extension)
                
            LayerHoleRadius.SetMarkerStyle(8)
            LayerHoleRadius.SetMarkerColor(kBlue)
            LayerHoleRadius.Draw("P")
            Depositcvs.SaveAs(OutFileMantisse+"_LayerHoleRadius"+Extension)
            
            # Empty !!!!
            FibreLengh.SetMarkerStyle(8)
            FibreLengh.SetMarkerColor(kBlue)
            FibreLengh.Draw("P")
            Depositcvs.SaveAs(OutFileMantisse+"_FibreLengh"+Extension)
                
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


                CheckNbChannel.SetMarkerStyle(8)
                CheckNbChannel.SetMarkerColor(kBlue)
                CheckNbChannel.Draw("P")
                Depositcvs.SaveAs(OutFileMantisse+"_CheckNbChannel"+Extension)
                

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
            ClusFraction          = tfile.Get('FTClusterCreator/ClusFraction')
            ClusSize              = tfile.Get('FTClusterCreator/ClusSize')
            ClusCharge            = tfile.Get('FTClusterCreator/ClusCharge')
            ClusChargeZoom        = tfile.Get('FTClusterCreator/ClusChargeZOOM')
            ClusChargeVsSize      = tfile.Get('FTClusterCreator/ClusChargevsSize') #2D
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
                
            for i in range(2):
                if i==1:
                    Clustercvs.SetLogy()
                    Extension = "_LOG"+self.OutFileType

                ClusChannelID.SetMarkerStyle(8)
                ClusChannelID.SetMarkerColor(kBlue)
                ClusChannelID.Draw("P")
                Clustercvs.SaveAs(OutFileMantisse+"_ClusChannelID"+Extension)
            
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
                
        
def main():
    t = Read()
    #t.DepositPlots()
    #t.DigitPlots()
    t.ClusterPlots()
    
if __name__ == "__main__":
    main()
