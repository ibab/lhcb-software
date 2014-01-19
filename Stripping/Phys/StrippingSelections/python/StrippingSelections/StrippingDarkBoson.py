#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
'''
Dark Bosons (ALPs, inflatons, etc)

Author: M. Williams
'''
__author__ = ['Mike Williams']

moduleName = 'DarkBoson'

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

import re
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from PhysSelPython.Wrappers import EventSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Configurables import LoKi__VoidFilter
from StandardParticles import StdAllNoPIDsPions, StdAllNoPIDsKaons, \
     StdAllNoPIDsProtons, StdNoPIDsUpPions, StdAllLooseMuons, \
     StdLooseDownMuons
from Beauty2Charm_LoKiCuts import LoKiCuts
from StandardParticles import StdAllLooseGammaLL, StdAllLooseGammaDD, \
    StdAllLooseElectrons, StdLooseAllPhotons#, StdDiElectronFromTracks
from Configurables import SubstitutePID
from copy import deepcopy

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

# Default configuration dictionary
config = {
    "KB" : { # K directly from B
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '250*MeV', 
    'P_MIN'         : '2000*MeV',
    'MIPCHI2DV_MIN' : 9, 
    'TRGHP_MAX'     : 0.3,
    'PROBNNK_MIN'   : 0.1, 
    },
    "KBhard" : { # K directly from B only for B -> KS0(pipi) K
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '250*MeV',
    'P_MIN'         : '2000*MeV',
    'MIPCHI2DV_MIN' : 9,
    'TRGHP_MAX'     : 0.3,
    'PROBNNK_MIN'   : 0.2,
    },
    "PiB" : { # pi directly from B
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '250*MeV', 
    'P_MIN'         : '2000*MeV',
    'MIPCHI2DV_MIN' : 9, 
    'TRGHP_MAX'     : 0.3,
    'PROBNNpi_MIN'  : 0.2 
    },
    "KX" : { # K from X
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '250*MeV', 
    'P_MIN'         : '0*MeV',
    'MIPCHI2DV_MIN' : 25, 
    'TRGHP_MAX'     : 0.3,
    'PROBNNK_MIN'   : 0.1 
    },
    "PiX" : { # pi from X
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '250*MeV', 
    'P_MIN'         : '0*MeV',
    'MIPCHI2DV_MIN' : 25, 
    'TRGHP_MAX'     : 0.3,
    'PROBNNpi_MIN'  : 0.65 
    },
    "KDX" : { # K DOWN from X
    'TRCHI2DOF_MAX' : 4,
    'PT_MIN'        : '125*MeV', 
    'P_MIN'         : '0*MeV',
    'MIPCHI2DV_MIN' : 25, 
    'TRGHP_MAX'     : 0.3,
    'PROBNNK_MIN'   : 0.1 
    },
    "PiDX" : { # pi DOWN from X
    'TRCHI2DOF_MAX' : 4,
    'PT_MIN'        : '125*MeV', 
    'P_MIN'         : '0*MeV',
    'MIPCHI2DV_MIN' : 25, 
    'TRGHP_MAX'     : 0.3,
    'PROBNNpi_MIN'  : 0.1
    },
    "MuX" : { # muon from X
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '100*MeV', 
    'P_MIN'         : '0*MeV',
    'MIPCHI2DV_MIN' : 9, 
    'TRGHP_MAX'     : 0.3,
    'PIDmu_MIN'     : -5
    },
    "MuJ" : { # muon from J/psi
    'TRCHI2DOF_MAX' : 4,
    'PT_MIN'        : '125*MeV', 
    'P_MIN'         : '0*MeV',
    'MIPCHI2DV_MIN' : 25, 
    'TRGHP_MAX'     : 0.3,
    'PIDmu_MIN'     : -4
    },
    "E" : { # electron cuts 
    'TRCHI2DOF_MAX' : 5,
    'MIPCHI2DV_MIN' : 9, 
    'PT_MIN'        : '100*MeV',
    'TRGHP_MAX'     : 0.4,
    'PIDe_MIN'      : 0
    },
    "XLL" : { # making the X for LL
    'VCHI2DOF_MAX'  : 10,
    'BPVVDCHI2_MIN' : 25,
    'PT_MIN'        : '250*MeV'
    },
    "XLLhard" : { # making the X for LL hard vertex for the pions
    'VCHI2DOF_MAX'  : 5,
    'BPVVDCHI2_MIN' : 25,
    'PT_MIN'        : '250*MeV'
    },
    "XDD" : { # making X for DD
    'VCHI2DOF_MAX'  : 25,
    'BPVVDCHI2_MIN' : 25,
    'PT_MIN'        : '0*MeV'
    },
    "J" : { # cuts on J/psi
    'VCHI2DOF_MAX'  : 12,
    'ADAMASS_MAX'   : '100*MeV',
    },
    "B2KX" : { # B -> K X
    'SUMPT_MIN'     : '0*MeV',
    'VCHI2DOF_MAX'  : 25,
    'BPVIPCHI2_MAX' : 50,
    'BPVLTIME_MIN'  : '0.2*ps',
    'AM_MIN'        : '4800*MeV', 
    'AM_MAX'        : '5800*MeV',
    'HAD_MINIPCHI2_MIN' : 25,
    'PT_MIN'        : '1000*MeV'
    },
    "B2HHX" : { # B -> Kpi X, KK X
    'SUMPT_MIN'     : '0*MeV',
    'VCHI2DOF_MAX'  : 25,
    'BPVIPCHI2_MAX' : 50,
    'BPVLTIME_MIN'  : '0.2*ps',
    'AM_MIN'        : '4800*MeV', 
    'AM_MAX'        : '5800*MeV',
    'HAD_MINIPCHI2_MIN' : 9,
    'PT_MIN'        : '1000*MeV'
    },
    "Prescales" : { "SS" : 0.1},
    'GECNTrkMax'   : 250
}

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class DarkBosonConf(LineBuilder):
    __configuration_keys__ = ('KB','KBhard','PiB','KX','PiX','KDX','PiDX',
                              'MuX','MuJ','E','XLL','XLLhard','XDD','J',
                              'B2KX','B2HHX','Prescales','GECNTrkMax')
 
    def __init__(self, moduleName, config) :
        
        LineBuilder.__init__(self, moduleName, config)
        piD = DataOnDemand(Location="Phys/StdNoPIDsDownPions/Particles")
        kD = DataOnDemand(Location="Phys/StdLooseDownKaons/Particles")
        
        # pre-filter inputs
        kB = filterInputs('KB',config['KB'],[StdAllNoPIDsKaons])
        kBhard = filterInputs('KBhard',config['KBhard'],[StdAllNoPIDsKaons])
        piB = filterInputs('PiB',config['PiB'],[StdAllNoPIDsPions])
        kX = filterInputs('KX',config['KX'],[StdAllNoPIDsKaons])
        piX = filterInputs('PiX',config['PiX'],[StdAllNoPIDsPions])
        kDX = filterInputs('KDX',config['KDX'],[kD])
        piDX = filterInputs('PiDX',config['PiDX'],[piD])
        muX = filterInputs('MuX',config['MuX'],[StdAllLooseMuons])
        muDX = filterInputs('MuDX',config['MuX'],[StdLooseDownMuons])
        muJ = filterInputs('MuJ',config['MuJ'],[StdAllLooseMuons])

        # make/filter X
        eestd = DataOnDemand(Location='Phys/StdDiElectronFromTracks/Particles')
        ee = filterEE('OS',config['E'],config['XLL'],[eestd])
        #eeDD = makeEEdd(config)
        mmX = makeX('MuMu',config['XLL'],'KS0 -> mu+ mu-',[muX])
        mmDDX = makeX('MuMuDD',config['XDD'],'KS0 -> mu+ mu-',[muDX])
        kkX = makeX('KK',config['XLL'],'KS0 -> K+ K-',[kX])
        kkDDX = makeX('KKDD',config['XDD'],'KS0 -> K+ K-',[kDX])
        pipiX = makeX('PiPi',config['XLLhard'],'KS0 -> pi+ pi-',[piX])
        pipiDDX = makeX('PiPiDD',config['XDD'],'KS0 -> pi+ pi-',[piDX])

        # SS X
        eeSS = filterEE('SS',config['E'],config['XLL'],[makeEESS()])
        mmSSX = makeX('MuMuSS',config['XLL'],'[KS0 -> mu+ mu+]cc',[muX])
        mmSSDDX = makeX('MuMuSSDD',config['XDD'],'[KS0 -> mu+ mu+]cc',[muDX])
        kkSSX = makeX('KKSS',config['XLL'],'[KS0 -> K+ K+]cc',[kX])
        kkSSDDX = makeX('KKSSDD',config['XDD'],'[KS0 -> K+ K+]cc',[kDX])
        pipiSSX = makeX('PiPiSS',config['XLL'],'[KS0 -> pi+ pi+]cc',[piX])
        pipiSSDDX = makeX('PiPiSSDD',config['XDD'],'[KS0 -> pi+ pi+]cc',[piDX])
        preSS = config['Prescales']['SS']

        j = makeJ(config['J'],[muJ])

        # B -> K pi lines
        self._makeBLine('B2KpiX2MuMu',['[B0 -> K+ pi- KS0]cc'],
                        [mmX,kB,piB],config['B2HHX'])

        self._makeBLine('B2KpiX2MuMuDD',['[B0 -> K+ pi- KS0]cc'],
                        [mmDDX,kB,piB],config['B2HHX'])

        self._makeBLine('B2KpiX2EE',['[B0 -> K+ pi- KS0]cc'],
                        [ee,kB,piB],config['B2HHX'])

        #self._makeBLine('B2KpiX2KK',['[B0 -> K+ pi- KS0]cc'],
        #                [kkX,kB,piB],config['B2HHX'])

        #self._makeBLine('B2KpiX2KKDD',['[B0 -> K+ pi- KS0]cc'],
        #                [kkDDX,kB,piB],config['B2HHX'])

        #self._makeBLine('B2KpiX2PiPi',['[B0 -> K+ pi- KS0]cc'],
        #                [pipiX,kB,piB],config['B2HHX'])

        #self._makeBLine('B2KpiX2PiPiDD',['[B0 -> K+ pi- KS0]cc'],
        #                [pipiDDX,kB,piB],config['B2HHX'])
                
        self._makeBLine('B2KpiX2MuMuSS',['[B0 -> K+ pi- KS0]cc'],
                        [mmSSX,kB,piB],config['B2HHX'],preSS)

        self._makeBLine('B2KpiX2MuMuDDSS',['[B0 -> K+ pi- KS0]cc'],
                        [mmSSDDX,kB,piB],config['B2HHX'])

        self._makeBLine('B2KpiX2EESS',['[B0 -> K+ pi- KS0]cc'],
                        [eeSS,kB,piB],config['B2HHX'],preSS)

        # B -> K K lines
        self._makeBLine('B2KKX2MuMu',['B0 -> K+ K- KS0'],
                        [mmX,kB],config['B2HHX'])

        self._makeBLine('B2KKX2MuMuDD',['B0 -> K+ K- KS0'],
                        [mmDDX,kB],config['B2HHX'])
        
        self._makeBLine('B2KKX2EE',['B0 -> K+ K- KS0'],
                        [ee,kB],config['B2HHX'])

        self._makeBLine('B2KKX2MuMuSS',['B0 -> K+ K- KS0'],
                        [mmSSX,kB],config['B2HHX'],preSS)

        self._makeBLine('B2KKX2MuMuDDSS',['B0 -> K+ K- KS0'],
                        [mmSSDDX,kB],config['B2HHX'])

        self._makeBLine('B2KKX2EESS',['B0 -> K+ K- KS0'],
                        [eeSS,kB],config['B2HHX'],preSS)
        
        #self._makeBLine('B2KKX2KK',['B0 -> K+ K- KS0'],
        #                [kkX,kB],config['B2HHX'])

        #self._makeBLine('B2KKX2KKDD',['B0 -> K+ K- KS0'],
        #                [kkDDX,kB],config['B2HHX'])

        #self._makeBLine('B2KKX2PiPi',['B0 -> K+ K- KS0'],
        #                [pipiX,kB],config['B2HHX'])

        #self._makeBLine('B2KKX2PiPiDD',['B0 -> K+ K- KS0'],
        #                [pipiDDX,kB],config['B2HHX'])

        # B -> K lines     
        self._makeBLine('B2KX2MuMu',['[B+ -> K+ KS0]cc'],
                        [mmX,kB],config['B2KX'])

        self._makeBLine('B2KX2MuMuDD',['[B+ -> K+ KS0]cc'],
                        [mmDDX,kB],config['B2KX'])

        self._makeBLine('B2KX2EE',['[B+ -> K+ KS0]cc'],
                        [ee,kB],config['B2KX'])
        
        self._makeBLine('B2KX2KK',['[B+ -> K+ KS0]cc'],
                        [kkX,kB],config['B2KX'])

        self._makeBLine('B2KX2KKDD',['[B+ -> K+ KS0]cc'],
                        [kkDDX,kB],config['B2KX'])
   
        self._makeBLine('B2KX2PiPi',['[B+ -> K+ KS0]cc'],
                        [pipiX,kBhard],config['B2KX'])

        self._makeBLine('B2KX2PiPiDD',['[B+ -> K+ KS0]cc'],
                        [pipiDDX,kB],config['B2KX'])

        self._makeBLine('B2KX2MuMuSS',['[B+ -> K+ KS0]cc'],
                        [mmSSX,kB],config['B2KX'],preSS)

        self._makeBLine('B2KX2MuMuDDSS',['[B+ -> K+ KS0]cc'],
                        [mmSSDDX,kB],config['B2KX'])
        
        self._makeBLine('B2KX2EESS',['[B+ -> K+ KS0]cc'],
                        [eeSS,kB],config['B2KX'],preSS)

        self._makeBLine('B2KX2KKSS',['[B+ -> K+ KS0]cc'],
                        [kkSSX,kB],config['B2KX'],preSS)

        self._makeBLine('B2KX2KKDDSS',['[B+ -> K+ KS0]cc'],
                        [kkSSDDX,kB],config['B2KX'])
   
        self._makeBLine('B2KX2PiPiSS',['[B+ -> K+ KS0]cc'],
                        [pipiSSX,kB],config['B2KX'],preSS)

        self._makeBLine('B2KX2PiPiDDSS',['[B+ -> K+ KS0]cc'],
                        [pipiSSDDX,kB],config['B2KX'])

        # B -> J/psi lines
        self._makeBLine('B2JK',['[B+ -> J/psi(1S) K+]cc'],
                        [j,kB],config['B2KX'])

        kst = filterKst()
        self._makeBLine('B2JKst',['[B0 -> J/psi(1S) K*(892)0]cc'],
                        [j,kst],config['B2KX'])
        rho = makeRho([piB])
        self._makeBLine('B2RhoX2MuMu',['B0 -> rho(770)0 KS0'],[rho,mmX],
                        config['B2HHX'])
        self._makeBLine('B2RhoX2MuMuDD',['B0 -> rho(770)0 KS0'],[rho,mmDDX],
                        config['B2HHX'])
        

    def _makeBLine(self,tag,dec,inputs,config,pre=1.0):
        sel = makeB(tag,dec,inputs,config)
        self._makeLine(tag,sel,pre)

    def _makeLine(self,name,sel,pre):
        filt = {'Code' :
                "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)"\
                " < %s )" \
                % config['GECNTrkMax'],
                'Preambulo' : [ "from LoKiTracks.decorators import *",
                                'from LoKiCore.functions    import *' ]}
        name = name + "DarkBosonLine"
        sline = StrippingLine(name,pre,selection=sel,checkPV=True,FILTER=filt,
                              EnableFlavourTagging=False)
        self.registerLine(sline)

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def filterInputs(which,conf,inputs):
    keys = [key.split('_')[0] for key in conf.keys()]    
    code = LoKiCuts(keys,conf).code()
    code = LoKiCuts.combine(['INMUON',code])
    #print "filterInputs: ", which, code
    return Selection(which+'DarkBosonFilter',Algorithm=FilterDesktop(Code=code),
                     RequiredSelections=inputs)

