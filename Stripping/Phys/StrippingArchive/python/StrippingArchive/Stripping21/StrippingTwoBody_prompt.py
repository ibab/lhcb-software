__author__ = ['Walter Bonivento']
__date__ = '19/05/2011'
__version__ = '$Revision: 1.0 $'

__all__ = ('TwoBody_promptConf',
           'makeTwoBody_prompt',
#           'makePi',
#           'makeK',
#           'makep',
#           'makeKs',
           'makeLm',
           'makeDz',
           'makeDp',
           'makeDs',
           'makeLc',
           'makePh',
           'makeJp',
           'makeDS')

confdict_TwoBody_prompt = {
       'nbody':              2,
                  'MinBMass':        3690.0,
                  'MaxBMass':        100000.0,
                  'MinBPt':          2000.0,
                  'MaxBVertChi2DOF':   10.0,
                  'MaxBPVVDCHI2Comb' : 10.,
         
                  'MinBPVDIRA' : 0.999,
                   'MinNvc':             3,
                  'MinFly':1,
 #                 'doPi':            True,
 #                  'doK':            True,
 #                  'dop':            True,
 #                 'doKs':            True,
                  'doLm':            True,
                  'doDz':            True,
                  'doDp':            True,
                  'doDs':            True,
                  'doLc':            True,
                  'doPh':            True,
                   'doJp':            True,
                  'doDS':            True,
	'MinSingleTrackPtPi' : 6000.0,
    'MinSingleTrackPtK' : 4000.0,
    'MinSingleTrackPtp' : 3500.0,  
	'MaxSingleTrackIPChi2DV' : 5.0,   # changed 
          'MinDPIDpSingle' : 10.0,
          'MinDPIDKSingle' : 5.0,
        'MinLongLivedDauPt' : 200.0,
        'MinLongLivedDauIPChi2' : 4.0,      
        'MinLongLivedPt' : 3000.0,
        'MinKsPt':2500.,
        'MaxLongLivedIPChi2' : 10.0,
        'MaxLongLivedVertChi2DOF' : 10.0,
 	'MinLongLivedPVVDChi2' : 200.0,   # halved
 	'MaxKsDeltaM' : 25.0, 
 	'MaxLmDeltaM' : 15.0, 

        'MinDPIDK' : 5.0,
        'MinDPIDp' : 5.0,
       
        'MinDPIDpK' : 5.0,
        'MinDPIDKforD0Dplus' : 0.0,
        'MinDDauPt' : 250.0,
        'MinDDauIPChi2' : 7.0,      
        'MinDPt' : 1200.0,       
        'MaxDIPChi2' : 10.0,   # changed
        'MaxDVertChi2DOF' : 10.0,
 	'MinDPVVDChi2' : 50.0,  # halved/2 
 	'MaxDDeltaM' : 30.0,

        'MinDDauIPChi2Lc': 2. ,   
        'MinDPVVDChi2Lc': 5., 


        'MinPhDauPt' : 1000.0,      
        'MaxPhDauIPChi2' : 5.0,  # changed      
        'MinPhPt' : 2500.0,      
        'MaxPhVertChi2DOF' : 10.0,
 	'MaxPhDeltaM' : 20.0, 

        'MinJpDauPt' : 500.0,      
        'MaxJpDauIPChi2' : 10.0,   # changed
        'MinJpPt' : 1000.0,      
        'MaxJpVertChi2DOF' : 10.0,
 #	'MaxJpDeltaM' : 50.0, 
 #       'MaxYDeltaM' : 1000.0,

        'MaxDSD0VertChi2DOF' : 10.0,
        'MinDSD0PVVDChi2' : 60.0,   # halved/2
        'MaxDSD0DeltaM' : 30.0,
        'MinDSPt' : 1000.0,      
        'MaxDSDeltaM':3.0,
        'MaxDSSlowPionIPChi2':7.,  # added   
        'MaxDSD0IPChi2':7.,  # added
        'MinDSD0DauPt' : 250.0, # added
        'MinDSD0DauIPChi2' : 9.0,
        'MinDSSlowPionDauPt' : 150.0, # added

#        'MinPi0RPt':3500.0,  
#        'MinPi0RMM':100.0,   
#       'MaxPi0RMM':170.0,
#        'MinPi0RGamCL':0.2,
#        'MinPi0MPt':3500.0,  # was 3500

#        'MinPhoPt:3500.0, 

                  }

#confdict_3body=confdict_2body.copy()
#confdict_3body['nbody']=3
#confdict_3body['doKS']=False
#confdict={'2body':confdict_2body, '3body':confdict_3body}


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, MergedSelection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder



class TwoBody_promptConf(LineBuilder) :
    __configuration_keys__ = (
        'nbody',
                  'MinBMass',
                  'MaxBMass',
                  'MinBPt' ,
                  'MaxBVertChi2DOF',
                  'MaxBPVVDCHI2Comb',
                   'MinBPVDIRA',
                   'MinNvc',
                  'MinFly',
#                  'doPi',
#                  'doK',
#                  'dop',
#                  'doKs',
                  'doLm',
                  'doDz',
                  'doDp',
                  'doDs',
                  'doLc',
                  'doPh',
                   'doJp',
                  'doDS',
	'MinSingleTrackPtPi',
        'MinSingleTrackPtK',
        'MinSingleTrackPtp', 
	'MaxSingleTrackIPChi2DV',   # changed 
        'MinDPIDpSingle' ,
          'MinDPIDKSingle' ,
        'MinLongLivedDauPt',
        'MinLongLivedDauIPChi2',      
        'MinLongLivedPt',
        'MinKsPt',
        'MaxLongLivedIPChi2',
        'MaxLongLivedVertChi2DOF',
 	'MinLongLivedPVVDChi2',   # halved
 	'MaxKsDeltaM', 
 	'MaxLmDeltaM' , 

        'MinDPIDK',
        'MinDPIDp',

        'MinDPIDpK',
        'MinDPIDKforD0Dplus',
        'MinDDauPt',
        'MinDDauIPChi2',      
        'MinDPt',       
        'MaxDIPChi2',   # changed
        'MaxDVertChi2DOF',
 	'MinDPVVDChi2',  # halved/2
   	'MaxDDeltaM',

        'MinDDauIPChi2Lc',   
        'MinDPVVDChi2Lc', 


        'MinPhDauPt',
        'MaxPhDauIPChi2',  # changed      
        'MinPhPt',      
        'MaxPhVertChi2DOF',
 	'MaxPhDeltaM', 

        'MinJpDauPt',      
        'MaxJpDauIPChi2',   # changed
        'MinJpPt',      
        'MaxJpVertChi2DOF',
 #	'MaxJpDeltaM', 
 #       'MaxYDeltaM',

        'MaxDSD0VertChi2DOF',
        'MinDSD0PVVDChi2',   # halved/2
        'MaxDSD0DeltaM',
        'MinDSPt',      
        'MaxDSDeltaM',
        'MaxDSSlowPionIPChi2',  # added   
        'MaxDSD0IPChi2',  # added
        'MinDSD0DauPt', # added
        'MinDSD0DauIPChi2',
        'MinDSSlowPionDauPt' # added

        )
    
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

#        self.selPi = makePi( 'PiFor'+name,
#                           MinSingleTrackPtPi  =config['MinSingleTrackPtPi'],
#                             MaxSingleTrackIPChi2DV=config['MaxSingleTrackIPChi2DV'])
#        self.selK = makeK( 'KFor'+name,
#                           MinSingleTrackPtK  =config['MinSingleTrackPtK'],
#                             MaxSingleTrackIPChi2DV=config['MaxSingleTrackIPChi2DV'],
#                                   MinDPIDKSingle =config['MinDPIDKSingle'])
#        self.selp = makep( 'pFor'+name,
#                           MinSingleTrackPtp  =config['MinSingleTrackPtp'],
#                             MaxSingleTrackIPChi2DV=config['MaxSingleTrackIPChi2DV'],
#                                   MinDPIDKSingle =config['MinDPIDKSingle'],
#                              MinDPIDpSingle =config['MinDPIDpSingle']
#                          )
        
        self.selDS = makeDS('DSFor'+name,
            MaxDSD0VertChi2DOF    =config['MaxDSD0VertChi2DOF'],
            MinDSD0PVVDChi2    =config['MinDSD0PVVDChi2'],
            MaxDSD0DeltaM    =config['MaxDSD0DeltaM'],
            MinDSPt    =config['MinDSPt'],
            MaxDSDeltaM    =config['MaxDSDeltaM'],
            MaxDSSlowPionIPChi2    =config['MaxDSSlowPionIPChi2'],
            MaxDSD0IPChi2    =config['MaxDSD0IPChi2'],
            MinDSD0DauPt    =config['MinDSD0DauPt'],
            MinDSD0DauIPChi2    =config['MinDSD0DauIPChi2'],
            MinDSSlowPionDauPt    =config['MinDSSlowPionDauPt'])
 #       self.selKs = makeKs('KsFor'+name,
 #           MinLongLivedDauPt        =config['MinLongLivedDauPt'],
 #           MinLongLivedDauIPChi2 =config['MinLongLivedDauIPChi2'],      
 #           MinLongLivedPt =config['MinLongLivedPt'],
 #           MinKsPt =config['MinKsPt'],
 #           MaxLongLivedIPChi2 =config['MaxLongLivedIPChi2'],
 #           MaxLongLivedVertChi2DOF =config['MaxLongLivedVertChi2DOF'],
