#!/usr/bin/bash
# R Jesse Chaney
# rchaney@pdx.edu

TEST_DIR=Testing
NUM_FILES=
NUM_DIRS=3

JDIR=~rchaney/Classes/cs333/Labs/Lab6
#JDIR=~rchaney/Classes/cs333/Labs/src/rockem
SDIR=.
START_DIR=${PWD}

while getopts "hfn:d:S:" OPT
do
    case $OPT in
        h)
            echo -e "options are \n\t-h help \n\t-f to force recreation \n\t-n # for number of randon and zero files to create"
            exit 0
            ;;
        n)
            NUM_FILES="-n $OPTARG -f"
            echo "*** number of files set to ${NUM_FILES}"
            ;;
        d)
            #set -x
            NUM_DIRS=$OPTARG
            rm -rf ${TEST_DIR}/Client?
            #exit
            ;;
        S)
            SDIR=$OPTARG
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

#set -x


echo "SDIR=${SDIR}"
if [ ! -d ${SDIR} ]
then
    echo "The directory ${SDIR} does not exist"
    exit 1
fi

if [ ${PWD} != ${JDIR} ]
then
    echo "Replacing create_random_files.bash"
    ln -fs ${JDIR}/create_random_files.bash .
fi

pushd ${SDIR}
${START_DIR}/create_random_files.bash ${NUM_FILES}

#cd ${SDIR}
#pushd ${SDIR}
#rm -rf ${TEST_DIR}
if [ ! -d ${TEST_DIR} ]
then
    mkdir ${TEST_DIR}
fi
cd ${TEST_DIR}

for D in $(seq ${NUM_DIRS})
do
    if [ ! -d Client${D} ]
    then
        mkdir Client${D}
    fi
    echo "Creating directory for test client Client${D}"
    cd Client${D}

    rm -f  C?_zeroes*.dat S_*.dat
    if [ -e ${START_DIR}/rockem_client ]
    then
        ln -sf  ${START_DIR}/rockem_client .
    else
        echo "  Client${D}: link to rockem_client not created, does not exist"
        echo "    looking in ${START_DIR}"
        echo "    You should build the client."
    fi
    for F in ../../zeroes*.dat
    do
	    TEST_FILE=$(basename ${F})
        SHA_FILE=C${D}_$(basename ${TEST_FILE} .dat).sha256
	    cp $F ./C${D}_${TEST_FILE}
        sha256sum < ./C${D}_${TEST_FILE} > ./${SHA_FILE}
    done

    sync ; sync ; sleep 2
    #sha256sum < C${D}_zeroes*.dat > C${D}_zeroes.sha256
    cd ..
    echo "Client${D} done"
done


if [ -e ${START_DIR}/rockem_server ]
then
    ln -sf ${START_DIR}/rockem_server .
else
    echo "  Link to rockem_server not created, does not exist"
    echo "    You should build the server."
fi

rm -f C?_zeroes*.dat S_*.dat
for F in ../random*.dat
do
    TEST_FILE=$(basename ${F})
    SHA_FILE=S_$(basename ${TEST_FILE} .dat).sha256
    cp ${F} ./S_${TEST_FILE}
    sha256sum < ./S_${TEST_FILE} > ./${SHA_FILE}
done
sync ; sync ; sleep 2
#sha256sum S_random*.dat > random.sha256

echo "server done"

#cd ..
popd
