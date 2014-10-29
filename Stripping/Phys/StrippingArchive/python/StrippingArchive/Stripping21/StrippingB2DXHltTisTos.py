'''
B->DX where currently D->hh,hhh (all pi,K mass combos) and X = pi,k.

Author: M. Williams
'''
__author__ = ['Mike Williams']

moduleName = 'B2DXHltTisTos'

from copy import copy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder, checkConfig
from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons, StdNoPIDsProtons
#from StandardParticles import StdNoPIDsDownPions
#from StandardParticles import StdLooseMergedPi0, StdLooseResolvedPi0

# Default configuration dictionary
config = {
    'TrkChi2Max'   : 4,         # Max track chi2/ndof
    'BCuts' : {
    'TauMin'       : 0.3,
    'IPChi2Max'    : 25,
    'DIRAMin'      : 0.999,
    'VChi2Max'     : 30,
    'PTSumMin'     : 5000,
    'BMassWindow'  : 500
    },
    'BachCuts' : {
    'IPChi2Min'    : 4,
    'PTMin'        : 500,
    'PMin'         : 5000
    },
    'DCuts' : {
    'DauPTMin'     : 100,
    'DauPMin'      : 1000,
    'DauIPChi2Min' : 4,
    'SumPTMin'     : 1500,
    'IPChi2Min'    : 0,
    'MaxDauIPChi2Min' : 36,
    'VChi2Max'     : 30,
    'DMassWindow'  : 100,
    'FDChi2Min'    : 36,
    'DOCAMax'      : 1.0
    },
    "Prescales" : {             # Prescales for individual lines
    "D2hh"         : 1.,
    "D2hhTIS"      : 1.,
    "D2hhWS"       : 0.1,
    "D2hhh"        : 1.,
    "D2hhhTIS"     : 1.,
    "D2hhhWS"      : 0.1,
    },
    'GECNTrkMax'   : 500
    }

class B2DXHltTisTosConf(LineBuilder):
    __configuration_keys__ = ('TrkChi2Max','BCuts','BachCuts','DCuts',
                              'Prescales','GECNTrkMax')

    def __init__(self, moduleName, config) :
        from Configurables import LoKi__VoidFilter as VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory

        LineBuilder.__init__(self, moduleName, config)
        modules = CoreFactory('CoreFactory').Modules
        for i in ['LoKiTracks.decorators']:
            if i not in modules : modules.append(i)

        dcuts = config['DCuts']
        bcuts = config['BCuts']
        bchcuts = config['BachCuts']
        dcuts['DauTrkChi2Max'] = config['TrkChi2Max']
        bchcuts['TrkChi2Max'] = config['TrkChi2Max']
        prescales = config['Prescales']

        d0RSSels = {'D2hh' : makeD2hhRS(moduleName,dcuts)}
        #d0WSSels = {'D2hhWS' : makeD2hhWS(moduleName,dcuts)}
        dchRSSels = {'D2hhh' : makeD2hhhRS(moduleName,dcuts)}
        dchWSSels = {'D2hhhWS' : makeD2hhhWS(moduleName,dcuts)}

        # right-sign lines
        for name, sel in d0RSSels.iteritems():
            tos,tis = makeB2D0hRS(moduleName,name,sel,bcuts,bchcuts)
            self.makeLine(tos,name,config)
            self.makeLine(tis,name+'TIS',config)

        for name, sel in dchRSSels.iteritems():
            tos,tis = makeB02DhRS(moduleName,name,sel,bcuts,bchcuts)
            self.makeLine(tos,name,config)
            self.makeLine(tis,name+'TIS',config)

        # wrong-sign lines
        for name, sel in dchRSSels.iteritems(): # using RS here is correct!
            wssel = dchWSSels[name+'WS']
            tos,tis = makeB02DhWS(moduleName,name,sel,wssel,bcuts,bchcuts)
            self.makeLine(tos,name+'WS',config)

    def makeLine(self,sel,tag,config):
        tmpSel = Selection(sel.name()+'FilterALL',
                           Algorithm=FilterDesktop(Code='ALL'),
                           RequiredSelections=[sel])
        filter = {"Code": "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s )" \
                 % config['GECNTrkMax']        ,
                 "Preambulo" : ["from LoKiTracks.decorators import *"]
                 }
        line = StrippingLine(sel.name()+'Line',
                             prescale=config['Prescales'][tag],
                             selection=tmpSel,
                             checkPV=True,FILTER=filter,
                             HLT="HLT_PASS_RE('Hlt2Topo.*Decision')")
        self.registerLine(line)

