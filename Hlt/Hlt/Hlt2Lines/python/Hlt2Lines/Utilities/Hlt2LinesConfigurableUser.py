from copy import deepcopy
from Utilities import allCuts, makeStages
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2LinesConfigurableUser(HltLinesConfigurableUser):
    __slots__ = {'_stages' : {},
                 'Prescale'  : {},
                 'Postscale' : {}}
    
    def __stages(self, source):
        # External stage sets the configurable to something that is not self.
        if source._configurable():
            conf = stage._configurable()
        else:
            conf = self

        cuts = allCuts(conf)
        deps = []
        stage = source.stage(deps, cuts)
        if stage in deps:
            print 'WARNING: Circular dependency %s %s %s.' % (source._name(), source._deps(),
                                                              source._inputs())
        stages = filter(lambda i: bool(i), deps + [stage])
        return stages
    
    def algorithms(self, stages):
        from HltLine.HltLine import bindMembers

        def __flatten(l):
            for i in l:
                if isinstance(i, bindMembers):
                    for j in __flatten(i.members()): yield j
                else:
                    yield i
                
        from Hlt2Lines.Utilities.Utilities import uniqueEverseen
        from Hlt2Stage import Hlt2Stage

        sd = {}
        for name, stg in stages.iteritems():
            sl = []
            for i in stg:
                if isinstance(i, Hlt2Stage):
                    sl.extend(self.__stages(i))
                else:
                    sl.append(i)
            sd[name] = list(uniqueEverseen(__flatten(sl)))

        return sd
