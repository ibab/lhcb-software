"""Utilities to get file paths"""

def get_digit(run, digit):
    """Get digit from run number"""
    return str(int(run))[-digit]

def get_mult_10(num, nzeros):
    """Append nzeros to num"""
    return str(num) + '0'*(nzeros)

def if_ndigits(run, ndigits):
    """Test if run number is ndigits long"""
    return len(str(run)) == ndigits

def make_dir_tree(run, prefix=''):
    """Make directory tree from run number grouped by decades"""
    length = len(str(run))
    tree, digit = '', ''
    for i in xrange(length, 2, -1):
        digit += get_digit(run, i)
        tree += get_mult_10(digit, i-1) + 's/'
    tree = tree + str(run)
    if prefix: return prefix + '/{0}'.format(tree)
    else: return tree

def get_last_run(runfile):
    """Return last run from runfile"""
    runfile = open(runfile, 'r')
    return int(runfile.readlines()[-1])
