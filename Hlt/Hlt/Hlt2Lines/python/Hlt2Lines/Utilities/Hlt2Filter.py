class Hlt2Filter(object):
    def __init__(self, prefix, name, code, inputs, **kwargs):
        self.__prefix = prefix
        self.__name = name
        self.__code = code
        self.__inputs = inputs
        self.__kwargs = kwargs

    def name(self):
        return self.__name
        
    def inputs(self):
        return self.__inputs

    def inputStages(self, cuts):
        return [i.stage(cuts) if hasattr(i, 'stage') else i for i in self.inputs()]
    
    def stages(self, cuts):
        def __flatten(l):
            import collections
            for i in l:
                if isinstance(i, Hlt2Filter):
                    for j in __flatten(i.inputs()): yield j
                    yield i
                else:
                    yield i
        inputs = tuple(__flatten(self.__inputs))
        if self in inputs:
            print 'WARNING: Circular dependency %s %s.' % (self.__name, self.__inputs)
        return tuple(i.stage(cuts) if hasattr(i, 'stage') else i for i in inputs) + (self.stage(cuts),)
                     
    def stage(self, cuts):
        from HltLine.HltLine import Hlt2Member
        from Configurables import FilterDesktop
        return Hlt2Member(FilterDesktop, self.__name + 'Filter', 
                          Code = self.__code % cuts[self.__name],
                          UseP2PVRelations = False,
                          Inputs = self.inputStages(cuts), **(self.__kwargs))

