#!/usr/bin/env python
"""Return a plot for a given run in the format expected by the offline GUI.

A run number is in invalid if it is not present in the run list.
A plot name is invalid if it is not present in the run file.
A sensor number is invalid if is not in {0, ..., 42, 64, ..., 106}.
If any invalid arguments are supplied, 1 is returned and the program exits.
"""
import sys
import argparse


def create_parser():
    parser = argparse.ArgumentParser(
        description=__doc__.split("\n")[0],
        epilog="\n".join(__doc__.split("\n")[1:]))
    parser.add_argument("run", type=int, help="Run number")
    parser.add_argument("plot", type=str, help="Plot name in run file")
    parser.add_argument("sensor", type=int, nargs="?", default=0,
                        help="Sensor number")
    parser.add_argument("--reference", action="store_true",
                        help="Return reference plot for the given arguments")
    return parser


def retrieve_run_view_plot(run, plot, sensor, reference):
    from veloview.runview import plots, response_formatters, utils

    # Check all arguments have valid values
    if not utils.valid_run(run):
        sys.stderr.write("Invalid run number provided: {0}".format(run))
        sys.exit(1)
    if not utils.valid_sensor(sensor):
        sys.stderr.write("Invalid sensor number provided: {0}".format(sensor))
        sys.exit(1)

    # Format the plot name with the sensor number
    # str.format will work even with no format specifiers in the string
    plot = plot.format(sensor)

    # Try to the get the plot object
    try:
        plot_obj = plots.get_run_plot(plot, run, reference=reference)
    except KeyError, e:
        sys.stderr.write("Invalid plot name provided: {0}".format(plot))
        sys.stderr.write("Exception caught: {0}".format(e))
        sys.exit(1)

    # Try to format the object in to JSON
    try:
        response = response_formatters.json_formatter(plot_obj)
    except TypeError, e:
        sys.stderr.write("Cannot handle plot type for plot {0}".format(plot))
        sys.stderr.write("Exception caught: {0}".format(e))

    sys.stdout.write(response)


def main():
    parser = create_parser()
    args = parser.parse_args()
    retrieve_run_view_plot(args.run, args.plot, args.sensor, args.reference)


if __name__ == "__main__":
    main()
    sys.exit(0)
