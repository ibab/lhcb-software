from Configurables import TeslaMonitor, TeslaBrunelMonitor

class TeslaH1:
  def __init__ (self, input, title, nBins, min, max, cut=None):
    try:
      line,particle,variable = input.split(':')
      print "TurboLine: ", line
      print "Particle: ", particle
      print "Code: ", variable
    except:
      raise Exception ( 
          "Name expected in format TurboLine:Particle:LoKiVariable. Got " +
          input  )

    if cut == None: cut = "(ALL)"

    histname = ( ''.join(e for e in particle if e.isalnum()) + "_" +
                 ''.join(e for e in variable if e.isalnum()) )
    algName = line + histname


    self._alg = TeslaMonitor ( algName, Folder = line,
        Inputs   = ["/Event/Turbo/"+line+"/Particles"],
        DecayDescriptor = "["+particle+"]CC",
        Cut      = cut,
        Code     = [variable],
        Name     = histname,
        Title    = title,
        Min      = [min], Max = [max], nBins = [nBins]
      )

  def getAlgorithm(self):
    return self._alg




class TeslaH2:
  def __init__ (self, input, title, nBinsX, minX, maxX,
                                    nBinsY, minY, maxY, 
                                    cut = None):
    try:
      line,particle,var1,var2 = input.split(':')
      print "TurboLine: ", line
      print "Particle: ", particle
      print "Code variable1: ", var1
      print "Code variable2: ", var2
    except:
      raise Exception ( 
          "Name expected in format TurboLine:Particle:LoKiVar1:LoKiVar2. Got " +
          input  )

    histname = ( ''.join(e for e in particle if e.isalnum()) + "_" +
                 ''.join(e for e in var1 if e.isalnum()) + "_" +
                 ''.join(e for e in var2 if e.isalnum()) 
                 ) 
    algName = line + histname

    if cut == None: cut = "(TRUE)"

    self._alg = TeslaMonitor ( algName, Folder = line,
        Inputs   = ["/Event/Turbo/"+line+"/Particles"],
        DecayDescriptor = "["+particle+"]CC",
        Code     = [var1, var2],
        Cut      = cut,
        Name     = histname,
        Title    = title,
        Min      = [minX,minY], Max = [maxX,maxY], nBins = [nBinsX,nBinsY]
      )

  def getAlgorithm(self):
    return self._alg


class TeslaH3:
  def __init__ (self, input, title, nBinsX, minX, maxX,
                                    nBinsY, minY, maxY,
                                    nBinsZ, minZ, maxZ,
                                    cut = None):
    try:
      line,particle,var1,var2,var3 = input.split(':')
      print "TurboLine: ", line
      print "Particle: ", particle
      print "Code variable1: ", var1
      print "Code variable2: ", var2
      print "Code variable2: ", var3
    except:
      raise Exception ( 
          "Name expected in format TurboLine:Particle:LoKiVar1:LoKiVar2:LoKiVar3. Got " +
          input  )

    histname = ( ''.join(e for e in particle if e.isalnum()) + "_" +
                 ''.join(e for e in var1 if e.isalnum()) + "_" +
                 ''.join(e for e in var2 if e.isalnum()) + "_" +
                 ''.join(e for e in var3 if e.isalnum()) 
               )
    algName = line + histname

    if cut == None: cut = "(TRUE)"

    self._alg = TeslaMonitor ( algName, Folder = line,
        Inputs   = ["/Event/Turbo/"+line+"/Particles"],
        DecayDescriptor = "["+particle+"]CC",
        Cut      = cut,
        Code     = [var1, var2, var3],
        Name     = histname,
        Title    = title,
        Min      = [minX,minY,minZ], 
        Max      = [maxX,maxY,maxZ], 
        nBins    = [nBinsX,nBinsY,maxZ]
      )

  def getAlgorithm(self):
    return self._alg


class TeslaBrunelMonitorConf:
  def __init__ (self, name, TurboInput,       
                      Folder          = None,
                      MatchLocations  = None, 
                      HistName        = None, 
                      HistTitle       = None,
                      Variables       = None,
                      Tools           = None,
                      TeslaMatcher    = None):
    self.name = name;
    if not isinstance(TurboInput, list): TurboInput = [TurboInput]
    self.TurboInput = TurboInput
    self.Folder = Folder if Folder else "TeslaBrunelMonitor";
    self.MatchLocations = MatchLocations if MatchLocations else {}
    self.HistName   = HistName   if HistName  else "TeslaBrunelMonitor"
    self.HistTitle  = HistTitle  if HistTitle else "TeslaBrunelMonitor"
    self.TeslaMatcher= TeslaMatcher if TeslaMatcher else "TeslaMatcher:PUBLIC"
    self.Tools= Tools if Tools else []
    self.Variables = Variables if Variables else []

  def addVariable ( self, name, particle, code,  max, cut=None):
    self.Variables += [{
      "NAME"     : name,
      "PARTICLE" : particle,
      "CODE"     : code,
      "MAX"      : max,
      "CUT"      : cut if cut else "(ALL)"
    }]

  def addTool ( self, tool ):
    self.Tools += tool;

  def _list ( self, row ):
    return [ var[row] for var in self.Variables ]


  def getAlgorithm ( self ):
    alg = TeslaBrunelMonitor ( self.name, Folder = self.Folder,
                      Inputs   = ["/Event/Turbo/"+line+"/Particles" 
                                              for line in self.TurboInput],
                      Particle = ["["+p+"]CC" for p in self._list('PARTICLE')],
                      Cut      = self._list("CUT"),
                      Code     = self._list("CODE"),
                      Name     = self._list("NAME"),
                      Max      = self._list("MAX"),
                      HistName = self.HistName,
                      HistTitle= self.HistTitle,
                      TeslaMatcher=self.TeslaMatcher,
                      MatchLocations = self.MatchLocations,
                    )


    for tool in self.Tools:
      alg.addTool ( tool )

    return alg;







 

