import os, sys, time, platform

def run():
  import Online.PVSS as PVSS
  import Online.RunInfo as RunInfo
  import Online.JobOptions.JobOptionsControl as WR
  import Online.Streaming.StreamingControl as StreamingControl
  import Online.Streaming.Simulator as StreamingSimulator
  import Online.RunInfoClasses.Monitoring as MonInfo
  name = 'Monitoring'
  mgr = PVSS.controlsMgr()
  info = MonInfo.MonitoringInfoCreator(mgr,'Storage')
  ctrl = StreamingControl.Control(mgr,name,info)
  sensor=PVSS.DeviceSensor(mgr,ctrl.control)
  sensor.addListener(ctrl)
  sensor.run(1)
  wr = WR.MonitoringOptionsWriter(mgr,'MonitoringJobOptions',info)
  if platform.system() == 'Linux':  wr.optionsDir = "/home/frankm/monoptions"
  else:                             wr.optionsDir = "J:/options"
  
  wr.run()
  
  sims = []
  sim  = []
  sim  = ['Slice00']
  for slice in sim:
    s = StreamingSimulator.Simulator(mgr,name+'_'+slice)
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
  
