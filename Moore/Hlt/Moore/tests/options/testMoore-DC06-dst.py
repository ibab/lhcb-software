#!/usr/bin/env gaudirun.py

Moore().hltType = 'Hlt1' # +Hlt2'
Moore().EvtMax = 2000
Moore().DataType = 'DC06'
Moore().Simulation = True
Moore().inputFiles = [ '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-1.dst'
                     , '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-2.dst'
                     , '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-3.dst'
                     , '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-4.dst' ]

LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"

EventSelector().PrintFreq = 100
