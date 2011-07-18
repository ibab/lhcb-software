import sys, os

class TerminalException(Exception): pass

def getSize():
    """
    compute the proper horizontal and vertical character sizes of a terminal
    """
    if sys.platform != "win32" :
        # Linux part
        def ioctl_GWINSZ(fd):
            try:
                import fcntl, termios, struct #@UnresolvedImport
                cr = struct.unpack('hh', fcntl.ioctl(fd, termios.TIOCGWINSZ, '1234'))
            except TerminalException:
                return None
            return cr
        cr = ioctl_GWINSZ(0) or ioctl_GWINSZ(1) or ioctl_GWINSZ(2)
        if not cr:
            try:
                fd = os.open(os.ctermid(), os.O_RDONLY)
                cr = ioctl_GWINSZ(fd)
                os.close(fd)
            except TerminalException:
                pass
        if not cr:
            try:
                cr = (os.environ['LINES'], os.environ['COLUMNS'])
            except TerminalException:
                cr = (25, 80)
        return int(cr[1]), int(cr[0])
    else :
        # windows part
        from ctypes import windll, create_string_buffer

        # stdin handle is -10
        # stdout handle is -11
        # stderr handle is -12

        h = windll.kernel32.GetStdHandle(-12) #@UndefinedVariable
        csbi = create_string_buffer(22)
        res = windll.kernel32.GetConsoleScreenBufferInfo(h, csbi) #@UndefinedVariable

        if res:
            import struct
            (_bufx, _bufy, _curx, _cury, _wattr, left, top, right, bottom, _maxx, _maxy) = struct.unpack("hhhhHhhhhhh", csbi.raw)
            sizex = right - left + 1
            sizey = bottom - top + 1
        else:
            sizex, sizey = 80, 25 # can't determine actual size - return default values

        return sizex, sizey
