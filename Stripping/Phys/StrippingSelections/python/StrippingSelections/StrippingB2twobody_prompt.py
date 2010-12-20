#
# B->twobody_prompt selection for stripping. Uses selection framework.
# look for unexpected resonances decaying to charmed hadrons 
#
# Author: R. Oldeman , W. Bonivento
#

def B2twobody_promptLine(

        moduleName = "B2twobody_prompt", 
        Prescale_prompt=1.,

        MaxLongTracksInEvent=80,
        MinBPt = 2000.0,       
	MaxBVertChi2DOF = 10., 
#        MaxBPVVDCHI2=9.,
#        MaxIPCHI2DV=9.,
        MinBPVDIRA = 0.999, # removed
	MinBMass = 4000.0,     
	MaxBMass = 100000.0,     
        MinNvc=3,
        MinFly=1,

	MaxSingleTrackChi2 = 5.0,
	MinSingleTrackPt = 5000.0,        
	MaxSingleTrackIPChi2DV = 5.0,   # changed 

        MinLongLivedDauPt = 200.0,
        MinLongLivedDauIPChi2 = 4.0,      
        MaxLongLivedDauTrkChi2 = 10.0,
        MinLongLivedPt = 3000.0,
        MinKsPt=2500.,
        MaxLongLivedIPChi2 = 10.0,
        MaxLongLivedVertChi2DOF = 10.0,

 	MinLongLivedPVVDChi2 = 200.0,   # halved
 	MaxKsDeltaM = 25.0, 
 	MaxLmDeltaM = 15.0, 

        MinDPIDK = 5.0,
        MinDPIDp = 5.0,
        MinDPIDpSingle = 10.0,
        MinDPIDpK = 5.0,
        MinDPIDKforD0Dplus = 0.0,
        MinDDauPt = 250.0,
        MinDDauIPChi2 = 7.0,      
        MaxDDauTrkChi2 = 5.0,
        MinDPt = 1200.0,       
        MaxDIPChi2 = 10.0,   # changed
        MaxDVertChi2DOF = 10.0,
 	MinDPVVDChi2 = 50.0,  # halved/2 
 	MaxDDeltaM = 30.0, 
#        MinDPoint= 0.999,

#        MinKSDauPt = 250.0,      
#        MinKSDauIPChi2 = 4.0,   # changed       
#        MaxKSDauTrkChi2 = 5.0,
#        MinKSPt = 1500.0,   # changed      
#        MaxKSIPChi2 = 10.0,# changed
#        MaxKSVertChi2DOF = 10.0,
#        MinKSPVVDChi2 = 50.0,# halved/2 
# 	MaxKSDeltaM = 40.0, # changed

        MinPhDauPt = 1000.0,      
        MaxPhDauIPChi2 = 5.0,  # changed      
        MaxPhDauTrkChi2 = 5.0,
        MinPhPt = 2500.0,      
#        MinPhIPChi2 = 16.0,  removed
        MaxPhVertChi2DOF = 10.0,
#        MinPhPVVDChi2 = 100.0,  removed
 	MaxPhDeltaM = 20.0, 

        MinJpDauPt = 500.0,      
        MaxJpDauIPChi2 = 10.0,   # changed
        MaxJpDauTrkChi2 = 5.0,  # changed
        MinJpPt = 1000.0,      
        MinJpIPChi2 = 0.0,   # removed
        MaxJpVertChi2DOF = 10.0,
        MinJpPVVDChi2 = 0.0,   # removed
 	MaxJpDeltaM = 50.0, 
        MaxYDeltaM = 1000.0,

        MaxDSD0VertChi2DOF = 10.0,
        MinDSD0PVVDChi2 = 60.0,   # halved/2
        MaxDSD0DeltaM = 30.0,
        MinDSPt = 1000.0,      
        MaxDSDeltaM=3.0,
        MaxDSSlowPionIPChi2=7.,  # added   
        MaxDSD0IPChi2=7.,  # added
        MinDSD0DauPt = 250.0, # added
        MinDSD0DauIPChi2 = 9.0,
        MinDSSlowPionDauPt = 150.0, # added

        MinPi0RPt=3500.0,  
        MinPi0RMM=100.0,   
        MaxPi0RMM=170.0,
        MinPi0RGamCL=0.2,
        MinPi0MPt=3500.0,  # was 3500

        MinPhoPt=3500.0, 



   ) : 

    from Configurables import CombineParticles, FilterDesktop,LoKi__VoidFilter
