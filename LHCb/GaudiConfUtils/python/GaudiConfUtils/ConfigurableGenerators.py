"""
Meta module to generate and expose ConfigurableGenerator classes.
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"

class MetaConfigurableGenerator(type):
    """
    Meta-class for ConfigurablesGenerator.

    Instrument the generated class with the same properties as the Configurable
    class it will generate.
    """
    def __new__(cls, name, bases, dct):
        """
        Add the property data members to the generated class and fill the __slots__
        data member accordingly.
        """
        if "__configurable__" not in dct:
            raise AttributeError, "class %s has no attribute '__configurable__'" % name
        # Set our slots from the  generated configurable class
        # excluding the 'private' slots
        conf = dct["__configurable__"]
        if conf is not None: # needed to allow that the base class is not bound to any configurable
            slots = conf.__slots__
        else:
            slots = []
        dct["__slots__"] = tuple([prop
                                  for prop in slots
                                  if not prop.startswith("_")])
        # generate the class
        return type.__new__(cls, name, bases, dct)

    def __setattr__(self, name, value):
        """
        A __setattr__ declaration here forbids modifications of the class object.
        """
        raise AttributeError("class %s has no attribute '%s'" % (self.__name__, name))


# The class Configurable has got several data members that are automatically
# added by the meta-class.
# pylint: disable-msg=E1101
class ConfigurableGenerator(object):
    """
    Base class for ConfigurableGenerators.
    """
    __metaclass__ = MetaConfigurableGenerator
    __configurable__ = None
    def __init__(self, **kwargs):
        """
        Initialize the instance properties from the constructor keyword arguments.
        """
        for n in kwargs:
            setattr(self, n, kwargs[n])

    def _props(self):
        """Return a dictionary of the properties."""
        props = {}
        for p in filter(lambda x: hasattr(self, x), self.__slots__):
            props[p] = getattr(self, p)
        return props

    def configurable(self, name):
        """
        Generate the configurable instance setting the properties to the values
        in the generator instance.
        """
        return apply(self.__configurable__, (name,), self._props())

    def __call__(self, name):
        """
        Generate the configurable instance setting the properties to the values
        in the generator instance.
        """
        return self.configurable(name)

    def __repr__(self):
        r = "%s(%s)" % (self.__class__.__name__,
                        ", ".join(["%s = %r" % i for i in self._props().items()]))
        return r

    def __eq__(self, other):
        return ((self.__class__ == other.__class__)
                and (self._props() == other._props()))
# pylint: enable-msg=E1101

class _ConfigurableGeneratorsModule(object):
    """
    'Meta' module used to access ConfigurableGenerator classes.
    """
    __path__ = None

    # Module name
    __name__ = __name__

    # Expose the base class for generators through the meta-module.
    ConfigurableGenerator = ConfigurableGenerator

    # This needs to be a dynamic property
    @property
    def __all__(self):
        import Gaudi.Configurables
        return Gaudi.Configurables.__all__

    def __makeGenerator__(self, name):
        """
        Return the generator for the Configurable class <name>.
        The created generator is stored in an internal property to avoid to
        generate it more than once.
        """
        code =  ("\n".join([  "from GaudiConfUtils.ConfigurableGenerators import ConfigurableGenerator"
                            , "from Gaudi.Configurables import %(name)s"
                            , "class %(name)s(ConfigurableGenerator):"
                            , "  '''Generator for configurable class %(name)s'''"
                            , "  __configurable__ = %(name)s"
                            ])) % { "name": name }
        locals = {} # temporary name space
        try:
            exec code in locals
        except:
            print "Exception in:"
            print code
            print "locals:", locals.keys()
            raise
        # Extract the new class from the temporary name space
        return locals[name]

    def __getattr__(self, name):
        """
        Main function of the meta-module.
        It delegates as much as possible to Gaudi.Configurables.
        """
        if name not in self.__all__:
            # We raise an AttributeError exception if the configurable could not be found
            # to respect the Python semantic.
            raise AttributeError("module '%s' does not have attribute '%s'" % (self.__name__, name))
        # create the generator class
        setattr(self, name, self.__makeGenerator__(name))
        return getattr(self, name)

# install the facade module instance as a module
import sys
sys.modules[__name__] = _ConfigurableGeneratorsModule()
