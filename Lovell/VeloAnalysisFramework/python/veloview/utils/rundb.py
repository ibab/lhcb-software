import json
import time
import urllib

# String constants describing magnet polarity
UP = 'up'
DOWN = 'down'
OFF = 'off'
# Enumerate all possible polarity states
POLARITIES = (UP, DOWN, OFF)

# Template URL for retrieving response from LHCb RunDB JSON API
# Template parameter {0}: Run number, e.g. 123987
RUNDB_API_URL = 'http://lbrundb.cern.ch/api/run/{0}/'


class RunDB(object):
    """Query the LHCb Run Database, RunDB, for run information.

    Uses the RunDB JSON API to retrieve information.  For information on the
    RunDB JSON API and the format of the response, see
    https://lbtwiki.cern.ch/bin/view/Online/RunDataBase.
    """
    def __init__(self, url=RUNDB_API_URL):
        """Initialise the object with template URL.

        url argument must have a single template argument that can be filled
        with the integer run number, e.g.

            >>> RunDB(url='http://example.com/run/{0}')

        If any error occurs during any query, such as the RunDB being
        uncontactable or an invalid run is passed (not found in the RunDB),
        None is returned.
        """
        self._url = url

    def query(self, run):
        """Return dictionary of run information.

        If there is any failure, such as the RunDB being uncontactable, or the
        run is invalid (i.e. not found in the RunDB), None is returned.
        """
        request = urllib.urlopen(self._url.format(run))
        # Make sure the request was OK, not e.g. 404 not found
        if request.getcode() != 200:
            return None
        try:
            d = json.load(request)
        except ValueError:
            # Couldn't decode the response
            return None
        return d

    def polarity(self, run):
        """Return the polarity of the run, one of UP, DOWN, or OFF constants.

        Example polarity check:

            >>> from veloview.utils import rundb
            >>> polarity = rundb.RunDB().run_polarity(149715)
            >>> if polartiy == rundb.UP:
            ...     # Do something
            ... else polarity == rundb.DOWN:
            ...     # Do something else

        If the RunDB query fails, None is returned.
        """
        response = self.query(run)
        # Check for null responses
        if not response:
            return None
        try:
            polarity = response['magnetState']
        except KeyError:
            # Polarity not included in JSON response
            return None
        # API returns UP for magnet up, DOWN for magnet down, OFF for off
        # Map these to our string constants and return the appropriate one
        return {
            'UP': UP,
            'DOWN': DOWN,
            'OFF': OFF
        }[polarity]

    def endtime(self, run):
        """Return epoch timestamp of the end time of the run. Result is a
        nonnegative number, or None of the query fails.
        """
        response = self.query(run)
        if not response:
            return None
        try:
            endtime = response['endtime']
        except KeyError:
            # End time not included in JSON response
            return None

        pattern = '%Y-%m-%dT%H:%M:%S'
        return long(time.mktime(time.strptime(endtime, pattern)))
