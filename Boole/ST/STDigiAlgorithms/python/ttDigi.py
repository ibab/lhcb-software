from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import *

from Configurables import MCSTDepositCreator

MCTTDepositCreator = MCSTDepositCreator('MCTTDepositCreator', DetType='TT')

# time of flight vector - always check that correct number stations defined
MCTTDepositCreator.TofVector = [7.8, 8.8]

# to add machine background
#MCTTDepositCreator.SpillVector = {"/","/Prev/","/PrevPrev/","/Next/", "/LHCBackground/"};
#MCTTDepositCreator.SpillTimes = {0.,-25.,-50., 25. , -3.3};

# charge sharing function  for TT
from Configurables import STChargeSharingTool
MCTTDepositCreator.ChargeSharerTypes = [ "500" ]
MCTTDepositCreator.addTool(STChargeSharingTool, name = 'STChargeSharingTool500')
MCTTDepositCreator.STChargeSharingTool500.Thickness = 0.500*mm
MCTTDepositCreator.STChargeSharingTool500.UseAnalyticErf = True
MCTTDepositCreator.STChargeSharingTool500.ErfWidth = 0.0224

from Configurables import SiAmplifierResponse
MCTTDepositCreator.ResponseTypes = [
    "12signal",
    "12capCoupling", 
    "43capCoupling",
    "43signal"
    ]
MCTTDepositCreator.addTool(SiAmplifierResponse, name = 'Response12signal')
MCTTDepositCreator.addTool(SiAmplifierResponse, name = 'Response12capCoupling')
MCTTDepositCreator.addTool(SiAmplifierResponse, name = 'Response43capCoupling')
MCTTDepositCreator.addTool(SiAmplifierResponse, name = 'Response43signal')
# 2 sensor + cable and 1 sensor + cable
MCTTDepositCreator.Response12signal.times = [-30.,-28.,-24.5,-14.5, -5., 0., 5.,
                                             15.5, 25., 35.5, 55.5, 75., 100.]
MCTTDepositCreator.Response12signal.values = [ 0., 0.03, 0.14, 0.58, 0.93, 1.0,
                                               0.94 ,0.67, 0.36, 0.14, -0.04,
                                               -0.1, -0.1]
MCTTDepositCreator.Response12signal.capacitance = 33.0*picofarad


MCTTDepositCreator.Response12capCoupling.times = [-30.5125, -27.1, -12.2, -8.1,
                                                  -6.1, -4.1, -0.2, 1.85, 7.85,
                                                  9.85, 13.85, 17.85, 22.85,
                                                  27.85, 37.85, 57.85, 77.85]
MCTTDepositCreator.Response12capCoupling.values = [0.0,  0.12, 1.1, 1.18, 1.24, 
                                                   1.16, 1.01, 0.91, 0.50, 0.27,
                                                   0.03, -0.21, -0.40, -0.56,
                                                   -0.65, -0.25 , -0.195 ]
MCTTDepositCreator.Response12capCoupling.capacitance = 33.0*picofarad
MCTTDepositCreator.Response12capCoupling.type = "capCoupling"

# 4 and 3 sensor sector
MCTTDepositCreator.Response43signal.times = [ -35., -30., -26., -20.5, -15.5,
                                              -10.5, -5., 0., 5., 9.5, 25., 
                                              39.4, 54.5, 74.5, 95.5, 119.5 ]
MCTTDepositCreator.Response43signal.values = [ 0.,  0., 0.035, 0.32, 0.51, 0.78,
                                               0.94, 1., 0.94, 0.87, 0.43, 0.15,
                                               0.04, -0.04, -0.09 ,-0.12]
MCTTDepositCreator.Response43signal.capacitance = 57.0*picofarad

MCTTDepositCreator.Response43capCoupling.times = [ -35.5, -20.5, -15.5, -10.55,
                                                   -5.5, -3.5, -0.5, 4.5 ,9.5,
                                                   19.5, 24.5, 29.5, 34.5, 39.5,
                                                   54.5, 74.5 ]
MCTTDepositCreator.Response43capCoupling.values = [ 0.0, 0.47,  0.8, 1.00, 1.07,
                                                    1.04, 1.00, 0.76, 0.54,
                                                    -0.05,-0.26,-0.42,-0.52,
                                                    -0.58,-0.51,-0.38 ]
MCTTDepositCreator.Response43capCoupling.capacitance = 57.0 * picofarad
MCTTDepositCreator.Response43capCoupling.type = "capCoupling"

#from Configurables import STSelectClustersByChannel, STRndmEffSelector
