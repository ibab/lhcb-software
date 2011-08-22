# Globals
fitter        = None
imageFileName = ''
canvas        = None
initialised   = False

def initialise():

    if not globals()['initialised'] :

        import os

        # Check results dir
        if not os.path.exists("results") : os.mkdir("results")

        from ROOT import gROOT
        # No info messages
        gROOT.ProcessLine("gErrorIgnoreLevel = kWarning;")
        # Batch mode (no TCanvas)
        gROOT.SetBatch(True)

        import GaudiPython
    
        # Initialise a few things
        from Configurables import DDDBConf, CondDB, LHCbApp, CondDBAccessSvc
        cDB = CondDB()

        #DDDBConf(DataType = "2009")
        #LHCbApp().DDDBtag   = "head-20110303"
        #LHCbApp().CondDBtag = "head-20110524"
        
        #DDDBConf(DataType = "2010")
        #LHCbApp().DDDBtag   = "head-20110303"
        #LHCbApp().CondDBtag = "head-20110524"
        
        DDDBConf(DataType = "2011")
        LHCbApp().DDDBtag   = "head-20110722" 
        LHCbApp().CondDBtag = "head-20110722"
        #LHCbApp().CondDBtag = "HEAD"

        # Aerogel Sub Tiles
        CondDB().LocalTags["LHCBCOND"] = ["rich1-20110624"]
        CondDB().LocalTags["DDDB"]     = ["rich1-20110624"]

        # Set message level to info and above only
        msgSvc().setOutputLevel(3)

        # Finally, initialize GaudiPython
        GaudiPython.AppMgr().initialize()

        # Initialise various DeRich objects
        loadRichDet()

        # flag as done
        globals()['initialised'] = True

def msgSvc():
    import GaudiPython
    return GaudiPython.AppMgr().service('MessageSvc','IMessageSvc')

def iDataSvc():
    import GaudiPython
    return GaudiPython.AppMgr().detSvc()

def umsSvc():
    import GaudiPython
    GaudiPython.AppMgr().createSvc('UpdateManagerSvc')
    return GaudiPython.AppMgr().service('UpdateManagerSvc','IUpdateManagerSvc')

def iDetDataSvc():
    import GaudiPython
    return GaudiPython.AppMgr().service('DetectorDataSvc','IDetDataSvc')

def richSystem():
    return iDataSvc()["Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem"]

def richAerogel():
    return iDataSvc()["Structure/LHCb/BeforeMagnetRegion/Rich1/Aerogel"]

def loadRichDet():
    iDataSvc()["Structure/LHCb/AfterMagnetRegion/Rich1"]
    iDataSvc()["Structure/LHCb/AfterMagnetRegion/Rich2"]
    iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel0"]
    iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel1"]
    iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich2/PDPanel0"]
    iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich2/PDPanel1"]
    richSystem()
    richAerogel()

def rootCanvas():
    from ROOT import TCanvas
    if globals()["canvas"] == None :
        import rootStyle
        rootStyle.applyRootStyle()
        globals()["canvas"] = TCanvas("CKCanvas","CKCanvas",1050,750)
    return globals()["canvas"]

def printCanvas(tag=''):
    canvas = rootCanvas()
    canvas.Update()
    splits = imageFileName.split(".")
    nsplits = len(splits)
    if nsplits == 0 :
        print "ERROR : No image type extension on file name", imageFileName
    else:
        imageType = splits[nsplits-1]
        if tag == '[' or tag == '{' : print "Printing to", imageFileName
        canvas.Print(imageFileName+tag,imageType)
        if tag == ']' or tag == '}' : print "Printing done ..."

def xmlHeader():
    return """<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">
<DDDB>

"""   

def xmlFooter():
    return """
</DDDB>"""

def run(rootFile="/usera/jonesc/AeroCalib-2011.root"):

    from ROOT import TFile

    # Load the Aerogel DetElem
    aerogel = richAerogel()

    # Load the root file
    file = TFile(rootFile)

    # Parameters
    minEntries = 100000

    # Set output PDF name
    globals()["imageFileName"] = "results/Aerogel.pdf"
    printCanvas('[')

    # Start the new XML
    newXML = xmlHeader()

    # primary tile IDs to use the full sub tile calibration for
    fullCalibTiles = [ 2, 4, 10, 12 ]

    # Loop over the tiles
    tiles = aerogel.radiators()
    print "found", len(tiles), "aerogel tiles"
    for tile in  tiles :

        # Load the conditions
        cond = tile.condition( "AerogelParameters" )

        # get the current value of n
        nzero = cond.paramAsDouble("CurrentAerogel_nAtFixedLambda")

        # Get the histogram to fit
        OK = False
        if tile.subTile() and tile.primaryTileID() in fullCalibTiles :
            
            histID = "RICH/RiCKResLongTight/Aerogel/subtiles/ckResAll-SubTile" + str(tile.tileID())
            print "Full SubTile Calibration for", tile.tileID()
            hist = file.Get(histID)
            if hist.GetEntries() < minEntries :
                print "  -> Stats too low. Try with full tile instead"
            else :
                OK = True
                    
        if not OK :
            
            histID = "RICH/RiCKResLongTight/Aerogel/tiles/ckResAll-Tile" + str(tile.primaryTileID())
            print "Full Tile Calibration for", tile.tileID()
            hist = file.Get(histID)
                    
        print "  -> Using", histID
    
        # fit
        result = fit(hist)
        printCanvas()
        
        if result['OK'] :
            N = newN(nzero,result['shift'])
            
            # Update condition
            cond.addParam("CurrentAerogel_nAtFixedLambda",N)

            print "   -> Shift =", result['shift'], "oldN =", nzero, "newN =", N

        # Write out the new condition
        newXML += cond.toXml() + '\n' + '\n' 

    newXML += xmlHeader()

    textShifts = open("results/New-Aerogel.xml",'w')
    textShifts.write(newXML)
    textShifts.close()
            
    printCanvas(']')
    file.Close()

def fit(hist):

    from ROOT import TF1

    # Default return result
    result = { 'OK' : False, "Message" : "No Message" }

    # Parameters
    minEntries = 100000
    FitColor = 2

    entries = hist.GetEntries()
    if entries < minEntries :
        
        result['Message'] = "Too few histogram entries"
        
    else:

        searchX = [-0.01,0.01]

        # Get x value of highest content bin in the search range
        maxBin = 0
        max = 0
        for bin in range(1,hist.GetNbinsX()-1):
            x = hist.GetBinCenter(bin) 
            if x > searchX[0] and x < searchX[1] :
                cont = hist.GetBinContent(bin)
                if cont > max :
                    max = cont
                    maxBin = bin
        
        # (rough estimate of peak position)
        xPeak = hist.GetBinCenter(maxBin)        
        if xPeak < searchX[0] or xPeak > searchX[1] : xPeak = 0

        # Fit range
        fitMin = xPeak - 0.0035
        fitMax = xPeak + 0.0035
        
        fFuncType = "gaus"
        fFitF = TF1("FitF",fFuncType,fitMin,fitMax)
        fFitF.SetParName(0,"Gaus Constant")
        fFitF.SetParName(1,"Gaus Mean")
        fFitF.SetParName(2,"Gaus Sigma")
        fFitF.SetLineColor(FitColor)
        hist.Fit(fFitF,"QRSE")
        #hist.Draw()
        #fFitF.Draw('SAME')

        result['shift'] = fFitF.GetParameter(1)
        result['OK'] = True

    return result

def newN(oldN,shift):

    from math import acos, cos

    thetaZero = acos(1.0/oldN)
    N = 1.0 / cos(thetaZero+shift)

    return N
