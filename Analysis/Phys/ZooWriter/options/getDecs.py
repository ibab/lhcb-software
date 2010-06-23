#!/bin/python

logfile = open("test_new.log")

#for line in logfile:
#  if line.find("List of available Hlt1Lines") != -1:
#    line = line.replace("# List of available Hlt1Lines : ['","",1)
#
#    line = line.replace("']","",1)
#    decisions = line.split("', '")
#sorting of the decisions
   
#muondecisions = []
#L0decisions = []
#for decision in decisions[:]:
#    if decision.find("L0") != -1:
#      L0decisions.append(decision)
#      index = decisions.index(decision)
#      del decisions[index]
#    elif decision.find("Mu") != -1:
#      muondecisions.append(decision)
#      index = decisions.index(decision)
#      del decisions[index]
##readd L0 and Muon decs
#for l0dec in L0decisions:
#  decisions.append(l0dec)
#for muondec in muondecisions:
#  decisions.append(muondec)
#decisions.sort()
#counter = 0
#wordcounter = 1
#print "//Trigger Word %2i\n" % wordcounter
#for decision in decisions:
#    print "MakeTriggerDec(%40s,   %2i,   %10s,   \"%5s\")" % (decision, wordcounter , hex(2**counter), decision)
#    counter+=1
#    if counter == 32: 
#      counter = 0
#      wordcounter+=1
#      print "//Trigger Word %2i\n" % wordcounter

for line in logfile:
  if line.find("List of available Hlt2Lines") != -1:
    line = line.replace("# List of available Hlt2Lines : ['","",1)

    line = line.replace("']","",1)
    decisions = line.split("', '")
#sorting of the decisions
   
topodecisions = []
charmdecisions = []
jpsidecisions = []

for decision in decisions[:]:
    if decision.find("Topo") != -1:
      print decision
      topodecisions.append(decision)
      index = decisions.index(decision)
      del decisions[index]
    elif (decision.find("Charm") != -1) or (decision.find("Dst") != -1):
      charmdecisions.append(decision)
      index = decisions.index(decision)
      del decisions[index]
##readd L0 and Muon decs
#for l0dec in L0decisions:
#  decisions.append(l0dec)
#for muondec in muondecisions:
#  decisions.append(muondec)
decisions.sort()

print "\n//------------ Topo -------------------\n" 
for decision in topodecisions:
    print "MakeTriggerDec(%40s,   %2i,   %10s,   \"%5s\")" % (decision, 0, 0x00000000, decision)
print "\n//------------ Charm -------------------\n" 
for decision in charmdecisions:
    print "MakeTriggerDec(%40s,   %2i,   %10s,   \"%5s\")" % (decision, 0, 0x00000000, decision)
print "\n//------------ All the rest -------------------\n" 
for decision in decisions:
    print "MakeTriggerDec(%40s,   %2i,   %10s,   \"%5s\")" % (decision, 0, 0x00000000, decision)

