from ROOT import *
from easygraphs import *

#---------------------------------------------------
def integralHisto( h ):
   h.ComputeIntegral()
   integral = h.GetIntegral()
   outHisto = h.Clone()
   outHisto.SetContent(integral)
   return outHisto


#---------------------------------------------------
def getContents ( h ):
    outList = []
    n = h.GetNbinsX()
    for i in range(n):
        outList.append(h.GetBinContent(i))
    return outList

#---------------------------------------------------
def normalizeHisto (h):
    integral = sum( getContents ( h ) )
    invIntegral = 1./integral
    outHisto = h.Clone()
    contents = getContents( h )
    normalizedContents = map(lambda a:a*invIntegral, contents)
    setContents( outHisto, normalizedContents)
    return outHisto
    
#---------------------------------------------------
def setContents ( h, inputList ):
    n = h.GetNbinsX()
    if n!=len(inputList):
        print 'INPUT LIST DOES NOT HAVE AS MUCH ELEMENTS AS HISTOGRAM BINS'
        raise RuntimeError

    i = 0
    for iValue in inputList:
        h.SetBinContent(i, iValue)
        i +=1
    return


#---------------------------------------------------
def reverseHisto ( h ):
    n = h.GetNbinsX()
    outHisto = h.Clone()
    contents = getContents( h )
    contents.reverse()
    setContents( outHisto, contents)
    return outHisto


#---------------------------------------------------
def efficiencyVsRetention(signalHisto, allHisto , factorSignal = 1., factorAll = 1., inv = False, **args ):
    if inv:
        signalHisto = reverseHisto(signalHisto)
        allHisto = reverseHisto(allHisto)
    integrSignal = map(lambda a:a*factorSignal, getContents( integralHisto( signalHisto)))
    integrAll = map(lambda a:a*factorAll, getContents( integralHisto( allHisto) ))
    return graph( integrSignal, integrAll, **args)


#---------------------------------------------------
def histotoolsExample():
    
    h1 = TH1F('titi','titi title',10,0.,10.) 

    contents = [0.,1.,2.,4.,6.,8.,3.,1.,0.,1.]
    setContents(h1, contents)

    h1.Draw()
    raw_input('Press key when you have looked at the original histogram \n') # To give you time to wach the histo

    h2 = reverseHisto( h1 )
    h2.Draw()
    raw_input('Press key when you have looked at the reversed histogram\n')

    h3 = normalizeHisto( h1 )
    h3.Draw()
    raw_input('Press key when you have looked at the normalized histogram\n')


    hSignal = h3
    hAll = h2

    myC = TCanvas()
    g = efficiencyVsRetention( hSignal, hAll, 1., 10000.,  title = 'Trigger optimization')
    prepare(g, xTit = 'Rate (Hz)', yTit = 'Efficiency' )
    g.Draw()
    myC.Update()
    raw_input('Press key when you have looked at the efficiency vs rate graph \n')

