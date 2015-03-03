from copy import deepcopy
from Hlt2Stage import Hlt2Stage
from Hlt2TisTosFilter import Hlt2TisTosStage

class Hlt2ParticleFilter(Hlt2TisTosStage):
    def __init__(self, name, code, inputs, dependencies = [], tistos = [],
                 nickname = None, shared = False, **kwargs):
        self.__code = code
        self.__kwargs = kwargs
        self.__stage = None
        Hlt2TisTosStage.__init__(self, name, inputs, dependencies, tistos, nickname, shared)

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

    def _makeMember(self, cuts, args):
        from HltLine.HltLine import Hlt2Member
        from Configurables import FilterDesktop
        return Hlt2Member(FilterDesktop, self._name() + 'Filter', shared = self._shared(),
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
    def __init__(self, name, code, inputs, dependencies = [], nickname = None,
                 shared = False, **kwargs):
        self.__code = code
        self.__kwargs = kwargs
        self.__stage = None
        Hlt2Stage.__init__(self, name, inputs, dependencies, nickname, shared)

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
                                 
    def stage(self, cuts):
        if self.__stage != None:
            return self.__stage
        from HltLine.HltLine import bindMembers
        from Configurables import LoKi__VoidFilter as VoidFilter
        vfilter = VoidFilter('Hlt2' + self._name(),
                             Code = self.__code % cuts.get(self._nickname(), cuts['Common']))
        self.__stage = bindMembers(None, self.dependencies(cuts) + self.inputStages(cuts) + [vfilter])
        return self.__stage
