from copy import deepcopy
from Hlt2Stage import Hlt2Stage
from Hlt2TisTosFilter import Hlt2TisTosStage

class Hlt2ParticleFilter(Hlt2TisTosStage):
    def __init__(self, name, code, inputs, dependencies = [], tistos = [],
                 nickname = None, shared = False, **kwargs):
        self.__code = code
        self.__kwargs = kwargs
        self.__cache = {}
        super(Hlt2ParticleFilter, self).__init__(name, inputs, dependencies,
                                                 tistos, nickname, shared)

    def hasOutput(self):
        return True

    def clone(self, name, **kwargs):
        args = deepcopy(self.__kwargs)
        args['name'] = name
        args['shared'] = self._shared()
        for arg, default in (('code',   self.__code),
                             ('inputs', self._inputs()),
                             ('tistos', self._tistos()),
                             ('dependencies', self._deps()),
                             ('nickname', self._nickname())):
            args[arg] = kwargs.pop(arg) if arg in kwargs else default
        args.update(kwargs)

        return Hlt2ParticleFilter(**args)

    def _makeMember(self, stages, cuts, args):
        from HltLine.HltLine import Hlt2Member
        from Configurables import FilterDesktop
        inputs = self.inputStages(stages, cuts)
        stages += inputs
        return Hlt2Member(FilterDesktop, self._name() + 'Filter', shared = self._shared(),
                          Inputs = inputs, **args)

    def stage(self, stages, cuts):
        key = self._hashCuts(cuts)
        if key in self.__cache:
            cached = self.__cache[key]
            stages += cached[1]
            return cached[0]

        localCuts = self._localCuts(cuts)
        args = deepcopy(self.__kwargs)
        args['Code'] = self.__code % localCuts
        if 'Preambulo' in args:
            args['Preambulo'] = [p % localCuts for p in args['Preambulo']]

        if not self._tistos():
            ## Return filter if no tistos is required
            deps = self.dependencies(cuts)
            stage = self._makeMember(deps, cuts, args)
            stages += deps
            self.__cache[key] = (stage, deps)
            return stage

        ## define the callback used to insert the right extra cut.
        def __partCutHandler(tagger, specs, args):
            tisTosCut = tagger.particleCut(specs.values())
            args['Code'] = '({0} & ({1}))'.format(tisTosCut, args['Code'])

        ## HandleTisTos adds inputs to stages, we have to add dependencies and
        ## return our own stage.
        deps = self.dependencies(cuts)
        stage = self._handleTisTos(deps, cuts, args, __partCutHandler)
        stages += deps
        self.__cache[key] = (stage, deps)
        return stage

class Hlt2VoidFilter(Hlt2Stage):
    def __init__(self, name, code, inputs, dependencies = [], nickname = None,
                 shared = False, **kwargs):
        self.__code = code
        self.__kwargs = kwargs
        self.__cache = {}
        super(Hlt2VoidFilter, self).__init__(name, inputs, dependencies, nickname, shared)

    def clone(self, name, **kwargs):
        args = deepcopy(self.__kwargs)
        args['name'] = name
        args['shared'] = self._shared()
        for arg, default in (('code',   self.__code),
                             ('inputs', self._inputs()),
                             ('dependencies', self._deps()),
                             ('nickname', self._nickname())):
            args[arg] = kwargs.pop(arg) if arg in kwargs else default
        args.update(kwargs)
        return Hlt2VoidFilter(**args)

    def stage(self, stages, cuts):
        key = self._hashCuts(cuts)
        if key in self.__cache:
            cached = self.__cache[key]
            stages += cached[1]
            return cached[0]

        from Configurables import LoKi__VoidFilter as VoidFilter
        cuts = self._localCuts(cuts)
        code = self.__code % cuts

        ## Also use Hlt2Member here to make sure that only the shared ones are
        ## actually shared.
        from HltLine.HltLine import Hlt2Member
        vfilter = Hlt2Member(VoidFilter, self._name(), shared = self._shared(), Code = code)

        ## Add inputs and dependencies to stages and return our own stage.
        deps =self.dependencies(cuts)
        inputs = self.inputStages(deps, cuts)
        stages += deps
        self.__cache[key] = (vfilter, deps)
        return vfilter