# 	    MinLongLivedPVVDChi2 =config['MinLongLivedPVVDChi2'],
#            MaxKsDeltaM=config['MaxKsDeltaM'],
#            MinBPVDIRA =config['MinBPVDIRA'])
        self.selLm = makeLm('LmFor'+name,
            MinLongLivedDauPt        =config['MinLongLivedDauPt'],
            MinLongLivedDauIPChi2 =config['MinLongLivedDauIPChi2'],      
            MinLongLivedPt =config['MinLongLivedPt'],
            MinBPVDIRA =config['MinBPVDIRA'],
            MaxLongLivedIPChi2 =config['MaxLongLivedIPChi2'],
            MaxLongLivedVertChi2DOF =config['MaxLongLivedVertChi2DOF'],
 	    MinLongLivedPVVDChi2 =config['MinLongLivedPVVDChi2'],
            MaxLmDeltaM=config['MaxLmDeltaM'])
        self.selDz = makeDz('DzFor'+name,
            MinBPVDIRA =config['MinBPVDIRA'],
            MinDPIDK  =config['MinDPIDK'],
            MinDPIDp   =config['MinDPIDp'],
            MinDPIDpSingle   =config['MinDPIDpSingle'],
            MinDPIDpK   =config['MinDPIDpK'],
            MinDPIDKforD0Dplus   =config['MinDPIDKforD0Dplus'],
            MinDDauPt   =config['MinDDauPt'],
            MinDDauIPChi2   =config['MinDDauIPChi2'],      
            MinDPt   =config['MinDPt'],       
            MaxDIPChi2   =config['MaxDIPChi2'],   # changed
            MaxDVertChi2DOF   =config['MaxDVertChi2DOF'],
 	    MinDPVVDChi2   =config['MinDPVVDChi2'],  # halved/2 
 	    MaxDDeltaM   =config['MaxDDeltaM'])
            
        self.selDp = makeDp('DpFor'+name,
            MinDPIDK  =config['MinDPIDK'],
            MinDPIDp   =config['MinDPIDp'],
            MinDPIDpSingle   =config['MinDPIDpSingle'],
            MinDPIDpK   =config['MinDPIDpK'],
            MinDPIDKforD0Dplus   =config['MinDPIDKforD0Dplus'],
            MinDDauPt   =config['MinDDauPt'],
            MinDDauIPChi2   =config['MinDDauIPChi2'],      
            MinDPt   =config['MinDPt'],       
            MaxDIPChi2   =config['MaxDIPChi2'],   # changed
            MaxDVertChi2DOF   =config['MaxDVertChi2DOF'],
 	    MinDPVVDChi2   =config['MinDPVVDChi2'],  # halved/2 
 	    MaxDDeltaM   =config['MaxDDeltaM'],
            MinBPVDIRA =config['MinBPVDIRA'])
        self.selDs = makeDs('DsFor'+name,
            MinBPVDIRA =config['MinBPVDIRA'],
            MinDPIDK  =config['MinDPIDK'],
            MinDPIDp   =config['MinDPIDp'],
            MinDPIDpSingle   =config['MinDPIDpSingle'],
            MinDPIDpK   =config['MinDPIDpK'],
            MinDPIDKforD0Dplus   =config['MinDPIDKforD0Dplus'],
            MinDDauPt   =config['MinDDauPt'],
            MinDDauIPChi2   =config['MinDDauIPChi2'],      
            MinDPt   =config['MinDPt'],       
            MaxDIPChi2   =config['MaxDIPChi2'],   # changed
            MaxDVertChi2DOF   =config['MaxDVertChi2DOF'],
 	    MinDPVVDChi2   =config['MinDPVVDChi2'],  # halved/2 
 	    MaxDDeltaM   =config['MaxDDeltaM'])
        self.selLc = makeLc('LcFor'+name,
            MaxDDeltaM   =config['MaxDDeltaM'],                   
            MinDDauIPChi2Lc =config['MinDDauIPChi2Lc'],   
            MinDPVVDChi2Lc =config['MinDPVVDChi2Lc'], 
            MinBPVDIRA =config['MinBPVDIRA'],
            MinDPIDK  =config['MinDPIDK'],
            MinDPIDp   =config['MinDPIDp'],
            MinDPIDpSingle   =config['MinDPIDpSingle'],
            MinDPIDpK   =config['MinDPIDpK'],
            MinDPIDKforD0Dplus   =config['MinDPIDKforD0Dplus'],
            MinDDauPt   =config['MinDDauPt'],
            MinDPt   =config['MinDPt'],       
            MaxDIPChi2   =config['MaxDIPChi2'],   # changed
            MaxDVertChi2DOF   =config['MaxDVertChi2DOF']
 	    )                                     
        self.selPh = makePh('PhFor'+name,
            MinDPIDK=config['MinDPIDK'],
            MinPhDauPt=config['MinPhDauPt'], 
            MaxPhDauIPChi2=config['MaxPhDauIPChi2'],  
            MinPhPt=config['MinPhPt'],      
            MaxPhVertChi2DOF=config['MaxPhVertChi2DOF'],
 	    MaxPhDeltaM=config['MaxPhDeltaM'])
        self.selJp = makeJp('JpFor'+name,
           MinJpDauPt=config['MinJpDauPt'],      
            MaxJpDauIPChi2=config['MaxJpDauIPChi2'],
            MinJpPt=config['MinJpPt'],
           MaxJpVertChi2DOF=config['MaxJpVertChi2DOF'],
                            )
        self.selTwoBody_prompt = makeTwoBody_prompt(name,
#                                     PiSel=self.selPi,
#                                     KSel=self.selK,
#                                     pSel=self.selp,               
#                                     KsSel=self.selKs,
                                     LmSel=self.selLm,
                                     DzSel=self.selDz,
                                     DpSel=self.selDp,
                                     DsSel=self.selDs,
                                     LcSel=self.selLc,
                                    PhSel=self.selPh,
                                    JpSel=self.selJp,
                                     DSSel=self.selDS,
                                     nbody          =config['nbody'],       
                                     MinBMass       =config['MinBMass'],       
                                     MaxBMass       =config['MaxBMass'],       
                                     MinBPt         =config['MinBPt'],         
                                     MaxBVertChi2DOF=config['MaxBVertChi2DOF'],
                                     MaxBPVVDCHI2Comb =config['MaxBPVVDCHI2Comb'],
                                     MinNvc         =config['MinNvc'],
                                     MinFly         =config['MinFly'],
#                                     doPi           =config['doPi'],
#                                                    doK           =config['doK'],
#                                                    dop           =config['dop'],
#                                      doKs           =config['doKs'],
                                     doLm           =config['doLm'],
                                    doDz           =config['doDz'],
                                    doDp           =config['doDp'],
                                     doDs           =config['doDs'],
                                      doLc           =config['doLc'],
                                    doPh           =config['doPh'],
                                     doJp           =config['doJp'],
                                     doDS           =config['doDS'])

        self.line = StrippingLine(name+"Line", selection = self.selTwoBody_prompt)
        self.registerLine(self.line)



