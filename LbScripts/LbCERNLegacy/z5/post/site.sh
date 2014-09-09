if [ ! -n "$X509_CERT_DIR" ]; then
   #export X509_CERT_DIR=/afs/cern.ch/project/gd/LCG-share/current/external/etc/grid-security/certificates
   export X509_CERT_DIR=/etc/grid-security/certificates
fi

if [ ! -n "$X509_VOMS_DIR" ]; then
   #export X509_VOMS_DIR=/afs/cern.ch/project/gd/LCG-share/current/external/etc/grid-security/vomsdir
   export X509_VOMS_DIR=/etc/grid-security/vomsdir
fi
