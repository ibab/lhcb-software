

#================================================================================
class gparser:
#--------------------------------------------------------------------------------
  def __init__(self):
    self.outfilename = ''
    self.delimiter = '@@@'
    self.delen = len(self.delimiter)
    self.argv0 = 'GOD'
    self.stream = ''
    self.currinfilename = ''
    self.currlinenum = 0
#--------------------------------------------------------------------------------
  def currpos(self):
    return ' (file: ' + self.currfilename + ', line: ' + self.currlinenum.__str__() + ')'
#--------------------------------------------------------------------------------
  def findinstance(self, instance):
    pcs = instance.split('.')
    moi = ''
    fun = ''
    if len(pcs) != 2:
      print self.argv0 + ': ERROR: Don\'nt know how to handle ' + instance + self.currpos()
    else:
      moi = pcs[0]
      fun = pcs[1]
    if moi not in globals():
      print self.argv0 + ': ERROR: There is no module or instance ' + instance + self.currpos()
      return None
    if fun not in globals()[moi].__class__.__dict__:
      print self.argv0 + ': ERROR: There is no function ' + instance + self.currpos()
      return None    
    return (moi,fun)
#--------------------------------------------------------------------------------
  def call(self, cmd):
    pos = cmd.find('(')
    instancename = ''
    if (pos != -1):
      instancename  = cmd[:pos]
      arguments = cmd[pos+1:-1]
      print arguments
    else:
      print self.argv0 + ': ERROR: Cannot parse command ' + cmd + ' correctly' + self.currpos()
    instance = self.findinstance(instancename)
    if instance == None:
      print self.argv0 + ': ERROR: Cannot find instance ' + instancename + self.currpos()
    else:
      try:
        str = globals()[instance[0]].__class__.__dict__[instance[1]]()
      except:
        print self.argv0 + ': ERROR: Could not call ' + cmd + self.currpos()
      return str
    return ''
#--------------------------------------------------------------------------------
  def parse(self, templfile, dict):
    self.currfilename = templfile
    f = open(self.currfilename)
    for line in f.readlines():
      self.currlinenum += 1
      if line.find(self.delimiter) == -1:
        self.stream += line
      elif line.count(self.delimiter)%2 != 0:
        print self.argv0 + ': ERROR: Found uneven number of delimiters' + self.currpos()
      else:
        i = 0
        line2 = ''
	splitElems = line.split(self.delimiter)
	indent = 0
	if splitElems[0].isspace() : indent = len(splitElems[0])
        for word in splitElems:
          if not i%2 :
            line2 += word
          else:
            if dict.has_key(word):
	      dword = dict[word]
              if len(dword):
		if indent : dword = dword.replace('\n','\n%s'%(' '*indent))
                line2 += dword
            else:
              print self.argv0 + ':ERROR: Could not find dictionary key ' + word + self.currpos()
          i += 1        
        if len(line2.split()) : self.stream += line2
    f.close()
#--------------------------------------------------------------------------------
  def reset(self):
    self.currfilename = ''
    self.currlinenum = 0
    self.stream = ''

