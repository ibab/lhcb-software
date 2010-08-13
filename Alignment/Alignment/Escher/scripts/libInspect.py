#!/usr/bin/env python
__author__  = "Nicola Chiapolini, Albert Bursche"
__version__ = "1.0"

import GaudiPython 
from Gaudi.Configuration import * 
from Configurables import LHCbApp, ApplicationMgr
from GaudiPython.Bindings import gbl 


def run(startElementNames, opts, addFunction):
    startGaudiInspect(opts.aligndb)

    for element in startElementNames:
      startElement = getElementByName(element)
      addFunction(startElement)


def startGaudiInspect(aligndb = ""):
    global appMgr
    
    LHCbApp().DDDBtag   = ""
    LHCbApp().CondDBtag = ""

    if aligndb:
      from Configurables import ( CondDB, CondDBAccessSvc )
      counter = 1
      for db in aligndb:
	alignCond = CondDBAccessSvc( 'AlignCond' + str(counter) )
	alignCond.ConnectionString = 'sqlite_file:' + db + '/LHCBCOND'
	CondDB().addLayer( alignCond )
	counter += 1
    
    appConf = ApplicationMgr( OutputLevel = INFO, AppName = 'myBrunel' )
    appMgr = GaudiPython.AppMgr()


def getElementByName(elementName):
    return appMgr.detSvc()[elementName]


def getChildrenOf(element):
    children = element.childIDetectorElements() 
    for child in children: 
      print child.name()
    
    return children


def printTTNames():
    print "/dd/Structure/LHCb/BeforeMagnetRegion/TT"
    print "/dd/Structure/LHCb/BeforeMagnetRegion/TT/TTa"
    print "/dd/Structure/LHCb/BeforeMagnetRegion/TT/TTa/TTaXLayer"
    print "/dd/Structure/LHCb/BeforeMagnetRegion/TT/TTa/TTaXLayer/R1Module1T"
    print "/dd/Structure/LHCb/BeforeMagnetRegion/TT/TTa/TTaXLayer/R3Module6T/Ladder4"
    print "/dd/Structure/LHCb/BeforeMagnetRegion/TT/TTa/TTaXLayer/R3Module6T/Ladder4/Sensor41"



### Class Definitions ###

class GeometryNames:
  """Class containing the axis labels corresponding to 
     the variables in ElementGeometry"""
     
  X  = "X/mm" 
  nX = X
  Y  = "Y/mm"
  nY = Y
  Z  = "Z/mm"
  nZ = Z
  TX = "Tx/mm"
  TY = "Ty/mm"
  TZ = "Tz/mm"
  DX = "#Delta_{x}/mm"
  DY = "#Delta_{y}/mm"
  DZ = "#Delta_{z}/mm"
    
  def phi(self):
      return "#phi"
    
  def eta(self):
      return "#eta"
    
  def theta(self):
      return "#theta"
    
  def rho(self):  
      return "#rho/mm"
  def r(self):
      return "r/mm"


class ElementGeometry:
  """Geometry info needed for Alignment"""
  
  def __init__(self, detElement):
      geo = detElement.geometry()

      try: 
	self.name = geo.alignmentCondition().name()
      except:
	self.name = detElement.name()
	return
	  
      self.ownCenter = gbl.Gaudi.XYZPoint()
      
      self.n_globalCenter = geo.toGlobalMatrixNominal() * self.ownCenter
      self.globalCenter   = geo.toGlobalMatrix() * self.ownCenter

      self.n_parentCenter = geo.ownToLocalMatrixNominal().Inverse() * \
	self.ownCenter
      self.parentCenter   = geo.ownToNominalMatrix().Inverse() * \
	self.n_parentCenter

      self.X = self.globalCenter.x()
      self.Y = self.globalCenter.y()
      self.Z = self.globalCenter.z()
      
      self.nX = self.n_globalCenter.x()
      self.nY = self.n_globalCenter.y()
      self.nZ = self.n_globalCenter.z()

      self.TX = self.globalCenter.x() - self.n_globalCenter.x() 
      self.TY = self.globalCenter.y() - self.n_globalCenter.y()
      self.TZ = self.globalCenter.z() - self.n_globalCenter.z()
      
      self.DX = self.parentCenter.x() - self.n_parentCenter.x() 
      self.DY = self.parentCenter.y() - self.n_parentCenter.y()
      self.DZ = self.parentCenter.z() - self.n_parentCenter.z()

  def phi(self):
      """
      polar angle
      """
      if (self.X!=0):
	  return math.atan(self.Y/self.X)
      else:
	  return 0.0

  def rho(self):
      """
      distance from z axis
      """
      return math.sqrt(self.X**2+self.Y**2)

  def theta(self):
      if self.Z!=0:
	  return math.atan(self.rho()/self.Z)

  def eta(self):
      """
      pseudorapidity 
      """
      return -math.log(math.tan(self.theta()/2))

  def r(self):
      """
      distance to origin        
      """
      return math.sqrt(self.X**2+self.Y**2+self.Z**2)

  def printNominal(self):
      print self.nX
      print self.nY
      print self.nZ

  def printGlobal(self):
      print self.X
      print self.Y
      print self.Z