#def makeTwoBody_prompt(name,PiSel,KsSel,LmSel,DzSel,DpSel,DsSel,LcSel,PhSel,JpSel,DSSel,
#                nbody,MinBMass,MaxBMass,MinBPt,MaxBVertChi2DOF,MinNvc,MinFly,
#                doPi,doKs,doLm,doDz,doDp,doDs,doLc,doPh,doJp,doDS):

def makeTwoBody_prompt(name,LmSel,DzSel,DpSel,DsSel,LcSel,PhSel,JpSel,DSSel,
                nbody,MinBMass,MaxBMass,MinBPt,MaxBVertChi2DOF,MaxBPVVDCHI2Comb, MinNvc,MinFly,
                doLm,doDz,doDp,doDs,doLc,doPh,doJp,doDS):
 

    class particle:
        def __init__(self,name,fly,nvc,q,b):
            self.name=name  
            self.fly=fly
            self.nvc=nvc
            self.Q=q
            self.B=b
            self.cp=self
        def CP(self,name):
            p= particle(name,self.fly,self.nvc,-self.Q,-self.B)
            p.cp=self
            self.cp=p
            return p


    # Define the particles (skip D_s+ since it's called D+)
    particles=[]
#    if doPi: particles.append(particle(        "pi+"        , 0, 1, +1,  0))
#    if doPi: particles.append(particles[-1].CP("pi-"        ))
#    if doK: particles.append(particle(        "K+"        , 0, 1, +1,  0))
#    if doK: particles.append(particles[-1].CP("K-"        ))
#    if dop: particles.append(particle(        "p+"        , 0, 1, +1,  0))
#    if dop: particles.append(particles[-1].CP("p~-"        ))
#    if doKs: particles.append(particle(        "KS0"       , 1, 1,  0,  0))
    if doLm: particles.append(particle(        "Lambda0"   , 1, 1,  0, +1))
    if doLm: particles.append(particles[-1].CP("Lambda~0"  ))
    if doDz:particles.append(particle(        "D0"        , 1, 2,  0,  0))
    if doDz:particles.append(particles[-1].CP("D~0"       ))
    if doDp or doDs:particles.append(particle(        "D+"        , 1, 2, +1,  0))
    if doDp or doDs:particles.append(particles[-1].CP("D-"        ))
    if doLc:  particles.append(particle(        "Lambda_c+" , 1, 2, +1, +1))
    if doLc:  particles.append(particles[-1].CP("Lambda_c~-"))
    if doDS:particles.append(particle(        "D*(2010)+" , 1, 2, +1,  0))
    if doDS:particles.append(particles[-1].CP("D*(2010)-" ))
    if doJp:   particles.append(particle(        "J/psi(1S)" , 1, 2,  0,  0))
    if doPh: particles.append(particle(        "phi(1020)" , 0, 2,  0,  0))



    #go through all twobody combinations
    descriptors = []
    for i1,p1 in enumerate(particles):
        for i2,p2 in enumerate(particles[i1:]):

            nvc=p1.nvc+p2.nvc
            fly=p1.fly+p2.fly
            if nvc<MinNvc: continue 
            if fly<MinFly: continue
            #check for charge and baryon number
            Q=p1.Q+p2.Q
            B=p1.B+p2.B
            if abs(Q)>1: continue
            if abs(B)>1: continue
