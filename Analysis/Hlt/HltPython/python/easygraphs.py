#  @date 2007-11-28
#  @author Hugo RUIZ hugo.ruiz@cern.ch
#  See examples in the last function of the module!
# =============================================================================
""" Simple shortcuts to produce and draw graphs starting from python lists"""
# =============================================================================
__author__ = "Hugo RUIZ hugo.ruiz@cern.ch"
# =============================================================================


from ROOT import *


#---------------------------------------------------
def graph(listX,listY,xErr=[],yErr=[],title=''
          ,markerStyle=21,markerColor=1,markerSize=1):

    """ A shortcut to easily produce a graph from some python lists.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""

    if len(listX) != len(listY):
        print 'Both lists must have equal lengths'; raise RuntimeError
    n=len(listX)
    if xErr and not yErr or yErr and not xErr: 'Both xErr and yErr (or none) have to be supplied'; raise RuntimeError
    if xErr:
        if len(listX)!=len(xErr) or len(listX)!=len(yErr): print 'Errors and data list must have equal lenght'; raise RuntimeError
        myGraph=TGraphErrors(n)
    else:
        myGraph=TGraph(n)
   
    for i in range(0,n):
        myGraph.SetPoint(i,listX[i],listY[i])
        if xErr:
            myGraph.SetPointError(i,xErr[i],yErr[i])

    if title: myGraph.SetTitle(title)
    myGraph.SetMarkerStyle(markerStyle)
    myGraph.SetMarkerColor(markerColor)
    myGraph.SetMarkerSize(markerSize)

    if xErr:
        myGraph.SetLineColor(markerColor)

    return myGraph


#---------------------------------------------------
def prepare( myGraph,xTit=False,yTit=False,
         xMin=False,xMax=False,yMin=False,yMax=False,interLine=False,
         labelSize = 0.03, titleSize = 0.04,
         yOffset = .7, xOffset = .7, title = ''):

    """ A shortcut to easily format a graph or a multigraph.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""


    myGraph.Draw('ASP')
    myGraph.SetTitle(title)
    if xTit:myGraph.GetHistogram().GetXaxis().SetTitle(xTit)
    if yTit:myGraph.GetHistogram().GetYaxis().SetTitle(yTit)

    
    myGraph.GetHistogram().GetYaxis().SetTitleOffset(yOffset)
    myGraph.GetHistogram().GetYaxis().SetTitleOffset(xOffset)

    myGraph.GetHistogram().GetYaxis().SetTitleSize(titleSize)
    myGraph.GetHistogram().GetXaxis().SetTitleSize(titleSize)
    
    myGraph.GetHistogram().GetXaxis().SetLabelSize(labelSize)
    myGraph.GetHistogram().GetYaxis().SetLabelSize(labelSize)
    if xMin or xMax: myGraph.GetHistogram().GetXaxis().SetLimits(xMin,xMax)
    if yMax: myGraph.GetHistogram().SetMaximum(yMax)
    if yMin: myGraph.GetHistogram().SetMinimum(yMin)
    return



#---------------------------------------------------
def wait():
    return raw_input('Press key \n')



#---------------------------------------------------
def easyGraphsExample():
    myC = TCanvas()
    xList = [1.,2.,3.]
    yList = [1.2,2.3,3.4]
    xErrList = [.1,.1,.1]
    yErrList = [.1,.1,.1]
    g1 = graph(xList, yList, xErrList, yErrList, title = 'Example1', markerColor = 1)
    g2 = graph(yList, xList, title = 'Example2', markerColor = 2 )
    m = TMultiGraph()
    m.Add(g1)
    m.Add(g2)
    prepare(m, xTit = 'Energy (GeV)', yTit = 'Time (ns)')
    m.Draw()
    myC.Update()
    wait()
    return
