"""
This module will define containers used for the project. They will store
histograms and their corresponding scores.
"""

from abc import ABCMeta, abstractmethod
import os

import ROOT

from ..config import Config
from .io import GRFIO
from .interface import ComparisonFunction
from .score_manipulation import Score, ERROR_LEVELS
from .exceptions import DescriptionDictionaryKeyException, \
        ComparisonFunctionNotFoundInEvalDictException, ComparisonFunctionNotCollableException, \
        HistogramNotFoundException
from ..utils.rootutils import detect_zombies

class Combiner(object):
    """
    This class is responsible for analysing data and writing an evaluation
    summary to Giant Root File.

    An instance of this class represents a node in the Combiner tree. A node
    can be either a branch (BranchCombiner), which has one or more child nodes
    and information on how to combine the error levels of its child nodes into
    a single result, or a leaf node (LeafCombiner), which contains information
    on how to generate an error level for a specific histogram.
    """
    __metaclass__ = ABCMeta # Abstract base class

    # Static class variables
    WARNING_LIMITS = ["minWW", "minWE"]  # keys for warning limits
    ERROR_LIMITS   = ["minEW", "minEE"]  # keys for error limit
    # Types for versioned tree branches
    SCORE_TYPE     = "VersionedObject<Double_t, TimeStamp, std::greater<TimeStamp> >"
    LVL_TYPE       = "VersionedObject<UShort_t, TimeStamp, std::greater<TimeStamp> >"

    def __init__(self, name, desc_dict, data_file_path, ref_file_path):
        """
        Super constructor for abstract Combiner class. Only to be called from
        subclass constructors.

        Parameters:
            name          : name of a combiner (must be consistent with desc and eval dicts)
            desc_dict     : a combiner description dictionary (which describes the tree structure)
            data_file_path: a path to a root file with data to analyse
            ref_file_path : a path to a root file that serves as reference
        """
        self.__name = name
        self.score = None
        self.warnings = 0
        self.errors = 0
        self.data_file_path = data_file_path
        self.ref_file_path = ref_file_path
        self.results = None
        self.summary_data = None

        self.desc_dict = self.__clipDescDict(desc_dict)
        self.__weight = self.desc_dict["weight"]

    def stringRep(self):
        """Creates a string representation of a combiner for logging purposes"""

        log_string = ["*********************************************************"]

        log_string.append("Combiner's name: {}".format(self.getName()))
        log_string.append("Combiner's description: {}".format(self.desc_dict))
        self.evaluate()
        log_string.append("Combiner's evaluation results: {}".format(self.results))

        return log_string

    def __clipDescDict(self, desc_dict):
        """Clips the description dictionary so that all combiners don't need to store the entire dict in their memory"""
        try:
            return desc_dict[self.getName()]
        except KeyError:
            raise DescriptionDictionaryKeyException(self.getName(), desc_dict.keys())

    def getName(self):
        """Get the name of this Combiner node"""
        return self.__name
    def getWeight(self):
        """Get the weight of this Combiner node (relative to that of its siblings)"""
        return self.__weight

    @classmethod
    def create(cls, name, desc_dict, eval_dict, data_file_path, ref_file_path):
        """
        Create a Combiner instance, either a BranchCombiner (if it has
        children) or a leaf combiner (if it contains a histogram comparison).
        """
        desc = desc_dict.get(name, None)
        if not desc:
            return None

        if "children" in desc.keys():
            return BranchCombiner(name, desc_dict, eval_dict, data_file_path, ref_file_path)
        else:
            return LeafCombiner(name, desc_dict, eval_dict, data_file_path, ref_file_path)

    @staticmethod
    def get_summary_data_from_hist(hist):
        """Gets summary data from hist"""
        pass  # TODO Discuss how to implement this, possibly it's not a good place for that function (-> eval_dict)

    @staticmethod
    def update_with_expected_state(result):
        """Updates result data with expected states"""
        pass  # TODO Discuss how to implement this and find a way of configuring a current expected state

    @abstractmethod
    def evaluate(self):
        """
        Evaluate this combiner. Abstract method, to be implemented by
        subclasses.
        """
        pass

    @abstractmethod
    def isRelevant(self):
        """
        Whether this Combiner instance is relevant to evaluate.
        """
        pass

    def getLevel(self):
        self.evaluate()
        return self.results["lvl"]

    def getScore(self):
        self.evaluate()
        return self.results["score"]

    def getWarningsErrors(self):
        return (self.getWarnings(), self.getErrors())

    def getWarnings(self):
        """
        Returns number of warnings for this Combiner (errors are also counted
        as warnings).
        """
        return self.warnings + self.errors

    def getErrors(self):
        """
        Returns number of errors for this Combiner.
        """
        return self.errors

    def getWritableResults(self):
        """
        Get a dictionary containing the score and level values. First evaluates
        this Combiner if necessary.
        """
        return {\
            "score": self.getScore().value,\
            "lvl":   self.getLevel().Value\
        }

    def getBranches(self):
        """
        Get a dictionary mapping branches required by this Combiner to the
        respective ROOT types of those branches.
        """
        return {"score": self.SCORE_TYPE, "lvl": self.LVL_TYPE}

    def get_combiner_summary(self):
        """Returns summary information (score, lvl, summary data)"""
        #Evaluate to ensure a result is available
        self.evaluate()

        if self.summary_data:
            resultCopy = dict(self.results)
            resultCopy.update(self.summary_data)
            return resultCopy
        else:
            return self.results

