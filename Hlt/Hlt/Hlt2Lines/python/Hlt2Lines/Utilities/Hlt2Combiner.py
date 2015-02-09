from copy import deepcopy
from Hlt2Stage import Hlt2Stage
class Hlt2Combiner(Hlt2Stage):
    __cutTypes = set(('MotherCut', 'DaughtersCuts', 'CombinationCut'))
    def __init__(self, prefix, name, decay, cuts, inputs, dependencies = [], **kwargs):
        self.__prefix = prefix
        self.__kwargs = kwargs
        self.__decay = decay
        
        for ct in cuts.keys():
            if ct not in self.__cutTypes:
                raise TypeError("Hlt2Combiner: cut type %s must be one of %s" % (ct, ' '.join(self.__cutTypes)))
        self.__cuts = cuts
        Hlt2Stage.__init__(self, name, inputs, dependencies)

    def clone(self, name, prefix = None, decay = None, cuts = None,
              inputs = None, dependencies = None, **kwargs):
        args = deepcopy(self.__kwargs)
        args.update(kwargs)
        deps = dependencies
        return Hlt2Combiner(prefix if prefix else self.__prefix, name,
                            decay  if decay  else self.__decay,
                            cuts   if cuts   else self.__cuts,
                            inputs if inputs else self._inputs(),
                            deps   if deps   else self._deps(),
                            **args)
                                 
    def stage(self, cuts):
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import CombineParticles
        args = deepcopy(self.__kwargs)
        for k, v in self.__cuts.iteritems():
            if k == 'DaughtersCuts':
                args[k] = dict((l, w % cuts.get(self._name(), cuts['Common'])) for l, w in v.iteritems())
            else:
                args[k] = v % cuts.get(self._name(), cuts['Common'])
        return Hlt2Member(CombineParticles, self.__prefix + self._name() + 'Combiner', 
                          DecayDescriptor = self.__decay, Inputs = self.inputStages(cuts), **args)

