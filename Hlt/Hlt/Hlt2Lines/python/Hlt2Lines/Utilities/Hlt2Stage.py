from copy import deepcopy
from Utilities import makeList, splitSpecs

class Hlt2Stage(object):
    def __init__(self, name, inputs, dependencies = [], nickname = None, shared = False):
        self.__name = name
        self.__nickname = name if nickname == None else nickname
        self.__inputs = inputs
        self.__deps = dependencies
        self.__shared = shared
        self.__depStages = None
        self.__inputStages = None
        
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
    
    def inputStages(self, cuts):
        if self.__inputStages == None:
            self.__inputStages = [i.stage(cuts) if hasattr(i, 'stage') else i for i in self._inputs()]
        return self.__inputStages

    def dependencies(self, cuts):
        if self.__depStages == None:
            self.__depStages = [i.stage(cuts) if hasattr(i, 'stage') else i for i in self._deps()]
        return self.__depStages
    
    def stages(self, cuts):
        from Hlt2Filter import Hlt2ParticleFilter
        from Hlt2Combiner import Hlt2Combiner
        # Flatten everything
        def __flatten(l):
            for i in l:
                if isinstance(i, Hlt2Stage):
                    for j in __flatten(i._deps()):
                        yield j
                    for j in __flatten(i._inputs()):
                        yield j
                    if i: yield i
                elif i:
                    yield i
        ## Make our own stage first, this might update deps/inputs!!
        stage = self.stage(cuts)
        ## Flatten the list of deps and inputs and create stages to return.
        deps = list(__flatten(self.__deps)) + list(__flatten(self.__inputs))
        if self in deps:
            print 'WARNING: Circular dependency %s %s %s.' % (self.__name, self.__deps, self.__inputs)
        stages = filter(lambda i: bool(i), [i.stage(cuts) if hasattr(i, 'stage') else i for i in deps])
        if stage:
            stages += [stage]
        return stages

class Hlt2ExternalStage(Hlt2Stage):
    def __init__(self, configurable, stage):
        self.__conf = configurable
        self.__stage = stage
        super(Hlt2ExternalStage, self).__init__(stage._name(), stage._inputs(),
                                                stage._deps(), stage._nickname(),
                                                stage._shared())
        
    def stage(self, cuts):
        return self.__stage.stage(cuts)

    def stages(self, cuts):
        from copy import deepcopy
        # Get a local instance of the cuts
        common = self.__conf.getProps().get('Common', {})
        cuts = deepcopy(self.__conf.getProps())
        for k, v in cuts.iteritems():
            if k != 'Common': v.update(common)

        super(Hlt2ExternalStage, self).stages(cuts)
