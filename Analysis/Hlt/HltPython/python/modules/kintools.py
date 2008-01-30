# =============================================================================
""" @namespace kintools
@brief Functions that deal with kinematics of particles, mcparticles or tracks
@date 2007-11-28
"""
# =============================================================================


import math
DEBUG=False
#---------------------------------------------------
def momList( a ):
    """ Converts the momentum of a particle or MCparticle or track or a momentum vector into a list [px, py, pz].
    @param a A particle or MCparticle or track or a momentum vector
    @return [px, py, pz]
    @author Hugo Ruiz, hugo.ruiz@cern.ch"""
    try:
        return [a.momentum().x(), a.momentum().y(), a.momentum().z()]
    
    except:
        return [a.x(),a.y(),a.z()]

#---------------------------------------------------
def mag( a ):
    """ Returns the modulus of the momentum of a particle or track  or MCparticle or a momentum list.
    @param a A particle or MCparticle or track or a momentum vector
    @return The modulus of the 3-vector
    @author Hugo Ruiz, hugo.ruiz@cern.ch"""
    try:
        vect = momList(a)
    except:
        vect = a
    return math.sqrt(vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2])



#---------------------------------------------------
def pt( a ):
    """ Returns the pt of a particle or MCparticle or track  or a momentum list.
    @param a A particle or MCparticle or track or a momentum vector
    @return The transverse momentum
    @author Hugo Ruiz, hugo.ruiz@cern.ch"""
    vect = momList(a)
    return math.sqrt(vect[0]*vect[0]+vect[1]*vect[1])


#---------------------------------------------------
def dot(a,b):
    """ Returns the dot product of two momentums of particles or MCparticles or tracks or momentum vectors.
    @param a A particle or MCparticle or track or a momentum vector
    @param b Another particle or MCparticle or track or a momentum vector
    @return The dot product of momenta
    @author Hugo Ruiz, hugo.ruiz@cern.ch"""
    vectA = momList(a)
    vectB = momList(b)
    return vectA[0]*vectB[0]+vectA[1]*vectB[1]+vectA[2]*vectB[2]


#---------------------------------------------------
def angle(a,b):
    """ Returns the angle between particles or MCparticles or tracks or momentum vectors.
    @param a A particle or MCparticle or track or a momentum vector
    @param b Another particle or MCparticle or track or a momentum vector
    @return The angle between the momenta
    @author Hugo Ruiz, hugo.ruiz@cern.ch
    @author Antonio Perez-Calero, aperez@ecm.ub.es"""
    #DEBUG=1
    if mag(a)*mag(b)!=0:
        if DEBUG: print 'Calculating angle'
        angleAB=math.acos(dot(a,b)/(mag(a)*mag(b)))
        if DEBUG: print 'Calculated angle',angleAB
        return angleAB
    else:
        print 'Could not compute angle, divide by zero! Angle value set to 1000'
        angleAB=1000
        if DEBUG:
            #print a
            #print b
            #print dot(a,b)
            print mag(a)*mag(b)
        return angleAB


#---------------------------------------------------
def invMass (partList):
    """ Returns the invariant mass of a set of particles or MC particles.
    @param partList A list of particles or MC particles
    @return The invariant mass of the system
    @author Hugo Ruiz, hugo.ruiz@cern.ch
    """
    mom = partList[0].momentum()
    for iPart in partList[1:]:
        mom += iPart.momentum()
    return mom.mass()


#---------------------------------------------------
def angularMatch( refPart, candidateList, angleMax = 0.001): 
    """ Returns the sublist of a list of candidates with angular match to a reference particle or MCParticle or track.
    @param refPart Particle (or MCParticle) to be matched
    @param candidateList List of candidate particles (or MCParticles). Can be just one object instead of a list.
    @param angleMax The maximum angle to consider that objects are matched.
    @returns List of matching candidates
    @author Hugo Ruiz, hugo.ruiz@cern.ch"""

    # Convert candidateList to a list if it is not
    #if len(candidateList)==0: return []
    #if not candidateList: return []
    try:
        a = candidateList[0]
        #print 'Reading list'
    except:
        candidateList = [candidateList]

    # Match
    matchedCandList = []
    for iCand in candidateList:
        try:
            #print 'calling angle calculation'
            iAngle =  angle(refPart,iCand)
            #print 'angle',iAngle
        except:
            print 'angularMatch: ANGLE COMPUTATION FAILED!!!'
            if DEBUG:
                print 'dot',dot(refPart,iCand),'mod product',mag(refPart)*mag(iCand),'cosine',dot(refPart,iCand)/(mag(refPart)*mag(iCand))
            raise RuntimeError
        if iAngle < angleMax:
            matchedCandList.append(iCand)
            #iCand.angle = iAngle

    return matchedCandList

 
#---------------------------------------------------
def samePart (iDec, jDec):
    """ Returns whether two particles have the same p and pid.
    @param iDec One particle or MCParticle
    @param jDec Another particle or MCParticle
    @returns A bool indicating whether they exactly match in momentum and particle id
    @author Hugo Ruiz, hugo.ruiz@cern.ch"""
    return iDec.momentum()==jDec.momentum() and iDec.particleID()==jDec.particleID()


