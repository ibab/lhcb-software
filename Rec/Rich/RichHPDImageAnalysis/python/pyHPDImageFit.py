
# Build pyminuit2. Would be nice if this was part of LCG area ...
import os, sys
os.system( "(cd pyminuit2 ; python setup.py build_ext --inplace)")
sys.path.append("pyminuit2/lib")

import minuit2

histFunction = None

class HPDBoundaryFcn :

    pixelsize = 0.5
    siliconx  = 16.0
    silicony  = 16.0

    errDef    = 0.0
    threshold = 0.0
    hist      = None
    sf        = 0.0
    boundary  = [ ]

    def __init__(self,_effDef=1.0,_threshold=0.0,_hist=None) :
        self.errDef    = _effDef
        self.threshold = _threshold
        self.hist      = _hist
        if _hist != None:
            self.sf        = (1.0*_hist.GetNbinsX())/(1.0*_hist.GetNbinsY())

    def nPixels(self):
        value = 0.0
        if None != self.hist : value = self.hist.Integral()
        return value

    def hasNeighbour(self,COL,ROW,thr):
        for icol in range(COL-1,COL+2):
            for irow in range(ROW-1,ROW+2):
                if COL == icol and ROW == irow : continue
                if ( icol >= 0 and icol < self.hist.GetNbinsX() and
                     irow >= 0 and irow < self.hist.GetNbinsY() ):
                    if self.hist.GetBinContent(icol+1,irow+1) > thr :
                        return True
        return False

    def findBoundary(self):

        if None == self.hist : return 0.0
        self.boundary = [ ]

        nbins = self.hist.GetNbinsX() * self.hist.GetNbinsY()
        thr   = self.threshold*self.hist.Integral()/(1.0*nbins)

        for icol in range(0,self.hist.GetNbinsX()):
            ROW0 = -1
            ROW1 = -1
            for irow in range(0,self.hist.GetNbinsY()):
                if ( self.hasNeighbour(icol,irow,thr) and
                     self.hist.GetBinContent(icol+1,irow+1) > thr ):
                    ROW0 = irow 
                    break
            for irow in range(0,self.hist.GetNbinsY()):
                if ( self.hasNeighbour(icol,irow,thr) and
                     self.hist.GetBinContent(icol+1,self.hist.GetNbinsX()-irow) > thr ):
                    ROW1 = self.hist.GetNbinsX() - irow - 1
                    break
            if -1 != ROW0 :
                self.boundary += [[icol,ROW0]]
            if -1 != ROW1 and ROW1 != ROW0 :
                self.boundary += [[icol,ROW1]]

        return len(self.boundary)

    def localXFromPixels( self, col ):
        return -1.0 * ( col*self.pixelsize + 0.5*self.pixelsize - 0.5*self.siliconx )

    def localYFromPixels( self, row ):
        return -1.0 * ( 0.5*self.silicony - row*self.pixelsize - 0.5*self.pixelsize )

    def localErrorFromPixels( self, pixerr ):
        return  pixerr * self.pixelsize

def chiSquare(x,y,r):

    from math import sqrt

    fitFunc = globals()['histFunction']
    
    chi2 = 0.0
    for bound in fitFunc.boundary:

        deltaCol = (1.0        * bound[0]) - x
        deltaRow = (fitFunc.sf * bound[1]) - y
        dist = sqrt( deltaCol*deltaCol + deltaRow*deltaRow )
        chi2 += (dist-r) * (dist-r) * 12.0

    return chi2
    
def fit( rootfile, hpdcopynr, minEntries ):

    # Fit result
    result = { "OK" : False }

    # Get the histogram for this HPD
    image = rootfile.Get('RICH/RichHPDImageSummary/Rich_HPD_'+str(hpdcopynr)+'_Image')

    entries = image.Integral()
    if entries < minEntries : return result

    cutFraction = 0.1
    minBoundary = 3.0

    # Make Fit Function
    fitFunc = HPDBoundaryFcn(_threshold=cutFraction,_hist=image)
    globals()['histFunction'] = fitFunc

    boundarySize = fitFunc.findBoundary()
    if boundarySize < minBoundary : return result

    # Do the fit
    m = minuit2.Minuit2 ( chiSquare, x=16, y=16, r=16 )
    m.strategy = 1
    m.migrad()

    # Get the results
    x    = fitFunc.localXFromPixels( m.values["x"] )
    y    = fitFunc.localYFromPixels( m.values["y"] )
    xErr = fitFunc.localErrorFromPixels( m.errors["x"] )
    yErr = fitFunc.localErrorFromPixels( m.errors["y"] )

    result["OK"]     = True
    result["XShift"] = (x,xErr)
    result["YShift"] = (y,yErr)
    return result