#    from Configurables import LoKi__ODINFilter as ODINFilter
#    from Configurables import LoKi__VoidFilter as VoidFilter
#    from Configurables import LoKi__HDRFilter as HltFilter

    from StrippingConf.StrippingLine import StrippingLine
    from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand, MergedSelection, EventSelection
    from Configurables import LoKi__VoidFilter as VoidFilter
    from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
    modules = CoreFactory('CoreFactory').Modules
    for i in ['LoKiTrigger.decorators']:
        if i not in modules : modules.append(i)

    # Define the shared cuts
    B_prompt_combcut =    "(in_range(%(MinBMass)s*MeV, AM, %(MaxBMass)s*MeV))" % locals()
    B_prompt_combcut += "& (APT>%(MinBPt)s*MeV)"%locals()
    B_prompt_cut = "(VFASPF(VCHI2/VDOF)<%(MaxBVertChi2DOF)s) "%locals()
#    B_prompt_cut += "& (BPVIPCHI2() < %(MaxBPVIPChi2)s)"%locals()
#    Bcut += "& (BPVVDCHI2 < %(MinBPVVDChi2)s)"% locals()  # changed
#    Bcut += "& (BPVDIRA > %(MinBPVDIRA)s)"% locals()

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
    StdPh      = DataOnDemand(Location = "Phys/StdLoosePhi2KK")        # qui c'e un errrore!!!!!!!!!!
#    StdKS      = DataOnDemand(Location = "Phys/StdLooseDetachedKst2Kpi")
    StdPi0M    = DataOnDemand(Location = "Phys/StdLooseMergedPi0")
    StdPi0R    = DataOnDemand(Location = "Phys/StdLooseResolvedPi0")
#    StdEtaR    = DataOnDemand(Location = "Phys/StdLooseResolvedEta")
    StdPho     = DataOnDemand(Location = "Phys/StdLooseAllPhotons")
 

    # SingleTrack kaon/pion/proton
    SingleTrackPi = FilterDesktop("SingleTrackFor" + moduleName)
#    SingleTrackp = FilterDesktop("SingleTrackFor" + moduleName)
    SingleTrackcut =   "(TRCHI2DOF<%(MaxSingleTrackChi2)s)"%locals()
    SingleTrackcut +="& (PT > %(MinSingleTrackPt)s*MeV)"%locals()
    SingleTrackcut +="& (MIPCHI2DV(PRIMARY) < %(MaxSingleTrackIPChi2DV)s)"%locals()
#    SingleTrackcutK = SingleTrackcut +"& (PIDK-PIDpi > %(MinDPIDK)s )"%locals()
#    SingleTrackcutp = SingleTrackcut +"& (PIDp-PIDpi > %(MinDPIDpSingle)s ) & (PIDp-PIDK > %(MinDPIDpK)s ) "%locals()
    SingleTrackPi.Code = SingleTrackcut
#    SingleTrackp.Code = SingleTrackcutp
    PiSel = Selection("PiSelFor" + moduleName, Algorithm = SingleTrackPi, RequiredSelections = [ StdPi ] )
