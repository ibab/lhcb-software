# $Id: StrippingB2nbody.py,v 1.0 2011-02-8 12:53:17 roldeman Exp $
__author__ = ['Rudolf Oldeman']
__date__ = '08/02/2011'
__version__ = '$Revision: 1.0 $'

__all__ = ('B2nbodyConf',
           'makeB2nbody',
           'makePi',
           'makeK',
           'makep', 
           'makeKs',
           'makeLm',
           'makeDz',
           'makeDp',
           'makeDs',
           'makeLc',
           'makePh',
           'makeKS',
           'makeJp',
           'makeDS')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, MergedSelection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

#name = "B2nbody"

class B2nbodyConf(LineBuilder) :
    __configuration_keys__ = ('nbody','MinBMass','MaxBMass','MinBPt','MaxBVertChi2DOF','MinBPVVDChi2','MaxBPVIPChi2','MinBPVDIRA','MinNvc',
                              'doPi','doK','dop','doKs','doLm','doDz','doDp','doDs','doLc','doPh','doKS','doJp','doDS',
                              'MinPiPt','MinPiIPChi2DV','MaxPiChi2',
                              'MinKPt','MinKIPChi2DV','MaxKChi2',
                              'MinpPt','MinpIPChi2DV','MaxpChi2',
                              'MaxKsDeltaM','MinKsPt','MaxKsVertChi2DOF','MinKsPVVDChi2','MinKsIPChi2','MinKsDauPt','MinKsDauIPChi2','MaxKsDauTrkChi2',
                              'MaxLmDeltaM','MinLmPt','MaxLmVertChi2DOF','MinLmPVVDChi2','MinLmIPChi2','MinLmPrtPt','MinLmPiPt','MinLmPrtIPChi2','MinLmPiIPChi2','MaxLmPrtTrkChi2','MaxLmPiTrkChi2',
                              'MaxDzDeltaM','MinDzPt','MaxDzVertChi2DOF','MinDzPVVDChi2','MinDzIPChi2','MinDzDauPt','MinDzDauIPChi2','MaxDzDauTrkChi2','MinDzPIDK',
                              'MaxDpDeltaM','MinDpPt','MaxDpVertChi2DOF','MinDpPVVDChi2','MinDpIPChi2','MinDpDauPt','MinDpDauIPChi2','MaxDpDauTrkChi2','MinDpPIDK',
                              'MaxDsDeltaM','MinDsPt','MaxDsVertChi2DOF','MinDsPVVDChi2','MinDsIPChi2','MinDsDauPt','MinDsDauIPChi2','MaxDsDauTrkChi2','MinDsPIDK',
                              'MaxLcDeltaM','MinLcPt','MaxLcVertChi2DOF','MinLcPVVDChi2','MinLcIPChi2','MinLcDauPt','MinLcDauIPChi2','MaxLcDauTrkChi2','MinLcPIDK','MinLcPIDp',
                              'MaxPhDeltaM','MinPhPt','MaxPhVertChi2DOF','MinPhPVVDChi2','MinPhIPChi2','MinPhDauPt','MinPhDauIPChi2','MaxPhDauTrkChi2',
                              'MaxKSDeltaM','MinKSPt','MaxKSVertChi2DOF','MinKSPVVDChi2','MinKSIPChi2','MinKSDauPt','MinKSDauIPChi2','MaxKSDauTrkChi2',
                              'MaxJpDeltaM','MinJpPt','MaxJpVertChi2DOF','MinJpPVVDChi2','MinJpIPChi2','MinJpDauPt','MinJpDauIPChi2','MaxJpDauTrkChi2',
                              'MaxDSDeltaM','MinDSPt','MaxDSVertChi2DOF','MinDSPVVDChi2','MinDSIPChi2','MinDSPiPt','MinDSPiIPChi2','MaxDSPiTrkChi2','MaxDSD0DeltaM',
                              'MinDSD0Pt','MaxDSD0VertChi2DOF','MinDSD0PVVDChi2','MinDSD0IPChi2','MinDSD0DauPt','MinDSD0DauIPChi2','MaxDSD0DauTrkChi2')
    
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        self.selPi = makePi( 'PiFor'+name,
                             MinPiPt      =config['MinPiPt'],
                             MinPiIPChi2DV=config['MinPiIPChi2DV'],
                             MaxPiChi2    =config['MaxPiChi2'])
        self.selK = makeK( 'KFor'+name,
                             MinKPt      =config['MinKPt'],
                             MinKIPChi2DV=config['MinKIPChi2DV'],
                             MaxKChi2    =config['MaxKChi2'])
        self.selp = makep( 'pFor'+name,
                             MinpPt      =config['MinpPt'],
                             MinpIPChi2DV=config['MinpIPChi2DV'],
                             MaxpChi2    =config['MaxpChi2'])
        self.selKs = makeKs('KsFor'+name,
                            MaxKsDeltaM     =config['MaxKsDeltaM'],    
                            MinKsPt         =config['MinKsPt'],        
                            MaxKsVertChi2DOF=config['MaxKsVertChi2DOF'],
                            MinKsPVVDChi2   =config['MinKsPVVDChi2'],  
                            MinKsIPChi2     =config['MinKsIPChi2'],
                            MinKsDauPt      =config['MinKsDauPt'],     
                            MinKsDauIPChi2  =config['MinKsDauIPChi2'], 
                            MaxKsDauTrkChi2 =config['MaxKsDauTrkChi2'])
        self.selLm = makeLm('LmFor'+name,
                            MaxLmDeltaM     =config['MaxLmDeltaM'],    
                            MinLmPt         =config['MinLmPt'],        
                            MaxLmVertChi2DOF=config['MaxLmVertChi2DOF'],
                            MinLmPVVDChi2   =config['MinLmPVVDChi2'],  
                            MinLmIPChi2     =config['MinLmIPChi2'],    
                            MinLmPrtPt      =config['MinLmPrtPt'],     
                            MinLmPiPt       =config['MinLmPiPt'],      
                            MinLmPrtIPChi2  =config['MinLmPrtIPChi2'], 
                            MinLmPiIPChi2   =config['MinLmPiIPChi2'],  
                            MaxLmPrtTrkChi2 =config['MaxLmPrtTrkChi2'],
                            MaxLmPiTrkChi2  =config['MaxLmPiTrkChi2'])
        self.selDz = makeDz('DzFor'+name,
                            MaxDzDeltaM     =config['MaxDzDeltaM'],   
                            MinDzPt         =config['MinDzPt'],        
                            MaxDzVertChi2DOF=config['MaxDzVertChi2DOF'],
                            MinDzPVVDChi2   =config['MinDzPVVDChi2'],  
                            MinDzIPChi2     =config['MinDzIPChi2'],    
                            MinDzDauPt      =config['MinDzDauPt'],     
                            MinDzDauIPChi2  =config['MinDzDauIPChi2'], 
                            MaxDzDauTrkChi2 =config['MaxDzDauTrkChi2'],
                            MinDzPIDK       =config['MinDzPIDK'])
        self.selDp = makeDp('DpFor'+name,
                            MaxDpDeltaM     =config['MaxDpDeltaM'],   
                            MinDpPt         =config['MinDpPt'],        
                            MaxDpVertChi2DOF=config['MaxDpVertChi2DOF'],
                            MinDpPVVDChi2   =config['MinDpPVVDChi2'],  
                            MinDpIPChi2     =config['MinDpIPChi2'],    
                            MinDpDauPt      =config['MinDpDauPt'],     
                            MinDpDauIPChi2  =config['MinDpDauIPChi2'], 
                            MaxDpDauTrkChi2 =config['MaxDpDauTrkChi2'],
                            MinDpPIDK       =config['MinDpPIDK'])
        self.selDs = makeDs('DsFor'+name,
                            MaxDsDeltaM     =config['MaxDsDeltaM'],   
                            MinDsPt         =config['MinDsPt'],        
                            MaxDsVertChi2DOF=config['MaxDsVertChi2DOF'],
                            MinDsPVVDChi2   =config['MinDsPVVDChi2'],  
                            MinDsIPChi2     =config['MinDsIPChi2'],    
                            MinDsDauPt      =config['MinDsDauPt'],     
                            MinDsDauIPChi2  =config['MinDsDauIPChi2'], 
                            MaxDsDauTrkChi2 =config['MaxDsDauTrkChi2'],
                            MinDsPIDK       =config['MinDsPIDK'])
        self.selLc = makeLc('LcFor'+name,
                            MaxLcDeltaM     =config['MaxLcDeltaM'],   
                            MinLcPt         =config['MinLcPt'],        
                            MaxLcVertChi2DOF=config['MaxLcVertChi2DOF'],
                            MinLcPVVDChi2   =config['MinLcPVVDChi2'],  
                            MinLcIPChi2     =config['MinLcIPChi2'],    
                            MinLcDauPt      =config['MinLcDauPt'],     
                            MinLcDauIPChi2  =config['MinLcDauIPChi2'], 
                            MaxLcDauTrkChi2 =config['MaxLcDauTrkChi2'],
                            MinLcPIDK       =config['MinLcPIDK'],
                            MinLcPIDp       =config['MinLcPIDp'])
        self.selPh = makePh('PhFor'+name,
                            MaxPhDeltaM     =config['MaxPhDeltaM'],     
                            MinPhPt         =config['MinPhPt'],         
                            MaxPhVertChi2DOF=config['MaxPhVertChi2DOF'],
                            MinPhPVVDChi2   =config['MinPhPVVDChi2'],   
                            MinPhIPChi2     =config['MinPhIPChi2'],     
                            MinPhDauPt      =config['MinPhDauPt'],      
                            MinPhDauIPChi2  =config['MinPhDauIPChi2'],  
                            MaxPhDauTrkChi2 =config['MaxPhDauTrkChi2'])
        self.selKS = makeKS('KSFor'+name,
                            MaxKSDeltaM     =config['MaxKSDeltaM'],     
                            MinKSPt         =config['MinKSPt'],         
                            MaxKSVertChi2DOF=config['MaxKSVertChi2DOF'],
                            MinKSPVVDChi2   =config['MinKSPVVDChi2'],   
                            MinKSIPChi2     =config['MinKSIPChi2'],     
                            MinKSDauPt      =config['MinKSDauPt'],      
                            MinKSDauIPChi2  =config['MinKSDauIPChi2'],  
                            MaxKSDauTrkChi2 =config['MaxKSDauTrkChi2'])
        self.selJp = makeJp('JpFor'+name,
                            MaxJpDeltaM     =config['MaxJpDeltaM'],     
                            MinJpPt         =config['MinJpPt'],         
                            MaxJpVertChi2DOF=config['MaxJpVertChi2DOF'],
                            MinJpPVVDChi2   =config['MinJpPVVDChi2'],   
                            MinJpIPChi2     =config['MinJpIPChi2'],     
                            MinJpDauPt      =config['MinJpDauPt'],      
                            MinJpDauIPChi2  =config['MinJpDauIPChi2'],  
                            MaxJpDauTrkChi2 =config['MaxJpDauTrkChi2']) 
        self.selDS = makeDS('DSFor'+name,
                            MaxDSDeltaM       =config['MaxDSDeltaM'],      
                            MinDSPt           =config['MinDSPt'],          
                            MaxDSVertChi2DOF  =config['MaxDSVertChi2DOF'], 
                            MinDSPVVDChi2     =config['MinDSPVVDChi2'],    
                            MinDSIPChi2       =config['MinDSIPChi2'],      
                            MinDSPiPt         =config['MinDSPiPt'],        
                            MinDSPiIPChi2     =config['MinDSPiIPChi2'],    
                            MaxDSPiTrkChi2    =config['MaxDSPiTrkChi2'],   
                            MaxDSD0DeltaM     =config['MaxDSD0DeltaM'],    
                            MinDSD0Pt         =config['MinDSD0Pt'],        
                            MaxDSD0VertChi2DOF=config['MaxDSD0VertChi2DOF'],
                            MinDSD0PVVDChi2   =config['MinDSD0PVVDChi2'],  
                            MinDSD0IPChi2     =config['MinDSD0IPChi2'],    
                            MinDSD0DauPt      =config['MinDSD0DauPt'],     
                            MinDSD0DauIPChi2  =config['MinDSD0DauIPChi2'], 
                            MaxDSD0DauTrkChi2 =config['MaxDSD0DauTrkChi2'])

        
        self.selB2nbody = makeB2nbody(name,
                                     PiSel=self.selPi,
                                     KSel =self.selK,
                                     pSel =self.selp,
                                     KsSel=self.selKs,
                                     LmSel=self.selLm,
                                     DzSel=self.selDz,
                                     DpSel=self.selDp,
                                     DsSel=self.selDs,
                                     LcSel=self.selLc,
                                     PhSel=self.selPh,
                                     KSSel=self.selKS,
                                     JpSel=self.selJp,
                                     DSSel=self.selDS,
                                     nbody          =config['nbody'],       
                                     MinBMass       =config['MinBMass'],       
                                     MaxBMass       =config['MaxBMass'],       
                                     MinBPt         =config['MinBPt'],         
                                     MaxBVertChi2DOF=config['MaxBVertChi2DOF'],
                                     MinBPVVDChi2   =config['MinBPVVDChi2'],   
                                     MaxBPVIPChi2   =config['MaxBPVIPChi2'],   
                                     MinBPVDIRA     =config['MinBPVDIRA'],     
                                     MinNvc         =config['MinNvc'],
                                     doPi           =config['doPi'],
                                     doK            =config['doK'],
                                     dop            =config['dop'],
                                     doKs           =config['doKs'],
                                     doLm           =config['doLm'],
                                     doDz           =config['doDz'],
                                     doDp           =config['doDp'],
                                     doDs           =config['doDs'],
                                     doLc           =config['doLc'],
                                     doPh           =config['doPh'],
                                     doKS           =config['doKS'],
                                     doJp           =config['doJp'],
                                     doDS           =config['doDS'])

        self.line = StrippingLine(name+"Line", selection = self.selB2nbody)
        self.registerLine(self.line)



