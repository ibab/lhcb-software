from ROOT import *

import os,time,math
import CondDBUI
import CondDBUI.Admin
from PyCool import cool
import GaudiPython #
from GaudiPython.Bindings import gbl 

from Gaudi.Configuration import * #


from Configurables import LHCbApp, ApplicationMgr#

def startInspect():
  global appMgr
  
  LHCbApp().DDDBtag   = "head-20100518"
  LHCbApp().CondDBtag = "head-20100518"

  try:
    if alignDB:
      from Configurables import ( CondDB, CondDBAccessSvc )
      counter = 1
      for db in alignDB:
	alignCond = CondDBAccessSvc( 'AlignCond' + str(counter) )
	alignCond.ConnectionString = 'sqlite_file:' + db + '/LHCBCOND'
	CondDB().addLayer( alignCond )
	counter += 1
  except:
    pass

  appConf = ApplicationMgr( OutputLevel = INFO, AppName = 'myBrunel' )
  appMgr = GaudiPython.AppMgr()
  print "/dd/Structure/LHCb/BeforeMagnetRegion/TT"

def getElementByName(elementName):
  return appMgr.detSvc()[elementName]



def printCoordinates(name, coordObject):
  print "*) "+name
  print coordObject.X()
  print coordObject.Y()
  print coordObject.Z()  


def inspect(detElement):
  print "**) "+detElement.name()

  geo = detElement.geometry()
  point = gbl.Gaudi.XYZPoint()
  toGlobal = geo.toGlobalMatrixNominal()
  nominalPosition = toGlobal * point
  printCoordinates("Nominal Position", nominalPosition)

  toGlobalTrans = gbl.Gaudi.Transform3D(toGlobal.Translation())
  toGlobalTransVec = toGlobal.Translation().Vect()
  print "*) Global Z-Translation to Nominal"
  print toGlobalTransVec.Z()  # 2485

  toGlobalDB = geo.toGlobalMatrix()
  dbPosition = toGlobalDB * point
  printCoordinates("Position", dbPosition)
  
  toDelta = toGlobalDB * toGlobal.Inverse()
  delta = toDelta * nominalPosition
  # delta == dbPosition

  toDeltaLocal = toGlobalTrans.Inverse() * toDelta * toGlobalTrans
  deltaLocal = toDeltaLocal * point
  printCoordinates("Shifts w.r.t. Nominal", deltaLocal)

  trans = gbl.vector('double')(3)
  rot = gbl.vector('double')(3)
  gbl.DetDesc.getZYXTransformParameters(toDeltaLocal,trans, rot)
  print "*) Translation vector"
  print trans.at(0)
  print trans.at(1)
  print trans.at(2)


def getChildren(detElement):
  print "Children of "+detElement.name()
  print "---"

  children = detElement.childIDetectorElements()
  for child in children:
    print child.name()

  return children


def getRotation(detElement):
  ex =  gbl.Gaudi.XYZPoint(1,0,0)
  discr = (detElement.geometry().toGlobalMatrixNominal().Rotation()
    *ex).Phi()/math.pi
  rotations = [0,-0.0277777777778,0.0277775985909, 0.972222222222,
    -0.972222401409,1,-1]
  pattern = [fabs (discr-x)<0.001 for x in rotations]
  if True not in pattern:
    print "unknown" , discr 
  if True in pattern[3:7]:
    print "180deg", discr
  else: 
    print "0deg", discr
    #RotZ180deg =  ROOT.Math.Transform3D(ROOT.Math.RotationZ(math.pi))
    #localdelta = localdelta * RotZ180deg


def listAllRotations():
  layers    = ["aX", "aU", "bV", "bX"]
  regions   = ["1", "2", "3"]
  topbottom = ["T", "B"]

  for layer in layers:
    for reg in regions:
      for mod in topbottom:
	element = getElementByName("/dd/Structure/LHCb/BeforeMagnetRegion/TT/TT"+
	  layer[0]+"/TT"+layer+"Layer/R"+reg+"Module1"+mod)
	print element.name()
	checkRotation(element)
	
