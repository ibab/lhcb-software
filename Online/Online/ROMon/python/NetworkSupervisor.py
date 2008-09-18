import os, sys

class Ping:
  def __init__(self,who,count=1,size=None):
    self.name = who
    self.count = count
    self.size = size
  def test(self):
    cmd = 'ping '
    if self.size: cmd = cmt + ' -s '+str(self.size)
    cmd = cmd + ' -q -c ' + str(self.count) + ' ' + self.name
    lines = os.popen3(cmd)[1].readlines()
    tag = '%d packets transmitted, %d received, 0%% packet loss'%(self.count,self.count,)
    for l in lines:
      if l.find(tag)>=0:
        return 1
    return None

if __name__=="__main__":
  test = Ping(sys.argv[1]).test()
  print test
  
