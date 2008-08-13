import platform, socket

node_name = socket.gethostname().upper()
if node_name.find('.') != -1:
  node_name = node_name[:node_name.find('.')]

# LHCb wide general ECS system name
lbecs_system_name      = 'LBECS'
jobopts_system_name    = 'LBECS'
storage_system_name    = 'STORAGE'
recstorage_system_name = 'RECSTORAGE'
monitor_system_name    = 'MONITORING'

def jobOptionsSystemName():  return jobopts_system_name
#def jobOptionsSystemName():  return "TEST";

# Subdetector definitions
detectors = {}
detectors['LHCb'] = {'PVSSSystem':    'ECSLHCb',
                     'PartitionID':   1,
                     'SubFarms':      35,
                     'StorageStreams':[('Higgs',1),('B2JPsi',1),('B2Dstar',1),
                                       ('B2Charm',2),('BBinc',5)],
                     'MonStreams':    [('MonMUON','Stream3',1), ('MonHCAL','Stream3',1),
                                       ('MonRICH','Stream3',3), ('MonECAL','Stream2',4),
                                       ('MonVELO','Stream2',4),('MonBBinc','Stream1',7)],
                     'Last':          0
                     }
detectors['MUON'] = {'PVSSSystem':    'ECSMUON',
                     'PartitionID':   10,
                     'SubFarms':5,
                     'StorageStreams':[('Events',3)],
                     'MonStreams':    [('MonRawData','Stream0',2)],
                     'Last':          0
                     }
detectors['VELO'] = {'PVSSSystem':    'ECSVELO',
                     'PartitionID':   100,
                     'SubFarms':      8,
                     'StorageStreams':[('Events',2)],
                     'MonStreams':    [('MonRawData','Stream0',3)],
                     'Last':          0
                     }
detectors['RICH'] = {'PVSSSystem':    'ECSRICH',
                     'PartitionID':   1000,
                     'SubFarms':      2,
                     'StorageStreams':[('Events',2)],
                     'MonStreams':    [('MonRawData','Stream0',3)],
                     'Last':          0
                     }
detectors['PRS']  = {'PVSSSystem':    'ECSPRS',
                     'PartitionID':   10000,
                     'SubFarms':      1,
                     'StorageStreams':[('Events',1)],
                     'MonStreams':    [('MonRawData','Stream0',2)],
                     'Last':          0
                     }
detectors['ECAL'] = {'PVSSSystem':    'ECSECAL',
                     'PartitionID':   100000,
                     'SubFarms':      1,
                     'StorageStreams':[('Events',1)],
                     'MonStreams':    [('MonRawData','Stream0',2)],
                     'Last':          0
                     }

# Generic setup
daq_numPartition    = 16

# HLT farm definitions
hlt_numSubFarms     = 50

# Joboptions control parameters
if platform.system() == 'Linux': jobopts_optsdir = "/group/online/dataflow/options"
else:                            jobopts_optsdir = "J:/options"
gauditask_startscript='/home/frankm/runGaudi.sh'


# Storage parameters
# Stream definitions are in Online.RunInfoClasses.Installer
storage_streams_per_node = 15
stroage_control_node= "STORECTRL01"
storage_recv_nodes  = ['STORERECV01','STORERECV02']
storage_strm_nodes  = ['STORESTRM01','STORESTRM02']

# Monitoring parameters
#
# monitor_type          defines the monitoring base name
# monitor_system_name   defines the PVSS system name
# monitor_control_node  defines the name of the controls PC
# monitor_relay_nodes   defines the relay nodes for the monitoring farm
# monitor_nodes         defines the worker nodes for the monitoring farm
# monitor_num_slices    defines the number of tasks in the worker nodes
# monitor_streams_per_node
#
# Stream definitions are in Online.RunInfoClasses.Installer
if node_name == "MONA06":
  # Generic setup: Maximum of 3 reconstruction partitions
  daq_numPartition         = 16
  # Monitoring system parameters
  monitor_system_name      = 'MONITORING2'
  monitor_type             = 'Monitoring2'
  monitor_control_node     =  node_name
  monitor_num_nodes        = 10
  monitor_streams_per_node = 8
  monitor_num_slices       = monitor_num_nodes*15
  monitor_relay_nodes      = [node_name+"01"]
  monitor_nodes            = []
  for i in xrange(9): monitor_nodes.append("%s%02d"%(node_name,i+1,))

if node_name == "MONA08":
  monitor_system_name      = 'MONITORING'
  monitor_type             = 'Monitoring'
  monitor_control_node     =  "MONA08"
  monitor_num_nodes        = 5
  monitor_num_slices       = 64
  monitor_streams_per_node = 8
  monitor_relay_nodes      = ["MONA0801"]
  monitor_nodes            = ["MONA0802","MONA0803","MONA0804","MONA0805"]

if node_name == "MONA09":
  # Generic setup: Maximum of 3 reconstruction partitions
  daq_numPartition         = 3
  # Monitoring system parameters
  monitor_system_name      = 'RECONSTRUCTION'
  monitor_type             = 'Reconstruction'
  monitor_control_node     =  node_name
  monitor_num_nodes        = 20
  monitor_streams_per_node = 8
  monitor_num_slices       = monitor_num_nodes*8
  monitor_relay_nodes      = [node_name+"01"]
  monitor_nodes            = []
  for i in xrange(19):
    monitor_nodes.append("%s%02d"%(node_name,i+1,))