def makeB2nbody(name,PiSel,KSel,pSel,KsSel,LmSel,DzSel,DpSel,DsSel,LcSel,PhSel,KSSel,JpSel,DSSel,
                nbody,MinBMass,MaxBMass,MinBPt,MaxBVertChi2DOF,MinBPVVDChi2,MaxBPVIPChi2,MinBPVDIRA,MinNvc,
                doPi,doK,dop,doKs,doLm,doDz,doDp,doDs,doLc,doPh,doKS,doJp,doDS):

    # Define a class that contains all relevant particle properties
    class particle:
        def __init__(self,name,nvc,q,b):
            self.name=name
            self.nvc=nvc        # number of vertex-contraining (pseudo)tracks
            self.Q=q            # electrical charge
            self.B=b            # baryon number
            self.cp=self
        def CP(self,name):
            p= particle(name,self.nvc,-self.Q,-self.B)
            p.cp=self
            self.cp=p
            return p
        def isCP(self):
            if self.cp.name==self.name: return True
            else: return False



    # Define the particles (skip D_s+ since it's called D+)
    particles=[]
    if doPi:         particles.append(particle(        "pi+"       ,1,  +1,  0))
    if doPi:         particles.append(particles[-1].CP("pi-"       ))
    if doK:          particles.append(particle(        "K+"        ,1,  +1,  0))
    if doK:          particles.append(particles[-1].CP("K-"        ))
    if dop:          particles.append(particle(        "p+"        ,1,  +1, +1))
    if dop:          particles.append(particles[-1].CP("p~-"       ))
    if doKs:         particles.append(particle(        "KS0"       ,0,   0,  0))
    if doLm:         particles.append(particle(        "Lambda0"   ,0,   0, +1))
    if doLm:         particles.append(particles[-1].CP("Lambda~0"  ))
    if doDz:         particles.append(particle(        "D0"        ,1,   0,  0))
    if doDz:         particles.append(particles[-1].CP("D~0"       ))
    if doDp or doDs: particles.append(particle(        "D+"        ,1,  +1,  0))
    if doDp or doDs: particles.append(particles[-1].CP("D-"        ))
    if doLc:         particles.append(particle(        "Lambda_c+" ,1,  +1, +1))
    if doLc:         particles.append(particles[-1].CP("Lambda_c~-"))
    if doPh:         particles.append(particle(        "phi(1020)" ,2,   0,  0))
    if doKS:         particles.append(particle(        "K*(892)0"  ,2,   0,  0))
    if doKS:         particles.append(particles[-1].CP("K*(892)~0" ))
    if doJp:         particles.append(particle(        "J/psi(1S)" ,2,   0,  0))
    if doDS:         particles.append(particle(        "D*(2010)+" ,2,  +1,  0))
    if doDS:         particles.append(particles[-1].CP("D*(2010)-" ))

    #go through all combinations
    combs=[]
    if nbody==2:
        for i1,p1 in enumerate(particles):
            for i2,p2 in enumerate(particles):
                if i2<i1: continue
                combs.append((p1,p2))
    if nbody==3:
        for i1,p1 in enumerate(particles):
            for i2,p2 in enumerate(particles):
                for i3,p3 in enumerate(particles):
                    if i2<i1 or i3<i2: continue
                    combs.append((p1,p2,p3))
                    

    #alphabetically ordered name of particles
    def pname(ps):
        names=[]
        for p in ps: names.append(p.name)
        names.sort()
        result=""
        for name in names:
            result=result+name+' '
        return result[:-1]

    descriptors = []
    for comb in combs:
        #check for the number of vertex-constraining (pseudo)tracks
        nvc=0
        for p in comb: nvc+=p.nvc
        if nvc<MinNvc: continue
        #check for charge and baryon number
        Q=0
        for p in comb: Q+=p.Q
        if abs(Q)>1: continue
        B=0
        for p in comb: B+=p.B
        if abs(B)>1: continue

        #assign identity to mother 
        mother=""
        if Q==0  and B==0: mother="B0"
        if Q==1  and B==0: mother="B+"
        if Q==0  and B==1: mother="Lambda_b0"
        if Q==-1 and B==1: mother="Xi_b-"
        if Q==+1 and B==1: mother="Xi_bc+"
        if mother=="": continue

        #check if final state is CP eigenstate
        CP=False
        ps=[]
        for p in comb:
            if not p.isCP(): ps.append(p)
        if len(ps)==0:CP=True
        if len(ps)==2 and ps[0].name==ps[1].cp.name : CP=True
        
        #for B0 non-CP combinations come twice. 
        if mother=="B0" and not CP:
            combcp=[x.cp for x in comb]
            if pname(comb)<pname(combcp):continue

        #build the decay descriptor
        descriptor=mother+" -> "+pname(comb)
        if not CP:descriptor="["+descriptor+"]cc"
        #print "DESCRIPTOR:",descriptor
        descriptors.append(descriptor)


    #make a merge of the input selections
    AllSel=[]
    if doPi:AllSel.append(PiSel)
    if doK: AllSel.append(KSel)
    if dop: AllSel.append(pSel)
    if doKs:AllSel.append(KsSel)
    if doLm:AllSel.append(LmSel)
    if doDz:AllSel.append(DzSel)
    if doDp:AllSel.append(DpSel)
    if doDs:AllSel.append(DsSel)
    if doLc:AllSel.append(LcSel)
    if doPh:AllSel.append(PhSel)
    if doKS:AllSel.append(KSSel)
    if doJp:AllSel.append(JpSel)
    if doDS:AllSel.append(DSSel)
    InputSel= MergedSelection("InputFor"+name, RequiredSelections = AllSel )

    _combinationCuts =    "(in_range(%(MinBMass)s*MeV, AM, %(MaxBMass)s*MeV))" % locals()
    _combinationCuts += "& (APT>%(MinBPt)s*MeV)"%locals()
    _motherCuts = "(VFASPF(VCHI2/VDOF)<%(MaxBVertChi2DOF)s)"%locals()
    _motherCuts += "& (BPVVDCHI2 > %(MinBPVVDChi2)s)"% locals()
    _motherCuts += "& (BPVIPCHI2() < %(MaxBPVIPChi2)s)"%locals()
    _motherCuts += "& (BPVDIRA > %(MinBPVDIRA)s)"% locals()

    #make a selection
    _B=CombineParticles(DecayDescriptors = descriptors, CombinationCut = _combinationCuts, MotherCut = _motherCuts)
    return Selection(name, Algorithm = _B, RequiredSelections = [InputSel] )

