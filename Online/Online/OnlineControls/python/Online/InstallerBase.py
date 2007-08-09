import Online.PVSS    as PVSS
import Online.Utils   as Utils
std = PVSS.gbl.std
PVSS.logPrefix = 'PVSS Control '
log            = Utils.log
error          = Utils.error
warning        = Utils.warning

# =============================================================================
class InstallerBase:
  # ===========================================================================
  def __init__(self, mgr, dev):
    "Default constructor"
    self.name = dev
    self.manager = mgr
    self.dps = PVSS.DataPointVector()
  # ===========================================================================
  def set(self, dp, value):
    "Set datapoint value in storage definition"
    self.dps.push_back(PVSS.DataPoint(self.manager,PVSS.DataPoint.original(self.name+'.'+dp)))
    self.dps.back().data = value
    return self
  # ===========================================================================
  def write(self,prefix=''):
    "Write information to datapoints"
    wr = self.manager.devWriter()
    for i in self.dps: wr.add(i)
    if wr.execute(1,1):
      log(prefix+'All DPs successfully created.')
    else:
      error(prefix+'Failed to set DP values.')
    self.dps.clear()
    return self
