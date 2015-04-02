#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Query and modify the VELO reference run database.

See veloview.runview.reference_database for documentation on the reference
database.

Run boundaries
--------------

To look at a sorted list of the run boundaries, do

```shell
$ velo_reference_db.py run_boundaries
```

To see the run boundaries a run falls in to, do

```shell
$ velo_reference_db.py run_boundaries RUN_NUMBER
```

If a run resides in the highest range of validity, the upper bound will be
Infinity.
If a run resides in the lowest range of validaity, the lower bound will be 0.

To modify the list of run boundaries and their reference runs, do

```shell
# Add a boundary at RUN_NUMBER with magnet up and down reference runs
$ velo_reference_db.py run_boundaries --add RUN_NUMBER \
        --up-run UP_RUN  --down-run DOWN_RUN

# Update the magnet up reference run for the boundary at RUN_NUMBER
$ velo_reference_db.py run_boundaries --update RUN_NUMBER --up-run NEW_UP_RUN

# Edit reference runs for both polarities for the boundary at RUN_NUMBER
$ velo_reference_db.py run_boundaries --update RUN_NUMBER \
        --up NEW_UP_RUN --down NEW_DOWN_RUN

# Remove a boundary at RUN_NUMBER
$ velo_reference_db.py run_boundaries --remove RUN_NUMBER
```

Boundary plots
--------------

Plots can be defined on a boundary to override specific plots within the run
range the boundary is the lower limit of.
These boundary plots can be managed in a similar way to boundary runs.

```shell
# View all plots defined on the boundary at RUN_NUMBER
$ velo_reference_db.py boundary_plots RUN_NUMBER

# Add a boundary plot called PLOT at RUN_NUMBER with magnet up and down
# reference runs
$ velo_reference_db.py boundary_plots --add RUN_NUMBER PLOT \
        --up-run UP_RUN  --down-run DOWN_RUN

# Update the magnet up reference run for the boundary plot PLOT at RUN_NUMBER
$ velo_reference_db.py boundary_plots --update RUN_NUMBER PLOT \
        --up-run NEW_UP_RUN

# Edit reference runs for both polarities for the boundary plot PLOT at
# RUN_NUMBER
$ velo_reference_db.py boundary_plots --update RUN_NUMBER PLOT \
        --up NEW_UP_RUN --down NEW_DOWN_RUN

# Remove a boundary plot PLOT at RUN_NUMBER
$ velo_reference_db.py boundary_plots --remove RUN_NUMBER PLOT
```


Reference runs
--------------

To see the run number of the reference file for a run, do

```shell
$ velo_reference_db.py dereference --run RUN_NUMBER
```

To see the run number of the reference file for a plot from a run, do

