from ParticleTranslator import PTranslate
def endProducts(mc,zcut = 1000, rhocut = 5.):
    out = []
    if mc.endVertices().size()==0:return [mc]
    for i in range(mc.endVertices().size()):
        ev =mc.endVertices().at(i).ptr()
        endV = ev.position()
        #endV = vector(endV.x(),endV.y(),endV.z())
        rho2 = endV.x()*endV.x()+endV.y()*endV.y()
        rhocut =rhocut*rhocut
        if endV.z()-mc.originVertex().position().z() > zcut or rho2 > rhocut: break
        prods = ev.products()
        for j in range(prods.size()):
            cand = prods.at(j).ptr()
            epc = endProducts(cand)
            for par in epc:
                out.append(par)
    if out == [] :return [mc]
    return out
def getFirstBs(pars):
    k,l = [], []
    for par in pars:
        mo = rootMother(par)
        ki = mo.key()
        if (not ki in k) and mo.particleID().hasBottom():
            k.append(ki)
            l.append(mo)
    return l
def printProducts(par,fMo = 0):
    Mpid = PTranslate(par.particleID().pid())
    sts = fMo*"\t"
    if not fMo: print "Starting in particle ",Mpid
    else: print sts + "Daughter ",Mpid," has products :"
    evs = par.endVertices()
    for i in range(evs.size()):
        ev = evs.at(i).ptr().products()
        hhh = []
        for j in range(ev.size()):
            dgt = ev.at(j).ptr()
            print sts + "\t"+PTranslate(dgt.particleID().pid())
            if dgt.endVertices().size() and dgt.endVertices().at(0).ptr().products().size(): hhh.append(dgt)
        for ddd in hhh:
            printProducts(ddd,fMo+1)

    

        
def rootB(mcpars,pid=531):
    """Returns a list with Bs's or (main mothers with pid if other)
    """
    broot = []
    for i in range(mcpars.size()):
        mcpar=mcpars.containedObjects()[i]
        if ((mcpar.particleID().abspid() == pid) and
            (mcpar.mother() == None)):
            broot.append(mcpar)
    return broot

def quark(q,mo1):
    """finds a quark in a number
    """
    mo1=1.*mo1
    mo=str(mo1)
    m1=mo[len(mo)-min(len(mo),6) : len(mo)-3]# 0, point and spin
    q=str(q)
    if m1.find(q)!=-1: return 1
    return 0

def rootMother(mcpar):
    """Returns the first mother of the MC particle mcpar.
    """

    mom = mcpar.mother()
    if (mom == None): return mcpar
    return rootMother(mom)

def rootMother3(mcpar,i=0):
    """Returns the first mother of the MC particle mcpar.
    """

    mom = mcpar.mother()
    if (mom == None): return mcpar,i
    i += 1
    return rootMother3(mom,i)

def decayFinder(mcpars,decay):

    DEBUG = True

    bmon = decay[0]
    bpid = PTranslate(bmon)
    if (DEBUG): print " mother pid ",bmon,bpid

    sons = decay[1]
    sonspid = map(PTranslate,sons)
    if (DEBUG): print " mother pid ",sons,sonspid

    bs = signalParticles3(mcpars,1,bpid)
    
    def isSon(x,pids):
        pid = x.particleID().pid()
        print " pid ",pid, " pids ",pids
        return (pid in pids)

    decays = []
    for b in bs:
        sdecays = []
        goodsons = filter(lambda x: isSon(x,sonspid),b)
        if (DEBUG): print " good sons ",len(goodsons)
        decays.append(goodsons)
        
    return decays
    
def signalParticles3(mcpars,ngenerations = 1, pid=531):
    """Returns a list with all the Bs's keys with all the products per key
    """
    roots = rootB(mcpars,pid)
    bsignal = []
    for iroot in roots:
        if (not abs(iroot.particleID().pid()) == pid): continue
        bsons = []
        for mcpar in mcpars:
            i = 0
            (mom,i) = rootMother3(mcpar,i)
            if ((mom.key() == iroot.key()) and (i <= ngenerations)):
                bsons.append(mcpar)
        bsignal.append(bsons)
    print " Bs ",len(bsignal)    
    return bsignal

def signalParticles2(mcpars):
    """Returns a list with all the Bs's keys with all the products per key
    """
    roots = rootB(mcpars)
    keys = map(lambda x: x.key(),roots)
    bsignal = {}
    for key in keys: bsignal[key] = [] 
    for i in range(mcpars.size()):
        mcpar=mcpars.containedObjects()[i]
        mom = rootMother(mcpar)
        if (mom.key() in keys):
            bsignal[key].append(mcpar)
    return bsignal


def makeList(cobjs):
    lobjs = cobjs.size()*[0]
    vobjs = cobjs.containedObjects()
    for i in range(cobjs.size()): lobjs[i] = vobjs[i]
    return lobjs

def byKey(key,lobjs):
    for obj in lobjs:
        if (obj.key() == key): return obj
    return None

def hasKey(objs,obj):
    for o in objs:
        if (o.key() == obj.key()): return True
    return False


def signalTracks(mcpars,tracks,link):
    """Returns the list with tracks in 'tracks' which came from a MC B decay
    """
    roots = rootB(mcpars)
    keys = map(lambda x: x.key(),roots)
    bsignal = {}
    for key in keys: bsignal[key] = [] 
    for track in tracks:
       
        mcpar=link.first(track.key())
        
        if mcpar and mcpar.originVertex().position().z()<750.: #last VELO station
            mom = rootMother(mcpar)
            if (mom.key() in keys):
                bsignal[key].append(track)
    return bsignal,keys