from StandardParticles import StdLoosePions
def makePi(name, MinPiPt, MinPiIPChi2DV, MaxPiChi2) :
    _code =  "(PT > %(MinPiPt)s*MeV)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY) > %(MinPiIPChi2DV)s)"%locals()
    _code+="& (TRCHI2DOF<%(MaxPiChi2)s)"%locals()
    _PiFilter=FilterDesktop(Code=_code)
    return Selection (name,  Algorithm = _PiFilter, RequiredSelections = [StdLoosePions])
                        
from StandardParticles import StdLooseKaons
def makeK(name, MinKPt, MinKIPChi2DV, MaxKChi2) :
    _code =  "(PT > %(MinKPt)s*MeV)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY) > %(MinKIPChi2DV)s)"%locals()
    _code+="& (TRCHI2DOF<%(MaxKChi2)s)"%locals()
    _KFilter=FilterDesktop(Code=_code)
    return Selection (name,  Algorithm = _KFilter, RequiredSelections = [StdLooseKaons])
                        
from StandardParticles import StdLooseProtons
def makep(name, MinpPt, MinpIPChi2DV, MaxpChi2) :
    _code =  "(PT > %(MinpPt)s*MeV)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY) > %(MinpIPChi2DV)s)"%locals()
    _code+="& (TRCHI2DOF<%(MaxpChi2)s)"%locals()
    _pFilter=FilterDesktop(Code=_code)
    return Selection (name,  Algorithm = _pFilter, RequiredSelections = [StdLooseProtons])

