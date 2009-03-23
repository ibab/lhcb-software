# =============================================================================
""" @namespace hltconf
@brief get the configuration of an Hlt alley, its filters
@author Jose A. Hernando hernando@cern.ch
@date 2008-06-16
"""
# =============================================================================

import GaudiPython
from math import *

VERBOSE = False
DEBUG = False
XDEBUG = True

HLTPROPERTIES = ["InputSelection","OutputSelection",
                 "InputSelection1","InputSelection2","InputSelections",
                 "FilterDescriptor","RecoName","MatchName","Quality",
                 "HistoDescriptor"]

VARIABLE_RANGE = {"IP":[0.,0.5],
                  "PT":[0.,6000.],
                  "Calo":[0.,10.],
                  "DOCA":[0.,1.],
                  "Fraction":[0.,2.],
                  "Dz":[0.,5.],
                  "Pointing":[0.,1.],
                  "Match":[0.,2.],
                  "OverNdf":[0.,40.],
                  "L0ET":[1500.,5100.]
                  }

VARIABLE_DEFAULT = 1e16

ALLEYSCONF = {}

MAKERS = ["HltVertexToTracks","HltVertexMaker1","HltVertexMaker2"]
FILTERS = ["HltTrackFilter","HltVertexFilter"]
TF = "HltTrackFilter"
VF = "HltVertexFilter"

COMPACT = True

EXCLUDE_FILTERS = ["IsBackward","Calo2DChi2","RZVeloTMatch","IsMuon",
                   "PT0","DoShareM3"]
EXCLUDE_BINDERS = ["Prep"]

DATA = {"Index":20000}

def functionName(filtername):
    """ returns the function name associated to a filter name
    i.e filtername = 'PT,>,2000', returns 'PT'
    @param filtername name of the filter i.e 'PT,>,2000'
    """
    cromos = filtername.split(",")
    name = cromos[0]
    return name

def infoID(annsvc,filtername,mayor="InfoID"):
    """ returns the integer ID associated to an name (string) of the values stored
    in the extra info of the HLT candidates """
    vals = annsvc.items("InfoID")
    cromos = filtername.split(",")
    name = cromos[0]
    val = filter(lambda x: x.first == name,vals)
    if len(val) != 1:
        print " Warning: no InfoID associated to ",name
        return -1
    return val[0].second


def confAlley(gaudi,alleyname,head="Hlt1",tail="FilterSequence"):
    """ return the ordered list of the algorithms (HltAlgoConf) of an alley
    with their properties
    """
    if (not ALLEYSCONF.has_key(alleyname)): 
        algos = []
        sequencer = head+alleyname+tail
        confSequencer(gaudi,sequencer,algos)
        if (DEBUG):
            for algo in algos: print algo
        ALLEYSCONF[alleyname] = algos
    return ALLEYSCONF[alleyname]

def _opt(name,type,value,com=" = "):
    """ returns a string with the 'option' 
    """
    s = str(name)+"."+str(type)+str(com)
    send = ";\n"
    def svalue(v):
        vs = str(v)
        if (isinstance(v,str)):
            vs = '"'+vs+'"'
        return vs
    if (isinstance(value,str)):
        s = s + svalue(value)+send
    if (isinstance(value,list)):
        if (len(value)<=0):
            s = s +"{}"+send
        else:
            s = s + '{'
            for v in value[:-1]:
                s = s + svalue(v)+","
            s = s + svalue(value[-1])+'}'+send
    return s

def stepname(a):
    HAS = ["FilterDescriptor","RecoName","MatchName"]
    name = None
    xname = a.property('FilterDescriptor')
    if (xname and len(xname)>0):
        name = str(xname[0])
        cromos = name.split(",")
        funs = cromos[0].split("_")
        fun = funs[0]
        com = cromos[1].replace("||","")
        val = cromos[2]
        name = fun+com+val
    xname = a.property('RecoName')
    if (xname): name = xname
    xname = a.property('MatchName')
    if (xname): name = xname
    #print " stepname ",a," ==> ",name
    return name

