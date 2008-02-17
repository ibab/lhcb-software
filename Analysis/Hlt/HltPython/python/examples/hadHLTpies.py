
# =============================================================================
"""
@namespace hadHLTpies
@brief Build pies and extract information of events triggering Hadronic HLT. Includes functions to test inputs (check if job was correctly configured), two functions to classify tracks triggering HadPreTriggerSingle depending on ghosts' category and reason of trigger, and another to do the same with vertices from HadTrigger. There's one extra function to test module including one job ready to be used and return some info in the form of plots.
@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
@date 2008-\05-02
"""
# =============================================================================

import gaudimodule

import array
import math
import copy

import causeTools as ca
import addDict as add
import tracksAndVerticesClassifyTools as trve
import drawPiesAndHistos as dph

DEBUG=False

#---------------------------------------------------


def checkGaudiModule(gaudimod):
    """Check if gaudimodule was properly stablished
    @param gaudimod Configuration of the analysis (DATACARDS and extra opts).
    @returns
    gaudimod if its correcty stablished
    HLTSUM factory to access different informaton
    TES transient event store
    @author Xabier Cid Vidal, xabier.cid.vidal@cern.ch
    """
    
    ##No gaudimod input
    if gaudimod==0:

        print "\n ERROR: Please input your gaudimodule.AppMgr()"
        return "wgm",None,None
           
    else:
        gaudi=gaudimod

    try:
        ## gaudi mod correctly configured. Build TES and HLTSUM
        gaudi.initialize()

        TES = gaudi.evtsvc()

        HLTSUM = gaudi.toolsvc().create("HltSummaryTool",
                                        interface="IHltConfSummaryTool")
            
    except:
            print "\n\n      WRONG OPTIONS FOR GAUDIMODULE, PLEASE CHECK THEM"
            return "wgm",None,None
            
    return gaudi,HLTSUM,TES


def classifyHPTtracksByGhosts(gaudimod=0,N=100000000,g_mode=False):

    """Classify tracks triggering in HLT according to the presence of ghosts. Need to input:

    @param gaudimod Configuration of the analysis (DATACARDS and extra opts).
    @param N Number of events you want to check. Default all.
    @param g_mode Boolean. Return or not Pie with information. Default True

    @returns numeric information and, if 'g_mode', also TPie with percentage of 111,110,10,01,00 tracks, where: 111 is 70% of hits in VELO from MCP1,70% of hits in TS MPC1 (same)/ 110 is 70% of hits in VELO from MCP1, 70% of hits in TS MPC2 (different)/ 01 is ghost in VELO (less than 70 % of hits from same MCP), 70% of hits in TS from MCP1/ 10  is 70% of hits in VELO from MCP1, ghost in TS (less than 70 % of hits from same MCP)/ 00 is ghost in VELO (less than 70 % of hits from same MCP), ghost in TS (less than 70 % of hits from same MCP).

        Also prints same data with uncertainties.

    @author Xabier Cid Vidal, xabier.cid.vidal@cern.ch
    """


    TRIG="HadPreTriggerSingle"
    
    ## check if gaudimodule was properly stablished
    gaudi,HLTSUM,TES=checkGaudiModule(gaudimod)

    if gaudi=="wgm":
            if g_mode: return None,None
            else: return None


    ## find for each event tracks and classify them
    def run(n,trig):
            gaudi.evtsel().rewind()
            types={}
            ## initialize output dictionary
            types["tracks"]=0
            types["events"]=0
            if DEBUG: types["anomaly"]=0

            for i in range(n):
                
                    ## run event 
                    gaudi.run(1)
                    try: sels = HLTSUM.selections()
                    except: break

                    if trig in sels:

                            ## count events of a certain track
                            types["events"]+=1

                            ## find tracks
                            tracks=HLTSUM.selectionTracks(trig)


                            if len(tracks) == 0 and DEBUG: types["anomaly"]+=1

                            ## classify them and add it to output dictionary
                            for track in tracks:

                                    add.add_elto_dic(types,trve.classifyTrackByGhosts(TES,track))
                                    ## count tracks number
                                    types["tracks"]+=1


            return types


    out=run(N,TRIG)

    labels=["111","110","01","10","00"]

    condl=True

    ## if there are no tracks, return nothing
    if out["tracks"]==0:
        print "NO TRACKS IN SELECTION"
        if g_mode: return None,None
        return None



    ## if one type is not in counter, add it as 0
    for l in labels:
            if not out.has_key(l):
                    out[l]=0

    print "\n\n OUT=",out
    print "\n\n"

    out_copy=copy.copy(out)        

    ## if not g_mode return only dict
    if not g_mode: return out_copy

    #else, draw Pie and return both
    tracks=out.pop("tracks")
    events=out.pop("events")
    pie=dph.drawPieFromDict(out,title="Percentage of each kind in "+str(tracks)+" tracks for "+TRIG,sort=True)
    return out_copy,pie
    

