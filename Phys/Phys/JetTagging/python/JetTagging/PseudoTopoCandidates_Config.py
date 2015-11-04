__version__ = "$Id: PseudoTopoCandidates_Config.py,v 1.1 2013-01-10 12:37:55 potterat Exp $"
__author__  = "Cedric Potterat <Cedric.Potterat@cern.ch>"

#######
#
# Thanks to Mike Williams, code copided from StrippingSelections.Beauty2Charm_Utils.py
#
#######

from StrippingSelections.Beauty2Charm_LoKiCuts import LoKiCuts
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import DataOnDemand, Selection, MergedSelection
from Configurables import OfflineVertexFitter
from Configurables import SubstitutePID
from StandardParticles import StdAllNoPIDsKaons
from PhysSelPython.Wrappers import SelectionSequence




class PseudoTopoCandidatesConf:
    def __init__(self,_name,_config = {}):
        self.name = _name
        self.config = {
            "ALL" : { # Cuts made on all charged input particles in all lines (excpt. upstream)
                'TRCHI2DOF_MAX' : 5,
                'PT_MIN'        : '100*MeV',
                'P_MIN'         : '1000*MeV',
                'MIPCHI2DV_MIN' : 4, 
                'TRGHP_MAX'     : 0.4
                },
            "V0s":{ 
                'PT_MIN'        : '500*MeV',
                'P_MIN'         : '1000*MeV',
                'MIPCHI2DV_MIN' : 4         
                }, 
            "Photons":{ 
                'PT_MIN'        : '2500*MeV',
                'P_MIN'         : '4000*MeV'
                },           
            'BB' : {'ADDSUMPT':0,'COSANGLE_MAX':0.99,'COSDPHI_MAX':0,'M_MIN':0,'MAXPT_MIN': 4000},
            'PIDE_MIN'          : -2.0
            }
        self.config.update(_config)
        self.algorithms = []
        self.kaons = self.filterInputs('mK',[StdAllNoPIDsKaons],self.config['ALL'])
        self.V0s = self.filterInputs('mV0s',[self.dataOnDemand('StdLooseKsLL'),self.dataOnDemand('StdLooseKsDD'),self.dataOnDemand('StdLooseLambdaLL'),self.dataOnDemand('StdLooseLambdaDD')],self.config['V0s'])
        self.Photons = self.filterInputs('mG',[self.dataOnDemand('StdLooseAllPhotons')],self.config['Photons'])
        self.topoKaons = self.topoInputs('mK',[self.kaons])
        self.topos = self.makeTopoCands( MergedSelection('allInput',[self.topoKaons,self.V0s]),self.config['BB']['ADDSUMPT'])
        self.seq = SelectionSequence('seq', TopSelection = self.topos)
        self.algorithms = [self.seq.sequence()]

    def dataOnDemand(self,loc): return DataOnDemand(Location="Phys/%s/Particles"%loc)

    def filterSelection(self,name,code,inputs,preambulo=None):
        alg = FilterDesktop(Code=code)
        if preambulo is not None : alg.Preambulo = preambulo
        return Selection(name+self.name,
                         Algorithm=alg,
                         RequiredSelections=inputs)

    def filterInputs(self,tag,inputs,config):
        '''Filter input particles.'''
        code = LoKiCuts(['TRCHI2DOF','PT','P','MIPCHI2DV','MM','CHILDCL1',
                         'CHILDCL2','PIDK','PIDp','BPVVDRHO','BPVVDZ','DOCAMAX','TRGHP'],
                        config).code()
        return self.filterSelection(tag+'Inputs',code,inputs)
   
    def topoInputsCuts(self): # Don't need IP chi2 cut b/c is in 1st filter
        return "(TRCHI2DOF<5) & (PT > 500*MeV) & (P > 5000*MeV)"
   
    def topoInputs(self,tag,inputs):
        '''Selects tracks that could have been used by the Topo.'''
        return self.filterSelection(tag+'TopoInputs',self.topoInputsCuts(),inputs)
   
    def makeTopoAllNBody(self,n,inputs,sumptcut=-1):
        decay = [["K*(892)0 -> K+ K+","K*(892)0 -> K+ K-","K*(892)0 -> K- K-",
                  "K*(892)0 -> K+ KS0","K*(892)0 -> K- KS0",
                  "K*(892)0 -> K+ Lambda0","K*(892)0 -> K- Lambda0",
                  "K*(892)0 -> K+ Lambda~0","K*(892)0 -> K- Lambda~0",
                  "K*(892)0 -> KS0 KS0"],
                 ["Delta0 -> K*(892)0 K+", "Delta0 -> K*(892)0 K-",
                  "Delta0 -> K*(892)0 KS0", "Delta0 -> K*(892)0 KS0",
                  "Delta0 -> K*(892)0 Lambda0",
                  "Delta0 -> K*(892)0 Lambda~0"],
                 ["B0 -> Delta0 K-","B0 -> Delta0 K+",
                  "B0 -> Delta0 KS0",
                  "B0 -> Delta0 Lambda0","B0 -> Delta0 Lambda~0"],
                 ["B0 -> K*(892)0  gamma"]
                 ]
        comboCuts = "(AM < 10000*MeV) & "
        if sumptcut > 0:
            comboCuts += '(ASUM(SUMTREE(PT,(ISBASIC),0.0)) > %d*MeV) & ' % sumptcut
        comboCuts += '(AALLSAMEBPV |(AMINCHILD(MIPCHI2DV(PRIMARY)) > 16))'
