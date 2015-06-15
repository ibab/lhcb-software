#!/usr/bin/env python
"""
Author  : Philip Ilten
Created : 2015-06-12

Script to generate SUPERCHIC code that can be used with Gauss. The
following changes are made from the vanilla SUPERCHIC source that is
downloaded.

1. The SUPERCHIC program is changed to a subroutine.
2. All variables not saved or stored in a common block in SUPERCHIC
   are added to the common block SCCOM.
3. Entry and exit points to the SUPERCHIC subroutine are generated.
4. All write statemements are made optional.
5. The random number generator, R2455, is replaced with the Gauss
   random number generator.
"""

import os, urllib2, Fortran
ver = 'superchicv1.47.f'
url = 'https://superchic.hepforge.org/'

# Download the FORTRAN source if needed.
if not os.path.isfile(ver):
    info = urllib2.urlopen(url + ver)
    text = info.read()
    code = file(ver, 'w')
    code.write(text)
    code.close()
org = [l.rstrip('\n').lower() for l in file(ver, 'r')]

# Generate common block code to access and save the SUPERCHIC state.
src = Fortran.Source(ver)
vrs = src.procedures['superchic'].filter(exclude=['DATA', 'IN-COMMON', 'SAVE'])
com = '      common/sccom/'
wdt = len(com)
for v in vrs:
    v = v.name + ','
    if wdt + len(v) >= 72: v = '\n     &' + v; wdt = len(v) - 1
    else: wdt += len(v)
    com += v
com = com[:-1]

# Define the SUPERCHIC modification dictionary (string values are replaced).
dct = {
    # Change SUPERCHIC to a subroutine.
    1:    "      subroutine superchic(entryp, exitp)",
    43:  ("      double precision pin(4),am(100),pout(4,100),wt,ein\n" +
          "      integer entryp,exitp,itr1,itr2\n      logical outevt,outhst\n" 
          "      common/output/outevt,outhst\n" + com),
    # Replace the weighted/un-weighted loop iterator.
    411:  "      do itr1=1,2",
    413:  "      if (ll.ge.0) then\n      ll = itr1\n      if(ll.eq.1)then",
    419:  "      endif\n      else\n      ll = -ll\n      endif",
    5237: "      if (ll.ge.2) then\n      exit\n      endif",
    # Replace the event loop iterator.
    982:  "      do itr2=1,ntotal",
    983:  ("      if (i.ge.0) then\n      i = itr2\n" + 
           "      else\n      i = -i\n      endif"),
    4680: "      if (itr2.ge.ntotal) then\n      exit\n      endif",
    # Change STOP to RETURN.
    5826: " 889  return",
    # Define output common block for HISTO1.
    5987: ("      save\n      logical outevt,outhst\n" +
           "      common/output/outevt,outhst"),
    # Redefine the random number generator.
    7819: ("      subroutine r2455(ran)\n      real*8 ran,cpyr\n" +
           "      integer idummy\n      ran = cpyr(idummy)\n      end"),
    # Define the entry/exit point lines (< 0 is before, > 0 after).
    215:    0, 5827: -1, # Full SUPERCHIC block.
    221:   -2,  247:  3, # Define xflag, chiflag, eflag, decay, mh, and idh.
    249:   -4,  281:  5, # Generate PS for decay = 4.
    285:   -6,  310:  7, # Define gen, surv, corr, chiv, photo, pflag, phdecays.
    314:   -8,  346:  9, # Generate PS for phdecay = 'psipi'.
    352:  -10,  373: 11, # Define pdf, cross, a2g, and ecut.
    380:  -12,  404: 13, # Setup the COM energy.
    408:  -14,  410: 15, # Define num and nev.
    410:  -16, 5239: 17, # Outside the weighted/un-weighted generation loop.
    414:  -18, 5238: 19, # Inside the weighted/unweighted generation loop.
    981:  -20, 4682: 21, # Outside the actual event generation loop.
    984:  -22, 4681: 23, # Inside the actual event generation loop.
    4666: -24, 4674: 25, # Outside the event generation termination.
    4669: -26, 4672: 27  # Inside the event generation termination.
    }

# Create the goto code for entry points.
pnt = []
for k,v in dct.iteritems():
    if type(v) == int: pnt += [v]
pnt = sorted(pnt); l = ''
for p in pnt:
    l += ("\n      if (entryp.eq.%i) then\n      goto %i\n      endif" % 
          (abs(p), 2000 + abs(p)))
dct[43] = dct[43] + l

# Generate the modified FORTRAN source.
mod = file('../src/SuperChic.f', 'w')
mod.write('c Generated automatically with Gen/SuperChic/python/SuperChic.py.'
          + '\nc Do not manually modify. Based on ' + ver + '.\n')
cnt = ''
for i, l in enumerate(org):
    # Skip R2455 lines.
    if (i + 1 >= 7820) and (i + 1 <= 7851): continue
    # Handle line continuations.
    l = cnt + l
    if i + 1 < len(org) and org[i + 1].startswith("     &"):
        cnt = l + "\n"; continue
    else: cnt = ""
    # Change all exerec.dat write statements.
    if ('open(30' in l) or ('close(30,' in l) or ('write(30' in l):
        l = "      if (outevt) then\n" + l + "\n      endif"
    # Change all outout.dat write statements.
    if ('open(10' in l) or ('close(10,' in l) or ('write(10' in l):
        l = "      if (outhst) then\n" + l + "\n      endif"
    # Apply the modification dictionary.
    if i + 1 in dct:
        v = dct[i + 1]
        if type(v) == str: l = v
        elif type(v) == int:
            if v < 0:
                l = (" " + str(2000 + abs(v)) + " continue\n      " +
                     ("if (exitp.eq.%i) then\n      return\n      endif\n" % 
                      abs(v)) + l)
            else:
                l = (l + "\n " + str(2000 + abs(v)) + " continue\n      " +
                     ("if (exitp.eq.%i) then\n      return\n      endif" % 
                      abs(v)))
    mod.write(l + '\n')
mod.close()

# Generate the C-interface.
src = Fortran.Source('../src/SuperChic.f')
cin = file('../SuperChic/SuperChic.h', 'w')
cin.write('// Generated automatically with Gen/SuperChic/python/SuperChic.py.'
          + '\n// Do not manually modify. Based on ' + ver + '.\n')
cin.write('extern "C" {\n\n')
cin.write(src.procedures['superchic'].cexpr() + '\n\n')
for k, c in src.commons.iteritems():
    if k != 'hepevt': cin.write(c.cexpr() + '\n\n')
cin.write('}\n')

