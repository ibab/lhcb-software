# =============================================================================
"""
@namespace oqTools
@brief Functions to access information of HadPreTriggerSingle tracks with 'offline quality'. Includes link between online tracks and possibly existing offline counterpart and analysis of events firing with these new offline tracks. Makes use of common classification tools from tracksAndVerticesClassifyTools and some low level from ghostsClassifyTools and causeTools.
@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
@date 2008-05-02
"""
# =============================================================================


import ghostsClassifyTools as ghc
import tracksAndVerticesClassifyTools as tr
import causeTools as ca


#---------------------------------------------------
def buildChannelIDs(track):
    """ Get channelIDs from Velo and TStation lhcbIDs

	@param track LHCb track to get channelIDs from
	@returns 2 lists with channelID's from Velo and TStations	
	
	@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
        """

    ## get velo and TStation lhcbIDs from track
    vids,tids=ghc.IDs(track.lhcbIDs())

    ## build output
    vids_id=[]
    tids_id=[]

    ## add to output channelID's
    for v in vids: vids_id.append(v.channelID())
    for t in tids: tids_id.append(t.channelID())

    return vids_id,tids_id

#---------------------------------------------------

def hasCorrectPercentage(IDs,IDs_b):
    """ Check whether there's a 70% of elements of 'IDs' in 'IDs_b'  
    
    @param IDs list with channelIDs whose presence in IDs_b you want to check
    @param IDs_b list with channelIDs to be compared with IDs
    @returns True if there is a 70% or False if not
    
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    """
    
    coinc=0
    ## if one member of IDs_b is in IDs, we add one
    for ID_b in IDs_b:
        if ID_b in IDs: coinc+=1

    ## we weight the number of coincidences with length of IDs
    if coinc*1.0/len(IDs)>=0.7: return True
    
    return False

#---------------------------------------------------

def getOfflineTrack(TES,track):
    """ If exists, get 'offline quality' track linked to the one used in HLT
    
    @param TES Transient event store 
    @param track LHCb track
    
    @returns Offline track if exists or None if it doesn't
    
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    """

    ## get offline best RC tracks from TES
    tracks_b=TES["Rec/Track/Best"]

    ## get channel ID's from Velo and TS from our online track
    vids_id,tids_id=buildChannelIDs(track)

    ## check wether at least 70% of Velo hits and 70% of TS hits from our track are present in one offline track
    for t_b in tracks_b:
        ## get IDs for offline track
        vids_id_b,tids_id_b=buildChannelIDs(t_b)
        ## check percentages
        percVE=hasCorrectPercentage(vids_id,vids_id_b)
        percTS=hasCorrectPercentage(tids_id,tids_id_b)

        ## if we get more than a 70 % both in VELO and TS we say they are the same, we keep it and stop looking for
        if percVE and percTS: return t_b

    ## if we didn't reach the condition, return None
    return None

#---------------------------------------------------

def trackTriggers(TES,track):
    """ Check if a HadPreTriggerSingle track triggers (by using 'online quality' vertices)
    
    @param TES Transient event store 
    @param track LHCb track
    
    @returns True if it does trigger, False if not	
    
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch

    """
    ## Check wether there's a track to check
    if not track: return False

    ## Get RC vertex to calculate IP
    rcvertex=ca.getrcvertex(TES,track)
    ## check if IP is >0.1 mm and pt is > 2.5 GeV
    if ca.imppar1(track,rcvertex)>0.1 and track.pt()>2500: return True
 
    return False

#---------------------------------------------------

def trackTriggersCF(TES,track):

    """ Check if a Confirmed track for HadTrigger accomplishes trigger conditions (by using 'online quality' vertices)
    
    @param TES Transient event store 
    @param track LHCb track
    
    @returns True if it does trigger, False if not	
    
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    """

    ## Check wether there's a track to check
    if not track: return False

    ## Get RC vertex to calculate IP
    rcvertex=ca.getrcvertex(TES,track)
    ## check if IP is >0.1 mm and pt is > 1 GeV
    if ca.imppar1(track,rcvertex)>0.1 and track.pt()>1000: return True

    return False

#---------------------------------------------------

def HPTtracksOfflineQuality(TES,HLTSUM):
    """ Get offline counterpart to 'good'  tracks used in HadPreTriggerSingle. 'Good' meaning existing offline and still accomplishing trigger conditions for that quality level. Works per event.
    
    @param TES Transient Event Store
    @param HLTSUM = gaudi.toolsvc().create('HltSummaryTool',interface='IHltConfSummaryTool')
    
    @returns List with these tracks
    
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    
    """
    ## Get online tracks triggering HadPreTriggerSingle
    tracks=HLTSUM.selectionTracks("HadPreTriggerSingle")
    ## build output,
    tracks_oq=[]
    for t in tracks:
        ## get offline quality track
        t_oq=getOfflineTrack(TES,t)
        ## if exists and triggers, add it to output
        if trackTriggers(TES,t_oq): tracks_oq.append(t_oq)

    return tracks_oq

#---------------------------------------------------

