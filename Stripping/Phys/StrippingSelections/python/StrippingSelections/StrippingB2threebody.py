#
# B->threebody selection for stripping. Uses selection framework. 
#
# Author: R. Oldeman
#

def B2threebodyLine(
        moduleName = "B2threebody", 

	MinBMass = 4500.0,     
	MaxBMass = 8000.0,     
        MinBPt = 2000.0,       
	MaxBVertChi2DOF = 10.0, 
	MinBPVVDChi2 = 225.0,
	MaxBPVIPChi2 = 15.0,    
        MinBPVDIRA = 0.0,
        MinNvc = 2,             # minimum number of vertex-constraining (pseudo)tracks

	MinSingleTrackPt = 2000.0,        
	MinSingleTrackIPChi2DV = 25.0,    
	MaxSingleTrackChi2 = 5.0,

 	MaxKsDeltaM = 40.0, 
        MinKsPt = 1500.0,       
        MaxKsVertChi2DOF = 10.0,
 	MinKsPVVDChi2 = 400.0, 
        MinKsIPChi2 = 0.0,
        MinKsDauPt = 200.0,
        MinKsDauIPChi2 = 16.0,      
        MaxKsDauTrkChi2 = 10.0,

 	MaxLmDeltaM = 15.0, 
        MinLmPt = 1500.0,       
        MaxLmVertChi2DOF = 10.0,
 	MinLmPVVDChi2 = 400.0, 
        MinLmIPChi2 = 0.0,
        MinLmPrtPt = 500.0,
        MinLmPiPt = 100.0,
        MinLmPrtIPChi2 = 4.0,      
        MinLmPiIPChi2 = 16.0,      
        MaxLmPrtTrkChi2 = 10.0,
        MaxLmPiTrkChi2 = 10.0,

 	MaxDDeltaM = 40.0, 
        MinDPt = 1500.0,       
        MaxDVertChi2DOF = 10.0,
 	MinDPVVDChi2 = 225.0, 
        MinDIPChi2 = 0.0,
        MinDDauPt = 250.0,
        MinDDauIPChi2 = 9.0,      
        MaxDDauTrkChi2 = 5.0,
        MinDPIDK = -5.0,
        MinDPIDp = -5.0,

 	MaxPhDeltaM = 30.0, 
        MinPhPt = 1500.0,      
        MaxPhVertChi2DOF = 10.0,
        MinPhPVVDChi2 = 100.0,
        MinPhIPChi2 = 16.0,
        MinPhDauPt = 500.0,      
        MinPhDauIPChi2 = 9.0,      
        MaxPhDauTrkChi2 = 10.0,

 	MaxJpDeltaM = 3000.0, 
        MinJpPt = 1000.0,      
        MaxJpVertChi2DOF = 10.0,
        MinJpPVVDChi2 = 16.0,
        MinJpIPChi2 = 0.0,
        MinJpDauPt = 500.0,      
        MinJpDauIPChi2 = 0.0,
        MaxJpDauTrkChi2 = 10.0,

        MaxDSDeltaM=153.0,
        MinDSPt = 1500.0,      
        MaxDSVertChi2DOF = 10.0,
        MinDSPVVDChi2 = 0.0,
        MinDSIPChi2 = 0.0,
        MinDSPiPt = 100.0,      
        MinDSPiIPChi2 = 0.0,      
        MaxDSPiTrkChi2 = 5.0,
        MaxDSD0DeltaM = 40.0,
        MinDSD0Pt = 1250.0,      
        MaxDSD0VertChi2DOF = 10.0,
        MinDSD0PVVDChi2 = 225.0,
        MinDSD0IPChi2 = 0.0,
        MinDSD0DauPt = 250.0,      
        MinDSD0DauIPChi2 = 9.0,      
        MaxDSD0DauTrkChi2 = 5.0,

        MinPi0RPt=2000.0,
        MinPi0RMM=100.0,
        MaxPi0RMM=170.0,
        MinPi0RGamCL=0.2,
        MinPi0MPt=3000.0,

        MinPhoPt=3000.0,
   ) : 

    from Configurables import CombineParticles, FilterDesktop, LoKi__VoidFilter
    from StrippingConf.StrippingLine import StrippingLine
    from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand, MergedSelection, EventSelection

    # Define the shared cuts
    Bcombcut =    "(in_range(%(MinBMass)s*MeV, AM, %(MaxBMass)s*MeV))" % locals()
    Bcombcut += "& (APT>%(MinBPt)s*MeV)"%locals()
    Bcut = "(VFASPF(VCHI2/VDOF)<%(MaxBVertChi2DOF)s)"%locals()
    Bcut += "& (BPVVDCHI2 > %(MinBPVVDChi2)s)"% locals()
    Bcut += "& (BPVIPCHI2() < %(MaxBPVIPChi2)s)"%locals()
    Bcut += "& (BPVDIRA > %(MinBPVDIRA)s)"% locals()

    # Standard particles 
    StdPi      = DataOnDemand(Location = "Phys/StdLoosePions")
    StdK       = DataOnDemand(Location = "Phys/StdLooseKaons")
    Stdp       = DataOnDemand(Location = "Phys/StdLooseProtons")
    StdKsLL    = DataOnDemand(Location = "Phys/StdLooseKsLL")
    StdKsDD    = DataOnDemand(Location = "Phys/StdLooseKsDD")
    StdLmLL    = DataOnDemand(Location = "Phys/StdLooseLambdaLL")
    StdLmDD    = DataOnDemand(Location = "Phys/StdLooseLambdaDD")
    StdD0      = DataOnDemand(Location = "Phys/StdLooseD02KPi")
    StdDp      = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi")
    StdDs      = DataOnDemand(Location = "Phys/StdLooseDplus2KKPi")
    StdLc      = DataOnDemand(Location = "Phys/StdLooseLambdac2PKPi")
    StdDS      = DataOnDemand(Location = "Phys/StdLooseDstarWithD02KPi")
    StdJp      = DataOnDemand(Location = "Phys/StdLooseDiMuon")
    StdPh      = DataOnDemand(Location = "Phys/StdLooseDetachedPhi2KK")
    StdPi0M    = DataOnDemand(Location = "Phys/StdLooseMergedPi0")
    StdPi0R    = DataOnDemand(Location = "Phys/StdLooseResolvedPi0")
    StdPho     = DataOnDemand(Location = "Phys/StdLooseAllPhotons")

    # SingleTrack kaon/pion/proton
    SingleTrack = FilterDesktop("SingleTrackFor" + moduleName)
    SingleTrackcut  =  "(PT > %(MinSingleTrackPt)s*MeV)"%locals()
    SingleTrackcut +="& (MIPCHI2DV(PRIMARY) > %(MinSingleTrackIPChi2DV)s)"%locals()
    SingleTrackcut +="& (TRCHI2DOF<%(MaxSingleTrackChi2)s)"%locals()
    SingleTrack.Code = SingleTrackcut
    PiSel = Selection("PiSelFor" + moduleName, Algorithm = SingleTrack, RequiredSelections = [ StdPi ] )
    KSel  = Selection("KSelFor"  + moduleName, Algorithm = SingleTrack, RequiredSelections = [ StdK  ] )
    pSel  = Selection("pSelFor"  + moduleName, Algorithm = SingleTrack, RequiredSelections = [ Stdp  ] )

    # Long-lived strange hadrons
    Ks=FilterDesktop("KsFor" + moduleName)
    Kscut =  "(ADMASS('KS0') < %(MaxKsDeltaM)s*MeV)"%locals()
    Kscut+="& (PT>%(MinKsPt)s*MeV)"%locals()
    Kscut+="& (VFASPF(VCHI2/VDOF)<%(MaxKsVertChi2DOF)s)"%locals()
    Kscut+="& (BPVVDCHI2>%(MinKsPVVDChi2)s)"%locals()
    Kscut+="& (MIPCHI2DV(PRIMARY)>%(MinKsIPChi2)s)"%locals()
    Kscut+="& CHILDCUT ( PT > %(MinKsDauPt)s , 1 )"%locals()
    Kscut+="& CHILDCUT ( PT > %(MinKsDauPt)s , 2 )"%locals()
    Kscut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKsDauIPChi2)s , 1 )"%locals()
    Kscut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKsDauIPChi2)s , 2 )"%locals()
    Kscut+="& CHILDCUT ( TRCHI2DOF < %(MaxKsDauTrkChi2)s , 1 )"%locals()
    Kscut+="& CHILDCUT ( TRCHI2DOF < %(MaxKsDauTrkChi2)s , 2 )"%locals()
    Ks.Code = Kscut
    KsMerged = MergedSelection( "MergedKS0For" + moduleName, RequiredSelections = [StdKsDD,StdKsLL] )
    KsSel=Selection("KsSelFor" + moduleName, Algorithm = Ks, RequiredSelections = [ KsMerged ] )

    Lm=FilterDesktop("LmFor" + moduleName)
    Lmcut =  "(ADMASS('Lambda0') < %(MaxLmDeltaM)s*MeV)"%locals()
    Lmcut+="& (PT>%(MinLmPt)s*MeV)"%locals()
    Lmcut+="& (VFASPF(VCHI2/VDOF)<%(MaxLmVertChi2DOF)s)"%locals()
    Lmcut+="& (BPVVDCHI2>%(MinLmPVVDChi2)s)"%locals()
    Lmcut+="& (MIPCHI2DV(PRIMARY)>%(MinLmIPChi2)s)"%locals()
    Lmcut+="& CHILDCUT ( PT > %(MinLmPrtPt)s , 1 )"%locals()
    Lmcut+="& CHILDCUT ( PT > %(MinLmPiPt)s , 2 )"%locals()
    Lmcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLmPrtIPChi2)s , 1 )"%locals()
    Lmcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLmPiIPChi2)s , 2 )"%locals()
    Lmcut+="& CHILDCUT ( TRCHI2DOF < %(MaxLmPrtTrkChi2)s , 1 )"%locals()
    Lmcut+="& CHILDCUT ( TRCHI2DOF < %(MaxLmPiTrkChi2)s , 2 )"%locals()
    Lm.Code = Lmcut
    LmMerged = MergedSelection( "MergedLmFor" + moduleName, RequiredSelections = [StdLmDD,StdLmLL] )
    LmSel=Selection("LmSelFor" + moduleName, Algorithm = Lm, RequiredSelections = [ LmMerged ] )

    # Charm hadrons
    Dz = FilterDesktop("DzFor" + moduleName)
    Dp = FilterDesktop("DpFor" + moduleName)
    Ds = FilterDesktop("DsFor" + moduleName)
    Lc = FilterDesktop("LcFor" + moduleName)
    Charm2Bcut  = "(PT>%(MinDPt)s*MeV)"%locals()
    Charm2Bcut+="& (VFASPF(VCHI2/VDOF)<%(MaxDVertChi2DOF)s)"%locals()
    Charm2Bcut+="& (BPVVDCHI2>%(MinDPVVDChi2)s)"%locals()
    Charm2Bcut+="& (MIPCHI2DV(PRIMARY)>%(MinDIPChi2)s)"%locals()
    Charm2Bcut+="& CHILDCUT ( PT > %(MinDDauPt)s , 1 )"%locals()
    Charm2Bcut+="& CHILDCUT ( PT > %(MinDDauPt)s , 2 )"%locals()
    Charm2Bcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 1 )"%locals()
    Charm2Bcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 2 )"%locals()
    Charm2Bcut+="& CHILDCUT ( TRCHI2DOF < %(MaxDDauTrkChi2)s , 1 )"%locals()
    Charm2Bcut+="& CHILDCUT ( TRCHI2DOF < %(MaxDDauTrkChi2)s , 2 )"%locals()
    Charm3Bcut=Charm2Bcut
    Charm3Bcut+="& CHILDCUT ( PT > %(MinDDauPt)s , 3 )"%locals()
    Charm3Bcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 3 )"%locals()
    Charm3Bcut+="& CHILDCUT ( TRCHI2DOF < %(MaxDDauTrkChi2)s , 3 )"%locals()
    Dzcut = Charm2Bcut+"& (ADMASS('D0') < %(MaxDDeltaM)s*MeV)"%locals()
    Dpcut = Charm3Bcut+"& (ADMASS('D+') < %(MaxDDeltaM)s*MeV)"%locals()
    Dscut = Charm3Bcut+"& (ADMASS('D_s+') < %(MaxDDeltaM)s*MeV)"%locals()
    Lccut = Charm3Bcut+"& (ADMASS('Lambda_c+') < %(MaxDDeltaM)s*MeV)"%locals()
    Dzcut+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 1 )"%locals()
    Dpcut+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 1 )"%locals()
    Dscut+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 1 )"%locals()
    Dscut+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 2 )"%locals()
    Lccut+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 1 )"%locals()
    Lccut+="& CHILDCUT ( PIDp-PIDpi > %(MinDPIDp)s , 2 )"%locals()
    Dz.Code = Dzcut
    Dp.Code = Dpcut
    Ds.Code = Dscut
    Lc.Code = Lccut
    DzSel  = Selection("DzSelFor" + moduleName,  Algorithm = Dz, RequiredSelections = [ StdD0  ] )
    DpSel  = Selection("DpSelFor" + moduleName,  Algorithm = Dp, RequiredSelections = [ StdDp  ] )
    DsSel  = Selection("DsSelFor" + moduleName,  Algorithm = Ds, RequiredSelections = [ StdDs  ] )
    LcSel  = Selection("LcSelFor" + moduleName,  Algorithm = Lc, RequiredSelections = [ StdLc  ] )

    # Light resonances 
    Ph = FilterDesktop("PhFor" + moduleName)
    Phcut =  "( ADMASS('phi(1020)') < %(MaxPhDeltaM)s*MeV)"%locals()
    Phcut+="& ( PT > %(MinPhPt)s )"%locals()
    Phcut+="& ( VFASPF(VCHI2/VDOF) < %(MaxPhVertChi2DOF)s )"%locals()
    Phcut+="& ( BPVVDCHI2 > %(MinPhPVVDChi2)s )"%locals()
    Phcut+="& ( MIPCHI2DV(PRIMARY) > %(MinPhIPChi2)s )"%locals()
    Phcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinPhDauIPChi2)s , 1 )"%locals()
    Phcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinPhDauIPChi2)s , 2 )"%locals()
    Phcut+="& CHILDCUT ( PT > %(MinPhDauPt)s , 1 )"%locals()
    Phcut+="& CHILDCUT ( PT > %(MinPhDauPt)s , 2 )"%locals()
    Phcut+="& CHILDCUT ( TRCHI2DOF < %(MaxPhDauTrkChi2)s , 1 )"%locals()
    Phcut+="& CHILDCUT ( TRCHI2DOF < %(MaxPhDauTrkChi2)s , 2 )"%locals()
    Ph.Code = Phcut
    PhSel  = Selection("PhSelFor" + moduleName,  Algorithm = Ph, RequiredSelections = [ StdPh  ] )

    # Heavy resonances 
    Jp = FilterDesktop("JpFor" + moduleName)
    Jpcut =  "( ADMASS('J/psi(1S)') < %(MaxJpDeltaM)s*MeV)"%locals()
    Jpcut+="& ( PT > %(MinJpPt)s )"%locals()
    Jpcut+="& ( VFASPF(VCHI2/VDOF) < %(MaxJpVertChi2DOF)s )"%locals()
    Jpcut+="& ( BPVVDCHI2 > %(MinJpPVVDChi2)s )"%locals()
    Jpcut+="& ( MIPCHI2DV(PRIMARY) > %(MinJpIPChi2)s )"%locals()
    Jpcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinJpDauIPChi2)s , 1 )"%locals()
    Jpcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinJpDauIPChi2)s , 2 )"%locals()
    Jpcut+="& CHILDCUT ( PT > %(MinJpDauPt)s , 1 )"%locals()
    Jpcut+="& CHILDCUT ( PT > %(MinJpDauPt)s , 2 )"%locals()
    Jpcut+="& CHILDCUT ( TRCHI2DOF < %(MaxJpDauTrkChi2)s , 1 )"%locals()
    Jpcut+="& CHILDCUT ( TRCHI2DOF < %(MaxJpDauTrkChi2)s , 2 )"%locals()
    Jp.Code = Jpcut
    JpSel  = Selection("JpSelFor" + moduleName,  Algorithm = Jp, RequiredSelections = [ StdJp  ] )

    DS = FilterDesktop("DSFor" + moduleName)
    DScut =  "(M-MAXTREE('D0'==ABSID,M)<%(MaxDSDeltaM)s)"%locals()
    DScut+="& ( PT > %(MinDSPt)s )"%locals()
    DScut+="& ( VFASPF(VCHI2/VDOF) < %(MaxDSVertChi2DOF)s )"%locals()
    DScut+="& ( BPVVDCHI2 > %(MinDSPVVDChi2)s )"%locals()
    DScut+="& ( MIPCHI2DV(PRIMARY) > %(MinDSIPChi2)s )"%locals()
    DScut+="& CHILDCUT ( PT > %(MinDSPiPt)s , 1 )"%locals()
    DScut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDSPiIPChi2)s , 1 )"%locals()
    DScut+="& CHILDCUT ( TRCHI2DOF < %(MaxDSPiTrkChi2)s , 1 )"%locals()
    DScut+="& CHILDCUT ( PT > %(MinDSD0Pt)s , 2 )"%locals()
    DScut+="& CHILDCUT ( VFASPF(VCHI2/VDOF) < %(MaxDSD0VertChi2DOF)s , 2 )"%locals()
    DScut+="& CHILDCUT ( BPVVDCHI2 > %(MinDSD0PVVDChi2)s , 2 )"%locals()
    DScut+="& CHILDCUT ( MIPCHI2DV(PRIMARY) > %(MinDSD0IPChi2)s , 2 )"%locals()
    DScut+="& CHILDCUT ( ADMASS('D0') < %(MaxDSD0DeltaM)s*MeV , 2 )"%locals()
    DScut+="& ( NINGENERATION ( ( PT > %(MinDSD0DauPt)s ) , 2 ) == 2 )"%locals()
    DScut+="& ( NINGENERATION ( ( MIPCHI2DV ( PRIMARY ) > %(MinDSD0DauIPChi2)s ) , 2 ) == 2 )"%locals()
    DScut+="& ( NINGENERATION ( ( TRCHI2DOF < %(MaxDSD0DauTrkChi2)s ) , 2 ) == 2 )"%locals()
    DS.Code = DScut
    DSSel  = Selection("DSSelFor" + moduleName,  Algorithm = DS, RequiredSelections = [ StdDS  ] )

    #neutrals
    Pi0R=FilterDesktop("Pi0RFor" + moduleName)
    Pi0Rcut =  " (PT > %(MinPi0RPt)s )"%locals()
    Pi0Rcut+= "& (MM > %(MinPi0RMM)s )"%locals()
    Pi0Rcut+= "& (MM < %(MaxPi0RMM)s )"%locals()
    Pi0Rcut+= "& (CHILD(CL,1)>%(MinPi0RGamCL)s )"%locals()
    Pi0Rcut+= "& (CHILD(CL,2)>%(MinPi0RGamCL)s )"%locals()
    Pi0R.Code=Pi0Rcut
    Pi0RSel = Selection("Pi0RSelFor" + moduleName, Algorithm =Pi0R, RequiredSelections = [StdPi0R] )
    Pi0M=FilterDesktop("Pi0MFor" + moduleName)
    Pi0Mcut = " (PT > %(MinPi0MPt)s )"%locals()
    Pi0M.Code=Pi0Mcut
    Pi0MSel = Selection("Pi0MSelFor" + moduleName, Algorithm =Pi0M, RequiredSelections = [StdPi0M] )
    Pi0Sel= MergedSelection("Pi0For" + moduleName, RequiredSelections = [Pi0RSel,Pi0MSel] )

    Pho=FilterDesktop("PhoFor" + moduleName)
    Phocut =  " (PT > %(MinPhoPt)s )"%locals()
    Pho.Code=Phocut
    PhoSel = Selection("PhoSelFor" + moduleName, Algorithm =Pho, RequiredSelections = [StdPho] )
    
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
    particles.append(particle(        "pi+"       ,1,  +1,  0))
    particles.append(particles[-1].CP("pi-"       ))
    particles.append(particle(        "K+"        ,1,  +1,  0))
    particles.append(particles[-1].CP("K-"        ))
    particles.append(particle(        "p+"        ,1,  +1, +1))
    particles.append(particles[-1].CP("p~-"       ))
    particles.append(particle(        "KS0"       ,0,   0,  0))
    particles.append(particle(        "Lambda0"   ,0,   0, +1))
    particles.append(particles[-1].CP("Lambda~0"  ))
    particles.append(particle(        "D0"        ,1,   0,  0))
    particles.append(particles[-1].CP("D~0"       ))
    particles.append(particle(        "D+"        ,1,  +1,  0))
    particles.append(particles[-1].CP("D-"        ))
    particles.append(particle(        "Lambda_c+" ,1,  +1, +1))
    particles.append(particles[-1].CP("Lambda_c~-"))
    particles.append(particle(        "D*(2010)+" ,2,  +1,  0))
    particles.append(particles[-1].CP("D*(2010)-" ))
    particles.append(particle(        "J/psi(1S)" ,2,   0,  0))
    particles.append(particle(        "phi(1020)" ,2,   0,  0))
    #particles.append(particle(        "K*(892)0"  ,2,   0,  0))
    #particles.append(particles[-1].CP("K*(892)~0" ))
    particles.append(particle(        "pi0"       ,0,   0,  0))
    particles.append(particle(        "gamma"     ,0,   0,  0))

    #alphabetically ordered name of particles
    def pname(ps):
        names=[]
        for p in ps: names.append(p.name)
        names.sort()
        result=""
        for name in names:
            result=result+name+' '
        return result[:-1]

    #go through all threebody combinations
    descriptors = []
    for i1,p1 in enumerate(particles):
        for i2,p2 in enumerate(particles):
            for i3,p3 in enumerate(particles):
                if i2<i1 or i3<i2: continue

                #check for the number of vertex-constraining (pseudo)tracks
                nvc=p1.nvc+p2.nvc+p3.nvc
                if nvc<MinNvc: continue
                #check for charge and baryon number
                Q=p1.Q+p2.Q+p3.Q
                B=p1.B+p2.B+p3.B
                if abs(Q)>1: continue
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
                if p1.cp.name==p2.name and p3.isCP(): CP=True
                if p1.cp.name==p3.name and p2.isCP(): CP=True
                if p2.cp.name==p3.name and p1.isCP(): CP=True
                if p1.isCP() and p2.isCP() and p3.isCP(): CP=True
                
                #for B0 non-CP combinations come twice. 
                if mother=="B0" and not CP:
                    if pname((p1,p2,p3))<pname((p1.cp,p2.cp,p3.cp)):continue
                
                #build the decay descriptor
                descriptor=mother+" -> "+pname((p1,p2,p3))
                if not CP:descriptor="["+descriptor+"]cc"
                #print "DESCRIPTOR:",descriptor
                descriptors.append(descriptor)

    #make a merge of the input selections
    AllRSel=[PiSel,KsSel,LmSel,DzSel,DpSel,DsSel,LcSel,DSSel,JpSel,PhSel,Pi0Sel,PhoSel]
    InputRSel= MergedSelection("RInputForB2threebody", RequiredSelections = AllRSel )
    InputSel= MergedSelection("InputForB2threebody", RequiredSelections = [InputRSel,KSel,pSel] )

    #make a selection
    B=CombineParticles("BFor"+moduleName)
    B.DecayDescriptors = descriptors
    B.CombinationCut = Bcombcut
    B.MotherCut = Bcut
    
    Bpresel= EventSelection(
        LoKi__VoidFilter("BpreselFor"+moduleName,
                         Code=" (CONTAINS('%s') > 2.5) " %  ( InputRSel.outputLocation() ) ),
        RequiredSelection=InputSel
    )
    BSel=Selection("BSelFor"+moduleName, Algorithm = B, RequiredSelections = [Bpresel] )
    line = StrippingLine("B2threebodyLine",  algos = [BSel] )

    return [line]
