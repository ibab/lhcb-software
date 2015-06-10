"""Utilities for testing the VELO monitor."""
import tempfile
import shutil

from veloview.config import Config as VeloConfig

RUNS = sorted(range(123987, 123995) + range(123960, 123975), reverse=True)


def set_up_temp_run_data_dir():
    """Create a temporary run data directory and run list file."""
    # Cache old run data directory value, set config to a temporary one
    VeloConfig().old_run_data_dir = VeloConfig().run_data_dir
    VeloConfig().run_data_dir = tempfile.mkdtemp()
    # Create a dummy run list
    with open(VeloConfig().processed_run_list_path, 'w') as f:
        for r in RUNS:
            f.write('{0}\n'.format(r))


def tear_down_temp_run_data_dir():
    """Undo set_up_temp_run_data_dir."""
    # Ensure we only remove temporary directories
    if VeloConfig().run_data_dir.startswith('/tmp'):
        shutil.rmtree(VeloConfig().run_data_dir)
    VeloConfig().run_data_dir = VeloConfig().old_run_data_dir
