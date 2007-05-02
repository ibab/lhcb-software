import os, sys, time, platform
import Online.PVSS as PVSS
import Online.JobOptions.JobOptionsControl as WR
import Online.Storage.StorageControl as StorageControl
import Online.Storage.Simulator as StorageSimulator

def run():
  import Online.PVSS as PVSS
  import Online.JobOptions.JobOptionsControl as WR
  import Online.Storage.StorageControl as StorageControl
  import Online.Storage.Simulator as StorageSimulator
  name = 'Storage'
  mgr = PVSS.controlsMgr()
  ctrl = StorageControl.Control(mgr,name)
  sensor=PVSS.DeviceSensor(mgr,ctrl.point)
  sensor.addListener(ctrl)
  sensor.run(1)
  wr = WR.StorageOptionsWriter(mgr,'JobOptions')
  if platform.system() == 'Linux':  wr.optionsDir = "/home/frankm/options"
  else:                             wr.optionsDir = "J:/options"
  
  wr.run()
  sims = []
  sim = [] # ['Slice00']
  for slice in sim:
    s = StorageSimulator.Simulator(mgr,name+'_'+slice)
    s.run()
    sims.append(s)

  print 'Sleeping ....'
  sys.stdout.flush()
  try:
    while(1):
      time.sleep(1)
  except Exception,X:
    print 'Exception:',str(X)
  except:
    print 'Unknown Exception'
  sys.stdout.flush()
  for i in sims:
    del(i)

if __name__ == "__main__":
  run()
  