#        comboCuts += " & (AMAXDOCA('LoKi::DistanceCalculator') < 0.2*mm)"
        momCuts = "(BPVDIRA > 0) & (BPVVDCHI2 > 100) & (VFASPF(VCHI2/VDOF)<10)"
        cp = CombineParticles(DecayDescriptors=decay[n-2],
                              CombinationCut=comboCuts,
                              MotherCut=momCuts)
        return Selection("TopoAll%dBodySel"%n,Algorithm=cp,RequiredSelections=inputs)
       
    def topoNforN(self,n,inputs,addsumpt):
        pid = "('K+'==ABSID)"
        minPtSum = 2000
        minPtSum += addsumpt
        cuts = '(SUMTREE(PT,%s,0.0) > %d*MeV)' % (pid,minPtSum)
        cuts += "&(INTREE(ISBASIC & (MIPCHI2DV(PRIMARY)>16) "\
            " & (PT > 1500*MeV)))"
#        cuts += '& (MINTREE(HASTRACK & %s,TRCHI2DOF) < 2)' % pid
        cuts += "& (NINTREE(('KS0'==ID)|('Lambda0'==ABSID)) <= 2)" 
        return self.filterSelection('Topo%dfor%dSel'%(n,n),cuts,[inputs]) 
   
    def topo2for3(self,all2):
        return self.filterSelection('Topo2for3Sel',
                               '(M < 10000*MeV)',[all2])
   
    def topo3for4(self,all3):
        return self.filterSelection('Topo3for4Sel','(M < 10000*MeV)',[all3]) 
   
    def topoBDT(self,n,inputs, V0s = True, rad = False):
        PIDs =['K+','KS0','Lambda0','Lambda~0']
        if rad:
            sel = self.filterSelection('Topo%dRadBodyBBDTSel'%n,
                                       "FILTER('BBDecTreeTool/TopoBBDT')",[inputs])
            PIDs.append('gamma')
        else:
            sel = self.filterSelection('Topo%dBodyBBDTSel'%n,
                                       "FILTER('BBDecTreeTool/TopoBBDT')",[inputs])             
        from Configurables import BBDecTreeTool as BBDT
        bbdt = BBDT(sel.name()+'.TopoBBDT')
        bbdt.Threshold = -1
        bbdt.ANNSvcKey = 9999
        bbdt.PIDs = PIDs
        bbdt.ParamFile = 'Hlt2Topo%dBody_BDTParams_v1r0.txt' % n
        return sel
   

    def topoSubPID(self,name,inputs):
        sub = SubstitutePID(name+'SubPID',
                            Code="DECTREE('X0 -> X X')")
        sub.MaxChi2PerDoF = -666
        sub.Substitutions = {'X0 -> X X'  : 'B0'}
        sel = Selection(name+'FinalSel',Algorithm=sub,
                        RequiredSelections=[inputs])
        filter = "INTREE(ID=='B0')" 
        return self.filterSelection(name,filter,[sel])
   
    def makeTopoCands(self,inputs,addsumpt):
        all2 = self.makeTopoAllNBody(2,[inputs])
        topo2 = self.topoNforN(2,all2,addsumpt)
        topo2_bdt = self.topoBDT(2,topo2)
        topo2_bdt = self.topoSubPID('Topo2Body',topo2_bdt)
        filt2 = self.topo2for3(all2)
        all3 = self.makeTopoAllNBody(3,[inputs,filt2])
        topo3 = self.topoNforN(3,all3,addsumpt)
        topo3_bdt = self.topoBDT(3,topo3)
        topo3_bdt = self.topoSubPID('Topo3Body',topo3_bdt)
        filt3 = self.topo3for4(all3)
        all4 = self.makeTopoAllNBody(4,[inputs,filt3],addsumpt)
        topo4 = self.topoNforN(4,all4,addsumpt)
        topo4_bdt = self.topoBDT(4,topo4)
        topo4_bdt = self.topoSubPID('Topo4Body',topo4_bdt)
        #allrad = self.makeTopoAllNBody(5,[ self.Photons ,filt2])
        #topoRad = self.topoNforN(5,allrad,addsumpt)
        #topoRad_bdt = self.topoBDT(3,topoRad, rad=True)
        #topoRad_bdt = self.topoSubPID('TopoRadBody',topoRad_bdt)
        return MergedSelection(self.name,[topo2_bdt,topo3_bdt,topo4_bdt])#[topo2,topo3,topo4]

