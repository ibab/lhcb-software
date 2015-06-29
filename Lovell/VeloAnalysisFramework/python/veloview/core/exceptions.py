"""This module will define project specific exceptions"""


# Exceptions for inheritance
class CombinerExceptionWithName(Exception):
    def __init__(self, name):
        self.name = name

class CombinerExceptionWithNameAndOtherInfo(Exception):
    def __init__(self, name, other):
        self.name = name
        self.other = other

# Score exceptions
class ScoreAssignmentException(Exception):
    def __str__(self):
        return "A score must be an integer value between 0 and 100"


class AddingScoreException(Exception):
    def __str__(self):
        return "Added scores sum to more than 100%"


class WeightedScoreException(Exception):
    def __str__(self):
        return "A score combined with its weight must be a value between 0 and 100"


# Container exceptions
class ComparisonFunctionNotFoundInEvalDictException(CombinerExceptionWithName):
    def __str__(self):
        return "A comparison function could not been found in the evaluation dictionary for the combiner {}".format(self.name)


class ComparisonFunctionNotCollableException(CombinerExceptionWithNameAndOtherInfo):
    def __str__(self):
        return "The comparison function {} for the combiner {} is not a function".format(self.other, self.name)


class WeightContainerLackingWeightException(Exception):
    def __str__(self):
        return "Weight needs to be assigned to the container before calculating its score"


class DescriptionDictionaryKeyException(CombinerExceptionWithNameAndOtherInfo):
    def __str__(self):
        return "Combiner's name: {} could not be found in the clipped combiners description dictionary ({})".format(self.name, self.other)


class CombinerIdentificationException(CombinerExceptionWithNameAndOtherInfo):
    def __str__(self):
        return "Combiner: {} could not be assign appropriate expected state values ({})".format(self.name, self.other)


class HistogramNotFoundException(CombinerExceptionWithNameAndOtherInfo):
    def __str__(self):
        return "Combiner {} can't find expected histogram {}".format(self.name, self.other)
