#! /usr/bin/env python
from ROOT import gROOT, TFile, TCanvas, TH1D
OutFileType = ".jpg"
ListOfFiles =["Bs_mumu_v4_nu20_EC-histos.root"]#,"Bs_mumu_v4_nu25_EC-histos.root","Bs_mumu_v4_nu50_EC-histos.root"]
for files in ListOfFiles:
    tfile = TFile.Open(files)
    OutFileMantisse = files[:files.find('-histos')]
    CheckEnergyPerHit=tfile.Get('MCFTDepositMonitor/CheckEnergyPerHit')
    CheckStereoAngle=tfile.Get('MCFTDepositMonitor/CheckStereoAngle')
    CheckHoleRadius=tfile.Get('MCFTDepositMonitor/CheckHoleRadius')
    CheckLayerHalfSizeY=tfile.Get('MCFTDepositMonitor/CheckLayerHalfSizeY')

    CheckFibreLengh=tfile.Get('MCFTDepositMonitor/CheckFibreLengh')
    CheckFibreFraction=tfile.Get('MCFTDepositMonitor/CheckFibreFraction')
    CheckAttFactor=tfile.Get('MCFTDepositMonitor/CheckAttFactor')
    CheckEnergyPerCell=tfile.Get('MCFTDepositMonitor/CheckEnergyPerCell')
    CheckNbChannel=tfile.Get('MCFTDepositMonitor/CheckNbChannel')


    DepFiredChannel=tfile.Get('MCFTDepositMonitor/DepFiredChannel')
    DepHitPerChannel=tfile.Get('MCFTDepositMonitor/DepHitPerChannel')
    DepEnergyPerChannel=tfile.Get('MCFTDepositMonitor/DepEnergyPerChannel')
    DepEnergyPerChannelZoom=tfile.Get('MCFTDepositMonitor/DepEnergyPerChannelZoom')
    DepEnergyPerChannelBigZoom=tfile.Get('MCFTDepositMonitor/DepEnergyPerChannelBigZoom')
    #DepNbFL=tfile.Get('MCFTDepositMonitor/DepNbFL')
    #DepNbFF=tfile.Get('MCFTDepositMonitor/DepNbFF')


    Depcvs = TCanvas("Depcvs","Depcvs")

    for i in range(2):
        if i==1:
            Depcvs.SetLogy()
            OutFileType = "_LOG"+OutFileType

        CheckEnergyPerHit.Draw()
        Depcvs.SaveAs(OutFileMantisse+"_CheckEnergyPerHit"+OutFileType)

        CheckStereoAngle.SetMarkerStyle(8)
        CheckStereoAngle.Draw("P")
        Depcvs.SaveAs(OutFileMantisse+"_CheckStereoAngle"+OutFileType)

        CheckHoleRadius.SetMarkerStyle(8)
        CheckHoleRadius.Draw("P")
        Depcvs.SaveAs(OutFileMantisse+"_CheckHoleRadius"+OutFileType)

        CheckLayerHalfSizeY.SetMarkerStyle(8)
        CheckLayerHalfSizeY.Draw("P")
        Depcvs.SaveAs(OutFileMantisse+"_CheckLayerHalfSizeY"+OutFileType)

        CheckNbChannel.Draw()
        Depcvs.SaveAs(OutFileMantisse+"_CheckNbChannel"+OutFileType)

        CheckFibreLengh.SetMarkerStyle(8)
        CheckFibreLengh.Draw("P")
        Depcvs.SaveAs(OutFileMantisse+"_CheckFibreLengh"+OutFileType)

        CheckFibreFraction.SetMarkerStyle(8)
        CheckFibreFraction.Draw("P")
        Depcvs.SaveAs(OutFileMantisse+"_CheckFibreFraction"+OutFileType)

        CheckAttFactor.SetMarkerStyle(8)
        CheckAttFactor.Draw("LP")
        Depcvs.SaveAs(OutFileMantisse+"_CheckAttFactor"+OutFileType)

        CheckEnergyPerCell.SetMarkerStyle(8)
        CheckEnergyPerCell.Draw("LP")
        Depcvs.SaveAs(OutFileMantisse+"_CheckEnergyPerCell"+OutFileType)

        DepEnergyPerChannel.SetMarkerStyle(8)
        DepEnergyPerChannel.Draw("P")
        Depcvs.SaveAs(OutFileMantisse+"_DepEnergyPerChannel"+OutFileType)

        DepEnergyPerChannelZoom.SetMarkerStyle(8)
        DepEnergyPerChannelZoom.Draw("P")
        Depcvs.SaveAs(OutFileMantisse+"_DepEnergyPerChannelZoom"+OutFileType)

        DepEnergyPerChannelBigZoom.SetMarkerStyle(8)
        DepEnergyPerChannelBigZoom.Draw("P")
        Depcvs.SaveAs(OutFileMantisse+"_DepEnergyPerChannelBigZoom"+OutFileType)
        
  
        DepFiredChannel.SetMarkerStyle(8)
        DepFiredChannel.Draw("P")
        Depcvs.SaveAs(OutFileMantisse+"_DepFiredChannel"+OutFileType)

        #DepNbFL.Draw()
        #Depcvs.SaveAs(OutFileMantisse+"_DepNbFL"+OutFileType)
        
        #DepNbFF.Draw()
        #Depcvs.SaveAs(OutFileMantisse+"_DepNbFF"+OutFileType)


    
    #Digit
    OutFileType = ".jpg"
    Digcvs= TCanvas("Digcvs","Digcvs")

    DigADCCount=tfile.Get('MCFTDigitMonitor/DigADCCount')
    DigADCCountvsEnergy=tfile.Get('MCFTDigitMonitor/DigADCCountvsEnergy')
    for i in range(2):
        if i==1:
            Digcvs.SetLogy()
            OutFileType = "_LOG"+OutFileType
        DigADCCount.SetMarkerStyle(8)
        DigADCCount.Draw("P")
        Digcvs.SaveAs(OutFileMantisse+"_DigADCCount"+OutFileType)
        
        DigADCCountvsEnergy.SetMarkerStyle(8)
        DigADCCountvsEnergy.Draw("SURF2")
        Digcvs.SaveAs(OutFileMantisse+"_DigADCCountvsEnergy"+OutFileType)


    # Cluster
    OutFileType = ".jpg"
    Cluscvs= TCanvas("Cluscvs","Cluscvs")
    ClusChannelID=tfile.Get('FTClusterMonitor/ClusChannelID')
    ClusFraction=tfile.Get('FTClusterMonitor/ClusFraction')
    ClusSize=tfile.Get('FTClusterMonitor/ClusSize')
    ClusCharge=tfile.Get('FTClusterMonitor/ClusCharge')
    ClusChargeZoom=tfile.Get('FTClusterMonitor/ClusChargeZoom')
    ClusChargeVsSize=tfile.Get('FTClusterMonitor/ClusChargevsSize')
    
    for i in range(2):
        if i==1:
              Cluscvs.SetLogy()
              OutFileType = "_LOG"+OutFileType

        ClusChannelID.SetMarkerStyle(8)
        ClusChannelID.Draw("P")
        Cluscvs.SaveAs(OutFileMantisse+"_ClusChannelID"+OutFileType)

        ClusFraction.SetMarkerStyle(8)
        ClusFraction.Draw("P")
        Cluscvs.SaveAs(OutFileMantisse+"_ClusFraction"+OutFileType)

        ClusSize.SetMarkerStyle(8)
        ClusSize.Draw("P")
        Cluscvs.SaveAs(OutFileMantisse+"_ClusSize"+OutFileType)

        ClusCharge.SetMarkerStyle(8)
        ClusCharge.Draw("P")
        Cluscvs.SaveAs(OutFileMantisse+"_ClusCharge"+OutFileType)

        ClusChargeZoom.SetMarkerStyle(8)
        ClusChargeZoom.Draw("P")
        Cluscvs.SaveAs(OutFileMantisse+"_ClusChargeZoom"+OutFileType)

        ClusChargeVsSize.SetMarkerStyle(8)
        ClusChargeVsSize.Draw("COLORZ")
        Cluscvs.SaveAs(OutFileMantisse+"_ClusChargeVsSize"+OutFileType)
