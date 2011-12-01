cd /group/online/dataflow/cmtuser/BitTorrent/Online/TorrentLoader/cmt;
. setup.x86_64-slc5-gcc43-dbg.vars;
export torrent_dir=`dirname $1`;
export torrent_file=$1;
shift;
#####cd ${torrent_dir};
#
# usage: make_torrent FILE [OPTIONS]
#
# Generates a torrent file from the specified file
# or directory and writes it to standard out
#
# OPTIONS:
# -m file     generate a merkle hash tree torrent.
#             merkle torrents require client support
#             the resulting full merkle tree is written to
#             the specified file
# -w url      adds a web seed to the torrent with
#             the specified url
# -t url      adds the specified tracker to the
#             torrent
# -p bytes    enables padding files. Files larger
#             than bytes will be piece-aligned
# -s bytes    specifies a piece size for the torrent
#             This has to be a multiple of 16 kiB
# -l          Don't follow symlinks, instead encode them as
#             links in the torrent file
# -o file     specifies the output filename of the torrent file
#             If this is not specified, the torrent file is
#             printed to the standard out, except on windows
#             where the filename defaults to a.torrent
#
# 128kB=131072 - 256kB=262144 - 512kB=524288
# 1MB=1048576 - 2 MB=2097152 - 4MB= - 8 MB=8388608 -  16 MB=16777216 - 32 MB=33554432
#
echo "Torrent file:${torrent_file}"
#echo "Torrent directory:${torrent_dir}"
# echo `which make_torrent` ${torrent_file} -o ${torrent_file}.torrent $*;
`which make_torrent` ${torrent_file} -o ${torrent_file}.torrent $*;
