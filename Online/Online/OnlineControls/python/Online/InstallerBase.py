import Online.PVSS, Online.Utils

# =============================================================================
class InstallerBase:
  """
  Generic base class for installers.

  @author M.Frank
  """
  # ===========================================================================
  def __init__(self, mgr, dev):
    "Default constructor"
    self.name    = dev
    self.manager = mgr
    self.dps     = Online.PVSS.DataPointVector()
  # ===========================================================================
  def set(self, dp, value):
    "Set datapoint value in storage definition"
    self.dps.push_back(Online.PVSS.DataPoint(self.manager,Online.PVSS.DataPoint.original(self.name+'.'+dp)))
    self.dps.back().data = value
    return self
  # ===========================================================================
  def write(self,prefix=''):
    "Write information to datapoints"
    wr = self.manager.devWriter()
    for i in self.dps: wr.add(i)
    if wr.execute(1,1):
      Online.Utils.log(prefix+'All DPs successfully created.',timestamp=1)
    else:
      Online.Utils.error(prefix+'Failed to set DP values.',timestamp=1)
    self.dps.clear()
    return self
