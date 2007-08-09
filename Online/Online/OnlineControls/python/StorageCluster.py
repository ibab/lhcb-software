import os, sys, time
import Online.SetupParams as Params
import Online.PVSSSystems as Systems

def run():
  import Online.PVSS as PVSS
  import Online.RunInfo as RunInfo
  import Online.JobOptions.JobOptionsControl as WR
  import Online.Streaming.StreamingControl as StreamingControl
  import Online.Streaming.Simulator as StreamingSimulator
  name = 'Storage'
  stream_mgr = Systems.controlsMgr(Params.storage_system_name)
  ctrl = StreamingControl.Control(stream_mgr,name,RunInfo.RunInfoCreator())
  sensor=PVSS.DeviceSensor(stream_mgr,ctrl.control)
  sensor.addListener(ctrl)
  sensor.run(1)

  jobopts_mgr = Systems.controlsMgr(Params.jobopts_system_name)
  wr = WR.StorageOptionsWriter(mgr,'StorageJobOptions',RunInfo.RunInfoCreator())
  wr.optionsDir = Params.jobopts_optsdir
  
  wr.run()
  
  sims = []
  sim = []
  sim = ['Slice00']
  for slice in sim:
    s = StreamingSimulator.Simulator(stream_mgr,name+'_'+slice)
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
  