#    KSel  = Selection("KSelFor"  + moduleName, Algorithm = SingleTrackK, RequiredSelections = [ StdK  ] )
#    pSel  = Selection("pSelFor"  + moduleName, Algorithm = SingleTrackp, RequiredSelections = [ Stdp  ] )

    # Long-lived strange hadrons
    Ks=FilterDesktop("KsFor" + moduleName)
    Lm=FilterDesktop("LmFor" + moduleName)
    LongLivedcut  = "(PT>%(MinLongLivedPt)s*MeV)"%locals()
    LongLivedcut+="& (MIPCHI2DV(PRIMARY)<%(MaxLongLivedIPChi2)s)"%locals()             # changed
    LongLivedcut+="& (VFASPF(VCHI2/VDOF)<%(MaxLongLivedVertChi2DOF)s)"%locals()
    LongLivedcut+="& (BPVVDCHI2>%(MinLongLivedPVVDChi2)s)"%locals()
    LongLivedcut+="& CHILDCUT ( PT > %(MinLongLivedDauPt)s , 1 )"%locals()
    LongLivedcut+="& CHILDCUT ( PT > %(MinLongLivedDauPt)s , 2 )"%locals()
    LongLivedcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLongLivedDauIPChi2)s , 1 )"%locals()
    LongLivedcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinLongLivedDauIPChi2)s , 2 )"%locals()
    LongLivedcut+="& CHILDCUT ( TRCHI2DOF < %(MaxLongLivedDauTrkChi2)s , 1 )"%locals()
    LongLivedcut+="& CHILDCUT ( TRCHI2DOF < %(MaxLongLivedDauTrkChi2)s , 2 )"%locals()
    LongLivedcut+="& (BPVDIRA> %(MinBPVDIRA)s)"%locals()
    Kscut = LongLivedcut+"& (PT>%(MinKsPt)s*MeV) & (ADMASS('KS0') < %(MaxKsDeltaM)s*MeV)"%locals()
    Lmcut = LongLivedcut+"& (ADMASS('Lambda0') < %(MaxLmDeltaM)s*MeV)"%locals()
    Ks.Code = Kscut
    Lm.Code = Lmcut
    KsMerged = MergedSelection( "MergedKS0For" + moduleName, RequiredSelections = [StdKsDD,StdKsLL] )
    LmMerged = MergedSelection( "MergedLmFor" + moduleName, RequiredSelections = [StdLmDD,StdLmLL] ) 
    KsSel=Selection("KsSelFor" + moduleName, Algorithm = Ks, RequiredSelections = [ KsMerged ] )
    LmSel=Selection("LmSelFor" + moduleName, Algorithm = Lm, RequiredSelections = [ LmMerged ] )

    # Charm hadrons
    Dz = FilterDesktop("DzFor" + moduleName)
    Dp = FilterDesktop("DpFor" + moduleName)
    Ds = FilterDesktop("DsFor" + moduleName)
    Lc = FilterDesktop("LcFor" + moduleName)
    Charm2Bcut  = "(PT>%(MinDPt)s*MeV)"%locals()
    Charm2Bcut+="& (MIPCHI2DV(PRIMARY)<%(MaxDIPChi2)s)"%locals()  # changed
    Charm2Bcut+="& (VFASPF(VCHI2/VDOF)<%(MaxDVertChi2DOF)s)"%locals()
    Charm2Bcut+="& (BPVVDCHI2>%(MinDPVVDChi2)s)"%locals()