def filterEE(which,econf,xconf,inputs):
    code = "(PT > %s) & (MINTREE('e+'==ABSID,PIDe) > %s)" \
           % (xconf['PT_MIN'],econf['PIDe_MIN'])
    code += " & (MINTREE('e+'==ABSID,MIPCHI2DV(PRIMARY)) > %s)" \
            % econf['MIPCHI2DV_MIN']
    code += " & (MINTREE('e+'==ABSID,PT) > %s)" % econf['PT_MIN']
    code += " & (MAXTREE('e+'==ABSID,TRGHP) < %s)" % econf['TRGHP_MAX']
    code = LoKiCuts.combine([code,LoKiCuts(['BPVVDCHI2','VCHI2DOF'],
                                           xconf).code()])
    #print "filterEE: ", code
    sel = Selection(which+'FilterEEDarkBosonFilter',
                    Algorithm=FilterDesktop(Code=code),
                    RequiredSelections=inputs)
    sub = SubstitutePID(which+'EESubPIDDarkBoson',
                        Code="DECTREE('J/psi(1S) -> e+ e-')")
    sub.MaxChi2PerDoF = -666
    sub.Substitutions = {'J/psi(1S) -> e+ e-'  : 'KS0'}
    return Selection(which+'EESubPIDDarkBosonSel',Algorithm=sub,
                     RequiredSelections=[sel])

