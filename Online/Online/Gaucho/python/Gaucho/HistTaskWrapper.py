""" This module exposes a class that wraps around the HistTask C++ class from
Gaucho.

    Authors: Alex Pearce <alex.pearce@cern.ch>
             Kevin Dungs <kevin.dungs@cern.ch>
"""
import ROOT
ROOT.gSystem.Load("libGaucho.so")
ROOT.gInterpreter.Declare('#include "Gaucho/HistTask.h"')

# ROOT "typedefs"
StdString = ROOT.std.string
VectorOfString = ROOT.vector('string')
VectorOfTObjectPtr = ROOT.vector('TObject*')


class HistTaskWrapper(object):
    """ This class wraps around the HistTask C++ class from Gaucho.
    It exposes all of the methods in a pythonic way.
    """
    @classmethod
    def _cast_TObject(cls, tobj):
        """ """
        cls_map = {
            ROOT.TH1D.Class(): ROOT.TH1D,
            ROOT.TH2D.Class(): ROOT.TH2D,
            ROOT.TH3D.Class(): ROOT.TH3D,
            ROOT.TProfile.Class(): ROOT.TProfile
        }
        cls = cls_map.get(tobj.IsA())
        if cls is None:
            raise Exception('Obtained TObject of unsupported type.')
        return cls(tobj)

    @classmethod
    def _task_list(cls, dns):
        """ Obtains a task list from DIM as `std::vector<std::string>`.
        This is only internally by the classmethod `task_list` which returns a
        Python list of Python strings.
        """
        locations = VectorOfString()
        err = ROOT.HistTask.TaskList(dns, locations)
        if err != 0:
            raise Exception('Could not obtain task list from DIM.')
        return locations

    @classmethod
    def task_list(cls, dns):
        """ Obtains task list from DIM. """
        return [t for t in cls._task_list(dns)]

    def __init__(self, task_name, dns):
        """ Sets up a new wrapper object for a given task name and dns that can
        then obtain the histograms associated with the task.
        """
        self._dns = dns
        self._task_name = task_name
        self._hist_task = ROOT.HistTask(task_name, dns)
        # These need to be set, otherwise movefromdim segfaults
        ROOT.TH1D.SetDefaultSumw2()
        ROOT.TH2D.SetDefaultSumw2()
        ROOT.TProfile.SetDefaultSumw2()

    def _directory(self):
        """ Obtains the histogram names for the task associated with this
        instance from DIM and returns a `std::vector<std::string>`.
        This is used internally by the `directory` method which unwraps the
        result in a Python list of Python strings.
        """
        dir_ = VectorOfString()
        err = self._hist_task.Directory(dir_)
        if err != 0:
            raise Exception('Could not obtain directory from DIM.')
        return dir_

    def directory(self):
        """ Obtains the histogram names for the task associated with this
        instance.
        """
        return [n for n in self._directory()]

    def _histograms(self, histogram_names=None):
        """ Obtains histograms of given names for task associated with this
        instance as a `std::vector<TObject*>`.
        This is used internally by the `histograms` method which unwraps the
        result into a Python list of the correct type (`TH1D`, `TH2D`, `TH3D`,
        or `TProfile`).
        """
        if histogram_names is None:
            histogram_names = self._directory()
        hists = VectorOfTObjectPtr()
        err = self._hist_task.Histos(histogram_names, hists)
        if err != 0:
            raise Exception('Could not obtain histograms from DIM.')
        return hists

    def histograms(self, histogram_names=None):
        """ Obtains histograms of given names for task associated with this
        instance.

        Input is a Python list of Python strings.
        """
        hist_names_vec = None
        if histogram_names is not None:
            # Convert list of strings to std::vector<std::string>
            hist_names_vec = VectorOfString()
            for hn in histogram_names:
                hist_names_vec.push_back(StdString(hn))
        return [self._cast_TObject(t)
                for t in self._histograms(hist_names_vec)]
