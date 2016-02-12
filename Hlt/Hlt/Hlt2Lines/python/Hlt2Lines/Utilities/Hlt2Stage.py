import collections
from copy import deepcopy
from Utilities import makeList, splitSpecs, makeStages


class Hlt2Stage(object):
    def __init__(self, name, inputs, dependencies = [], nickname = None, shared = False,
                 configurable = None):
        if type(name) != str:
            raise RuntimeError("Hlt2Stage: name must be a string, not a %s." % name)
        self.__name = name
        self.__nickname = name if nickname == None else nickname
        self.__inputs = inputs
        self.__deps = dependencies
        self.__shared = shared
        self.__depStages = {}
        self.__inputStages = {}
        self.__configurable = configurable

        for arg, msg in ((self.__inputs, "inputs"), (self.__deps, "dependencies")):
            if not isinstance(arg, collections.Iterable) or isinstance(arg, basestring):
                raise RuntimeError("Hlt2Stage: %s must be an iterable, not a %s." % (msg, inputs))

    def _hashCuts(self, cuts):
        return hash(str(self._localCuts(cuts)))

    def _configurable(self):
        return self.__configurable

    def _name(self):
        return self.__name

    def _nickname(self):
        return self.__nickname

    def _inputs(self):
        return self.__inputs

    def _addInputs(self, inputs):
        self.__inputs.extend(inputs)

    def _deps(self):
        return self.__deps

    def _addDeps(self, deps):
        self.__deps.extend(deps)

    def _shared(self):
        return self.__shared

    def _localCuts(self, cuts):
        common = cuts.get('Common', {})
        return cuts.get(self._nickname(), common)

    def outputStages(self, stages, cuts):
        outs = self.stage(stages, cuts)
        if type(outs) not in (list, tuple):
            outs = [outs]
        stages += list(outs)
        return outs

    def inputStages(self, stages, cuts):
        key = self._hashCuts(cuts)
        if key in self.__inputStages:
            inputs, deps = self.__inputStages[key]
            stages += deps
            return inputs

        deps = []
        inputs = []
        for i in self._inputs():
            if isinstance(i, Hlt2Stage):
                inputs += i.outputStages(deps, cuts)
            else:
                deps.append(i)
                inputs.append(i)
        self.__inputStages[key] = (inputs, deps)
        stages += deps
        return inputs

    def dependencies(self, cuts):
        key = self._hashCuts(cuts)
        if key not in self.__depStages:
            deps = []
            deps += makeStages(deps, self._deps(), cuts)
            self.__depStages[key] = deps
        return self.__depStages[key]


class Hlt2ExternalStage(Hlt2Stage):
    def __init__(self, configurable, stage):
        self.__stage = stage
        self.__cache = {}
        super(Hlt2ExternalStage, self).__init__(stage._name(), [],
                                                [], stage._nickname(),
                                                stage._shared(), configurable)

    def stage(self, stages, cuts):
        ## Return what we create directly and add dependencies to stages.
        ## Get a local instance of the cuts
        from Utilities import allCuts
        cuts = allCuts(self._configurable())
        key = self._hashCuts(cuts)
        if key in self.__cache:
            cached = self.__cache[key]
            stages += cached[1]
            return cached[0]

        deps = []
        stage = self.__stage.stage(deps, cuts)
        self.__cache[key] = (stage, deps)
        stages += deps
        return stage
