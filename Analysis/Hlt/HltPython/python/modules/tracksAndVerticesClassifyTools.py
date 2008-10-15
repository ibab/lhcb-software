# =============================================================================
"""
@namespace tracksAndVerticesClassifyTools
@brief Functions to classify tracks and vertices in HLT according to ghosts, reason of trigger and presence of intruders and holes. Includes one function of each type for tracks and vertices and makes use of low level modules ghostsClassifyTools, holesIntrudersTools and causeTools. Used in hadHLTpies.py
@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
@date 2008-05-02
"""
# =============================================================================



#! /usr/bin/env python


import ghostsClassifyTools as ghc
import holesIntrudersTools as hi
import causeTools as ca


DEBUG=False
####################################################################################################


def classifyTrackByGhosts(TES,track):
	"""classify tracks according to ghosts
	@param TES Transient Event Store
	@param track LHCb track
	@returns 111,110,01,10,00, where 111 is 70% of hits in VELO from MCP1,70% of hits in TS MPC1 (same)/ 110 is 70% of hits in VELO from MCP1, 70% of hits in TS MPC2 (different)/ 01 is ghost in VELO (less than 70 % of hits from same MCP), 70% of hits in TS from MCP1/ 10  is 70% of hits in VELO from MCP1, ghost in TS (less than 70 % of hits from same MCP)/ 00 is ghost in VELO (less than 70 % of hits from same MCP), ghost in TS (less than 70 % of hits from same MCP).

	@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
	@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
	"""

	## find IDs from TStations and VELO
	lhcbIDs=track.lhcbIDs()
	vID,tID=ghc.IDs(lhcbIDs)

	## find wether there are ghosts in them and if not, associated particles
	mcparVe,HVs = ghc.IsNotGhost(TES,vID,0.7)
	mcparTs,HTs = ghc.IsNotGhost(TES,tID,0.7)	

	## stablish categories
	if HVs:

		if HTs:
			if mcparVe==mcparTs: return "111" 
			else: return "110" 

		else: return "10" 

	else:

		if HTs:	return "01"
		else: return "00"


#---------------------------------------------------

def reasonOfTrigger(TES,track,single_di="single"):
	""" classify 'pre trigger' track according to reason of trigger:

	@param TES Transient Event Store
	@param track LHCb track
	@param single_di Are you talking about 'single' or 'di' alleys?
	@returns

	   - ghost:          Has less than 70% of hits from same MC particle

	   - phys:           Comes from a MCparticle which accomplishes the trigger conditions

	   - pt:             Comes from a MCparticle with pt<2.4 GeV to which reconstruction has given a pt bigger than that 

	  - vertex:         Comes from a MCparticle without IP which has been given it due to a wrong vertex used in calculation

	  - IP_slope:       Comes from a MCparticle without IP which has been given it due to a bad slope reconstruction

	  - IP_vertex:      Comes from a MCparticle without IP which has been given it due to a bad vertex reconstruction

	  - IP_both:        Comes from a MCparticle without IP which has been given it due both to a bad slope and vertex reconstruction

	  - IP_combination: Comes from a MCparticle without IP which has been given it due to a bad slope and vertex together reconstruction


	@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
	@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
	"""

	## find mcpar associated to track
	mcpar=ghc.track_to_mcp(TES,track)

	## if there is no, is a ghost
	if not mcpar: return "ghost"

	## find both mc vertex and rc vertex
	mcvertex=ca.getmcvertex(mcpar)
	rcvertex=ca.getrcvertex(TES,track)

	## define wether the track is physics
	if ca.isPhys(mcpar,mcvertex,single_di): return "phys"

	else:
		## if wrong ip causes triggering
		if ca.imppar1(mcpar,mcvertex)<=0.1:
			## find if we used a wrong vertex
			if ca.isVertex(mcvertex,rcvertex): return "vertex"
			## if not, find element whose resolution caused the miscalculation
			else: return ca.whichIP(track,mcpar,rcvertex,mcvertex)

		## if it wasn't ip, it had to be pt
		else: return "pt"


#---------------------------------------------------

def hasHoles(TES,track,VELO):
	"""find out whether 8 first mcpar hits are 8 first used in track reconstruction

	@param TES Transient Event Store. In order to have this function working you need to add to HltMCSeq.Members 'HltLHCbIDMCParticleRelation', in HltMC.opts
	@param track LHCb track
	@param VELO VELO = det['/dd/Structure/LHCb/BeforeMagnetRegion/Velo'], where det = gaudi.detsvc()
	@returns True if track has holes or False if not
	@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
	@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
	"""
	## find hits in velo
	vID,tID=ghc.IDs(track.lhcbIDs())
	## find mcpar associated to VELO track
	mcpar=ghc.getmcparvelo(TES,track)
	rel = TES["Hlt/Link/MCParticleLHCbID"]
	## find sensors where we should find hits from this particle
	sensho=hi.sensors_should(mcpar,rel)
	## find sensors where we actually find them
	senhas=hi.sensors_have(TES,vID,mcpar.key())

	cond=False
	## compare both list of sensors
	comp=hi.compare(sensho,senhas,mcpar,VELO)
	## determine if we miss one amongst 8 first
	for el in comp:
		if el in range(1,9):
			cond=True
			break

	return cond


