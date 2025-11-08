#!/bin/bash
# R. Jesse Chaney
# rchaney@pdx.edu

PROG1=mystat
CLASS=cs333
TERM=f25
LAB=Lab1

JDIR=~rchaney/Classes/${CLASS}/Labs/src/${PROG1}
JDIR=~rchaney/Classes/${CLASS}/Labs/${LAB}
#JDIR=~rchaney/Classes/${CLASS}/Labs/mystat_files

SDIR=.
JPROG1=${JDIR}/${PROG1}
SPROG1=${SDIR}/${PROG1}

DIFF=diff
DIFF_OPTIONS=" -w"
#DIFF_OPTIONS=" -w -i"

VERBOSE=0
TOTAL_POINTS=0
#VIEW_POINTS=0
CLEANUP=1
FILE_HOST=babbage

show_help()
{
    echo "${SCRIPT}"
    echo "    -C : do not delete all the various test files. Automatic if a test fails."
    echo "    -v : provide some verbose output. Currently, this does nothing...  :-("
    echo "    -x : see EVERYTHING that is going one. LOTS of output. Hard to understand."
    echo "    -h : print this AMAZING help text."
}

while getopts "xvCh" opt
do
    case $opt in
	    C)
	        # Skip removal of data files
	        CLEANUP=0
	        ;;
	    v)
	        # Print extra messages.
	        VERBOSE=1
	        ;;
	    x)
	        # If you really, really, REALLY want to watch what is going on.
	        echo "Hang on for a wild ride."
	        set -x
	        ;;
        h)
            show_help
            exit 0
            ;;
	    \?)
	        echo "Invalid option" >&2
	        echo ""
	        show_help
	        exit 1
	        ;;
	    :)
	        echo "Option -$OPTARG requires an argument." >&2
            show_help
	        exit 1
	        ;;
    esac
done

make clean
make clean mystat

./mk_funny.bash


if [ $? -ne 0 ]
then
    echo "make failed! You need to fix that."
    echo "0 points"
    exit
fi

if [ ! -x ${SPROG1} ]
then
    echo "Ooopsie! Where is ${SPROG1}?"
    echo "0 points"
    exit
fi

FUNNYS="FUNNYbroken FUNNYdir FUNNYhardlink FUNNYnoaccess FUNNYpipe FUNNYregfile FUNNYsocket FUNNYsymlink"
DEV="/dev/mem /dev/vda"

for FUNNY in ${FUNNYS}
do
#    ${JPROG1} ${JDIR}/${FUNNY} > J_${FUNNY}.out
#    ${SPROG1} ${JDIR}/${FUNNY} > S_${FUNNY}.out

    ${JPROG1} ./${FUNNY} > J_${FUNNY}.out
    ${SPROG1} ./${FUNNY} > S_${FUNNY}.out

    ${DIFF} -q ${DIFF_OPTIONS} J_${FUNNY}.out S_${FUNNY}.out > D_${FUNNY}.out 2> D_${FUNNY}.err

    if [ $? -eq 0 ]
    then
        ((TOTAL_POINTS+=5))
        echo "Excellent!  TOTAL_POINTS=${TOTAL_POINTS}   ${FUNNY} passes"
    else
        echo -e "Files differ. Try this:\n\t${DIFF} ${DIFF_OPTIONS} J_${FUNNY}.out S_${FUNNY}.out"
        echo -e "\tor this: vimdiff J_${FUNNY}.out S_${FUNNY}.out"
    fi
done

for FUNNY in ${DEV}
do
    FNAME=$(basename ${FUNNY})
    ${JPROG1} ${FUNNY} > J_${FNAME}.out
    ${SPROG1} ${FUNNY} > S_${FNAME}.out

    ${DIFF} -q ${DIFF_OPTIONS} J_${FNAME}.out S_${FNAME}.out > D_${FNAME}.out 2> D_${FNAME}.err

    if [ $? -eq 0 ]
    then
        ((TOTAL_POINTS+=5))
        echo "Excellent!  TOTAL_POINTS=${TOTAL_POINTS}   ${FUNNY} passes"
    else
        echo -e "Files differ. Try this:\n\t${DIFF} ${DIFF_OPTIONS} J_${FNAME}.out S_${FNAME}.out"
        echo -e "\tor this: vimdiff J_${FNAME}.out S_${FNAME}.out"
    fi
done

FUNNIES=""
for FUNNY in ${FUNNYS}
do
    FUNNIES+="./${FUNNY} "
done


${JPROG1} ${FUNNIES} > J_ALL.out
${SPROG1} ${FUNNIES} > S_ALL.out

${DIFF} -q ${DIFF_OPTIONS} J_ALL.out S_ALL.out > D_ALL.out 2> D_ALL.err

if [ $? -eq 0 ]
then
    ((TOTAL_POINTS+=50))
    echo -e "\nSuper excellent!  TOTAL_POINTS=${TOTAL_POINTS}   ALL passes"
else
    echo "** Are you processing ALL the files on the command line? **"
    echo -e "Files differ. Try this:\n\t${DIFF} ${DIFF_OPTIONS} J_${FUNNY}.out S_${FUNNY}.out"
    echo -e "\tor this: vimdiff J_${FUNNY}.out S_${FUNNY}.out"
fi

if [ ${CLEANUP} -ne 0 ]
then
    rm -f J_*.out S_*.out D_*.{out,err}
fi
