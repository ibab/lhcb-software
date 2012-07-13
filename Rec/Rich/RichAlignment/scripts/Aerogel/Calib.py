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
        
        #DDDBConf(DataType = "2011")
        #LHCbApp().DDDBtag   = "head-20110722" 
        #LHCbApp().CondDBtag = "head-20110722"
        #LHCbApp().CondDBtag = "HEAD"

        DDDBConf(DataType = "2012")
        LHCbApp().DDDBtag   = "head-20120413"
        LHCbApp().CondDBtag = "head-20120420"
        LHCbApp().CondDBtag = "HEAD"
        CondDB().addLayer(CondDBAccessSvc("2012Aerogel",
                                          ConnectionString="sqlite_file:2012Aerogel.db/LHCBCOND",
                                          DefaultTAG="HEAD"))

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
    #iDataSvc()["Structure/LHCb/AfterMagnetRegion/Rich1"]
    #iDataSvc()["Structure/LHCb/AfterMagnetRegion/Rich2"]
    #iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel0"]
    #iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel1"]
    #iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich2/PDPanel0"]
    #iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich2/PDPanel1"]
    richSystem()
    #richAerogel()

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

def run(rootFile="2012.root"):

    from ROOT import TFile

    # Load the Aerogel DetElem
    aerogel = richAerogel()

    # Load the root file
    file = TFile(rootFile)

    # Parameters
    minEntries = 1000000

    # Set output PDF name
    globals()["imageFileName"] = "results/Aerogel.pdf"
    printCanvas('[')

    # Root histo dir
    rootHistDir = "RICH/RiCKResLongTight/Aerogel/"
    #rootHistDir = "RICH/RiCKResLong/Aerogel/"

    # Start the new XML
    newXML = xmlHeader()

    # primary tile IDs to use the full sub tile calibration for
    #fullCalibTiles = [ ]
    fullCalibTiles = [ 2, 3, 6, 11, 13 ]
    #fullCalibTiles = [ 2 ]
    #fullCalibTiles = [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 ]

    # Loop over the (all) tiles
    tiles = [ ]
    for t in aerogel.fullTileRadiators() : tiles += [t]
    for t in aerogel.subTileRadiators()  : tiles += [t]
    print "found", len(tiles), "aerogel tiles"
    lastPrimaryTile = -999
    for tile in tiles :

        # Load the conditions
        cond = tile.condition( "AerogelParameters" )

        # get the current value of n
        nzero = cond.paramAsDouble("CurrentAerogel_nAtFixedLambda")

        # Get the histogram to fit
        OK = False
        useSubTile = tile.subTile() and tile.primaryTileID() in fullCalibTiles
        if useSubTile :
            
            histID = rootHistDir + "subtiles/ckResAll-SubTile" + str(tile.tileID())
            print "Full SubTile Calibration for", tile.tileID()
            hist = file.Get(histID)
            if hist.GetEntries() < minEntries :
                print "  -> Stats too low. Try with full tile instead"
            else :
                OK = True
                    
        if not OK :
            
            histID = rootHistDir + "tiles/ckResAll-Tile" + str(tile.primaryTileID())
            print "Full Tile Calibration for", tile.tileID()
            hist = file.Get(histID)
                    
        print "  -> Using", histID
    
        # fit
        result = fit(hist)
        if useSubTile or lastPrimaryTile != tile.primaryTileID() : printCanvas()
        lastPrimaryTile = tile.primaryTileID()

        # Extract the shift
        shift = 0
        if result['OK'] : shift = result['shift']
        # Manual forced shift list
##        forcedList = { }
        forcedList = {
#             0  : 0.0,
#             1  : 0.0,
             4  : 0.0,
             7  : 0.0,
             8  : 0.0,
             12 : 0.0,
             15 : 0.0
             }
        if tile.primaryTileID() in forcedList.keys() :
            shift = forcedList[tile.primaryTileID()]
            print "   -> Forcing shift to", shift
        
        # New n
        N = newN(nzero,shift)
        
        # Update condition
        cond.addParam("CurrentAerogel_nAtFixedLambda",N)
        
        # Print
        print "   -> Shift =", shift, "oldN =", nzero, "newN =", N               

        # Write out the new condition
        newXML += cond.toXml() + '\n' + '\n' 

    # Finish the XML file 
    newXML += xmlFooter()

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

        searchX = [-0.005,0.005]

        # Get x value of highest content bin in the search range
        maxBin = 0
        max = 0
        for bin in range(1,hist.GetNbinsX()):
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

        hist.GetXaxis().SetRangeUser(-0.025,0.025)
        
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
    return 1.0 / cos( acos(1.0/oldN) + shift )