def makeKs(name, MaxKsDeltaM, MinKsPt, MaxKsVertChi2DOF, MinKsPVVDChi2, MinKsIPChi2, MinKsDauPt, MinKsDauIPChi2, MaxKsDauTrkChi2) :
    _code =  "(ADMASS('KS0') < %(MaxKsDeltaM)s*MeV)"%locals()
    _code+="& (PT>%(MinKsPt)s*MeV)"%locals()
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxKsVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinKsPVVDChi2)s)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)>%(MinKsIPChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinKsDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinKsDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKsDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKsDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxKsDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxKsDauTrkChi2)s , 2 )"%locals()
    _KsFilter = FilterDesktop(Code = _code)
    _stdKsLL    = DataOnDemand(Location = "Phys/StdLooseKsLL/Particles")
    _stdKsDD    = DataOnDemand(Location = "Phys/StdLooseKsDD/Particles")
    _KsMerged = MergedSelection( "MergedKsFor" + name, RequiredSelections = [_stdKsDD,_stdKsLL] )
    return Selection (name, Algorithm = _KsFilter, RequiredSelections = [_KsMerged])


def makeLm(name, MaxLmDeltaM, MinLmPt, MaxLmVertChi2DOF, MinLmPVVDChi2, MinLmIPChi2, MinLmPrtPt, MinLmPiPt, MinLmPrtIPChi2, MinLmPiIPChi2, MaxLmPrtTrkChi2, MaxLmPiTrkChi2) :
    _code =  "(ADMASS('Lambda0') < %(MaxLmDeltaM)s*MeV)"%locals()
    _code+="& (PT>%(MinLmPt)s*MeV)"%locals()
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxLmVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinLmPVVDChi2)s)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)>%(MinLmIPChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinLmPrtPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinLmPiPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLmPrtIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLmPiIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxLmPrtTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxLmPiTrkChi2)s , 2 )"%locals()
    _LmFilter = FilterDesktop(Code = _code)
    _stdLmLL    = DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")
    _stdLmDD    = DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")
    _LmMerged = MergedSelection( "MergedLmFor" + name, RequiredSelections = [_stdLmDD,_stdLmLL] )
    return Selection (name, Algorithm = _LmFilter, RequiredSelections = [_LmMerged])

