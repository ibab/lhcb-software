import traceback, Online.ProcessorFarm

def run():
  """
  Execute Processor farm's storage system component to
   - allocate execution slots and
   - write job options.

  @author M.Frank
  """
  try:
    Online.ProcessorFarm.execute('-type Storage -wait'.split(' '))
  except Exception,X:
    print 'Failed to start Storage streaming device.'
    traceback.print_exc()

if __name__ == "__main__":
  run()