def makeEEdd(config):
    from CommonParticles.Utils import *
    from Configurables import DiElectronMaker,ProtoParticleCALOFilter
    eedd = DiElectronMaker('EEDDForDarkBoson')
    eedd.Particle = "KS0"
    selector = trackSelector(eedd, trackTypes=["Downstream"]) 
    eedd.addTool(ProtoParticleCALOFilter, name='Electron')
    eedd.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>'%s'" \
                               % config['PID']['E']['PIDe_MIN']]
    eedd.DiElectronMassMax = 5000
    eedd.DiElectronMassMin = 0
    eedd.DiElectronPtMin = 250
    eedd.ElectronPtMin = 100
    eedd.ElectronPIDcut = config['PID']['E']['PIDe_MIN']
    
    sel = Selection('EEDDDarkBosonSel',Algorithm=eedd)
    code = LoKiCuts(['BPVVDCHI2','VCHI2DOF'],config['V']).code()
    #print 'makeEEdd', code
    return Selection('FilterEEDDDarkBoson',Algorithm=FilterDesktop(Code=code),
                     RequiredSelections=[sel])

def makeEESS():
    from CommonParticles.Utils import *
    from Configurables import DiElectronMaker,ProtoParticleCALOFilter
    eess = DiElectronMaker('EESSForDarkBoson')
    eess.Particle = "J/psi(1S)"
    selector = trackSelector(eess, trackTypes=["Long"]) 
    eess.addTool(ProtoParticleCALOFilter, name='Electron')
    eess.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>-2"]
    eess.DiElectronMassMax = 5000
    eess.DiElectronMassMin = 0
    eess.DiElectronPtMin = 250
    eess.ElectronPtMin = 100
    return Selection('EESSDarkBosonSel',Algorithm=eess)

