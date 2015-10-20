""" Defines a wrapper around the TrendingTool that exposes its reading-related
functionality in a pythonic way.

At this point, it just exposes the minimal needed functionality that is needed
for the presenter.

    Author: Alex Pearce <alex.pearce@cern.ch>
            Kevin Dungs <kevin.dungs@cern.ch>
"""

import GaudiPython as gp

# TODO: Is a hack
# This becomes unecessary once the ITrendingTool has a proper dictionary
# associated to it.
gp.ROOT.gInterpreter.Declare('#include "Trending/ITrendingTool.h"')


class CouldNotLoadTagsException(Exception):
    pass


class CouldNotSelectTagException(Exception):
    pass


class TrendReader(object):
    """ Read trend files and obtain their precious contents.
    
    For the moment, this wraps around the `TrendingTool` in a very hacky way.
    Ideally at some point in the future, the necessary functionality should be
    moved from the Gaudi tool into a separate C++ class that can be used from
    Python without a hassle.
    """
    def __init__(self, filename, name=None):
        """ Create a new `TrendingTool` and open the specified file for
        reading.
        """
        identifier = 'TrendingTool'
        if name is not None:
            identifier += '/{}'.format(name)
        self.ttool = gp.AppMgr().toolsvc().create(identifier,
                                                  interface='ITrendingTool')
        self.ttool.openRead(filename)

    def _tags(self):
        """ Read available tags in the file.

        This method is used internally by the `tags()` method which turns the
        `std::vector<std::string>` into a python list of python strings.
        """
        from ROOT import vector
        tags = vector('string')()
        if not self.ttool.tags(tags):
            raise CouldNotLoadTagsException()
        return tags

    def tags(self):
        """ Read available tags in the file. """
        return [t for t in self._tags()]

    def _itervalues(self):
        """ A generator that iterates over the current set of data specified
        via `ttool.select`. 
        
        This is used internally by the `get_data()` method.
        """
        from array import array
        time = array('I', [0])
        value = array('f', [0])
        while self.ttool.nextValue(time, value):
            yield time[0], value[0]

    def get_data(self, tag, start_time=0, end_time=0):
        """ Obtain data for given tag.
        
        Start time and end time can be specified optionally.
        """
        if not self.ttool.select(start_time, end_time, tag):
            raise CouldNotSelectTagException()
        return [(t, v) for t, v in self._itervalues()]
