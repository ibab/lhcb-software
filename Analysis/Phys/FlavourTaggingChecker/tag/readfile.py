#!/usr/bin/env python

# Importing modules
from ROOT import *
from math import *
import commands

f = open('../../vecchi/dati/2011/tmp/my.ALL.Strip13b', 'r')
print f

Bupdg = 5279.17
MassWindow = 25
Maxtracks = 150
Mintracks = 150
break200 = 0
counter = 0
for line in f:
    counter+=1
    if (break200==1):
        if (counter>=200): break
    a=line.split()
    if (float(a[3])>0.3): #propertime
        if (float(a[2])<(Bupdg+MassWindow)):
            if (float(a[2])>(Bupdg-MassWindow)):
#                if (float(a[6])>Maxtracks): #maxtracks
                 if (float(a[6])<Mintracks): #mintracks
  #              if (float(a[8])==1): #trigger
                    print a[0], a[1]
        

            



