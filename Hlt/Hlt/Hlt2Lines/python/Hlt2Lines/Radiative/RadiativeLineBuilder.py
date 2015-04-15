#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   RadiativeLineBuilder.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   09.04.2015
# =============================================================================
"""Base class for radiative lines."""


class RadiativeLineBuilder(object):
    @staticmethod
    def get_stages(_):
        raise NotImplementedError()

    @staticmethod
    def get_cuts():
        raise NotImplementedError()

    @staticmethod
    def get_l0():
        return {}

    @staticmethod
    def get_hlt1():
        return {}


# EOF
