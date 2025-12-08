#!/bin/bash
# Beginning sample script
#
# R Jesse Chaney
# rchaney@pdx.edu

make clean all
set -m
prog=example1

./$prog
./$prog -h
./$prog -n 
./$prog -n "-12"

./$prog -vn 5
./$prog -vn "-1" 1

./$prog -vn 9223372036854775807
function kill_prog () 
{
    local type=$1
    ./$prog -s &
    PID=`pgrep  $prog`
    kill -$type $!
}
kill_prog "SIGINT"
kill_prog "SIGQUIT"
kill_prog "SIGHUP"
kill_prog "SIGUSR1"
kill_prog "SIGUSR2"
kill_prog "SIGTERM"

U=`ulimit -v 1 > /dev/null; ./$prog -n 100000`
echo U=${U}

gcov -c example1
