#!/usr/bin/env python
__author__  = "Nicola Chiapolini, Albert Bursche"
__version__ = "1.0"

from ROOT import *
from GaudiPython.Bindings import gbl 


class DynamicGraph(TGraph):
  """Wraper around TGraph 
     allows to add points one by one
     automatically adjusts size of TGraph"""
  
  def __init__(self,name):
      super( DynamicGraph, self).__init__()
      self.n = 1000
      self.current = 0
      self.SetMarkerSize(1)
      self.SetMarkerStyle(2)
      self.SetTitle(name)
  
  def Fill(self,point):
      if self.GetN() < self.current:
	  self.Set(self.GetN()+1000)
      self.SetPoint(self.current,point[0],point[1])
      self.current += 1


class Plot(object):
  """simple xy-plot with pairs of alignment variables"""

  def __init__(self,name,regex,fun,selector=lambda x:True):
      import re
      self.name = name
      self.re = re.compile(regex)
      self.functor = fun
      self.selector = selector
      
      self.points  = []
        
  def Fill(self,do):
      if self.re.match(do.name):
        if self.selector(do):
	  self.points.append(self.functor(do))
  
  def makeGraph(self):
      self.graph = DynamicGraph(self.name)
      for p in self.points:
	self.graph.Fill(p)
      self.graph.SetMarkerColor(2)
	  
  def getGraph(self):
      if (not hasattr(self, 'graph')):
	self.makeGraph()
      return self.graph

  def draw(self, options = "ap", labels = ""):
      self.getGraph().Draw(options)
      if (options == "ap"):
        self.setAxes(labels)

  def getLabels(self):
      try:
	labels = self.functor(GeometryNames())
      except:
	labels = ["X","Y"]
      return labels

  def setAxes(self, labels = ""):         
      xaxis = self.graph.GetXaxis()
      yaxis = self.graph.GetYaxis()
      if len(labels) == 2:
        x = labels[0]
        y = labels[1]
      else:
        [x,y] = self.getLabels()
      
      for (val,axis) in [(x,xaxis),(y,yaxis)]:
        axis.SetTitle(val)
      #self.initialised = True  


class ArrowPlot(Plot):
  """Plot showing two points plus an arrow for each 
     alignment varibales. (e.g. usful to display shifts)"""
    
  def __init__(self,name,regex,fun,selector=lambda x:True):
      super( ArrowPlot, self ).__init__(name,regex,fun,selector)
      del self.points
      
      self.arrows      = []
      self.startPoints = []
      self.endPoints   = []
        
  def Fill(self,do):
      if self.re.match(do.name):
	if self.selector(do):
	  vals = self.functor(do)
	  self.startPoints.append([vals[0],vals[1]])
	  self.endPoints.append(  [vals[2],vals[3]])

  def makeGraph(self):
      self.graph = DynamicGraph(self.name)
      for s, e in zip(self.startPoints, self.endPoints):
	self.graph.Fill(s)
	self.graph.Fill(e)
	a = TArrow(s[0],s[1],e[0],e[1],0.005,"->")
	self.arrows.append(a)
      self.graph.SetMarkerColor(2)
  
  def draw(self, options = "ap", labels = ""):
      super( ArrowPlot, self).draw(options, labels)

      for a in self.arrows:
	a.Draw()

  def getLabels(self):
      try:
	[x,y,x2,y2] = self.functor(GeometryNames())
      except:
	x = "X"
	y = "Y"
      return [x, y]


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


class PlotFunctions:
  @staticmethod
  def zxShifts(do):
      return [do.nZ,do.nX,do.Z,do.X]
      
  @staticmethod
  def xyShifts(do):
      return [do.nX,do.nY,do.X,do.Y]

  @staticmethod
  def zyShifts(do):
      return [do.nZ,do.nY,do.Z,do.Y]

