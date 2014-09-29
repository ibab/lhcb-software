import traceback, Online.Streaming

def run():
  """
  Execute Storage system streaming component to
   - allocate execution slots and
   - write job options.

  @author M.Frank
  """
  try:
    Online.Streaming.execute('-type Storage -wait'.split(' '))
  except Exception,X:
    print 'Failed to start Storage streaming device.'
    traceback.print_exc()

if __name__ == "__main__":
  run()