def alleySteps(alley):
    malley = []
    for a in alley:
        sname = stepname(a)
        if (sname and sname != 'None' and (sname.find("IsBackward")<0)):
            malley.append(a)
    return malley


def stepsSelections(gaudi,alleyname):
    """ return the list with the selections of an alley (in order of execution)
    @param gaudi: gaudi application
    @param alleyname: name of the alley, i.e 'SingleHadron'
    """
    alley = confAlley(gaudi,alleyname)
    steps = alleySteps(alley)
    osels = []
    for istep in steps:
        sel =  istep.property("OutputSelection")
        if (sel): osels.append(sel)
    return osels

def selname(algo):
    """ return the output selection of this algorithm
    """
    if (not algo.props.has_key('OutputSelection')): return None
    return algo.props['OutputSelection']

def writeAlley(gaudi,alleyname,fname="alley.txt"):
    """ writes a file with a list with (algoname,stepname,selectioname)
    """
    alley = confAlley(gaudi,alleyname)
    f = open(fname,"a")
    names = map(lambda x: x.name,alley)
    nsteps = map(stepname,alley)
    nsels = map(selname,alley)
    f.write(alleyname+"Alley = [\n")
    for i in range(len(names)):
        s = "('"+str(names[i])+"','"+str(nsteps[i])+"','"+str(nsels[i])+"')"
        if (i<len(names)-1): s = s+",\n"
        else: s= s+"\n]\n"
        f.write(s)
    f.close()        

def annSvcDB(asvc):
    """ returns in a dictionary the contents of the ANNSvc
    it holds the Hlt1SelectionID name <-> int and the InfoID name <-> int
    """
    DB = {}
    majors = asvc.majors()
    for major in majors:
        dmajor = {}
        vals = asvc.items(major)
        for val in vals:
            dmajor[val.first] = val.second
        DB[major] = dmajor
    return DB

def extendANNSvc(gaudi,alleyname,fname="ANNSvcExtended.py",index=0):
    """ Extend the configuration python file for the ANNSvc with the names
    if the selections of a given alley
    @param gaudi: guadi application
    @param alleyname: i.e 'SingleHadron'
    @param fname, filename with the configurations default 'ANNSvcExtended.py'
    @param index, incremental index of the selections (automatic default:2000)
    """
    if (index == 0): index = DATA['Index']
    f = open(fname,"a")
    osels = stepsSelections(gaudi,alleyname)
    f.write("from Configurables import HltANNSvc \n")
    f.write("HltANNSvc().Hlt1SelectionID.update( {\n")
    for osel in osels[:-1]:
        f.write('"'+osel+'" : '+str(index)+',\n')
        index = index +1
    f.write('"'+osels[-1]+'" : '+str(index)+"})\n")
    f.close()
    index = index +1
    DATA['Index'] = index

class HltAlgoConf():
    """ A class to hold the important properties of an HltAlgorithm
    (type, name, properties)
    """
    def __init__(self,type,name,props={}):
        self.type = type
        self.name = name
        self.props = props

    def property(self,name):
        """ return the property of the algo
        """
        if (not self.props.has_key(name)): return None
        return self.props[name]

    def opts(self):
        """ return the opts of the algo
        """
        s = ""
        for key in self.props.keys():
            if key in HLTPROPERTIES:
                if ((key == "OutputSelection") and
                     (self.props["OutputSelection"] == self.name)):
                    continue
                if (key == "HistoDescriptor"): continue
                s = s + _opt(self.name,key,self.props[key])
        return s

    def __repr__(self):
        """ print the algo information (name, type, filter)
        """
        c = " type: "+str(self.type)+" name: "+str(self.name)
        fs = _property(self,"FilterDescriptor")
        if (fs):
            c = c + " { "
            ss = map(str,fs)
            for s in ss: c = c + s +" "
            c = c + "} "
        return c

