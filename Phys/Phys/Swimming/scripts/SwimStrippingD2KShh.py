from Configurables import Swimming
from Gaudi.Configuration import *

hhes = ['PP', 'KK']
lldd = ['LL', 'DD']
selconj = ['', 'SelConjugate']
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
Swimming().OffCands = dict(("/Event/CharmToBeSwum/Phys/{selconj}D2KS{hh}{ll}ForDstarD2KShh".format(ll=ll,selconj=sc,hh=hh), dstarloc.format(ll=ll,hh=hh)) for ll in lldd for sc in selconj for hh in hhes)
Swimming().StripCands = dict((l, l.replace(Swimming().getProp('StrippingStream') + '/','')) for l in Swimming().OffCands.keys())
Swimming().MuDSTCands = [dstarloc.format(ll=ll,hh=hh) for ll in lldd for hh in hhes]
Swimming().StrippingLines = [l.split('/')[-1] for l in Swimming().getProp('MuDSTCands')]
Swimming().SkipEventIfNoMuDSTCandFound = True
Swimming().SkipEventIfNoMuDSTCandsAnywhere = False
Swimming().SelectMethod = 'all'
Swimming().OutputType = 'MDST'
Swimming().UseFileStager = False

# The custom event loop
from Gaudi.Configuration import setCustomMainLoop
from Swimming.EventLoop import SwimmingEventLoop
setCustomMainLoop(SwimmingEventLoop)