```shell
$ velo_reference_db.py dereference --run RUN_NUMBER --plot PLOT_NAME
```
"""
import argparse
import sys
import json

import ROOT
# Stop ROOT processing command line arguments (breaks argparse --help)
# https://root.cern.ch/phpBB3/viewtopic.php?f=14&t=15601
ROOT.PyConfig.IgnoreCommandLineOptions = True

from veloview.core import config as veloview_config
from veloview.runview.reference_database import (
    ReferenceDatabase,
    InvalidBoundary,
    InvalidPlot
)
from veloview.utils.rundb import RunDB


def exit_success():
    sys.exit(0)


def exit_failure(code=1):
    sys.exit(code)


def fatal(msg):
    pjson(ret_dict(failure=msg))
    exit_failure()


def pjson(obj):
    print json.dumps(obj)


def ret_dict(success=None, failure=None):
    """Construct standard dictionary returned on cmd success/failure.

    Example usage:

        >>> ret_dict(success='Yes!')
        {'success': True, 'message': 'Yes!'}
        >>> ret_dict(failure='No!')
        {'success': False, 'message': 'No!'}

    If both success and failure are not None, success will be set to False,
    and the failure argument will be used as the message

        >>> ret_dict(success='Yes!', failure='No!')
        {'success': False, 'message': 'No!'}
    """
    return {'success': failure is None, 'message': failure or success}


def add_boundary_run(db, boundary, up_run=None, down_run=None):
    """Add a boundary run to the DB with magnet up and down reference runs.

    Even though an up and down run are required to add a boundary, we set
    defaults to None so that this method can take care of missing arguments.
    """
    if not (up_run and down_run):
        fatal('Must specify both --up-run and --down-run to add a boundary.')
    db.add_run_boundary(boundary, up_run, down_run)
    return ret_dict(success='Added boundary {0}'.format(boundary))


def remove_boundary_run(db, boundary):
    """Remove a boundary run from the DB."""
    db.remove_run_boundary(boundary)
    return ret_dict(success='Removed boundary {0}'.format(boundary))


def update_boundary_run(db, boundary, up_run=None, down_run=None):
    """Update a boundary run in the DB."""
    if not (up_run or down_run):
        fatal('Must specify at least one --up-run or --down-run.')
    db.update_run_boundary(boundary, up_run, down_run)
    return ret_dict(success='Updated boundary {0}'.format(boundary))


def find_boundary_runs(db, run):
    """Return 2-tuple of boundary runs surrounding run."""
    boundaries = db.surrounding_boundaries(run)
    ret = ret_dict(success=''.format(run))
    ret['data'] = boundaries
    return ret


def add_boundary_plot(db, boundary, plot, up_run=None, down_run=None):
    """Add a boundary run to the DB with magnet up and down reference runs.

    Even though an up and down run are required to add a boundary, we set
    defaults to None so that this method can take care of missing arguments.
    """
    if not (up_run and down_run):
        fatal('Must specify both --up-run and --down-run to add a boundary.')
    try:
        db.add_boundary_plot(boundary, plot, up_run, down_run)
    except InvalidPlot as e:
        msg = 'Could not add plot {0} at boundary {1}: {2}'.format(
            plot, boundary, e.message
        )
        return ret_dict(failure=msg)
    return ret_dict(success='Added plot {0} to boundary {1}'.format(
        plot, boundary
    ))


def remove_boundary_plot(db, boundary, plot):
    """Remove a boundary run from the DB."""
    db.remove_boundary_plot(boundary, plot)
    return ret_dict(success='Removed plot {0} at boundary {1}'.format(
        plot, boundary
    ))


def update_boundary_plot(db, boundary, plot, up_run=None, down_run=None):
    """Update a boundary run in the DB."""
    if not (up_run or down_run):
        fatal('Must specify at least one --up-run or --down-run.')
    db.update_boundary_plot(boundary, plot, up_run, down_run)
    return ret_dict(success='Updated plot {0} at boundary {1}'.format(
        boundary, plot
    ))


def find_boundary_plots(db, boundary):
    plots = db.boundary_plots(boundary)
    return [{
        'plot_name': plot[0],
        'magnet_up_reference_run': plot[1],
        'magnet_down_reference_run': plot[2]
    } for plot in plots]


def run_boundaries(args):
    from veloview.core import config

    if sum([args.add, args.remove, args.update]) > 1:
        fatal('Cannot add, remove, or update boundaries simultaneously')

    # Print list of run boundaries in the DB if we're not manipulating them
    if not args.run_number:
        with ReferenceDatabase(config.reference_db) as db:
            pjson(db.run_boundaries())
        return

    with ReferenceDatabase(config.reference_db) as db:
        run = args.run_number
        up, down = args.up_run, args.down_run
        if args.add:
            pjson(add_boundary_run(db, run, up, down))
        elif args.remove:
            pjson(remove_boundary_run(db, run))
        elif args.update:
            pjson(update_boundary_run(db, run, up, down))
        else:
            pjson(find_boundary_runs(db, run))


def boundary_plots(args):
    from veloview.core import config

    nactions = sum([args.add, args.remove, args.update])
    if nactions > 1:
        fatal('Cannot add, remove, or update boundary plots simultaneously')

    run = args.boundary_run
    plot = args.plot_name

    # Print boundary plots if given a boundary and no actions
    if not run:
        return pjson(ret_dict(failure='Boundary plot not specified'))

    # Print list of plots defined on the boundary if no plot name and no
    # actions are given
    if run and not plot and nactions == 0:
        with ReferenceDatabase(config.reference_db) as db:
            pjson(find_boundary_plots(db, run))
        return

    # Everything past this point requires a plot name and an action
    if not plot:
        return pjson(ret_dict(failure='Plot name not specified'))
    if nactions == 0:
        return pjson(ret_dict(failure='No action specified, e.g. --add'))

    with ReferenceDatabase(config.reference_db) as db:
        up, down = args.up_run, args.down_run
        if args.add:
            pjson(add_boundary_plot(db, run, plot, up, down))
        elif args.remove:
            pjson(remove_boundary_plot(db, run, plot))
        elif args.update:
            pjson(update_boundary_plot(db, run, plot, up, down))


def dereference(args):
    from veloview.core import config

    if not args.run:
        fatal('Nominal run not specified')

    run = args.run
    plot = args.plot
    polarity = RunDB().polarity(run)
    if not polarity:
        fatal('Could not resolve polarity of run {0}'.format(run))

    if run and not plot:
        with ReferenceDatabase(config.reference_db) as db:
            run = db.reference_run(run, polarity)
    if run and plot:
        with ReferenceDatabase(config.reference_db) as db:
            run = db.reference_run_for_plot(run, plot, polarity)

    ret = ret_dict(success='')
    ret['data'] = run
    pjson(ret)


def create_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument("--run-data-dir", default=veloview_config.run_data_dir,
                        help="Directory to search for run list and data")
    subparsers = parser.add_subparsers()

    parser_rb = subparsers.add_parser('run_boundaries',
                                      help='View and modify run boundaries')
    parser_rb.add_argument('--add', action='store_true',
                           help='Add a run boundary')
    parser_rb.add_argument('--remove', action='store_true',
                           help='Remove a run boundary')
    parser_rb.add_argument('--update', action='store_true',
                           help='Edit the reference runs for a run boundary')
    parser_rb.add_argument('run_number', type=int, nargs='?',
                           help='Boundary run number to process (optional)')
    parser_rb.add_argument('--up-run', type=int,
                           help='Run number for magnet up reference run')
    parser_rb.add_argument('--down-run', type=int,
                           help='Run number for magnet down reference run')
    parser_rb.set_defaults(func=run_boundaries)

    parser_bp = subparsers.add_parser('boundary_plots',
                                      help='View and modify boundary plots')
    parser_bp.add_argument('--add', action='store_true',
                           help='Add a boundary plot')
    parser_bp.add_argument('--remove', action='store_true',
                           help='Remove a boundary plot')
    parser_bp.add_argument('--update', action='store_true',
                           help='Edit the reference runs for a boundary plot')
    parser_bp.add_argument('boundary_run', type=int, nargs='?',
                           help='Boundary run number to process (optional)')
    parser_bp.add_argument('plot_name', type=str, nargs='?',
                           help='Plot name to process (optional)')
    parser_bp.add_argument('--up-run', type=int,
                           help='Run number for magnet up reference run')
    parser_bp.add_argument('--down-run', type=int,
                           help='Run number for magnet down reference run')
    parser_bp.set_defaults(func=boundary_plots)

    parser_deref = subparsers.add_parser(
        'dereference',
        help='Find the reference run for a given plot and/or run')
    parser_deref.add_argument('--run', help='Run number')
    parser_deref.add_argument('--plot', help='Plot name')
    parser_deref.set_defaults(func=dereference)

    return parser


if __name__ == '__main__':
    parser = create_parser()
    args = parser.parse_args()
    from veloview.core import config
    config.update_run_data_dir(args.run_data_dir)
    args.func(args)
    exit_success()
