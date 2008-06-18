import traceback, Online.ProcessorFarm

def run():
  """
  Execute Storage system streaming component to
   - allocate execution slots and
   - write job options.

  @author M.Frank
  """
  try:
    Online.ProcessorFarm.execute('-type Subfarm -wait'.split(' '))
  except Exception,X:
    print 'Failed to start subfarm device.'
    traceback.print_exc()

if __name__ == "__main__":
  run()