# resonance!
#            if Q!=0: continue
#            if B!=0: continue
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
            if p1.cp.name==p2.name:CP=True
            if p1.cp.name==p1.name and p2.cp.name==p2.name:CP=True

            #for B0 non-CP combinations come twice. 
            if mother=="B0" and not CP and p1.name+p2.name<p1.cp.name+p2.cp.name:continue

            #build the decay descriptor
            descriptor=mother+" -> "+p1.name+" "+p2.name
            if not CP:descriptor="["+descriptor+"]cc"
            #print "DESCRIPTOR:",descriptor
            descriptors.append(descriptor)

    #make a merge of the input selections
    AllSel=[]
#    if doPi:AllSel.append(PiSel)
#    if doK:AllSel.append(KSel)
#    if dop:AllSel.append(pSel)
#    if doKs:AllSel.append(KsSel)
    if doLm:AllSel.append(LmSel)
    if doDz:AllSel.append(DzSel)
    if doDp:AllSel.append(DpSel)
    if doDs:AllSel.append(DsSel)
    if doLc:AllSel.append(LcSel)
    if doPh:AllSel.append(PhSel)
    if doJp:AllSel.append(JpSel)
    if doDS:AllSel.append(DSSel)
    InputSel= MergedSelection("InputFor"+name, RequiredSelections = AllSel )

    _combinationCuts =    "(in_range(%(MinBMass)s*MeV, AM, %(MaxBMass)s*MeV))" % locals()
    _combinationCuts += "& (APT>%(MinBPt)s*MeV)"%locals()
    _motherCuts = "(VFASPF(VCHI2/VDOF)<%(MaxBVertChi2DOF)s) "%locals()
    _motherCuts += "& (BPVVDCHI2<%(MaxBPVVDCHI2Comb)s) "%locals()

    #make a selection
    _B=CombineParticles(DecayDescriptors = descriptors, CombinationCut = _combinationCuts, MotherCut = _motherCuts)
    return Selection(name, Algorithm = _B, RequiredSelections = [InputSel] )

#from StandardParticles import StdAllLoosePions
  

