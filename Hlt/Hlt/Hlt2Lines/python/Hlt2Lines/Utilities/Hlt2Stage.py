from copy import deepcopy

class Hlt2Stage(object):
    def __init__(self, name, inputs, dependencies = []):
        self.__name = name
        self.__inputs = inputs
        self.__deps = dependencies
        
    def _name(self):
        return self.__name

    def _inputs(self):
        return self.__inputs

    def _deps(self):
        return self.__deps
                                    
    def inputStages(self, cuts):
        return [i.stage(cuts) if hasattr(i, 'stage') else i for i in self._inputs()]

    def dependencies(self, cuts):
        return [i.stage(cuts) if hasattr(i, 'stage') else i for i in self._deps()]
    
    def stages(self, cuts):
        from Hlt2Filter import Hlt2ParticleFilter
        from Hlt2Combiner import Hlt2Combiner
        def __flatten(l):
            import collections
            for i in l:
                if isinstance(i, Hlt2Stage):
                    for j in __flatten(i._deps()): yield j
                    for j in __flatten(i._inputs()): yield j
                    yield i
                else:
                    yield i
        deps = list(__flatten(self.__deps)) + list(__flatten(self.__inputs))
        if self in deps:
            print 'WARNING: Circular dependency %s %s %s.' % (self.__name, self.__deps, self.__inputs)
        return [i.stage(cuts) if hasattr(i, 'stage') else i for i in deps] + [self.stage(cuts)]

    
