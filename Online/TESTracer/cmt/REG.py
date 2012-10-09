import os
lines = os.popen("grep REGOBJ Brunel.TES.trace.log","r").readlines()
s = {}
a = []
for l in lines:
  items = l.split()
  alg = items[3][1:]
  if items[2]=='(Framework)':
    tag = items[7]
  else:
    tag = items[8]
  ##print alg,tag
  if not s.has_key(tag):
    s[tag] = len(s)
    a.append(tag)

for i in a:
  print i
