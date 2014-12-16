veloview
========

A data quality analysis framework used to monitor the LHCb Vertex Locator.

Usage
-----

To install the `veloview` Python module, you can use the [git repository](https://git.cern.ch/web/LHCbVeloView.git).

```bash
$ git clone https://git.cern.ch/reps/LHCbVeloView
$ cd LHCbVeloView/veloview
$ python setup.py build
$ python setup.py install
```

Ideally we would recommend installation with [`pip`](https://pypi.python.org/pypi/pip), but there is a [bug installing modules in subdirectories](https://github.com/pypa/pip/issues/1600) which, whilst fixed in the development branch, has not been fixed in a release (as of pip 1.5.6).
When the fix is released, you should be able to install `veloview` with

```bash
$ pip install -e "git+https://git.cern.ch/reps/LHCbVeloView#egg=veloview&subdirectory=veloview"
```

If you do not want to install the package, instead want to run it from
the repository (useful when developing).  Source the script,
[sourceme.sh](sourceme.sh).  At the moment this is only supported when
you are using bash as your shell.

To use the module once it's installed (or the environment is setup
with `sourceme.sh`), just `import` it.

```python
import veloview
```

Dependencies
------------

`veloview` depends on ROOT and PyROOT.  Other Python dependencies are
given in the [`requirements.txt`](requirements.txt) file.  You can use
`pip` to install the Python dependencies.

```bash
$ pip install -r requirements.txt
```

Testing
-------

The test suite for the `veloview` module is contained within the
[`tests`](veloview/tests) directory of the package.  It uses the
unittest module, part of the Python standard library.  You can run the
tests in the following ways.

* When veloview has been installed or the enviroment has been setup
  with `sourceme.sh`.

  ```bash
  $ python -m unittest discover
  $ python -m unittest veloview.tests.test_<module>_<test> # etc
  ```

  To get a verbose output, add the `-v` flag.

* You can also run tests with make with something like this:

  ```bash
  $ cd veloview
  $ make tests # run all tests
  $ make dqm-tests # run tests with real-world DQM files, needs AFS
  $ make test_<module>_<test> # ... etc (run individual tests)
  $ make OPTS=-v test_<module>_<test> # for verbose output
  $ make debug_test_<module>_<test> # for debugging with pdb
  ```

Notes
-----

The `veloview` package provides several modules:
[`core`](veloview/core), [`analysis`](veloview/analysis), and
[`utils`](veloview/utils).  The `core` module provides the data
quality framework used for automatic analysis.  `analysis` provides
various comparison functions to be used with the framework in `core`
for analysis.

Any applications and scripts (interactive or for cron jobs) should go
in the [`bin`](bin) directory.  If it doesn't exist, create it.
