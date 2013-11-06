# MODULES
from GaudiKernel.SystemOfUnits import *
from Configurables import MassBiasesMonitor

def exampleJpsiMonitor(name,sequence):

    mon = MassBiasesMonitor(name,
                            InputLocation = sequence.outputLocation(),
                            MinMass = 3.02*GeV, MaxMass = 3.17*GeV,
                            BinsMom = 10, MinMom = 20*GeV, MaxMom = 250*GeV,
                            MomDistr = "RFEXPO", MomParams = [20.55*GeV, 0.0877/GeV, 0.0181/GeV],
                            BinsPt = 10, MinPt = 3*GeV, MaxPt = 15*GeV,
                            PtDistr = "EXPO", PtParams = [0.5/GeV],
                            BinsMomDif = 10, MaxMomDif = 150*GeV, MomDifDistr = "EXPO", MomDifParams = [0.0348/GeV],
                            BinsAsym = 10, AsymDistr = "ASYM", AsymParams = [-0.648,0.627,-1.136],
                            BinsEta = 10, MinEta = 2, MaxEta = 5.5, EtaDistr = "GAMMA", EtaParams = [32.36,0.1052,0],
                            BinsTx = 10, MaxTx = 0.2, TxDistr = "RFEXPO", TxParams = [-0.0123,0.31,33.26],
                            BinsTy = 10, MaxTy = 0.2, TyDistr = "RFEXPO", TyParams = [-0.0123,0.31,33.26],
                            BinsOpenAngle = 10, MinOpenAngle = 0, MaxOpenAngle = 0.5,
                            OpenAngleDistr = "GAMMA", OpenAngleParams = [3.881, 0.02825, 0]
                            )
    return mon
    
def examplePsi2SMonitor(name,sequence):

    mon = MassBiasesMonitor(name,
                            InputLocation = sequence.outputLocation(),
                            MinMass = 3.586*GeV, MaxMass = 3.786*GeV,
                            BinsMom = 10, MinMom = 20*GeV, MaxMom = 250*GeV,
                            MomDistr = "GAUSEXPO", MomParams = [30.05*GeV, 5.128*GeV, 0.021/GeV],
                            BinsPt = 10, MinPt = 3.75*GeV, MaxPt = 15*GeV,
                            PtDistr = "EXPO", PtParams = [0.64/GeV],
                            BinsMomDif = 10, MaxMomDif = 150*GeV, MomDifDistr = "EXPO", MomDifParams = [0.0336/GeV],
                            BinsAsym = 10, AsymDistr = "ASYM", AsymParams = [2.174,-5.277,1.818],
                            BinsEta = 10, MinEta = 2, MaxEta = 5.5, EtaDistr = "GAMMA", EtaParams = [36.2,0.09104,0],
                            BinsTx = 10, MaxTx = 0.2, TxDistr = "RFEXPO", TxParams = [-0.0117, 0.1516, 30.8],
                            BinsTy = 10, MaxTy = 0.2, TyDistr = "RFEXPO", TyParams = [-0.0117, 0.1516, 30.8],
                            BinsOpenAngle = 10, MinOpenAngle = 0, MaxOpenAngle = 0.5,
                            OpenAngleDistr = "GAMMA", OpenAngleParams = [4.482, 0.03068, 0]
                            )

    return mon

