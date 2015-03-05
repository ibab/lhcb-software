from copy import deepcopy
from Hlt2TisTosFilter import Hlt2TisTosStage
from Configurables import CombineParticles


class Hlt2Combiner(Hlt2TisTosStage):
    __cutTypes = set(('MotherCut', 'DaughtersCuts', 'CombinationCut') +
                     ('Combination%sCut' % s for s in Hlt2Combiner.__counter(8)))
    def __init__(self, name, decay, inputs, dependencies = [], tistos = [],
                 combiner = CombineParticles, nickname = None, shared = False, **kwargs):
        self.__decay = decay
        self.__stage = None

        ## Allow cuts to specified as kwargs
        cuts = {}
        for ct in self.__cutTypes:
            if ct in kwargs: cuts[ct] = kwargs.pop(ct)
        self.__cuts = cuts
        self.__kwargs = kwargs

        ## Support NXBodyDecays
        self.__combiner = combiner
        super(Hlt2Combiner, self).__init__(self, name, inputs, dependencies,
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
            args[arg] = kwargs.pop(arg) if arg in kwargs else default
        args.update(kwargs)
        
        return Hlt2Combiner(**args)

    def _makeMember(self, cuts, args):
        from HltLine.HltLine import Hlt2Member
        decays = [self.__decay] if type(self.__decay) == str else self.__decay
        return Hlt2Member(self.__combiner, self._name() + 'Combiner', shared = self._shared(),
                          DecayDescriptors = decays, Inputs = self.inputStages(cuts),
                          **args)
                                     
    def stage(self, cuts):
        if self.__stage != None:
            return self.__stage
        
        ## Copy args and substitute cut values
        args = deepcopy(self.__kwargs)
        for k, v in self.__cuts.iteritems():
            if k == 'DaughtersCuts':
                args[k] = dict((l, w % cuts.get(self._nickname(), cuts['Common'])) for l, w in v.iteritems())
            else:
                args[k] = v % cuts.get(self._nickname(), cuts['Common'])

        if not self._tistos():
            ## Return combiner if no tistos is required
            self.__stage = self._makeMember(cuts, args)
            return self.__stage

        ## define the callback used to insert the right extra cut.
        def __combCutHandler(tagger, specs, args):
            tisTosCut = tagger.combinationCut(specs.values())
            if 'CombinationCut' in args:
                args['CombinationCut'] = '{0} & {1}'.format(tisTosCut, args['CombinationCut'])
            else:
                args['CombinationCut'] = tisTosCut

        self.__stage = self._handleTisTos(cuts, args, __combCutHandler)
        return self.__stage
        ## Convenience function to create the actual particle combiner

    def __counter(n):
        m = 3
        while m <= n:
            yield ''.join(str(i) for i in range(1, m))
            m += 1
