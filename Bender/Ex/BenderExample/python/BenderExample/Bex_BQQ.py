#! /usr/bin/env python
from Bender.MainMC import *                
from math import sqrt as s_q_r_t
selectVertexMin = LoKi.SelectVertex.selectMin

###Defining some variables
MBd = 5279.00

class B2QQ(AlgoMC):            
    def analyse(self):
        self.COUNTER["EVT"] += 1
        TES = appMgr().evtsvc()
       
        pvs = self.vselect("pvs", ISPRIMARY)
        if pvs.empty():
            return SUCCESS
        
        ips2cuter = MIPCHI2(pvs,self.geo())
        extracuts = (ips2cuter > self.mIps_Cut**2) & (PT>600)

        selections =  TES["Phys/Selections"].containedObjects()  
        for selection in selections:
            if self.ReqOF_Sel and selection.location() == '/Event/Phys/PreselB2HH' and not selection.found(): return SUCCESS
            
        muplus  = self.select ( 'mu+' , (self.mupcuts)&(extracuts))
        muminus = self.select ( 'mu-' , (self.mumcuts)&(extracuts))
        ips2cuter = MIPCHI2(pvs,self.geo())

        bsloop = self.loop("mu+ mu-", "B_s0")
        for b in bsloop:
            if not b.vertex(): continue
            mu1, mu2 = b(1), b(2)
         
            if mu1.proto().track().type() != 3: continue
            if mu2.proto().track().type() != 3: continue
            
            self.COUNTER["CandidatesAnalyzed"] += 1
            PVips2 = VIPCHI2( b.particle(), self.geo() )
            PV = selectVertexMin( pvs, PVips2, (PVips2 >= 0. ) )
            Dis2 = VDCHI2( PV )
            
            DZ = VZ( b ) - VZ( PV )
            
            Bmass = M(b)
            
            if abs(Bmass - MBd ) > self.MASSWINDOW:
                self.COUNTER["k_MassWindow"] += 1
                continue
                
            Chi2 = VCHI2(b)
            if Chi2 > self.Chi2_Cut:
                self.COUNTER["k_Chi2"] += 1
                continue
                
            Bips = s_q_r_t(PVips2(PV))
            if Bips > self.Bips_Cut:
                self.COUNTER["k_BIps"] += 1
                continue
                
            sigDOFS = Dis2(b)
            if sigDOFS < 0 :
                self.COUNTER["k_DOFS"] += 1
                continue
                
            sigDOFS = s_q_r_t(sigDOFS)*DZ/abs(DZ)
            if sigDOFS < self.signedDOFS_Cut :
                self.COUNTER["k_DOFS"] += 1
                continue
                
            Bpt = PT(b)
            if Bpt < self.Bpt_Cut:
                self.COUNTER["k_Bpt"] += 1
                continue
            mu1ips, mu2ips = s_q_r_t(ips2cuter(mu1)),s_q_r_t(ips2cuter(mu2))
            lessIPSmu = min(mu1ips,mu2ips)
            if lessIPSmu< self.mIps_Cut:
                self.COUNTER["k_muIps"] += 1
                continue
                
            if self.TUP:
                o1, o2 = mu1.proto().track().position(), mu2.proto().track().position()
                tup = self.nTuple( self.name() )
                tup.column("evt", self.COUNTER["EVT"])
                tup.column("lessIPSmu",lessIPSmu)
                tup.column("mu1p1", PX(mu1))
                tup.column("mu1p2", PY(mu1))
                tup.column("mu1p3", PZ(mu1))
                tup.column("mu2p1", PX(mu2))
                tup.column("mu2p2", PY(mu2))
                tup.column("mu2p3", PZ(mu2))
                tup.column("mu1o1", o1.x()) 
                tup.column("mu1o2", o1.y()) 
                tup.column("mu1o3", o1.z()) 
                tup.column("mu2o1", o2.x()) 
                tup.column("mu2o2", o2.y()) 
                tup.column("mu2o3", o2.z()) 
                tup.column("PIDmu1", PIDmu(mu1))
                tup.column("PIDmu2", PIDmu(mu2))
                tup.column("PIDk1", PIDK(mu1))
                tup.column("PIDk2", PIDK(mu2))
                tup.column("PIDe1", PIDe(mu1))
                tup.column("PIDe2", PIDe(mu2))
                tup.column("PIDp1", PIDp(mu1))
                tup.column("PIDp2", PIDp(mu2))
                tup.column("SV1", VX(b))
                tup.column("SV2", VY(b))
                tup.column("SV3", VZ(b))
                
                tup.column("PV1", VX(PV))
                tup.column("PV2", VY(PV))
                tup.column("PV3", VZ(PV))
               
              
                tup.column("ID1",ID(mu1))
                tup.column("ID2",ID(mu2))
                
                tup.column("Bdissig", sigDOFS)

                tup.column("Bpt",Bpt)          
                tup.column("Bips" , Bips)
                tup.column("Vchi2", Chi2)
                tup.column("PVchi2", PV.chi2())
                tup.column("Bmass", Bmass)
                tup.column("evtNum", TES["Rec/Header"].evtNumber())
                tup.column("runNum", TES["Rec/Header"].runNumber())
                
                tup.write()
            b.save("Bs")

        if self.selected("Bs").empty():            
            return SUCCESS
        
        self.COUNTER["Bender(evts) " + self.decayname] += 1
        
        self.setFilterPassed( True )
        print "Preselected "+self.decayname
        return SUCCESS

    def finalize(self):
        print self.decayname + " counter. \n -----------"
        print self.COUNTER
        return SUCCESS
