#!/bin/bash
# R Jesse Chaney
# rchaney@pdx.edu

NUM_FILES=10

while getopts "hfn:" OPT
do
    case $OPT in
        h)
            echo -e "options are \n\t-h help \n\t-f to force recreation \n\t-n # for number of randon and zero files to create"
            exit 0
            ;;
        n)
            NUM_FILES=$OPTARG
            echo "*** number of files set to ${NUM_FILES}"
            ;;
        f)
            echo "*** forcing file creation"
            rm -f random*.dat zeroes*.dat
            rm -f .create_files_done
            ;;
        \?)
            echo "Invalid option" >&2
            exit 1
            ;;
    esac
done

if [ -e .create_files_done ]
then
    echo "*** files already created ***"
    #echo "*** if you want to force recreation, 'rm .create_files_done' ***"
    echo "*** if you want to force recreation, use the -f option on the command line ***"
    echo "*** then, rerun this script ***"
    exit 0
fi

RAND_FILE=random
ZER_FILE=zeroes
RDEVICE=/dev/urandom
ZDEVICE=/dev/zero

#set -x

for FILECOUNT in $(seq ${NUM_FILES})
do
    FILESIZE=${RANDOM}
    FILESIZE=$((FILESIZE + 1024 * FILECOUNT))
    base64 ${RDEVICE} | head -c "$FILESIZE" > ${RAND_FILE}${FILECOUNT}.dat
    chmod a+r ${RAND_FILE}${FILECOUNT}.dat

    FILESIZE=${RANDOM}
    FILESIZE=$((FILESIZE + 2048 * FILECOUNT))
    base64 ${ZDEVICE} | head -c "$FILESIZE" > ${ZER_FILE}${FILECOUNT}.dat
    chmod a+r ${ZER_FILE}${FILECOUNT}.dat
done

FILESIZE=5000000
base64 ${RDEVICE} | head -c "$FILESIZE" > random_big.dat
chmod a+r random_big.dat

FILESIZE=7500000
base64 /dev/zero | head -c "$FILESIZE" > zeroes_big.dat
chmod a+r zeroes_big.dat

touch .create_files_done
echo "*** done ***"
