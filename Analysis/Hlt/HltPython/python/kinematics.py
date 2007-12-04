#  @date 2007-11-28
#  @author Hugo RUIZ hugo.ruiz@cern.ch
# =============================================================================
""" Simple shortcuts and kinematical calculations"""
# =============================================================================
__author__ = "Hugo RUIZ hugo.ruiz@cern.ch"
# =============================================================================


import math

#---------------------------------------------------
def momVect( a ):
    """ Converts the momentum of a particle or MCparticle or track or a momentum vector into a list.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""
    try:
        return [a.momentum().x(), a.momentum().y(), a.momentum().z()]
    
    except:
        return [a.x(),a.y(),a.z()]

#---------------------------------------------------
def mag( a ):
    """ Returns the modulus of the momentum of a particle or track  or MCparticle or a momentum vector.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""
    try:
        vect = momVect(a)
    except:
        vect = a
    return math.sqrt(vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2])



#---------------------------------------------------
def pt( a ):
    """ Returns the pt of a particle or MCparticle or track  or a momentum vector.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""
    vect = momVect(a)
    return math.sqrt(vect[0]*vect[0]+vect[1]*vect[1])


#---------------------------------------------------
def dot(a,b):
    """ Returns the dot product of two momentums of particles or MCparticles or tracks or momentum vectors.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""
    vectA = momVect(a)
    vectB = momVect(b)
    return vectA[0]*vectB[0]+vectA[1]*vectB[1]+vectA[2]*vectB[2]


#---------------------------------------------------
def angle(a,b):
    """ Returns the angle between particles or MCparticles or tracks or momentum vectors.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""
    if DEBUG:
        print a
        print b
        print dot(a,b)
        print mag(a)*mag(b)
    return acos(dot(a,b)/(mag(a)*mag(b)))



#---------------------------------------------------
def invMass (partList):
    """ Returns the invariant mass of a set of particles or MC particles.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch
    """
    mom = partList[0].momentum()
    for iPart in partList[1:]:
        mom += iPart.momentum()
    return mom.mass()


#---------------------------------------------------
def angularMatch( refPart, candidateList, angleMax = 0.001): 
    """ Returns the sublist of a list of candidates with angular match to a reference particle or MCParticle or track.
    angleMax is the maximum angle to consider that objects are matched.
    candidateList can be just one object instead of a list.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""

    # Convert candidateList to a list if it is not
    if not candidateList: return []
    try:
        a = candidateList[0]
    except:
        candidateList = [candidateList]

    # Match
    matchedCandList = []
    for iCand in candidateList:
        try:
            iAngle =  angle(refPart,iCand)
        except:
            print 'angularMatch: ANGLE COMPUTATION FAILED!!!'
            raise RuntimeError
        if iAngle < angleMax:
            matchedCandList.append(iCand)
            iCand.angle = iAngle

    return matchedCandList

 
#---------------------------------------------------
def samePart (iDec, jDec):
    """ Returns whether two particles have the same p and pid.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""
    return iDec.momentum()==jDec.momentum() and iDec.particleID()==jDec.particleID()


#---------------------------------------------------
def getIPError( pt ):
    """ Returns IP uncertainty from the parameterization published in LHCb-2005-012.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""
    x = fabs(1/pt)
    return 0.0223869 - (0.00424545*x)+ (0.0175535*x**2) -(0.005477*x**3)+ (0.00104437*x**4) -(0.000104878*x**5)+ (0.00000418932*x**6)
