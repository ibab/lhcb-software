import collections, re
from copy import deepcopy
from Hlt2Stage import Hlt2Stage
from Utilities import makeList, splitSpecs

__base = 42 ** 2
_globalSpecs = dict((sel, __base + i) for i, sel in enumerate(("Hlt1Track.*Decision%TOS", "Hlt1TrackAllL0Decision%TOS",
                                                                "Hlt1TrackMuonDecision%TOS", "Hlt1TrackPhotonDecision%TOS")))

class Hlt2TisTosGlobalTagger(Hlt2Stage):
    def __init__(self, specsOrKeys):
        ## Fix extraInfo entry number
        specsOrKeys = makeList(specsOrKeys)
        self.__specs = set()
        self.__keys = set()
        re_spec = re.compile('.*%T[OUI]S$')
        for sk in specsOrKeys:
            if re_spec.match(sk):
                self.__specs.add(sk)
            else:
                self.__keys.add(sk)
        if bool(self.__keys) == bool(self.__specs):
            raise AttributeError("Either only specs or only cut keys must be specified")
        if self.__specs and not all(map(lambda s: s in _globalSpecs, self.__specs)):
            raise AttributeError("All specs must be globally taggable.")
        self.__cache = None

        from HltLine.HltDecodeRaw import DecodeHlt1SelRep
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking as Hlt2LongTracking
        tracks = Hlt2LongTracking().hlt2PrepareTracks()
        super(Hlt2TisTosGlobalTagger, self).__init__('Hlt2TisTosGlobalTagger', [tracks],
                                                     dependencies = [DecodeHlt1SelRep], shared = True)
        
    def __checkSpecs(self, specs):
        bad = filter(lambda x: x not in _globalSpecs, specs)
        if bad:
            raise KeyError('WARNING: Hlt2TisTosGlobalTagger: unknown specs %s requested.' % ' '.join(bad))
        return specs

    def particleCut(self, keys):
        keys = makeList(keys)
        return ' & '.join(('(MAXTREE(HASTRACK, TINFO({0}, -1)) > 0.5)'.format(k) for k in keys))

    def combinationCut(self, keys):
        keys = makeList(keys)
        return ' & '.join(("(AMAXCHILD(MAXTREE(HASTRACK, TINFO({0}, -1))) > 0.5)".format(k) for k in keys))

    def infoKeys(self, specs):
        specs = makeList(specs)
        self.__checkSpecs(specs)
        return [self.__specs[k] for k in specs]
        
    def clone(self, name, **kwargs):
        raise RuntimeError('Hlt2TisTosGlobalTagger.clone is not implemented.')

    def stage(self, stages, cuts):
        if self.__cache:
            stages += self.__cache[1]
            self.__cache[0]

        ## Get the actual specs from the cuts if needed and create a dictionary
        ## to pass to the tagger.
        if self.__keys:
            specs = {}
            for k in self.__keys:
                tmp = cuts[k]
                if isinstance(tmp, basestring):
                    if tmp not in _globalSpecs:
                        raise AttributeError("All specs must be globally taggable.")
                    specs[tmp] = _globalSpecs[tmp]
                else:
                    if not all(map(lambda s: s in _globalSpecs, tmp)):
                        raise AttributeError("All specs must be globally taggable.")
                    for s in tmp:
                        specs[s] = _globalSpecs[s]
        else:
            specs = dict((s, _globalSpecs[s]) for s in self.__specs)
        ## Build the tagger
        from Configurables import TisTosTrackTagger
        deps = self.dependencies(cuts)
        inputs = self.inputStages(deps, cuts)
        stage = TisTosTrackTagger('Hlt1TrackTagger', TisTosSpecs = specs, PassOnAll = True,
                                  Inputs = [i.outputSelection() for i in inputs],
                                  ProjectTracksToCalo = False)
        stages += deps
        self.__cache = (stage, deps)
        return stage

