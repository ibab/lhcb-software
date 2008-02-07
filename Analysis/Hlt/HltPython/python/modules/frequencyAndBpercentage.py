# =============================================================================
""" @namespace frequencyAndBpercentage
@brief Functions to look for B percentage and output rate of different triggers
@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
@date 2008-05-02
"""
# =============================================================================

#! /usr/bin/env python

import gaudimodule
from ROOT import *
from math import *
from array import *

#---------------------------------------------------

def eventHasQuark(TES,quark):
	"""find out wether there's a certain quark in an event

	@param TES Transient Event Store
	@param quark (char):
	   u - up
	   d - down
	   s - strange
	   c - charm
	   b - bottom
	   t - top
	@returns True if quark is present or False if not
	@author Xabier Cid Vidal, xabier.cid.vidal@cern.ch
	"""
	mcpars=TES["/Event/MC/Particles"]
	for m in mcpars:

		if quark=="u" and m.particleID().hasUp(): return True
		if quark=="d" and m.particleID().hasDown(): return True
		if quark=="s" and m.particleID().hasStrange(): return True
		if quark=="c" and m.particleID().hasCharm(): return True
		if quark=="b" and m.particleID().hasBottom(): return True
		if quark=="t" and m.particleID().hasTop(): return True

	return False

#---------------------------------------------------
	
def quarksAndFrequencyInfo(gaudimod=0, TRIGGERS=['L0TriggerHadron',"HadPreTriggerSingle","HadTrigger"],input_f=1000,g_mode=False,N=100000000):

	"""Find percentage of events with at least a b quark and also of events with at least a c quark (when there's no b in it). Also find output rate of each trigger in TRIGGERS.

	@param gaudimod Configuration of the analysis (DATACARDS and extra opts).
	@param TRIGGERS List of triggers to be analysed. Default L0 Hadronic, Hadronic PreTrigger, Hadronic Trigger
	@param input_f (float) Input frequency of your selection in KHz (float)
	@param g_mode  (boolean) Graphical mode. If true returns a dictionary with pies for each trigger including output rate info and purities. Default false
	@param N (int) Number of events to be analysed. Default all

	@returns Returns a dictionary with information collected (number of events of each type) and if g_mode also corresponding pies.
	@author Xabier Cid Vidal, xabier.cid.vidal@cern.ch
	@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
	"""
	
	## check if gaudimodule was properly stablished
	
        if gaudimod==0:
            print "\n ERROR: Please input your gaudimodule.AppMgr()"
            return None

        else:
            gaudi=gaudimod

            try:
		    gaudi.initialize()
	                
		    TES = gaudi.evtsvc()
		    HLTSUM = gaudi.toolsvc().create("HltSummaryTool",
                                interface="IHltConfSummaryTool")

		    gaudi.evtsel().rewind()

            except:
                print "\n  ERROR: Please configure your gaudi"
                return None

	## initialize dictionaries for output
	counters={}

	for trig in TRIGGERS:
		counters[trig]={"all_t":0,"c_exclusive":0,"b_inclusive":0,"nbc":0}
	counters["all"]=0

	for i in range(N):

		## run event
		
		gaudi.run(1)
		
		try: sels = HLTSUM.selections()
		except: break

		## count all events
		counters["all"]+=1

		## check wether there are b and/or c quarks in it
		has_B = eventHasQuark(TES,"b")
		has_C = eventHasQuark(TES,"c")
		
		
		for trig in TRIGGERS:

			if trig in sels:
				## count all events triggering trig
				counters[trig]["all_t"]+=1
				
				## if there are b's: b inclusive
				if has_B: counters[trig]["b_inclusive"]+=1

				## if there aro no b's but c's: c exclusive
				elif has_C: counters[trig]["c_exclusive"]+=1

				## else: count "others" (no b or c)
				else: counters[trig]["nbc"]+=1

	## if no g_mode return only dictionary counters       	
	if not g_mode: return counters

	## Define labels
	labels=["b_inclusive","c_exclusive","nbc"]
	newlabels=["Total b","Exclusive c","Others"]
	freq={}
	titles={}
	f_unc={}

	## Calculate output frequency of each trigger with its uncertainty. Define titles
	for trig in TRIGGERS:
		freq[trig]=counters[trig]["all_t"]*1.0/counters["all"]*input_f
		## uncertainty from error propagation in "a/b"
		f_unc[trig]=sqrt(counters[trig]["all_t"]*counters["all"]*(counters[trig]["all_t"]+counters["all"]))/counters["all"]**2*input_f
		## the information goes to "titles"
		titles[trig]=trig+" purities. Output rate = ("+str(round(freq[trig],3))+"+-"+str(round(f_unc[trig],3))+") KHz)"
	
	vals={}
	colors={}
	valsa={}
	colorsa={}
	pies={}

	## Draw pies for each trigger. In titles we include output frequency info
	for trig in TRIGGERS:
		
		colors[trig]=[]
		vals[trig]=[]

		for i in range(3):
			vals[trig].append(float(counters[trig][labels[i]])/counters[trig]["all_t"])
			colors[trig].append(i+2)

		valsa[trig]=array("f",vals[trig])
		
		colorsa[trig]=array("i",colors[trig])

		pies[trig]=TPie("Percentage in "+trig,titles[trig],3,valsa[trig],colorsa[trig])

		for i in range(3):
			pies[trig].SetEntryLabel(i,newlabels[i])

		pies[trig].SetLabelFormat("#splitline{%perc}{%txt}")
		pies[trig].SetRadius(0.25)


	## return both counters and pies
	return counters,pies
