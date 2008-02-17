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
import addDict as add


#---------------------------------------------------

def getmcpars(TES,fIDs):
	"""get all the linked mcparticles from a set of IDs
	"""
	if DEBUG: print TES.dump()
	mcpars=[]
	for i in fIDs:
		mcp=mct.mcParticleFromLHCbID(i, TES)
		if mcp:	mcpars.append(mcp)

	return mcpars


#---------------------------------------------------

def getmcpars_all(TES,fIDs):
	"""get all the mcparticles from a set of IDs
	"""
	if DEBUG: print TES.dump()
	mcpars=[]
	for i in fIDs:
		mcpars.append(mct.mcParticleFromLHCbID(i, TES))

	return mcpars



#---------------------------------------------------

def getmcpar(TES,fIDs):
	"""get the mcparticle with the biggest weight amongst all the linked from the IDs
	"""

	def kfromv(dict,v):
		for k in dict:
			if dict[k]==v: return k

	def mcfromk(mcpars,key):
		for m in mcpars:
			if m.key()==key: return m

	mcpars=getmcpars(TES,fIDs)

	a={None:0.0}

	for v in mcpars:
		add.add_elto_dic(a,v.key())

	b=a.values()
	b.sort(reverse=True)
	return mcfromk(mcpars,kfromv(a,b[0]))



#---------------------------------------------------


def IDs(lhcbIDs):
	"""find hits coming from Velo and TStations
	"""
	vID=[]
	tID=[]

	for lh in lhcbIDs:
		if lh.isVelo():
			vID.append(lh)
		elif lh.isST()or lh.isOT:
			tID.append(lh)


	return vID,tID



#---------------------------------------------------

def getmcparvelo(TES,track):
	"""get mcpar from velo hits
	"""
	vID,tID=IDs(track.lhcbIDs())
	return getmcpar(TES,vID)



#---------------------------------------------------

def getmcparTS(TES,track):
	"""get mcpar from T stations hits
	"""
	vID,tID=IDs(track.lhcbIDs())
	return getmcpar(TES,tID)



#---------------------------------------------------

def weight(TES,fIDs):
	"""find the biggest weight amongst the linked mcparticles
	"""

	mcpars=getmcpars_all(TES,fIDs)
	a={None:0.0}
	all=0

	for v in mcpars:
		if v: add.add_elto_dic(a,v.key())
		else: add.add_elto_dic(a,v)

	for v in a.values():
		all+=v

	if a.has_key(None): a.pop(None)
	b=a.values()
	b.sort(reverse=True)
	if all!=0.0 and len(b)>=0: return b[0]*1.0/all
	else: return 0.0


#---------------------------------------------------

def IsNotGhost(TES,fIDs,cut=0.7):
	"""determine whether a set of IDs has more than 'cut'% of hits from the same MCparticle and which is this
	"""

	def kfromv(dict,v):
		for k in dict:
			if dict[k]==v: return k

	mcpars=getmcpars_all(TES,fIDs)
	a={None:0.0}
	all=0

	for v in mcpars:
		if v: add.add_elto_dic(a,v.key())
		else: add.add_elto_dic(a,v)

	for v in a.values():
		all+=v

	if a.has_key(None): a.pop(None)
	b=a.values()
	b.sort(reverse=True)

	if all!=0.0 and len(b)>0: c=b[0]/all
	else: return None,False

	if c>=cut: return kfromv(a,b[0]),True
	else: return None,False


#---------------------------------------------------

def track_to_mcp(TES,track):
	"""returns the MCP linked to track if this has a weight bigger than 0.7. If there's no such a particle, returns None
	"""
	
	mcpars = mct.mcParticlesFromLHCbIDs(track.lhcbIDs(),TES)
	j=0
	ind=0
	w=0.0
	for mcpar in mcpars:
		if mcpar[1]>w and mcpar[0]:
			w=mcpar[1]
			ind=j
		j+=1
	if DEBUG: print "w=",w
	if w>0.7: return mcpars[ind][0]

	else: return None

