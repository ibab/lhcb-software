runInfoCmds = """
import StorageInstaller
StorageInstaller.install()

import Online.PVSS as PVSS
from   Online.RunInfo import RunInfo as RunInfo
import FSMTree, StorageInstaller, StorageControl
from StorageSimulator import Simulator as Simulator

mgr,ctrl,sensor = StorageControl.run()
sim0 = Simulator(mgr,'TestStorage_Slice00')
sim0.run()
sim1 = Simulator(mgr,'TestStorage_Slice01')
sim1.run()
sim2 = Simulator(mgr,'TestStorage_Slice02')
sim2.run()

PVSS.setDebug(2)
PVSS.setDebug(0)

mgr = PVSS.controlsMgr()
ctrl = Control(mgr,'TestStorage')
sensor=PVSS.DeviceSensor(mgr,ctrl.point)
sensor.addListener(ctrl)
sensor.run(1)

import FSMTree
FSMTree.Dump(mgr,'TestStorage_Slice00').showAll()

ctrl.show()
info=RunInfo(mgr,'LHCb')
info.show()


ctrl.allocate('MUON',5,2)
ctrl.free('MUON')

reload(StorageControl)
mgr,ctrl,sensor = StorageControl.run()
ctrl.free('MUON')
ctrl.allocate('MUON',5,2)

"""
runTestCmds = """
import StorageControl
mgr,ctrl=StorageControl.runTest()
"""
