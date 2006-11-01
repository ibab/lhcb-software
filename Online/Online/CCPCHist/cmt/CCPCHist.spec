Summary: A simple histogramming library (not only) for the CCPCs
Name: CCPCHist
Version: 
Release: 
URL: http://cern.ch/lhcb-online/ecs/ccpc
Source0: %{name}-%{version}.tar.bz2
#Patch: 
License: GPL
Group: LHCb Online/ccpc 
BuildRoot: %{_tmppath}/%{name}-root
Packager: Niko Neufeld <niko.neufeld@cern.ch>

%description
A GAUCHO compatible histogramming library
%prep
%setup -q
#%patch -p 1
%build
# Currently no build step needed. Will promote this to proper build using CMT
#make ROOT="$RPM_BUILD_ROOT" 
%install
rm -rf $RPM_BUILD_ROOT 
mkdir -p $RPM_BUILD_ROOT/usr/local/lib
mkdir -p $RPM_BUILD_ROOT/usr/local/include
cp *.h $RPM_BUILD_ROOT/usr/local/include
cp lib* $RPM_BUILD_ROOT/usr/local/lib

%files
%defattr(-,root,root,-)
/usr/local/lib/
/usr/local/include/

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%changelog
* Wed Nov  1 2006 Niko NEUFELD <niko@lbonccpc01.cern.ch> - v2r1-1
- Initial build




