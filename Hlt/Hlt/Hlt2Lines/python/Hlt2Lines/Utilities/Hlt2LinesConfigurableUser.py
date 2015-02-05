from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
class Hlt2LinesConfigurableUser(HltLinesConfigurableUser):
    __slots__ = {'_stages' : {}}

    def stages(self):
        from copy import deepcopy
        algos = []
        # Get a local instance of the cuts
        common = self.getProps()['Common']
        cuts = deepcopy(self.getProps())
        for v in cuts.itervalues():
            v.update(common)
        s = {}

        from itertools import chain
        from Hlt2Lines.Utilities.Utilities import uniqueEverseen
        for name, stages in self._stages.iteritems():
            flattened = chain.from_iterable([i.stages(cuts) if hasattr(i, 'stages') else (i,) for i in stages])
            s[name] = tuple(uniqueEverseen(flattened))
        return s

