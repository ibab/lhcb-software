#!/bin/sh

# default file to be tested in order
# 1) $XDG_CONFIG_HOME/LHCb/default (if XDG_CONFIG_HOME exists)
# 2) $HOME/.config/LHCb/default
# 3) for f in $XDG_CONFIG_DIRS : $f/LHCb/default
# 4) /etc/default/LHCb

cfgfiles=""
if [[  -n "$XDG_CONFIG_HOME" ]]; then
  cfgfiles="$cfgfiles $XDG_CONFIG_HOME/LHCb/default"
fi
if [[ -n "$HOME" ]]; then
  cfgfiles="$cfgfiles $HOME/.config/LHCb/default" 
fi

if [[ -n "$XDG_CONFIG_DIRS" ]]; then
  for d in $(echo $XDG_CONFIG_DIRS | tr -s ':' ' ')
  do 
    cfgfiles="$cfgfiles $d/LHCb/default"
  done 
  unset d
fi
cfgfiles="$cfgfiles /etc/default/LHCb"
cfgfiles="$cfgfiles /etc/xdg/LHCb/default"


for c in $cfgfiles
do
  echo $c
  if [[ -r $c ]]; then
    eval `cat $c | sed -n -e '/^[^+]/s/\([^=]*\)[=]\(.*\)/\1="\2"; export \1;/gp'`
    break;
  fi
done

unset c
unset cfgfiles