class BranchCombiner(Combiner):
    def __init__(self, name, desc_dict, eval_dict, data_file_path, ref_file_path):
        """
        Parameters:
            desc_dict     : a combiner description dictionary (which describes the tree structure)
            eval_dict     : an evaluation dictionary that holds information about analysis method for each histogram
            data_file_path: a path to a root file with data to analyse
            ref_file_path : a path to a root file that serves as reference
            name          : name of a combiner (must be consistent with desc and eval dicts)
        """

        super(BranchCombiner, self).__init__(name, desc_dict, data_file_path, ref_file_path)
        self.warningLimits = [self.desc_dict[limit] for limit in Combiner.WARNING_LIMITS]
        self.errorLimits   = [self.desc_dict[limit] for limit in Combiner.ERROR_LIMITS  ]
        self.__createChildren(desc_dict, eval_dict)

    def __str__(self):
        """String representation of this BranchCombiner."""
        log_string = self.stringRep()

        children_names = [child.getName() for child in self.children]
        log_string.append("Combiner's warnings: {}, of which errors: {}".format(*self.getWarningsErrors()))
        log_string.append("Children:")
        for child in self.children:
            childDesc = "\tBranch: {} (weight {})\tScore: {} \tLevel: {} \tWarnings: {} \tErrors: {}".format(child.getName(), child.getWeight(), child.getScore(), child.getLevel(), *child.getWarningsErrors())
            if isinstance(child, LeafCombiner):
                childDesc += "\tPath: {}".format(child.getPath())
            log_string.append(childDesc)

        for child in self.children:
            if isinstance(child, BranchCombiner):
                log_string.append(str(child))
        log_string.append("*********************************************************")

        return "\n".join(log_string)

    def __createChildren(self, desc_dict, eval_dict):
        """Creates children for this BranchCombiner."""
        myDesc = desc_dict[self.getName()]
        if not myDesc or not "children" in myDesc.keys():
            return []

        self.children = []
        for combinerName in myDesc["children"]:
            child = Combiner.create(combinerName, desc_dict, eval_dict, self.data_file_path, self.ref_file_path)
            if child and child.isRelevant():
                self.children.append(child)

        self.sumChildWeights = sum([child.desc_dict["weight"] for child in self.children])

    def __calcLvl(self):
        """
        Calculates level depending on the description dictionary/expected
        state.
        """
        mapped_warnings = self.__mapNrToLvl(self.getWarnings(), self.warningLimits)
        mapped_errors   = self.__mapNrToLvl(self.getErrors(),   self.errorLimits  )
        return max(mapped_warnings, mapped_errors)

    def __mapNrToLvl(self, number, limit):
        """
        Maps number of warnings/errors to a specific error state.
        @returns a value of the enum ERROR_LEVELS.
        """
        if 0 <= number < limit[0]:
            return ERROR_LEVELS.OK
        elif limit[0] <= number < limit[1]:
            return ERROR_LEVELS.WARNING
        return ERROR_LEVELS.ERROR

    def __calcResults(self):
        """
        Calculates combiner's score and level.
        """
        score = Score(0)

        for child in self.children:
            score += child.getScore() * child.getWeight() / self.sumChildWeights
            childWarnings, childErros = child.getWarningsErrors()
            self.warnings += childWarnings
            self.errors   += childErros

        lvl = self.__calcLvl()

        return ComparisonFunction.create_final_dict(score, lvl)

    def getWritableResults(self):
        results = super(BranchCombiner, self).getWritableResults()
        for child in self.children:
            results[child.getName()] = child.getWritableResults()
        return results

    def getBranches(self):
        results = super(BranchCombiner, self).getBranches()
        for child in self.children:
            results[child.getName()] = child.getBranches()
        return results

    def evaluate(self):
        """
        Evaluate this BranchCombiner by evluating all its children and
        combining the results. If the results have already been calculated,
        this method returns silently.
        """
        if self.results:
            return

        for child in self.children:
            child.evaluate()

        self.results = self.__calcResults()

    def isRelevant(self):
        return bool(self.children)

