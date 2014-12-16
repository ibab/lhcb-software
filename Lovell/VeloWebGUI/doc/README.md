# VELO monitor

A monitoring application for the LHCb Vertex Locator, deriving from the [`jobmonitor`](https://github.com/alexpearce/jobmonitor).

Currently the application uses the dummy `veloview` module, which contains very basic methods for fetching objects from ROOT files.
The vision is that the functionality provided by the dummy module will be migrated to the actual `veloview` module, part of the [LHCbVeloView repository](https://git.cern.ch/web/LHCbVeloView.git).

Setup
-----

Clone the repository, set up a [`virtualenv`](http://virtualenv.readthedocs.org/en/latest/) (with [`virtualenvwrapper`](http://virtualenvwrapper.readthedocs.org/en/latest/)), then run the application. (You can skip the creation of the `virtualenv` to install the requirements globally.)

```bash
$ git clone https://path/to/repo/velo-monitor.git
$ cd velo-monitor
$ mkvirtualenv velo-monitor
$ pip install -r requirements.txt
$ honcho start
```

The `honcho start` command will initialise the application, a [Redis](http://redis.io/) server, and one worker.
To start more than one worker you can use the `-c` option, like `honcho start -c worker=4`.

Requirements
------------

A [Redis](http://redis.io/) server is used to queuing jobs and storing their results.
[ROOT](http://root.cern.ch/) is used to perform the tasks, namely getting data out of histograms, and to generate the data in the first place.

Deployment
----------

The VELO monitor will be deployed on a machine running [Scientific Linux](https://www.scientificlinux.org/), but should by deployable on any Linux distribution. The [dependencies](#requirements) must be satisfied.

With a copy/clone of this repository on the machine, first install the [required Python packages](requirements.txt) and then generate the appropriate init files.

```bash
# Inside the velo-monitor repository
$ pip install -r requirements.txt
$ honcho export -a velo -u deploy -c web=1,worker=4,redis=1 -p 8000 -s /bin/bash upstart ./init
```

This example `honcho` command creates init files, in the `init/` directory, for [`upstart`](http://upstart.ubuntu.com/), a service/task manager installed on several distributions. Once these files are copied to the appropriate location for your distribution (`/etc/init` on Scientific Linux), you can start the `velo` process, which will start the workers, web server, and Redis database.

```bash
$ sudo cp init/* /etc/init
$ sudo start velo
```

### Virtualenv

To isolate `velo_monitor` dependencies from other Python applications, you can use a [`virtualenv`](http://virtualenv.readthedocs.org/en/latest/). It is most useful when combined with [`virtualenvwrapper`](http://virtualenvwrapper.readthedocs.org/en/latest/).

```bash
$ mkvirtualenv velo-monitor
```

You can then proceed to deploy as usual (`pip install -r ...`). To resume working in the virtual environment, use `workon velo-monitor`.

One caveat when using a `virtualenv` is that one must ‘activate’ the environment before using any binaries within, such as `gunicorn`. This means the init scripts must be modified. For the upstart scripts mentioned above, this means sourcing the activation script for the virtual environment, e.g.:

```bash
. /home/deploy/virtualenvs/velo-monitor/bin/activate
```

### Vagrant

[Vagrant](https://www.vagrantup.com/) allows simple instantiation and management of development virtual machines (VMs).
The Vagrantfile provided allows any developer to quickly start a VM with an environment that will not only run the monitor, but that is the same as every other developer.
This uniformity means less time is spent getting the monitor up and running, and as the operating system of the VM is Scientific Linux 6, there is parity with the machines in the online network on which the monitor runs in production.

Assuming Vagrant and [VirtualBox](https://www.virtualbox.org/) are installed correctly, the VM for the monitor can be created with

```bash
$ vagrant up --provision
```

and then when prompted to reload, do so with

```bash
$ vagrant reload --provision
```

Both of these steps can take some time, upwards of ten minutes.

If you then want to run the monitor, SSH into the VM, create the [virtualenv](http://www.virtualenv.org/) and install the dependencies.

```bash
$ vagrant ssh
$ cd /vagrant
$ mkvirtualenv velo-monitor
$ pip install -r requirements.txt
$ pip install -e /veloview/veloview
```

When resuming development, you will need to activate the [virtualenv](http://www.virtualenv.org/), in the VM, to load the appropriate packages

```bash
workon velo-monitor
```

and then start the server

```bash
cd /vagrant
honcho start -f Procfile.vagrant
```

Then [visit the site](http://localhost:5000/) on your development machine.