def confSequencer(gaudi,sequencer,algos):
    """ add in the list of algos objects of 'HltAlgoConf'
    with the relevant properties
    """
    seq = gaudi.algorithm(sequencer)
    if (not seq):
        print "ERROR : no sequencer named ", sequencer
    props = seq.properties()
    membersprop = props["Members"]
    members = membersprop.value() 
    for member in members:
        type = member
        algo = member
        sel  = None
        i0 = member.find("/")
        if (i0>0):
            type = member[:i0]
            algo = member[i0+1:]
        if (type != "GaudiSequencer"):
            props = gaudi.algorithm(algo).properties()
            hltprops = {}
            for hltprop in HLTPROPERTIES:
                if (props.has_key(hltprop)):
                    value = gaudi.algorithm(algo).properties()[hltprop].value()
                    hltprops[hltprop] = value
                    if (VERBOSE): print hltprop," = ",value
            algos.append(HltAlgoConf(type,algo,hltprops))
        else:
            confSequencer(gaudi,algo,algos)

def hisrange(name):
    """ return a range [XMIN,XMAX] for a given function name: i.e PT
    """
    for key in VARIABLE_RANGE.keys():
        if (name.find(key)>=0): return VARIABLE_RANGE[key]
    return [0.,100.]

def defineRange(name,x0,xf):
    VARIABLE_RANGE[name] = [x0,xf]
    
def writeInternalMonitorOpts(gaudi,alleyname):
    """ Write an opts file with the internal histograms options of an alley
    """
    alleyConf = confAlley(gaudi,alleyname)
    ss = []
    for a in alleyConf:
        def hopt(title):
            ran = hisrange(title)
            s = a.name+'.HistoDescriptor += {"'+title+'":("'+title+'",'+str(ran[0])+','+str(ran[1])+',100)};\n'
            ss.append(s)
            return
        if (a.type.find("Hlt")<0): continue
        s = a.name+".HistogramUpdatePeriod = 1;\n"
        ss.append(s)
        sel = a.property("OutputSelection")
        if (sel): hopt(sel)
        isFilter = (a.type.find("Filter")>0)
        isMaker = (a.type.find("Maker")>0)
        if (isFilter or isMaker):
            filters = a.property("FilterDescriptor")
            if (not filters): continue
            for f in filters:
                cromos = f.split(",")
                filter = cromos[0]
                hopt(filter)
                hopt(filter+"Best")
    f = open("Hlt1"+alleyname+"InternalMonitor.opts","w")
    for s in ss: f.write(s)
    f.close()


class hltfilter():
    """ class that holds the information of an hlt filter, i.e 'PT,>,5000.'
    passing some candidates return the quantities associated to the filter
    """ 
    def __init__(self,name,id,x0=0.,xf=100.):
        cromos = name.split(",")
        self.name = cromos[0]
        self.comparator = cromos[1]
        self.value = cromos[2]
        self.id = id
        if (DEBUG): print " filter ID ",name,self.id
        if (len(cromos)>3): self.value2 = cromos[3]
        self.xrange = hisrange(name)

    def vals(self,candis,xhisto=None):        
        vals0 = map(lambda x: x.info(self.id,VARIABLE_DEFAULT),candis)
        vals = filter(lambda x : x!=VARIABLE_DEFAULT, vals0)
        if (self.comparator.find("||")>=0):
            vals = map(abs,vals)
            if (DEBUG): print self.name,vals
        if (xhisto):
            for v in vals: xhisto.Fill(v,1.)
        return vals

    def best(self,candis,xhisto=None):
        tvals = self.vals(candis)
        if (len(tvals)<=0): return 0.
        tvals.sort()
        best = tvals[0]
        if (self.comparator.find(">")>=0): best=tvals[-1]
        if (DEBUG): print best
        if (xhisto): xhisto.Fill(best,1.)
        return best

def _property(algo,prop,default = None):
    """ returns a property of an HltAlgoConf, if not, returns default
    """
    val = default
    if (algo.props.has_key(prop)):
        val =  algo.property(prop)
    if (VERBOSE):
        print " _property ",algo,prop,val
    return val

