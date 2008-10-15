# =============================================================================
"""
@namespace holesIntrudersTools
@brief Low level module. Contains functions to deal with intruders and holes. Examples are functions to find which sensors has hit a mcpar or if there's at least one particle different in a certain set of them. Used in tracksAndVerticesClassifyTools.py
@author Xabier Cid Vidal xabier.cid.vidal@cern.ch
@date 2008-05-02
"""
# =============================================================================


import GaudiPython
import ghostsClassifyTools as ghc

DEBUG=False
LHCbID        = GaudiPython.gbl.LHCb.LHCbID

#---------------------------------------------------

def intruder(mcpars8,mcp8):
    """find if all the hits correspond to the same mcparticle
    """

    c8=0
    for mc in mcpars8:
        if mc.key()==mcp8.key(): c8+=1

    if c8==8: return False

    else: return True


#---------------------------------------------------

def sensors_should(mcpar,rel):
    """find which sensors hit mcpar
    """
    ids = rel.relations(mcpar.key())
    out=[]
    if DEBUG: print "   IDS=", ids
    for i in ids:
            id=LHCbID(i[0])
            if id.isVelo():
                    out.append(id.veloID().sensor())
    
    return out


#---------------------------------------------------

def sensors_have(TES,vID,mkey):
    """find sensors whose hits were used to make the track
    """
    out=[]
    allmcpars=ghc.getmcpars(TES,vID)
    i=0
    for e in allmcpars:
            if e and e.key()==mkey:
                    out.append(vID[i].veloID().sensor())
            i+=1

    return out


#---------------------------------------------------

def compare(sho,has,mcpar,VELO):
    """find the position in velo of holes
    """

    def getz(sens):
            for i in range(132):
                    if VELO.sensor(i) and VELO.sensor(i).sensorNumber()==sens: return VELO.sensor(i).z()

    def sortz(sh,pz):
        zs=map(lambda x: [getz(x),x],sh)
        if pz>0: zs.sort()
        else: zs.sort(reverse=True)
        return map(lambda x: x[1],zs)

    pz=mcpar.momentum().z()
    sho=sortz(sho,pz)
    out=[]
    for s in sho:
            if s not in has: out.append(s)
    return out
