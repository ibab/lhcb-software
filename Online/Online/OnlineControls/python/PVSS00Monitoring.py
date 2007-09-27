import traceback
import Online.Streaming

def run():
  """
  Execute Monitoring system streaming component to
   - allocate execution slots and
   - write job options.

  @author M.Frank
  """
  try:
    Online.Streaming.execute('-type Monitoring -wait'.split(' '))
  except Exception,X:
    print 'Failed to start Monitoring streaming device.'
    traceback.print_exc()

if __name__ == "__main__":
  run()
