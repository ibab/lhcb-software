# coding=utf-8
"""This module provides functions for trending information"""

from functools import wraps
from logging import getLogger, debug, warning
logger = getLogger(__name__)


# decorator for trending functions
def check_hist(comparefn):
    """Check data histogram"""
    @wraps(comparefn)
    def wrapper(*args, **kwargs):
        # NB: change to args[1] if we ever move to a class
        # implementation, since in that case, arg[0] would be self
        if args[0]:
            return comparefn(*args, **kwargs)
        else:
            raise ValueError('Bad data histogram: {}'.format(args[0]))
    return wrapper


# class Trending(object):
#     """Base class for trending functions"""

#     def __call__(self, hist):
#         """Reimplement in derived class"""
#         return NotImplemented


@check_hist
def Median(hist):
    """Get median"""
    return dict(median = NotImplemented)


@check_hist
def Mean(hist):
    """Get mean"""
    return dict(mean = hist.GetMean())


@check_hist
def Variance(hist):
    """Get variance (RMS)"""
    return dict(var = hist.GetRMS())


@check_hist
def MPV(hist):
    """Get most probable value"""
    return dict(mpv = NotImplemented)


@check_hist
def Landau(hist):
    """Get Landau fit parameters"""
    return dict(landau = NotImplemented)
