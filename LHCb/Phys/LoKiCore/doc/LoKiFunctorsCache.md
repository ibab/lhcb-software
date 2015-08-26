# LoKiFunctorsCache

As discussed in [LHCBPS-1357](https://its.cern.ch/jira/browse/LHCBPS-1357), the
use of the ROOT/Python bindings has a high cost in terms of memory.
Although the cost is constant (depending only on the amount of header files
being considered), it could be too heavy in some circumstances, like when using
LoKi Functors in Stripping applications.
To reduce the cost, but maintain the functionality, we decided to make it
possible to _precompile_ some functors so that we do not need to invoke the
ROOT/Python bindings during the execution of our job.

The precompiled collections of functors are stored in a cache library, each
functor being identified by a cryptographic hash of the functor Python
definition, and loaded at run time via the GaudiPluginService.


## Building a Cache Library

A cache library is usually meant for a specific application (set of options),
but we can have more cache libraries for a single application or one cache
library for a few applications (e.g. if the number of required functors is\
small).

To build a cache library in a package, one need to add a few lines to the
`CMakeLists.txt` file, for example:

    # load the function loki_functors_cache
    include(LoKiFunctorsCache)
    # generate the cache library
    loki_functors_cache(MyFunctorsCache
                        options/MyOptions.py
                        LINK_LIBRARIES LoKiHltLib
                        SPLIT 20)

where `include(LoKiFunctorsCache)` is required to be able to call the
function `loki_functors_cache()` which declares a cache library called
`MyFunctorsCache`, generated for the configuration in `options/MyOptions.py`.
The argument `LINK_LIBRARIES` has the same meaning as in the function
`gaudi_add_module()` (add libraries to the link statement). The `SPLIT`
argument is used to distribute the functors across several files, which is very
useful to reduce the compile time when the number of functors is very high.

The special CMake option `LOKI_BUILD_FUNCTOR_CACHE` (ON by default) can be
used to disable (globally to the project) the build of cache libraries, for
example to speed up the build during development.


## Running with the Cache Libraries

There are two ways of running using the functors cache libraries:

* **default**: use the cache if possible, otherwise fall back on the usual
     Python based definition
* **cache only**: if the cache for a functor cannot be found, the application
     fails (useful in production or to check that all the needed functors are
     actually in the cache libraries)
* **Python only**: ignore completely the cache libraries and use only Python
     based definition (useful for validation)

The *cache only* mode is enabled globally by setting the environment variable
`LOKI_DISABLE_PYTHON` (to anything), or on a factory-by-factory basis via the
property `UsePython`.  In a similar way, the *Python only* mode is controlled
via the environment variable `LOKI_DISABLE_CACHE` (to anything), or the
property `UseCache`.
