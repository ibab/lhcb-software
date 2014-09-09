if ( ! $?X509_CERT_DIR ) then
   #setenv X509_CERT_DIR /afs/cern.ch/project/gd/LCG-share/current/external/etc/grid-security/certificates
   if ( -d /etc/grid-security/certificates) then
      setenv X509_CERT_DIR /etc/grid-security/certificates
   endif
endif

if ( ! $?X509_VOMS_DIR ) then
   #setenv X509_VOMS_DIR /afs/cern.ch/project/gd/LCG-share/current/external/etc/grid-security/vomsdir
   if ( -d /etc/grid-security/certificates) then
      setenv X509_VOMS_DIR /etc/grid-security/vomsdir
   endif
endif
