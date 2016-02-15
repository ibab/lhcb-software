from flask import Blueprint
import jobmonitor


# Only export externally useful methods
__all__ = ['create_app', 'wsgi']


def create_app():
    """Create a Flask application deriving from jobmonitor."""
    from velo_monitor.run_view import run_view
    from velo_monitor.velo_view import velo_view
    from velo_monitor.job_resolvers import (
        run_view_resolver,
        trends_resolver
    )
    app = jobmonitor.create_app()
    app.config.from_object('velo_monitor.config')

    if not app.debug:
        add_logging(app)

    example = Blueprint('velo_monitor', __name__,
                        template_folder='templates',
                        static_folder='static',
                        static_url_path='/{0}'.format(__name__))
    app.register_blueprint(example)

    app.register_blueprint(run_view, url_prefix='/run_view')
    app.register_blueprint(velo_view, url_prefix='/velo_view')

    app.add_job_resolver(run_view_resolver)
    app.add_job_resolver(trends_resolver)

    return app


def wsgi(*args, **kwargs):
    """Create an app and pass it arguments from a WSGI server, like gunicorn.

    With this, run with something like `gunicorn velo_monitor:wsgi`.
    """
    return create_app()(*args, **kwargs)


def add_logging(app):
    """Add log-to-file and log-to-mail to a Flask app."""
    import logging

    # Record to file on WARNING
    file_handler = logging.FileHandler('log/velo_monitor.log')
    file_handler.setLevel(logging.WARNING)

    # Send an email to the admins on ERROR
    mail_handler = logging.handlers.SMTPHandler(
        '127.0.0.1',
        'alex.pearce@cern.ch',
        app.config.get('ADMINS'),
        'VELO monitoring app failed'
    )
    mail_handler.setLevel(logging.ERROR)

    # Attach the handlers to the app
    app.logger.addHandler(file_handler)
    # app.logger.addHandler(mail_handler)