#    Charm2Bcut+="& CHILDCUT ( BPVDIRA> %(
    Charm2Bcut+="& CHILDCUT ( PT > %(MinDDauPt)s , 1 )"%locals()
    Charm2Bcut+="& CHILDCUT ( PT > %(MinDDauPt)s , 2 )"%locals()
    Charm2Bcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 1 )"%locals()
    Charm2Bcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 2 )"%locals()
    Charm2Bcut+="& CHILDCUT ( TRCHI2DOF < %(MaxDDauTrkChi2)s , 1 )"%locals()
    Charm2Bcut+="& CHILDCUT ( TRCHI2DOF < %(MaxDDauTrkChi2)s , 2 )"%locals()
    Charm2Bcut+="& (BPVDIRA> %(MinBPVDIRA)s)"%locals()
    Charm3Bcut=Charm2Bcut
    Charm3Bcut+="& CHILDCUT ( PT > %(MinDDauPt)s , 3 )"%locals()
    Charm3Bcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinDDauIPChi2)s , 3 )"%locals()
    Charm3Bcut+="& CHILDCUT ( TRCHI2DOF < %(MaxDDauTrkChi2)s , 3 )"%locals()
    Dzcut = Charm2Bcut+"& (ADMASS('D0') < %(MaxDDeltaM)s*MeV)"%locals()
    Dpcut = Charm3Bcut+"& (ADMASS('D+') < %(MaxDDeltaM)s*MeV)"%locals()
    Dscut = Charm3Bcut+"& (ADMASS('D_s+') < %(MaxDDeltaM)s*MeV)"%locals()
    Lccut = Charm3Bcut+"& (ADMASS('Lambda_c+') < %(MaxDDeltaM)s*MeV)"%locals()
    Dzcut+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDKforD0Dplus)s , 1 )"%locals()
    Dpcut+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 1 )"%locals()
    Dscut+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 1 )"%locals()
    Dscut+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 2 )"%locals()
    Lccut+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 1 )"%locals()
    Lccut+="& CHILDCUT ( PIDp-PIDpi > %(MinDPIDp)s , 2 )"%locals()
    Lccut+="& CHILDCUT ( PIDp-PIDK > %(MinDPIDpK)s , 2 )"%locals()
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
    Phcut =  "CHILDCUT ( MIPCHI2DV ( PRIMARY ) < %(MaxPhDauIPChi2)s , 1 )"%locals()
    Phcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) < %(MaxPhDauIPChi2)s , 2 )"%locals()
    Phcut+="& CHILDCUT ( PT > %(MinPhDauPt)s , 1 )"%locals()
    Phcut+="& CHILDCUT ( PT > %(MinPhDauPt)s , 2 )"%locals()
    Phcut+="& CHILDCUT ( TRCHI2DOF < %(MaxPhDauTrkChi2)s , 1 )"%locals()
    Phcut+="& CHILDCUT ( TRCHI2DOF < %(MaxPhDauTrkChi2)s , 2 )"%locals()
    Phcut+="& ( PT > %(MinPhPt)s )"%locals()
#    Phcut+="& ( MIPCHI2DV(PRIMARY) > %(MinPhIPChi2)s )"%locals()
    Phcut+="& ( VFASPF(VCHI2/VDOF) < %(MaxPhVertChi2DOF)s )"%locals()
#    Phcut+="& ( BPVVDCHI2 > %(MinPhPVVDChi2)s )"%locals()
    Phcut+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 1 )"%locals()
    Phcut+="& CHILDCUT ( PIDK-PIDpi > %(MinDPIDK)s , 2 )"%locals()
    Phcut+="& ( ADMASS('phi(1020)') < %(MaxPhDeltaM)s*MeV)"%locals()
    Ph.Code = Phcut
    PhSel  = Selection("PhSelFor" + moduleName,  Algorithm = Ph, RequiredSelections = [ StdPh  ] )

