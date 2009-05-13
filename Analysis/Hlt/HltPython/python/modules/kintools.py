# =============================================================================
""" @namespace kintools
@brief Functions that deal with kinematics of particles, mcparticles or tracks
@date 2007-11-28
"""
# =============================================================================


from math import *
import GaudiPython

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
def dotLists(list1,list2):
    """ Returns dot product of two python lists
    @param  list1,list2 Input lists
    @returns dot Dot product
    @author Antonio Perez-Calero Izquierdo aperez@ecm.ub.es
    """
    if len(list1)!=len(list2):
        print "Vectors must be of equal dimension"
        return
    else:
        dot=0.
        for i in range(len(list1)):
            dot+=list1[i]*list2[i]

        return dot


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
    return sqrt(vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2])



#---------------------------------------------------
def pt( a ):
    """ Returns the pt of a particle or MCparticle or track  or a momentum list.
    @param a A particle or MCparticle or track or a momentum vector
    @return The transverse momentum
    @author Hugo Ruiz, hugo.ruiz@cern.ch"""
    vect = momList(a)
    return sqrt(vect[0]*vect[0]+vect[1]*vect[1])


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
        angleAB=acos(dot(a,b)/(mag(a)*mag(b)))
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
def dimuMass(track1,track2):
    """ Returns the invariant mass of two tracks, given muon mass hypothesis 
    @param track1, track2 input tracks
    @returns invariant mass (MeV) 
    @author Antonio Perez Calero, aperez@ecm.ub.es
    """
    mass=105.65840148924789
    mom1 = track1.momentum()
    mom2 = track2.momentum()
    a={"tx":mom1.x()/mom1.z(),"ty":mom1.y()/mom1.z(),"P":track1.p(),"E":sqrt(mass*mass + track1.p()*track1.p())}
    b={"tx":mom2.x()/mom2.z(),"ty":mom2.y()/mom2.z(),"P":track2.p(),"E":sqrt(mass*mass + track2.p()*track2.p())}
    dotdir=a["tx"]*b["tx"] + a["ty"]*b["ty"] + 1
    moda = sqrt(a["tx"]*a["tx"] + a["ty"]*a["ty"] + 1)
    modb = sqrt(b["tx"]*b["tx"] + b["ty"]*b["ty"] + 1)
    InvMass = sqrt(2*mass*mass +2*a["E"]*b["E"] - 2*a["P"]*b["P"]*(dotdir/(moda*modb)))
    return InvMass


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


#---------------------------------------------------
def Pointing(part,PV):
    """ Returns the pointing of a reconstructed particle with respect to a vertex, as used in the trigger.
    Kind of a normalized transverse momentum with respect to the flight displacement, transformed to lay within the range [0, 1].
    @param part, initial reconstructed particle
    @param PV, Primary vertex from which pointing is calculated
    @returns pointing value
    @author Antonio Perez Calero, aperez@ecm.ub.es
    """
    endV = part.endVertex()
    endPos = endV.position()
    pvPos = PV.position()
    disp = [endPos.x()-pvPos.x() , endPos.y()-pvPos.y() , endPos.z()-pvPos.z()]
    
    daugs  = endV.outgoingParticlesVector()

    momTot = GaudiPython.gbl.Gaudi.LorentzVector()
    for dau in daugs: momTot+=dau.momentum()
    momB = momList(momTot)
    cosB = dotLists(momB,disp)/(sqrt(dotLists(momB,momB))*sqrt(dotLists(disp,disp)))
    sinB = sqrt(1-cosB*cosB)
    momBsinB = part.p()*sinB

    Sum=0
    for dau in daugs:
        mom_dau = momList(dau.momentum())
        cos = dotLists(mom_dau,disp)/(sqrt(dotLists(mom_dau,mom_dau))*sqrt(dotLists(disp,disp)))
        sin = sqrt(1-cos*cos)
        Sum += dau.p()*sin

    try:
        pointing = momBsinB/(Sum+momBsinB)
    except:
        print "Divide by zero"
        pointing = 1

    return pointing
