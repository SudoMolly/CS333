#!/bin/bash
#
#

show_help()
{
    echo "HELP"
    exit 0
}

parse()
{
    exval=$1
    ret=0
    case "$exval" in
        0)
            echo "exit success"
            ret=0
            ;;
        1)
            echo "no argument"
            ret=1
            ;;
        2)
            echo "invalid argument"
            ret=2
            ;;
        3)
            echo "unknown exit_value"
            ret=3
            ;;
    esac
}

while read LINE
do
    V=0
    H=1
    E=1
    B=0
    for VAL in ${LINE}
    do
        while getopts "he:b:v" OPT
        do
            case "$OPT" in 
                h)
                    H=1
                    ;;
                e)
                    E=${OPTARG}
                    H=0
                    ;;
                b)
                    B=${OPTARG}
                    H=0
                    ;;
                v)
                    V=1
                    H=0
                    ;;
            esac
        done
        if [ $H -eq '1' ]
        then
            show_help
        fi
        for B in E
        do
            ./hydra "$[($OPTIND)]"
            parse "$?"
        done
    done
    echo "DONE"
done




