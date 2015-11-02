def run_view_resolver(name):
    """Resolve a job for the run view.

    Requires the job name to start with `run_view`.
    """
    if name != 'runview.plots.get_run_plot_with_reference':
        return None
    return 'veloview.{0}'.format(name)
