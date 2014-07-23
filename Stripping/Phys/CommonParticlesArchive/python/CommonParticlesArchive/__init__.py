
def isdir( z, name ):
    return any(x.startswith("%s/" % name.rstrip("/")) for x in z.namelist())

def isValidArchive(path):

    zipname = "python.zip"

    if not zipname in path :

        # Standard file system
        import os
        return os.path.exists(pth)

    else :

        # Python zip file. So manually handle this ..
        import zipfile
        splits = path.split(zipname+"/")
        zfile = splits[0] + zipname
        entry = splits[1]
        z = zipfile.ZipFile( zfile, 'r' )
        return isdir(z,entry)

def redirectCommonParticles(stripping):
    import os, sys

    # Construct the archive python path from this modules path
    pth = os.path.sep.join( __file__.split(os.path.sep)[0:-1] + [stripping] )

    # Check this is a known archived version
    if not isValidArchive(pth) :
        raise Exception( "CommonParticles Archive '"+pth+"' is not found" )

    # Append the archive path, so it is picked up first
    sys.path.insert(0,pth)
