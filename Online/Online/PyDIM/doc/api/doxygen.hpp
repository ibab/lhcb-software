/*!
\mainpage

\section PyDIM

PyDIM is a Python interface to <a href="http://dim.web.cern.ch/dim/">DIM</a>.
PyDIM could be used to create DIM clients and servers, using an API very similar
to the one that is used for <a href="http://dim.web.cern.ch/dim/dimC.html">C</a>.

\section requirements Requirements

\subsection requirements-dim DIM

As the name suggest, the package relies on the DIM package.

PyDIM has been developed using the v16rXX, v17rXX and v18r0 DIM versions and
works with any. It has not been tested with older versions but I believe it
has good chances to work.
It does not depend on the DIM internals but only on DIM's API.
This is why it should be compatible with a wide range of DIM versions.

\subsection requirements-python Python

PyDIM has been tested with Python 2.3, 2.4 and 2.5.

Note: The package has been developed on both 32 and 64 bits architectures.

\section install Installation

   \subsection build Building the package from scratch and installing it:
   -# Install DIM. I would suggest placing the DIM libraries and header files in
    <code>/usr/local/lib</code> and <code>/usr/local/include</code> respectively.
Please make the necessary changes so that the run-time linker finds libdim.so
(basically add an entry in /etc/ld.so.conf that points to the DIM library directory).
Make sure that the DIM library is built using the <b>same</b> compiler and
architecture as the Python installation you want to use.
   -# Get the latest SVN version: <code>svn co svn+ssh://lbgw02/group/online/SVN_REPOS/PyDIM</code> or from CVS (TODO: add link to the official Online CVS)
   -# cd to the PyDIM directory and execute "sudo make install".

In the general case this should take care of building the library.

If the DIM libraries are not installed in the usual locations, the install
script will look for an environment variable with the actual location. The
variables that can be used are:

- DIMDIR
- DIMHOME
- DIM_HOME

Remember to point that variable to the main DIM installation directory, i.e. the
one that contains the sub-directories <code>dim</code> and the specific one for
your operating system, e.g. <code>linux</code>.

For building on Windows you will need the same compiler (the same cl version)
as the one used to build DIM.
At this point, the default DIM C compiler is included in the Microsoft .Net 2003
framework.
Ideally the Python interpreter should be build using the same compiler (any Python
version > 2.3 should do the job).

Windows does not come by default with any SVN or CVS clients
so you might want to install the Cygwin linux like environment for convenience.
In this case make sure that the .Net paths are still correctly set.


\subsection rpm RPM Installation and building instructions
PyDIM comes already built as and RPM package with a dependency on the DIM RPM.
In this manner the PyDIM libraries can be deployed more easily on all the machines
inside the Online system using rpm, yum or quattor.
These RPMs (including the DIM ones) can be found on AFS under:

<pre>/afs/cern.ch/lhcb/project/web/online/online-rpm-repo/</pre>

They are also replicated to the pit on the yum server
(<b>TODO</b>: Add the actual machine name and path to them).

The RPMs are for both i386 and x86_64 architectures and are compiled gcc 3.4.X
against Python 2.3 (these are the standard Python, gcc versions for SLC 4.6).

There are two dependencies for building the package: python-devel and dim-devel.
Those packages contains the header files for Python and DIM respectively.

RPMs packages are build using the distutils module, this can be done by running
the setup script with command <code>bdist_rpm</code>:

<pre>
    $ python setup.py bdist_rpm
</pre>

The <code>bdist_rpm</code> command will create binary and source packages in
the <code>dist</code> directory. The command provides options for the process,
such as creating only the <code>.spec</code> file:

<pre>
    $ python setup.py bdist_rpm --spec-only
</pre>

Or building only the RPM source package:

<pre>
    $ python setup.py bdist_rpm --source-only
</pre>

The full list of the options can be displayed with the help command:

<pre>
    $ python setup.py --help bdist_rpm
</pre>

\subsubsection disutils-slc5 Building RPMs packages in SLC5

Builing RPM packages with distutils in SLC5 can be tricky. Check this
<a href="https://lbtwiki.cern.ch/bin/view/Online/RpmPythonDistutils">guide</a>
with the description of the problem and a workaround.

\subsection msi MSI Installation and building instructions
PyDIM can be also be build as an MSI package or as a Windows installer.
In the most simple scenario a simple "make msi" will do the job. This relies on the following assumption that
the .Net environment is set up properly as described in the building instructions.
The *.msi and *.exe packages will be found in the PyDIM/dist directory.

\subsection make Summary of make targets
- build -> only builds the package

- install -> builds and install the package in /usr/lib/pythonXX (requires root). It does not include the documentation.

- doc -> builds the documentation using doxygen. You will require a version >= 1.5.XX. The typical SLC installation has doxygen 1.3.XX and this has poor support for parsing Python sources. You can use an older version but the documentation will be malformed.

- rpm -> creates a RPM. See also the pydim.spec file. Might require root privileges.

- msi -> creates a MSI installer package in the PyDIM/dist directory.


\section usage Usage
   -# In case you have PyDIM installed as a RPM (recommended) then just set up the DIM_DNS_NODE and say "import pydim" from Python.
   -# In case you have your own local installation you have to set up your environment properly to point to the appropriate libraries and scripts.

The setup/setup.sh is provided for bash shells as an environment setup template.
It tries to set up the DIM_DNS_NODE, adds the PyDIM/python directory to
your PYTHONPATH, LD_LIBRARY_PATH.

You can execute some tests/examples from the 'tests' directory.
Those scripts can be used as examples of the package's functionality.

\section examples Examples

The following example shows to run a server with two simple services.


	\code
		import sys, time
		import pydim

		def service_callback(tag):
			"""
			Service callbacks are functions (in general, Python callable objects)
			that take one argument: the DIM tag used when the service was added,
			and returns a tuple with the values that corresponds to the service
			parameters definition in DIM.
			"""
			# Calculate the value of the server
			# ...
			val1 = 3.11
			val2 = 42
			return (val1, val2)

		def callback2(tag):
			# Calculate the value
			# ...
			# Remember, the callback function must return a tuple
			return ( "hello world", )

		# The function dis_add_service is used to register the service in DIM
		svc = pydim.dis_add_service('test service 1', "F:1;I:1;", service_callback, 1)

		# Register another service
		svc2 = pydim.dis_add_service('another service', "C", callback2, 2)

		# The return value is the service identifier. It can be used to check
		# if the service was registered correctly.
		if not svc2:
			sys.stderr.write("An error occurred while registering the service\n")
			sys.exit(1)

		# A service must be updated before using it.
		pydim.dis_update_service(svc)
		pydim.dis_update_service(svc2)

		# Start the DIM server
		pydim.dis_start_serving('server-name')

		while True:
			# Update the service periodically
			pydim.dis_update_service(svc)
			time.sleep(5)

	\endcode

\section contact Contact

This module was originally developed by
<a href="http://consult.cern.ch/xwho/people/652087">Radu Stoica</a>.

Now it's maintained by
<a href="http://consult.cern.ch/xwho/people/417932">Niko Neufeld</a> and
<a href="http://consult.cern.ch/xwho/people/681596">Juan Manuel Caicedo</a>.

Feel free to send your questions and bugfixes.

\defgroup examples Examples of using PyDIM
Various examples using the C and C++ DIM APIs.

You can execute some tests/examples from the 'tests' directory.
These scripts can be used as examples of the module's functionality.
*/