def makeDz(name, MaxDzDeltaM, MinDzPt, MaxDzVertChi2DOF, MinDzPVVDChi2, MinDzIPChi2, MinDzDauPt, MinDzDauIPChi2, MaxDzDauTrkChi2, MinDzPIDK):
    _code = "(PT>%(MinDzPt)s*MeV)"%locals()
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxDzVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinDzPVVDChi2)s)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)>%(MinDzIPChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinDzDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDzDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDzDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDzDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDzDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDzDauTrkChi2)s , 2 )"%locals()
    _code+="& (ADMASS('D0') < %(MaxDzDeltaM)s*MeV)"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDzPIDK)s , 1 )"%locals()
    _DzFilter = FilterDesktop(Code = _code)
    _stdDz = DataOnDemand(Location = "Phys/StdLooseD02KPi/Particles")
    return Selection (name, Algorithm = _DzFilter, RequiredSelections = [_stdDz])

def makeDp(name, MaxDpDeltaM, MinDpPt, MaxDpVertChi2DOF, MinDpPVVDChi2, MinDpIPChi2, MinDpDauPt, MinDpDauIPChi2, MaxDpDauTrkChi2, MinDpPIDK):
    _code = "(PT>%(MinDpPt)s*MeV)"%locals()
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxDpVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinDpPVVDChi2)s)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)>%(MinDpIPChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinDpDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDpDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDpDauPt)s , 3 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDpDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDpDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDpDauIPChi2)s , 3 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDpDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDpDauTrkChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDpDauTrkChi2)s , 3 )"%locals()
    _code+="& (ADMASS('D+') < %(MaxDpDeltaM)s*MeV)"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDpPIDK)s , 1 )"%locals()
    _DpFilter = FilterDesktop(Code = _code)
    _stdDp = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi/Particles")
    return Selection (name, Algorithm = _DpFilter, RequiredSelections = [_stdDp])

