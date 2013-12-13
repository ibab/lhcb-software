from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import *

from Configurables import MCSTDepositCreator
#STRndmEffSelector

MCITDepositCreator = MCSTDepositCreator('MCITDepositCreator', DetType='IT')

# time of flight vector - always check that correct number stations defined
MCITDepositCreator.TofVector = [25.9, 28.3, 30.5]

# to add machine background
#MCITDepositCreator.SpillVector = ["/","/Prev/","/PrevPrev/", "/Next/", "/LHCBackground/"]
#MCITDepositCreator.SpillTimes = [0.,-25.,-50.,25. ,-3.3]


# charge sharing function  for IT
from Configurables import STChargeSharingTool
MCITDepositCreator.ChargeSharerTypes = ["320", "410" ]
MCITDepositCreator.addTool(STChargeSharingTool, name = 'STChargeSharingTool320')
MCITDepositCreator.STChargeSharingTool320.Thickness = 0.320*mm
MCITDepositCreator.STChargeSharingTool320.UseAnalyticErf = True
MCITDepositCreator.STChargeSharingTool320.ErfWidth = 0.0182

MCITDepositCreator.addTool(STChargeSharingTool, name = 'STChargeSharingTool410')
MCITDepositCreator.STChargeSharingTool410.Thickness = 0.410*mm
MCITDepositCreator.STChargeSharingTool410.UseAnalyticErf = True
MCITDepositCreator.STChargeSharingTool410.ErfWidth = 0.0222

from Configurables import SiAmplifierResponse
MCITDepositCreator.ResponseTypes = [
    "LRsignal",
    "LRcapCoupling", 
    "TBsignal",
    "TBcapCoupling"
    ]
MCITDepositCreator.addTool(SiAmplifierResponse, name = 'ResponseLRsignal')
MCITDepositCreator.addTool(SiAmplifierResponse, name = 'ResponseLRcapCoupling')
MCITDepositCreator.addTool(SiAmplifierResponse, name = 'ResponseTBsignal')
MCITDepositCreator.addTool(SiAmplifierResponse, name = 'ResponseTBcapCoupling')


# LHCb 1 signal
MCITDepositCreator.ResponseTBsignal.times = [ -30., -26., -18.2, -10., -5., 
                                              0., 5., 9.8, 19.8, 24.8, 29.8,
                                              59.8, 79.8,  85., 99.82 ]
MCITDepositCreator.ResponseTBsignal.values = [ 0.0, 0.03, 0.32, 0.74, 0.94, 
                                               1.0, 0.94, 0.8, 0.43, 0.26, 0.12,
                                               -0.11, -0.10, -0.099, -0.09]
MCITDepositCreator.ResponseTBsignal.capacitance = 18.0*picofarad

# LHCb 1 cap coupling
MCITDepositCreator.ResponseTBcapCoupling.times = [ -30.2426, -26.2426, -22.2426,
                                                   -18.2426, -14.2426, -10.2426,
                                                   -6.24262, -4.24262, -2.24262,
                                                   -0.242623, 1.75738, 3.75738,
                                                   5.75738, 9.75738, 14.7574,
                                                   19.7574, 24.7574, 29.7574,
                                                   34.7574, 39.7574, 59.7574,
                                                   79.7574]
MCITDepositCreator.ResponseTBcapCoupling.values = [0.00553926, 0.119517, 
                                                   0.30299, 0.582403, 0.864871,
                                                   1.07898, 1.15976 ,1.19488,
                                                   1.09999, 1.01967, 0.8575,
                                                   0.788192, 0.655131, 0.371741,
                                                   0.0303733, -0.176473,
                                                   -0.34389, -0.422551,
                                                   -0.448658, -0.472643,
                                                   -0.184689, -0.0524401 ]
MCITDepositCreator.ResponseTBcapCoupling.capacitance = 18.0*picofarad
MCITDepositCreator.ResponseTBcapCoupling.type = "capCoupling"

# LHCb 2 Vfs 0 - fast shaping 
MCITDepositCreator.ResponseLRsignal.times = [-30., -28., -21.5, -9.5, -5., 0.,
                                             5., 8.5, 10.5, 16.5, 25., 30.5,
                                             40.5, 60.5, 80.5, 100. ]
MCITDepositCreator.ResponseLRsignal.values = [0., 0.03 ,0.15, 0.76, 0.92, 1.,
                                              0.92, 0.83, 0.76, 0.51, 0.23, 0.1,
                                              0., -0.06, -0.08, -0.1]
MCITDepositCreator.ResponseLRsignal.vfs = 0
MCITDepositCreator.ResponseLRsignal.capacitance = 33.0*picofarad

MCITDepositCreator.ResponseLRcapCoupling.times = [-30.5125, -27.1 , -12.2, -8.1,
                                                  -6.1, -4.1, -0.2, 1.85, 7.85,
                                                  9.85,13.85, 17.85, 22.85, 
                                                  27.85, 37.85, 57.85, 77.85]
MCITDepositCreator.ResponseLRcapCoupling.values = [0.0,  0.12, 1.1, 1.18, 1.24, 
                                                   1.16, 1.01, 0.91, 0.50, 0.27,
                                                   0.03, -0.21, -0.40, -0.56,
                                                   -0.65, -0.25 , -0.195 ]
MCITDepositCreator.ResponseLRcapCoupling.vfs = 0
MCITDepositCreator.ResponseLRcapCoupling.capacitance = 33.0*picofarad
MCITDepositCreator.ResponseLRcapCoupling.type = "capCoupling"

# MCITDepositCreator.DetType = "IT";
# MCITDigitCreator.DetType = "IT";
# ITDigitCreator.DetType = "IT";
# ITClusterCreator.DetType = "IT";
# ITClusterKiller.DetType = "IT";

####from Configurables import STSignalToNoiseTool
####ToolSvc().addTool(STSignalToNoiseTool, name = 'STSignalToNoiseToolIT')
####ToolSvc().STSignalToNoiseToolIT.DetType = "IT";
####ToolSvc().STSignalToNoiseToolIT.ConversionToADC = 0.001;
# MCITDepositCreator.scaling = 1.33;

# ITDigitCreator.allStrips = True;
# TTDigitCreator.allStrips = True;

#from Configurables import STSelectClustersByChannel
#ToolSvc().addTool(STSelectClustersByChannel, name = 'ITKiller')

from Configurables import STSelectClustersByChannel, STRndmEffSelector
# Tools
ToolSvc().addTool(STSelectClustersByChannel, 'ITKiller')
ToolSvc().ITKiller.addTool(STRndmEffSelector, 'Selector')
ToolSvc().ITKiller.Selector.DetType = "IT"
