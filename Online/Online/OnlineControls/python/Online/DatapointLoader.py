import Online.Utils, Online.PVSS

# =============================================================================
class DatapointLoader:
  """ @class DatapointLoader
  

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self,manager,name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the datapoint

        @return reference to initialized object
    """
    self.name = name
    self.dpName = name
    self.manager = manager
    self.reader = self.manager.devReader()

  # ===========================================================================
  def dp(self,name):
    "Access internal datapoint of the datapoint structure."
    return self.dp2(self.dpName,name)

  # ===========================================================================
  def dp2(self,parent,name):
    "Access internal datapoint of the datapoint structure."
    return Online.PVSS.DataPoint(self.manager,Online.PVSS.DataPoint.original(parent+'.'+name))

  # ===========================================================================
  def load(self):
    "Load the datapoints assigned to the reader."
    if self.reader.execute(1,1):
      return self
    Online.Utils.error('Failed to load run information for:'+self.name)
    return None
