#!/usr/bin/env python
"""Return plots for a given run in the format expected by the offline GUI.

A run number is invalid if it is not present in the run list.
A plot name is invalid if it is not present in the run file.
A sensor number is invalid if is not in {0, ..., 42, 64, ..., 106}.
The reference plot is returned along with the nominal plot, if it can be found, 
as an array of length 2.
If the refernce plot cannot be found, null is used as a placeholder.
It is up to the caller to decide if the two returned plots are consistent with 
one another.
If any invalid arguments are supplied, 1 is returned and the program exits.
"""
# This is due to what appears to be a ROOT bug, where doing `import ROOT`
# prints a control sequence to stdout on xterm terminal types
import os
import sys
import argparse

from veloview.config import Config


def create_parser():
    parser = argparse.ArgumentParser(
        description=__doc__.split("\n")[0],
        epilog="\n".join(__doc__.split("\n")[1:]))
    parser.add_argument("run", type=int, help="Run number")
    parser.add_argument("plot", type=str, help="Plot name in run file")
    parser.add_argument("sensor", type=int, nargs="?", default=0,
                        help="Sensor number")
    parser.add_argument("--run-data-dir", default=Config().run_data_dir,
                        help="Directory to search for run list and data")
    parser.add_argument("--no-reference", action="store_true",
                        help="Omit the reference plot")
    return parser


def exit_with_error(msg):
    """Print the failure JSON response with msg and return exit code 1."""
    import json
    sys.stdout.write(json.dumps({'success': False, 'message': msg}))
    sys.exit(1)


def retrieve_run_view_plot(run, plot, sensor, noreference, run_data_dir):
    import json
    from veloview.runview import plots, response_formatters, utils

    # Change the run data directory to the user-specified one
    Config().run_data_dir = run_data_dir

    # Check all arguments have valid values
    err = False
    if not utils.valid_run(run):
        err = True
        msg = "Invalid run number provided: {0}".format(run)
    if not utils.valid_sensor(sensor):
        err = True
        msg = "Invalid sensor number provided: {0}".format(sensor)
    if err:
        exit_with_error(msg)

    # Format the plot name with the sensor number
    # str.format will work even with no format specifiers in the string
    plot = plot.format(sensor)

    # Try to the get the plot object, formatting it to JSON
    try:
        if noreference:
            # Return a 2-tuple to be consistent with the nominal+reference case
            response = (plots.get_run_plot(
                plot, run,
                reference=False,
                formatter=response_formatters.json_formatter
            ), json.dumps(None))
        else:
            response = plots.get_run_plot_with_reference(
                plot, run, formatter=response_formatters.json_formatter
            )
    except KeyError, e:
        err = True
        msg = (
            "Invalid plot name provided: {0}. "
            "Exception caught: {1}."
        ).format(plot, e)
    except TypeError, e:
        err = True
        msg = (
            "Cannot handle plot type for plot: {0}. "
            "Exception caught: {1}."
        ).format(plot, e)
    except Exception, e:
        # We finally catch every, to make sure clients have an error message to
        # show
        err = True
        msg = (
            "An unidentified exception was raised fetching plot: {0}. "
            "Please contact the maintainer with the information."
            "Exception: {1}"
        ).format(plot, e)

    if err:
        exit_with_error(msg)

    # Return a JSON list containing the two plot objects
    return response


def main():
    parser = create_parser()
    args = parser.parse_args()
    retrieve_run_view_plot(args.run, args.plot, args.sensor, args.no_reference,
                           args.run_data_dir)


if __name__ == "__main__":
    main()
    sys.exit(0)
