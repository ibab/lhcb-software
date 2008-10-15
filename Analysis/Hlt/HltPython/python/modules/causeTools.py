# =============================================================================
"""
@namespace causeTools
@brief Low level module. Contains tools to find reason of trigger in tracks and vertices of Hadronic HLT. Examples are IP calculator or functions to get Reconstructed and MC vertices in an event. Includes also some functions to fill information relevant for had HLT. Used in tracksAndVerticesClassifyTools.py and hadHLTexamples.py
@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
@date 2008-05-02
"""
# =============================================================================

import math
import addDict as add
import mctools as mct
import pidtools as pidt
import ghostsClassifyTools as ghc

DEBUG=False

#---------------------------------------------------

def imppar1(track,vertex):
	"""calculate IP of track or mcparticle in relation to vertex
	"""

	## vertex pos
	x0=vertex.position().x()
	y0=vertex.position().y()
	z0=vertex.position().z()


	if DEBUG:
		print "x0=",x0
		print "y0=",y0
		print "z0=",z0

	## momenta
	pz=track.momentum().z()
	sx=float(track.momentum().x())/pz
	sy=float(track.momentum().y())/pz

	if DEBUG:
		print "pz=",pz
		print "sx=",sx
		print "sy=",sy

	
	if track.classID()==10010:
	##track position
		xH=track.position().x()
		yH=track.position().y()
		zH=track.position().z()

	elif track.classID()==210:
	## mcpar position
	    xH=track.originVertex().position().x()
	    yH=track.originVertex().position().y()
	    zH=track.originVertex().position().z()
	    
	else:
		return 0.0

	if DEBUG:
		print "xH=",xH
		print "yH=",yH
		print "zH=",zH

	## reconstruct parameter impact
	landa=1/pz*1/(sx**2+sy**2+1)*((x0-xH)*sx+(y0-yH)*sy+(z0-zH))
	A=xH+sx*pz*landa
	B=yH+sy*pz*landa
	C=zH+pz*landa

	b= math.sqrt(((A-x0)**2+(B-y0)**2+(C-z0)**2))

	return b 

#---------------------------------------------------

def eve_p(par):
	"""find particle produced in primary vertex where par comes from
	"""
	evep,i=mct.eve(par)

	return evep 
        	

#---------------------------------------------------

def getmcvertex(mcpar):
	"""get MC primary vertex
	"""
	return eve_p(mcpar).originVertex()

#---------------------------------------------------

def getrcvertex(TES,track):
	"""get vertex used in RC
	"""
	
	vertices=TES["/Event/Hlt/Vertex/PV2D"]
	
	## find vertex for smallest imppar
	par=imppar1(track,vertices[0])
	vertex=vertices[0]

	for v in vertices:
		if imppar1(track,v)<par:
			par=imppar1(track,v)
			vertex=v
	return vertex



#---------------------------------------------------

def isPhys(mcpar,mcvertex,single_di="single"):
	"""set if the pretrigger track is physics
	"""

	if single_di=="single": mcptmin=5000
	else: mcptmin=2400
	
	IPphys=imppar1(mcpar,mcvertex)
	if IPphys>=0.1 and mcpar.pt()>=mcptmin: phys=True
	else: phys=False

	return phys

#---------------------------------------------------

def isPhysCF(mcpar,mcvertex):
	"""set if the confirmation track is physics
	"""

	
	IPphys=imppar1(mcpar,mcvertex)
	if IPphys>=0.1 and mcpar.pt()>=1000: phys=True
	else: phys=False

	return phys

#---------------------------------------------------

def isVertex(mcvertex,rcvertex):
	"""set wether we have used a wrong vertex
	"""

	def positions(vertex):
		"""build an array with the positions of a vertex
		"""
		out=[]
		out.append(vertex.position().x())
		out.append(vertex.position().y())
		out.append(vertex.position().z())
		return out
	
	dif=0
	rcpos=positions(rcvertex)
	mcpos=positions(mcvertex)
	for i in range(3):
		dif+=(rcpos[i]-mcpos[i])**2
	dif=math.sqrt(dif)
	if dif>5*0.072:
		return True
	else: return False

#---------------------------------------------------

