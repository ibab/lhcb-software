from copy import deepcopy
from Hlt2TisTosFilter import Hlt2TisTosStage
from Configurables import CombineParticles

class Hlt2Combiner(Hlt2TisTosStage):
    __cutTypes = set(('MotherCut', 'DaughtersCuts', 'CombinationCut'))
    def __init__(self, prefix, name, decay, inputs, dependencies = [], tistos = [],
                 combiner = CombineParticles, **kwargs):
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
        Hlt2TisTosStage.__init__(self, prefix, name, inputs, dependencies, tistos)

    def clone(self, name, **kwargs):
        args = deepcopy(self.__kwargs)
        args['name'] = name
        cuts = deepcopy(self.__cuts)
        for ct in self.__cutTypes:
            if ct in kwargs: cuts[ct] = kwargs.pop(ct)
        args.update(cuts)
        for arg, default in (('prefix', self._prefix()),
                             ('decay',  self.__decay),
                             ('inputs', self._inputs()),
                             ('tistos', self._tistos()),
                             ('dependencies', self._deps()),
                             ('combiner', self.__combiner)):
            args[arg] = kwargs.pop(arg) if arg in kwargs else default
        args.update(kwargs)
        
        return Hlt2Combiner(**args)

    def _makeMember(self, cuts, args):
        from HltLine.HltLine import Hlt2Member
        return Hlt2Member(self.__combiner, self._prefix() + self._name() + 'Combiner',
                          DecayDescriptor = self.__decay, Inputs = self.inputStages(cuts), **args)
                                     
    def stage(self, cuts):
        if self.__stage != None:
            return self.__stage
        
        ## Copy args and substitute cut values
        args = deepcopy(self.__kwargs)
        for k, v in self.__cuts.iteritems():
            if k == 'DaughtersCuts':
                args[k] = dict((l, w % cuts.get(self._name(), cuts['Common'])) for l, w in v.iteritems())
            else:
                args[k] = v % cuts.get(self._name(), cuts['Common'])

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
