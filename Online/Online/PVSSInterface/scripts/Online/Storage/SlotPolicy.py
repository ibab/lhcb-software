# =============================================================================
class SlotPolicy:
  """ Base class to implement slot allocation policy

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, debug):
    "Default constructor. "
    self.debug = debug
    
  # ===========================================================================
  def leastOccupied(self,slots):
    "Determine slot, which least occupied ie. with the maximum number of free slots."
    mult = 0
    item = None
    for n,sl in slots.items():
      size = len(sl)
      if size > mult:
        item = n
        mult = size
    return item

  # ===========================================================================
  def collectFreeSlots(self,slices):
    "Collect all slots which are actually free."
    slots = {}
    for i in slices:
      if len(i) > 0:
        key = i[:i.find(':')]
        if not slots.has_key(key): slots[key] = [];
        slots[key].append(i)
    return slots