def _type(algo):
    """ returns the type 'Track','Vertex' of an HltAlgoConf
    """
    if (VERBOSE): print "_type :",algo
    if (algo.type.find("Vertex")>0): return "Vertex"
    return "Track"

def _isFilter(a):
    """ returns true if a HltAlgConf is a filter type
    (HltTrackFilter,HltVertexFilter)
    """
    return (a.type.find("Filter")>0)

def _produceSelection(alley,selname):
    """ returns the algorithm in the list of algorithms that produces
    the selection name
    """
    if (not selname): return None
    for a in alley:
        aselname = _property(a,"OutputSelection")
        if (aselname == selname):
            if (VERBOSE): print " algo ",a," produces ",selname
            return a
    print "Error no algorithm produces selection ",selname
    return None

def _segment(alley,outsel):
    """ return the list of algorithms that produces a given selection
    """
    if (DEBUG):
        print " _segment for ",outsel
    algos = []
    insel = outsel
    next = True
    algo = _produceSelection(alley,insel)
    if (not algo): return None
    type = _type(algo)
    while (next):
        algo = _produceSelection(alley,insel)
        insel = None
        if (algo):
            algos.append(algo)
            insel = _property(algo,"InputSelection")
        if (algo.type.find("VertexMaker")>0): next = False
        if (not algo): next = False
        elif (algo.type.find(type)<0): next = False
        if (not insel): next = False
        elif (insel.find("TES")>=0): next = False
        if (VERBOSE):
            print " segment step: ",algo,insel,next
    if (type == "Track" and COMPACT):
        algo = algos[-1]
        isel1 = _property(algo,"InputSelection1")
        isel2 = _property(algo,"InputSelection2")
        if (VERBOSE): algo,isel1,isel2
        def extend(isel):
            algos1 = _segment(alley,isel)
            for a in algos1: algos.append(a)
        if (isel1): extend(isel1)
        if (isel2): extend(isel2)
    if (DEBUG):
        print " segment ",algos
    return algos

def _acceptFilter(f,novalid = EXCLUDE_FILTERS):
    """ returns true if the filter *f* i.e 'PT,>5000' it is not
    in the list of excluded filters *novalid*
    @param f filter i.e 'PT,>,5000.'
    @param novalid list of no valid filters
    """
    ok = True
    for n in novalid:
        if (f.find(n)>=0): ok = False
    if (VERBOSE): print " _acceptFilter: ",ok
    return ok

def _filters(algos):
    """ returns the list of filters associated to the list of algos (HltAlgConf) 
    """
    ofs = []
    ralgos = algos[:]
    ralgos.reverse()
    for a in ralgos:
        if (a.type in FILTERS):
            fs = _property(a,"FilterDescriptor")
            for f in fs:
                if (_acceptFilter(f)): ofs.append(f)
    if (DEBUG): print " _filters :",ofs
    return ofs

def _binder(f):
    """ returns the filter splited in function,binder i.e 'IP_PV2D,>,0.1' => [IP,PV2D]
    """
    bin = None
    cromos = f.split(",")
    cromos = cromos[0].split("_")
    if (len(cromos)>1): bin = cromos[1]
    return cromos

def _renamebinders(fs,inname,outname):
    """ rename a list of filters, if the inname is in the binder of the filter,
    the binder is replaced by the outname,
    i.e 'IP_PV2D,>,0.1' inname='PV2D' outname='PV3D' => 'IP_PV3D,>,0.1'
    """
    ofs = []
    for f in fs:
        of = f
        cromos = _binder(f)
        if (len(cromos)>1):
            if (cromos[1].find(inname)>=0):
                of = f.replace(cromos[1],outname)
        ofs.append(of)
    if (DEBUG):
        print "_renamebinders: ",inname,outname
        print "_renamebinders: ",ofs,
    return ofs

def _binders(filters):
    """ returns the list of binders associated to the filters,
    it does not include the binders already in binders, and the one in the novalid list
    i.e filters = ['IP_PV2D,>,0.1'] ==> [PV2D]
    """
    bins = []
    def valid(f):
        for n in novalid:
            if (f.find(n)>=0): return False
        return True
    for f in filters:
        cromos = _binder(f)
        if (len(cromos)>1):
            bin = cromos[1]
            if (bin not in bins): bins.append(bin)
    if (DEBUG): print " _binders: ",bins
    return bins