#---------------------------------------------------

def hasIntruders(TES,track,VELO):
	"""find out whether a track has hits amongst 8 first from different mcpars

	@param TES Transient Event Store
	@param track LHCb track
	@param VELO VELO = det['/dd/Structure/LHCb/BeforeMagnetRegion/Velo'], where det = gaudi.detsvc()
	
	@returns True if track has intruders or False if not
	@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
	@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
	"""

	## find velo hits sosrted according to z
	vID,tID=ghc.IDs_sorted(track.lhcbIDs(),VELO)
	## find mcpars associated to 8 first hits
	mcpars8=ghc.getmcpars(TES,vID[:8],all_opt=False)
	## find most popular amongst them
	mcp8=ghc.getmcpar(TES,vID[:8],remove_none=True)

	## returns True if one these hits wasn't left by the most popular mcpar
	return hi.intruder(mcpars8,mcp8)

#---------------------------------------------------

def findHLTvertices(HLTSUM,TRIG):
	"""Find vertices triggering HLT after removing repeated ones

	@param HLTSUM = gaudi.toolsvc().create('HltSummaryTool',interface='IHltConfSummaryTool')

	@param TRIG Trigger to be analysed. Can be either Hlt1HadronDiDecision or Hlt1HadronDiVFForward.
	@returns vertices for hadHLT dialley, removing repeated
	@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
	
	"""
	## find original vertices in HLTSUM
	hvertices0=HLTSUM.selectionVertices(TRIG)

	hvertices=[]
	pos=[]

	# remove repeated vertices
	for h in hvertices0:

		par=[h.position().x(),h.position().y(),h.position().z()]
		cond_eq=False

		for p in pos:
			k=0
			m=0
			for el in p:

				if round(el,10)==round(par[k],10): m+=1

				k+=1
			## if two coordinates are exactly the same, vertices are the same
			if m>=2:
				cond_eq=True
				break
		## if vertex position not in the list we append it to output
		if not cond_eq:
			pos.append(par)
			hvertices.append(h)

	
	return hvertices


#---------------------------------------------------
def classifyVertexByGhosts(TES,vertex):
	"""Classify vertices from dialley in ghost and no ghosts:

	@param TES Transient Event Store
	@param vertex LHCb vertex

	@returns	
	      - ghost:    There's at least one track linked to vertex non '111'
	      - no_ghost: All tracks linked to vertex are '111'

        @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
        @author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
	"""

	tracks=vertex.tracks()
	## if one track is not "111" return "ghost"
	for t in tracks:
		if classifyTrackByGhosts(TES,t)!="111": return "ghost"
	return "no_ghost"

#---------------------------------------------------

def classifyVertexByReasonOfTrigger(TES,vertex):
	""" Classify vertices from dialley according to the reason of their trigger.

	@param TES Transient Event Store
	@param vertex LHCb vertex

	@returns	
	
	     - ghost:   There's at least one track linked to vertex non '111'
	     - physics: All tracks linked to vertex are physics and come from same eve.
	     - random_vertex: all tracks linked to vertex are physics but they don't come from same MC particle.
	     - no_phys_other: there is at least one track linked to vertex being non physics

	     To see explanation of types of tracks mentioned here type help(getInformationFromHPTtracks) in hadHLTpies

	@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
	@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch

	"""

	def class_pt(tracks):
		## small tool to return from a set of tracks two with biggest pt
		out=[]
		for t in tracks: out.append([t.pt(),t])
		out.sort(reverse=True)
		return out[0][1],out[1][1]

			
	## if vertex is ghost, return so
	if classifyVertexByGhosts(TES,vertex)=="ghost": return "ghost"

	## get vertex tracks
	tracks=vertex.tracks()
	reasons=[]
	## classify them according to pt (PreTrig-High pt, Confirmed -Low pt)
	track_pt,track_ht=class_pt(tracks)

	## find reason of Trigger in both cases
	reasons.append(reasonOfTrigger(TES,track_pt,single_di="di"))
	reasons.append(ca.reasonOfTriggerCF(TES,track_ht))

	## if both tracks are physics
	if reasons.count("phys")==2:
		## if they come from same eve, return phys
		if ca.same_eve(TES,track_pt,track_ht): return "phys"
		## if not,
		else: return "random_vertex"
	## if one of the tracks is not physics	
	return "no_phys_other"
