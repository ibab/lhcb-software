#!/usr/bin/env python
# =============================================================================
# @file Beauty2XGamma_Utils.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @date 2011-11-17
# =============================================================================
"""Utilities module for the Radiative Stripping.

Imports the Beauty2Charm_Utils and overwrites some of its functionality to adapt it to the Radiative Stripping.
"""

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import DataOnDemand, Selection, MergedSelection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Configurables import OfflineVertexFitter

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class ProtoLine(object):
    '''Stores selections, prescales, etc. that will become lines.'''
    def __init__(self,sels,pre=1.0):
        self.selections = sels
        self.pre = pre

    def prescale(self,line,name,config):
        if not name in config['Prescales'].keys(): return self.pre
        else: return config['Prescales'][name]

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def dataOnDemand(loc): return DataOnDemand(Location="Phys/%s/Particles"%loc)

def filterSelection(name,code,inputs):
    return Selection(name+'Beauty2GammaFilter',
                     Algorithm=FilterDesktop(Code=code),
                     RequiredSelections=inputs)

def filterInputs(tag,inputs,config):
    '''Filter input particles.'''
    code = LoKiCuts(['TRCHI2DOF','PT','P','MIPCHI2DV','MM','CHILDCL1',
                     'CHILDCL2','PIDK','PIDp','BPVVDRHO','BPVVDZ','DOCAMAX','CL','TRGHP'],
                    config).code()
    return filterSelection(tag+'Inputs',code,inputs)

def filterPi0s(which,inputs,config):
    cuts = ['PT','P']
    if which is 'Resolved':
        cuts.append('CHILDCL1')
        cuts.append('CHILDCL2')
    code = LoKiCuts.combine(["ABSID==111",LoKiCuts(cuts,config).code()])
    return filterSelection('Pi0'+which+'Inputs',code,inputs)
    
def filterPhotons(inputs, config):
    cuts = ['PT', 'CL']
    code = LoKiCuts.combine(["ABSID==22",LoKiCuts(cuts,config).code()])
    return filterSelection("Gamma", code, inputs)

#def filterPhotonsConv(inputs, config):
#    ''' New version to make the converted photons'''
#    posElec = LoKiCuts.combine(["ID==11",LoKiCuts(cuts,config).code()])
#    negElec = LoKiCuts.combine(["ID==-11",LoKiCuts(cuts,config).code()])
#    elec = LoKiCuts.combine(["ABSID==11",LoKiCuts(cuts,config).code()])

def filterPhotonsConv(inputs,config):
    ''' Makes converted photons before the magnet '''
    cuts = ['MM','VCHI2DOF']
    code = LoKiCuts(cuts,config).code()
    return filterSelection("GammaConv", code, inputs)
    #comboCuts = LoKiCuts(cuts,config).code()
    #momCuts = LoKiCuts(['VCHI2DOF'],config).code()
    #decay = ['gamma -> e+ e-']
    #comb = CombineParticles(CombinationCut=comboCuts, MotherCut=momCuts, DecayDescriptors=decay)
    #return Selection('GammaConv', Algorithm=comb, RequiredSelections = inputs)
    #code = LoKiCuts.combine(["ABSID==11",LoKiCuts(cuts,config).code()])
    #return filterSelection("GammaConv", code, inputs)

def topoInputsCuts(): # Don't need IP chi2 cut b/c is in 1st filter
    return "(TRCHI2DOF<3) & (PT > 500*MeV) & (P > 5000*MeV)"

def softtopoInputsCuts(): # Don't need IP chi2 cut b/c is in 1st filter
    return "(TRCHI2DOF<3) & (PT > 350*MeV) & (P > 1500*MeV)"

def topoKSInputsCuts(): # Don't need IP chi2 cut b/c is in 1st filter
    return "(PT > 500*MeV) & (P > 5000*MeV) & (BPVVDCHI2 > 1000)"

def topoInputs(tag,inputs):
    '''Selects tracks that could have been used by the Topo.'''
    return filterSelection(tag+'TopoInputs',topoInputsCuts(),inputs)

