from copy import deepcopy
from Hlt2Stage import Hlt2Stage
from Hlt2MergedStage import Hlt2MergedStage
from HltLine.HltLine import Hlt2SubSequence

class Hlt2JetBuilder(Hlt2Stage):
    def __init__(self, name, tags, nickname = None,
                 shared = False, **kwargs):
        self.__tags = tags
        self.__kwargs = kwargs
        self.__cache = {}

        ## particle flow
        from Configurables import HltJetConf
        inputs = [HltJetConf().particleFlow()]

        super(Hlt2JetBuilder, self).__init__(name, inputs = inputs, nickname = nickname, shared = shared)

    def clone(self, name, **kwargs):
        args = deepcopy(self.__kwargs)
        args['name'] = name
        args['shared'] = self._shared()
        for arg, default in (('tags', self.__tags),
                             ('inputs', self._inputs()),
                             ('nickname', self._nickname())):
            args[arg] = kwargs.pop(arg) if arg in kwargs else default
        args.update(kwargs)
        return Hlt2JetBuilder(**args)

    def stage(self, stages, cuts):
        key = self._hashCuts(cuts)
        if key in self.__cache:
            cached = self.__cache[key]
            stages += cached[1]
            return cached[0]

        ## Add inputs and dependencies to stages and return our own stage.
        deps = self.dependencies(cuts)

        ## The tags are optional, so we make a subsequence for them, and then
        ## plug their dependencies and the tag stages themselves in another
        ## subsequence that has IgnoreFilterPassed set to True
        ## FIXME: Turn this into an Hlt2OptionalStage class for use elsewhere
        tagDeps = []
        tagStage = Hlt2MergedStage(self._name() + 'InputTags', self.__tags,
                                   nickname = self._nickname(), shared = self._shared()).stage(tagDeps, cuts)
        tagStage = Hlt2SubSequence(self._name() + 'InputTagsSeq', tagDeps + [tagStage],
                                   shared = self._shared(), IgnoreFilterPassed = True)
        deps += [tagStage]

        # Get the outputs to pass to the InputTags property
        from itertools import chain
        tagOutputs = [c for c in chain.from_iterable([t.outputStages([], cuts) for t in self.__tags])]

        ## Make the jet builder
        from HltLine.HltLine import Hlt2Member
        from Configurables import HltJetBuilder
        cuts = self._localCuts(cuts)
        props = {k : v for k, v in cuts.iteritems() if hasattr(HltJetBuilder, k)}
        inputs = self.inputStages(deps, cuts)
        builder = Hlt2Member(HltJetBuilder, self._name(), Inputs = inputs, shared = self._shared(),
                             InputTags = tagOutputs, **props)

        ## Add inputs and dependencies to stages and return our own stage.
        stages += deps
        self.__cache[key] = (builder, deps)
        return builder
