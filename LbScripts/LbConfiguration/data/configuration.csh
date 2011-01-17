#!/bin/csh

# default file to be tested in order
# 1) $XDG_CONFIG_HOME/LHCb/default (if XDG_CONFIG_HOME exists)
# 2) $HOME/.config/LHCb/default
# 3) for f in $XDG_CONFIG_DIRS : $f/LHCb/default (if XDG_CONFIG_DIRS exists)
# 4) /etc/default/LHCb

set cfgfiles=""
if ( $?XDG_CONFIG_HOME ) then
  set cfgfiles="$cfgfiles $XDG_CONFIG_HOME/LHCb/default"
endif
if ( $?HOME ) then
  set cfgfiles="$cfgfiles $HOME/.config/LHCb/default" 
endif

if ( $?XDG_CONFIG_DIRS ) then
  foreach d (`echo $XDG_CONFIG_DIRS | tr -s ':' ' ' `)
    set cfgfiles="$cfgfiles $d/LHCb/default"
  end
  unset d
endif
set cfgfiles="$cfgfiles /etc/default/LHCb" 

foreach c ( $cfgfiles )
  echo $c
  if ( -r $c ) then
    eval `cat $c | sed -n -e '/^[^+]/s/\(\\\$[^ ]*\)/"\\\\\1"/' -e '/^[^+]/s/\([^=]*\)[=]\(.*\)/setenv \1 \"\2\";/gp'`
  break
  endif
end

unset c
unset cfgfiles