#---------------------------------------------------


def getInformationFromHPTtracks(gaudimod=0,classify_ghosts=[],intruders=[],holes=[],g_mode=False,N=100000000):

    """
    Build pies with information about forward tracks triggering in HadPreTrigger Single. 

    @param gaudimod Configuration of the analysis (DATACARDS and extra opts).
    @param classify_ghosts (string): 111,110,10,01,00; where 111 is 70% of hits in VELO from MCP1,70% of hits in TS MPC1 (same)/ 110 is 70% of hits in VELO from MCP1, 70% of hits in TS MPC2 (different)/ 01 is ghost in VELO (less than 70 % of hits from same MCP), 70% of hits in TS from MCP1/ 10  is 70% of hits in VELO from MCP1, ghost in TS (less than 70 % of hits from same MCP)/ 00 is ghost in VELO (less than 70 % of hits from same MCP), ghost in TS (less than 70 % of hits from same MCP).
    @param Intruders (boolean): 8 first mcpar hits are 8 first used in track reconstruction  
    @param Holes (boolean): track has hits amongst 8 first from different mcpars
    @param g_mode (boolean) Graphical mode. If true returns three groups of pies. First histogram of delta p. Second dict with info of physics tracks when they don't come from b or c (histogram of decay radio, histogram of decay z and pie with most popular eves). Third list of main pies. Default false
    @param N (int) Number of events to be analysed. Default all

    @returns

      out_deltap: List with entries of delta p over p.

      out_nbc: Dict with info about physics tracks not coming from b or c
                   - list with decay radio of eves
                   - list with decay z of eves
                   - dict with eves and number of times they appeared

      out_pies: 5 dictionaries with info of cause of triggering (number of tracks of each type):
                   - First pie: Physics tracks percentage
                            - ghost:    Has less than 70% of hits from same MC particle
                            - phys:     Comes from a MCparticle which accomplishes the trigger conditions
                            - non_phys: Comes from a MCparticle not accomplishing them

                   - Second pie: Origin of physics tracks
                            - Comes from a B
                            - Comes from a C
                            - Doesn't come frome any of them

                   - Third pie: Non physics, why?
                            - pt: Comes from a MCparticle with pt<2.4 GeV to which reconstruction has given a pt bigger than that
                            - ip: Comes from a MCparticle with IP<0.1 mm  to which reconstruction has given an IP bigger than that

                   - Fourth pie: What has given the track an ip
                            - vertex: IP has been given due to a wrong vertex used in calculation
                            - resol:  IP has been given due to some element resolution

                   - Fifth pie: Element whose resolution has caused wrong IP (and therefore trigger)
                            - IP_slope:       Bad slope resolution
                            - IP_vertex:      Bad vertex resolution
                            - IP_both:        Both elements resolution
                            - IP_combination: Combination of both elements

      If  'g_mode' also returns one histogram, one dictionary and one list with the same info in the form of histograms and pies. Particularly plots_nbc['eves'] is a list where first element is a pie an second another list keeping a legend for this pie and boxes used in this legend. Just need to type plots_nbc['eves'][0].Draw() and plots_nbc['eves'][1][0].Draw() to see everything

    @author Xabier Cid Vidal, xabier.cid.vidal@cern.ch
    """

    TRIG="HadPreTriggerSingle"


    ## check if gaudimodule was properly stablished    
    gaudi,HLTSUM,TES=checkGaudiModule(gaudimod)

    if gaudi=="wgm":
        if g_mode: return None,None,None,None,None,None
        return None,None,None


    det = gaudi.detsvc()
    VELO = det['/dd/Structure/LHCb/BeforeMagnetRegion/Velo']


    j=1

    ## check which conditions are imposed
    searchg=False
    searchi=False
    searchh=False


    if not isinstance(classify_ghosts,list): searchg=True

    if not isinstance(intruders,list): searchi=True

    if not isinstance(holes,list): searchh=True

    ## build output
    tracks_numb=0
    out_pies=[]
    for i in range(5):
            out_pies.append({})

    out_nbc={"eves":{},"decay_radio":[],"decay_z":[]}
    out_deltap=[]

    ## run the N events
    for i in range(N):

            gaudi.run(1)
            if DEBUG: print "\n  EVENT NUMB=",j
            try: sels = HLTSUM.selections()
            except:
                    break
            ## make sure our event triggers our TRIG
            if TRIG in sels:

                    tracks = HLTSUM.selectionTracks(TRIG)

                    for track in tracks:

                            condg=True
                            condi=True
                            condh=True

                            ## check if each imposed condition is accomplished
                            if searchg:
                                condg=False
                                if classify_ghosts==trve.classifyTrackByGhosts(TES,track): condg=True

                            if searchi:
                                condi=False
                                if intruders==trve.hasIntruders(TES,track): condi=True

                            if searchh:
                                condh=False
                                if holes==trve.hasHoles(TES,track,VELO): condh=True

                            ## If so
                            if condg and condi and condh:

                                    tracks_numb+=1

                                    reason=trve.reasonOfTrigger(TES,track)

                                    ca.fill_deltap(TES,track,out_deltap)

                                    ## first pie: ghost/physics/no physics
                                    if reason=="ghost":
                                            add.add_elto_dic(out_pies[0],reason)


                                    elif reason=="phys":
                                            add.add_elto_dic(out_pies[0],reason)
                                            ## second pie: physics tracks origin b/c/neither b nor c
                                            add.add_elto_dic(out_pies[1],ca.origin_quark(TES,track))
                                            ## fill information about non b or c physics tracks
                                            ca.non_b_or_c_ph(TES,track,out_nbc["eves"],out_nbc["decay_radio"],out_nbc["decay_z"])

                                    else:
                                            add.add_elto_dic(out_pies[0],"non_phys")
                                            ## third pie: ip or pt cause of wrong triggering
                                            if reason=="pt":
                                                    add.add_elto_dic(out_pies[2],reason)
                                            else:
                                                    add.add_elto_dic(out_pies[2],"ip")
                                                    ## fourth pie: ip given because of misuse of vertex or elements resolution
                                                    if reason=="vertex":
                                                            add.add_elto_dic(out_pies[3],reason)
                                                    else:
                                                            add.add_elto_dic(out_pies[3],"ip_resol")
                                                            ## fifth pie: element whose resolution caused trigger (slope, vertex, combination or both)
                                                            add.add_elto_dic(out_pies[4],reason)


            j+=1


    ## If there are no tracks return Nothing
    if tracks_numb==0:
        print "THERE ARE NO TRACKS AVALIABLE IN YOUR SELECTION"
        if g_mode: return None,None,None,None,None,None
        else: return None,None,None



    ## if not g_mode, only return data
    if not g_mode:
            return out_pies,out_nbc,out_deltap


    ## build names
    name_tracks=str(tracks_numb)+" forward tracks from "+TRIG
    name_tracks_s="forward tracks from "+TRIG

    #################################################################
    ## BUILD HISTOGRAM OF DELTA P OVER P ##

    hist_deltap=dph.histFromList(out_deltap,title="Resolution in momentum using "+name_tracks,name="\Deltap/p",nbins=100,fbin=-0.5,lbin=0.5,xtitle="\Deltap/p")

    
    ##############################################################################
    ## BUILD HISTOGRAMS DECAY POS ##

    plots_nbc={}
    plots_nbc["decay_radio"]=dph.histFromList(out_nbc["decay_radio"],title="Radio of decay of non B or C eves using "+name_tracks,name="Radio of decay",nbins=100,fbin=0,lbin=20,xtitle="Radio of Decay (mm)")
    plots_nbc["decay_z"]=dph.histFromList(out_nbc["decay_z"],title="Z position in decay of non B or C eves  using "+name_tracks,name="Position of decay",nbins=100,fbin=-300,lbin=1000,xtitle="Z Position of Decay (mm)")


    ## BUILD PIE OF NON B OR C EVES
    
    if len(out_nbc["eves"])>8: hm=8
    else: hm=len(out_nbc["eves"])
    
    pie_e,leg_e=dph.drawPieLessElements(out_nbc["eves"],els=hm,title=" Eves from Physics forward tracks from "+TRIG+" not having B or C quarks",opt="perc",leg=True)

    plots_nbc["eves"]=[pie_e,leg_e]


    #######################################################################3
    ## BUILD A LIST WITH FIVE MAIN PIES ##

    labels=[]
    
    labels.append(["phys","non_phys","ghost"])
    labels.append(["isB","isC","others"])
    labels.append(["pt","ip"])
    labels.append(["vertex","ip_resol"])
    labels.append(["IP_vertex","IP_slope","IP_both","IP_combination"])

    ## If one of the labels is not there, add its value as 0. Calculate total numbers for percentages
    for i in range(5):
            total=0
            for el in labels[i]:
                    if not out_pies[i].has_key(el): out_pies[i][el]=0

    ## Define new labels
    labelsnew_p=[]
    labelsnew_p.append(["Physics","Non physics","Ghosts"])
    labelsnew_p.append(["Has B","Has C","#splitline{Does not have}{neither B nor C}"])
    labelsnew_p.append(["We can blame RC pt","We can blame RC IP"])
    labelsnew_p.append(["#splitline{We have used a}{wrong vertex}","Resolution effect"])
    labelsnew_p.append(["Resolution of vertices","Slopes resolution","Resolution of both","Combination of both"])

    ## Build dict for drawPieFromDict
    labelsnew=[]
    for i in range(5):
        labelsnew.append({})
        j=0
        for el in labels[i]:
            labelsnew[i][el]=labelsnew_p[i][j]
            j+=1

    ## Define titles of pies
    titles_o=[]
    titles_o.append("Percentage of physics and non physics in ")
    titles_o.append("Quarks in eves of physics ")
    titles_o.append("Cause of triggering in non physics ")
    titles_o.append("Check who gave an IP to the non physics ")
    titles_o.append("Elements whose resolution gave an IP to the non physics ")

    titles=[]

    for t in titles_o:
            titles.append(t+name_tracks_s)


    ## Build TPies
    pies=[]
    for i in range(5):
        pies.append(dph.drawPieFromDict(out_pies[i],title=titles[i],labels=labelsnew[i],sort=True))

    ##########################################################################################

    ## RETURN

    return out_pies,out_nbc,out_deltap,pies,plots_nbc,hist_deltap


