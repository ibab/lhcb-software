#!/bin/sh

########################################
# Root provisioning
########################################
# Configure the VM for running the VELO web GUI.
#
# Updates the VM, installs ROOT dependencies, and then,
# after a reboot, configures AFS and runs the user provisions.
#
# Initialise the VM with provisioning
#   vagrant up --provision
# Reload the VM when prompted, provisioning a final time
#   vagrant reload --provision
# This restart is done to ensure any upgraded kernel is loaded.
########################################

echo "Beginning provisioning"

YUM=/usr/bin/yum

# Add the SLC6 XROOTD stable repository
# http://xrootd.org/binaries/xrootd-stable-slc6.repo
sudo cat > /etc/yum.repos.d/xrootd-stable-slc6.repo << EOF
[xrootd-stable]
name=XRootD Stable repository
baseurl=http://xrootd.org/binaries/stable/slc/6/$basearch http://xrootd.cern.ch/sw/repos/stable/slc/6/$basearch
gpgcheck=1
enabled=1
protect=0
gpgkey=http://xrootd.cern.ch/sw/releases/RPM-GPG-KEY.txt
EOF

echo "Updating packages"
sudo $YUM -y update

echo "Installing required packages"
# The bare necessities:
#   vim and git
#   OpenAFS
#   ROOT dependencies
#   apache
#   Python module dependencies
sudo $YUM install -y \
  vim git \
  kmod-openafs openafs openafs-client \
  libXpm \
  httpd httpd-devel \
  python-devel gcc

PREPFILE=$HOME/.preparation

if [ ! -f $PREPFILE ]; then
  touch $PREPFILE
  echo "You now need to reboot the VM and rerun the provisioning."
  echo "To do this, run:"
  echo "  vagrant reload --provision"
  echo "I will then continue setting up the VM."
  exit
else
  rm -f $PREPFILE
  echo "Resuming provisioning"
fi

# Download, compile, and install the Redis server and CLI
echo "Installing Redis"
curl -O http://download.redis.io/redis-stable.tar.gz
tar xvzf redis-stable.tar.gz
cd redis-stable
make
sudo cp src/redis-server src/redis-cli /usr/local/bin
# Provide default configuration
sudo mkdir -p /usr/local/etc
sudo cp redis.conf /usr/local/etc
cd -
rm -rf redis-stable
rm redis-stable.tar.gz
# Enable memory overcommit
# http://redis.io/topics/admin
sudo cat >> /etc/sysctl.conf << EOF

vm.overcommit_memory = 1
EOF


echo "Configuring AFS"
echo "cern.ch" > $HOME/ThisCell
sudo mv $HOME/ThisCell /usr/vice/etc/ThisCell
sudo /sbin/chkconfig --add afs
sudo /sbin/chkconfig afs on
sudo /sbin/service afs start

echo "Configuring Apache"
APACHECONF=/etc/httpd/conf/httpd.conf
# Alter default global configuration
sudo sed -i 's/Listen 80/Listen 8008/' $APACHECONF
sudo sed -i 's/#NameVirtualHost \*:80/NameVirtualHost *:8008/' $APACHECONF
sudo sed -i 's/#Sendfile off/Sendfile Off/' $APACHECONF
# Add configuration for the VELO site
sudo cat > /etc/httpd/conf.d/VeloWebGUI.conf << EOF
LoadModule proxy_uwsgi_module /usr/lib64/httpd/modules/mod_proxy_uwsgi.so
<VirtualHost *:8008>
  DocumentRoot "/var/www/html"

    <Directory "/var/www/html">
      Options Indexes FollowSymLinks MultiViews
      AllowOverride None
      Order allow,deny
      Allow from all
    </Directory>

  ProxyPass / uwsgi://127.0.0.1:8000/
</VirtualHost>
EOF

echo "Setting up uwsgi Apache module"
git clone https://github.com/unbit/uwsgi.git
cd uwsgi/apache2
# This must be the same version as in requirements.txt
git checkout 2.0.7
sudo apxs -i -c mod_proxy_uwsgi.c
cd -
sudo rm -rf uwsgi
sudo chkconfig --levels 345 httpd on
sudo service httpd start

# We don't need the firewall for the VM
sudo service iptables stop
sudo chkconfig iptables off

# Relax permissions so Apache can talk to the uWSGI proxy
sudo setsebool -P httpd_can_network_connect 1

# Run the user provision as the vagrant user
su vagrant -c '/vagrant/user_provisioning.sh'

echo "Provisioning complete! Reboot once more."
