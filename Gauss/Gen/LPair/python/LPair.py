#!/usr/bin/env python
"""
Author  : Philip Ilten
Created : 2015-08-03

Script to generate LPAIR code that can be used with Gauss. The
following changes are made to the version of LPAIR passed to Ronan
McNulty by Andrey Shamov which is contained in LPair.120920.tgz. The
following changes are made.

1. The random number generator, RLU, is replaced with the Gauss
   random number generator.
2. The initialization routines are over-written so the parameters in the
   common blocks can be set directly.
3. Unnecessary subroutines are remove to minimize CERNLIB dependencies. 
"""

import os, urllib2, tarfile, shutil, Fortran
ver = {'LPAIR': 'LPair.120920', 'CERNLIB': '2006'}
url = {'LPAIR': None, 'CERNLIB': 'http://cernlib.web.cern.ch/cernlib/'
       'download/2006_source/tar/'}

# Helper function to copy files.
def copy(src, dst, include = None, exclude = None):
    filenames = [os.path.join(src, f) for f in os.listdir(src)]
    for f in filenames:
        if exclude and os.path.basename(f) in exclude: continue
        if include and not os.path.basename(f) in include: continue
        if os.path.isfile(f):
            shutil.copy(f, os.path.join(dst, os.path.basename(f)))
        elif not os.path.isdir(os.path.join(dst, os.path.basename(f))):
            shutil.copytree(f, os.path.join(dst, os.path.basename(f)))
        
# Untar the LPAIR source.
if not os.path.isdir(ver['LPAIR']):
    tgz  = tarfile.open(ver['LPAIR'] + '.tgz', 'r:gz')
    tgz.extractall()
    tgz.close()

# Untar the CERNLIB source.
if not os.path.isdir(ver['CERNLIB']):
    info = urllib2.urlopen(url['CERNLIB'] + ver['CERNLIB'] + '_src.tar.gz')
    data = info.read()
    code = file(ver['CERNLIB'] + '.tgz', 'w')
    code.write(data)
    code.close()
    tgz  = tarfile.open(ver['CERNLIB'] + '.tgz', 'r:gz')
    tgz.extractall()
    tgz.close()

# Copy the needed source files.
copy(ver['LPAIR'] + '/SRC', '../src',
     exclude = ['README', 'ffkeyi.f', 'gmupdf.f', 'shamov_aux.f',
                'treat_atlas.f', 'zdis.f'])
copy(ver['LPAIR'] + '/PHOTOS', '../src',
     exclude = ['MakeLibrary.sh'])
copy(ver['CERNLIB'] + '/src/packlib/kernlib/kerngen/tcgen', '../src',
     include = ['icfnbl.F', 'iclocl.F', 'icdeci.F'])
copy(ver['CERNLIB'] + '/src/packlib/kernlib/kerngen/tcgens', '../src',
     include = ['iucomp.F', 'ucopy.F'])
copy(ver['CERNLIB'] + '/src/mclibs/jetset/jetset', '../src',
     include = ['ulmass.F', 'luexec.F', 'lujoin.F', 'lulist.F',
                'luchge.F', 'luprep.F', 'lucomp.F', 'lushow.F', 'luindf.F',
                'ludecy.F', 'luerrm.F', 'luboei.F', 'lustrf.F', 'lurobo.F',
                'lukfdi.F', 'ulangl.F', 'lutaud.F', 'luptdi.F', 'luzdis.F',
                'lulogo.F', 'luname.F', 'plu.F'])

# Copy the needed header folders.
copy(ver['CERNLIB'] + '/src/packlib/kernlib/kerngen/kerngen', '../src/kerngen',
     exclude = ['CVS', 'Imakefile'])

# Use the Gaudi random number generator.
rlu = file('../src/rlu.F', 'w')
rlu.write('      FUNCTION RLU(IDUMMY)\n      REAL*8 CPYR\n'
          '      RLU = CPYR(IDUMMY)\n      RETURN\n      END\n')
rlu.close()

# Remove unnecessary subroutines which generate unwanted dependencies.
gmuini = file('../src/gmuini.f', 'w')
gmuini.write('      SUBROUTINE GMUINI\n      END\n')
gmuini.close()
gmucha = file('../src/gmucha.f', 'w')
gmucha.write('      SUBROUTINE GMUCHA\n      END\n')
gmucha.close()
src = [l.rstrip('\n').lower() for l in file('../src/shamov_zdummycalls.f', 'r')]
idx = range(0, 4) + range(7, 9) + range(13, len(src))
zdummy = file('../src/shamov_zdummycalls.f', 'w')
for i, l in enumerate(src):
    if i in idx: zdummy.write(l + '\n')
zdummy.close()

# Generate the C-interface.
cin = file('../LPair/LPair.h', 'w')
cin.write('// Generated automatically with Gen/LPair/python/LPair.py.'
          + '\n// Do not manually modify. Based on ' + ver['LPAIR'] + '.\n')
cin.write('extern "C" {\n\n')
src = Fortran.Source('../src/zduevt.f')
cin.write(src.procedures['zduevt'].cexpr() + '\n\n')
src = Fortran.Source('../src/zduini.f')
cin.write(src.procedures['zduini'].cexpr() + '\n\n')
for k, c in src.commons.iteritems(): cin.write(c.cexpr() + '\n\n')
cin.write('}\n')
