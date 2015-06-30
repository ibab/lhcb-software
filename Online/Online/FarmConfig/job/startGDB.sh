#!/bin/bash
sleep 10;

/home/frankm/bin/debug2 --pid ${1} <<EOF>/tmp/gdb.${1}.log
b Thread.cpp:345
cont
bt
p t
p *t
cont
bt
p t
p *t
cont
bt
p t
p *t
cont
bt
p t
p *t
cont
bt
p t
p *t
cont
bt
p t
p *t
p thread
p *thread
info thr
thread apply all where
cont
bt
p thread
p *thread
info thr
thread apply all where
cont
bt
p thread
p *thread
info thr
thread apply all where
cont
bt
p thread
p *thread
info thr
thread apply all where
cont
bt
p thread
p *thread
info thr
thread apply all where
cont
bt
p thread
p *thread
info thr
thread apply all where
cont
bt
p thread
p *thread
info thr
thread apply all where
cont
bt
p thread
p *thread
info thr
thread apply all where
cont
bt
p thread
p *thread
info thr
thread apply all where
cont
bt
p thread
p *thread
info thr
thread apply all where
cont
bt
p thread
p *thread
info thr
thread apply all where
cont
bt
info thr
thread apply all where
cont
bt
info thr
thread apply all where

detach
quit
y
EOF