#    KS = FilterDesktop("KSFor" + moduleName)
#    KScut =  "CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKSDauIPChi2)s , 1 )"%locals()
#    KScut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) > %(MinKSDauIPChi2)s , 2 )"%locals()
#    KScut+="& CHILDCUT ( PT > %(MinKSDauPt)s , 1 )"%locals()
#    KScut+="& CHILDCUT ( PT > %(MinKSDauPt)s , 2 )"%locals()
#    KScut+="& CHILDCUT ( TRCHI2DOF < %(MaxKSDauTrkChi2)s , 1 )"%locals()
#    KScut+="& CHILDCUT ( TRCHI2DOF < %(MaxKSDauTrkChi2)s , 2 )"%locals()
#    KScut+="& ( PT > %(MinKSPt)s )"%locals()
#    KScut+="& ( MIPCHI2DV(PRIMARY) < %(MaxKSIPChi2)s )"%locals()
#    KScut+="& ( VFASPF(VCHI2/VDOF) < %(MaxKSVertChi2DOF)s )"%locals()
#    KScut+="& ( BPVVDCHI2 > %(MinKSPVVDChi2)s )"%locals()
#    KScut+="& (ADMASS('K*(892)0') < %(MaxKSDeltaM)s*MeV)"%locals()
#    KS.Code = KScut
#    KSSel  = Selection("KSSelFor" + moduleName,  Algorithm = KS, RequiredSelections = [ StdKS  ] )


    # Heavy resonances 
    Jp = FilterDesktop("JpFor" + moduleName) 
    Yp = FilterDesktop("YFor" + moduleName)
    Jpcut =  "CHILDCUT ( MIPCHI2DV ( PRIMARY ) < %(MaxJpDauIPChi2)s , 1 )"%locals()  # changed
    Jpcut+="& CHILDCUT ( MIPCHI2DV ( PRIMARY ) < %(MaxJpDauIPChi2)s , 2 )"%locals()  # changed
    Jpcut+="& CHILDCUT ( PT > %(MinJpDauPt)s , 1 )"%locals()
    Jpcut+="& CHILDCUT ( PT > %(MinJpDauPt)s , 2 )"%locals()
    Jpcut+="& CHILDCUT ( TRCHI2DOF < %(MaxJpDauTrkChi2)s , 1 )"%locals()
    Jpcut+="& CHILDCUT ( TRCHI2DOF < %(MaxJpDauTrkChi2)s , 2 )"%locals()
    Jpcut+="& ( PT > %(MinJpPt)s )"%locals()
#    Jpcut+="& ( MIPCHI2DV(PRIMARY) > %(MinJpIPChi2)s )"%locals()       # removed
    Jpcut+="& ( VFASPF(VCHI2/VDOF) < %(MaxJpVertChi2DOF)s )"%locals()
#    Jpcut+="& ( BPVVDCHI2 > %(MinJpPVVDChi2)s )"%locals()  # removed
    Jcuts=Jpcut+"& ( ((M>3046*MeV)&(M<3146*MeV)) | ((M>3670*MeV)&(M<3720*MeV)) | ((M>9100*MeV)&(M<10500*MeV)) )"%locals()
    Jp.Code = Jcuts
    JpSel  = Selection("JpSelFor" + moduleName,  Algorithm = Jp, RequiredSelections = [ StdJp  ] )

    DS = FilterDesktop("DSFor" + moduleName)
    DScut =  "(abs(M-MAXTREE('D0'==ABSID,M)-145.42)<%(MaxDSDeltaM)s)"%locals()
    DScut+="& CHILDCUT ( VFASPF(VCHI2/VDOF) < %(MaxDSD0VertChi2DOF)s , 2 )"%locals()
    DScut+="& CHILDCUT ( BPVVDCHI2 > %(MinDSD0PVVDChi2)s , 2 )"%locals()
    DScut+="& CHILDCUT ( BPVDIRA> %(MinBPVDIRA)s, 2 )"%locals()
    DScut+="& CHILDCUT ( ADMASS('D0') < %(MaxDSD0DeltaM)s*MeV , 2 )"%locals()
    DScut+="& ( PT > %(MinDSPt)s )"%locals()
    DScut+="& CHILDCUT ( MIPCHI2DV(PRIMARY) < %(MaxDSSlowPionIPChi2)s , 1 )"%locals()     # added
    DScut+="& CHILDCUT ( MIPCHI2DV(PRIMARY) < %(MaxDSD0IPChi2)s , 2 )"%locals()     # added 
#    DScut+="& CHILDCUT ( MinDsD0DauPt
    DScut+="& ( NINGENERATION ( ( PT > %(MinDSD0DauPt)s ) , 2 ) == 2 )"%locals()
#    DScut+="& (1 == NINGENERATION ( (PIDK-PIDpi > %(MinDPIDKforD0Dplus)s ),2))"%locals()
    DScut+="& ( NINGENERATION ( ( MIPCHI2DV ( PRIMARY ) > %(MinDSD0DauIPChi2)s ) , 2 ) == 2 )"%locals()
    DScut+="& CHILDCUT ( PT > %(MinDSSlowPionDauPt)s ,1)"%locals()     # added 
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
    