def makeDs(name, MaxDsDeltaM, MinDsPt, MaxDsVertChi2DOF, MinDsPVVDChi2, MinDsIPChi2, MinDsDauPt, MinDsDauIPChi2, MaxDsDauTrkChi2, MinDsPIDK):
    _code = "(PT>%(MinDsPt)s*MeV)"%locals()
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxDsVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinDsPVVDChi2)s)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)>%(MinDsIPChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinDsDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDsDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDsDauPt)s , 3 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDsDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDsDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDsDauIPChi2)s , 3 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDsDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDsDauTrkChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDsDauTrkChi2)s , 3 )"%locals()
    _code+="& (ADMASS('D_s+') < %(MaxDsDeltaM)s*MeV)"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDsPIDK)s , 1 )"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDsPIDK)s , 2 )"%locals()
    _DsFilter = FilterDesktop(Code = _code)
    _stdDs = DataOnDemand(Location = "Phys/StdLooseDplus2KKPi/Particles")
    return Selection (name, Algorithm = _DsFilter, RequiredSelections = [_stdDs])

def makeLc(name, MaxLcDeltaM, MinLcPt, MaxLcVertChi2DOF, MinLcPVVDChi2, MinLcIPChi2, MinLcDauPt, MinLcDauIPChi2, MaxLcDauTrkChi2, MinLcPIDK, MinLcPIDp):
    _code = "(PT>%(MinLcPt)s*MeV)"%locals()
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxLcVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinLcPVVDChi2)s)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)>%(MinLcIPChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinLcDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinLcDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinLcDauPt)s , 3 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLcDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLcDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLcDauIPChi2)s , 3 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxLcDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxLcDauTrkChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxLcDauTrkChi2)s , 3 )"%locals()
    _code+="& (ADMASS('Lambda_c+') < %(MaxLcDeltaM)s*MeV)"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinLcPIDK)s , 1 )"%locals()
    _code+="& CHILDCUT ( PIDp-PIDpi > %(MinLcPIDp)s , 2 )"%locals()
    _LcFilter = FilterDesktop(Code = _code)
    _stdLc = DataOnDemand(Location = "Phys/StdLooseLambdac2PKPi/Particles")
    return Selection (name, Algorithm = _LcFilter, RequiredSelections = [_stdLc])


