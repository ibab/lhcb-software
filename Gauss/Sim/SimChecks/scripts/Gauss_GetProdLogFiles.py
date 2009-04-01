#!/usr/bin/env python
#################################################################################
#  Given the prodIDs of the samples to compare, this script downloads the first 
#  non-empty Gauss.logs and GaussHistos.roots available and analyze them  using
#  SimulationReference.py 
#  Usage:
#  python Gauss_GetProdLogFiles.py <webpage> <REF_prodid> <CURR_prodid> 
#  Exemple: 
#  python Gauss_GetProdLogFiles.py http://lhcb-logs.cern.ch/storage/lhcb/MC/ 4497 3974
#################################################################################

import urllib, sys, os, re, array

# --- read arguments ---
url = "http://lhcb-logs.cern.ch/storage/lhcb/MC/"
Url=sys.argv[1]
refId= sys.argv[2]
curId= sys.argv[3]

print "refId", refId
print "curId", curId


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
	resultdir = re.search( '^<img .* href=\" *(\S+)\/\"' , line )
	dir = resultdir.group(1)
	return dir+"/"

# --- search and save .log file ---
def saveLogFile(line,baseUrl,jobId):
	global logDone
	if re.search( '.*href=\" *(\S+)\.log\"' , line ) or re.search( jobId, line ):
		resultFile = re.search( '.*href=\" *(\S+)\.log\"' , line )
		logName = resultFile.group(1)
		logUrl = baseUrl+logName+".log"
		logFile = open("gauss"+jobId+".log","w")
		print "\n>>> Download ", logUrl, "\n"
		logFile.write(getHTML(logUrl))
		logFile.close()
		logDone = True

# --- search and save .root file ---
def saveRootFile(line,baseUrl,jobId):
	global rootDone
	if re.search( '.*href=\" *(\S+)\.root\"' , line ):
		resultFile = re.search( '.*href=\" *(\S+)\.root\"' , line )
		rootName = resultFile.group(1)
		rootUrl = baseUrl+rootName+".root"
		rootFile = open("gauss"+jobId+".root","w")
		print "\n>>> Download ", rootUrl, "\n"
		rootFile.write(getHTML(rootUrl))
		rootFile.close()
		rootDone = True

# --- get .root and .log files from directory tree starting at url ---
def getFiles(url,depth2,jobId):
	global logDone, rootDone
	searchedUrl.insert(depth2,url)
	depth2 += 1
	htmlSource = getHTMLlines(url)
	for line in htmlSource:
		if logDone and rootDone: return
		if 'Gauss' in line:
			if not logDone: saveLogFile(line,searchedUrl[depth2-1],jobId)
			if not rootDone: saveRootFile(line,searchedUrl[depth2-1],jobId)
		elif re.search( '^<img .* href=\" *(\S+)\/\"' , line ):
			dir = getDir(line)
			url += dir
			searchedUrl.insert(depth2,searchedUrl[depth2-1]+dir)
			getFiles(searchedUrl[depth2],depth2,jobId)


# --- parse recursively page content ---
def parseHTML(url,depth,jobId):
	if rootDone and logDone: return
	newUrl.insert(depth,url)
	depth += 1
	if depth>3: return   # job URL cannot be more than 3 directories deep w.r.t. base URL
	htmlSource = getHTMLlines(url)
	# --- loop on url lines ---
	for line in htmlSource:
		#print line
		if jobId in line:
			#print line
			finalDir = getDir(line)
			finalUrl = newUrl[depth-1] + finalDir
			print ">>> FINAL URL -> ", finalUrl
			getFiles(finalUrl,depth2,jobId)
			break
		if re.search( '^<img .* href=\" *(\S+)\/\"' , line ):
			dir = getDir(line)
			url += dir
			newUrl.insert(depth,newUrl[depth-1]+dir)
			parseHTML(newUrl[depth],depth,jobId)

# --------------------
# --- main program ---
# --------------------

global logDone, rootDone
# reference files 
logDone = False
rootDone = False

depth=0
newUrl=[]
depth2=0
searchedUrl=[]

parseHTML(url,depth,refId)

# current files
logDone = False
rootDone = False

depth=0
newUrl=[]
depth2=0
searchedUrl=[]

parseHTML(url,depth,curId)

# execute monitoring script command:


cmd = 'python Gauss_SimRefPlots.py -r ./gauss'+refId+'.root -l  ./gauss'+refId+'.log -s ./gauss'+curId+'.root -f  ./gauss'+curId+'.log -v -i -a -k D'

os.system(cmd)

