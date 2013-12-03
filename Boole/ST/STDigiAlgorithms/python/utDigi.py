from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import *

from Configurables import MCSTDepositCreator, STChargeSharingTool
from Configurables import SiAmplifierResponse, STSelectClustersByChannel, STRndmEffSelector

MCUTDepositCreator = MCSTDepositCreator('MCUTDepositCreator', DetType='UT')

# time of flight vector
MCUTDepositCreator.TofVector = [7.8, 8.8]

MCUTDepositCreator.SpillVector = ["/"]
MCUTDepositCreator.SpillTimes = [0.]

# charge sharing function  for UT
MCUTDepositCreator.ChargeSharerTypes = [ "250" ]
MCUTDepositCreator.addTool(STChargeSharingTool, name = 'STChargeSharingTool250')
MCUTDepositCreator.STChargeSharingTool250.Thickness = 0.250*mm
MCUTDepositCreator.STChargeSharingTool250.UseAnalyticErf = True
MCUTDepositCreator.STChargeSharingTool250.ErfWidth = 0.005

# beetle response
MCUTDepositCreator.ResponseTypes = [ "20signal", "20capCoupling" ]
MCUTDepositCreator.addTool(SiAmplifierResponse, "Response20signal" )
MCUTDepositCreator.addTool(SiAmplifierResponse, "Response20capCoupling" )

MCUTDepositCreator.Response20signal.times = [-30.0, -25.0, -20.0, -18.0, -16.0, -14.0, -12.0, -10.0, -8.0, -6.0,
                                             -4.0, -2.0,  0.0,  2.0,  4.0,  6.0,  8.0, 10.0, 12.0, 14.0, 16.0, 
                                             18.0, 20.0, 22.0, 24.0, 26.0, 28.0, 30.0, 32.0, 34.0, 36.0, 38.0,
                                             40.0, 42.0, 44.0, 46.0, 48.0, 50.0, 52.0, 54.0, 56.0, 60.0, 100.0]

MCUTDepositCreator.Response20signal.values = [0.000, 0.000, 0.027, 0.081, 0.181, 0.308, 0.444, 0.579, 0.705, 0.816, 0.906,
                                              0.969, 1.000, 0.995, 0.954, 0.882, 0.790, 0.686, 0.579, 0.475, 0.377, 0.287,
                                              0.207, 0.136, 0.077, 0.028, -0.004, -0.017, -0.025, -0.029, -0.030, -0.028,
                                              -0.024, -0.021, -0.018, -0.017, -0.016, -0.016, -0.015, -0.014, -0.013,
                                              -0.010, -0.005]
MCUTDepositCreator.Response20signal.capacitance = 20.0*picofarad

MCUTDepositCreator.Response20capCoupling.times = [-30.0, -25.0, -20.0, -18.0, -16.0, -14.0, -12.0, -10.0, -8.0, -6.0,
                                                  -4.0, -2.0,  0.0,  2.0,  4.0,  6.0,  8.0, 10.0, 12.0, 14.0, 16.0, 
                                                  18.0, 20.0, 22.0, 24.0, 26.0, 28.0, 30.0, 32.0, 34.0, 36.0, 38.0, 
                                                  40.0, 42.0, 44.0, 46.0, 48.0, 50.0, 52.0, 54.0, 56.0, 60.0, 100.0]

# use the same for now
MCUTDepositCreator.Response20capCoupling.values =  [0.000, 0.000, 0.027, 0.081, 0.181, 0.308, 0.444, 0.579, 0.705, 0.816, 0.906,
                                                    0.969, 1.000, 0.995, 0.954, 0.882, 0.790, 0.686, 0.579, 0.475, 0.377, 0.287,
                                                    0.207, 0.136, 0.077, 0.028, -0.004, -0.017, -0.025, -0.029, -0.030, -0.028,
                                                    -0.024, -0.021, -0.018, -0.017, -0.016, -0.016, -0.015, -0.014, -0.013,
                                                    -0.010, -0.005]

MCUTDepositCreator.Response20capCoupling.capacitance = 20.0*picofarad
MCUTDepositCreator.Response20capCoupling.type = "capCoupling"

# 5% cross talk
MCUTDepositCreator.XTalkParamsLeftOdd   = [0.05, 0]
MCUTDepositCreator.XTalkParamsLeftEven  = [0.05, 0]
MCUTDepositCreator.XTalkParamsRightOdd  = [0.05, 0]
MCUTDepositCreator.XTalkParamsRightEven = [0.05, 0]

# Tools
ToolSvc().addTool(STSelectClustersByChannel, 'UTKiller')
ToolSvc().UTKiller.addTool(STRndmEffSelector, 'Selector')
ToolSvc().UTKiller.Selector.DetType = "UT"

#ToolSvc.STSignalToNoiseToolUT.DetType = "UT"
