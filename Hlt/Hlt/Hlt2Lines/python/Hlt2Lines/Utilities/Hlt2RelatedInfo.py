from Hlt2Stage import Hlt2Stage

class Hlt2RelatedInfo(Hlt2Stage):
    _toolCache = {}

    def __init__(self, name, type, dependencies = [], nickname = None, **args):
        super(Hlt2RelatedInfo, self).__init__(name, [],
                                              dependencies, nickname,
                                              shared = False)
        self.__args = args
        self.__type = type

    def __configureInfoTool(self, toolConfig):
        toolTypeName = self.__type.getType()

        def __makePropKey(props):
            return hash(repr(sorted(props.items())))

        # make a hashable key from the properties dict
        propkey = __makePropKey(toolConfig)

        # Get the entry in the cache map for this type of tool
        if toolTypeName not in Hlt2RelatedInfo._toolCache :
            Hlt2RelatedInfo._toolCache[toolTypeName] = {"ToolNum" : 1, "Tools" : {}}
        configmap = Hlt2RelatedInfo._toolCache[toolTypeName]

        # Does an instance with the exact configuration requested already exist ?
        toolName = "UNDEFINED"
        if propkey in configmap["Tools"]:
            # Just reuse the existing tool
            toolName = configmap["Tools"][propkey]
        else:
            # need to configure a new tool
            # Get instance class type

            # Construct global name
            toolName = toolTypeName + "_" + str(configmap["ToolNum"])

            # Increment count for next time
            configmap["ToolNum"] += 1

            # Instead of using the Hlt1Tool wrapper we explicitly make an
            # instance owned by tool svc, because these tools are all public
            # tools.
            toolInstance = self.__type("ToolSvc." + toolName)

            # set options, we'll have gotten the common ones too, which we don't
            # care about if they are not properties of our tool
            for property, value in toolConfig.iteritems():
                setattr( toolInstance, property, value )

            # Save toolname in the cache map
            configmap["Tools"][propkey] = toolName

            # for debugging ...
            #toolInstance.OutputLevel = 1

        # Add tool to algorithm, as a public tool
        return toolTypeName + '/' + toolName + ":PUBLIC"

    def infoStage(self, cuts, input):
        ## Configure the algorithms that runs the tools
        from Configurables import AddRelatedInfo
        from HltLine.HltLine import Hlt2Member, Hlt2SubSequence

        ## and Create the tool and algorithm
        args = self._localCuts(cuts)
        args.update(self.__args)
        location = args.get('Location', None)
        daughterLocations = args.get('DaughterLocations', None)
        if not location and not daughterLocations:
            raise Exception('\n Neither "Location" nor "DaughterLocations" are defined in RelatedInfo dictionary')

        ## Split the arguments
        toolConf = {}
        algArgs = {}
        algAtt = ("Location", "DaughterLocations")
        for k, v in args.iteritems():
            if k in algAtt:
                algArgs[k] = v
            elif hasattr(self.__type, k):
                toolConf[k] = v

        relatedInfoAlg = Hlt2Member(AddRelatedInfo, "RelatedInfo" + self._name(),
                                    Inputs = [input],
                                    Tool = self.__configureInfoTool(toolConf),
                                    **algArgs)

        if self._deps():
            deps = self.dependencies({})
            return Hlt2SubSequence(self._name() + 'RelatedInfoSequence', deps + [relatedInfoAlg],
                                   shared = self._shared())
        else:
            return relatedInfoAlg


class Hlt2RelInfoConeVariables(Hlt2RelatedInfo):
    def __init__(self, name, nickname = None):
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking as Hlt2LongTracking
        tracks = Hlt2LongTracking().hlt2PrepareTracks()
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking as Hlt2DownTracking
        longTracks = Hlt2LongTracking().hlt2PrepareTracks()
        downTracks = Hlt2DownTracking().hlt2PrepareTracks()
        locations = [t.outputSelection() for t in (longTracks, downTracks)]
        from Configurables import RelInfoConeVariables
        super(Hlt2RelInfoConeVariables, self).__init__(name, RelInfoConeVariables,
                                                       [longTracks, downTracks], nickname,
                                                       Inputs = locations)

class Hlt2RelInfoVertexIsolation(Hlt2RelatedInfo):
    def __init__(self, name, nickname = None):
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions as Hlt2NoPIDsPions
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownPions as Hlt2DownPions
        from Configurables import RelInfoVertexIsolation
        super(Hlt2RelInfoVertexIsolation, self).__init__(name, RelInfoVertexIsolation, [Hlt2NoPIDsPions, Hlt2DownPions], nickname,
                                                         InputParticles = [Hlt2NoPIDsPions.outputSelection(),
                                                                           Hlt2DownPions.outputSelection()])
