# =============================================================================
"""
@namespace oqTools
@brief Functions to access information of HadHTL tracks with 'offline quality'. Includes link between online tracks and possibly existing offline counterpart and analysis of events firing with these new offline tracks. Makes use of common classification tools from tracksAndVerticesClassifyTools and some low level from ghostsClassifyTools and causeTools.
@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
@date 2008-05-02
"""
# =============================================================================


import ghostsClassifyTools as ghc
import tracksAndVerticesClassifyTools as tr
import causeTools as ca


TRIG_DICT={"single":"Hlt1HadronSingleTFGFwrd","single_conf":"Hlt1HadronSingleDecision","didec":"Hlt1HadronDiVFForward","didec_conf":"Hlt1HadronDiDecision"}



#---------------------------------------------------
def buildChannelIDs(track,dets=True):
    """ Get channelIDs from Velo and TStation lhcbIDs or from all of them

	@param track LHCb track to get channelIDs from
        @param dets (bool) Separate Velo and TS hits or return all channelIDs together
	@returns 2 lists with channelID's from Velo and TStations	
	
	@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
        """

    ## get velo and TStation lhcbIDs from track
    vids,tids=ghc.IDs(track.lhcbIDs())

    ## build output
    vids_id=[]
    tids_id=[]
    ids_all=[]

    ## add to output channelID's
    for v in vids:
        vids_id.append(v.channelID())
        ids_all.append(v.channelID())
    for t in tids:
        tids_id.append(t.channelID())
        ids_all.append(t.channelID())

    if dets: return vids_id,tids_id
    else: return ids_all

#---------------------------------------------------

def hasCorrectPercentage(IDs,IDs_b,perc=70):
    """ Check whether there's a 70% of elements of 'IDs' in 'IDs_b'  
    
    @param IDs list with channelIDs whose presence in IDs_b you want to check
    @param IDs_b list with channelIDs to be compared with IDs
    @param perc percentage over 100 needed to return a True
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
    cont_best=TES["Rec/Track/Best"]
    return getCounterpartInContainer(cont_best,track)

#---------------------------------------------------

def getCounterpartInContainer(container,input_track,opt_det=True):
    """ If exists, get a track in container linked to 'input track'
    
    @param container container having tracks where we want to find the counterpart
    @param input_track LHCb track
    @param opt_det (Bool) Ask counterpart to have 70% of hits both in VELO and TS. If false this condition must not be per detector
    
    @returns Track in container if exists or None if it doesn't
    
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    """
        
    ## get channel ID's from Velo and TS from our online track
    if opt_det: vids_id,tids_id=buildChannelIDs(input_track)
    else: ids=buildChannelIDs(input_track,dets=False)

    ## check wether at least 70% of Velo hits and 70% of TS hits from our track are present in one offline track
    for t_b in container:
        if opt_det:
            ## get IDs for offline track
            vids_id_b,tids_id_b=buildChannelIDs(t_b)
            ## check percentages
            percVE=hasCorrectPercentage(vids_id,vids_id_b)
            percTS=hasCorrectPercentage(tids_id,tids_id_b)
            ## if we get more than a 70 % both in VELO and TS we say they are the same, we keep it and stop looking for
            if percVE and percTS: return t_b

        else:
            ## get IDs for offline track
            ids_b=buildChannelIDs(t_b,dets=False)
            ## check percentage
            perc=hasCorrectPercentage(ids,ids_b,perc=75)
            ## if we get more than a 70 % we say they are the same, we keep it and stop looking for
            if perc: return t_b


    ## if we didn't reach the condition, return None
    return None

    

#---------------------------------------------------

def trackTriggers(TES,track,single_di="single"):
    """ Check if a hadHLT track triggers (by using 'online quality' vertices)
    
    @param TES Transient event store 
    @param track LHCb track
    
    @returns True if it does trigger, False if not	
    
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch

    """
    if single_di=="di": pt_min=2500
    else: pt_min=5000

    ## Check wether there's a track to check
    if not track: return False

    ## Get RC vertex to calculate IP
    rcvertex=ca.getrcvertex(TES,track)
    ## check if IP is >0.1 mm and pt is > 2.5 GeV
    if ca.imppar1(track,rcvertex)>0.1 and track.pt()>pt_min: return True
 
    return False

#---------------------------------------------------

def trackTriggersCF(TES,track):

    """ Check if a Confirmed track for DiHadronLine accomplishes trigger conditions (by using 'online quality' vertices)
    
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

