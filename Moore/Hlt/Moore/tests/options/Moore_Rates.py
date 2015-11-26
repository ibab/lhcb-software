#!/usr/bin/env python
# Script to test the rates of a HLT configuration
# Mika Vesterinen

import os, sys, subprocess, re, optparse, math

# Configuration
import Gaudi.Configuration
from Gaudi.Configuration import *
from Configurables import GaudiSequencer as Sequence
from Configurables import EventSelector, HltConf
from Moore.Configuration import Moore

# GaudiPython
from GaudiPython import AppMgr

def getrate(scale,numer,denom):
    eff = float(numer)/float(denom)
    err = math.sqrt((eff-eff**2)/denom)
    return [scale*eff,scale*err]

def table_row(iLine,name,incl,excl):
    print ('|%s|%s\t' %(iLine,name)).expandtabs(60),
    print ('|%s\t|%s\t|' %(incl,excl)).expandtabs(20)

### this was copied from the independence tests
### I hoped that I could switch off some lines like this
### but it doesn't work 
### so I found another hackier solution...
def setLines(desiredlines1,desiredlines2):
    from HltLine.HltLine import hlt1Lines
    lines1 = [ i for i in hlt1Lines() if  i.name() in desiredlines1 ]
    Sequence( "Hlt1" ).Members = [ i.configurable() for i in lines1 ]
    from HltLine.HltLine import hlt2Lines
    lines2 = [ i for i in hlt2Lines() if  i.name() in desiredlines2 ]
    Sequence( "Hlt2" ).Members = [ i.configurable() for i in lines2 ]
    from HltConf.HltMonitoring import HltMonitoringConf
    HltMonitoringConf().configureHltMonitoring(lines1, lines2)
    Sequence( "HltEndSequence" ).Members = []        


