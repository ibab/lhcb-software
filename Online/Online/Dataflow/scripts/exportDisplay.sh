#!/bin/bash
if test -n "${1}"; then
  export DISPLAY=${1}
  echo "export DISPLAY to ${DISPLAY}"
fi;
