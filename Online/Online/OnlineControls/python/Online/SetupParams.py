import platform

# LHCb wide general ECS system name
lbecs_system_name   = 'LBECS'
jobopts_system_name = 'LBECS'
storage_system_name = 'STORAGE'
monitor_system_name = 'MONITORING'

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
if platform.system() == 'Linux': jobopts_optsdir = "/home/frankm/options"
else:                            jobopts_optsdir = "J:/options"

# Storage parameters
# Stream definitions are in Online.RunInfoClasses.Installer
storage_streams_per_node = 15
stroage_control_node= "STORECTRL01"
storage_recv_nodes  = ['STORERECV01','STORERECV02']
storage_strm_nodes  = ['STORESTRM01','STORESTRM02']

# Monitoring parameters
# Stream definitions are in Online.RunInfoClasses.Installer
monitor_num_slices  = 64
monitor_control_node=  "MONA08"
monitor_relay_nodes = ["MONA0801"]
monitor_nodes       = ["MONA0802","MONA0803","MONA0804","MONA0805"]
monitor_streams_per_node = 8
