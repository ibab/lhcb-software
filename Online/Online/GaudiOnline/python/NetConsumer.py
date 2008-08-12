import os,OnlineEnv

algs = [OnlineEnv.storeExplorer(load=1,freq=0.0005)]
src  = os.environ['NODENAME']+'/EvtServ'
app  = OnlineEnv.netConsumerApp(OnlineEnv.PartitionID,OnlineEnv.PartitionName,src,algs)
OnlineEnv.end_config()
