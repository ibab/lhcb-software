from copy import deepcopy
from Hlt2Stage import Hlt2Stage
class Hlt2TisTosTagger(Hlt2Stage):
    def __init__(self, prefix, name, specs, inputs, **kwargs):
        self.__kwargs = kwargs
        self.__specs  = specs
        self.__prefix = prefix
        Hlt2Stage.__init__(self, name, inputs)

    def clone(self, name, prefix = None, specs = None, inputs = None, **kwargs):
        args = deepcopy(self.__kwargs)
        args.update(kwargs)
        return Hlt2TisTosTagger(prefix if prefix else self.__prefix, name,
                                specs  if specs  else self.__specs,
                                inputs if inputs else self._inputs(),
                                **args)

    def stage(self, cuts):
        from HltLine.HltLine import Hlt2Member
        from Configurables import TisTosParticleTagger
        return Hlt2Member(TisTosParticleTagger, self.__prefix + self._name() + 'TisTosTagger', 
                          TisTosSpecs = self.__specs,
                          Inputs = self.inputStages(cuts), **self.__kwargs)        
    
class Hlt2Hlt1TrackAllL0TosTagger(Hlt2Stage):
    def __init__(self):
        self.__infoKey = 42 ** 2
        ## Fix extraInfo entry number
        self.__specs  = {"Hlt1Track.*Decision%TOS" : self.__infoKey}
        from HltLine.HltDecodeRaw import DecodeHlt1SelRep
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking as Hlt2LongTracking
        tracks = Hlt2LongTracking().getProp('__hlt2PrepareTracksSeq__')
        Hlt2Stage.__init__(self, 'Hlt2Hlt1TrackAllL0TosTagger', [tracks], [DecodeHlt1SelRep])

    def infoKey(self):
        return self.__infoKey
        
    def infoKeys(self):
        return (self.__infoKey,)
        
    def clone(self, name, **kwargs):
        raise RuntimeError('Hlt2Hlt1TrackAllL0TosTagger.clone is not implemented.')

    def motherCut(self):
        return "(MAXTREE(PALL, TINFO(%d, -1)) > 0) " % self.infoKey()

    def combinationCut(self):
        return "(AMAXCHILD(MAXTREE(PALL, TINFO(%d, -1))) > 0) " % self.infoKey()
    
    def stage(self, cuts):
        from HltLine.HltLine import bindMembers
        from Configurables import TisTosTrackTagger
        tagger = TisTosTrackTagger(self._name(), TisTosSpecs = self.__specs, PassOnAll = True,
                                   Inputs = [self._inputs()[-1].outputSelection()])
        return bindMembers(None, self.inputStages(cuts) + [tagger])

