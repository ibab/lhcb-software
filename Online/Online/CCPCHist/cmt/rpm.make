#
# makefile for the CCCPHist rpm 
#
# author: niko.neufeld@cern.ch 
#
# $Id: rpm.make,v 1.5 2007-04-02 15:03:59 jost Exp $
#
SHELL=/bin/bash
RANLIB=$(CCPCROOT)/usr/bin/ranlib
CC=$(CCPCROOT)/usr/bin/cc
LD=$(CCPCROOT)/usr/bin/ld

LIBNAME=CCPCHist
PKGNAME=CCPCHist
ARLIB=lib$(LIBNAME).a
SHLIB=lib$(LIBNAME).so
INCDIR=../$(PKGNAME)
BINDIR=../$(CMTCONFIG)

# change this for new version
LBVERSION=v2r0

SHLIBNAME=lib$(LIBNAME).so.$(LBVERSION)
ARLIBNAME=lib$(LIBNAME).a.$(LBVERSION)

INSTALLDIR=$(ROOT)/$(CCPCROOT)/usr/local

DIST=$(PKGNAME)-$(LBVERSION)
TEMPSPEC=$(PKGNAME).spec
SPEC=$(PKGNAME)-$(LBVERSION)-$(RPMRELEASE).spec
RPMRELEASE=1
RPMBUILDDIR=/tmp/$(PKGNAME)-rpmbuild
RPMREPO=/afs/cern.ch/lhcb/project/web/online/ccpc/slc4X/ccpc


all: rpm

dist:
	rm -fr $(DIST) $(DIST).tar $(DIST).tar.bz2
	mkdir $(DIST)
	  cp $(BINDIR)/lib$(PKGNAME).*   $(DIST)
	  cp $(INCDIR)/hist_types.h $(INCDIR)/CCPCHist.h  $(DIST)
	  tar -cf $(DIST).tar $(DIST)
	  bzip2 $(DIST).tar
	  rm -fr $(DIST)

spec: $(SPEC)

$(SPEC): $(TEMPSPEC)
	  echo -e "s/Version:/& $(LBVERSION)/"'\n'"s/Release:/& $(RPMRELEASE)/"'\n''/\%files/a\\\n' |  sed -f - $< > $@

.PHONY: clean  distclean installrpm

rpm: $(SPEC) dist
	  rm -fr $(RPMBUILDDIR)
	  mkdir $(RPMBUILDDIR)
	  cd $(RPMBUILDDIR); mkdir SPECS SOURCES RPMS SRPMS BUILD
	  cp -f $(SPEC) $(RPMBUILDDIR)/SPECS
	  cp -f $(DIST).tar.bz2  $(RPMBUILDDIR)/SOURCES
	  cd $(RPMBUILDDIR) ; rpmbuild --define "_topdir $(RPMBUILDDIR)" -ba\
	  SPECS/$(SPEC)   

installrpm: rpm
	  cp $(RPMBUILDDIR)/SRPMS/$(PKGNAME)*.rpm $(RPMREPO)/SRPMS
	  cp $(RPMBUILDDIR)/RPMS/i386/$(PKGNAME)*.rpm $(RPMREPO)/RPMS
	  createrepo $(RPMREPO)/RPMS
	  createrepo $(RPMREPO)/SRPMS


clean:
	  $(RM) $(OBJECTS) *.o $(ARLIB) $(SHLIB) $(APPS) $(DIST).tar.bz2 $(SPEC) $(SHLIBSO)
	  $(RM) doc/html/ doc/latex/ doc/man/ -R

distclean: clean 
	  $(RM)  *~ *.bak core




