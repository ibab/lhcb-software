"""
Utilities for displaced vertex stripping: wrapper class around PatPV3D
"""

__all__ = ( "SelectionFromAlgorithmWithTools", "SelectionPatPV3DWrapper" )

from GaudiConfUtils import isConfigurable

from SelPy.utils import ( UniquelyNamedObject, ClonableObject, SelectionBase
                        , compatibleSequences, IncompatibleInputLocations
                        , CloneCallable, makeOutputLocation, setOutputLocation )

from PhysSelPython.Wrappers import checkConfigurable, checkName

def connectToRequiredSelections(selection, inputSetter) :
    """
    Make selection get input data from its requiredSelections via an inputSetter method (str).
    Changed from SelPy.utils to descend into the configurable tree of PatPV3D if inputSetter contains "."
    """
    _outputLocations = [sel.outputLocation() for sel in selection.requiredSelections()]
    _outputLocations = filter(lambda s : s != '', _outputLocations)
    configurable = selection.algorithm()

    if inputSetter:
        attrTree = inputSetter.split(".")
        for attrName in attrTree[:-1]:
            if hasattr(configurable, attrName):
                configurable = getattr(configurable, attrName)
        _inputLocations = getattr(configurable, attrTree[-1])
        if len(_inputLocations) != 0 :
            if not compatibleSequences(_outputLocations,
                                       _inputLocations) :
                raise IncompatibleInputLocations('InputLocations of input algorithm incompatible with RequiredSelections!'\
                                                 '\nInputLocations: '+str(_inputLocations)+\
                                                 '\nRequiredSelections: '+str(_outputLocations))
        configurable.__setattr__(attrTree[-1], list(_outputLocations))

"""
Selection-like class, constructed from a configurable algorithm (instead of ConfigurableGenerator),
with the option to set delegate the input location to a tool property (e.g. PVOfflineTool for PatPV3D
Merged code from PhysSelPython.Wrappers and SelPy.selection
"""
class SelectionFromAlgorithmWithTools( UniquelyNamedObject, ClonableObject, SelectionBase ) :

    def __init__( self
                , name
                , Algorithm
                , RequiredSelections = []
                , OutputBranch       = "Phys"
                , InputDataSetter    = "Inputs"
                , OutputDataSetter   = "Output"
                , Extension          = "Particles" ) :

        checkName(name)

        UniquelyNamedObject.__init__(self, name)
        ClonableObject.__init__(self, locals())

        _outputLocation = makeOutputLocation(name = self.name(),
                                             branch    = OutputBranch,
                                             leaf      = Extension)

        alg = Algorithm.clone(name)

        SelectionBase.__init__(self,
                               algorithm = alg,
                               outputLocation = _outputLocation,
                               requiredSelections = RequiredSelections )

        connectToRequiredSelections(self, InputDataSetter)
        setOutputLocation(self, OutputDataSetter)

"""
Construct a SelectionFromAlgorithmWithTools with default settings for PatPV3D
"""
def SelectionPatPV3DWrapper( name
                           , Algorithm
                           , RequiredSelections = []
                           , OutputBranch       = "Phys"
                           , InputDataSetter    = "PVOfflineTool.InputTracks"
                           , OutputDataSetter   = "OutputVerticesName"
                           , Extension          = "Vertices" ) :
    return SelectionFromAlgorithmWithTools( name
             , Algorithm
             , RequiredSelections = RequiredSelections
             , OutputBranch       = OutputBranch
             , InputDataSetter    = InputDataSetter
             , OutputDataSetter   = OutputDataSetter
             , Extension          = Extension )
