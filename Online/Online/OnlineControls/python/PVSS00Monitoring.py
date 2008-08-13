import traceback
import Online.Streaming
import Online.SetupParams as Params

def run():
  """
  Execute Monitoring system streaming component to
   - allocate execution slots and
   - write job options.

  @author M.Frank
  """
  try:
    cmd = '-name '+Params.monitor_type+' -type Monitoring -wait'
    Online.Streaming.execute(cmd.split(' '))
  except Exception,X:
    print 'Failed to start Monitoring streaming device for system:',Params.monitor_system_name,Params.monitor_type
    traceback.print_exc()

if __name__ == "__main__":
  run()
