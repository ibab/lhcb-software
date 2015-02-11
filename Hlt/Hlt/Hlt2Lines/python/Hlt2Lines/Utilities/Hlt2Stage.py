from copy import deepcopy
from Utilities import makeList, splitSpecs

class Hlt2Stage(object):
    def __init__(self, name, inputs, dependencies = []):
        self.__name = name
        self.__inputs = inputs
        self.__deps = dependencies
        self.__depStages = None
        self.__inputStages = None
        
    def _name(self):
        return self.__name

    def _inputs(self):
        return self.__inputs

    def _addInputs(self, inputs):
        self.__inputs.extend(inputs)

    def _deps(self):
        return self.__deps

    def _addDeps(self, deps):
        self.__deps.extend(deps)
                                        
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
        # Flatten everything, including bindMembers.
        def __flatten(l):
            import collections
            for i in l:
                if isinstance(i, Hlt2Stage):
                    for j in __flatten(i._deps()): yield j
                    for j in __flatten(i._inputs()): yield j
                    yield i
                else:
                    yield i
        ## Make the stage first, this might update deps/inputs!!
        stage = self.stage(cuts)
        ## Flatten the list of deps and inputs and create stages to return.
        deps = list(__flatten(self.__deps)) + list(__flatten(self.__inputs))
        if self in deps:
            print 'WARNING: Circular dependency %s %s %s.' % (self.__name, self.__deps, self.__inputs)
        return [i.stage(cuts) if hasattr(i, 'stage') else i for i in deps] + [stage]