def HLTtracksOfflineQuality(TES,HLTSUM,TRIG):
    """ Get offline counterpart to 'good'  tracks used in HadHLT. 'Good' meaning existing offline and still accomplishing trigger conditions for that quality level. Works per event.
    
    @param TES Transient Event Store
    @param HLTSUM = gaudi.toolsvc().create('HltSummaryTool',interface='IHltConfSummaryTool')
    @param TRIG Trigger to be analysed
    
    @returns List with these tracks
    
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    
    """
    ## Get online tracks triggering SingleAlley or 'pretrigger' tracks in DiAlley
    if TRIG=="Hlt1HadronSingleTFGFwrd" or TRIG=="Hlt1HadronSingleDecision": sd="single"
    else: sd="di"
    
    tracks=HLTSUM.selectionTracks(TRIG)
    ## build output,
    tracks_oq=[]
    for t in tracks:
        ## get offline quality track
        t_oq=getOfflineTrack(TES,t)
        ## if exists and triggers, add it to output
        if trackTriggers(TES,t_oq,sd): tracks_oq.append(t_oq)

    return tracks_oq

#---------------------------------------------------

def HLTverticesOfflineQuality(TES,HLTSUM,TRIG):
    """ Get only those vertices used in Hlt1HadronDiVFForward linked to two good online tracks.'Good' meaning having offline counterpart which still triggers.
    
    @param TES Transient Event Store
    @param HLTSUM = gaudi.toolsvc().create('HltSummaryTool',interface='IHltConfSummaryTool')
    @param TRIG Trigger to be analysed

    
    @returns List with these vertices	
    
    @author Xabier Cid Vidal xabier.cid.vidal@cern.ch
    
    """

    def class_pt(tracks):
        ## small tool to return from a set of tracks two with biggest pt
	out=[]
	for t in tracks: out.append([t.pt(),t])
	out.sort(reverse=True)
	return out[0][1],out[1][1]

    ## get vertices used in Hadron Di alley
    vertices=tr.findHLTvertices(HLTSUM,TRIG)
    ## build output
    vertices_oq=[]

    for vert in vertices:
        ## classify tracks linked to vertex in 'PreTrigger' (big pt) and 'Confirmed' (low pt)
        trackHLT,trackCF=class_pt(vert.tracks())
        ## link these two tracks to confirmed
        trackHLT_oq=getOfflineTrack(TES,trackHLT)
        trackCF_oq=getOfflineTrack(TES,trackCF)
        ## check if both accomplish corresponding triggering condition. If so, keep vertex
        if trackTriggers(TES,trackHLT_oq,single_di="di") and trackTriggersCF(TES,trackCF_oq): vertices_oq.append(vert) 

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

    ## if Single Alley, look for any online track existing and still triggering offline. If there's at least one, return True
    if trig=="Hlt1HadronSingleTFGFwrd" or trig=="Hlt1HadronSingleDecision":
        tracks_q=HLTtracksOfflineQuality(TES,HLTSUM,trig)
        if len(tracks_q)>0: return True
        return False

    ## if DiAlley, look for any vertex whose linked tracks still exist and trigger offline.  If there's at least one, return True
    if trig=="Hlt1HadronDiVFForward" or trig=="Hlt1HadronDiDecision":
        vertices_q=HLTverticesOfflineQuality(TES,HLTSUM,trig)
        if len(vertices_q)>0: return True
        return False

    ## if trig not in DiAlley or SingleAlley and in HLTSUM.selections(), return True
    return True


#---------------------------------------------------

def classifyVertexByGhostsOfflineQuality(TES,vertex):
    """Classify vertices from HadHLT in ghost and no ghosts by using 'offline quality' tracks linked to them:
    
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
    """ Classify vertices from HadHLT according to the reason of their trigger by using 'offline quality' tracks linked to them.
    
    @param TES Transient Event Store
    @param vertex LHCb vertex
    
    @returns	
    
    - ghost:   There's at least one track linked to vertex non '111'
    - physics: All tracks linked to vertex are physics and come from same eve.
    - random_vertex: all tracks linked to vertex are physics but they don't come from same MC particle.
    - no_phys_other: there is at least one track linked to vertex being non physics
    
    To see explanation of types of tracks mentioned here type help(getInformationFromHLTtracks) in hadHLTpies
    
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
    ## classify them according to pt (PreTrig-High pt, Confirmed -Low pt)
    track_pt,track_ht=class_pt(tracks)

    ## find equivalents with 'offline quality' (if vertex is not ghost, there must be!)
    track_pt_oq=getOfflineTrack(TES,track_pt)
    track_ht_oq=getOfflineTrack(TES,track_ht)

    ## find reason of Trigger in both cases
    reasons.append(tr.reasonOfTrigger(TES,track_pt_oq,single_di="di"))
    reasons.append(ca.reasonOfTriggerCF(TES,track_ht_oq))

    ## if both tracks are physics
    if reasons.count("phys")==2:
        ## if they come from same eve, return phys
        if ca.same_eve(TES,track_pt,track_ht): return "phys"
        ## if not,
        else: return "random_vertex"
    ## if one of the tracks is not physics
    return "no_phys_other"


    
