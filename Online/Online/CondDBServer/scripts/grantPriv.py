#!/usr/bin/env python
import os, sys
from PyCool import cool

from CondDBUI import CondDB

from subprocess import Popen
from pprint import pformat

filename = "know_priv_nodes.py"

try:
   cache = eval(open(filename).read())
except IOError, x:
   if x.errno == 2:
      cache = {}
   else:
      raise

for connStr in ["CondDBPrivate(owner)/PRIVATE", "CondDBOnline(owner)/ONLINE" ]:
   db = CondDB(connStr)
   new_list = db.getAllNodes()

   if new_list != cache.get(connStr, []):
      proc = Popen(["coolPrivileges", connStr, "GRANT", "READER", "lhcb_conddb_reader"])
      #proc = Popen("echo " + " ".join(["coolPrivileges", '"%s"' % connStr, "GRANT", "READER", "lhcb_conddb_reader"]), shell = True)
      proc.wait()
      cache[connStr] = db.getAllNodes()

open(filename, "w").write(pformat(cache)+"\n")