def dmassStr(functor,particle,window):
    usesStrange = ['D+','D-','B0']
    if particle in usesStrange:
        particle_s = particle[0]+'_s'+particle[-1]
        return "((%s('%s') < %d*MeV) | (%s('%s') < %d*MeV))" \
               % (functor,particle,window,functor,particle_s,window)
    else:
        return "(%s('%s') < %d*MeV)" % (functor,particle,window)

def makeD2XSel(name, config, decay):
    '''Does all D->X selections.'''
    where = decay.find('D')
    if where < 0: print 'ERROR! No D in descriptor "%s"' % decay
    which = decay[where:where+2]
    input = []
    if decay.find('K') >= 0: input.append(StdNoPIDsKaons)
    if decay.find('pi') >= 0: input.append(StdNoPIDsPions)

    dauCut = "((TRCHI2DOF<%(DauTrkChi2Max)s) & " \
             "(PT > %(DauPTMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
             "(MIPCHI2DV(PRIMARY) > %(DauIPChi2Min)s))" % config
    dauCuts = {'pi+':dauCut,'K+':dauCut}
    #comboCut = "(APT>%(PTMin)s*MeV) & " \
    comboCut = "(ASUM(PT)>%(SumPTMin)s*MeV) & " \
               "AHASCHILD(((ABSID == 'K+')|(ABSID == 'pi+')) & "\
               "(PT > 500*MeV) & (P > 5000*MeV)) &"\
               "(AMAXDOCA('LoKi::TrgDistanceCalculator') < %(DOCAMax)s) & " \
               "AHASCHILD(((ABSID == 'K+')|(ABSID == 'pi+')) & "\
               "(MIPCHI2DV(PRIMARY) > %(MaxDauIPChi2Min)s)) & " \
               % config
    dmassWindow = int(config['DMassWindow'])
    comboCut += dmassStr('ADAMASS',which,dmassWindow+10)
    momCut = dmassStr('ADMASS',which,dmassWindow)+' & '
    momCut +="(VFASPF(VCHI2/VDOF)<%(VChi2Max)s) & (BPVVDCHI2 > %(FDChi2Min)s)"\
              "& (BPVIPCHI2() > %(IPChi2Min)s)" % config

    d2hh = CombineParticles(DecayDescriptors=[decay],DaughtersCuts=dauCuts,
                            CombinationCut=comboCut,MotherCut=momCut)

    return Selection(name,Algorithm=d2hh,RequiredSelections=input)

def makeD2XMerged(name,moduleName,decays,dcuts):
    '''Merges D->X selections.'''
    sels = []
    for tag, decay in decays.iteritems():
        sels.append(makeD2XSel(tag+'For'+moduleName,dcuts,decay))
    return MergedSelection(name+'For'+moduleName,RequiredSelections=sels)

def makeD2hhRS(moduleName,dcuts):
    '''Makes D -> pi+pi-,K+K-,K-pi+,K+pi-'''
    decays = {'D2pipi': 'D0 -> pi- pi+',
              'D2Kpi' :'[D0 -> K- pi+]cc',
              'D2KK'  : 'D0 -> K- K+'}
    return makeD2XMerged('D2hh',moduleName,decays,dcuts)