#---------------------------------------------------


def getInformationFromHTvertices(gaudimod=0,intruders=[],holes=[],g_mode=False,N=100000000):

    """
    Build pies with information about vertices triggering in HadTrigger. 

    @param gaudimod Configuration of the analysis (DATACARDS and extra opts).

    @param Intruders (boolean): 8 first mcpar hits are 8 first used in track reconstruction (for tracks linked to vertices)  
    @param Holes (boolean): track has hits amongst 8 first from different mcpars (for tracks linked to vertices)
    @param g_mode (boolean) Graphical mode. If true returns a list of 4 pies. First, percentage of ghost and no ghost vertices. Second, percentage of physics and no physics no ghost vertices. Third, percentage of physics vertices coming from B and C. Fourth, percentage of non physics vertices types. Default false
    @param N (int) Number of events to be analysed. Default all

    @returns list of dictionaries with counters of each entry:

           - First: Ghost and no ghost vertices percentage
                    - ghost:    All tracks linked to vertex are '111'
                    - no_ghost: There's at least one track linked to vertex non '111'

           - Second: Physics and no physics vertices percentage
                    - physics: All tracks linked to vertex are physics and come from same eve.
                    - non_physics: Previous conditions are not fulfilled

           - Third: Origin of physics vertices
                    - Comes from a B
                    - Comes from a C
                    - Doesn't come frome any of them

           - Fourth: Non physics, why?
                    - random_vertex: all tracks linked to vertex are physics but they don't come from same MC particle.
                    - no_phys_other: there is at least one track linked to vertex being non physics

      To see explanation of types of tracks mentioned here type help(trackPies) in tracks_HPT_pies.
      If  'g_mode' also returns 4 pies.

    @author Xabier Cid Vidal, xabier.cid.vidal@cern.ch
    """

    TRIG="HadTrigger"


    ## check if gaudimodule was properly stablished
    gaudi,HLTSUM,TES=checkGaudiModule(gaudimod)

    if gaudi=="wgm":
        if g_mode: return None,None
        return None


    det = gaudi.detsvc()
    VELO = det['/dd/Structure/LHCb/BeforeMagnetRegion/Velo']

    j=1

    ## check which conditions are imposed
    searchi=False
    searchh=False

    if not isinstance(intruders,list): searchi=True

    if not isinstance(holes,list): searchh=True

    ## build output
    vert_numb=0
    out_pies=[]
    for i in range(4):
            out_pies.append({})


    ## run the N events
    for i in range(N):

            gaudi.run(1)
            if DEBUG: print "\n  EVENT NUMB=",j
            try: sels = HLTSUM.selections()
            except:
                    break
            ## make sure our event triggers our TRIG
            if TRIG in sels:
                    ## find vertices triggering HadTrigger
                    vertices=trve.findHTvertices(HLTSUM)

                    for vert in vertices:

                            condi=True
                            condh=True

                            ## check if each imposed condition is accomplished

                            if searchi:
                                condi=False
                                condi_l=[]
                                for track in vert.tracks():
                                        condi_l.append(trve.hasIntruders(TES,track))
                                if intruders and (True in condi_l): condi=True
                                if not intruders and not (True in condi_l): condi=True

                            if searchh:
                                condh=False
                                condh_l=[]
                                for track in vert.tracks():
                                        condh_l.append(trve.hasHoles(TES,track,VELO))
                                if holes and (True in condh_l): condh=True
                                if not holes and not (True in condh_l): condh=True


                            ## If so
                            if condi and condh:

                                    vert_numb+=1

                                    reason=trve.classifyVertexByReasonOfTrigger(TES,vert)

                                    ## first pie: ghost/no_ghost
                                    if reason=="ghost":
                                            add.add_elto_dic(out_pies[0],reason)

                                    else:
                                            add.add_elto_dic(out_pies[0],"no_ghost")
                                            ## second pie: physics/no_physics
                                            if reason=="phys":
                                                    add.add_elto_dic(out_pies[1],reason)
                                                    ## third pie: physics tracks origin b/c/neither b nor c
                                                    add.add_elto_dic(out_pies[2],ca.origin_quark(TES,vert.tracks()[0]))

                                            else:
                                                    add.add_elto_dic(out_pies[1],"non_phys")
                                                    ## fourth pie: random vertex or some non physics track
                                                    add.add_elto_dic(out_pies[3],reason)


            j+=1


    ## If there are no vertices return Nothing
    if vert_numb==0:

            print "THERE ARE NO VERTICES AVALIABLE IN YOUR SELECTION"
            if g_mode: return None,None
            else: return None



    ## if not g_mode, only return data
    if not g_mode:
            return out_pies


    #######################################################################3
    ## BUILD A LIST WITH 4 MAIN PIES
    piesn=[2,2,3,2]

    labels=[]
    

    labels.append(["ghost","no_ghost"])
    labels.append(["phys","non_phys"])
    labels.append(["isB","isC","others"])
    labels.append(["random_vertex","no_phys_other"])

    ## If one of the labels is not there, add its value as 0. Calculate total numbers for percentages
    for i in range(4):
            for el in labels[i]:
                    if not out_pies[i].has_key(el): out_pies[i][el]=0
                    
    ## Define new labels
    labelsnew_p=[]

    labelsnew_p.append(["Ghost","No ghost"])
    labelsnew_p.append(["Physics","No physics"])
    labelsnew_p.append(["Has B","Has C","#splitline{Does not have}{neither B nor C}"])
    labelsnew_p.append(["#splitline{Tracks in vertex}{come from different eves}","No physics tracks"])


    ## Build dict for drawPieFromDict
    labelsnew=[]
    for i in range(4):
        labelsnew.append({})
        j=0
        for el in labels[i]:
            labelsnew[i][el]=labelsnew_p[i][j]
            j+=1

    
    ## Define titles of pies
    titles_o=[]
    titles_o.append("Percentage of ghost and no ghost ")
    titles_o.append("Cause of triggering in non physics ")
    titles_o.append("Quarks in eves of physics ")
    titles_o.append("Types of non physics ")

    titles=[]

    for t in titles_o:
            titles.append(t+"vertices from HadTrigger")


    
    ## Build TPies
    pies=[]
    for i in range(4):
        pies.append(dph.drawPieFromDict(out_pies[i],title=titles[i],labels=labelsnew[i],sort=True))

    ##########################################################################################
    ## RETURN

    return out_pies,pies


