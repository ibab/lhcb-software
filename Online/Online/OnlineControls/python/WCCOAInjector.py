import traceback
import Online.Streaming

def run():
  """
   Execute HLT job options writer

   @author M.Frank
  """
  try:
    Online.Streaming.execute('-name INJ -type InjectorOptions -wait'.split(' '))
  except Exception,X:
    print 'Failed to start Injector job options writer.'
    traceback.print_exc()

if __name__ == "__main__":
  run()