def makeD2hhWS(moduleName,dcuts):
    '''Makes D -> pi+pi+,pi-pi-,K+K+,K-K-,K+pi+,K-pi-'''
    decays = {'D2pipiWS':'[D0 -> pi- pi-]cc',
              'D2KpiWS' :'[D0 -> K- pi-]cc',
              'D2KKWS'  :'[D0 -> K- K-]cc'}
    return makeD2XMerged('D2hhWS',moduleName,decays,dcuts)

def makeD2hhhRS(moduleName,dcuts):
    '''Makes D+ -> pi+pi+pi-,K-pi+pi+,K-K+pi+, pi-K+K+,pi-pi+K+ and c.c.s'''
    decays = {'D2pipipi':'[D+ -> pi- pi+ pi+]cc',
              'D2Kpipi' :'[D+ -> K- pi+ pi+]cc',
              'D2KKpi'  :'[D+ -> K- K+ pi+]cc',
              'D2piKK'  :'[D+ -> pi- K+ K+]cc',
              'D2pipiK' :'[D+ -> pi- pi+ K+]cc'}
    return makeD2XMerged('D2hhh',moduleName,decays,dcuts)

def makeD2hhhWS(moduleName,dcuts):
    '''Makes D+ -> pi+pi+pi+,K+pi+pi+,K+K+pi+ and c.c.s'''
    decays = {'D2pipipiWS':'[D+ -> pi+ pi+ pi+]cc',
              'D2KpipiWS' :'[D+ -> K+ pi+ pi+]cc',
              'D2KKpiWS'  :'[D+ -> K+ K+ pi+]cc'}
    return makeD2XMerged('D2hhhWS',moduleName,decays,dcuts)

def makeB2DhSel(name, decay, dsel, bcuts, bchcuts):
    '''Does all B -> Dh selections.'''
    dec = decay[0]
    where = dec.find('B')
    if where < 0: print 'ERROR! No B in descriptor "%s"' % dec
    which = dec[where:where+2]
    bach = None
    if dec.find('pi') > 0:
        where = dec.find('pi')
        bach = dec[where:where+3]
    elif dec.find('K') > 0:
        where = dec.find('K')
        bach = dec[where:where+2]
    else: print 'ERROR! No pi or K found in descriptor "%s"' % dec

    input = [dsel]
    if bach.find('K') >= 0: input.append(StdNoPIDsKaons)
    elif bach.find('pi') >= 0: input.append(StdNoPIDsPions)

    bachCut = "((TRCHI2DOF<%(TrkChi2Max)s) & " \
              "(PT > %(PTMin)s*MeV) & (P > %(PMin)s*MeV) & " \
              "(MIPCHI2DV(PRIMARY) > %(IPChi2Min)s))" % bchcuts
    bachCuts = {bach:bachCut}
    momCut = "(SUMTREE(PT,((ABSID=='K+')|(ABSID=='pi+')),0.0) > %s*MeV)" \
               % bcuts['PTSumMin']
    comboCut = dmassStr('ADAMASS',which,int(bcuts['BMassWindow']))
    momCut += " & ((VFASPF(VCHI2/VDOF)<%(VChi2Max)s)  & " \
              "(BPVIPCHI2() < %(IPChi2Max)s) & (BPVLTIME()>%(TauMin)s*ps)  & "\
              "(BPVDIRA > %(DIRAMin)s))" % bcuts

    b2dh = CombineParticles(DecayDescriptors=decay,DaughtersCuts=bachCuts,
                            CombinationCut=comboCut,MotherCut=momCut)
    return Selection(name,Algorithm=b2dh,RequiredSelections=input)

def makeTISTOSFilter(name,specs):
    from Configurables import TisTosParticleTagger
    tisTosFilter = TisTosParticleTagger(name+'TISTOSFilter')
    tisTosFilter.TisTosSpecs = specs
    tisTosFilter.ProjectTracksToCalo = False
    tisTosFilter.CaloClustForCharged = False
    tisTosFilter.CaloClustForNeutral = False
    tisTosFilter.TOSFrac = {4:0.0, 5:0.0}
    return tisTosFilter

