import traceback
import Online.Streaming

def run():
  """
   Execute HLT job options writer

   @author M.Frank
  """
  try:
    Online.Streaming.execute('-name HLT -type HLTOptions -wait'.split(' '))
  except Exception,X:
    print 'Failed to start HLT job options writer.'
    traceback.print_exc()

if __name__ == "__main__":
  run()
