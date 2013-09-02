ref="../refs/physics.2012_v14r8.timing.ref"

newref=ref+".new"

changeref=newref+".auto"

fref=open(ref)
fnref=open(newref)
tref=fref.read()
tnref=fnref.read()
fref.close()
fnref.close()

added=[line for line in tnref.split('\n') if line.split(':')[0] not in tref]

print '\n'.join(added)
print len(added)

olddict={}
newdict={}

replace=[]

for line in tref.split('\n'):
    olddict[':'.join(line.split(':')[:-1])]=int(line.split(':')[-1])

for line in tnref.split('\n'):
    newdict[':'.join(line.split(':')[:-1])]=int(line.split(':')[-1])

for line in tnref.split('\n'):
    line=line.strip()
    hltline=':'.join(line.split(':')[:-1]).strip()
    if hltline in olddict:
        replace.append(": ".join([hltline,str(olddict[hltline])]))
    else:
        replace.append(line)

cref=open(changeref,'w')
cref.write("\n".join(replace))
cref.close()