def _compactSegment(segment,name,endname=""):
    """ compact a segment into two algorithms (filter + maker)
    if a track -> a TrackFilter
    if a vertex -> a VerteFilter and a VertexMaker
    """
    fs = _filters(segment)
    type = _type(segment[0])
    atype = TF
    aname = "Hlt"+name+endname
    if (type == "Vertex"): atype = VF
    algo = HltAlgoConf(atype,aname,{})
    fs = _filters(segment)
    algo.props["FilterDescriptor"] = fs
    algo.props["OutputSelection"] = aname
    if (atype != "Vertex"):
        algo.props["InputSelection"] = "TES:Rec/Track/Best"
    vseg = [algo]
    a0 = segment[-1]
    if (a0.type in MAKERS):
        makername = "Hlt"+name+"Maker"
        algo1 = HltAlgoConf(a0.type,makername,a0.props)
        algo1.props["OutputSelection"] = makername
        algo.props["InputSelection"] = makername
        for key in a0.props.keys(): algo1.props[key] = a0.props[key]
        vseg.append(algo1)
        if (_type(algo1) != _type(algo)):
            print "WARNING: inconsistent segment! ",vseg
    if (DEBUG):
        print "_compactSegment: ",vseg
    return vseg


class HltBlock():
    """ Class to hold the compact information of an alley
    An alley it is reduced to a block that has a filter a maker
    and a list of blocks as inputs
    and in addition, a list of binders, or external selections,
    needed to run this alley
    """
    def __init__(self,alley,outsel,blockname,endname=""):
        algo = _produceSelection(alley,outsel)
        if (not algo): return 
        type = _type(algo)
        self.type = type
        self.name = blockname
        self.algos = _segment(alley,outsel)
        vsegment = _compactSegment(self.algos,blockname,endname)
        self.afilter = None
        self.amaker = None
        self.binders = None
        self.inblocks = []
        if (vsegment):
            self.afilter = vsegment[0]
            fs = _property(self.afilter,"FilterDescriptor")
            self.binders = _binders(fs)
            if (len(vsegment)>1): self.amaker = vsegment[1]
        if (self.amaker):
            self.inblocks = []
            inputs = ["InputSelection","InputSelection1","InputSelection2"]
            for i in range(len(inputs)):
                isel = _property(self.amaker,inputs[i])
                if (isel):
                    iname = blockname+"Input"+str(i)
                    iblock = HltBlock(alley,isel,iname)
                    self.inblocks.append(iblock)
                    self.amaker.props[inputs[i]] = "Hlt"+iname
        if (DEBUG):
            print self

    def _updateBinders(self):
        if (self.afilter):
            fs = _property(self.afilter,"FilterDescriptor")
            self.binders = _binders(fs)
        for ibk in self.inblocks:
            ibk._updateBinders()
            sbins = filter(lambda x: x not in self.binders,ibk.binders)
            ibk.binders = sbins

    def renameBinders(self,names):
        """ swap the name of the binders that match names
        in input1 - input2 blocks
        """
        if (len(self.inblocks) != 2): return
        afilter0 = self.inblocks[0].afilter
        afilter1 = self.inblocks[1].afilter
        sel0 = _property(afilter0,"OutputSelection")
        sel1 = _property(afilter1,"OutputSelection")
        for name in names:
            fs0 = _property(afilter0,"FilterDescriptor")
            fs1 = _property(afilter1,"FilterDescriptor")
            ofs0 =  _renamebinders(fs0,name,sel1)
            ofs1 =  _renamebinders(fs1,name,sel0)
            afilter0.props["FilterDescriptor"] = ofs0
            afilter1.props["FilterDescriptor"] = ofs1
        self._updateBinders()
        if (DEBUG):
            print " rename binders: ",names
            print "        block: ",block

    def removeFilters(self,names):
        """ remove filters from the filter and internal blocks
        """
        def freduce(afilter):
            fs = _property(afilter,"FilterDescriptor")
            ofs = filter(lambda x: not (x.find(inname)>=0),fs)
            afilter.props["FilterDescriptor"] = ofs
        if (self.afilter): freduce(self.afilter)
        for ibk in self.inblocks:
            ibk.removeFilters(names)
        _updateBinders()
        if (DEBUG):
            print " remove binders: ",names
            print "        block: ",self

    def opts(self,mother="Hlt1"):
        """ write opts for a compact block (binders, inputs, maker and filter)
        """
        seqname = "Hlt"+self.name+"Seq"
        s = _opt(mother,"Members",["GaudiSequencer/"+seqname]," += ")
        def optBinder(b):
            bname = seqname.replace("Seq","SF")+b
            ss =  _opt(seqname,"Members",["HltSelectionFilter/"+bname]," += ")
            ss = ss + _opt(bname,"InputSelections",[b])
            return ss
        if (self.binders):
            for b in self.binders: s = s + optBinder(b)
        for ibk in self.inblocks:
            s = s + ibk.opts(seqname)
        if (self.amaker):
            aname = self.amaker.type+"/"+self.amaker.name
            s = s + _opt(seqname,"Members",[aname]," += ")
            s = s + self.amaker.opts()
        if (self.afilter):
            aname = self.afilter.type+"/"+self.afilter.name
            s = s + _opt(seqname,"Members",[aname]," += ")
            s = s + self.afilter.opts()
        return s        
                    
    def __repr__(self):
        """ writes the block
        """
        c = " block name: "+str(self.name)+"\n"
        c = c+" block type: "+str(self.type)+"\n"
        if (self.binders):
            c = c + " binders: "
            ss = map(str, self.binders)
            for s in ss: c = c + s + " "
            c = c + "\n"
        if (self.afilter):
            c = c + " afilter: "+str(self.afilter) +"\n"
        if (self.amaker):
            c = c + " amaker: "+str(self.amaker) +"\n"
       ##  if (len(self.algos)>0):
