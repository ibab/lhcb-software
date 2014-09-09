if [ ! -n "$X509_CERT_DIR" ]; then
   #export X509_CERT_DIR=/afs/cern.ch/project/gd/LCG-share/current/external/etc/grid-security/certificates
   if [ -d /etc/grid-security/certificates ]; then
      export X509_CERT_DIR=/etc/grid-security/certificates
   fi
fi

if [ ! -n "$X509_VOMS_DIR" ]; then
   #export X509_VOMS_DIR=/afs/cern.ch/project/gd/LCG-share/current/external/etc/grid-security/vomsdir
   if [ -d /etc/grid-security/vomsdir ]; then
      export X509_VOMS_DIR=/etc/grid-security/vomsdir
   fi
fi