#def makePi(name, MinSingleTrackPtPi,MaxSingleTrackIPChi2DV) :
#    _code =" (PT > %(MinSingleTrackPtPi)s*MeV)"%locals()
#    _code +="& (MIPCHI2DV(PRIMARY) < %(MaxSingleTrackIPChi2DV)s)"%locals()
#    _PiFilter=FilterDesktop(Code=_code)
#    _stdPi = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")
#    return Selection (name,  Algorithm = _PiFilter, RequiredSelections = [_stdPi])
#def makeK(name, MinSingleTrackPtK,MaxSingleTrackIPChi2DV,MinDPIDKSingle) :
#    _code =" (PT > %(MinSingleTrackPtK)s*MeV)"%locals()
#    _code +="& (MIPCHI2DV(PRIMARY) < %(MaxSingleTrackIPChi2DV)s)"%locals()
#    _code +="& (PIDK-PIDpi > %(MinDPIDKSingle)s)"%locals()
#    _KFilter=FilterDesktop(Code=_code)
#    _stdK = DataOnDemand(Location = "Phys/StdTightKaons/Particles") 
#    return Selection (name,  Algorithm = _KFilter, RequiredSelections = [_stdK])
#def makep(name, MinSingleTrackPtp,MaxSingleTrackIPChi2DV,MinDPIDpSingle,MinDPIDKSingle) :
#    _code =" (PT > %(MinSingleTrackPtp)s*MeV)"%locals()
#    _code +="& (MIPCHI2DV(PRIMARY) < %(MaxSingleTrackIPChi2DV)s)"%locals()
#    _code +="&(PIDK-PIDpi > %(MinDPIDKSingle)s)"%locals()
#    _code +="&(PIDp-PIDpi > %(MinDPIDpSingle)s)"%locals()
#    _pFilter=FilterDesktop(Code=_code)
#    _stdp = DataOnDemand(Location = "Phys/StdTightProtons/Particles")
#    return Selection (name,  Algorithm = _pFilter, RequiredSelections = [_stdp])
def makeDS(name,           MaxDSD0VertChi2DOF,
        MinDSD0PVVDChi2 ,
        MaxDSD0DeltaM    ,
        MinDSPt    ,
        MaxDSDeltaM ,
        MaxDSSlowPionIPChi2 ,
        MaxDSD0IPChi2 ,
        MinDSD0DauPt ,
        MinDSD0DauIPChi2 ,
        MinDSSlowPionDauPt ):
    _code =  "(abs(M-MAXTREE('D0'==ABSID,M)-145.42)<%(MaxDSDeltaM)s)"%locals()
    _code+="& CHILDCUT ( VFASPF(VCHI2/VDOF) < %(MaxDSD0VertChi2DOF)s , 2 )"%locals()
    _code+="& CHILDCUT ( BPVVDCHI2 > %(MinDSD0PVVDChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( ADMASS('D0') < %(MaxDSD0DeltaM)s*MeV , 2 )"%locals()
    _code+="& ( PT > %(MinDSPt)s )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV(PRIMARY) < %(MaxDSSlowPionIPChi2)s , 1 )"%locals()     # added
    _code+="& CHILDCUT ( MIPCHI2DV(PRIMARY) < %(MaxDSD0IPChi2)s , 2 )"%locals()     # added 
    _code+="& ( NINGENERATION ( ( PT > %(MinDSD0DauPt)s ) , 2 ) == 2 )"%locals()
    _code+="& ( NINGENERATION ( ( MIPCHI2DV ( PRIMARY ) > %(MinDSD0DauIPChi2)s ) , 2 ) == 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDSSlowPionDauPt)s ,1)"%locals()     # added 
    
    _DSFilter = FilterDesktop(Code = _code)
    _stdDS = DataOnDemand(Location = "Phys/StdLooseDstarWithD02KPi/Particles")
    return Selection (name, Algorithm = _DSFilter, RequiredSelections = [_stdDS])
                                                               
#def makeKs( name, MinLongLivedDauPt,
#           MinLongLivedDauIPChi2,      
#           MinLongLivedPt ,
#             MinBPVDIRA  ,
#           MinKsPt,
#           MaxLongLivedIPChi2,
#           MaxLongLivedVertChi2DOF ,
# 	   MinLongLivedPVVDChi2,
#            MaxKsDeltaM) :
#    _code  = "(PT>%(MinLongLivedPt)s*MeV)"%locals()
#    _code+="& (MIPCHI2DV(PRIMARY)<%(MaxLongLivedIPChi2)s)"%locals()             # changed
#    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxLongLivedVertChi2DOF)s)"%locals()
#    _code+="& (BPVVDCHI2>%(MinLongLivedPVVDChi2)s)"%locals()
#    _code+="& CHILDCUT ( PT > %(MinLongLivedDauPt)s , 1 )"%locals()
#    _code+="& CHILDCUT ( PT > %(MinLongLivedDauPt)s , 2 )"%locals()
#    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLongLivedDauIPChi2)s , 1 )"%locals()
#    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLongLivedDauIPChi2)s , 2 )"%locals()
#    _code+="& (BPVDIRA> %(MinBPVDIRA)s)"%locals()
#    _code+="& (PT>%(MinKsPt)s*MeV) & (ADMASS('KS0') < %(MaxKsDeltaM)s*MeV)"%locals()


