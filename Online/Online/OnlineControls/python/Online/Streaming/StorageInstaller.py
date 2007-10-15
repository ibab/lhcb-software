"""
Install the Online Storage Control components

Commands:

import Online.Streaming.StorageInstaller as SI
mgr=SI.install('Storage')

Author: M.Frank

"""
import Online.SetupParams, Online.PVSSSystems, Online.Streaming.StreamInstaller

# =============================================================================
def install(name,system=Online.SetupParams.storage_system_name):
  "Install required components for the storage"
  
  inst = Online.Streaming.StreamInstaller.Installer(Online.PVSSSystems.controlsMgr(system),name)
  inst.numPartition     = Online.SetupParams.daq_numPartition
  inst.recvNodes        = Online.SetupParams.storage_recv_nodes
  inst.strmNodes        = Online.SetupParams.storage_strm_nodes
  inst.numRecvSlices    = Online.SetupParams.hlt_numSubFarms
  inst.numStreamSlices  = len(inst.strmNodes)*Online.SetupParams.storage_streams_per_node
  inst.create()
  return inst.manager

# =============================================================================
if __name__ == "__main__":
  install('Storage')
