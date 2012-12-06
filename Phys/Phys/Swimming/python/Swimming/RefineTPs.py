from copy import copy
from pprint import pprint
__all__ = ["refine"]

#The recursive refinement of the turning points
def refine(myGlobs, mycand, thisturn, lastGranularity, numberOfRefinements) :
    #
    from SwimmingUtils  import getBinfo, runSwimmingStep
    from SwimmingTisTos import evaluateTisTos

    # This algorithm recursively refines the turning points searching
    # around the current best estimate of the TP position. Since there
    # exists a preferred direction (we swim from extremely positive to 
    # extremely negative lifetimes), we only need to search on one side
    # of the TP. If we know that at 40mm the decision was 0 and at 44mm
    # it was 1, the refinement searches between 40 and 44. If you then
    # find that 41.2mm was 0 and 41.6mm was 1 you search between these 
    # two, and so on. 
    HltDecLast = copy(thisturn[4])
    refinedTurningPoints = []
    # How far to swim
    newGranularity = lastGranularity / myGlobs.granularityRefinement
    # Where to start?
    swimPoint = thisturn[0] - lastGranularity + newGranularity 
    while (swimPoint < (thisturn[0] + newGranularity)) :
        runSwimmingStep(myGlobs, mycand, swimPoint)
        HltDec = evaluateTisTos(myGlobs, mycand,swimPoint)
        if HltDec != HltDecLast:
            bParams = getBinfo(myGlobs, mycand)
            refinedTurningPoints.append([swimPoint, bParams[0], bParams[1], HltDec, HltDecLast])
            HltDecLast = copy(HltDec)
        swimPoint += newGranularity
    if refinedTurningPoints == []:
        refinedTurningPoints.append(thisturn)
    #We finished this refinement, a little printout never hurt
    if myGlobs.DEBUGMODE :
        print "****************************"
        print "The refined turning points are"
        pprint(refinedTurningPoints)
        print "****************************"
    numberOfRefinements -= 1
    if numberOfRefinements == 0:
        # We're done and at the end of the recursion, so we split the last
        # interval in 2 and have one extra calculation of candidate properties
        # used to update the turning point. The TisTos information is kept.
        for turn in refinedTurningPoints:
            swimPoint = turn[0] + 0.5 * newGranularity
            if myGlobs.DEBUGMODE:
                print "final splitting of the interval which goes from %s to %s" % (turn[0], swimPoint)
            runSwimmingStep(myGlobs, mycand, swimPoint)
            bParams = getBinfo(myGlobs, mycand)
            turn[0] = swimPoint
            turn[1] = bParams[0]
            turn[2] = bParams[1]
        if myGlobs.DEBUGMODE :
            print "****************************"
            print "The final refined turning point is:"
            pprint(refinedTurningPoints)
            print "****************************"
        return refinedTurningPoints
    else :
        #Need to refine every new turning point which we find
        toreturn = []
        for turn in refinedTurningPoints :
            toreturn += refine(myGlobs, mycand, turn, newGranularity, numberOfRefinements)
        return toreturn