#    EtaR=FilterDesktop("EtaRFor" + moduleName)
#    EtaRcut =  " (PT > %(MinEtaRPt)s )"%locals()
#    EtaR.Code=EtaRcut
#    EtaRSel = Selection("EtaRSelFor" + moduleName, Algorithm =EtaR, RequiredSelections = [StdEtaR] )

    # Define a class that contains all relevant particle properties
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
    particles.append(particle(        "pi+"        , 0, 1, +1,  0))
    particles.append(particles[-1].CP("pi-"        ))
#    particles.append(particle(        "p+"        , 0, 1, +1, +1))
#    particles.append(particles[-1].CP("p~-"       ))
    particles.append(particle(        "KS0"       , 1, 1,  0,  0))
    particles.append(particle(        "Lambda0"   , 1, 1,  0, +1))
    particles.append(particles[-1].CP("Lambda~0"  ))
    particles.append(particle(        "D0"        , 1, 2,  0,  0))
    particles.append(particles[-1].CP("D~0"       ))
    particles.append(particle(        "D+"        , 1, 2, +1,  0))
    particles.append(particles[-1].CP("D-"        ))
    particles.append(particle(        "Lambda_c+" , 1, 2, +1, +1))
    particles.append(particles[-1].CP("Lambda_c~-"))
    particles.append(particle(        "D*(2010)+" , 1, 2, +1,  0))
    particles.append(particles[-1].CP("D*(2010)-" ))
    particles.append(particle(        "J/psi(1S)" , 1, 2,  0,  0))
    particles.append(particle(        "phi(1020)" , 0, 2,  0,  0))
    particles.append(particle(        "pi0"     , 0 ,1,   0,  0))
    particles.append(particle(        "gamma"   , 0 ,1,   0,  0))
#    particles.append(particle(        "eta"   , 0 ,1,   0,  0)) 


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
#    AllSel=[PiSel,KSel,pSel,KsSel,LmSel,DzSel,DpSel,DsSel,LcSel,DSSel,JpSel,PhSel,KSSel]
    AllSel_prompt=[PiSel,LmSel,DzSel,DpSel,DsSel,LcSel,DSSel,JpSel,PhSel,KsSel,Pi0Sel,PhoSel]
    InputSel_prompt= MergedSelection("InputForB2twobody_prompt", RequiredSelections = AllSel_prompt )

    #make a selection
    B_prompt=CombineParticles("B_prompt_For"+moduleName)
    B_prompt.DecayDescriptors = descriptors
    B_prompt.CombinationCut = B_prompt_combcut
    B_prompt.MotherCut = B_prompt_cut
   
    Bpresel= EventSelection(
        LoKi__VoidFilter("BpreselFor"+moduleName,
                         Code=" (CONTAINS('%s') > 1.5) " %  ( InputSel_prompt.outputLocation() ) ),
        RequiredSelection=InputSel_prompt
    )

    BSel_prompt=Selection("B_prompt_SelFor"+moduleName, Algorithm = B_prompt, RequiredSelections = [ Bpresel ] )
    
    # Define the GEC on the number of tracks, needed in order to control
    # the time for the combinatorics (especially once we add the 4-body D states)
    B_prompt_StrippingNumTracksGEC = VoidFilter( 'B_prompt_StrippingNumTracksGEC' )
    B_prompt_StrippingNumTracksGEC.Code = "TrSOURCE('Rec/Track/Best') >> TrLONG >>  (TrSIZE < %(MaxLongTracksInEvent)s ) "%locals() 
                                   
    VertexFilterMin = LoKi__VoidFilter(  'VertexFilterMin', Code= "CONTAINS('Rec/Vertex/Primary')==1")
    #    VertexFilterMax = LoKi__VoidFilter(  'VertexFilterMax', Code= "CONTAINS('Rec/Vertex/Primary')<3")
    line = StrippingLine("B2twobody_promptLine",   prescale = "%(Prescale_prompt)s  "%locals(), algos = [VertexFilterMin,  B_prompt_StrippingNumTracksGEC, BSel_prompt] )

    return [line]
