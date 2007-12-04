# =============================================================================
""" @namespace histotools
@brief Simple histogram operations like reversing or normalizing, and getting eff vs ret plots out of them

For an example see histotoolsExample() function
@author Hugo Ruiz hugo.ruiz@cern.ch
@date 2007-11-28
"""
# =============================================================================

from ROOT import *
import easygraphs

#---------------------------------------------------
def integralHisto( h ):
   """ Produce a cummulative histogram
   @param h Input histogram
   @returns A integral in which each bin corresponds to the cummulative of the input histogram
   @author Hugo Ruiz, hugo.ruiz@cern.ch
   """
   h.ComputeIntegral()
   integral = h.GetIntegral()
   outHisto = h.Clone('Integral_'+h.GetName())
   outHisto.SetContent(integral)
   return outHisto


#---------------------------------------------------
def getContents ( h , overFlows = False ):
   """ Returns a python list with the bin contents
   @param h Input histogram
   @param overFlows If set to True, the 1st and last bin correspond to underflow and overflow
   @returns Python list with bin contents
   @author Hugo Ruiz, hugo.ruiz@cern.ch
   """
   outList = []
   n = h.GetNbinsX()
   for i in range(n+2):
      outList.append(h.GetBinContent(i))
   if overFlows:
      return outList
   else:
      return outList[1:-1]
   
#---------------------------------------------------
def setContents ( h, inputList ):
   """ Sets histogram bin contents from a python list
   @param h Input histogram
   @param inputList List of contents to be set
   @author Hugo Ruiz, hugo.ruiz@cern.ch
   """
   n = h.GetNbinsX()
   if n!=len(inputList):
      print 'INPUT LIST DOES NOT HAVE AS MUCH ELEMENTS AS HISTOGRAM BINS'
      raise RuntimeError
   
   i = 1
   for iValue in inputList:
      h.SetBinContent(i, iValue)
      i +=1
   return
   
   
#---------------------------------------------------
def normalizeHisto (h, integral = 1.):
   """ Normalizes a histogram to have an integral equal to the required value
   @param h Input histogram
   @param integral Required value of the integral
   @returns Normalized histogram
   @author Hugo Ruiz, hugo.ruiz@cern.ch
   """
   oldIntegral = sum( getContents ( h ) )
   invIntegral = integral/oldIntegral
   outHisto = h.Clone('Normalized_'+h.GetName())
   contents = getContents( h )
   normalizedContents = map(lambda a:a*invIntegral, contents)
   setContents( outHisto, normalizedContents)
   return outHisto


#---------------------------------------------------
def reverseHisto ( h ):
   """ Reverses x coordinate of a histogram
   @param h Input histogram
   @returns Reversed histogram
   @author Hugo Ruiz, hugo.ruiz@cern.ch
    """
   n = h.GetNbinsX()
   outHisto = h.Clone('Reverse_'+h.GetName())
   contents = getContents( h )
   contents.reverse()
   setContents( outHisto, contents)
   return outHisto


#---------------------------------------------------
def efficiencyVsRetention(signalHisto, allHisto , factorSignal = 1., factorAll = 1., reverse = False, **args ):
   """ Produces a efficiency vs retentioin plot from distributions of signal and all events of a given variable
   @param signalHisto Signal distribution histogram
   @param allHisto All events distribution histogram
   @param factorSignal Factor required to convert number of signal events into an efficiency
   @param factorAll Factor required to convert number of events into a rate
   @param invert If set to true, both histograms are inverted  
   @returns An efficiency vs retention graph
   @author Hugo Ruiz, hugo.ruiz@cern.ch
   """
   if reverse:
      signalHisto = reverseHisto(signalHisto)
      allHisto = reverseHisto(allHisto)
   integrSignal = map(lambda a:a*factorSignal, getContents( integralHisto( signalHisto)))
   integrAll = map(lambda a:a*factorAll, getContents( integralHisto( allHisto) ))
   return easygraphs.graph( integrSignal, integrAll, **args)
   
   
#---------------------------------------------------
def histotoolsExample():
   
   h1 = TH1F('titi','titi title',10,0.,10.) 

   contents = [0.,1.,2.,3.,4.,5.,6.,7.,8.,9.]
   setContents(h1, contents)

   myC = TCanvas()
   h1.Draw()
   myC.Update()
   raw_input('Press key when you have looked at the original histogram \n') # To give you time to wach the histo


   h2 = reverseHisto( h1 )
   h2.Draw()
   myC.Update()
   raw_input('Press key when you have looked at the reversed histogram\n')

   h3 = normalizeHisto( h1 )
   h3.Draw()
   myC.Update()
   raw_input('Press key when you have looked at the normalized histogram\n')


   hSignal = h3
   hAll = h2

   myC = TCanvas()
   g = efficiencyVsRetention( hSignal, hAll, 1., 10000.,  title = 'Trigger optimization')
   easygraphs.prepare(g, xTit = 'Rate (Hz)', yTit = 'Efficiency' )
   g.Draw()
   myC.Update()
   raw_input('Press key when you have looked at the efficiency vs rate graph \n')

   return
