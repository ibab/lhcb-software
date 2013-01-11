import os
lines = os.popen('grep " StoreTracer " Brunel.traces','r').readlines()

start = 0
for l in lines:
  try:
    prop = ''
    items = l.split()
    
    seq = int(items[0])
    alg = items[3][1:]

    if alg == 'BrunelInit':
      start=1
    if start == 1 and items[3] == '(Framework)':
      #print 'Done.'
      break
    elif items[3]=='(Framework)':
      tag = items[7]
      cmd = items[5]
    elif len(items)>9:
      prop = items[9]
      tag = items[8]
      cmd = items[6]
    elif len(items)>8:
      tag = items[8]
      cmd = items[6]
    else:
      print seq,items
      continue
    #if seq > 1: continue
    print seq,'%-32s %-16s %s %s'%(alg,cmd,tag,prop,)
  except Exception,X:
    print l,X
