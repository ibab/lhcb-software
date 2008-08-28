import os, sys, socket
HLT = 1
STORAGE = 2
MONITORING = 3
TOP_LOGGER = 4

db_dir = '..'+os.sep+'xml'+os.sep

def getNodes(host):
  lines = os.popen('ssh -q '+host+' "tmLs | grep \"Node:\" | sort" | grep Node')
  return [l[:-1].replace('"','').replace('.','').replace('  ',' ').split(' ')[3] for l in lines]

def nodeType(typ):
  if typ == HLT:
    return ('HLTControl','HLTWorker')
  elif typ == STORAGE:
    return ('StorageControl','StorageWorker')
  elif typ == MONITORING:
    return ('MonitoringControl','MonitoringWorker')
  return ('','')

def create(typ,controlsPC):
  ctr = controlsPC.upper()
  nodes = getNodes(ctr)
  ctr_type,wrk_type = nodeType(typ)
  txt = '<TaskInventory>\n  <NodeList>\n   <Name>%s</Name>\n'%(ctr,)
  for i in nodes:
    if i.upper()==ctr:
      txt = txt + '   <Node name="%s"   type="%s"/>\n'%(i,ctr_type)
    else:
      txt = txt + '   <Node name="%s"   type="%s"/>\n'%(i,wrk_type)
  txt = txt + '  </NodeList>\n</TaskInventory>\n'
  file = open(db_dir+ctr+'.xml','w')
  print >>file, txt
  file.close()
  print '.........> Wrote XML configuration file for sub-cluster:',ctr

if __name__ == "__main__":
  rows = "a'b'c'd'e"
  columns = "1'2'3'4'6'7'8'9'10'11"
  for row in rows.split("'"):
    for column in columns.split("'"):
      create(HLT,'hlt%s%02d'%(row,int(column)))
  create(STORAGE,'storectl01')
  create(MONITORING,'mona08')
  print '.........> All Done.'