def HPverticesOfflineQuality(TES,HLTSUM):
    """ Get only those vertices used in HadTrigger linked to two good online tracks.'Good' meaning having offline counterpart which still triggers.
    
    @param TES Transient Event Store
    @param HLTSUM = gaudi.toolsvc().create('HltSummaryTool',interface='IHltConfSummaryTool')
    
    @returns List with these vertices	
    
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    
    """

    def class_pt(tracks):
        ## small tool to return from a set of tracks two with biggest pt
	out=[]
	for t in tracks: out.append([t.pt(),t])
	out.sort(reverse=True)
	return out[0][1],out[1][1]

    ## get vertices used in HadTrigger
    vertices=tr.findHTvertices(HLTSUM)
    ## build output
    vertices_oq=[]

    for vert in vertices:
        ## classify tracks linked to vertex in 'HadPreTrigger' (big pt) and 'Confirmed' (low pt)
        trackHPT,trackCF=class_pt(vert.tracks())
        ## link these two tracks to confirmed
        trackHPT_oq=getOfflineTrack(TES,trackHPT)
        trackCF_oq=getOfflineTrack(TES,trackCF)
        ## check if both accomplish corresponding triggering condition. If so, keep vertex
        if trackTriggers(TES,trackHPT_oq) and trackTriggersCF(TES,trackCF_oq): vertices_oq.append(vert) 

    return vertices_oq

#---------------------------------------------------

def eventTriggersOfflineQuality(TES,HLTSUM,trig):

    """ Check if an event fires certain trigger when using offline quality tracks

    @param TES Transient Event Store
    @param HLTSUM = gaudi.toolsvc().create('HltSummaryTool',interface='IHltConfSummaryTool')
    @param trig trigger to be analised
    
    @returns True if does trigger, False if not
    
    
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    
    """

    
    ## if doesn't trigger current HLT, return False
    if trig not in HLTSUM.selections(): return False

    ## if HadPreTrigger, look for any online track existing and still triggering offline. If there's at least one, return True
    if trig=="HadPreTriggerSingle":
        tracks_q=HPTtracksOfflineQuality(TES,HLTSUM)
        if len(tracks_q)>0: return True
        return False

    ## if HadTrigger, look for any vertex whose linked tracks still exist and trigger offline.  If there's at least one, return True
    if trig=="HadTrigger":
        vertices_q=HPverticesOfflineQuality(TES,HLTSUM)
        if len(vertices_q)>0: return True
        return False

    ## if trig not HadTrigger or HadPretrigger and in HLTSUM.selections(), return True
    return True


#---------------------------------------------------

def classifyVertexByGhostsOfflineQuality(TES,vertex):
    """Classify vertices from HadTrigger in ghost and no ghosts by using 'offline quality' tracks linked to them:
    
    @param TES Transient Event Store
    @param vertex LHCb vertex
    
    @returns	
    - ghost:    There's at least one track linked to vertex non '111'
    - no_ghost: All tracks linked to vertex are '111'
    
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    """

    ## get tracks linked to vertex
    tracks=vertex.tracks()
    ## if one track is not 'offline quality' 111 return "ghost"
    for t in tracks:
        if tr.classifyTrackByGhosts(TES,getOfflineTrack(TES,t))!="111": return "ghost"
    return "no_ghost"

#---------------------------------------------------

def classifyVertexByReasonOfTriggerOfflineQuality(TES,vertex):
    """ Classify vertices from HadTrigger according to the reason of their trigger by using 'offline quality' tracks linked to them.
    
    @param TES Transient Event Store
    @param vertex LHCb vertex
    
    @returns	
    
    - ghost:   There's at least one track linked to vertex non '111'
    - physics: All tracks linked to vertex are physics and come from same eve.
    - random_vertex: all tracks linked to vertex are physics but they don't come from same MC particle.
    - no_phys_other: there is at least one track linked to vertex being non physics
    
    To see explanation of types of tracks mentioned here type help(getInformationFromHPTtracks) in hadHLTpies
    
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    
    """

    def class_pt(tracks):
        ## small tool to return from a set of tracks two with biggest pt 
        out=[]
        for t in tracks: out.append([t.pt(),t])
        out.sort(reverse=True)
        return out[0][1],out[1][1]
			
    ## if vertex is ghost 'offline quality', return so
    if classifyVertexByGhostsOfflineQuality(TES,vertex)=="ghost": return "ghost"
    ## get vertex tracks
    tracks=vertex.tracks()
                
    reasons=[]
    ## classify them according to pt (HadPreTrig-High pt, Confirmed -Low pt)
    track_pt,track_ht=class_pt(tracks)

    ## find equivalents with 'offline quality' (if vertex is not ghost, there must be!)
    track_pt_oq=getOfflineTrack(TES,track_pt)
    track_ht_oq=getOfflineTrack(TES,track_ht)

    ## find reason of Trigger in both cases
    reasons.append(tr.reasonOfTrigger(TES,track_pt_oq))
    reasons.append(ca.reasonOfTriggerHT(TES,track_ht_oq))

    ## if both tracks are physics
    if reasons.count("phys")==2:
        ## if they come from same eve, return phys
        if ca.same_eve(TES,track_pt,track_ht): return "phys"
        ## if not,
        else: return "random_vertex"
    ## if one of the tracks is not physics
    return "no_phys_other"


    