def makePh(name, MaxPhDeltaM, MinPhPt, MaxPhVertChi2DOF, MinPhPVVDChi2, MinPhIPChi2, MinPhDauPt, MinPhDauIPChi2, MaxPhDauTrkChi2):
    _code =  "( ADMASS('phi(1020)') < %(MaxPhDeltaM)s*MeV)"%locals()
    _code+="& ( PT > %(MinPhPt)s )"%locals()
    _code+="& ( VFASPF(VCHI2/VDOF) < %(MaxPhVertChi2DOF)s )"%locals()
    _code+="& ( BPVVDCHI2 > %(MinPhPVVDChi2)s )"%locals()
    _code+="& ( MIPCHI2DV(PRIMARY) > %(MinPhIPChi2)s )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinPhDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinPhDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinPhDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinPhDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxPhDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxPhDauTrkChi2)s , 2 )"%locals()
    _PhFilter = FilterDesktop(Code = _code)
    _stdPh = DataOnDemand(Location = "Phys/StdLooseDetachedPhi2KK/Particles")
    return Selection (name, Algorithm = _PhFilter, RequiredSelections = [_stdPh])

def makeKS(name, MaxKSDeltaM, MinKSPt, MaxKSVertChi2DOF, MinKSPVVDChi2, MinKSIPChi2, MinKSDauPt, MinKSDauIPChi2, MaxKSDauTrkChi2):
    _code =  "(ADMASS('K*(892)0') < %(MaxKSDeltaM)s*MeV)"%locals()
    _code+="& ( PT > %(MinKSPt)s )"%locals()
    _code+="& ( VFASPF(VCHI2/VDOF) < %(MaxKSVertChi2DOF)s )"%locals()
    _code+="& ( BPVVDCHI2 > %(MinKSPVVDChi2)s )"%locals()
    _code+="& ( MIPCHI2DV(PRIMARY) > %(MinKSIPChi2)s )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKSDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKSDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinKSDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinKSDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxKSDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxKSDauTrkChi2)s , 2 )"%locals()
    _KSFilter = FilterDesktop(Code = _code)
    _stdKS = DataOnDemand(Location = "Phys/StdLooseDetachedKst2Kpi/Particles")
    return Selection (name, Algorithm = _KSFilter, RequiredSelections = [_stdKS])
                                                            
