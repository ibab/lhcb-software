#!/usr/bin/env gaudirun.py
#
# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore.py
# or just
#   ../options/Moore.py
#
import Gaudi.Configuration
from Moore.Configuration import Moore

#use new splitting of Hlt2
Moore().Split=''

##only for experts!
from Configurables import MooreExpert
#turn of TISTOS, "Gerhard's Sledgehammer"
MooreExpert().Hlt2Independent=True