def testForHadHLTpies():
    	"""
        Test for functions in hadHLTpies. Prepares a job to run with a set of events which fired HadPreTriggerSingle. Configuration includes Datacards, HltJob and HltMC options. 
	@returns Prints information about ghosts' and 'reason of trigger' classification of HadTriggerSingle tracks and the same for HadTrigger vertices. Returns one pie and two other lists of pies with same information. For further reading in categories do help for the other functions in this module.
	@author Xabier Cid Vidal, xabier.cid.vidal@cern.ch
	"""

	## set physical location for different options
	HOMEDIR = "/afs/cern.ch/user/j/jcidvida/cmtuser/DaVinci_v19r5"

	## options for HLT
	HLTJOB    = HOMEDIR+"/opts/HltJob.opts"
	## datacards to be read (min bias events)
	DATACARDS = HOMEDIR+"/opts/diesti/dstPT_long.opts"
	## options to have access to montecarlo information
	UNPACK    = HOMEDIR+"/opts/UnpackMC.opts"
	MCOPTS = HOMEDIR+"/opts/HltMC.opts"

	## prepare job configuration
	FILES=[HLTJOB,DATACARDS,MCOPTS,UNPACK]
	EOPTS=['HltSummaryWriter.SaveAll = true', 'HltSelectionDataToTes.CopyAll = true', 'HistogramPersistencySvc.OutputFile = "freq_and_bperc.root"']

	## configure job to make it ready to quarksAndFrequencyInfo
	gaudi = gaudimodule.AppMgr(outputlevel=3)
	gaudi.config(files = FILES,options=EOPTS)

        ## find percentage of ghosts tracks triggering HadPreTriggerSingle
        count1,pie_tracks_ghosts=classifyHPTtracksByGhosts(gaudi,N=500,g_mode=True)

        ## find all plots related to reason of triger for tracks triggering HadPreTriggerSingle
        pies_info,nbc,deltap,pies_tracks_reason,h_nbc,h_dp=getInformationFromHPTtracks(gaudi,classify_ghosts="111",g_mode=True,N=500)

        ## find all plots related to ghosts and reason of trigger for vertices in HadTrigger
        count2,pies_vertices=getInformationFromHTvertices(gaudi,g_mode=True,N=1000)


        ## print some information
        print "\nclassify HPT tracks by ghosts\n",count1
        print "\nclassify HPT tracks by reason of trigger\n",pies_info
        print "\nclassify HT vertices by ghosts and reasons of trigger\n",count2

        ## return 3 groups of pies only with most relevant information
        return pie_tracks_ghosts,pies_tracks_reason,pies_vertices