#    _KsFilter = FilterDesktop(Code = _code)
#    _stdKsLL    = DataOnDemand(Location = "Phys/StdAllLooseKsLL/Particles")
#    _stdKsDD    = DataOnDemand(Location = "Phys/StdAllLooseKsDD/Particles")
#    _KsMerged = MergedSelection( "MergedKsFor" + name, RequiredSelections = [_stdKsDD,_stdKsLL] )
#    return Selection (name, Algorithm = _KsFilter, RequiredSelections = [_KsMerged])


def makeLm(name, MinLongLivedDauPt,
           MinLongLivedDauIPChi2,      
           MinLongLivedPt ,
            MinBPVDIRA  ,
           MaxLongLivedIPChi2,
           MaxLongLivedVertChi2DOF ,
 	   MinLongLivedPVVDChi2,
            MaxLmDeltaM) :
    _code  = "(PT>%(MinLongLivedPt)s*MeV)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)<%(MaxLongLivedIPChi2)s)"%locals()             # changed
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxLongLivedVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinLongLivedPVVDChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinLongLivedDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinLongLivedDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLongLivedDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLongLivedDauIPChi2)s , 2 )"%locals()
    _code+="& (BPVDIRA> %(MinBPVDIRA)s)"%locals()
    _code+="& (ADMASS('Lambda0') < %(MaxLmDeltaM)s*MeV)"%locals()

    _LmFilter = FilterDesktop(Code = _code)
    _stdLmLL    = DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")
    _stdLmDD    = DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")
    _LmMerged = MergedSelection( "MergedLmFor" + name, RequiredSelections = [_stdLmDD,_stdLmLL] )
    return Selection (name, Algorithm = _LmFilter, RequiredSelections = [_LmMerged])

def makeDz(name,
        MinBPVDIRA  ,
        MinDPIDK,
        MinDPIDp,
        MinDPIDpSingle,
        MinDPIDpK,
        MinDPIDKforD0Dplus,
        MinDDauPt,
        MinDDauIPChi2,      
        MinDPt,       
        MaxDIPChi2,   # changed
        MaxDVertChi2DOF,
 	MinDPVVDChi2,  # halved/2 
 	MaxDDeltaM):
    _code  = "(PT>%(MinDPt)s*MeV)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)<%(MaxDIPChi2)s)"%locals()  # changed
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxDVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinDPVVDChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinDDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 2 )"%locals()
    _code+="& (BPVDIRA> %(MinBPVDIRA)s)"%locals()
    _code+="& (ADMASS('D0') < %(MaxDDeltaM)s*MeV)"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDKforD0Dplus)s , 1 )"%locals()
    _DzFilter = FilterDesktop(Code = _code)
    _stdDz = DataOnDemand(Location = "Phys/StdLooseD02KPi/Particles")
    return Selection (name, Algorithm = _DzFilter, RequiredSelections = [_stdDz])


def makeDp(name,
        MinBPVDIRA  ,
        MinDPIDK,
        MinDPIDp,
        MinDPIDpSingle,
        MinDPIDpK,
        MinDPIDKforD0Dplus,
        MinDDauPt,
        MinDDauIPChi2,      
        MinDPt,       
        MaxDIPChi2,   # changed
        MaxDVertChi2DOF,
 	MinDPVVDChi2,  # halved/2 
 	MaxDDeltaM):
    _code  = "(PT>%(MinDPt)s*MeV)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)<%(MaxDIPChi2)s)"%locals()  # changed
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxDVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinDPVVDChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinDDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 2 )"%locals()
    _code+="& (BPVDIRA> %(MinBPVDIRA)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinDDauPt)s , 3 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 3 )"%locals()
    _code+="& (ADMASS('D+') < %(MaxDDeltaM)s*MeV)"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 1 )"%locals()
    _DpFilter = FilterDesktop(Code = _code)
    _stdDp = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi/Particles")
    return Selection (name, Algorithm = _DpFilter, RequiredSelections = [_stdDp])

    
def makeDs(name,
        MinBPVDIRA  ,
        MinDPIDK,
        MinDPIDp,
        MinDPIDpSingle,
        MinDPIDpK,
        MinDPIDKforD0Dplus,
        MinDDauPt,
        MinDDauIPChi2,      
        MinDPt,       
        MaxDIPChi2,   # changed
        MaxDVertChi2DOF,
 	MinDPVVDChi2,  # halved/2 
 	MaxDDeltaM):
    _code  = "(PT>%(MinDPt)s*MeV)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)<%(MaxDIPChi2)s)"%locals()  # changed
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxDVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinDPVVDChi2)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinDDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 2 )"%locals()
    _code+="& (BPVDIRA> %(MinBPVDIRA)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinDDauPt)s , 3 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 3 )"%locals()
    _code+="& (ADMASS('D_s+') < %(MaxDDeltaM)s*MeV)"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 1 )"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 2 )"%locals()
    _DsFilter = FilterDesktop(Code = _code)
    _stdDs = DataOnDemand(Location = "Phys/StdLooseDplus2KKPi/Particles")
    return Selection (name, Algorithm = _DsFilter, RequiredSelections = [_stdDs])


