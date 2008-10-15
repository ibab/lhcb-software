# =============================================================================
"""
@namespace frequencyAndBpercentage
@brief Functions to look for B percentage and output rate of different triggers, given a configured job and an input rate. Includes one function to draw output of previous function in the form of pies. Also one to find out whether a certain quark is present in an event and another to test module by giving a set of datacards and configuring a job.
@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
@date 2008-05-02
"""
# =============================================================================

import GaudiPython
import math
import copy
import drawPiesAndHistos as dph
import oqTools as oq
#---------------------------------------------------


TRIG_DICT={"single":"Hlt1HadronSingleTFGFwrd","single_conf":"Hlt1HadronSingleDecision","didec":"Hlt1HadronDiVFForward","didec_conf":"Hlt1HadronDiDecision"}


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
	
def getQuarksAndFrequencyInfo(gaudimod=0, TRIGGERS=["L0HadronDecision","Hlt1HadronSingleDecision" , "Hlt1HadronDiDecision"],input_f=1000,g_mode=False,oq_mode=False,N=10000000):

	"""Find percentage of events with at least a b quark and also of events with at least a c quark (when there's no b in it). Also find output rate of each trigger in TRIGGERS.

	@param gaudimod Configuration of the analysis (DATACARDS and extra opts).
	@param TRIGGERS List of triggers to be analysed. Default L0 Hadronic, Hadronic PreTrigger, Hadronic Trigger
	@param input_f (float) Input frequency of your selection in KHz
	@param g_mode  (boolean) Graphical mode. If true returns a dictionary with pies for each trigger including output rate info and purities. Default false
        @param oq_mode Boolean. Keep only events having offline counterpart to HPT and HT online tracks if they exist and still trigger. Default False.
	@param N (int) Number of events to be analysed. Default all

	@returns Returns a dictionary with information collected (number of events of each type) and if g_mode also corresponding pies.
	@author Xabier Cid Vidal, xabier.cid.vidal@cern.ch
	@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
	"""
	
	## check if gaudimodule was properly stablished
	
        if gaudimod==0:
            print "\n ERROR: Please input your GaudiPython.AppMgr()"
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

		mcpars=TES["MC/Particles"]
		if not mcpars: break

		sels=HLTSUM.selections()

		## count all events
		counters["all"]+=1

		## check wether there are b and/or c quarks in it
		has_B = eventHasQuark(TES,"b")
		has_C = eventHasQuark(TES,"c")
		
		
		for trig in TRIGGERS:

			if oq_mode: cond_trig=oq.eventTriggersOfflineQuality(TES,HLTSUM,trig)
			else: cond_trig=(trig in sels)

			if cond_trig:
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
	## else, return both counters and pies
	return counters,drawQuarksAndFrequencyInfo(counters,TRIGGERS,input_f,oq_mode)
	

#----------------------------------------

def drawQuarksAndFrequencyInfo(counters,TRIGGERS=["L0HadronDecision","Hlt1HadronSingleDecision" , "Hlt1HadronDiDecision"],input_f=1000,oq_mode=False):

        """ Build pies with information about output rate and purities given output from getQuarksAndFrequencyInfo and a list of triggers.
        @param counters Output of getQuarksAndFrequencyInfo. Dictionary with all the info about 'reason of trigger'.
        @param TRIGGERS List of triggers whose output and purity you want to draw. Default L0 Hadronic, Hadronic PreTrigger, Hadronic Trigger
	@param input_f (float) Input frequency of your selection in KHz.
        @param oq_mode Mention in titles you're using 'offline quality' tracks. Default False.
    
        @returns
          Graphic information with one pie for each trigger in the input.
          For more info of classification introduced, type 'help(getQuarksAndFrequencyInfo)' in this module.
        @author Xabier Cid Vidal, xabier.cid.vidal@cern.ch
        """

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
	all=counters_c.pop("all")

	## Calculate output frequency of each trigger with its uncertainty. Define titles. Build pies
	pies={}
	for trig in TRIGGERS:
		count_trig=copy.copy(counters_c[trig])
		all_t=count_trig.pop("all_t")
		freq[trig]=all_t*1.0/all*input_f
		## uncertainty from error propagation in "a/b"
		f_unc[trig]=math.sqrt(all_t*all*(all_t+all))/all**2*input_f
		## the information goes to "titles"
		titles[trig]=trig+" purities. Output rate = ("+str(round(freq[trig],3))+"+-"+str(round(f_unc[trig],3))+" KHz)"
		if oq_mode and (trig in TRIG_DICT.values()): titles[trig]+=", having used 'offline quality' tracks"

		## Draw pies for each trigger. In titles we include output frequency info
		pies[trig]=dph.drawPieFromDict(count_trig,title=titles[trig],labels=newlabels[trig],sort=True)

	
	## return pies
	return pies



#---------------------------------------------------
	
def testForQuarksAndFrequencyInfo():
	"""Test for quarksAndFrequencyInfo. Prepares job to run in a set of minbias events. Configuration includes Datacards, HltJob and HltMC options. 
	@returns Returns a dictionary with information collected (number of events of each type) and corresponding pies for 'L0HadronDecision','Hlt1HadronSingleDecision' , 'Hlt1HadronDiDecision' in selected datacards (running HltJob in minbias set of events).
	@author Xabier Cid Vidal, xabier.cid.vidal@cern.ch
	"""

	## set physical location for different options
	HOMEDIR = "/afs/cern.ch/user/j/jcidvida/cmtuser/HLT/DaVinci_v19r14"

	## options for HLT
	HLTJOB = "$HLTSYSROOT/options/HltJob.opts"
	## DaVinci Job
	DVJOB = "$DAVINCIROOT/options/DaVinci.opts"
	## datacards to be read (min bias events)
	DATACARD= HOMEDIR+"/opts/dstL0_minbias_lumi2.opts"
	## options to have access to montecarlo information
	MCOPTS   = HOMEDIR+"/opts/HltMC.opts"
	
	## prepare job configuration
	EOPTS = ["HltSummaryWriter.SaveAll = true"]
	## configure job to make it ready for quarksAndFrequencyInfo
	gaudi = GaudiPython.AppMgr(outputlevel=3)
	gaudi.config(files = [DVJOB,HLTJOB,DATACARD,MCOPTS],options=EOPTS)

	## return info and pies
	return getQuarksAndFrequencyInfo(gaudi,g_mode=True,N=3000)