##             c = c + " algos: "
##             ss = map(str, self.algos)
##             for s in ss: c = c + s + " "
##             c = c +"\n"
        if (self.inblocks):
            c = c + " input blocks: " +"\n"
            ss = map(str, self.inblocks)
            for s in ss: c = c + s + "\n"
            c = c+ "\n"
        return c


class HltAlleyConf():
    """ Class that holds the full information of an alley
    it has the list of algorithms in sequencial order (alley)
    it has the list of algorithms organized as a compact block (block)
    returns the list of options for the compact alley
    """
    def __init__(self,gaudi,alleyname,compactname="Emu"):
        self.alleyname = alleyname
        self.alley = confAlley(gaudi,alleyname)
        self.outputselection = _property(self.alley[-1],"OutputSelection")
        self.block = HltBlock(self.alley,self.outputselection,
                              compactname+self.alleyname,"Decision")
        self.block.renameBinders([alleyname])
    
    def algoBySelection(self,selname):
        """ returns an algorithm that produces that selection
        """
        for a in self.alley:
            if (_property(a,"OutputSelection") == selname): return a
        print "Error no algorithm produces selection ",selname
        return None

    def algoByName(self,algoname):
        """ returns an algorithms by name
        """
        for a in self.alley:
            if (a.name == algoname):
                return a
        print "Error no algorithm ",algoname
        return None

    def opts(self,mother="Hlt1"):
        """ returns the opts as unique string of the compact block
        """
        return self.block.opts(mother)

    def __repr__(self):
        return str(self.block)
            
def test(gaudi,alleysnames=["SingleHadron","DiHadron"],debug = False):
    if (debug): DEBUG = True
    for aname in alleysnames:
        ah = HltAlleyConf(gaudi,aname)
        s = ah.block.opts()
        print "==================================\n"
        print "test Alley Name: \n",ah.alleyname
        print "test Alley: \n",ah.alley
        print "test Block: \n",ah.block