def whichIP(track,mcpar,rcvertex,mcvertex):
	"""set which element to blame within resolutions (pt or others:track,vertex,both or combination)
	"""
	
	if imppar1(track,mcvertex)>=0.1:
		if imppar1(mcpar,rcvertex)>=0.1: return "IP_both" 
		else: return "IP_slope"

	elif imppar1(mcpar,rcvertex)>=0.1: return "IP_vertex"
	else: return "IP_combination"

	return counters

#---------------------------------------------------

def fill_deltap(TES,track,list):

	"""Fill deltap over p for a track and its associated mcparticle
	"""
	def module(x,y,z): return math.sqrt(x**2+y**2+z**2)

	mcpar=ghc.track_to_mcp(TES,track)

	if not mcpar: return None
	
	xdif=mcpar.momentum().x()-track.momentum().x()
	ydif=mcpar.momentum().y()-track.momentum().y()
	zdif=mcpar.momentum().z()-track.momentum().z()

	mcmod=module(mcpar.momentum().x(),mcpar.momentum().y(),mcpar.momentum().z())
	trackmod=module(track.momentum().x(),track.momentum().y(),track.momentum().z())

	deltapop=module(xdif,ydif,zdif)/mcmod
	
	if mcmod>=trackmod: list.append(deltapop)
	else: list.append(-deltapop)


#---------------------------------------------------

def non_b_or_c_ph(TES,track,dic_e,list_r,list_z):
	"""find eves when they don't have b or c quark and their decay point (radio and z)
	"""
	mcpar=ghc.track_to_mcp(TES,track)
	##find eves
	evep=eve_p(mcpar)
	## if eve has not b or c
	if not evep.particleID().hasBottom() and not evep.particleID().hasCharm():
		## fill eves
		add.add_elto_dic(dic_e,pidt.PTranslate(evep.particleID().pid()))

		## find decay point
		ver=evep.endVertices()[0]
		## fill r of decay
		list_r.append(math.sqrt(ver.position().x()**2+ver.position().y()**2))
		## fill z of decay
		list_z.append(ver.position().z())


#---------------------------------------------------

def origin_quark(TES,track):
	"""find wether the particle originating track has quark B or C
	"""
	
	evep=eve_p(ghc.track_to_mcp(TES,track))
	parID=evep.particleID()

	if parID.hasBottom(): return "isB"
	elif parID.hasCharm(): return "isC"
	else: return "others"
	

#---------------------------------------------------

def reasonOfTriggerCF(TES,track):
	""" classify confirmation track according to reason of trigger:

	- ghost:          Has less than 70% of hits from same MC particle

	- phys:           Comes from a MCparticle which accomplishes the trigger conditions

	- pt:             Comes from a MCparticle with pt<1 GeV to which reconstruction has given a pt bigger than that 

	- vertex:         Comes from a MCparticle without IP which has been given it due to a wrong vertex used in calculation

	- IP_slope:       Comes from a MCparticle without IP which has been given it due to a bad slope reconstruction

	- IP_vertex:      Comes from a MCparticle without IP which has been given it due to a bad vertex reconstruction

	- IP_both:        Comes from a MCparticle without IP which has been given it due both to a bad slope and vertex reconstruction

	- IP_combination: Comes from a MCparticle without IP which has been given it due to a bad slope and vertex together reconstruction

	"""

	## find mcpar associated to track
	mcpar=ghc.track_to_mcp(TES,track)

	## if there is no, is a ghost
	if not mcpar: return "ghost"

	## find both mc vertex and rc vertex
	mcvertex=getmcvertex(mcpar)
	rcvertex=getrcvertex(TES,track)

	## define wether the track is physics
	if isPhysCF(mcpar,mcvertex): return "phys"

	else:
		## if wrong ip causes triggering
		if imppar1(mcpar,mcvertex)<=0.1:
			## find if we used a wrong vertex
			if isVertex(mcvertex,rcvertex): return "vertex"
			## if not, find element whose resolution caused the miscalculation
			else: return whichIP(track,mcpar,rcvertex,mcvertex)

		## if it wasn't ip, it had to be pt
		else: return "pt"


#---------------------------------------------------

def same_eve(TES,track_pt,track_ht):
	"""find out wether both tracks come from same mcparticle
	"""
	
	mcp_pt=ghc.track_to_mcp(TES,track_pt)
	mcp_ht=ghc.track_to_mcp(TES,track_ht)
	if mcp_pt and mcp_ht:
		if eve_p(mcp_pt).key()==eve_p(mcp_ht).key(): return True

	return False