class RootCombiner(BranchCombiner):
    MASTER_COMBINER_NAME = "MasterCombiner"    # name of the root combiner

    def __init__(self, desc_dict, eval_dict, data_file_path, ref_file_path, runnr):
        """
        Parameters:
            desc_dict     : a combiner description dictionary (which describes the tree structure)
            eval_dict     : an evaluation dictionary that holds information about analysis method for each histogram
            data_file_path: a path to a root file with data to analyse
            ref_file_path : a path to a root file that serves as reference
        """
        super(RootCombiner, self).__init__(RootCombiner.MASTER_COMBINER_NAME, desc_dict, eval_dict, data_file_path, ref_file_path)
        self.runnr = runnr

    def write_to_grf(self):
        """
        Write the entire tree to a Giant Root File.
        """
        fileName = Config().grf_file_path
        mode = "update" if os.path.isfile(fileName) else "create"

        writer = GRFIO(fileName, mode = mode, branches = self.getBranches())
        writer.fill(self.getWritableResults())
        writer.write()
        writer.close()

    def getBranches(self):
        branches = super(RootCombiner, self).getBranches()
        branches['runnr'] = 'UInt_t'
        return branches

    def isRelevant(self):
        return True # Root is always relevant

    def getWritableResults(self):
        results = super(RootCombiner, self).getWritableResults()
        results["runnr"] = self.runnr
        return results

class LeafCombiner(Combiner):
    FUNC_KEY = "Function"
    ARG_KEY  = "Argument"

    def __init__(self, name, desc_dict, eval_dict, data_file_path, ref_file_path):
        super(LeafCombiner, self).__init__(name, desc_dict, data_file_path, ref_file_path)

        if not self.getName() in eval_dict:
            raise ComparisonFunctionNotFoundInEvalDictException(self.getName())
        myEval = eval_dict[self.getName()]

        self.__setCompareFunc(myEval)
        self.__path = self.desc_dict["path"]

    def __str__(self):
        """Get a string representation of this object."""
        log_string = self.stringRep()
        log_string.append("Combiner's histogram: {}".format(self.desc_dict["path"]))
        return "\n".join(log_string)

    def getPath(self):
        """
        Get the path of this LeafCombiner, that is, the path in the data and
        reference ROOT files where the relevant histogram is located.
        """
        return self.__path

    def __setCompareFunc(self, myEval):
        """
        Read the compare function and its arguments from the eval dictionary.
        """
        compClass = myEval[LeafCombiner.FUNC_KEY]
        function = compClass.compare
        if not callable(function):
            raise ComparisonFunctionNotCollableException(self.getName(), function.__name__)
        self.compare_func = function

        arg = ""
        if LeafCombiner.ARG_KEY in myEval:
            arg = myEval[LeafCombiner.ARG_KEY]
        self.compare_arg = arg if arg else ""

    def __getHistos(self, data_file, ref_file):
        """Get histograms from files."""
        hist_name = self.getPath()
        data_hist = None
        ref_hist = None

        if data_file:
            data_hist = data_file.Get(hist_name)
        if not data_hist:
            raise HistogramNotFoundException(self.getName(), hist_name)

        if ref_file:
            ref_hist = ref_file.Get(hist_name)
        if not ref_hist:
            raise HistogramNotFoundException(self.getName(), hist_name)

        return data_hist, ref_hist

    def evaluate(self):
        """
        Evaluate this leaf by performing a check on the data histogram, either
        directly or by comparing it to the corresponding reference histogram.
        """
        if self.results:
            return

        data_file = ROOT.TFile(self.data_file_path)
        ref_file = ROOT.TFile(self.ref_file_path)
        data_file, ref_file = detect_zombies(data_file, ref_file)

        data_hist, ref_hist = self.__getHistos(data_file, ref_file)
        self.results = self.compare_func(data_hist, ref_hist, self.compare_arg)
        lvl = self.getLevel()
        if lvl == ERROR_LEVELS.WARNING:
            self.warnings = 1
        elif lvl == ERROR_LEVELS.ERROR:
            self.errors = 1

        #self.results = self.update_with_expected_state(result)  # TODO Uncomment when update_with-expected_state will be implemented
        #self.summary_data = self.get_summary_data_from_hist(data_hist)  # TODO Uncomment when get_summary_data_from_hist will be implemented

        data_file.Close()
        ref_file.Close()

    def isRelevant(self):
        return True # Leaf nodes are always relevant
