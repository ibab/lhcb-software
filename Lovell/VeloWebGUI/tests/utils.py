"""Utilities for testing the VELO monitor."""
from veloview.core import config as veloview_config

RUNS = sorted(range(123987, 123995) + range(123960, 123975), reverse=True)


def set_up_run_list():
    """Create a dummy run list file filled with RUNS."""
    prlf = '/tmp/runList.txt'
    veloview_config.old_prlf = veloview_config.processed_run_list_file
    veloview_config.processed_run_list_file = prlf
    with open(prlf, "w") as f:
        for r in RUNS:
            f.write("{0}\n".format(r))


def tear_down_run_list():
    veloview_config.processed_run_list_file = veloview_config.old_prlf
