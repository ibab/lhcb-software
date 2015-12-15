import os, sys, pkgutil, importlib
import Gaudi.Configuration

from Configurables import HltConf
conf = HltConf()
knownStreams = conf.knownStreams()

def getSettings():
    import HltSettings
    settings = {}
    for _, name, is_pkg in pkgutil.iter_modules(HltSettings.__path__[:2]):
        if is_pkg:
            continue
        pkg_name = HltSettings.__name__ + '.' + name
        try:
            settings[name] = getattr(importlib.import_module(pkg_name), name)()
        except ImportError as e:
            pass
        except AttributeError as e:
            print 'WARNING: Badly named settings object in %s' % pkg_name
            continue
    return settings

passed = True
sets = getSettings()
for name, settings in sets.iteritems():
    if not hasattr(settings, 'Streams'):
        continue
    for stream, rb in settings.Streams().iteritems():
        if stream not in knownStreams:
            print 'FATAL: Unknown stream defined in %s' % name
            passed = False

if passed:
    print 'PASSED'
else:
    print 'FAILED'

sys.exit(0)
