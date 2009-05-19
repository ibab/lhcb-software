#!/usr/bin/python
#####
# get efficiencies
#
###
import sys, os, re, glob
###############################################################################
def getStats(f,signal,sel,wiki):
###############################################################################
    """ returns the stats
    by Patrick Koppenburg"""
#    print "ENTRY ", signal, sel
    inTable = 0
    SelEff = 0
    Hlt2Sel = 0
    foundTable = 0
    Track = 0
    EqualL = 0
    Hlt2EffAlone = 0 
    for line in f:
      if inTable==0:
        if re.search("Correlation Table",line):
          inTable = 1
          foundTable = 1
      else:
#        print inTable, line
        if re.search("=======",line):
          EqualL+=1
          if (  EqualL==3 ):
            break
        else:  # in table
          if re.search("L0",line):
#            print "L0 line:", line
            L0Eff = float((line.split()[2]).replace("%",""))
 #           print "L0", L0Eff
          if re.search("Hlt1Global",line): 
 #           print line
            Hlt1Eff   = float((line.split()[2]).replace("%",""))
            Hlt1EffL0 = float((line.split()[4]).replace("%",""))
            Hlt1EffOn2 = float((line.split()[6]).replace("%",""))
 #           print "HltDecision", HltEff, HltEffL0
          if ( re.search("Hlt2Global",line) and ( Hlt2EffAlone == 0 )): 
 #           print line
            Hlt2EffAlone = float((line.split()[4]).replace("%",""))
            Hlt2EffOn1   = float((line.split()[5]).replace("%",""))
            Trigger      = Hlt2EffAlone*Hlt1Eff/100
            HltEff       = 100.*Trigger/L0Eff
 #           print "Hlt2EffAlone ", Hlt2EffAlone, Hlt2EffOn1, Trigger, HltEff
          if re.search("Hlt2"+sel+"Decision",line): 
 #           print line
            Hlt2Sel = float((line.split()[4]).replace("%",""))
            if (( line.split()[-1]).find('#')<0) :
                SelEff = float((line.split()[-1]).replace("%",""))
            else: SelEff = 0
 #           print "Hlt2Sel"+sel, Hlt2Sel, SelEff
          if re.search("FilterTrueTracks",line): 
 #           print line
            Track = float((line.split()[4]).replace("%",""))
 #           print "Tracking", Track
    if foundTable == 1:
        if ( wiki ):
 #           print Hlt2EffAlone, int(Hlt2EffAlone+0.5)
            print "| <nox>"+signal+"  | <nox>Hlt2"+sel+"  |  ",int(L0Eff+0.5),"% |  ",int(Track+0.5),"% |  ",int(SelEff+0.5),"% |   ",int(Hlt2Sel+0.5),"% |   ",int(Hlt2EffAlone+0.5),"% |   ",int(Hlt2EffOn1+0.5),"% |   ",int(Hlt1EffL0+0.5),"% |   ",int(HltEff+0.5),"% |   ",int(Trigger+0.5),"% | "
        else :
            print signal.replace("_","\_"), "&", sel.replace("_","\_"), "&", int(L0Eff+0.5), "\\% &", int(Track+0.5), "\\% &", int(SelEff+0.5), "\\% & ", int(Hlt2Sel+0.5), "\\% & ", int(Hlt2EffAlone+0.5), "\\% & ", int(Hlt2EffOn1+0.5), "\\% & ", int(Hlt1EffL0+0.5), "\\% & ", int(HltEff+0.5), "\\% & ", int(Trigger+0.5), "\\% \\\\"

###############################################################################
# Main program
###############################################################################
#
# Arguments
#
mainhead = "<unset>"
wiki = True
if ( len(sys.argv) == 3 ):
    if ( sys.argv[1] == "--tex" ):
        wiki = False
        mainhead = sys.argv[2]
    elif ( sys.argv[2] == "--tex" ):
        wiki = False
        mainhead = sys.argv[2]
if ( len(sys.argv) == 2 ):
    if ( sys.argv[1] == "--tex" ):
        wiki = False
    else :
        mainhead = sys.argv[1]
#
#
#
if ( mainhead == "<unset>" ):
    head = "Hlt12"