def softtopoInputs(tag,inputs):
    '''Selects tracks that could have been used by the Topo.'''
    return filterSelection(tag+'SoftTopoInputs',softtopoInputsCuts(),inputs)

def hasTopoChild():
    return "AHASCHILD((ISBASIC & "+topoInputsCuts()+")|((ABSID=='KS0') & "\
    +topoKSInputsCuts() +"))"

def hasTopoChildren():
    return "NINTREE((ISBASIC & "+topoInputsCuts()+")|((ABSID=='KS0') & "\
           +topoKSInputsCuts() +")) > 1"

def has1TrackChild():
    return "INTREE(ISBASIC & (P>5000*MeV) & (PT>1000*MeV) & (TRCHI2DOF<2.5) "\
           "& (MIPCHI2DV(PRIMARY)>16) & (MIPDV(PRIMARY)>0.1*mm))"

def makeTISTOSFilter(name):
    specs = {'Hlt2Topo(2|3|4)Body.*Decision%TOS':0,
             'Hlt2Topo(2|3|4)Body.*Decision%TIS':0,
             'Hlt2IncPhi.*Decision%TOS':0,
             'Hlt2IncPhi.*Decision%TIS':0,
             'Hlt2RadiativeTopo.*Decision%TOS':0,    ## Cut based raditive topological 
             'Hlt2RadiativeTopo.*Decision%TIS':0,    ## Cut based raditive topological
             'Hlt2TopoRad.*Decision%TOS':0,          ## BBDT based radiative topological
             'Hlt2TopoRad.*Decision%TIS':0}          ## BBDT based radiative topological
    from Configurables import TisTosParticleTagger
    tisTosFilter = TisTosParticleTagger(name+'TISTOSFilter')
    tisTosFilter.TisTosSpecs = specs
    #tisTosFilter.ProjectTracksToCalo = False
    #tisTosFilter.CaloClustForCharged = False
    #tisTosFilter.CaloClustForNeutral = False
    #tisTosFilter.TOSFrac = {4:0.0, 5:0.0}
    return tisTosFilter

def tisTosSelection(sel):
    '''Filters Selection sel to be TOS OR TIS.'''
    tisTosFilter = makeTISTOSFilter(sel.name())
    return Selection(sel.name()+'TISTOS', Algorithm=tisTosFilter, RequiredSelections=[sel])


def TOSFilter( name = None, sel = None, trigger = None ):
    """
    Function to return a selection object, filtering for TOS candidates from input selection
    """
    from Configurables import TisTosParticleTagger
    
    _filter = TisTosParticleTagger(name+"_TriggerTos")
    _filter.TisTosSpecs = { trigger+"%TOS" : 0 }
    _filter.NoRegex = True
    
    from PhysSelPython.Wrappers import Selection
    _sel = Selection("Sel" + name + "_TriggerTos", RequiredSelections = sel, Algorithm = _filter )
    return _sel


def filterPID(name,input,config,level=1):
    cuts = ["(NINGENERATION(('p+'==ABSID) & (PIDp < %s),%d) == 0)" \
            % (config['P']['PIDp_MIN'],level),            
            "(NINGENERATION(('K+'==ABSID) & (PIDK < %s), %d) == 0)" \
            % (config['K']['PIDK_MIN'],level),
            "(NINGENERATION(('pi+'==ABSID) & (PIDK > %s), %d) == 0)" \
            % (config['PI']['PIDK_MAX'],level)]
    return filterSelection(name,LoKiCuts.combine(cuts),input)

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
def makeB2X(name, decay, inputs, config, useIP=True, resVert=False, usePi0=False):
    '''Makes all B -> X selections.'''
    from copy import deepcopy
    comboCuts = LoKiCuts(['SUMPT','AM'],config).code()
    flightCuts = ['BPVLTIME']
    if useIP:
        flightCuts += ['BPVIPCHI2','BPVDIRA']
    
    momCuts = [LoKiCuts(['VCHI2DOF'],config).code(),
               #has1TrackChild(),
               has1TrackChild(),
               hasTopoChildren(),
               LoKiCuts(flightCuts,config).code()]
    momCuts = LoKiCuts.combine(momCuts)
    b2x = CombineParticles(DecayDescriptors=decay,
                           CombinationCut=comboCuts,
                           MotherCut=momCuts)
    if usePi0:
        #b2x = b2x.configurable(name+'Beauty2XGammaCombiner')
        #b2x.ParticleCombiners.update({'':'MomentumCombiner'})
        b2x.ParticleCombiners = { '' : 'LoKi::VertexFitter' }
    if resVert:
        b2x = b2x.configurable(name+'Beauty2XGammaVertexFitter')
        b2x.addTool(OfflineVertexFitter)
        b2x.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
        b2x.OfflineVertexFitter.useResonanceVertex = False
    return Selection(name, Algorithm=b2x, RequiredSelections=inputs)

