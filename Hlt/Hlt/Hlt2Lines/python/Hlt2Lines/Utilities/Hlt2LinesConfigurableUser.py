from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
class Hlt2LinesConfigurableUser(HltLinesConfigurableUser):
    __slots__ = {'Prescale'  : {},
                 'Postscale' : {}}
    
    def stages(self, stages):
        from copy import deepcopy
        algos = []
        # Get a local instance of the cuts
        common = self.getProps()['Common']
        cuts = deepcopy(self.getProps())
        for k, v in cuts.iteritems():
            if k != 'Common': v.update(common)

        sd = {}
        from Hlt2Lines.Utilities.Utilities import uniqueEverseen
        for name, stages in stages.iteritems():
            sl = []
            for i in stages:
                if hasattr(i, 'stages'):
                    sl.extend(i.stages(cuts))
                else:
                    sl.append(i)
            sd[name] = list(uniqueEverseen(sl))
        ## from pprint import pprint
        ## pprint(sd)
        return sd