def filterKst():
    kst = DataOnDemand(Location='Phys/StdLooseDetachedKst2Kpi/Particles')
    code = '(MIPCHI2DV(PRIMARY) > 25)'
    return Selection('KstDarkBosonFilter',Algorithm=FilterDesktop(Code=code),
                     RequiredSelections=[kst])

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def makeX(which,config,dec,inputs):
    comboCuts = "(AM < 5000*MeV) & (APT > %s)" % config['PT_MIN']
    if(which.find('DD') < 0): 
        comboCuts += " & (ACUTDOCA(0.2*mm,''))"
    comboCuts += "& (ADOCACHI2CUT(25,''))"
    momCuts = LoKiCuts(['BPVVDCHI2','VCHI2DOF'],config).code()
    momCuts = LoKiCuts.combine(['(BPVDIRA > 0)',momCuts])
    x = CombineParticles(DecayDescriptors=[dec],
                         CombinationCut=comboCuts,MotherCut=momCuts)
    #print 'makeX:',which,comboCuts,momCuts
    return Selection("X2"+which+"DarkBosonSel",Algorithm=x,
                     RequiredSelections=inputs)

def makeJ(config,inputs):
    comboCuts = "(ADAMASS('J/psi(1S)') < %s)" % config['ADAMASS_MAX']    
    comboCuts += " & (ACUTDOCA(0.2*mm,''))"
    comboCuts += "& (ADOCACHI2CUT(25,''))"
    momCuts = LoKiCuts(['VCHI2DOF'],config).code()
    momCuts = LoKiCuts.combine(['(BPVDIRA > 0)',momCuts])
    j = CombineParticles(DecayDescriptors=['J/psi(1S) -> mu+ mu-'],
                         CombinationCut=comboCuts,MotherCut=momCuts)
    #print 'makeJ:',comboCuts,momCuts
    return Selection("J2MuMuDarkBosonSel",Algorithm=j,
                     RequiredSelections=inputs)

