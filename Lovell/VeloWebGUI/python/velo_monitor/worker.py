"""worker
Start a single rq worker to watch the queue database.
A worker is only started if this file is called directly.
"""
import os
import logging
try:
    import urllib.parse as urlparse
except ImportError:
    import urlparse
import redis
import rq

# List of rq.Queue instances to create
QUEUES = ['default']
# Default options used when creating an rq.Queue
QUEUE_OPTS = dict(default_timeout=15)


def create_connection():
    """Return a redis.Redis instance connected to REDIS_URL."""
    # REDIS_URL is defined in .env and loaded into the environment by Honcho
    redis_url = os.getenv('REDIS_URL')
    # If it's not defined, use the Redis default
    if not redis_url:
        redis_url = 'redis://localhost:6379'
    urlparse.uses_netloc.append('redis')
    url = urlparse.urlparse(redis_url)
    return redis.StrictRedis(
        host=url.hostname,
        port=url.port,
        db=0,
        password=url.password
    )


def create_queue(name, **kwargs):
    """Create an rq.Queue using QUEUE_OPTS merged with kwargs."""
    opts = QUEUE_OPTS.copy()
    opts.update(**kwargs)
    return rq.Queue(name, **opts)


def work():
    """Start an rq worker on the connection provided by create_connection."""
    # Preload ROOT module to reduce worker startup time
    import ROOT  # noqa
    with rq.Connection(create_connection()):
        worker = rq.Worker(list(map(create_queue, QUEUES)))
        # Quiet workers to suppress large result output
        # https://github.com/nvie/rq/issues/136
        worker.log.setLevel(logging.WARNING)
        worker.work()

if __name__ == '__main__':
    work()
