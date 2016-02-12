from Hlt2Stage import Hlt2Stage
from Utilities import makeStages

class Hlt2MergedStage(Hlt2Stage):
    def __init__(self, name, inputs, dependencies = [],
                 nickname = None, shared = False, **kwargs):
        """Create a MergedStage from inputs."""
        def __shared(i):
            if hasattr(i, '_shared'):
                return i._shared()
            elif hasattr(i, 'Shared'):
                return i.Shared
            else:
                return True
        self.__kwargs = {}
        self.__cache = {}
        
        if shared and not all(__shared(i) for i in inputs):
            raise RuntimeError("All inputs to a shared Hlt2MergedStage must be shared.")
        self.__inputs = inputs
        ## Set Hlt2Stage inputs and dependencies to []; we take care of it
        ## ourselves.
        super(Hlt2MergedStage, self).__init__(name, [], dependencies = dependencies,
                                              nickname = nickname, shared = shared)

    def clone(self, name, **kwargs):
        args = deepcopy(self.__kwargs)
        args['name'] = name
        args['shared'] = self._shared()
        for arg, default in (('inputs', self._inputs()),
                             ('dependencies', self._deps()),
                             ('nickname', self._nickname())):
            args[arg] = kwargs.pop(arg) if arg in kwargs else default
        args.update(kwargs)

        return Hlt2MergedStage(**args)
        
    def outputStages(self, stages, cuts):
        # Return our inputs as outputs to indicate that those should be used as
        # directly as inputs to whatever this MergedStage is an input to,
        # instead of the return of stage
        outs = makeStages([], self.__inputs, cuts)
        stages += [self.stage(stages, cuts)]
        return outs
            
    def stage(self, stages, cuts):
        key = self._hashCuts(cuts)
        if key in self.__cache:
            stage, deps = self.__cache[key]
            stages += deps
            return stage

        from HltLine.HltLine import bindMembers
        def __flatten(l):
            for i in l:
                if isinstance(i, bindMembers):
                    for j in __flatten(i.members()): yield j
                else:
                    yield i
        
        ## Make SubSequences for each of the input that are in regular AND mode.
        ## Do a flatten and unique per subsequence. Plug these into another
        ## SubSequence that runs in ModeOR without ShortCircuit.
        from Utilities import uniqueEverseen
        from HltLine.HltLine import Hlt2SubSequence
        subAlgos = []
        for i, ipt in enumerate(self.__inputs):
            deps = []
            deps += makeStages(deps, [ipt], cuts)
            subAlgos.append((ipt._name() if hasattr(ipt, '_name') else str(i),
                             list(uniqueEverseen(__flatten(deps)))))

        # Find the algorithms common to all inputs
        base = subAlgos[0][1]
        i = 0
        for alg in base:
            done = False
            for _, sa in subAlgos[1:]:
                if i > len(sa):
                    done = True
                    break
                elif id(sa[i]) != id(alg):
                    done = True
                    break
            if done:
                break
            i += 1

        # Common part will be included as dependencies
        common = base[: i]
        
        # Make the subsequences
        subs = []            
        for name, algos in subAlgos:
            if i == len(algos):
                continue
            subs.append(Hlt2SubSequence(self._name() + name + 'SubSequence',
                                        algos[i : ], shared = self._shared()))
        
        from HltLine.HltLine import Hlt2SubSequence
        stage = Hlt2SubSequence(self._name() + 'Merged', subs,
                                shared = self._shared(),
                                ModeOR = True, ShortCircuit = False, **self.__kwargs)
        deps = self.dependencies(cuts) + common
        self.__cache[key] = (stage, deps)
        stages += deps
        return stage
