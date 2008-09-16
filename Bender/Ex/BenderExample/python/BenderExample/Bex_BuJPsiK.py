#! /usr/bin/env python
from Bender.MainMC import *                
from math import sqrt as s_q_r_t
selectVertexMin = LoKi.SelectVertex.selectMin

###Defining some variables
MBu = 5279.000
KMASS = 493.677
MJPsi = 3096.920

class BuJPsiK(AlgoMC):            
    def analyse(self):
        self.COUNTER["EVT"] += 1
        print self.COUNTER["EVT"]
        TES = appMgr().evtsvc()
        selections =  TES["Phys/Selections"].containedObjects() 

        pvs_ = self.vselect("pvs_", ISPRIMARY)
        if not pvs_.size(): return SUCCESS
      
        jpsis = self.select("jpsi", ((ABSID == 443) & (M > MJPsi-60) & (M < MJPsi + 60) ))
        ip2cuter = MIPCHI2(pvs_,self.geo())
        kaons = self.select("k", (ABSID == 321)&(ip2cuter > self.mIps_Cut**2))
        
        for b in self.loop("jpsi k", "B+"):
            jpsi, kaon = b(1), b(2) # Bu Daughters
        
            if kaon.proto().track().type() != 3: continue
            mu1, mu2 = jpsi.daughters().at(0), jpsi.daughters().at(1) # muons
            key1 = mu1.proto().track().key()
            key2 = mu2.proto().track().key()
            keyk = kaon.proto().track().key()
            if keyk == key1: continue
            if keyk == key2: continue
            if abs(M(b) - MBu ) > self.MASSWINDOW: continue
            if abs(M(jpsi) - MJPsi) > 60:
                continue
           
            PVips2 = VIPCHI2( b.particle(), self.geo())
            JPsiIps2 = VIPCHI2(jpsi, self.geo())
            PV = selectVertexMin(pvs_, PVips2, (PVips2 >= 0. ))
            Dis2 = VDCHI2( PV )
       
            Bpt = PT(b)
            if Bpt < self.Bpt_Cut: continue
            Bips = s_q_r_t(PVips2(PV))
            JPips = s_q_r_t(JPsiIps2(PV))
           
            sigDOFS = Dis2(b)
            if sigDOFS<0: continue
            DZ = VZ(b) - VZ(PV)
            sigDOFS = s_q_r_t(sigDOFS)*DZ/abs(DZ)
            dDsig = Dis2(jpsi)
            if dDsig<0: continue
            
            dDsig = s_q_r_t(dDsig)*DZ/abs(DZ)
            
            if max(dDsig,sigDOFS) < self.signedDOFS_Cut: continue
            if Bips > self.Bips_Cut: continue
            tup = self.nTuple( self.name() )
            track1 = mu1.proto().track()
            track2 = mu2.proto().track()
            o1, o2 = track1.position(), track2.position()
          

            tup.column("evt", self.COUNTER["EVT"]) 
            tup.column("JPsiChi2", jpsi.endVertex().chi2())  
            
            tup.column("Vchi2", VCHI2(b))  
            tup.column("k1ips",s_q_r_t(ip2cuter(kaon)))
            tup.column("mu1ips",s_q_r_t(ip2cuter(mu1)))
            tup.column("mu2ips",s_q_r_t(ip2cuter(mu2)))
            
            tup.column("JPsiMass", M(jpsi))
            tup.column("Bmass", M(b))      
            tup.column("Bpt",Bpt)
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

          
            tup.column("k1p1", PX(kaon))       
            tup.column("k1p2", PY(kaon))       
            tup.column("k1p3", PZ(kaon))       
            
            tup.column("QK", Q(kaon))          
            
            tup.column("PIDmu1", PIDmu(mu1))   
            tup.column("PIDmu2", PIDmu(mu2))   
            tup.column("PIDk", PIDK(kaon))     
            tup.column("PIDkp",PIDK(kaon) - PIDp(kaon))
            tup.column("SV1", VX(b))           
            tup.column("SV2", VY(b))           
            tup.column("SV3", VZ(b))           
            
            tup.column("evtNum", TES["Rec/Header"].evtNumber())
            tup.column("runNum", TES["Rec/Header"].runNumber())
           
            tup.column("PV1", VX(PV)) 
            tup.column("PV2", VY(PV)) 
            
            tup.column("Bdissig", sigDOFS)
            tup.column("dDsig",dDsig)
                        
            tup.write()
            b.save("B+")

        if self.selected("B+").size():
            print "Bender has found a B+ --> JPsi K+ candidate"
            self.COUNTER["Sel"] += 1
        return SUCCESS

    def finalize(self):
        print "B+ --> J/Psi K+ counter. \n -----------"
        print self.COUNTER
        return SUCCESS
