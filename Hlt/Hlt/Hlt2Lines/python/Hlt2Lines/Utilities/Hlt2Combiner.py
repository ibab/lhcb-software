from copy import deepcopy
from Hlt2TisTosFilter import Hlt2TisTosStage
from Configurables import CombineParticles

class Hlt2Combiner(Hlt2TisTosStage):
    def __counter(n):
        m = 3
        while m <= n:
            yield ''.join(str(i) for i in range(1, m))
            m += 1

    __cutTypes = set(('MotherCut', 'DaughtersCuts', 'CombinationCut') +
                     tuple('Combination%sCut' % s for s in __counter(8)))

    def __init__(self, name, decay, inputs, dependencies = [], tistos = [],
                 combiner = CombineParticles, nickname = None, shared = False, **kwargs):
        self.__decay = decay
        self.__cache = {}

        ## Allow cuts to specified as kwargs
        cuts = {}
        for ct in self.__cutTypes:
            if ct in kwargs: cuts[ct] = kwargs.pop(ct)
        self.__cuts = cuts
        self.__kwargs = kwargs

        ## Support NXBodyDecays
        self.__combiner = combiner
        super(Hlt2Combiner, self).__init__(name, inputs, dependencies,
                                           tistos, nickname, shared)

    def clone(self, name, **kwargs):
        args = deepcopy(self.__kwargs)
        args['name'] = name
        cuts = deepcopy(self.__cuts)
        for ct in self.__cutTypes:
            if ct in kwargs: cuts[ct] = kwargs.pop(ct)
        args.update(cuts)
        for arg, default in (('decay',  self.__decay),
                             ('inputs', self._inputs()),
                             ('tistos', self._tistos()),
                             ('dependencies', self._deps()),
                             ('combiner', self.__combiner),
                             ('nickname', self._nickname())):
            args[arg] = kwargs.pop(arg, default)
        args.update(kwargs)
        
        return Hlt2Combiner(**args)

    def _makeMember(self, stages, cuts, args):
        ## Return what we create directly and add what we depend on to stages.
        from HltLine.HltLine import Hlt2Member
        decays = [self.__decay] if type(self.__decay) == str else self.__decay            
        inputs = self.inputStages(stages, cuts)
        return Hlt2Member(self.__combiner, self._name() + 'Combiner', shared = self._shared(),
                          DecayDescriptors = decays, Inputs = inputs, **args)
                                     
    def stage(self, stages, cuts):
        key = self._hashCuts(cuts)
        if key in self.__cache:
            cached = self.__cache[key]
            stages += cached[1]
            return cached[0]
        
        ## Copy args and substitute cut values
        args = deepcopy(self.__kwargs)
        localCuts = self._localCuts(cuts)
        for k, v in self.__cuts.iteritems():
            if k == 'DaughtersCuts':
                args[k] = {l : w % localCuts for l, w in v.iteritems()}
            else:
                args[k] = v % localCuts
    
        if 'Preambulo' in args:
            args['Preambulo'] = [p % localCuts for p in args['Preambulo']]
                
        if not self._tistos():
            ## Return combiner if no tistos is required
            deps = self.dependencies(cuts)
            member = self._makeMember(deps, cuts, args)
            stages += deps
            self.__cache[key] = (member, deps)
            return member

        ## define the callback used to insert the right extra cut.
        def __combCutHandler(tagger, specs, args):
            tisTosCut = tagger.combinationCut(specs.values())
            if 'CombinationCut' in args:
                args['CombinationCut'] = '({0} & ({1}))'.format(tisTosCut, args['CombinationCut'])
            else:
                args['CombinationCut'] = tisTosCut

        ## HandleTisTos adds inputs to stages, we have to add dependencies and
        ## return our own stage.
        deps = self.dependencies(cuts)
        stage = self._handleTisTos(deps, cuts, args, __combCutHandler)
        stages += deps
        self.__cache[key] = (stage, deps)
        return stage
