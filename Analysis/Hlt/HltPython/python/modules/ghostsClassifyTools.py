# =============================================================================
"""
@namespace ghostsClassifyTools
@brief Low level module. Functions to deal with ghosts classification and link between hits and mcparticles. Examples are functions to find mcparticles linked to velo and tstations in HLT forward tracks or to get weight of a certain mcparticle in a track. Used in tracksAndVerticesClassifyTools.py, causeTools.py, hadHLTpies.py
@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
@date 2008-05-02
"""
# =============================================================================


#! /usr/bin/env python

DEBUG=False

import mctools as mct

#---------------------------------------------------
def getmcpars(TES,fIDs,all_opt=True):
	"""get all the mcparticles from a set of IDs. If all_opt=False, only add those linked.
	"""
	if DEBUG: print TES.dump()
	mcpars=[]
	for i in fIDs:
		mcp=mct.mcParticleFromLHCbID(i, TES)
		if all_opt: mcpars.append(mcp)
		elif mcp: mcpars.append(mcp)

	return mcpars

#---------------------------------------------------

def getmcpar(TES,fIDs,remove_none=False):
	"""get the mcparticle with the biggest weight amongst all the linked from the IDs
	"""

	mcpars = mct.mcParticlesFromLHCbIDs(fIDs,TES)
	if not remove_none: return mcpars[0][0]
	for m in mcpars:
		if m[0]: return m[0]


#---------------------------------------------------

def track_to_mcp(TES,track):
	"""returns the MCP linked to track if this has a weight bigger than 0.7. If there's no such a particle, returns None
	"""
	
	mcpars = mct.mcParticlesFromLHCbIDs(track.lhcbIDs(),TES)
	if mcpars[0][1]>.7 and mcpars[0][0]: return mcpars[0][0]
	else: return None

#---------------------------------------------------

def IsNotGhost(TES,fIDs,cut=0.7):
	"""determine whether a set of IDs has more than 'cut'% of hits from the same MCparticle and which is this
	"""

	mcpars=mct.mcParticlesFromLHCbIDs(fIDs, TES)
	if mcpars[0][1]>=cut: return mcpars[0][0],True
	return None,False


#---------------------------------------------------
def IDs(lhcbIDs):
	"""find hits coming from Velo and TStations
	"""

	vID=[]
	tID=[]

	for lh in lhcbIDs:
		if lh.isVelo():
			vID.append(lh)
		elif lh.isIT() or lh.isOT():
			tID.append(lh)
	return vID,tID

#---------------------------------------------------
def IDs_sorted(lhcbIDs,VELO):
	"""find hits coming from Velo and TStations and return velo sorted
	"""
	vID=[]
	tID=[]

	for lh in lhcbIDs:
		if lh.isVelo():
			vID.append([VELO.sensor(lh.veloID()).z(),lh])
		elif lh.isIT() or lh.isOT():
			tID.append(lh)

	vID.sort()
	vID_sorted=map(lambda x: x[1],vID)
		
	return vID_sorted,tID



#---------------------------------------------------

def getmcparvelo(TES,track):
	"""get mcpar from velo hits
	"""
	vID,tID=IDs(track.lhcbIDs())
	return getmcpar(TES,vID,remove_none=True)



#---------------------------------------------------

def getmcparTS(TES,track):
	"""get mcpar from T stations hits
	"""
	vID,tID=IDs(track.lhcbIDs())
	return getmcpar(TES,tID,remove_none=True)



