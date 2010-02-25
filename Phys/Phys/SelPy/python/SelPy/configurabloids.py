#
from copy import copy

__all__ = ('DummyAlgorithm',
           'DummySequencer',
           'ConfigurableGenerator')

class ConfigurableGenerator(object) :
    ''' Singleton to store dummy configurabloids'''

    class __impl(object) :
        _configurable_map = {}
        def __call__(self, name, conf_type, **kwargs) :
            if name in self._configurable_map.keys() :
                conf = self._configurable_map[name]
                if type(conf) == conf_type :
                    print 'Returning ', type(conf), '/', name
                    return conf
                else :
                    raise TypeError(name+ ' already exists with different type ')
            else :
                print 'Creating a ', conf_type, ' called ', name
                conf = conf_type(name, **kwargs)
                self._configurable_map[name] = conf
                return conf


    __instance = None
    
    def __init__(self) :
        if ConfigurableGenerator.__instance is None :
            ConfigurableGenerator.__instance = ConfigurableGenerator.__impl()
        self.__dict__['_ConfigurableGenerator__instance'] = ConfigurableGenerator.__instance


    def __call__(self, name, conf_type, **kwargs) :
        return self.__instance(name, conf_type, **kwargs)

    def __getattr__(self, attr) :
        print 'getattr ', attr
        return getattr(self.__instance, attr)
    def __setattr(self, attr) :
        return setattr(self.__instance, attr)

class DummyAlgorithmOld(object) :
    def __init__(self, name, InputLocations = []) :
        self.InputLocations = copy(InputLocations)
        self._name = name

    def name(self) :
        return self._name
    def clone(self, name, **kwargs) :
        return DummyAlgorithm(name, **kwargs)

class DummyAlgorithm(object) :

    __instance = {}

    def __init__(self, name, InputLocations = []) :
        self._name = name
        if not name in DummyAlgorithm.__instance.keys() :
            DummyAlgorithm.__instance[name] = DummyAlgorithm.__impl(name,
                                                                    InputLocations)
        if InputLocations != [] and InputLocations != DummyAlgorithm.__instance[name].InputLocations :
            DummyAlgorithm.__instance[name].InputLocations = copy(InputLocations)

    class __impl(object) :
        def __init__(self, name, InputLocations) :
            self.InputLocations = copy(InputLocations)
            self._name = name
        def name(self) :
            return self._name
        
    def clone(self, name, **kwargs) :
        return DummyAlgorithm(name, **kwargs)
#        if name in DummyAlgorithm.__instance.keys() :
#            raise KeyError('Cannot clone with existing name ' + name)
#        else :
#            return DummyAlgorithm(name, **kwargs)

    def __getattr__(self, attr) :
        return getattr(self.__instance[self._name], attr)
    def __setattr(self, attr) :
        return setattr(self.__instance[self._name], attr)

class DummySequencer(object) :
    def __init__(self, name, Members = []) :
        self.Members = copy(Members)
        self._name = name
    def name(self) :
        return self._name