else :
    head = mainhead
files = sorted(glob.glob(head+'-*'))
if ( wiki ):
    print "| *Signal*  | *Sel*  | *L0* | *Tk* | *Sel/Tk* | *Sel* | *Hlt2* | *Hlt2/1* | *Hlt1* | *Hlt* | *Trigger* |"    
else :
    print "\\begin{slide}{Signal efficiencies of HLT 2 after HLT 1}"
    print "\\topleft{\\scalebox{0.5}{\\begin{tabular}{ll|CC|CCC|C|C||| LLLLLL}"
    print "Signal & Sel & \\text{L0} & \\text{Tk} & \\text{Sel}|\\text{Tk} & \\text{Sel} & \\text{Hlt2} & \\text{Hlt2}\\left|\\right.\\text{1} & \\text{Hlt1} & \\text{Hlt} && \\text{Trigger} \\\\"
    print "\\hline"
for f in files :
    s = f.split('-')[1]
    #        print f, s
    h = s
    if ( s=="B2JpsiX_MuMu"): h = "UnbiasedDiMuon"
    if ( s=="B2MuMu"): h = "UnbiasedDiMuon"
    if ( s=="Bu2eeK"): h = "Bu2LLKSignal"
    if ( s=="Bu2MuMuK"): h = "Bu2LLKSignal"
    if ( s=="Bs2DsH"): h = "TopoTF4BodyReq4Yes"
    if ( s=="Bs2JpsiPhi"): h = "Bs2JpsiPhiSignal"
    if ( s=="Bd2JpsiMuMuKsUnbiased"): h = "UnbiasedJPsi"
    if ( s=="Bd2JpsiMuMuKsBiased"): h = "UnbiasedJPsi"
    if ( s=="Bs2DsDs"): h = "TopoTF4BodyReq4Yes"
    if ( s=="Bs2JpsieePhi"): h = "TopoTF4BodyReq4Yes"
    if ( s=="Bs2KstarKstar"): h = "TopoTF4BodyReq4Yes"
    if ( s=="HidValley"): h = "B2MuTrackNoPT"
    if ( s=="Z0"): h = "Z02MuMu"
    
    f  = open(f, 'r')
    getStats(f,s,h,wiki)
if ( wiki ):
    print "   $ *Signal* : Signal sample as described in =Hlt/HltSelChecker= options.  "
    print "   $ *Sel* : Selection used as ``signal'' selection. See =Hlt/HltSelections= options. "
    print "   $ *L0* : L0 efficiency."
    print "   $ *Tk* : Hlt2 tracking efficiency."
    print "   $ *Sel/Tk* : Efficiency of appropriate Hlt2 selection given tracks are found."
    print "   $ *Sel* : Hlt2 selection efficiency efficiency, product of two above."
    print "   $ *Hlt2* : Overall Hlt2 efficiency including all selections."
    print "   $ *Hlt1/Hlt1* : Hlt2 efficiency given Hlt1."
    print "   $ *Hlt1* : Hlt1 efficiency."
    print "   $ *Hlt1/Hlt2* : Hlt2 efficiency given Hlt2."
    print "   $ *Hlt* : Overall Hlt efficiency after L0."
    print "   $ *Trigger* : Overall Trigger efficiency."
    print " "
    print "All efficiencies after L0 and offline (except L0 of course). Null efficiencies are due to missing or incorrect Hlt2 selections. To know what is exactly done and what offline selection is used see =HltSelChecker=. "
    print " "
else:
    print "\\end{tabular}}}"
    print "\\skipit{"
    print "\\topright{\\smallpage{0.25}{\\begin{description}"
    print "\\item[L0:] L0 eff."
    print "\\item[Tk:] Total track eff (for L0 yes)."
    print "\\item[Sel:] Eff. of dedicated Hlt2 selection."
    print "\\item[Hlt2:] Hlt2 Eff."
    print "\\item[Hlt1:] Hlt1 eff."
    print "\\item[Trigger:] L0$\\times$ Hlt1$\\times$ Hlt2"
    print "\\end{description}"
    print "}}"
    print "}% skipit"
    print "\\end{slide}"
    print "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
