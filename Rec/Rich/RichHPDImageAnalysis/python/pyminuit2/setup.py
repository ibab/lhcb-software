#!/usr/bin/env python

from distutils.core import setup, Extension
import os, subprocess

##################################################################################################
try :
    incdir=os.popen('root-config --incdir').read()
    incdir=incdir.strip('\n')
    libdir=os.popen('root-config --libdir').read()
    libdir=libdir.strip('\n')
except RuntimeError:
    print "root-config not found in PATH"
    
setup(name="pyMinuit2",
      version="1.0.0",
      description="pyMinuit2: Minuit2 interface for minimizing Python functions",
      author="Johann Cohen-Tanugi",
      author_email="johann.cohentanugi@gmail.com",
      url="http://?",
      package_dir={"": "lib"},
      ext_modules=[Extension(os.path.join("minuit2"),
                             [os.path.join("minuit2.cpp")],
                             library_dirs=[libdir],
                             
libraries=["Core","Cint","RIO","Net","Hist","Graf","Rint","Matrix","MathCore","Minuit2"],
                             include_dirs=[incdir]
                             )])
