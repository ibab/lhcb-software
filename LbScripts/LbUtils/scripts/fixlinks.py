from LbUtils.Links import fixLinks

import sys

if __name__ == '__main__':
    for d in sys.argv[1:]:
        fixLinks(d)
