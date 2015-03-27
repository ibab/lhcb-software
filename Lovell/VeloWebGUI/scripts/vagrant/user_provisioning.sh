#!/bin/sh

########################################
# User provisioning
########################################
# Configure the vagrant user's login shell (bash), install pip, rbenv,
# and a Ruby.
# This script will be executed by the vagrant user
#   su vagrant -c '/vagrant/user_provisioning.sh'
########################################

echo "Starting user provisioning"

# Set up ROOT/PyROOT by sourcing software on AFS
echo "Configuring shell"
cat >> $HOME/.bashrc << EOF

GCC=/afs/cern.ch/sw/lcg/external/gcc/4.8/x86_64-slc6-gcc48-opt
ROOT=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.14/x86_64-slc6-gcc48-opt/root
PYTHON=/afs/cern.ch/sw/lcg/external/Python/2.7.4/x86_64-slc6-gcc48-opt
source \$GCC/setup.sh
source \$ROOT/bin/thisroot.sh
PATH=\$PYTHON/bin:\$HOME/.local/bin:\$PATH
LD_LIBRARY_PATH=\$PYTHON/lib:\$LD_LIBRARY_PATH

# Add Lovell modules
PYTHONPATH=/Lovell/VeloWebGUI/python:/Lovell/VeloAnalysisFramework/python:\$PYTHONPATH

export PATH
export LD_LIBRARY_PATH

EOF
source $HOME/.bash_profile

echo "Setting up pip and virtualenv"
curl https://bootstrap.pypa.io/get-pip.py > $HOME/get-pip.py
python $HOME/get-pip.py --user
pip install --user virtualenv virtualenvwrapper
rm -f $HOME/get-pip.py
# Set up for virtualenvwrapper
WORKON_HOME=$HOME/virtualenvs
echo "export WORKON_HOME=$WORKON_HOME" >> .bashrc
echo "source \$HOME/.local/bin/virtualenvwrapper.sh" >> .bashrc
mkdir -p $WORKON_HOME

source $HOME/.bash_profile

mkvirtualenv VeloWebGUI
pip install -r /Lovell/VeloWebGUI/doc/requirements.txt

echo "User provisioning complete!"