def main():
    
    parser = optparse.OptionParser( usage = "usage: %prog [options]" )

    parser.add_option( "--debug", action="store_true", dest="debug",
                       default=False, help="Debug?")
    
    parser.add_option( "-d", "--datatype", action="store", dest="DataType", 
                       default="2015", help="DataType to run on.")
    
    parser.add_option( "-n", "--evtmax", type="int", action = "store", dest = "EvtMax",
                       default = 10000, help = "Number of events to run" )

    parser.add_option( "--dddbtag", action="store", dest="DDDBtag",
                       default='dddb-20150526', help="DDDBTag to use" )

    parser.add_option( "--conddbtag", action = "store", dest = "CondDBtag",
                       default = 'cond-20150617', help = "CondDBtag to use" )

    parser.add_option( "--settings", action = "store", dest="ThresholdSettings",
                       default = 'Physics_25ns_September2015',
                       help = "ThresholdSettings to use")

    parser.add_option( "--TCK", action = "store", dest="TCK",
                       default = '',
                       help = "HLT TCK. If unspecified,then run from settings")
    
    parser.add_option( "--simulation", action = "store_true", dest = "Simulation",
                       default = False, help = "Run on simulated data")

    parser.add_option( "--input_rate",action="store",dest="input_rate",
                       default=1.e6,help="Input rate from L0 in Hz")

    parser.add_option( "--tuplefile",action="store",dest="tuplefile",
                       default="",help="Output root file")

    parser.add_option( "--inputdata",action="store",dest="inputdata",
                       default="Physics1600TestNode",help="Name of inputdata")

    # Parse the arguments
    (options, args) = parser.parse_args()

    #### configured from the arguments
    input_rate = options.input_rate
    Moore().EvtMax = options.EvtMax
    Moore().DDDBtag   = options.DDDBtag
    Moore().CondDBtag = options.CondDBtag
    Moore().Simulation = options.Simulation
    Moore().DataType   = options.DataType
    if options.TCK != "":
        Moore().UseTCK = True
        Moore().InitialTCK = options.TCK
    else:
        Moore().ThresholdSettings = options.ThresholdSettings
        Moore().UseTCK = False
    
    #### hard coded here
    Moore().ForceSingleL0Configuration = False
    Moore().OutputLevel = 6
    Moore().RemoveInputHltRawBanks = True
    Moore().Simulation = False
    Moore().Split=""
    Moore().CheckOdin = False
    from Configurables import CondDB
    CondDB().IgnoreHeartBeat = True                                                                                  
    CondDB().EnableRunChangeHandler = True                                                                    
    EventSelector().PrintFreq = 100

    #### configure raw data
    importOptions('$STDOPTS/DecodeRawEvent.py')
    EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
    from Configurables import  DataOnDemandSvc
    DataOnDemandSvc().AlgMap['Hlt/DecReports'] =  "HltDecReportsDecoder"
    
    from GaudiConf import IOHelper
    if options.inputdata == "Physics1600":
        IOHelper("MDF").inputFiles(["mdf:root://eoslhcb.cern.ch//eos/lhcb/wg/HLT/BWdivData/Run164440_L0Filtered_0x00A2_Mika/2015NB_25ns_0x00A2_0.mdf"])
    elif options.inputdata == "Physics1600TestNode":
        IOHelper("MDF").inputFiles(["/localdisk/bw_division/run164440_L0Filtered_0x00A2_Mika/2015NB_25ns_0x00A2_0.mdf",
                                    "/localdisk/bw_division/run164440_L0Filtered_0x00A2_Mika/2015NB_25ns_0x00A2_1.mdf",
                                    "/localdisk/bw_division/run164440_L0Filtered_0x00A2_Mika/2015NB_25ns_0x00A2_2.mdf",
                                    "/localdisk/bw_division/run164440_L0Filtered_0x00A2_Mika/2015NB_25ns_0x00A2_3.mdf",
                                    "/localdisk/bw_division/run164440_L0Filtered_0x00A2_Mika/2015NB_25ns_0x00A2_4.mdf"])
        
        
    ### getting ready for the event loop
    gaudi = AppMgr(outputlevel=4)
    gaudi.ExtSvc += ['ToolSvc']                                                                               
    gaudi.ExtSvc.append( 'DataOnDemandSvc' )    
    gaudi.initialize()

    ### get the list of active line names
    Lines = {}
    for level in ["Hlt1","Hlt2"]:
        Lines[level] = set()
        for m in Sequence(level).Members:
            Lines[level].add(m.name())

    ### remove certain lines from the accounting
    remove = set(["Hlt1MBNoBias",
                  "Hlt2Transparent",
                  "Hlt1Global",
                  "Hlt2Global"])
                  
    Lines["Hlt1"] -= remove
    Lines["Hlt2"] -= remove
    ### this is what I would prefer to do...
    #appendPostConfigAction( setLines(Lines["Hlt1"],Lines["Hlt2"]))
    
    ### option to create a tuple with all of the decisions
    if options.tuplefile != "":
        from ROOT import (TTree, TFile)
        from array import array
        TF = TFile(options.tuplefile,"RECREATE")
        DecMaps = {} ## dicto of branches for the TTrees
        DecTrees = {} ## dicto of TTrees
        for level in ["Hlt1","Hlt2"]:
            DecTrees[level] = TTree('Tuple%s'%level,'Tuple%s'%level)
            DecMaps[level] = {}
            for l in Lines[level]:
                DecMaps[level][l] = array( 'i', [ 0 ] )
                DecTrees[level].Branch(l,DecMaps[level][l], '%sDecision/I'%l)
    
    ### this will be dictionary of lines and their counters for the rates
    line_stats = {}
    for line in Lines["Hlt1"].union(Lines["Hlt2"]).union(set(["Hlt1Global","Hlt2Global"])):
        line_stats[line] = {"passed_incl":0,
                            "passed_excl":0}
    
    ### counters for various regex
    stream_stats = {"Turbo":{"filter":"Hlt2.(?!.*?TurboCalib).*Turbo"},
                    "Turcal":{"filter":"Hlt2.*TurboCalib"},
                    "Full":{"filter":"Hlt2.(?!.*?Turbo).(?!.*?TurboCalib)"}}
    for k,v in stream_stats.iteritems():
        v["pass_this_event"] = False
        v["passed"] = 0

    print '*'*100
    print Lines
    print line_stats
    print stream_stats
    print '*'*100
    
    
    i = 0
    processed = 0
    #### start of the event loop
    while i < Moore().EvtMax:
        i+=1
        # run the sequences on this event
        gaudi.run(1)
        processed +=1
        if not gaudi.evtsvc()['Hlt1/DecReports']: break
            
        ### reset the stream counters
        for s in stream_stats.keys():
            stream_stats[s]["pass_this_event"] = False
        
        ### this is a nasty hack since I can't seem to switch
        ### off some of the Hlt1 lines with a post config action
        ### so I have to do my own book-keeping to see if one
        ### of the good hlt1 lines fired.
        ### I only consider events in Hlt2 if they pass my subset
        ### of good hlt1 lines.
        PassMyGlobal = {"Hlt1":False,
                        "Hlt2":False}
        
        # loop over levels
        for level in ["Hlt1","Hlt2"]:
            if level == "Hlt2" and not PassMyGlobal["Hlt1"]: continue
                
            # do the dec reports exist?
            # for Hlt2 this check does the job of telling us whether the event passed Hlt1
            if gaudi.evtsvc()['%s/DecReports'%level]:
                # get the dec reports
                reps =  gaudi.evtsvc()['%s/DecReports'%level]
                # get the list of lines from the reports
                #lines = reps.decReports().keys()
                # counter for how many hlt lines fired in this event
                nPassed = 0
                # loop over all lines
                for line in Lines[level]:
                    # protection. why is this needed though?
                    if not line+"Decision" in reps.decReports().keys(): 
                        print '%s not in %s' %(line,reps.decReports().keys())
                        continue
                    
                    # just check this once
                    LINE_FIRED = reps.decReport(line+"Decision").decision()
                    
                    # my global counter
                    if LINE_FIRED: PassMyGlobal[level] = True
                        
                    # does this event fire any lines that match my "streams"?
                    if LINE_FIRED and level == "Hlt2" and not line == "Hlt2Global": # and not line in remove:
                        for s in stream_stats.keys():
                            if re.match(stream_stats[s]["filter"], line, flags=0):
                                stream_stats[s]["pass_this_event"] = True
                    
                    # set the variable to be stored in the tuple
                    if options.tuplefile != "":
                        if LINE_FIRED:
                            DecMaps[level][line][0] = 1
                        else:
                            DecMaps[level][line][0] = 0

                    # if this is the first fired event then 
                    # need to initialise the dictionary entry
                    #if not line in line_stats.keys():
                    #    line_stats[line] = {"passed_incl":0,
                    #                        "passed_excl":0}
                    # increment the counter for this line
                    if LINE_FIRED:
                        line_stats[line]["passed_incl"]  += 1
                        if not "Global" in line:
                            nPassed +=1 ### for the exclusives

                # my own global counter
                if PassMyGlobal[level]:
                    line_stats["%sGlobal"%level]["passed_incl"] += 1
                # now go back and count the number of exclusive fires of this line
                # just need to ignore HltXGlobal
                for line in Lines[level]:
                    if not line+"Decision" in reps.decReports().keys(): continue # protection
                    if reps.decReport(line+"Decision").decision() and nPassed == 1:
                        if not "Global" in line:
                            line_stats[line]["passed_excl"]  += 1
                
                # fill the tree
                if options.tuplefile != "":
                    DecTrees[level].Fill()

                # stream accounting
                for s in stream_stats.keys():
                    if stream_stats[s]["pass_this_event"] == True:
                        stream_stats[s]["passed"] +=1
                    
                
    # write the root file
    if options.tuplefile != "":
        TF.Write()

    # I was hoping that this would dump all of the gaudi related 
    # printout before I print the summary that I'm interested in.
    # Apparently this doesn't work, but it's only really a cosmetic thing.
    #gaudi.finalize()

    sys.stdout.flush()    
    #############################################
    ###### print the summary tables #############
    #############################################
    for k,v in line_stats.iteritems():
        v["processed"] = processed
    for k,v in stream_stats.iteritems():
        v["processed"] = processed
    
        
    
    GlobalRates = {}
    print '-'*100
    print 'HLT rates summary starts here'
    print '-'*100
    #### print the global rates
    print 'removed lines: %s' %remove
    print 'processed: %s' %processed
    print '%s Hlt1Lines' %(len(Lines['Hlt1']))
    print '%s Hlt2Lines' %(len(Lines['Hlt2']))
    for level in ['Hlt1','Hlt2']:
        rate = getrate(1.e-3*input_rate,line_stats["%sGlobal"%level]["passed_incl"],line_stats["%sGlobal"%level]["processed"])
        print '%sGlobal rate = (%s+-%s)kHz' %(level,rate[0],rate[1])
    ### print the stream rates
    for k,v in stream_stats.iteritems():
        print '%s rate = %s kHz' %(k,v["passed"]/float(v["processed"]))
    #### order by inclusive rate
    for level in ['Hlt1','Hlt2']:
        iLine = -1 ## counter for line
        table_row("**","*Line*","*Incl.*","*Excl.*")
        OrderedStats = {}
        for key, value in line_stats.iteritems():
            if level in key:
                OrderedStats[key] = value["passed_incl"]
        for line_name, rate in sorted(OrderedStats.iteritems(), key=lambda (v,k): (k,v),reverse=True):
            iLine += 1
            rate_incl = getrate(1.e-3*input_rate,line_stats[line_name]["passed_incl"],line_stats[line_name]["processed"])
            rate_excl = getrate(1.e-3*input_rate,line_stats[line_name]["passed_excl"],line_stats[line_name]["processed"])
            if "Global" in line_name:
                GlobalRates[level] = rate_incl
            else:
                table_row(iLine,line_name.replace("Decision",""),
                          "%.2f+-%.2f" %(rate_incl[0],rate_incl[1]),
                          "%.2f+-%.2f" %(rate_excl[0],rate_excl[1]))
    
    ## do we pass the test??
    MaxRates = {"Hlt1":400.,
                "Hlt2":40.}
    for level in ['Hlt1','Hlt2']:
        if GlobalRates[level][0] > MaxRates[level]:
            'print %s rate = %s > max = %s' %(level,GlobalRates[level][0],MaxRates[level])
        else:
            print '%s rates OK' %level
    print '-'*100
    print 'HLT rates summary ends here'
    print '-'*100
    sys.stdout.flush()    
    
if __name__ == "__main__":
    sys.exit( main() )
