from copy import deepcopy
from Hlt2Stage import Hlt2Stage
from Hlt2TisTosFilter import Hlt2TisTosStage

class Hlt2ParticleFilter(Hlt2TisTosStage):
    def __init__(self, prefix, name, code, inputs, dependencies = [], tistos = [],
                 **kwargs):
        self.__code = code
        self.__kwargs = kwargs
        self.__stage = None
        Hlt2TisTosStage.__init__(self, prefix, name, inputs, dependencies, tistos)

    def clone(self, name, prefix = None, code = None, inputs = None,
              dependencies = None, tistos = None, **kwargs):
        args = deepcopy(self.__kwargs)
        args.update(kwargs)
        deps = dependencies
        return Hlt2ParticleFilter(prefix if prefix else self._prefix(), name,
                                 code   if code   else self.__code,
                                 inputs if inputs else self._inputs(),
                                 deps   if deps   else self._deps(),
                                 tistos if tistos else self._tistos(),
                                 **args)

    def _makeMember(self, cuts, args):
        from HltLine.HltLine import Hlt2Member
        from Configurables import FilterDesktop
        return Hlt2Member(FilterDesktop, self._prefix() + self._name() + 'Filter', 
                          Inputs = self.inputStages(cuts), **args)
                                     
    def stage(self, cuts):
        if self.__stage != None:
            return self.__stage
        args = {'Code' : self.__code % cuts.get(self._name(), cuts['Common'])}
        if not self._tistos():
            ## Return combiner if no tistos is required
            self.__stage = self._makeMember(cuts, args)
            return self.__stage

        ## define the callback used to insert the right extra cut.
        def __partCutHandler(tagger, specs, args):
            tisTosCut = tagger.particleCut(specs.values())
            args['Code'] = '{0} & {1}'.format(tisTosCut, args['Code'])

        self.__stage = self._handleTisTos(cuts, args, __partCutHandler)
        return self.__stage
    
class Hlt2VoidFilter(Hlt2Stage):
    def __init__(self, prefix, name, code, inputs, dependencies = [], **kwargs):
        self.__prefix = prefix
        self.__code = code
        self.__kwargs = kwargs
        self.__stage = None
        Hlt2Stage.__init__(self, name, inputs, dependencies)

    def clone(self, name, prefix = None, code = None, inputs = None,
              dependencies = []):
        deps = dependencies
        return Hlt2VoidFilter(prefix if prefix else self.__prefix, name,
                              code   if code   else self.__code,
                              inputs if inputs else self._inputs(),
                              deps   if deps   else self._deps())
                                 
    def stage(self, cuts):
        if self.__stage != None:
            return self.__stage
        from HltLine.HltLine import bindMembers
        from Configurables import LoKi__VoidFilter as VoidFilter
        vfilter = VoidFilter('Hlt2' + self.__prefix + self._name() + 'VoidFilter',
                             Code = self.__code % cuts.get(self._name(), cuts['Common']))
        self.__stage = bindMembers(None, self.dependencies(cuts) + self.inputStages(cuts) + [vfilter])
        return self.__stage