def exampleUps1SMonitor(name,sequence):

    mon = MassBiasesMonitor(name,
                            InputLocation = sequence.outputLocation(),
                            MinMass = 9.06030*GeV, MaxMass = 9.86030*GeV,
                            BinsMom = 10, MinMom = 30*GeV, MaxMom = 400*GeV,
                            MomDistr = "RFEXPO", MomParams = [32.21*GeV, 0.0005138/GeV, 0.01904/GeV],
                            BinsPt = 10, MinPt = 0*GeV, MaxPt = 20*GeV,
                            PtDistr = "GAMMA", PtParams = [2.161,2.151*GeV,0],
                            BinsMomDif = 10, MaxMomDif = 200*GeV, MomDifDistr = "EXPO", MomDifParams = [0.01097/GeV],
                            BinsAsym = 10, AsymDistr = "ASYM", AsymParams = [-2.0,15.8,-12.76],
                            BinsEta = 10, MinEta = 2, MaxEta = 7, EtaDistr = "GAMMA", EtaParams = [23.75,0.1748,0],
                            BinsTx = 10, MaxTx = 0.2, TxDistr = "EXPO", TxParams = [35.5],
                            BinsTy = 10, MaxTy = 0.2, TyDistr = "EXPO", TyParams = [35.5],
                            BinsOpenAngle = 10, MinOpenAngle = 0, MaxOpenAngle = 0.5,
                            OpenAngleDistr = "GAMMA", OpenAngleParams = [4.615, 0.04838, 0]
                            )
    return mon


def exampleUpsAllSMonitor(name,sequence):

    mon = MassBiasesMonitor(name,
                            InputLocation = sequence.outputLocation(),
                            MinMass = 8.5*GeV, MaxMass = 11.5*GeV,
                            BinsMom = 10, MinMom = 30*GeV, MaxMom = 400*GeV,
                            MomDistr = "RFEXPO", MomParams = [32.21*GeV, 0.0005138/GeV, 0.01904/GeV],
                            BinsPt = 10, MinPt = 0*GeV, MaxPt = 20*GeV,
                            PtDistr = "GAMMA", PtParams = [2.161,2.151*GeV,0],
                            BinsMomDif = 10, MaxMomDif = 200*GeV, MomDifDistr = "EXPO", MomDifParams = [0.01097/GeV],
                            BinsAsym = 10, AsymDistr = "ASYM", AsymParams = [-2.0,15.8,-12.76],
                            BinsEta = 10, MinEta = 2, MaxEta = 7, EtaDistr = "GAMMA", EtaParams = [23.75,0.1748,0],
                            BinsTx = 10, MaxTx = 0.2, TxDistr = "EXPO", TxParams = [35.5],
                            BinsTy = 10, MaxTy = 0.2, TyDistr = "EXPO", TyParams = [35.5],
                            BinsOpenAngle = 10, MinOpenAngle = 0, MaxOpenAngle = 0.5,
                            OpenAngleDistr = "GAMMA", OpenAngleParams = [4.615, 0.04838, 0]
                            )

    return mon

def exampleZ0Monitor(name,selection):

    mon = MassBiasesMonitor(name,
                            InputLocation = sequence.outputLocation(),
                            MinMass = 60*GeV, MaxMass = 120*GeV,
                            BinsMom = 10, MinMom = 0*GeV, MaxMom = 2500*GeV,
                            MomDistr = "GAUSEXPO", MomParams = [484*GeV, 145*GeV, 0.00205/GeV],
                            BinsPt = 10, MinPt = 0*GeV, MaxPt = 50*GeV,
                            PtDistr = "GAMMA", PtParams = [1.93053,4.27169*GeV,0],
                            BinsMomDif = 10, MaxMomDif = 1500*GeV, MomDifDistr = "EXPO", MomDifParams = [0.00253/GeV],
                            BinsAsym = 10, AsymDistr = "ASYM", AsymParams = [1.425,-3.43,2.257],
                            BinsEta = 10, MinEta = 2, MaxEta = 9, EtaDistr = "GAMMA", EtaParams = [17.7, 0.2917, 0],
                            BinsTx = 10, MaxTx = 0.2, TxDistr = "EXPO", TxParams = [70],
                            BinsTy = 10, MaxTy = 0.2, TyDistr = "EXPO", TyParams = [70],
                            BinsOpenAngle = 10, MinOpenAngle = 0, MaxOpenAngle = 0.75,
                            OpenAngleDistr = "GAMMA", OpenAngleParams = [4.83, 0.053665, 0]
                            )

    return mon
                            
