imageFileName = ''
canvas        = None

def makeRatios(fileList,referenceFile):

    from ROOT import gROOT, TFile, TH1, TProfile, gStyle

    # No info messages
    gROOT.ProcessLine("gErrorIgnoreLevel = kWarning;")
    # Batch mode (no TCanvas)
    gROOT.SetBatch(True)

    # Get list of file names from text file
    files = rootFileListFromTextFile(fileList)

    # Make summed reference plot
    print "Getting reference"
    t = TFile(referenceFile)
    refOccs = t.Get('RICH/RichRawDataSize/hpds/SizeVHPDCopyNumber')
    
    # Loop over the ROOT files, open them, and make an average occ plot
    first = True
    for file in files :
        tfile = TFile(file)
        occs = tfile.Get('RICH/RichRawDataSize/hpds/SizeVHPDCopyNumber')
        if occs != None :
            refOccs.Add(occs)

    # Set output PDF name
    globals()["imageFileName"] = "HPD-Occs.pdf"
    printCanvas('[')

    print "Making ratio plots"
    for file in files :
        tfile = TFile(file)
        run = getIntInfo(file,'Run')

        occs = tfile.Get('RICH/RichRawDataSize/hpds/SizeVHPDCopyNumber')

        occs.SetTitle( 'Run ' + str(run) )
        occs.GetXaxis().SetTitle('HPD Copy Number')
        occs.GetYaxis().SetTitle('HPD Av Occ / Reference')
        
        occs.Divide(refOccs)

        occs.SetMaximum(3)
        occs.SetMinimum(0)
        
        #occs.GetXaxis().SetRangeUser(0,195)   # RICH1
        #occs.GetXaxis().SetRangeUser(196,483) # RICH2

        occs.Draw('p HIST')
        printCanvas()
    
    printCanvas(']')


def rootFileListFromTextFile(rootFileList):

    # Open the text file
    files = open(rootFileList,'r')

    # list of root files
    rootfiles = [ ]

    # Read file list from text file
    for file in files.read().split('\n') :
        if len(file) > 0 :
            rootfiles += [file]
    
    # Close the file
    files.close()

    # Return the file list
    return rootfiles

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

def rootCanvas():
    from ROOT import TCanvas
    if globals()["canvas"] == None :
        import rootStyle
        rootStyle.applyRootStyle()
        globals()["canvas"] = TCanvas("CKCanvas","CKCanvas",1050,750)
        globals()["canvas"].SetGrid()
    return globals()["canvas"]

# Get the run number from file name (ugly, but works ...)
def getIntInfo(filename,type):
    run = 0
    split = ''
    splits = filename.split('_')
    for split in splits:
        s = split.split('-')
        if s[0] == type : run = s[1]
    return int(run.split('.')[0])
