#!/usr/bin/python
#####
# get efficiencies
#
###
import sys, os, re, glob
###############################################################################
def getStats(f,signal):
###############################################################################
    """ returns the stats
    by Patrick Koppenburg"""
    debug = False
    if (debug): print "ENTRY ", signal
    foundTable = 0
    BestSelDict = {}
    epsilon = 0
    L0Eff = 0.
    Hlt1EffL0 = 0.
    Hlt2EffOn1 = 0.
    Trigger = 0.
    EqualL = 0
    foundTable = False
    numL0 = 0.
    numHlt1 = 0.
    for line in f:
        if (re.search("11:L0",line)):
          if (debug): print "L0 line:", line
          numL0 = float(line.split('|')[5].rstrip().lstrip())
          L0Eff = 100.*float(line.split('|')[6].rstrip().lstrip())
          if (debug): print "L0", L0Eff
        elif (re.search("46:HLT",line)): # Hlt1-Physics
          if (debug): print line
          numHlt1 = float(line.split('|')[3].rstrip().lstrip())  
          Hlt1EffL0 = 100.*numHlt1/numL0
          if (debug): print "Hlt1 Physics", Hlt1EffL0
        elif ( re.search("77:HLT",line)): # Hlt2-Physics
          if (debug): print line
          Hlt2EffOn1   = 100.*float(line.split('|')[3].rstrip().lstrip())/numHlt1
          Trigger      = 100.*float(line.split('|')[3].rstrip().lstrip())/float(line.split('|')[2].rstrip().lstrip())
          if (debug): print "Hlt2 Physics", Hlt2EffOn1
          if (debug): print "Trigger efficiency", Trigger
        if not re.search("Correlation Table",line) and not foundTable: continue
        foundTable = True
        if re.search("=======",line):
          EqualL+=1
          if (  EqualL==3 ): break  
        elif ((re.search("Decision",line)) and (re.search("Hlt2",line)) and not (re.search("PassThrough",line))):
          epsilon = epsilon + 0.00001
          if (debug): print line
          sel =  (line.split()[1]).replace("Hlt2","").replace("Decision","")
          Hlt2Sel = float((line.split()[5]).replace("%",""))+epsilon   # on Hlt1
          if (debug): print sel, Hlt2Sel
          BestSelDict[sel] = Hlt2Sel # make sure they never get same efficiency
    if foundTable :
        s0,v0,tos0,tis0 = sortedDictValues(BestSelDict,0)
        s1,v1,tos1,tis1 = sortedDictValues(BestSelDict,1)
        print "| <nox>"+signal," | ",int(Trigger+0.5),"%  |  ",int(L0Eff+0.5),"% |  ",int(Hlt1EffL0+0.5),"% |  ",int(Hlt2EffOn1+0.5),"% | <nox>"+s0+":",int(v0+0.5),"%/",int(tos0+0.5),"%/",int(tis0+0.5),"%  | <nox>"+s1+":",int(v1+0.5),"%/",int(tos1+0.5),"%/",int(tis1+0.5),"%  |"

def sortedDictValues(adict,item):
    array = sorted(adict.values())
#    print array
    val = array[-1-item]
    tos = -100
    tis = -100
#    print "looking for ", val
    for k, v in adict.iteritems():
        if (v==val): key=k
    return key,val,tos,tis
        
###############################################################################
# Main program
###############################################################################
#
# Arguments
#
mainhead = "<unset>"
wiki = True
if ( len(sys.argv) == 2 ):
    mainhead = sys.argv[1]
    head = mainhead
else :
    raise Exception("Usage: ''python GetEfficiencies.py <head>'' will run on all files called <head>-* (note the dash)")

files = sorted(glob.glob(head+'-*'))
print "\n\n---++ Signal efficiencies\n"
print "| *Signal*  | *Trigger* | *L0* | *Hlt* | *Hlt1* | *Hlt2* | *Hlt2/1* | *Tk* | *Best Sel (All/TOS/TIS)* | *2nd Sel (All/TOS/TIS)* |"    
for f in files :
    s = f.split('-')[1]
    #        print f, s
    
    f  = open(f, 'r')
    getStats(f,s)
    
print "   $ *Signal* : Signal sample as described in =Hlt/HltSelChecker= options.  "
print "   $ *Trigger* : Overall Trigger efficiency."
print "   $ *L0* : L0 efficiency."
print "   $ *Hlt* : Hlt efficiency on L0 events."
print "   $ *Hlt1* : Hlt1 efficiency on L0 events."
print "   $ *Hlt2* : Overall Hlt2 efficiency including all selections."
print "   $ *Hlt2/Hlt1* : Hlt2 efficiency given Hlt1."
print "   $ *Tk* : Hlt2 tracking efficiency on all tracks."
print "   $ *Sel* : Name and efficiency  (on Hlt1 accepted events) of 4 best Hlt2 selections."
print " "
print "All efficiencies after L0 and offline (except L0 of course). Null efficiencies are due to missing or incorrect Hlt2 selections. To know what is exactly done and what data is used see =HltSelChecker=. "
print " "