def makeLambda2X(name, decay, inputs, config, hasLambdaPion=False):
    '''Makes all Lambda -> X selections.'''
    from copy import deepcopy
    comboCuts = LoKiCuts(['SUMPT','AM'],config).code()
    #flightCuts = ['BPVLTIME']
    #if useIP:
    #    flightCuts += ['BPVIPCHI2','BPVDIRA']
    #momCuts = [LoKiCuts(['VCHI2DOF'],config).code(),
    momCuts = [ 
               has1TrackChild()
              ]
    momCuts = LoKiCuts.combine(momCuts)
    b2x = CombineParticles(DecayDescriptors=decay,
                           CombinationCut=comboCuts,
                           MotherCut=momCuts)
    if hasLambdaPion:
        b2x.ParticleCombiners = { '' : 'LoKi::VertexFitter' }
    return Selection(name, Algorithm=b2x, RequiredSelections=inputs)


def makeB2XSels(decays, xtag, inputs, config, useIP=True, resVert=True, usePi0=False, hasLambdaPion=False):
    '''Returns a list of the Hb->X selections.'''
    sels = []
    for tag, decay in decays.iteritems():
        #sname = tag + xtag + 'Beauty2XGamma'
        sname = tag + xtag
        ## we only want the std vertexing when there are no neutrals in the resonances 
        #if tag == 'Lb2PHGamma':
        #    sel = makeLambda2X(sname, decay, inputs[tag], config, hasLambdaPion)
        if tag == 'Lb2PPiGamma':
            sel = makeLambda2X(sname, decay, inputs[tag], config, hasLambdaPion=True)
        elif 'Lb2LGamma' in tag:
            sel = makeLambda2X(sname, decay, inputs[tag], config, hasLambdaPion=True)
        elif tag == 'Lb2PKGamma':
            sel = makeLambda2X(sname, decay, inputs[tag], config, hasLambdaPion) 
        #if tag in lambdaTags : 
        #    sel = makeLambda2X(sname, decay, inputs[tag], config, hasLambdaPion)
            #sel = tisTosSelection(sel)
            #sel = filterSelection(sname + '_B2CBBDT',
            #sel = filterSelection(sname, 
            #                  "FILTER('BBDecTreeTool/B2CBBDT')",
            #                  [sel])
            #from Configurables import BBDecTreeTool as BBDT
            #bbdt = BBDT(sel.name()+'.B2CBBDT')
            #bbdt.Threshold = config['BBDT_MIN']
            #bbdt.ParamFile = 'Beauty2Charm_BDTParams_v1r0.txt'
        elif tag == 'B2OmegaGammaHHH': 
            sel = makeB2X(sname, decay, inputs[tag], config, useIP, usePi0)
        else:
            sel = makeB2X(sname, decay, inputs[tag], config, useIP) 
        #sel = tisTosSelection(sel)
        #sel = filterSelection(sname + 'B2CBBDT',
        #                      "FILTER('BBDecTreeTool/B2CBBDT')",
        #                      [sel])
        #from Configurables import BBDecTreeTool as BBDT
        #bbdt = BBDT(sel.name()+'.B2CBBDT')
        #bbdt.Threshold = config['BBDT_MIN']
        #bbdt.ParamFile = 'Beauty2Charm_BDTParams_v1r0.txt'
        
        sels.append(sel)
    return sels

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
