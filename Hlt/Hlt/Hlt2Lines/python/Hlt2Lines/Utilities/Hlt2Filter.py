from copy import deepcopy
from Hlt2Stage import Hlt2Stage
class Hlt2ParticleFilter(Hlt2Stage):
    def __init__(self, prefix, name, code, inputs, dependencies = [], **kwargs):
        self.__prefix = prefix
        self.__code = code
        self.__kwargs = kwargs

        Hlt2Stage.__init__(self, name, inputs, dependencies)

    def clone(self, name, prefix = None, code = None, inputs = None,
              dependencies = None, **kwargs):
        args = deepcopy(self.__kwargs)
        args.update(kwargs)
        deps = dependencies
        return Hlt2ParticleFilter(prefix if prefix else self.__prefix, name,
                                 code   if code   else self.__code,
                                 inputs if inputs else self._inputs(),
                                 deps   if deps   else self._deps(),
                                 **args)
                                 
    def stage(self, cuts):
        from HltLine.HltLine import Hlt2Member
        from Configurables import FilterDesktop
        return Hlt2Member(FilterDesktop, self.__prefix + self._name() + 'Filter', 
                          Code = self.__code % cuts.get(self._name(), cuts['Common']),
                          Inputs = self.inputStages(cuts), **(self.__kwargs))

class Hlt2VoidFilter(Hlt2Stage):
    def __init__(self, prefix, name, code, inputs, dependencies = [], **kwargs):
        self.__prefix = prefix
        self.__code = code
        self.__kwargs = kwargs
        Hlt2Stage.__init__(self, name, inputs, dependencies)

    def clone(self, name, prefix = None, code = None, inputs = None,
              dependencies = []):
        deps = dependencies
        return Hlt2VoidFilter(prefix if prefix else self.__prefix, name,
                              code   if code   else self.__code,
                              inputs if inputs else self._inputs(),
                              deps   if deps   else self._deps())
                                 
    def stage(self, cuts):
        from HltLine.HltLine import bindMembers
        from Configurables import LoKi__VoidFilter as VoidFilter
        vfilter = VoidFilter('Hlt2' + self.__prefix + self._name() + 'VoidFilter',
                             Code = self.__code % cuts.get(self._name(), cuts['Common']))
        return bindMembers(None, self.dependencies(cuts) + self.inputStages(cuts) + [vfilter])
