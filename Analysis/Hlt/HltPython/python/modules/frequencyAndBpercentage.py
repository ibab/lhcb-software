# =============================================================================
"""
@namespace frequencyAndBpercentage
@brief Functions to look for B percentage and output rate of different triggers, given a configured job and an input rate. Includes one function to find out whether a certain quark is present in an event and another to test module by giving a set of datacards and configuring a job.
@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
@date 2008-05-02
"""
# =============================================================================

import gaudimodule
import math
import copy
import drawPiesAndHistos as dph

#---------------------------------------------------

def eventHasQuark(TES,quark):
	"""find out wether there's a certain quark in an event

	@param TES Transient Event Store
	@param quark (char): u - up/ d - down/ s - strange/ c - charm/ b - bottom/ t - top
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

	## Define labels and prepare newlabels for drawPieFromDict
	labels=["b_inclusive","c_exclusive","nbc"]
	newlabels_o=["Total b","Exclusive c","Others"]
	newlabels={}
	
	for trig in TRIGGERS:
		newlabels[trig]={}
		j=0
		for l in labels:
			newlabels[trig][l]=newlabels_o[j]
			j+=1
	
	freq={}
	titles={}
	f_unc={}

	##Prepare counters to use in drawPieFromDict
	counters_c=copy.copy(counters)
	all=counters.pop("all")

	## Calculate output frequency of each trigger with its uncertainty. Define titles. Build pies
	pies={}
	for trig in TRIGGERS:
		all_t=counters[trig].pop("all_t")
		freq[trig]=all_t*1.0/all*input_f
		## uncertainty from error propagation in "a/b"
		f_unc[trig]=math.sqrt(all_t*all*(all_t+all))/all**2*input_f
		## the information goes to "titles"
		titles[trig]=trig+" purities. Output rate = ("+str(round(freq[trig],3))+"+-"+str(round(f_unc[trig],3))+") KHz)"
		## Draw pies for each trigger. In titles we include output frequency info
		pies[trig]=dph.drawPieFromDict(counters[trig],title=titles[trig],labels=newlabels[trig],sort=True)

	
	## return both counters and pies
	return counters_c,pies



#---------------------------------------------------
	
def testForQuarksAndFrequencyInfo():
	"""Test for quarksAndFrequencyInfo. Prepares job to run in a set of minbias events. Configuration includes Datacards, HltJob and HltMC options. 
	@returns Returns a dictionary with information collected (number of events of each type) and corresponding pies for L0TriggerHadron,HadPreTriggerSingle and HadTrigger in selected datacards (running HltJob in minbias set of events).
	@author Xabier Cid Vidal, xabier.cid.vidal@cern.ch
	"""

	## set physical location for different options
	HOMEDIR = "/afs/cern.ch/user/j/jcidvida/cmtuser/DaVinci_v19r5"

	## options for HLT
	HLTJOB    = HOMEDIR+"/opts/HltJob.opts"
	## datacards to be read (min bias events)
	DATACARDS = HOMEDIR+"/opts/dstL0_minbias_lumi2.opts"
	## options to have access to montecarlo information
	UNPACK    = HOMEDIR+"/opts/UnpackMC.opts"
	MCOPTS = HOMEDIR+"/opts/HltMC.opts"

	## prepare job configuration
	FILES=[HLTJOB,DATACARDS,MCOPTS,UNPACK]
	EOPTS=['HltSummaryWriter.SaveAll = true', 'HltSelectionDataToTes.CopyAll = true', 'HistogramPersistencySvc.OutputFile = "freq_and_bperc.root"']

	## configure job to make it ready to quarksAndFrequencyInfo
	gaudi = gaudimodule.AppMgr(outputlevel=3)
	gaudi.config(files = FILES,options=EOPTS)

	## return info and pies
	return quarksAndFrequencyInfo(gaudi,g_mode=True,N=3000)