def makeRho(inputs):
    comboCuts = "(AM > 550*MeV) & (AM < 1050*MeV) & ADOCACHI2CUT(25,'')"
    momCuts = "(VFASPF(VCHI2/VDOF)<10) & (MIPCHI2DV(PRIMARY)> 16)"
    momCuts += " & (M > 600*MeV) & (M < 1000*MeV)"
    rho = CombineParticles(DecayDescriptors=['rho(770)0 -> pi+ pi-'],
                           CombinationCut=comboCuts,MotherCut=momCuts)
    #print 'makeRho:',comboCuts,momCuts
    return Selection("Rho2PiPiDarkBosonSel",Algorithm=rho,
                     RequiredSelections=inputs)

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def makeB(name,decays,inputs,config):
    comboCuts = LoKiCuts(['AM','SUMPT'],config).code()
    momCuts = LoKiCuts(['PT','VCHI2DOF','BPVLTIME','BPVIPCHI2'],config).code()
    cuts = '(BPVDIRA > 0) & '
    cuts += '(NINGENERATION(ISBASIC & (MIPCHI2DV(PRIMARY) < %s),1)==0)' \
            % config['HAD_MINIPCHI2_MIN']
    momCuts = LoKiCuts.combine([cuts,momCuts])
    momCuts += ' & (MM > %s) & (MM < %s)' % (config['AM_MIN'],config['AM_MAX'])
    alg = CombineParticles(DecayDescriptors=decays)
    alg.ReFitPVs = True
    alg.CombinationCut=comboCuts
    alg.MotherCut=momCuts
    #print 'makeB:',name,comboCuts,momCuts,decays
    return Selection(name+"DarkBosonSel",Algorithm=alg,
                     RequiredSelections=inputs)
    
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

