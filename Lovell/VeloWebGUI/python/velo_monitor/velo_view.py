from collections import OrderedDict
from flask import (
    Blueprint,
    render_template,
    g
)

from veloview.runview import dq_trends

velo_view = Blueprint('velo_view', __name__,
                     template_folder='templates/velo_view')


@velo_view.route('/overview')
def overview():
    """Display table of DQ summaries for recent runs."""
    g.active_page = 'velo_view/overview'
    return render_template('velo_view/overview.html')


@velo_view.route('/trends')
def trends():
    """Show change of DQ quantities over time."""
    g.active_page = 'velo_view/trends'
    g.trendables = dq_trends.get_trending_variables()
    return render_template('velo_view/trends/layout.html')
