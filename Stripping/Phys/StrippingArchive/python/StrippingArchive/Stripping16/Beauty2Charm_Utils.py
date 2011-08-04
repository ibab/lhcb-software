#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import DataOnDemand, Selection, MergedSelection
from Beauty2Charm_LoKiCuts import LoKiCuts

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class ProtoLine(object):
    '''Stores selections, prescales, etc. that will become lines.'''
    def __init__(self,sels,pre=1.0):
        self.selections = sels
        self.pre = pre

    def prescale(self,line,config):
        if not line.name() in config['Prescales'].keys(): return self.pre
        else: return config['Prescales'][line.name()]

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def dataOnDemand(loc): return DataOnDemand(Location="Phys/%s/Particles"%loc)

def filterSelection(name,code,inputs):
    return Selection(name+'Beauty2CharmFilter',
                     Algorithm=FilterDesktop(Code=code),
                     RequiredSelections=inputs)

def filterInputs(tag,inputs,config):
    '''Filter input particles.'''
    code = LoKiCuts(['TRCHI2DOF','PT','P','MIPCHI2DV','MM','CHILDCL1',
                     'CHILDCL2','PIDK','PIDp','BPVVDRHO','BPVVDZ','DOCAMAX'],
                    config).code()
    return filterSelection(tag+'Inputs',code,inputs)

def filterPi0s(which,inputs,config):
    cuts = ['PT','P']
    if which is 'Resolved':
        cuts.append('CHILDCL1')
        cuts.append('CHILDCL2')
    code = LoKiCuts.combine(["ABSID==111",LoKiCuts(cuts,config).code()])
    return filterSelection('Pi0'+which+'Inputs',code,inputs)

def topoInputsCuts(): # Don't need IP chi2 cut b/c is in 1st filter
    return "(TRCHI2DOF<3) & (PT > 500*MeV) & (P > 5000*MeV)"

def topoKSInputsCuts(): # Don't need IP chi2 cut b/c is in 1st filter
    return "(PT > 500*MeV) & (P > 5000*MeV) & (BPVVDCHI2 > 1000)"

def topoInputs(tag,inputs):
    '''Selects tracks that could have been used by the Topo.'''
    return filterSelection(tag+'TopoInputs',topoInputsCuts(),inputs)

def hasTopoChild():
    return "AHASCHILD((ISBASIC & "+topoInputsCuts()+")|((ABSID=='KS0') & "\
           +topoKSInputsCuts() +"))"

def hasTopoChildren():
    return "NINTREE((ISBASIC & "+topoInputsCuts()+")|((ABSID=='KS0') & "\
           +topoKSInputsCuts() +")) > 1"

def has1TrackChild():
    return "INTREE(ISBASIC & (P>10000*MeV) & (PT>1700*MeV) & (TRCHI2DOF<2.5) "\
           "& (MIPCHI2DV(PRIMARY)>16) & (MIPDV(PRIMARY)>0.1*mm))"

def makeTISTOSFilter(name,tistos):
    specs = {'Hlt2Topo(2|3|4)Body.*Decision%'+tistos:0,
             'Hlt2IncPhi.*Decision%'+tistos:0}
    from Configurables import TisTosParticleTagger
    tisTosFilter = TisTosParticleTagger(name+'TISTOSFilter')
    tisTosFilter.TisTosSpecs = specs
    tisTosFilter.ProjectTracksToCalo = False
    tisTosFilter.CaloClustForCharged = False
    tisTosFilter.CaloClustForNeutral = False
    tisTosFilter.TOSFrac = {4:0.0, 5:0.0}
    return tisTosFilter

def tisTosSelection(sel, tistos):
    '''Filters Selection sel to be tistos (TOS or TIS).'''
    tisTosFilter = makeTISTOSFilter(sel.name()+tistos+'Filter',tistos)
    return Selection(sel.name()+tistos+'Sel', Algorithm=tisTosFilter,
                     RequiredSelections=[sel])

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def makeB2X(name,decay,inputs,config):
    '''Makes all B -> X selections.'''
    comboCuts = LoKiCuts(['SUMPT','AM','AMAXDOCA'],config).code()
    momCuts = [LoKiCuts(['VCHI2DOF'],config).code(),has1TrackChild(),
               hasTopoChildren(),
               LoKiCuts(['BPVIPCHI2','BPVLTIME','BPVDIRA'],config).code()]
    momCuts = LoKiCuts.combine(momCuts)
    b2x = CombineParticles(DecayDescriptors=decay,CombinationCut=comboCuts,
                           MotherCut=momCuts)
    return Selection(name,Algorithm=b2x,RequiredSelections=inputs)

def makeB2XMerged(name,decays,xtag,inputs,config):
    '''Merges B->X selections.  Returns the TIS and TOS selections.  The name
    is misleading as the selections are not actually merged.  They used to be;
    however, this was slow so now they are returned as lists.
    '''
    tosSels = []
    tisSels = []
    for tag, decay in decays.iteritems():
        sname = tag+xtag+'Beauty2Charm'
        if len(decays) > 1: sname += 'Sel'
        sel = makeB2X(sname,decay,inputs[tag],config)
        tosSels.append(tisTosSelection(sel,'TOS'))
        tisSels.append(tisTosSelection(sel,'TIS'))
    sname = name+xtag+'Beauty2Charm'
    return {'TOS':tosSels,'TIS':tisSels}
    #if len(decays) == 1: return {'TOS':tosSels[0],'TIS':tisSels[0]}
    #tos = MergedSelection(sname+'TOS',RequiredSelections=tosSels)
    #tis = MergedSelection(sname+'TIS',RequiredSelections=tisSels)
    #return {'TOS':tos,'TIS':tis}

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