def makeB2DhTOS(name, sel):
    ''' TOS filters B->Dh selections'''
    tisTosFilter = makeTISTOSFilter(name,{'Hlt2Topo.*Decision%TOS':0})
    return Selection(name, Algorithm=tisTosFilter, RequiredSelections=[sel])

def makeB2DhTIS(name, sel):
    ''' TIS filters B->Dh selections'''
    tisTosFilter = makeTISTOSFilter(name,{'Hlt2Topo.*Decision%TIS':0})
                                    #{'Hlt1Global%TIS':0,'Hlt2Global%TIS':0})
    return Selection(name, Algorithm=tisTosFilter, RequiredSelections=[sel])

def makeB2DhMerged(name,moduleName,decays,dname,dsels,bcuts,bchcuts):
    '''Merges B->Dh selections.'''
    tos_sels = []
    tis_sels = []
    for tag, decay in decays.iteritems():
        sname = tag+'With'+dname+'For'+moduleName
        sel = makeB2DhSel(sname+'Sel',decay,dsels[tag],bcuts,bchcuts)
        tos_sels.append(makeB2DhTOS('TOS'+sname,sel))
        tis_sels.append(makeB2DhTIS('TIS'+sname,sel))
    #sname = name+'With'+dname#+'For'+moduleName
    sname = moduleName+'With'+name+dname # change requested by Anton in line names
    tos = MergedSelection(sname+'TOS',RequiredSelections=tos_sels)
    tis = MergedSelection(sname+'TIS',RequiredSelections=tis_sels)
    return (tos,tis)

def makeB02DhRS(moduleName,dname,dsel,bcuts,bchcuts):
    '''Makes all B0 -> D+h (h=pi-,K-) + c.c.'''
    decays = {'B02DPi': ["[B0 -> D- pi+]cc"],
              'B02DK' : ["[B0 -> D- K+]cc"]}
    dsels = {'B02DPi':dsel,'B02DK':dsel}
    return makeB2DhMerged('B02Dh',moduleName,decays,dname,dsels,bcuts,bchcuts)

def makeB02DhWS(moduleName,dname,dsel,dwssel,bcuts,bchcuts):
    ''' Makes all B0 -> D+h wrong sign combos + c.c.'''
    decays = {'B02DPiWS'   : ["[B0 -> D- pi-]cc"],
              'B02DWSPi'   : ["[B0 -> D- pi+]cc"],
              'B02DWSPiWS' : ["[B0 -> D- pi-]cc"],
              'B02DKWS'    : ["[B0 -> D- K-]cc"],
              'B02DWSK'    : ["[B0 -> D- K+]cc"],
              'B02DWSKWS'  : ["[B0 -> D- K-]cc"]}
    dsels = {'B02DPiWS':dsel, 'B02DWSPi':dwssel, 'B02DWSPiWS':dwssel,
             'B02DKWS':dsel, 'B02DWSK':dwssel, 'B02DWSKWS':dwssel}
    return makeB2DhMerged('B02DhWS',moduleName,decays,dname,dsels,bcuts,
                          bchcuts)

def makeB2D0hRS(moduleName,dname,dsel,bcuts,bchcuts):
    '''Makes all B+ -> D0h+ (h=pi,K) + c.c.'''
    decays = {'B2DPi': ["[B+ -> D~0 pi+]cc","[B+ -> D0 pi+]cc"],
              'B2DK' : ["[B+ -> D~0 K+]cc","[B+ -> D0 K+]cc"]}
    dsels = {'B2DPi':dsel,'B2DK':dsel}
    return makeB2DhMerged('B2Dh',moduleName,decays,dname,dsels,bcuts,bchcuts)