def makeJp(name, MaxJpDeltaM, MinJpPt, MaxJpVertChi2DOF, MinJpPVVDChi2, MinJpIPChi2, MinJpDauPt, MinJpDauIPChi2, MaxJpDauTrkChi2):
    _code =  "( ADMASS('J/psi(1S)') < %(MaxJpDeltaM)s*MeV)"%locals()
    _code+="& ( PT > %(MinJpPt)s )"%locals()
    _code+="& ( VFASPF(VCHI2/VDOF) < %(MaxJpVertChi2DOF)s )"%locals()
    _code+="& ( BPVVDCHI2 > %(MinJpPVVDChi2)s )"%locals()
    _code+="& ( MIPCHI2DV(PRIMARY) > %(MinJpIPChi2)s )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinJpDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinJpDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinJpDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinJpDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxJpDauTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxJpDauTrkChi2)s , 2 )"%locals()
    _JpFilter = FilterDesktop(Code = _code)
    _stdJp = DataOnDemand(Location = "Phys/StdLooseDiMuon/Particles")
    return Selection (name, Algorithm = _JpFilter, RequiredSelections = [_stdJp])

def makeDS(name, MaxDSDeltaM, MinDSPt, MaxDSVertChi2DOF, MinDSPVVDChi2, MinDSIPChi2, MinDSPiPt, MinDSPiIPChi2,MaxDSPiTrkChi2, MaxDSD0DeltaM,
           MinDSD0Pt, MaxDSD0VertChi2DOF, MinDSD0PVVDChi2, MinDSD0IPChi2, MinDSD0DauPt, MinDSD0DauIPChi2, MaxDSD0DauTrkChi2):
    _code =  "(M-MAXTREE('D0'==ABSID,M)<%(MaxDSDeltaM)s)"%locals()
    _code+="& ( PT > %(MinDSPt)s )"%locals()
    _code+="& ( VFASPF(VCHI2/VDOF) < %(MaxDSVertChi2DOF)s )"%locals()
    _code+="& ( BPVVDCHI2 > %(MinDSPVVDChi2)s )"%locals()
    _code+="& ( MIPCHI2DV(PRIMARY) > %(MinDSIPChi2)s )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDSPiPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDSPiIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( TRCHI2DOF < %(MaxDSPiTrkChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDSD0Pt)s , 2 )"%locals()
    _code+="& CHILDCUT ( VFASPF(VCHI2/VDOF) < %(MaxDSD0VertChi2DOF)s , 2 )"%locals()
    _code+="& CHILDCUT ( BPVVDCHI2 > %(MinDSD0PVVDChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV(PRIMARY) > %(MinDSD0IPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( ADMASS('D0') < %(MaxDSD0DeltaM)s*MeV , 2 )"%locals()
    _code+="& ( NINGENERATION ( ( PT > %(MinDSD0DauPt)s ) , 2 ) == 2 )"%locals()
    _code+="& ( NINGENERATION ( ( MIPCHI2DV ( PRIMARY ) > %(MinDSD0DauIPChi2)s ) , 2 ) == 2 )"%locals()
    _code+="& ( NINGENERATION ( ( TRCHI2DOF < %(MaxDSD0DauTrkChi2)s ) , 2 ) == 2 )"%locals()
    _DSFilter = FilterDesktop(Code = _code)
    _stdDS = DataOnDemand(Location = "Phys/StdLooseDstarWithD02KPi/Particles")
    return Selection (name, Algorithm = _DSFilter, RequiredSelections = [_stdDS])
                                                               

