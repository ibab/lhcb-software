#
# B->twobody selection for stripping. Uses selection framework. 
#
# Author: R. Oldeman
#

def B2twobodyLine(
        moduleName = "B2twobody", 

        MinBPt = 2000.0,       
	MaxBVertChi2DOF = 15.0, 
	MaxBPVIPChi2 = 15.0,    
	MinBPVVDChi2 = 225.0,
        MinBPVDIRA = 0.0,
	MinBMass = 4500.0,     
	MaxBMass = 8000.0,     

	MaxSingleTrackChi2 = 5.0,
	MinSingleTrackPt = 2000.0,        
	MinSingleTrackIPChi2DV = 25.0,    

        MinLongLivedDauPt = 200.0,
        MinLongLivedDauIPChi2 = 9.0,      
        MaxLongLivedDauTrkChi2 = 10.0,
        MinLongLivedPt = 1500.0,       
        MinLongLivedIPChi2 = 0.0,
        MaxLongLivedVertChi2DOF = 10.0,
 	MinLongLivedPVVDChi2 = 400.0, 
 	MaxKsDeltaM = 40.0, 
 	MaxLmDeltaM = 20.0, 

        MinDPIDK = -5.0,
        MinDPIDp = -5.0,
        MinDDauPt = 250.0,
        MinDDauIPChi2 = 9.0,      
        MaxDDauTrkChi2 = 5.0,
        MinDPt = 2000.0,       
        MinDIPChi2 = 0.0,
        MaxDVertChi2DOF = 10.0,
 	MinDPVVDChi2 = 225.0, 
 	MaxDDeltaM = 40.0, 

        MinKSDauPt = 500.0,      
        MinKSDauIPChi2 = 16.0,      
        MaxKSDauTrkChi2 = 5.0,
        MinKSPt = 2000.0,      
        MinKSIPChi2 = 16.0,
        MaxKSVertChi2DOF = 10.0,
        MinKSPVVDChi2 = 100.0,
 	MaxKSDeltaM = 150.0, 

        MinPhDauPt = 500.0,      
        MinPhDauIPChi2 = 9.0,      
        MaxPhDauTrkChi2 = 10.0,
        MinPhPt = 1500.0,      
        MinPhIPChi2 = 16.0,
        MaxPhVertChi2DOF = 10.0,
        MinPhPVVDChi2 = 100.0,
 	MaxPhDeltaM = 30.0, 

        MinJpDauPt = 500.0,      
        MinJpDauIPChi2 = 0.0,
        MaxJpDauTrkChi2 = 10.0,
        MinJpPt = 1000.0,      
        MinJpIPChi2 = 0.0,
        MaxJpVertChi2DOF = 10.0,
        MinJpPVVDChi2 = 0.0,
 	MaxJpDeltaM = 3000.0, 

        MaxDSD0VertChi2DOF = 10.0,
        MinDSD0PVVDChi2 = 225.0,
        MaxDSD0DeltaM = 40.0,
        MinDSPt = 2000.0,      
        MaxDSDeltaM=155.0,

   ) : 

    from Configurables import CombineParticles, FilterDesktop
    from StrippingConf.StrippingLine import StrippingLine
    from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand, MergedSelection

    # Define the shared cuts
    Bcombcut =    "(in_range(%(MinBMass)s*MeV, AM, %(MaxBMass)s*MeV))" % locals()
    Bcombcut += "& (APT>%(MinBPt)s*MeV)"%locals()
    Bcut = "(VFASPF(VCHI2/VDOF)<%(MaxBVertChi2DOF)s)"%locals()
    Bcut += "& (BPVIPCHI2() < %(MaxBPVIPChi2)s)"%locals()
    Bcut += "& (BPVVDCHI2 > %(MinBPVVDChi2)s)"% locals()
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
    StdLc      = DataOnDemand(Location = "Phys/StdVeryLooseLambdac2PKPi")
    StdDS      = DataOnDemand(Location = "Phys/StdLooseDstarWithD02KPi")
    StdJp      = DataOnDemand(Location = "Phys/StdLooseDiMuon")
    StdPh      = DataOnDemand(Location = "Phys/StdLooseDetachedPhi2KK")
    StdKS      = DataOnDemand(Location = "Phys/StdLooseDetachedKst2Kpi")

    # SingleTrack kaon/pion/proton
    SingleTrack = FilterDesktop("SingleTrackFor" + moduleName)
    SingleTrackcut =   "(TRCHI2DOF<%(MaxSingleTrackChi2)s)"%locals()
    SingleTrackcut +="& (PT > %(MinSingleTrackPt)s*MeV)"%locals()
    SingleTrackcut +="& (MIPCHI2DV(PRIMARY) > %(MinSingleTrackIPChi2DV)s)"%locals()
    SingleTrack.Code = SingleTrackcut
    PiSel = Selection("PiSelFor" + moduleName, Algorithm = SingleTrack, RequiredSelections = [ StdPi ] )
    KSel  = Selection("KSelFor"  + moduleName, Algorithm = SingleTrack, RequiredSelections = [ StdK  ] )
    pSel  = Selection("pSelFor"  + moduleName, Algorithm = SingleTrack, RequiredSelections = [ Stdp  ] )

    # Long-lived strange hadrons
    Ks=FilterDesktop("KsFor" + moduleName)
    Lm=FilterDesktop("LmFor" + moduleName)
    LongLivedcut  = "(PT>%(MinLongLivedPt)s*MeV)"%locals()
    LongLivedcut+="& (MIPCHI2DV(PRIMARY)>%(MinLongLivedIPChi2)s)"%locals()
    LongLivedcut+="& (VFASPF(VCHI2/VDOF)<%(MaxLongLivedVertChi2DOF)s)"%locals()
    LongLivedcut+="& (BPVVDCHI2>%(MinLongLivedPVVDChi2)s)"%locals()
    LongLivedcut+="& CHILDCUT ( PT > %(MinLongLivedDauPt)s , 1 )"%locals()
    LongLivedcut+="& CHILDCUT ( PT > %(MinLongLivedDauPt)s , 2 )"%locals()
    LongLivedcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLongLivedDauIPChi2)s , 1 )"%locals()
    LongLivedcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLongLivedDauIPChi2)s , 2 )"%locals()
    LongLivedcut+="& CHILDCUT ( TRCHI2DOF < %(MaxLongLivedDauTrkChi2)s , 1 )"%locals()
    LongLivedcut+="& CHILDCUT ( TRCHI2DOF < %(MaxLongLivedDauTrkChi2)s , 2 )"%locals()
    Kscut = LongLivedcut+"& (ADMASS('KS0') < %(MaxKsDeltaM)s*MeV)"%locals()
    Lmcut = LongLivedcut+"& (ADMASS('Lambda0') < %(MaxLmDeltaM)s*MeV)"%locals()
    Ks.Code = Kscut
    Lm.Code = Lmcut
    KsSel=Selection("KsSelFor" + moduleName, Algorithm = Ks, RequiredSelections = [ StdKsDD,StdKsLL ] )
    LmSel=Selection("LmSelFor" + moduleName, Algorithm = Lm, RequiredSelections = [ StdLmDD,StdLmLL ] )

    # Charm hadrons
    Dz = FilterDesktop("DzFor" + moduleName)
    Dp = FilterDesktop("DpFor" + moduleName)
    Ds = FilterDesktop("DsFor" + moduleName)
    Lc = FilterDesktop("LcFor" + moduleName)
    Charm2Bcut  = "(PT>%(MinDPt)s*MeV)"%locals()
    Charm2Bcut+="& (MIPCHI2DV(PRIMARY)>%(MinDIPChi2)s)"%locals()
    Charm2Bcut+="& (VFASPF(VCHI2/VDOF)<%(MaxDVertChi2DOF)s)"%locals()
    Charm2Bcut+="& (BPVVDCHI2>%(MinDPVVDChi2)s)"%locals()
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
    Phcut =  "CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinPhDauIPChi2)s , 1 )"%locals()
    Phcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinPhDauIPChi2)s , 2 )"%locals()
    Phcut+="& CHILDCUT ( PT > %(MinPhDauPt)s , 1 )"%locals()
    Phcut+="& CHILDCUT ( PT > %(MinPhDauPt)s , 2 )"%locals()
    Phcut+="& CHILDCUT ( TRCHI2DOF < %(MaxPhDauTrkChi2)s , 1 )"%locals()
    Phcut+="& CHILDCUT ( TRCHI2DOF < %(MaxPhDauTrkChi2)s , 2 )"%locals()
    Phcut+="& ( PT > %(MinPhPt)s )"%locals()
    Phcut+="& ( MIPCHI2DV(PRIMARY) > %(MinPhIPChi2)s )"%locals()
    Phcut+="& ( VFASPF(VCHI2/VDOF) < %(MaxPhVertChi2DOF)s )"%locals()
    Phcut+="& ( BPVVDCHI2 > %(MinPhPVVDChi2)s )"%locals()
    Phcut+="& ( ADMASS('phi(1020)') < %(MaxPhDeltaM)s*MeV)"%locals()
    Ph.Code = Phcut
    PhSel  = Selection("PhSelFor" + moduleName,  Algorithm = Ph, RequiredSelections = [ StdPh  ] )

    KS = FilterDesktop("KSFor" + moduleName)
    KScut =  "CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKSDauIPChi2)s , 1 )"%locals()
    KScut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKSDauIPChi2)s , 2 )"%locals()
    KScut+="& CHILDCUT ( PT > %(MinKSDauPt)s , 1 )"%locals()
    KScut+="& CHILDCUT ( PT > %(MinKSDauPt)s , 2 )"%locals()
    KScut+="& CHILDCUT ( TRCHI2DOF < %(MaxKSDauTrkChi2)s , 1 )"%locals()
    KScut+="& CHILDCUT ( TRCHI2DOF < %(MaxKSDauTrkChi2)s , 2 )"%locals()
    KScut+="& ( PT > %(MinKSPt)s )"%locals()
    KScut+="& ( MIPCHI2DV(PRIMARY) > %(MinKSIPChi2)s )"%locals()
    KScut+="& ( VFASPF(VCHI2/VDOF) < %(MaxKSVertChi2DOF)s )"%locals()
    KScut+="& ( BPVVDCHI2 > %(MinKSPVVDChi2)s )"%locals()
    KScut+="& (ADMASS('K*(892)0') < %(MaxKSDeltaM)s*MeV)"%locals()
    KS.Code = KScut
    KSSel  = Selection("KSSelFor" + moduleName,  Algorithm = KS, RequiredSelections = [ StdKS  ] )


    # Heavy resonances 
    Jp = FilterDesktop("JpFor" + moduleName)
    Jpcut =  "CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinJpDauIPChi2)s , 1 )"%locals()
    Jpcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinJpDauIPChi2)s , 2 )"%locals()
    Jpcut+="& CHILDCUT ( PT > %(MinJpDauPt)s , 1 )"%locals()
    Jpcut+="& CHILDCUT ( PT > %(MinJpDauPt)s , 2 )"%locals()
    Jpcut+="& CHILDCUT ( TRCHI2DOF < %(MaxJpDauTrkChi2)s , 1 )"%locals()
    Jpcut+="& CHILDCUT ( TRCHI2DOF < %(MaxJpDauTrkChi2)s , 2 )"%locals()
    Jpcut+="& ( PT > %(MinJpPt)s )"%locals()
    Jpcut+="& ( MIPCHI2DV(PRIMARY) > %(MinJpIPChi2)s )"%locals()
    Jpcut+="& ( VFASPF(VCHI2/VDOF) < %(MaxJpVertChi2DOF)s )"%locals()
    Jpcut+="& ( BPVVDCHI2 > %(MinJpPVVDChi2)s )"%locals()
    Jpcut+="& ( ADMASS('J/psi(1S)') < %(MaxJpDeltaM)s*MeV)"%locals()
    Jp.Code = Jpcut
    JpSel  = Selection("JpSelFor" + moduleName,  Algorithm = Jp, RequiredSelections = [ StdJp  ] )

    DS = FilterDesktop("DSFor" + moduleName)
    DScut =  "(M-MAXTREE('D0'==ABSID,M)<%(MaxDSDeltaM)s)"%locals()
    DScut+="& CHILDCUT ( VFASPF(VCHI2/VDOF) < %(MaxDSD0VertChi2DOF)s , 2 )"%locals()
    DScut+="& CHILDCUT ( BPVVDCHI2 > %(MinDSD0PVVDChi2)s , 2 )"%locals()
    DScut+="& CHILDCUT ( ADMASS('D0') < %(MaxDSD0DeltaM)s*MeV , 2 )"%locals()
    DScut+="& ( PT > %(MinDSPt)s )"%locals()
    DS.Code = DScut
    DSSel  = Selection("DSSelFor" + moduleName,  Algorithm = DS, RequiredSelections = [ StdDS  ] )

    
    # Define a class that contains all relevant particle properties
    class particle:
        def __init__(self,name,tla,q,b,sel,cut):
            self.name=name
            self.tla=tla
            self.Q=q
            self.B=b
            self.sel=sel
            self.cut=cut
            self.cp=self
        def CP(self,name,tla):
            p= particle(name,tla,-self.Q,-self.B,self.sel,self.cut)
            p.cp=self
            self.cp=p
            return p


    # Define the particles
    particles=[]
    particles.append(particle(        "pi+",       "Pip",  +1,  0, PiSel, SingleTrackcut))
    particles.append(particles[-1].CP("pi-",       "Pim"))
    particles.append(particle(        "K+",        "Kpl",  +1,  0, KSel,  SingleTrackcut))
    particles.append(particles[-1].CP("K-",        "Kmn"))
    particles.append(particle(        "p+",        "Prt",  +1, +1, pSel,  SingleTrackcut))
    particles.append(particles[-1].CP("p~-",       "Pbr"))
    particles.append(particle(        "KS0",       "Ksh",   0,  0, KsSel, Kscut))
    particles.append(particle(        "Lambda0",   "Lmz",   0, +1, LmSel, Lmcut))
    particles.append(particles[-1].CP("Lambda~0",  "Lmb"))
    particles.append(particle(        "D0",        "Dzr",   0,  0, DzSel, Dzcut))
    particles.append(particles[-1].CP("D~0",       "Dzb"))
    particles.append(particle(        "D+",        "Dpl",  +1,  0, DpSel, Dpcut))
    particles.append(particles[-1].CP("D-",        "Dmn"))
    particles.append(particle(        "D+",        "Dsp",  +1,  0, DsSel, Dscut))
    particles.append(particles[-1].CP("D-",        "Dsm"))
    particles.append(particle(        "Lambda_c+", "Lcp",  +1, +1, LcSel, Lccut))
    particles.append(particles[-1].CP("Lambda_c~-","Lcm"))
    particles.append(particle(        "D*(2010)+", "DSp",  +1,  0, DSSel, DScut))
    particles.append(particles[-1].CP("D*(2010)-", "DSm"))
    particles.append(particle(        "J/psi(1S)", "Jps",   0,  0, JpSel, Jpcut))
    particles.append(particle(        "phi(1020)", "Phi",   0,  0, PhSel, Phcut))
    particles.append(particle(        "K*(892)0",  "KS0",   0,  0, KSSel, KScut))
    particles.append(particles[-1].CP("K*(892)~0", "KSb"))

    #go through all twobody combinations
    descriptors = []
    for i1,p1 in enumerate(particles):
        for i2,p2 in enumerate(particles[i1:]):
            #check for charge and baryon number
            Q=p1.Q+p2.Q
            B=p1.B+p2.B
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
            if p1.cp.name==p2.name:CP=True
            if p1.cp.name==p1.name and p2.cp.name==p2.name:CP=True

            #for B0 non-CP combinations come twice. 
            if mother=="B0" and not CP and p1.tla+p2.tla<p1.cp.tla+p2.cp.tla:continue

            #build the decay descriptor
            descriptor=mother+" -> "+p1.name+" "+p2.name
            if not CP:descriptor="["+descriptor+"]cc"
            #print "DESCRIPTOR:",descriptor
            descriptors.append(descriptor)

    #make a merge of the input selections
    AllSel=[PiSel,KSel,pSel,KsSel,LmSel,DzSel,DpSel,DsSel,LcSel,DSSel,JpSel,PhSel,KSSel]
    InputSel= MergedSelection("InputForB2twobody", RequiredSelections = AllSel )

    #make a selection
    B=CombineParticles("BFor"+moduleName)
    B.DecayDescriptors = descriptors
    B.CombinationCut = Bcombcut
    B.MotherCut = Bcut
    BSel=Selection("BSelFor"+moduleName, Algorithm = B, RequiredSelections = [InputSel] )

    line = StrippingLine("B2twobodyLine",  algos = [BSel] )

    return [line]
