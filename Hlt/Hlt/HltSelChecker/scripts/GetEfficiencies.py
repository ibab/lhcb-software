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
    if (debug): print "ENTRY ", signal, sel
    inTable = 0
    foundTable = 0
    Track = 0
    EqualL = 0
    Hlt2EffAlone = 0
    BestSelDict = {}
    epsilon = 0
    for line in f:
      doPrint = False
      if inTable==0:
        if re.search("Correlation Table",line):
          inTable = 1
          foundTable = 1
      else:
#        if (debug): print inTable, line
        if re.search("=======",line):
          EqualL+=1
          if (  EqualL==3 ):
            break
        else:  # in table
          if (re.search("L0",line)):
#            if (debug): print "L0 line:", line
            L0Eff = float((line.split()[2]).replace("%",""))
 #           if (debug): print "L0", L0Eff
          elif (re.search("Hlt1Global",line)): 
 #           if (debug): print line
            Hlt1Eff   = float((line.split()[2]).replace("%",""))
            Hlt1EffL0 = float((line.split()[4]).replace("%",""))
            Hlt1EffOn2 = float((line.split()[6]).replace("%",""))
 #           if (debug): print "HltDecision", HltEff, HltEffL0
          elif ( re.search("Hlt2Global",line) and ( Hlt2EffAlone == 0 )): 
 #           if (debug): print line
            Hlt2EffAlone = float((line.split()[4]).replace("%",""))
            Hlt2EffOn1   = float((line.split()[5]).replace("%",""))
            Trigger      = Hlt2EffAlone*Hlt1Eff/100
            HltEff       = 100.*Trigger/L0Eff
 #           if (debug): print "Hlt2EffAlone ", Hlt2EffAlone, Hlt2EffOn1, Trigger, HltEff
          elif (re.search("FilterTrueTracks",line)): 
 #           if (debug): print line
            Track = float((line.split()[4]).replace("%",""))
 #           if (debug): print "Tracking", Track
          elif ((re.search("Decision",line)) and (re.search("Hlt2",line))):
            epsilon = epsilon + 0.00001
            if (debug): print line
            sel =  (line.split()[1]).replace("Hlt2","").replace("Decision","")
            Hlt2Sel = float((line.split()[4]).replace("%",""))+epsilon
            if (debug): print sel, Hlt2Sel
            BestSelDict[sel] = Hlt2Sel # make sure they never get same efficiency
#            if (( line.split()[-1]).find('#')<0) :
#                SelEff = float((line.split()[-1]).replace("%",""))  # track
    if foundTable:
        s0,v0 = sortedDictValues(BestSelDict,0)
        s1,v1 = sortedDictValues(BestSelDict,1)
        s2,v2 = sortedDictValues(BestSelDict,2)
        s3,v3 = sortedDictValues(BestSelDict,3)
        print "| <nox>"+signal," | ",int(Trigger+0.5),"%  |  ",int(L0Eff+0.5),"% |   ",int(HltEff+0.5),"% |   ",int(Hlt1EffL0+0.5),"% |  ",int(Hlt2EffAlone+0.5),"% |   ",int(Hlt2EffOn1+0.5),"% |  ",int(Track+0.5),"% | <nox>"+s0+":",int(v0+0.5),"%  | <nox>"+s1+":",int(v1+0.5),"%  | <nox>"+s2+":",int(v2+0.5),"%  | <nox>"+s3+":",int(v3+0.5), "%  |"

def sortedDictValues(adict,item):
    array = sorted(adict.values())
#    print array
    val = array[-1-item]
#    print "looking for ", val
    for k, v in adict.iteritems():
        if (v==val): key=k
    return key,val
        
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
#
#
#
if ( mainhead == "<unset>" ):
    head = "Hlt12"
else :
    head = mainhead
files = sorted(glob.glob(head+'-*'))
print "| *Signal*  | *Trigger* | *L0* | *Hlt* | *Hlt1* | *Hlt2* | *Hlt2/1* | *Tk* | *Best Sel* | *2nd Sel* | *3rd Sel* | *4th Sel* |"    
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
print "All efficiencies after L0 and offline (except L0 of course). Null efficiencies are due to missing or incorrect Hlt2 selections. To know what is exactly done and what offline selection is used see =HltSelChecker=. "
print " "
