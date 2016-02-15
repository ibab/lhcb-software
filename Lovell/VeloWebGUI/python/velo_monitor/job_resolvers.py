def run_view_resolver(name):
    """Resolve a job for the run view.

    Requires the job name to start with `run_view`.
    """
    whitelist = [
        'runview.plots.get_run_plot_with_reference',
        'runview.dq_trends.get_dq_values'
    ]
    if name not in whitelist:
        return None
    return 'veloview.{0}'.format(name)


def trends_resolver(name):
    """Resolve a job for the trending view."""
    if name != 'runview.plots.get_trending_plot':
        return None
    return 'veloview.{0}'.format(name)
