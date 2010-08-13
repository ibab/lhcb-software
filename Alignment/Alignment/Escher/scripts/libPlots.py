#!/usr/bin/env python
__author__  = "Nicola Chiapolini, Albert Bursche"
__version__ = "1.0"

from ROOT import *
from libInspect import *

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
      #try:
      labels = self.functor(GeometryNames())
      #except:
#	labels = ["X","Y"]
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

