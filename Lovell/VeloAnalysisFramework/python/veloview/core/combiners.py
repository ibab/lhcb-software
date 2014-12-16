"""This module will define containers used for the project. They will store histograms and their corresponding scores."""

import ROOT

from .interface import ComparisonFunction
from .score_manipulation import Score, ERROR_LEVELS
from .evaluation_dictionary import FUNC_KEY, ARG_KEY
from .exceptions import DescriptionDictionaryKeyException, \
    ComparisonFunctionNotFoundInEvalDictException, ComparisonFunctionNotCollableException


EXP_STATES_KEYS_BRANCH = ["weight", "minWW", "minWE", "minEW", "minEE"]  # keys that are expected in branch combiners
EXP_STATES_KEYS_LEAF = ["weight", "maxError", "maxWarning", "path"]  # keys that are expected in leaf combiners
WARNING_LIMITS = ["minWW", "minWE"]  # keys for warning limits
ERROR_LIMITS = ["minEW", "minEE"]  # keys for error limit
MASTER_COMBINER_NAME = "MasterCombiner"  # name of the root combiner


class Combiner(object):
    """This class is responsible for analysing data and writing an evaluation summary to Giant Root File"""

    def __init__(self, desc_dict, eval_dict, data_file_path, ref_file_path="", name=MASTER_COMBINER_NAME):
        """Parameters:
            desc_dict       :   a combiner description dictionary (which describes the tree structure
            eval_dict       :   an evaluation dictionary that holds information about analysis method for each histogram
            data_file_path  :   a path to a root file with data to analyse
            ref_file_path   :   a path to a root file that serves as reference
            name            :   name of a combiner (must be consisted with desc and eval dicts"""
        self.name = name
        self.score = None
        self.state = None
        self.warnings = 0
        self.errors = 0
        self.children = []
        self.eval_dict = eval_dict
        self.data_file_path = data_file_path
        self.ref_file_path = ref_file_path
        self.compare_func = None
        self.results = None
        self.summary_data = None

        self.desc_dict = self.clip_desc_dict(desc_dict)
        self.children = self.create_children()
        self.compare_func, self.compare_arg = self.get_comparison_data()

    def __str__(self):
        """Creates a string representation of a combiner for logging purposes"""

        def pop_combiners(dict_, iterable):
            temp_dict = dict(dict_)
            if iterable:
                for iter_ in iterable:
                    temp_dict.pop(iter_)
            return temp_dict

        def add_main_branches_scores(children, log_string):
            log_string.append("Combiner's Main Branches:")
            for child in children:
                log_string.append("\tBranch: {} \tScore: {} \tLevel: {} \tErrors: {} \tWarning: {} ".format(child.name,
                                                                                                child.results["score"],
                                                                                                child.results["lvl"],
                                                                                                child.errors,
                                                                                                child.warnings))

        log_string = ["*********************************************************"]
        if self.name == MASTER_COMBINER_NAME:
            log_string.append("*********************************************************")

        log_string.append("Combiner's name: {}".format(self.name))
        children_names = [child.name for child in self.children]
        log_string.append("Combiner's description: {}".format(pop_combiners(self.desc_dict, children_names)))
        log_string.append("Combiner's children: {}".format(children_names))
        log_string.append("Combiner's evaluation results: {}".format(self.results))

        if self.children:
            log_string.append("Combiner's errors: {} and warnings: {}".format(self.get_nr_of_errors(),
                                                                              self.get_nr_of_warnings()))
            if self.name == MASTER_COMBINER_NAME:
                add_main_branches_scores(self.children, log_string)

            for child in self.children:
                log_string.append(str(child))
            log_string.append("*********************************************************")

        else:
            log_string.append("Combiner's histogram: {}".format(self.desc_dict["path"]))

        return "\n".join(log_string)

    def clip_desc_dict(self, desc_dict):
        """Clips the description dictionary so that all combiners don't need to store the entire dict in their memory"""
        try:
            return desc_dict[self.name]
        except KeyError:
            raise DescriptionDictionaryKeyException(self.name, desc_dict.keys())

    def create_children(self):
        """Creates children for branch combiners"""
        return [Combiner(self.desc_dict, self.eval_dict, self.data_file_path, self.ref_file_path, combiner_name)
                for combiner_name in self.desc_dict.keys()
                if "Combiner" in combiner_name and combiner_name != self.name]

    def get_comparison_data(self):
        """Gets a comparison function for leaf combiners"""
        if not self.children:
            if self.name in self.eval_dict:
                return self.get_compare_func(), self.get_compare_arg()
            else:
                raise ComparisonFunctionNotFoundInEvalDictException(self.name)
        else:
            return None, None

    def get_compare_func(self):
        function = self.eval_dict[self.name][FUNC_KEY].compare
        if not callable(function):
            raise ComparisonFunctionNotCollableException(self.name, function.__name__)
        else:
            return function

    def get_compare_arg(self):
        combiner_ed = self.eval_dict[self.name]
        arg = None
        if ARG_KEY in combiner_ed:
            arg = combiner_ed[ARG_KEY]
        return arg

    def evaluate(self):
        """Evaluates leaves and branches"""
        if self.results:
            return

        elif self.children:
            for child in self.children:
                child.evaluate()
            self.results = self.calc_results()

        else:
            data_file = ROOT.TFile(self.data_file_path)
            ref_file = ROOT.TFile(self.ref_file_path)
            data_file, ref_file = self.detect_zombies(data_file, ref_file)

            data_hist, ref_hist = self.get_histos(data_file, ref_file)
            self.results = self.compare_func(data_hist, ref_hist, self.compare_arg)
            #self.results = self.update_with_expected_state(result)  # TODO Uncomment when update_with-expected_state will be implemented
            #self.summary_data = self.get_summary_data_from_hist(data_hist)  # TODO Uncomment when get_summary_data_from_hist will be implemented

            data_file.Close()
            ref_file.Close()

    @staticmethod
    def detect_zombies(file1, file2):
        """Detect root zombie files"""
        if file1.IsZombie():
            file1 = None
        if file2.IsZombie():
            file2 = None

        return file1, file2

    def get_histos(self, data_file, ref_file):
        """Gets histograms from files"""
        data_hist = None
        ref_hist = None

        if data_file:
            data_hist = data_file.Get(self.desc_dict["path"])
        if ref_file:
            ref_hist = ref_file.Get(self.desc_dict["path"])

        return data_hist, ref_hist

    @staticmethod
    def get_summary_data_from_hist(hist):
        """Gets summary data from hist"""
        pass  # TODO Discuss how to implement this, possibly it's not a good place for that function (-> eval_dict)

    @staticmethod
    def update_with_expected_state(result):
        """Updates result data with expected states"""
        pass  # TODO Discuss how to implement this and find a way of configuring a current expected state

    def calc_results(self):
        """Calculates combiner's score and level"""
        score = Score(0)
        summed_weights = self.calc_summed_weights()

        for child in self.children:
            score += child.results["score"] * child.desc_dict["weight"] / summed_weights
            if child.children:
                self.warnings += child.warnings
                self.errors += child.errors
            else:
                if child.results["lvl"] == ERROR_LEVELS.WARNING:
                    self.warnings += 1
                elif child.results["lvl"] == ERROR_LEVELS.ERROR:
                    self.errors += 1

        lvl = self.calc_lvl()
        function = ComparisonFunction()

        return function.create_final_dict(score, lvl)

    def calc_summed_weights(self):
        """Calculates summed weight"""
        summed_weights = 0
        for child in self.children:
            summed_weights += child.desc_dict["weight"]
        return summed_weights

    def get_nr_of_warnings(self):
        """Returns number of warnings for branch combiners (errors are also counted as warnings)"""
        return self.warnings + self.errors

    def get_nr_of_errors(self):
        """returns number of error for branch combiners"""
        return self.errors

    def calc_lvl(self):
        """Calculates lvl depending on the description dictionary/expected state"""
        mapped_warnings = self.map_number_to_lvl(self.get_nr_of_warnings(), WARNING_LIMITS)
        mapped_errors = self.map_number_to_lvl(self.get_nr_of_errors(), ERROR_LIMITS)

        if mapped_errors == ERROR_LEVELS.ERROR or mapped_warnings == ERROR_LEVELS.ERROR:
            return ERROR_LEVELS.ERROR
        elif mapped_errors == ERROR_LEVELS.WARNING or mapped_warnings == ERROR_LEVELS.WARNING:
            return ERROR_LEVELS.WARNING
        else:
            return ERROR_LEVELS.OK

    def map_number_to_lvl(self, number, limit):
        """Maps number of warnings/errors to a specific state"""
        if 0 <= number < self.desc_dict[limit[0]]:
            ans = ERROR_LEVELS.OK
        elif self.desc_dict[limit[0]] <= number < self.desc_dict[limit[1]]:
            ans = ERROR_LEVELS.WARNING
        else:
            ans = ERROR_LEVELS.ERROR

        return ans

    def write_to_grf(self, writer=None):
        """Writes combiner's summary data to a GRF"""
        if not self.results:
            self.evaluate()
        if not writer:
            writer = GRFWriter()
        if self.children:
            for child in self.children:
                child.write_to_grf(writer)
            writer.write()
        else:
            writer.accumulate(self)

    def get_combiner_summary(self):
        """Returns summary information (score, lvl, summary data)"""
        if not self.results:
            self.evaluate()
        if self.summary_data:
            dict_ = dict(self.results)
            dict_.update(self.summary_data)
            return dict_
        else:
            return self.results


class GRFWriter(object):
    """A container that gathers information to be written into a GRF and writes it"""

    def __init__(self):
        self.data_to_write = {}

    def accumulate(self, combiner):
        """Accumulates data from each combiner"""
        self.data_to_write[combiner.name] = combiner.get_combiner_summary()

    def write(self):
        """Writes accumulated data into GRF"""
        for key, value in self.data_to_write.iteritems():
            raise NotImplementedError("This feature will be implemented in the near future")