class Hlt2TisTosParticleTagger(Hlt2Stage):
    def __init__(self, name, tistos, inputs, nickname = None, shared = False, **kwargs):
        specsOrKeys = makeList(tistos)
        self.__specs = set()
        self.__keys = set()
        re_spec = re.compile('.*%T[OUI]S$')
        for sk in specsOrKeys:
            if re_spec.match(sk):
                self.__specs.add(sk)
            else:
                self.__keys.add(sk)
        if bool(self.__keys) == bool(self.__specs):
            raise AttributeError("Either only specs or only cut keys must be specified")
                            
        self.__kwargs = kwargs
        self.__cache = {}
        from HltLine.HltDecodeRaw import (DecodeL0MUON, DecodeL0CALO,
                                          DecodeL0FullCALO, DecodeL0DU)
        from Configurables import L0DecReportsMaker, L0SelReportsMaker
        from HltLine.HltLine import bindMembers
        args = dict(nickname = nickname, shared = shared,
                    dependencies = [DecodeL0DU, DecodeL0MUON, DecodeL0CALO,
                                    DecodeL0FullCALO, L0DecReportsMaker(),
                                    L0SelReportsMaker()])
        super(Hlt2TisTosParticleTagger, self).__init__(name, inputs, **args)

    def clone(self, name, **kwargs):
        args = deepcopy(self.__kwargs)
        args['name'] = name
        args['shared'] = self._shared()
        for arg, default in (('tistos', self.__code),
                             ('inputs', self._inputs()),
                             ('nickname', self._nickname())):
            args[arg] = kwargs.pop(arg) if arg in kwargs else default
        args.update(kwargs)

        return Hlt2TisTosTagger(**args)

    def __globalTagger(self, stages, specs, cuts):
        tagger = Hlt2TisTosGlobalTagger(specs)
        code = tagger.particleCut(specs.values())
        from Hlt2Filter import Hlt2ParticleFilter
        return Hlt2ParticleFilter('Global' + self._name(), code,
                                  self._inputs(), [tagger], shared = self._shared(),
                                  nickname = self._nickname(), **self.__kwargs).stage(stages, cuts)
    
    def __localTagger(self, stages, specs, cuts, inputs = None):
        from HltLine.HltLine import Hlt2Member
        from Configurables import TisTosParticleTagger
        if inputs == None:
            inputs = self.inputStages(stages, cuts)
        else:
            from Utilities import makeStages
            inputs = makeStages([], inputs, cuts)
            stages += inputs
        stage = Hlt2Member(TisTosParticleTagger, self._name() + 'TisTosTagger', shared = self._shared(),
                           TisTosSpecs = specs, Inputs = inputs, **self.__kwargs)
        return stage

    def stage(self, stages, cuts):
        key = self._hashCuts(cuts)
        if key in self.__cache:
            cached = self.__cache[key]
            stages += cached[1]
            return cached[0]

        localCuts = self._localCuts(cuts)
        if self.__keys:
            specs = []
            for k in self.__keys:
                tmp = localCuts[k]
                if isinstance(tmp, basestring):
                    specs.append(tmp)
                else:
                    specs.extend(list(tmp))
        else:
            specs = self.__specs

        localSpecs, globalSpecs = splitSpecs(specs, _globalSpecs)

        deps = self.dependencies(cuts)
        if localSpecs and globalSpecs:
            globalTagger = self.__globalTagger(deps, globalSpecs, cuts)
            stage = self.__localTagger(deps, localSpecs, cuts, inputs = [globalTagger])
        elif localSpecs:
            stage = self.__localTagger(deps, localSpecs, cuts)
        elif globalSpecs:
            stage = self.__globalTagger(deps, globalSpecs, cuts)
        self.__cache[key] = (stage, deps)
        stages += deps
        return stage

class Hlt2TisTosStage(Hlt2Stage):
    def __init__(self, name, inputs, dependencies = [], tistos = [],
                 nickname = None, shared = False):
        self.__tistos = makeList(tistos)
        super(Hlt2TisTosStage, self).__init__(name, inputs, dependencies, nickname, shared)
    
    def _tistos(self):
        return self.__tistos
        
    def _handleTisTos(self, stages, cuts, args, handler):
        ## Make a list of specs, substituting cuts if needed; the value
        ## corresponding to a cut can be a list.
        tistos = []
        localCuts = self._localCuts(cuts)
        for tt in self.__tistos:
            tmp = localCuts.get(tt, tt)
            if isinstance(tmp, basestring):
                tistos.append(tmp)
            else:
                tistos.extend(list(tmp))
        ## Split the specs in local and global
        localSpecs, globalSpecs = splitSpecs(tistos, _globalSpecs)
        deps = []
        if globalSpecs:
            ## Handle any globally taggable specs first and add the required
            ## combination cut to our combination cut.
            globalTagger = Hlt2TisTosGlobalTagger(globalSpecs.keys())
            handler(globalTagger, globalSpecs, args)
            deps += [globalTagger.stage(deps, cuts)]

        ## Make the combiner
        stage = self._makeMember(deps, cuts, args)
            
        if localSpecs:
            ## There are also local specs, so a TisTosParticleTagger is
            ## needed, that takes the output of the member as input

            ## We already have everything that the member depends on, only add
            ## it here as dependency itself.
            deps += [stage]
            tmp = []
            stage = Hlt2TisTosParticleTagger(self._name(), localSpecs, [stage],
                                             self._nickname(), self._shared()).stage(tmp, cuts)

        stages += deps
        return stage

