"""This module is responsible for score manipulations"""

from .exceptions import AddingScoreException, ScoreAssignmentException, WeightedScoreException
from ..utils.utils import enum

# definition of the error levels for the project
ERROR_LEVELS = enum("OK", "WARNING", "ERROR")

class Score(object):
    """A class that will hold a score (integer value between 0 and 100). Scores will be later assigned to histograms."""

    def __init__(self, val):
        if isinstance(val, int):
            val = float(val)

        if isinstance(val, float):
            if self.isInRange(val):
                self.value = val
                return

        raise ScoreAssignmentException

    def __add__(self, other):
        try:
            temp_val = self.value + other.value
        except AttributeError:
            if self.isNumeric(other):
                temp_val = self.value + other
            else:
                raise AddingScoreException

        if self.isInRange(temp_val):
            return Score(temp_val)
        else:
            raise AddingScoreException

    def __iadd__(self, other):
        try:
            temp_val = self.value + other.value
        except AttributeError:
            if self.isNumeric(other):
                temp_val = self.value + other
            else:
                raise AddingScoreException

        if self.isInRange(temp_val):
            self.value = temp_val
            return self
        else:
            raise AddingScoreException

    def __mul__(self, other):
        temp_val = self.value * other
        if self.isInRange(temp_val):
            return Score(temp_val)
        else:
            raise WeightedScoreException

    def __imul__(self, other):
        temp_val = self.value * other
        if self.isInRange(temp_val):
            self.value = temp_val
            return self
        else:
            raise WeightedScoreException

    def __div__(self, other):
        temp_val = self.value / other
        if self.isInRange(temp_val):
            return Score(temp_val)
        else:
            raise WeightedScoreException

    def __idiv__(self, other):
        temp_val = self.value / other
        if self.isInRange(temp_val):
            self.value = temp_val
            return self
        else:
            raise WeightedScoreException

    def __cmp__(self, other):
        return cmp(self.value, other.value)

    def __repr__(self):
        return "%.2f%%" % self.value

    def __str__(self):
        return repr(self)

    def __call__(self):
        return self.value

    def __int__(self):
        return int(self.value)

    def __float__(self):
        return float(self.value)

    @staticmethod
    def isInRange(value):
        return Score.isNumeric(value) and 0.0 <= value <= 100.0

    @staticmethod
    def isNumeric(value):
        return isinstance(value, (int, long, float))
