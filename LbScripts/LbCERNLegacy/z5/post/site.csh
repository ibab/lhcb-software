if ( ! $?X509_CERT_DIR ) then
   setenv X509_CERT_DIR /afs/cern.ch/project/gd/LCG-share/current/external/etc/grid-security/certificates
endif

if ( ! $?X509_VOMS_DIR ) then
   setenv X509_VOMS_DIR /afs/cern.ch/project/gd/LCG-share/current/external/etc/grid-security/vomsdir
endif
