#!/usr/bin/env python
#################################################################################
#  Fill plot of "dE/dx MPV vs betagamma" running over the summary page of 
#  the simulation monitoring:
#  http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/gauss/simulation/
#  validation/index.html
#  User boundaries (mcSetIni, mcSetEnd) need to be specified to choose the proper
#  sample
#################################################################################

import urllib, sys, os, re
from ROOT import gROOT, gPad, gStyle, TCanvas, TF1, TFile, TH1F, TH2F, TArrayF, TStopwatch, TAttMarker


# --- read arguments ---
url = "http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/gauss/simulation/validation/index.html"
if url is None:
	url=sys.argv[1]
mcSetIni = "PGUN: mu+mu- (LHEP+EMopt1+DRays OFF+KineHT)"
mcSetEnd = "PGUN: mu+mu- (LHEP+EMopt1+DRays ON+KineHT)"
if mcSetIni is None:
	mcSetIni= sys.argv[2]


# --- get url content (as a single string) ---
def getHTML(url):
	print ">>> URL -> ", url
	usock = urllib.urlopen(url)
	htmlSource = usock.read()  
	usock.close()
	return htmlSource

# --- get url content (line by line) ---
def getHTMLlines(url):
	print ">>> URL -> ", url
	usock = urllib.urlopen(url)
	htmlSource = usock.readlines()  
	usock.close()
	return htmlSource

# --- extract directory name from HTML line ---
def getDir(line):
	resultdir = re.search( '^<A href=\" *(\S+)\"' , line )
	dir = resultdir.group(1)
	return dir

def getValue(url):
        global meanFound
        htmlSource = getHTMLlines(url)
        # --- loop on url lines ---
        for line in htmlSource:
                if meanFound:
                        if not re.search("^<", line):
                                return line
                elif re.search( "IT Most Probable deposited charge", line ):
                                meanFound = True

def getValue2(url):
        global begaFound
        htmlSource = getHTMLlines(url)
        # --- loop on url lines ---
        for line in htmlSource:
                if begaFound:
                        if not re.search("^<", line):
                                return line
                elif re.search( "IT Mean beta \* gamma", line ):
                                begaFound = True


	
# --- parse recursively page content ---
def parseHTML(url):
	global mcSetIniFound, meanFound, begaFound,  impv
	htmlSource = getHTMLlines(url)
	# --- loop on url lines ---
	for line in htmlSource:
		if not mcSetIniFound:
			if mcSetIni in line: 
				mcSetIniFound = True
		elif mcSetEnd in line: return
		elif re.search( '^<A href=\" *(\S+)\"' , line ):
			#print line
			dir = getDir(line)
			newUrl = url
			newUrl = re.sub('index.html', dir, newUrl)
			#print newUrl
			mostprobv = getValue(newUrl)
			meanFound = False
			betagamma = getValue2(newUrl)
			begaFound = False

			print mostprobv
			print betagamma
			impv+=1
			mpv.insert(impv,mostprobv)
			bega.insert(impv,betagamma)
			continue

# --------------------
# --- main program ---
# --------------------

meanFound = False
begaFound = False

mcSetIniFound = False

depth=0
newUrl=[]
depth2=0
searchedUrl=[]

impv=0
mpv=[]
bega=[]


parseHTML(url)
print mpv
print bega

# fill histogram

gROOT.Reset()
gStyle.SetOptLogx();
c1 = TCanvas("c1", "Plot", 1024, 768)
c1.SetGridx()
c1.SetGridy()

h2 = TH2F ("h2","histo2", 2000, 0, 2000, 1000, 0., 500.)
n2xBins = h2.GetNbinsX();
n2yBins = h2.GetNbinsY();
for i in range(12):
	h2.Fill(float(bega[i-1]),float(mpv[i-1])*1000.)

gStyle.SetMarkerSize(6.); 
h2.SetMarkerStyle(23);
h2.GetXaxis().SetTitle("#beta#gamma");
h2.GetYaxis().SetTitle("MPV/keV");

h2.Draw("")
gPad.Update()
c1.Print("muons_IT_drays_off_ht.eps")

outfname = "muons_IT_drays_off_ht.root"
fout = TFile(outfname,"NEW")
h2.Write()
fout.Close() 