def makeLc(name,      
         MinBPVDIRA  ,
        MinDPIDK,
       MinDPIDp,
        MinDPIDpSingle,
       MinDPIDpK,
       MinDPIDKforD0Dplus,
        MinDDauPt,
       MinDDauIPChi2Lc,      
        MinDPt,       
       MaxDIPChi2,   # changed
        MaxDVertChi2DOF,
 	MinDPVVDChi2Lc,  # halved/2 
	MaxDDeltaM):
    _code  = "(PT>%(MinDPt)s*MeV)"%locals()
    _code+="& (MIPCHI2DV(PRIMARY)<%(MaxDIPChi2)s)"%locals()  # changed
    _code+="& (VFASPF(VCHI2/VDOF)<%(MaxDVertChi2DOF)s)"%locals()
    _code+="& (BPVVDCHI2>%(MinDPVVDChi2Lc)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinDDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinDDauPt)s , 2 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2Lc)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2Lc)s , 2 )"%locals()
    _code+="& (BPVDIRA> %(MinBPVDIRA)s)"%locals()
    _code+="& CHILDCUT ( PT > %(MinDDauPt)s , 3 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2Lc)s , 3 )"%locals()
    _code+="& (ADMASS('Lambda_c+') < %(MaxDDeltaM)s*MeV)"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 1 )"%locals()
    _code+="& CHILDCUT ( PIDp-PIDpi > %(MinDPIDp)s , 2 )"%locals()
    _code+="& CHILDCUT ( PIDp-PIDK > %(MinDPIDpK)s , 2 )"%locals()
    _LcFilter = FilterDesktop(Code = _code)
    _stdLc = DataOnDemand(Location = "Phys/StdLooseLambdac2PKPi/Particles")
    return Selection (name, Algorithm = _LcFilter, RequiredSelections = [_stdLc])


    
def makePh(name, MinPhDauPt,
        MaxPhDauIPChi2,  
        MinPhPt,      
       MaxPhVertChi2DOF,
 	MaxPhDeltaM,
           MinDPIDK):
    _code =  "CHILDCUT ( MIPCHI2DV ( PRIMARY ) < %(MaxPhDauIPChi2)s , 1 )"%locals()
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) < %(MaxPhDauIPChi2)s , 2 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinPhDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinPhDauPt)s , 2 )"%locals()
    _code+="& ( PT > %(MinPhPt)s )"%locals()
    _code+="& ( VFASPF(VCHI2/VDOF) < %(MaxPhVertChi2DOF)s )"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 1 )"%locals()
    _code+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 2 )"%locals()
    _code+="& ( ADMASS('phi(1020)') < %(MaxPhDeltaM)s*MeV)"%locals()
    _PhFilter = FilterDesktop(Code = _code)
    _stdPh = DataOnDemand(Location = "Phys/StdLoosePhi2KK/Particles")
    return Selection (name, Algorithm = _PhFilter, RequiredSelections = [_stdPh])

                                       
def makeJp(name,        MinJpDauPt,      
        MaxJpDauIPChi2,  
         MinJpPt,      
         MaxJpVertChi2DOF,
#  	MaxJpDeltaM, 
#        MaxYDeltaM
              ):
    _code =  "CHILDCUT ( MIPCHI2DV ( PRIMARY ) < %(MaxJpDauIPChi2)s , 1 )"%locals()  # changed
    _code+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) < %(MaxJpDauIPChi2)s , 2 )"%locals()  # changed
    _code+="& CHILDCUT ( PT > %(MinJpDauPt)s , 1 )"%locals()
    _code+="& CHILDCUT ( PT > %(MinJpDauPt)s , 2 )"%locals()
    _code+="& ( PT > %(MinJpPt)s )"%locals()
    _code+="& ( VFASPF(VCHI2/VDOF) < %(MaxJpVertChi2DOF)s )"%locals()
    _code+="& ( ((M>3046*MeV)&(M<3146*MeV)) | ((M>3636*MeV)&(M<3736*MeV)) | ((M>9260*MeV)&(M<10550*MeV)) )"%locals()
    _JpFilter = FilterDesktop(Code = _code)
    _stdJp = DataOnDemand(Location = "Phys/StdLooseDiMuon/Particles")
    return Selection (name, Algorithm = _JpFilter, RequiredSelections = [_stdJp])

