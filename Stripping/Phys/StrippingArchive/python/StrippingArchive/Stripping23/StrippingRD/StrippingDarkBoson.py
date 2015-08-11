#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
'''
Dark Bosons (ALPs, inflatons, WTFs, etc)

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
from CommonParticles.Utils import *
from Configurables import DiElectronMaker,ProtoParticleCALOFilter


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

# Default configuration dictionary
default_config = {
    'NAME' : 'DarkBoson',
    'BUILDERTYPE' : 'DarkBosonConf' ,
    'STREAMS' : {
        'Dimuon' : [
           'StrippingB2KpiX2MuMuDDDarkBosonLine',
           'StrippingB2KpiX2MuMuDDSSDarkBosonLine',
           'StrippingB2KKX2MuMuDDDarkBosonLine',
           'StrippingB2KKX2MuMuDDSSDarkBosonLine',
           'StrippingB2KX2MuMuDDDarkBosonLine',
           'StrippingB2KX2MuMuDDSSDarkBosonLine',
           'StrippingB2KpiX2PiPiDDDarkBosonLine',
           'StrippingB2KKX2PiPiDDDarkBosonLine',
           'StrippingB2KX2PiPiDDDarkBosonLine',
           'StrippingB2KX2PiPiDDSSDarkBosonLine',
           'StrippingB2RhoX2MuMuDDDarkBosonLine',
           'StrippingB2KpiX2KKDDDarkBosonLine',
           'StrippingB2KKX2KKDDDarkBosonLine',
           'StrippingB2KX2KKDDDarkBosonLine',
           'StrippingB2KX2KKDDSSDarkBosonLine',
           ],
        'Leptonic' : [
           'StrippingB2KpiX2MuMuDarkBosonLine', 
           'StrippingB2KpiX2MuMuSSDarkBosonLine',
           'StrippingB2KKX2MuMuDarkBosonLine',
           'StrippingB2KKX2MuMuSSDarkBosonLine',
           'StrippingB2KX2MuMuDarkBosonLine',
           'StrippingB2KX2MuMuSSDarkBosonLine',
           'StrippingB2KpiX2PiPiDarkBosonLine',
           'StrippingB2KKX2PiPiDarkBosonLine',
           'StrippingB2KX2PiPiDarkBosonLine',
           'StrippingB2KX2PiPiSSDarkBosonLine',
           'StrippingB2RhoX2MuMuDarkBosonLine',
           'StrippingB2KpiX2KKDarkBosonLine',
           'StrippingB2KKX2KKDarkBosonLine',
           'StrippingB2KX2KKDarkBosonLine',
           'StrippingB2KX2KKSSDarkBosonLine',
           'StrippingB2KpiX2EEDarkBosonLine',
           'StrippingB2KpiX2EESSDarkBosonLine',
           'StrippingB2KKX2EEDarkBosonLine',
           'StrippingB2KKX2EESSDarkBosonLine',
           'StrippingB2KX2EEDarkBosonLine',
           'StrippingB2KX2EESSDarkBosonLine',
           'StrippingB2JKDarkBosonLine',
           'StrippingB2JKstDarkBosonLine',
           ]
    }, 
    'WGs'     : [ 'RD' ],
    'CONFIG'  : {  
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
    'P_MIN'         : '3000*MeV',
    'MIPCHI2DV_MIN' : 36,
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
    'P_MIN'         : '3000*MeV',
    'MIPCHI2DV_MIN' : 25, 
    'TRGHP_MAX'     : 0.3,
    'PROBNNK_MIN'   : 0.1 
    },
    "PiX" : { # pi from X
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '250*MeV', 
    'P_MIN'         : '3000*MeV',
    'MIPCHI2DV_MIN' : 36, 
    'TRGHP_MAX'     : 0.3,
    'PROBNNpi_MIN'  : 0.6 
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
    'VCHI2DOF_MAX'  : 15,
    'BPVVDCHI2_MIN' : 25,
    'PT_MIN'        : '0*MeV'
    },
    "J" : { # cuts on J/psi
    'VCHI2DOF_MAX'  : 12,
    'ADAMASS_MAX'   : '100*MeV',
    },
    "B2KX" : { # B -> K X
    'SUMPT_MIN'     : '0*MeV',
    'VCHI2DOF_MAX'  : 15,
    'BPVIPCHI2_MAX' : 10,
    'BPVLTIME_MIN'  : '0.2*ps',
    'AM_MIN'        : '4800*MeV', 
    'AM_MAX'        : '5800*MeV',
    'HAD_MINIPCHI2_MIN' : 25,
    'PT_MIN'        : '1000*MeV'
    },
    "B2HHX" : { # B -> Kpi X, KK X
    'SUMPT_MIN'     : '0*MeV',
    'VCHI2DOF_MAX'  : 15,
    'BPVIPCHI2_MAX' : 10,
    'BPVLTIME_MIN'  : '0.2*ps',
    'AM_MIN'        : '4800*MeV', 
    'AM_MAX'        : '5800*MeV',
    'HAD_MINIPCHI2_MIN' : 9,
    'PT_MIN'        : '1000*MeV'
    },
    "Prescales" : { "SS" : 0.1, "DD" : 0.5, "KK": 0.25},
    "GECNTrkMax" : 250
    }
}

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class DarkBosonConf(LineBuilder):
    __configuration_keys__ = ('KB','KBhard','PiB','KX','PiX','KDX','PiDX',
                              'MuX','MuJ','E','XLL','XLLhard','XDD','J',
                              'B2KX','B2HHX','Prescales','GECNTrkMax')
 
    def __init__(self, moduleName, default_config) :
        
        LineBuilder.__init__(self, moduleName, default_config)
        piD = DataOnDemand(Location="Phys/StdNoPIDsDownPions/Particles")
        kD = DataOnDemand(Location="Phys/StdLooseDownKaons/Particles")
        
        # pre-filter inputs
        kB = filterInputs('KB',default_config['KB'],[StdAllNoPIDsKaons])
        kBhard = filterInputs('KBhard',default_config['KBhard'],[StdAllNoPIDsKaons])
        piB = filterInputs('PiB',default_config['PiB'],[StdAllNoPIDsPions])
        kX = filterInputs('KX',default_config['KX'],[StdAllNoPIDsKaons])
        piX = filterInputs('PiX',default_config['PiX'],[StdAllNoPIDsPions])
        kDX = filterInputs('KDX',default_config['KDX'],[kD])
        piDX = filterInputs('PiDX',default_config['PiDX'],[piD])
        muX = filterInputs('MuX',default_config['MuX'],[StdAllLooseMuons])
        muDX = filterInputs('MuDX',default_config['MuX'],[StdLooseDownMuons])
        muJ = filterInputs('MuJ',default_config['MuJ'],[StdAllLooseMuons])

        # make/filter X
        eestd = DataOnDemand(Location='Phys/StdDiElectronFromTracks/Particles')
        ee = filterEE('OS',default_config['E'],default_config['XLL'],[eestd])
        #eeDD = makeEEdd(config)
        mmX = makeX('MuMu',default_config['XLL'],'KS0 -> mu+ mu-',[muX])
        mmDDX = makeX('MuMuDD',default_config['XDD'],'KS0 -> mu+ mu-',[muDX])
        kkX = makeX('KK',default_config['XLL'],'KS0 -> K+ K-',[kX])
        kkDDX = makeX('KKDD',default_config['XDD'],'KS0 -> K+ K-',[kDX])
        pipiX = makeX('PiPi',default_config['XLLhard'],'KS0 -> pi+ pi-',[piX])
        pipiDDX = makeX('PiPiDD',default_config['XDD'],'KS0 -> pi+ pi-',[piDX])

        # SS X
        eeSS = filterEE('SS',default_config['E'],default_config['XLL'],[makeEESS()])
        mmSSX = makeX('MuMuSS',default_config['XLL'],'[KS0 -> mu+ mu+]cc',[muX])
        mmSSDDX = makeX('MuMuSSDD',default_config['XDD'],'[KS0 -> mu+ mu+]cc',[muDX])
        kkSSX = makeX('KKSS',default_config['XLL'],'[KS0 -> K+ K+]cc',[kX])
        kkSSDDX = makeX('KKSSDD',default_config['XDD'],'[KS0 -> K+ K+]cc',[kDX])
        pipiSSX = makeX('PiPiSS',default_config['XLL'],'[KS0 -> pi+ pi+]cc',[piX])
        pipiSSDDX = makeX('PiPiSSDD',default_config['XDD'],'[KS0 -> pi+ pi+]cc',[piDX])
        preSS = default_config['Prescales']['SS']
        
        preDD = default_config['Prescales']['DD']
        preKK = default_config['Prescales']['KK']

        j = makeJ(default_config['J'],[muJ])

        # B -> K pi lines
        self._makeBLine('B2KpiX2MuMu',['[B0 -> K+ pi- KS0]cc'],
                        [mmX,kB,piB],default_config['B2HHX'])

        self._makeBLine('B2KpiX2MuMuDD',['[B0 -> K+ pi- KS0]cc'],
                        [mmDDX,kB,piB],default_config['B2HHX'])

        self._makeBLine('B2KpiX2EE',['[B0 -> K+ pi- KS0]cc'],
                        [ee,kB,piB],default_config['B2HHX'])

        self._makeBLine('B2KpiX2KK',['[B0 -> K+ pi- KS0]cc'],
                        [kkX,kB,piB],default_config['B2HHX'],preKK)

        self._makeBLine('B2KpiX2KKDD',['[B0 -> K+ pi- KS0]cc'],
                        [kkDDX,kB,piB],default_config['B2HHX'])

        self._makeBLine('B2KpiX2PiPi',['[B0 -> K+ pi- KS0]cc'],
                        [pipiX,kB,piB],default_config['B2HHX'],preSS)

        self._makeBLine('B2KpiX2PiPiDD',['[B0 -> K+ pi- KS0]cc'],
                        [pipiDDX,kB,piB],default_config['B2HHX'],preDD)
                
        self._makeBLine('B2KpiX2MuMuSS',['[B0 -> K+ pi- KS0]cc'],
                        [mmSSX,kB,piB],default_config['B2HHX'],preSS)

        self._makeBLine('B2KpiX2MuMuDDSS',['[B0 -> K+ pi- KS0]cc'],
                        [mmSSDDX,kB,piB],default_config['B2HHX'])

        self._makeBLine('B2KpiX2EESS',['[B0 -> K+ pi- KS0]cc'],
                        [eeSS,kB,piB],default_config['B2HHX'],preSS)

        # B -> K K lines
        self._makeBLine('B2KKX2MuMu',['B0 -> K+ K- KS0'],
                        [mmX,kB],default_config['B2HHX'])

        self._makeBLine('B2KKX2MuMuDD',['B0 -> K+ K- KS0'],
                        [mmDDX,kB],default_config['B2HHX'])
        
        self._makeBLine('B2KKX2EE',['B0 -> K+ K- KS0'],
                        [ee,kB],default_config['B2HHX'])

        self._makeBLine('B2KKX2MuMuSS',['B0 -> K+ K- KS0'],
                        [mmSSX,kB],default_config['B2HHX'],preSS)

        self._makeBLine('B2KKX2MuMuDDSS',['B0 -> K+ K- KS0'],
                        [mmSSDDX,kB],default_config['B2HHX'])

        self._makeBLine('B2KKX2EESS',['B0 -> K+ K- KS0'],
                        [eeSS,kB],default_config['B2HHX'],preSS)
        
        self._makeBLine('B2KKX2KK',['B0 -> K+ K- KS0'],
                        [kkX,kB],default_config['B2HHX'])

        self._makeBLine('B2KKX2KKDD',['B0 -> K+ K- KS0'],
                        [kkDDX,kB],default_config['B2HHX'])

        self._makeBLine('B2KKX2PiPi',['B0 -> K+ K- KS0'],
                        [pipiX,kB],default_config['B2HHX'],preDD)

        self._makeBLine('B2KKX2PiPiDD',['B0 -> K+ K- KS0'],
                        [pipiDDX,kB],default_config['B2HHX'])

        # B -> K lines     
        self._makeBLine('B2KX2MuMu',['[B+ -> K+ KS0]cc'],
                        [mmX,kB],default_config['B2KX'])

        self._makeBLine('B2KX2MuMuDD',['[B+ -> K+ KS0]cc'],
                        [mmDDX,kB],default_config['B2KX'])

        self._makeBLine('B2KX2EE',['[B+ -> K+ KS0]cc'],
                        [ee,kB],default_config['B2KX'])
        
        self._makeBLine('B2KX2KK',['[B+ -> K+ KS0]cc'],
                        [kkX,kB],default_config['B2KX'])

        self._makeBLine('B2KX2KKDD',['[B+ -> K+ KS0]cc'],
                        [kkDDX,kB],default_config['B2KX'])
   
        self._makeBLine('B2KX2PiPi',['[B+ -> K+ KS0]cc'],
                        [pipiX,kBhard],default_config['B2KX'])

        self._makeBLine('B2KX2PiPiDD',['[B+ -> K+ KS0]cc'],
                        [pipiDDX,kB],default_config['B2KX'])

        self._makeBLine('B2KX2MuMuSS',['[B+ -> K+ KS0]cc'],
                        [mmSSX,kB],default_config['B2KX'],preSS)

        self._makeBLine('B2KX2MuMuDDSS',['[B+ -> K+ KS0]cc'],
                        [mmSSDDX,kB],default_config['B2KX'])
        
        self._makeBLine('B2KX2EESS',['[B+ -> K+ KS0]cc'],
                        [eeSS,kB],default_config['B2KX'],preSS)

        self._makeBLine('B2KX2KKSS',['[B+ -> K+ KS0]cc'],
                        [kkSSX,kB],default_config['B2KX'],preSS)

        self._makeBLine('B2KX2KKDDSS',['[B+ -> K+ KS0]cc'],
                        [kkSSDDX,kB],default_config['B2KX'])
   
        self._makeBLine('B2KX2PiPiSS',['[B+ -> K+ KS0]cc'],
                        [pipiSSX,kB],default_config['B2KX'],preSS)

        self._makeBLine('B2KX2PiPiDDSS',['[B+ -> K+ KS0]cc'],
                        [pipiSSDDX,kB],default_config['B2KX'])

        # B -> J/psi lines
        self._makeBLine('B2JK',['[B+ -> J/psi(1S) K+]cc'],
                        [j,kB],default_config['B2KX'])

        kst = filterKst()
        self._makeBLine('B2JKst',['[B0 -> J/psi(1S) K*(892)0]cc'],
                        [j,kst],default_config['B2KX'])
        rho = makeRho([piB])
        self._makeBLine('B2RhoX2MuMu',['B0 -> rho(770)0 KS0'],[rho,mmX],
                        default_config['B2HHX'])
        self._makeBLine('B2RhoX2MuMuDD',['B0 -> rho(770)0 KS0'],[rho,mmDDX],
                        default_config['B2HHX'])
        

    def _makeBLine(self,tag,dec,inputs,default_config,pre=1.0):
        sel = makeB(tag,dec,inputs,default_config)
        self._makeLine(tag,sel,pre)

    def _makeLine(self,name,sel,pre):
        filt = {'Code' :
                "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)"\
                " < %s )" \
                % default_config['CONFIG']['GECNTrkMax'],               
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
    #code = LoKiCuts.combine(['INMUON',code])
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
    code = LoKiCuts.combine([code,LoKiCuts(['HASVERTEX','BPVVDCHI2','VCHI2DOF'],
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

def makeEEdd(default_config):
    eedd = DiElectronMaker('EEDDForDarkBoson')
    eedd.Particle = "KS0"
    selector = trackSelector(eedd, trackTypes=["Downstream"]) 
    eedd.addTool(ProtoParticleCALOFilter, name='Electron')
    eedd.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>'%s'" \
                               % default_config['PID']['E']['PIDe_MIN']]
    eedd.DiElectronMassMax = 5000
    eedd.DiElectronMassMin = 0
    eedd.DiElectronPtMin = 250
    eedd.ElectronPtMin = 100
    eedd.ElectronPIDcut = default_config['PID']['E']['PIDe_MIN']
    
    sel = Selection('EEDDDarkBosonSel',Algorithm=eedd)
    code = LoKiCuts(['HASVERTEX','BPVVDCHI2','VCHI2DOF'],default_config['V']).code()
    #print 'makeEEdd', code
    return Selection('FilterEEDDDarkBoson',Algorithm=FilterDesktop(Code=code),
                     RequiredSelections=[sel])

def makeEESS():
    eess = DiElectronMaker('EESSForDarkBoson')
    eess.Particle = "J/psi(1S)"
    selector = trackSelector(eess, trackTypes=["Long"]) 
    eess.addTool(ProtoParticleCALOFilter, name='Electron')
    eess.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>-2"]
    eess.DiElectronMassMax = 5000
    eess.DiElectronMassMin = 0
    eess.DiElectronPtMin = 250
    eess.ElectronPtMin = 100
    eess.OppositeSign = False
    return Selection('EESSDarkBosonSel',Algorithm=eess)

def filterKst():
    kst = DataOnDemand(Location='Phys/StdLooseDetachedKst2Kpi/Particles')
    code = '(MIPCHI2DV(PRIMARY) > 25)'
    return Selection('KstDarkBosonFilter',Algorithm=FilterDesktop(Code=code),
                     RequiredSelections=[kst])

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def makeX(which,default_config,dec,inputs):
    comboCuts = "(AM < 5000*MeV) & (APT > %s)" % default_config['PT_MIN']
    if(which.find('DD') < 0): 
        comboCuts += " & (ACUTDOCA(0.2*mm,''))"
    comboCuts += "& (ADOCACHI2CUT(25,''))"
    momCuts = LoKiCuts(['HASVERTEX','BPVVDCHI2','VCHI2DOF'],default_config).code()
    momCuts = LoKiCuts.combine(['(BPVDIRA > 0)',momCuts])
    x = CombineParticles(DecayDescriptors=[dec],
                         CombinationCut=comboCuts,MotherCut=momCuts)
    #print 'makeX:',which,comboCuts,momCuts
    return Selection("X2"+which+"DarkBosonSel",Algorithm=x,
                     RequiredSelections=inputs)

def makeJ(default_config,inputs):
    comboCuts = "(ADAMASS('J/psi(1S)') < %s)" % default_config['ADAMASS_MAX']    
    comboCuts += " & (ACUTDOCA(0.2*mm,''))"
    comboCuts += "& (ADOCACHI2CUT(25,''))"
    momCuts = LoKiCuts(['HASVERTEX','VCHI2DOF'],default_config).code()
    momCuts = LoKiCuts.combine(['(BPVDIRA > 0)',momCuts])
    j = CombineParticles(DecayDescriptors=['J/psi(1S) -> mu+ mu-'],
                         CombinationCut=comboCuts,MotherCut=momCuts)
    #print 'makeJ:',comboCuts,momCuts
    return Selection("J2MuMuDarkBosonSel",Algorithm=j,
                     RequiredSelections=inputs)

def makeRho(inputs):
    comboCuts = "(AM > 550*MeV) & (AM < 1050*MeV) & ADOCACHI2CUT(25,'')"
    momCuts = "(VFASPF(VCHI2/VDOF)<10) & (MIPCHI2DV(PRIMARY)> 16) & HASVERTEX"
    momCuts += " & (M > 600*MeV) & (M < 1000*MeV)"
    rho = CombineParticles(DecayDescriptors=['rho(770)0 -> pi+ pi-'],
                           CombinationCut=comboCuts,MotherCut=momCuts)
    #print 'makeRho:',comboCuts,momCuts
    return Selection("Rho2PiPiDarkBosonSel",Algorithm=rho,
                     RequiredSelections=inputs)

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def makeB(name,decays,inputs,default_config):
    comboCuts = LoKiCuts(['AM','SUMPT'],default_config).code()
    momCuts = LoKiCuts(['HASVERTEX','PT','VCHI2DOF','BPVLTIME','BPVIPCHI2'],default_config).code()
    cuts = '(BPVDIRA > 0) & '
    cuts += '(NINGENERATION(ISBASIC & (MIPCHI2DV(PRIMARY) < %s),1)==0)' \
            % default_config['HAD_MINIPCHI2_MIN']
    momCuts = LoKiCuts.combine([cuts,momCuts])
    momCuts += ' & (MM > %s) & (MM < %s)' % (default_config['AM_MIN'],default_config['AM_MAX'])
    alg = CombineParticles(DecayDescriptors=decays)
    alg.ReFitPVs = True
    alg.CombinationCut=comboCuts
    alg.MotherCut=momCuts
    #print 'makeB:',name,comboCuts,momCuts,decays
    return Selection(name+"DarkBosonSel",Algorithm=alg,
                     RequiredSelections=inputs)
    
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

