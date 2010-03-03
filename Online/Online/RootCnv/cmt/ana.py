import sys
class FirstSummary:
  def __init__(self):
    self.type = ''
    self.protocol = ''
    self.nevt = 0
    self.data={}
    self.opt={}
    self.step={}
    self.stepno={}

class Summary:
  def __init__(self):
    self.type = ''
    self.protocol = ''
    self.step = {}

class Step:
    def __init__(self):
        self.name=''
        self.type=''
        self.protocol = ''
        self.data=''
        self.usage=None
        self.cpu = 0.
        self.events = 0
        self.user=0.
        self.system=0.
        self.ellapsed = 0.
        
    def toString(self):
        s1 = '%-8s %-8s %-12s %7d %8.2f %8.2f %8.2f %7.2f '%(self.type,self.protocol,self.name,self.events,self.ellapsed,self.user,self.system,self.cpu)
        s2 = '%8.2e %8.2e %8.2e'%(self.ellapsed/self.events,self.user/self.events,self.system/self.events)
        return s1+s2

    def diff(self,other):
        e1 = self.ellapsed/self.events
        e2 = other.ellapsed/other.events
        e3 = (self.user+self.system)/self.events
        e4 = (other.user+other.system)/other.events
        s1 = '%8.2e %8.2e'%(e1-e2,e3-e4)
        return s1
    
class Summary:
    def __init__(self):
        self.step={}

lines=open(sys.argv[1],"r").readlines()
step = None
line = 999

all_summs={}
prot=None
summary=None
for l in lines:
  if l.find('Write ')==0:
    itm=l.split(' ')
    step = 0;
    line = 0
    summary = FirstSummary()    
    summary.nevt = int(itm[4])
    summary.type = itm[1]
    summary.protocol = itm[3]
    summary.step[step] = 'Write'
    all_summs[summary.protocol]=summary
    summary.stepno[summary.step[step]] = step
  elif l.find('Read ')==0:
    itm=l.split(' ')
    if not all_summs.has_key(itm[3]):
      summary = FirstSummary()    
      summary.nevt = int(itm[4])
      summary.type = itm[1]
      summary.protocol = itm[3]
      all_summs[summary.protocol]=summary
    step = 2;
    line = 0
    summary.step[step] = 'Read'
    summary.stepno[summary.step[step]] = step
  elif l.find('CollWrite')==0:
    step = 1;
    line = 0
    summary.step[step] = 'CollWrite'
    summary.stepno[summary.step[step]] = step
  elif l.find('CollRead 90')==0:
    step = 3;
    line = 0
    summary.step[step] = 'CollRead 90'
    summary.stepno[summary.step[step]] = step
  elif l.find('CollRead 50')==0:
    step = 4;
    line = 0
    summary.step[step] = 'CollRead 50'
    summary.stepno[summary.step[step]] = step
  elif l.find('CollRead 10')==0:
    step = 5;
    line = 0
    summary.step[step] = 'CollRead 10'
    summary.stepno[summary.step[step]] = step

  if step is not None:
    line = line+1
    if line==2:
      summary.data[step] = l[:-1]
    elif line==3:
      summary.opt[step] = summary.nevt
      if step>1:
        summary.opt[step] = int(l[l.find(' #=')+3:-1])
  

steps = {}
opts = {}
for k in all_summs.keys():
  summary = all_summs[k]
  ###print 'Summary:  %32s [%s] %d  %d steps'%(summary.protocol,summary.type,summary.nevt,len(summary.data),)
  for s in summary.data.keys():
    opt=''
    if summary.opt.has_key(s): opt=summary.opt[s]
    ###print '%-16s %s'%(summary.step[s],summary.data[s])#,'\n',opt
    steps[s]=summary.step[s]
    opts[s]=summary.opt[s]

sum = {}
sum['castor'] = Summary()
sum['file'] = Summary()


for sk in steps.keys():
  i = steps[sk]
  for k in all_summs.keys():
    summary = all_summs[k]
    #print i,'Summary:  %32s [%s] %d  %d steps'%(summary.protocol,summary.type,summary.nevt,len(summary.data),)
    for s in summary.data.keys():
      if i==summary.step[s]:
        nevt = opts[s]
        itms = summary.data[s].split('\t')[0].split(' ')
        #print itms
        user = float(itms[0][:-1])
        system = float(itms[1][:-1])
        cpu = float(itms[3][:-1])
        ellapsed = (user+system)/cpu*100.0
        #print '%-8s %-8s %-12s %s %6.2f %6.2f %4.2f %4.1f'%(summary.type,summary.protocol,summary.step[s],summary.data[s],ellapsed,user,system,cpu,)
        s1 = '%-8s %-8s %-12s %8.2f %8.2f %7.2f %4.1f %7d'%(summary.type,summary.protocol,summary.step[s],ellapsed,user,system,cpu,nevt,)
        s2 = '%8.2e %8.2e %8.2e'%(ellapsed/nevt,user/nevt,system/nevt)
        ###print s1,s2

        item = Step()
        item.name = summary.step[s]
        item.type = summary.type
        item.protocol = summary.protocol
        item.data = summary.data[s]
        item.cpu = cpu
        item.events = nevt
        item.user=user
        item.system=system
        item.ellapsed = ellapsed
        item.usage = (nevt,cpu,user,system,ellapsed)
        sum[summary.protocol].step[summary.stepno[i]] = item

print 132*'-'        
file   = sum['file']
castor = sum['castor']
for s in file.step.keys():
  step_f = file.step[s]
  step_c = castor.step[s]
  print step_f.toString()
  print step_c.toString(),step_c.diff(step_f)

