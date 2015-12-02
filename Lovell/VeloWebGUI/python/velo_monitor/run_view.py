from collections import OrderedDict
from flask import (
    Blueprint,
    current_app,
    request,
    flash,
    redirect,
    url_for,
    render_template,
    abort,
    g
)

from veloview.config import Config as VeloConfig
from veloview.runview import utils as runview_utils

run_view = Blueprint('run_view', __name__,
                     template_folder='templates/run_view')


def default_run():
    """Retrieve the default run rumber from veloview.

    Used when no run number is specified by the user, or if the given run is
    invalid (as judged by valid_run`).
    """
    return runview_utils.run_list()[0]


def nearby_runs(run, runs, distance=3):
    """Return the runs +/- distance either side of run in runs."""
    idx = runs.index(run)
    lower = idx - distance if idx >= distance else 0
    upper = idx + distance + 1
    return runs[lower:upper]


@run_view.route('/', defaults={'run': default_run(), 'page': '', 'sensor': 0})
@run_view.route('/<int:run>', defaults={'page': '', 'sensor': 0})
@run_view.route('/<int:run>/<page>', defaults={'sensor': 0})
@run_view.route('/<int:run>/<page>/<int:sensor>')
def run_view_builder(run, page, sensor):
    # See if the request was for a particular run/page/sensor and redirect
    # If any GET parameter wasn't specified, or was specified with an invalid
    # value, fall back to the default
    if request.args:
        run = request.args.get('run', run)
        page = request.args.get('page', page)
        sensor = request.args.get('sensor', sensor)
        # Check that the integer URL params are castable
        try:
            run = int(run)
        except ValueError:
            new_run = default_run()
            flash('Invalid run number "{0}", reset to "{1}"'.format(
                run, new_run
            ), 'error')
            run = new_run
        try:
            sensor = int(sensor)
        except ValueError:
            new_sensor = 0
            flash('Invalid sensor number "{0}", reset to "{1}"'.format(
                sensor, new_sensor
            ), 'error')
            sensor = new_sensor
        url = url_for('run_view.run_view_builder',
                      run=run,
                      page=page,
                      sensor=sensor)
        return redirect(url)

    # Check if the run number is valid, redirecting to default if not
    if not runview_utils.valid_run(run):
        new_run = default_run()
        flash('Invalid run number "{0}", reset to "{1}"'.format(run, new_run),
              'error')
        run = new_run
        url = url_for('run_view.run_view_builder',
                      run=run,
                      page=page,
                      sensor=sensor)
        return redirect(url)

    # Check if the sensor number is valid, redirecting to default (0) if not
    if not runview_utils.valid_sensor(sensor):
        flash('Invalid sensor number "{0}", reset to "0"'.format(sensor),
              'error')
        sensor = 0
        url = url_for('run_view.run_view_builder',
                      run=run,
                      page=page,
                      sensor=sensor)
        return redirect(url)

    # Add the dummy DQ page
    pages = VeloConfig().run_view_pages.items()
    pages.insert(0, ('dq', {'title': 'Data quality'}))
    pages = OrderedDict(pages)

    # Load the default page from the configuration if we're at the root
    if page == '':
        page = current_app.config['DEFAULT_CHILDREN'].get('run_view', None)
        if page is not None:
            page = page[len('run_view/'):]
    # Else load the page data associated with the route's page
    page_data = pages.get(page, None)

    # Set up the required template variables and render the page
    g.page = page
    g.pages = pages
    g.page_data = page_data
    g.run = run
    g.runs = runview_utils.run_list()
    g.nearby_runs = nearby_runs(g.run, g.runs)
    g.sensor = sensor
    g.active_page = 'run_view/{0}'.format(page)

    # 404 if the page doesn't exist in the config dict
    if page_data is None:
        abort(404)

    return render_template('run_view/dynamic.html')


# Delegate the page not found hits to the catchall blueprint
@run_view.errorhandler(404)
def page_not_found(e):
    g.pages = VeloConfig().run_view_pages
    g.active_page = 'run_view/404'
    return render_template('run_view/404.html'), 404


# Define a filter used to sanitise the "short name" of run view plots in to
# valid URL hash strings
@run_view.app_template_filter()
def sanitise(s):
    """Return s with all non-alphanumeric characters replaced with '_'."""
    return ''.join([c.lower() if c.isalnum() else '_' for c in s])
