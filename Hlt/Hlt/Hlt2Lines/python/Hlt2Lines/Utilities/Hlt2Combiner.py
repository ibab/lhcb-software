from copy import deepcopy
from Hlt2TisTosFilter import Hlt2TisTosStage

class Hlt2Combiner(Hlt2TisTosStage):
    __cutTypes = set(('MotherCut', 'DaughtersCuts', 'CombinationCut'))
    def __init__(self, prefix, name, decay, cuts, inputs, dependencies = [], tistos = [],
                 **kwargs):
        self.__kwargs = kwargs
        self.__decay = decay
        self.__stage = None
        for ct in cuts.keys():
            if ct not in self.__cutTypes:
                raise TypeError("Hlt2Combiner: cut type %s must be one of %s" % (ct, ' '.join(self.__cutTypes)))
        self.__cuts = cuts
        Hlt2TisTosStage.__init__(self, prefix, name, inputs, dependencies, tistos)

    def clone(self, name, prefix = None, decay = None, cuts = None,
              inputs = None, dependencies = None, tistos = None, **kwargs):
        args = deepcopy(self.__kwargs)
        args.update(kwargs)
        deps = dependencies
        return Hlt2Combiner(prefix if prefix else self._prefix(), name,
                            decay  if decay  else self.__decay,
                            cuts   if cuts   else self.__cuts,
                            inputs if inputs else self._inputs(),
                            deps   if deps   else self._deps(),
                            tistos if tistos else self._tistos(),
                            **args)

    def _makeMember(self, cuts, args):
        from HltLine.HltLine import Hlt2Member
        from Configurables import CombineParticles
        return Hlt2Member(CombineParticles, self._prefix() + self._name() + 'Combiner',
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
            self.__stage = self._makeMember(args)
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
