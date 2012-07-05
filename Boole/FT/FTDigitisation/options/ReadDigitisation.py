#! /usr/bin/env python
from ROOT import gROOT, TFile, TCanvas, TH1D
OutFileType = ".jpg"
ListOfFiles =["Bs_mumu_v4_nu20_EC-histos.root","Bs_mumu_v4_nu25_EC-histos.root","Bs_mumu_v4_nu50_EC-histos.root"]
for files in ListOfFiles:
    tfile = TFile.Open(files)
    OutFileMantisse = files[:files.find('-histos')]
    #Deposit
    Depcvs = TCanvas("Depcvs","Depcvs")
    Depcvs.SetLogy()
    
    DepEnergyPerHit=tfile.Get('MCFTDepositCreator/DepEnergyPerHit')
    DepEnergyPerChannel=tfile.Get('MCFTDepositCreator/DepEnergyPerChannel')
    DepNbChannel=tfile.Get('MCFTDepositCreator/DepNbChannel')
    DepFiredChannel=tfile.Get('MCFTDepositCreator/DepFiredChannel')

    DepEnergyPerHit.Draw()
    Depcvs.SaveAs(OutFileMantisse+"_DepEnergyPerHit"+OutFileType)

    DepEnergyPerChannel.Draw()
    Depcvs.SaveAs(OutFileMantisse+"_DepEnergyPerChannel"+OutFileType)

    DepNbChannel.Draw()
    Depcvs.SaveAs(OutFileMantisse+"_DepNbChannel"+OutFileType)
    
    DepFiredChannel.Draw()
    Depcvs.SaveAs(OutFileMantisse+"_DepFiredChannel"+OutFileType)

    #Digit
    Digcvs= TCanvas("Digcvs","Digcvs")
    Digcvs.SetLogy()
    DigADCCount=tfile.Get('MCFTDigitCreator/DigADCCount')
    DigADCCountvsEnergy=tfile.Get('MCFTDigitCreator/DigADCCountvsEnergy')
    
    DigADCCount.Draw()
    Digcvs.SaveAs(OutFileMantisse+"_DigADCCount"+OutFileType)
    DigADCCountvsEnergy.Draw()
    Digcvs.SaveAs(OutFileMantisse+"_DigADCCountvsEnergy"+OutFileType)


    # Cluster
    Cluscvs= TCanvas("Cluscvs","Cluscvs")
    Cluscvs.SetLogy()
    ClusChannelID=tfile.Get('FTClusterCreator/ClusChannelID')
    ClusFraction=tfile.Get('FTClusterCreator/ClusFraction')
    ClusSize=tfile.Get('FTClusterCreator/ClusSize')
    ClusCharge=tfile.Get('FTClusterCreator/ClusCharge')

    ClusChannelID.Draw()
    Cluscvs.SaveAs(OutFileMantisse+"_ClusChannelID"+OutFileType)

    ClusFraction.Draw()
    Cluscvs.SaveAs(OutFileMantisse+"_ClusFraction"+OutFileType)
    
    ClusSize.Draw()
    Cluscvs.SaveAs(OutFileMantisse+"_ClusSize"+OutFileType)

    ClusCharge.Draw()
    Cluscvs.SaveAs(OutFileMantisse+"_ClusCharge"+OutFileType)
