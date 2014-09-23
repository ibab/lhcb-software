from Configurables import Swimming
from Gaudi.Configuration import *

hhes = ['PP', 'KK']
lldd = ['LL', 'DD']
selconj = ['', 'SelConjugate']
d0loc = "/Event/CharmToBeSwum/Phys/{selconj}D2KS{hh}{ll}ForDstarD2KShh"
dstarloc = '/Event/CharmToBeSwum/Phys/DstarD2KShh{hh}{ll}Line'
Swimming().Debug = False
Swimming().DataType = '2012'
Swimming().EvtMax = -1
Swimming().Simulation = False
Swimming().Persistency = 'ROOT'
Swimming().InputType = 'DST'
Swimming().SwimStripping = True
Swimming().StrippingStream = 'CharmToBeSwum'
Swimming().StrippingVersion = 'Stripping20'
Swimming().StrippingFile = 'DstarD2KShh'
Swimming().StrippingLineGroup = 'DstarD2KShh'
Swimming().OffCands = dict((d0loc.format(ll=ll,selconj=sc,hh=hh), dstarloc.format(ll=ll,hh=hh)) for ll in lldd for sc in selconj for hh in hhes)
Swimming().StripCands = dict((d0loc.format(ll=ll,hh=hh,selconj=sc), [d0loc.format(ll=ll,hh=hh,selconj='').replace(Swimming().getProp('StrippingStream') + '/', '')]) for ll in lldd for sc in selconj for hh in hhes)
Swimming().MuDSTCands = [dstarloc.format(ll=ll,hh=hh) for ll in lldd for hh in hhes]
Swimming().StrippingLines = [l.split('/')[-1] for l in Swimming().getProp('MuDSTCands')]
Swimming().SkipEventIfNoMuDSTCandFound = True
Swimming().SkipEventIfNoMuDSTCandsAnywhere = False
Swimming().SelectMethod = 'all'
Swimming().OutputType = 'MDST'
Swimming().UseFileStager = False
Swimming().LifetimeFitter = ['DecayTreeFitter', 'LifetimeFitter']
Swimming().DecayTreeFitterConstraints = {
    'DTFD0KS0'  : { 'D0' : -1.0, 'KS0' : -1.0 },
    'DTFKS0'    : { 'KS0' : -1.0 }  
    }
Swimming().StoreExtraTPs = True

# The custom event loop
from Gaudi.Configuration import setCustomMainLoop
from Swimming.EventLoop import SwimmingEventLoop
setCustomMainLoop(SwimmingEventLoop)
