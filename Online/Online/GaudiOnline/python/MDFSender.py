import os,OnlineEnv
rcv = os.environ['NODENAME']+'/RECEIVER'
app = OnlineEnv.dataSenderApp(OnlineEnv.PartitionID,OnlineEnv.PartitionName,rcv,'RESULT',False)
OnlineEnv.end_config()
