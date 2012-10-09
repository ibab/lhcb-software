import os
lines = os.popen('grep " StoreTracer " Brunel.traces','r').readlines()

start = 0
for l in lines:
  try:
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
    elif len(items)>8:
      tag = items[8]
      cmd = items[6]
    else:
      continue
    if seq > 1: continue
    print cmd,tag
  except Exception,X:
    print l,X
