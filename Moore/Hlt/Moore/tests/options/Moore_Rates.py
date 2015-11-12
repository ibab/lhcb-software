#!/usr/bin/env python
#
# Script to test independence of Hlt lines

import os, sys, subprocess, re, optparse

# Configuration
import Gaudi.Configuration
from Gaudi.Configuration import *
from Configurables import GaudiSequencer as Sequence
from Configurables import EventSelector, HltConf

from Moore.Configuration import Moore

# GaudiPython
from GaudiPython import AppMgr

def main():

    # Setup the option parser
    usage = "usage: %prog [options] inputfile <inputfile>"
    parser = optparse.OptionParser( usage = usage )
    parser.add_option( "-d", "--datatype", action="store", dest="DataType", 
                       default="2015", help="DataType to run on.")
    parser.add_option( "-n", "--evtmax", type="int", action = "store", dest = "EvtMax",
                       default = 10000, help = "Number of events to run" )
    parser.add_option( "--dddbtag", action="store", dest="DDDBtag",
                       default='dddb-20150526', help="DDDBTag to use" )
    parser.add_option( "--conddbtag", action = "store", dest = "CondDBtag",
                       default = 'cond-20150617', help = "CondDBtag to use" )
    parser.add_option( "--settings", action = "store", dest="ThresholdSettings",
                       default = 'Physics_25ns_October2015',
                       help = "ThresholdSettings to use")
    parser.add_option( "-s", "--simulation", action = "store_true", dest = "Simulation",
                       default = False, help = "Run on simulated data")

    # Parse the arguments
    (options, args) = parser.parse_args()

    #### configured from the arguments
    Moore().EvtMax = options.EvtMax
    Moore().ThresholdSettings = options.ThresholdSettings
    Moore().DDDBtag   = options.DDDBtag
    Moore().CondDBtag = options.CondDBtag
    Moore().Simulation = options.Simulation
    Moore().DataType   = options.DataType
    Moore().inputFiles = args

    #### hard coded here
    Moore().ForceSingleL0Configuration = False
    Moore().OutputLevel = 6
    Moore().RemoveInputHltRawBanks = True
    Moore().Simulation = False
    Moore().Split="Hlt1"
    Moore().UseTCK = False
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
    IOHelper("MDF").inputFiles(["mdf://root://eoslhcb.cern.ch//eos/lhcb/wg/HLT/BWdivData/Run164440_L0Filtered_0x00A2_Mika/2015NB_25ns_0x00A2_0.mdf",
                                "mdf://root://eoslhcb.cern.ch//eos/lhcb/wg/HLT/BWdivData/Run164440_L0Filtered_0x00A2_Mika/2015NB_25ns_0x00A2_1.mdf",
                                "mdf://root://eoslhcb.cern.ch//eos/lhcb/wg/HLT/BWdivData/Run164440_L0Filtered_0x00A2_Mika/2015NB_25ns_0x00A2_2.mdf",
                                "mdf://root://eoslhcb.cern.ch//eos/lhcb/wg/HLT/BWdivData/Run164440_L0Filtered_0x00A2_Mika/2015NB_25ns_0x00A2_3.mdf",
                                "mdf://root://eoslhcb.cern.ch//eos/lhcb/wg/HLT/BWdivData/Run164440_L0Filtered_0x00A2_Mika/2015NB_25ns_0x00A2_4.mdf",
                                "mdf://root://eoslhcb.cern.ch//eos/lhcb/wg/HLT/BWdivData/Run164440_L0Filtered_0x00A2_Mika/2015NB_25ns_0x00A2_5.mdf"])
    
    gaudi = AppMgr(outputlevel=4)
    gaudi.ExtSvc += ['ToolSvc']                                                                                         
    gaudi.ExtSvc.append( 'DataOnDemandSvc' )    
    gaudi.initialize()
    evtmax = Moore().EvtMax
    i = 0

    processed = 0
    line_stats = {}
    while i < evtmax:
        i+=1
        gaudi.run(1)
        try:
            reps =  gaudi.evtsvc()['Hlt1/DecReports']
            lines = reps.decReports().keys()
            nPassed = 0
            for line in lines:
                if not line in line_stats.keys():
                    line_stats[line] = {"passed_incl":0,
                                        "passed_excl":0}
                if reps.decReport(line).decision():
                    line_stats[line]["passed_incl"]  += 1
                    if not "Global" in line:
                        nPassed +=1 
            for line in lines:
                if reps.decReport(line).decision() and nPassed == 1:
                    if not "Global" in line:
                        line_stats[line]["passed_excl"]  += 1
            processed +=1
        except:
            print 'no more events?'
            break

    for k,v in line_stats.iteritems():
        v["processed"] = processed
    
    #import json
    #with open('rates.json', 'w') as outfile:
    #    json.dump(line_stats, outfile)
    #with open('rates.json') as json_data:
    #rates = json.load(json_data)
    #json_data.close()
    #pprint(rates)
    
    #################################
    ###### print the summary 
    #################################
    import math
    iLine = 0
    #### order by inclusive rate
    print '-'*100
    print 'Hlt1 rates, inclusive, exclusive, both in kHz'
    print '-'*100
    OrderedStats = {}
    for key, value in line_stats.iteritems():
        OrderedStats[key] = value["passed_incl"]
    for line_name, rate in sorted(OrderedStats.iteritems(), key=lambda (v,k): (k,v),reverse=True):
        iLine += 1
        eff_incl = line_stats[line_name]["passed_incl"]/float(line_stats[line_name]["processed"])
        eff_excl = line_stats[line_name]["passed_excl"]/float(line_stats[line_name]["processed"])
        if "Hlt1Global" in line_name:
            GlobalRate = 1.e3*eff_incl
        print ('%s:%s\t' %(iLine,line_name.replace("Decision",""))).expandtabs(40),
        print ('%.2f+-%.2f\t' %(1.e3*eff_incl,1.e3*math.sqrt((eff_incl-eff_incl**2)/float(line_stats[line_name]["processed"])))).expandtabs(25),
        print ('%.2f+-%.2f' %(1.e3*eff_excl,1.e3*math.sqrt((eff_excl-eff_excl**2)/float(line_stats[line_name]["processed"])))).expandtabs(25)

    print '-'*100
    ### give a large rate just to let things pass for now
    if GlobalRate > 400:
        print 'Hlt1Rate > 400 kHz!'
    else:
        print 'Hlt1 rates OK'
    print '-'*100

if __name__ == "__main__":
    sys.exit( main() )
