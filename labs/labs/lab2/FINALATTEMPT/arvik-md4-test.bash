#!/bin/bash
# R Jesse Chaney
# rchaney@pdx.edu

DIR=arvik
PROG=${DIR}-md4
ARVIK_EXT=arvik

SUM=md5sum
CLASS=cs333
POINTS=0
TOTAL_POINTS=387
POINTS_PER_TEST=16
POINTS_PER_BAD=7
CLEANUP=1
WARNINGS=0
FILE_HOST=babbage
DIFF=diff
#DIFF_OPTIONS="-B -w -i"
DIFF_OPTIONS="-w"
#VIMDIFF_OPTIONS="-c 'set diffopt+=iwhite'"
VIMDIFF="vimdiff"

RANDOM_FILE_SIZE=${RANDOM_FILE_SIZE:-50000000}

COWSAY=/usr/games/cowsay

FAIL_COUNT=0
LAB=Lab2
VERBOSE=0
CORE_COUNT=0
VALGRIND=valgrind
NOLEAKS="All heap blocks were freed -- no leaks are possible"
LEAKS_FOUND=0

TO=10s
TOS="-s QUIT --kill-after=60 --preserve-status"

#SDIR=${PWD}
SDIR=.
#JDIR=~rchaney/Classes/${CLASS}/Labs/src/${DIR}
JDIR=~rchaney/Classes/${CLASS}/Labs/${LAB}

SPROG=${SDIR}/${PROG}
JPROG=${JDIR}/${PROG}

CORRUPT_FILE=JUNK

signalCaught()
{
    echo "++++ caught signal while running script ++++"
}

signalCtrlC()
{
    echo "Caught Control-C"
    echo "You will neeed to clean up some files"
    exit
}

signalSegFault()
{
    echo "+++++++ Caught Segmentation Fault from your program! OUCH!  ++++++++"
}

coreDumpMessage()
{
    if [ $1 -eq 139 ]
    then
        echo "      >>> core dump during $2 testing"
        ((CORE_COUNT++))
    elif [ $1 -eq 137 ]
    then
        echo "      >>> core dump during $2 testing"
        ((CORE_COUNT++))
    elif [ $1 -eq 134 ]
    then
        echo "      >>> abort during $2 testing"
        ((CORE_COUNT++))
    elif [ $1 -eq 124 ]
    then
        echo "      >>> timeout during $2 testing"
    #else
        #echo "$1 is a okay"
    fi
    sync
}

chmodAndTimeStampFiles()
{
    #chmod a+r Constitution.txt Iliad.txt jargon.txt words.txt ?-s.txt
    #local GROUP=$(groups | cut -d ' ' -f 2)

	chmod a+r Constitution.txt Iliad.txt jargon.txt words.txt ?-s.txt text-*k.txt
	chmod g+wr,o+r,u-w text-*.txt
	chmod a+rx *.bin
	chmod g+w random-333.bin
	chmod a-x zeroes-1M.bin
	chmod o-r zeroes-1M.bin
	chmod og-w,g-x,og+r zeroes-4M.bin
	chmod g-r random-2M.bin
	chmod u-w,g+x,o+w [012]-s.txt
	chmod u+rx,g+x,g-w,o+rx [3-6]-s.txt
    #if [ -n "${GROUPS}" ]
    #then
    #    # not al studnets in in more than 1 group
	#    chgrp ${GROUP} ?-s.txt
    #fi

	touch -t 200110111213.14 text-5k.txt
	touch -t 197009011023.44 text-75k.txt
	touch -t 199912122358.59 words.txt
	touch -t 197805082150.59 Iliad.txt
	touch -t 202112110217.44 jargon.txt
	touch -t 198012050303.06 ran*.bin
	touch -t 197507181910.36 [01]-s.txt
	touch -t 196608040311.36 [23]-s.txt
	touch -t 198706070809.36 [45]-s.txt
	touch -t 199003030303.03 6-s.txt
	touch -t 195301011532.57 zeroes-4M.bin
	touch -t 195102071532.57 zeroes-1M.bin

    chmod og+rx even_file.txt even_file.bin
    chmod og+x odd_file.txt odd_file.bin

    touch -t 199209121525.01 even_file.txt even_file.bin odd_file.txt odd_file.bin zero_big.bin


    sync ; sync ; sync ; sleep 1 ; sync ; sync ; sync ; sleep 1
    
    echo -e "\tFiles chmod-ified and time stamped."
}

randomFiles()
{
    local EVEN_VAL=$(( ${RANDOM_FILE_SIZE} + $RANDOM * 2 ))
    local ODD_VAL=$(( EVEN_VAL + 1 ))

    #echo "even: ${EVEN_VAL}    odd: ${ODD_VAL}"
    
    head -c ${EVEN_VAL} < /dev/urandom          \
        | xxd                            \
        | cut -d ' ' -f2,3,4,5,6,7,8,9   \
        | tr -d ' '                      \
             > even_file.txt

    head -c ${ODD_VAL} < /dev/urandom          \
        | xxd                            \
        | cut -d ' ' -f2,3,4,5,6,7,8,9   \
        | tr -d ' '                      \
             > odd_file.txt
    ODD_SIZE=$(stat --format="%s" odd_file.txt)
    ODD_SIZE=$(( ODD_SIZE % 2 ))
    if [ ${ODD_SIZE} -eq 0 ]
    then
        #echo "adding byte"
        printf '\x01' >> odd_file.txt
    fi

    head -c ${EVEN_VAL} < /dev/urandom         \
         > even_file.bin

    head -c ${ODD_VAL} < /dev/urandom          \
         > odd_file.bin

    head -c ${ODD_VAL} < /dev/zero             \
         > zero_big.bin

    echo -e "\tRandom files created."
}

copyTestFiles()
{
	echo -e "*** Copying test files into current directory."
    
    #rm -f ${PROG}.h
    #ln -s ${JDIR}/${PROG}.h .
    
    rm -f Constitution.txt Iliad.txt jargon.txt text-*k.txt words.txt
    rm -f random-*.bin zeroes-*.bin
    rm -f [0-9]-s.txt

    #echo "cp ${JDIR}/zer*.bin ."
    cp ${JDIR}/*.txt .
    cp ${JDIR}/random-*.bin .
    cp ${JDIR}/zeroes-*.bin .

    #chmodAndTimeStampFiles

	#rm -f corruptTest?.${PROG} goodTest?.${PROG}
    rm -f ${PROG}_*_[JS].${DIR}
    rm -f ${PROG}_*_[JS].{out,err}

    rm -f ${ARVIK_EXT}_*_toc[12].{ar,stoc,diff} ${ARVIK_EXT}_*_toc[12]_*.{ar,stoc,diff}

    randomFiles
    
    chmodAndTimeStampFiles
    
	echo -e "\tTest files copied. Permissions and dates set."

    sync ; sync ; sync
}

build()
{
    BuildFailed=0

    echo -e "\nBuilding ..."

    rm -f .WARN.err  .WARN.out
    rm -f ${DIR}.h
    ln -fs ${JDIR}/${DIR}.h .

    make clean > /dev/null 2> /dev/null
    make clean all 2> .WARN.err > .WARN.out
    RET=$?
    if [ ${RET} -ne 0 ]
    then
        echo "Build failed. Is 'make clean' broken?"
        BuildFailed=1
        return
    fi
    # I could use test -s .WARN.err
    if [ ! -e .WARN.err ]
    then
        # if the student deletes the .WARN.err file, fake warnings
        echo > .WARN.err
    fi
    
    NUM_BYTES=$(wc -c < .WARN.err)
    if [ ${NUM_BYTES} -eq 0 ]
    then
        echo "    You have no compiler warnings messages. Good job."
    else
        echo ">>> You have compiler warnings messages. That is -20 percent!"
        cat .WARN.err
        WARNINGS=1
        ${COWSAY} -f vader "OUCH!!! That hurts! Where is that compiler warnings fixer-upper?"
    fi

    if [ ! -x ${PROG} ]
    then
        BuildFailed=1
    fi
    
    echo -e "Build done ..."
}

testHelp()
{
    echo "Testing help text ..."

    HELP_FILE=${RANDOM}_help
    ${SPROG} -h 2>&1 > ${PROG}_${HELP_FILE}_S.out
    ${JPROG} -h 2>&1 > ${PROG}_${HELP_FILE}_J.out

    ${DIFF} ${DIFF_OPTIONS} ${PROG}_${HELP_FILE}_S.out ${PROG}_${HELP_FILE}_J.out > /dev/null
    if [ $? -eq 0 ]
    then
        ((POINTS+=${POINTS_PER_TEST}))
        echo -e "\thelp text is good"
    else
        echo ">>> help text needs help"
        echo "    https://www.youtube.com/watch?v=2Q_ZzBGPdqE"
        CLEANUP=0
        echo "    Try this: ${DIFF} ${DIFF_OPTIONS} -y ${PROG}_${HELP_FILE}_S.out ${PROG}_${HELP_FILE}_J.out"
        echo "     Or this: ${VIMDIFF} ${VIMDIFF_OPTIONS} ${PROG}_${HELP_FILE}_S.out ${PROG}_${HELP_FILE}_J.out"
    fi

    echo "** Help text done..."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

testTOC()
{
    echo "Testing TOC ..."

    TOC_FILE=${ARVIK_EXT}_${RANDOM}_toc1
    ${JPROG} -cf ${TOC_FILE}.${ARVIK_EXT} [0-9]-s.txt

    ${JPROG} -tf ${TOC_FILE}.${ARVIK_EXT} > ${TOC_FILE}_J1.stoc
    ${SPROG} -f ${TOC_FILE}.${ARVIK_EXT} -t > ${TOC_FILE}_S1.stoc

    ${JPROG} -vtf ${TOC_FILE}.${ARVIK_EXT} > ${TOC_FILE}_J2.ltoc
    ${SPROG} -f ${TOC_FILE}.${ARVIK_EXT} -tv > ${TOC_FILE}_S2.ltoc

    ${DIFF} ${DIFF_OPTIONS} ${TOC_FILE}_J1.stoc ${TOC_FILE}_S1.stoc > ${TOC_FILE}_diff1.diff 2>&1
    if [ $? -eq 0 ]
    then
        ((POINTS+=${POINTS_PER_TEST}))
        echo -e "\tsmall TOC text is good"
        echo -e "\tPOINTS=${POINTS}"
    else
        echo ">>> help TOC needs help"
        CLEANUP=0
        echo "    Try this: ${DIFF} ${DIFF_OPTIONS} -y ${TOC_FILE}_J1.stoc ${TOC_FILE}_S1.stoc"
        echo "     Or this: ${VIMDIFF} ${VIMDIFF_OPTIONS} ${TOC_FILE}_J1.stoc ${TOC_FILE}_S1.stoc"
        echo -e "    Also\n\tyour arvik: ${SPROG} -t -f ${TOC_FILE}.${ARVIK_EXT}"
        echo -e "    \tJesse arvik: ${JPROG} -t -f ${TOC_FILE}.${ARVIK_EXT}"
    fi

    ${DIFF} ${DIFF_OPTIONS} ${TOC_FILE}_J2.ltoc ${TOC_FILE}_S2.ltoc > ${TOC_FILE}_diff2.diff 2>&1
    if [ $? -eq 0 ]
    then
        ((POINTS+=${POINTS_PER_TEST}))
        echo -e "\tverbose TOC text is good"
        echo -e "\tPOINTS=${POINTS}"
    else
        echo ">>> verbose TOC needs help"
        CLEANUP=0
        echo "    Try this: ${DIFF} ${DIFF_OPTIONS} -y ${TOC_FILE}_J2.ltoc ${TOC_FILE}_S2.ltoc"
        echo "     Or this: ${VIMDIFF} ${VIMDIFF_OPTIONS} ${TOC_FILE}_J2.ltoc ${TOC_FILE}_S2.ltoc"
        echo -e "    Also\n\tyour arvik: ${SPROG} -tf ${TOC_FILE}.${ARVIK_EXT} -v"
        echo -e "    \tJesse arvik: ${JPROG} -tf ${TOC_FILE}.${ARVIK_EXT} -v"
    fi

    echo "** TOC tests done..."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

testCreateTextFiles()
{
    echo "Testing create text archives ..."

    local TEST_FAIL=0

    TOC_FILE=${ARVIK_EXT}_${RANDOM}_text1
    chmodAndTimeStampFiles
    F1=${TOC_FILE}_J1.${ARVIK_EXT}
    ${JPROG} -cf ${F1} [0-9]-s.txt
    
    chmodAndTimeStampFiles
    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} -cf ${TOC_FILE}_S1.${ARVIK_EXT} [0-9]-s.txt 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with text files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "    Command was: ${SPROG} -c -f ${TOC_FILE}_S1.${ARVIK_EXT} [0-9]-s.txt"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    ${DIFF} ${DIFF_OPTIONS} ${F1} ${TOC_FILE}_S1.${ARVIK_EXT} > ${TOC_FILE}_diff1.diff 2>&1
    if [ $? -eq 0 ]
    then
        ((POINTS+=${POINTS_PER_TEST}))
        echo -e "\tcreate text archive text 1 is good"
        echo -e "\tPOINTS=${POINTS}"
    else
        echo ">>> create text archive needs help"
        CLEANUP=0
        echo "    Try this: ${DIFF} ${DIFF_OPTIONS} -y ${F1} ${TOC_FILE}_S1.${ARVIK_EXT}"
        echo "     Or this: ${VIMDIFF} ${F1} ${TOC_FILE}_S1.${ARVIK_EXT}"
        echo -e "    Also\n\tyour arvik: ${SPROG} -cf ${TOC_FILE}_S1.${ARVIK_EXT} [0-9]-s.txt"
        echo -e "    \tJesse arvik: ${JPROG} -cf ${F1} [0-9]-s.txt"
    fi


    TOC_FILE=${ARVIK_EXT}_${RANDOM}_text2
    chmodAndTimeStampFiles
    F2=${TOC_FILE}_J1.${ARVIK_EXT}
    ${JPROG} -f ${F2} Iliad.txt jargon.txt text-5k.txt text-75k.txt words.txt -c
    
    chmodAndTimeStampFiles
    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} -f ${TOC_FILE}_S1.${ARVIK_EXT} Iliad.txt jargon.txt text-5k.txt text-75k.txt words.txt -c 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with text files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "    Command was: ${SPROG} -c -f ${TOC_FILE}_S1.${ARVIK_EXT} Iliad.txt jargon.txt text-5k.txt text-75k.txt words.txt"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    ${DIFF} ${DIFF_OPTIONS} ${F2} ${TOC_FILE}_S1.${ARVIK_EXT} > ${TOC_FILE}_diff1.diff 2>&1
    if [ $? -eq 0 ]
    then
        ((POINTS+=${POINTS_PER_TEST}))
        echo -e "\tcreate text archive text 2 is good"
        echo -e "\tPOINTS=${POINTS}"
    else
        echo ">>> create text archive needs help"
        CLEANUP=0
        echo "    Try this: ${DIFF} ${DIFF_OPTIONS} -y ${F2} ${TOC_FILE}_S1.${ARVIK_EXT}"
        echo "     Or this: ${VIMDIFF} ${F2} ${TOC_FILE}_S1.${ARVIK_EXT}"
        echo -e "    Also\n\tyour arvik: ${SPROG} -cf ${TOC_FILE}_S1.${ARVIK_EXT} Iliad.txt jargon.txt text-5k.txt text-75k.txt words.txt"
        echo -e "    \tJesse arvik: ${JPROG} -cf ${F2} Iliad.txt jargon.txt text-5k.txt text-75k.txt words.txt"
    fi


    TOC_FILE=${ARVIK_EXT}_${RANDOM}_text3
    chmodAndTimeStampFiles
    F3=${TOC_FILE}_J1.${ARVIK_EXT}
    ${JPROG} -f ${F3} Iliad.txt jargon.txt text-5k.txt text-75k.txt words.txt -c [0-9]-s.txt
    
    chmodAndTimeStampFiles
    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} -f ${TOC_FILE}_S1.${ARVIK_EXT} Iliad.txt jargon.txt text-5k.txt text-75k.txt words.txt -c [0-9]-s.txt 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with text files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "    Command was: ${SPROG} -f ${TOC_FILE}_S1.${ARVIK_EXT} Iliad.txt jargon.txt text-5k.txt text-75k.txt words.txt -c [0-9]-s.txt"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    ${DIFF} ${DIFF_OPTIONS} ${F3} ${TOC_FILE}_S1.${ARVIK_EXT} > ${TOC_FILE}_diff1.diff 2>&1
    if [ $? -eq 0 ]
    then
        ((POINTS+=${POINTS_PER_TEST}))
        echo -e "\tcreate text archive text 3 is good"
        echo -e "\tPOINTS=${POINTS}"
    else
        echo ">>> create text archive needs help"
        CLEANUP=0
        echo "    Try this: ${DIFF} ${DIFF_OPTIONS} -y ${F3} ${TOC_FILE}_S1.${ARVIK_EXT}"
        echo "     Or this: ${VIMDIFF} ${F3} ${TOC_FILE}_S1.${ARVIK_EXT}"
        echo -e "    Also\n\tyour arvik: ${SPROG} -f ${TOC_FILE}_S1.${ARVIK_EXT} Iliad.txt jargon.txt text-5k.txt text-75k.txt words.txt -c [0-9]-s.txt"
        echo -e "    \tJesse arvik: ${JPROG} -f ${F3} Iliad.txt jargon.txt text-5k.txt text-75k.txt words.txt -c [0-9]-s.txt"
    fi


    TOC_FILE=${ARVIK_EXT}_${RANDOM}_text4
    chmodAndTimeStampFiles
    NESTED_TEXT=${TOC_FILE}_J1.${ARVIK_EXT}
    F4=${TOC_FILE}_J1.${ARVIK_EXT}

    touch -t 198012050303.06 ${F1} ${F2} ${F3}
    ${JPROG} ${F1} -f ${F4} ${F2} -c ${F3}
    
    chmodAndTimeStampFiles
    touch -t 198012050303.06 ${F1} ${F2} ${F3}
    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} ${F1} -f ${TOC_FILE}_S1.${ARVIK_EXT} ${F2} -c ${F3} 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create nested archive with text files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "    Command was: ${SPROG} ${F1} -f ${TOC_FILE}_S1.${ARVIK_EXT} ${F2} -c ${F3}"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    ${DIFF} ${DIFF_OPTIONS} ${F4} ${TOC_FILE}_S1.${ARVIK_EXT} > ${TOC_FILE}_diff1.diff 2>&1
    if [ $? -eq 0 ]
    then
        ((POINTS+=${POINTS_PER_TEST}))
        echo -e "\tcreate nested text archive text is good"
        echo -e "\tPOINTS=${POINTS}"
    else
        echo ">>> create nested text archive needs help"
        CLEANUP=0
        echo "    Try this: ${DIFF} ${DIFF_OPTIONS} -y ${F4} ${TOC_FILE}_S1.${ARVIK_EXT}"
        echo "     Or this: ${VIMDIFF} ${F4} ${TOC_FILE}_S1.${ARVIK_EXT}"
        echo -e "    Also\n\tyour arvik: ${SPROG} ${F1} -f ${TOC_FILE}_S1.${ARVIK_EXT} ${F2} -c ${F3}"
        echo -e "    \tJesse arvik: ${JPROG} ${SPROG} ${F1} -f ${F4} ${F2} -c ${F3}"
    fi


    TOC_FILE=${ARVIK_EXT}_${RANDOM}_text5
    chmodAndTimeStampFiles
    F5=${TOC_FILE}_J1.${ARVIK_EXT}
    ${JPROG} -c [0-9]-s.txt > ${F5}
    
    chmodAndTimeStampFiles
    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} -c [0-9]-s.txt > ${TOC_FILE}_S1.${ARVIK_EXT} 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing create archive with text files stdout"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "    Command was: ${SPROG} -c [0-9]-s.txt > ${TOC_FILE}_S1.${ARVIK_EXT}"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    ${DIFF} ${DIFF_OPTIONS} ${F5} ${TOC_FILE}_S1.${ARVIK_EXT} > ${TOC_FILE}_diff1.diff 2>&1
    if [ $? -eq 0 ]
    then
        ((POINTS+=${POINTS_PER_TEST}))
        echo -e "\tcreate text archive text stdout is good"
        echo -e "\tPOINTS=${POINTS}"
    else
        echo ">>> create text archive stdout needs help"
        CLEANUP=0
        echo "    Try this: ${DIFF} ${DIFF_OPTIONS} -y ${F5} ${TOC_FILE}_S1.${ARVIK_EXT}"
        echo "     Or this: ${VIMDIFF} ${F5} ${TOC_FILE}_S1.${ARVIK_EXT}"
        echo -e "    Also\n\tyour arvik: ${SPROG} -c [0-9]-s.txt > ${TOC_FILE}_S1.${ARVIK_EXT}"
        echo -e "    \tJesse arvik: ${JPROG} -c [0-9]-s.txt > ${F5}"
    fi

    
    echo "** create text archives tests done..."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

testCreateBinFiles()
{
    echo "Testing create bin archives ..."

    BIN_FILES="random-333.bin"
    TOC_FILE=${ARVIK_EXT}_${RANDOM}_bin1
    chmodAndTimeStampFiles
    F1=${TOC_FILE}_J2.${ARVIK_EXT}
    ${JPROG} -cf ${F1} ${BIN_FILES}

    chmodAndTimeStampFiles
    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} -cf ${TOC_FILE}_S2.${ARVIK_EXT} ${BIN_FILES} 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "1 testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "    Command was: ${SPROG} -cf ${TOC_FILE}_S2.${ARVIK_EXT} ${BIN_FILES}"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    JSUM=$(${SUM} ${F1} | awk '{print $1;}')
    SSUM=$(${SUM} ${TOC_FILE}_S2.${ARVIK_EXT} | awk '{print $1;}')
    if [ ${JSUM} != ${SSUM} ]
    then
        echo ">>> FAILED single binary archive member, with ${BIN_FILES}"
        echo ">>> Fix this before trying more tests"
        echo "    Command was: ${SPROG} -cf ${TOC_FILE}_S2.${ARVIK_EXT} ${BIN_FILES}"
        echo -e "    Try this:\n\tcmp ${F1} ${TOC_FILE}_S2.${ARVIK_EXT}"
        return
    fi
    ((POINTS+=${POINTS_PER_TEST}))
    echo -e "\tPassed single binary archive member, with \n\t\t${BIN_FILES}\n\tPOINTS=${POINTS}"

    
    #################################################################################
    BIN_FILES="random-24M.bin random-2M.bin random-333.bin"
    TOC_FILE=${ARVIK_EXT}_${RANDOM}_bin2
    chmodAndTimeStampFiles
    F2=${TOC_FILE}_J2.${ARVIK_EXT}
    ${JPROG} -f ${F2} -c ${BIN_FILES}

    chmodAndTimeStampFiles
    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} -f ${TOC_FILE}_S2.${ARVIK_EXT} -c ${BIN_FILES} 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "2 testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "    Command was: ${SPROG} -f ${TOC_FILE}_S2.${ARVIK_EXT} -c ${BIN_FILES}"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    JSUM=$(${SUM} ${F2} | awk '{print $1;}')
    SSUM=$(${SUM} ${TOC_FILE}_S2.${ARVIK_EXT} | awk '{print $1;}')
    if [ ${JSUM} != ${SSUM} ]
    then
        echo ">>> FAILED single binary archive member, with ${BIN_FILES}"
        echo ">>> Fix this before trying more tests"
        echo "    Command was: ${SPROG} -f ${TOC_FILE}_S2.${ARVIK_EXT} -c ${BIN_FILES}"
        echo -e "    Try this:\n\tcmp ${F2} ${TOC_FILE}_S2.${ARVIK_EXT}"
        return
    fi
    ((POINTS+=${POINTS_PER_TEST}))
    echo -e "\tPassed binary archive members, with \n\t\t${BIN_FILES}\n\tPOINTS=${POINTS}"

    
    #################################################################################
    BIN_FILES="random-24M.bin random-2M.bin random-333.bin"
    BIN_FILES2="zeroes-1M.bin zeroes-4M.bin"
    TOC_FILE=${ARVIK_EXT}_${RANDOM}_bin3
    chmodAndTimeStampFiles
    F3=${TOC_FILE}_J2.${ARVIK_EXT}
    ${JPROG} -xctf ${F3} ${BIN_FILES} -c ${BIN_FILES2}

    chmodAndTimeStampFiles
    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} -xctf ${TOC_FILE}_S2.${ARVIK_EXT} ${BIN_FILES} -c ${BIN_FILES2} 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "3 testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "    Command was: ${SPROG} -xctf ${TOC_FILE}_S2.${ARVIK_EXT} ${BIN_FILES} -c ${BIN_FILES2}"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    JSUM=$(${SUM} ${TOC_FILE}_J2.${ARVIK_EXT} | awk '{print $1;}')
    SSUM=$(${SUM} ${TOC_FILE}_S2.${ARVIK_EXT} | awk '{print $1;}')
    if [ ${JSUM} != ${SSUM} ]
    then
        echo ">>> FAILED single binary archive member, with ${BIN_FILES} ${BIN_FILES2}"
        echo ">>> Fix this before trying more tests"
        echo "    Command was: ${SPROG} -xctf ${TOC_FILE}_S2.${ARVIK_EXT} ${BIN_FILES} -c ${BIN_FILES2}"
        echo -e "    Try this:\n\tcmp ${F3} ${TOC_FILE}_S2.${ARVIK_EXT}"
        return
    fi
    ((POINTS+=${POINTS_PER_TEST}))
    echo -e "\tPassed binary archive members, with ${BIN_FILES} ${BIN_FILES2}\n\tPOINTS=${POINTS}"


    #################################################################################
    #BIN_FILES="random-24M.bin random-2M.bin random-333.bin"
    #BIN_FILES2="zeroes-1M.bin zeroes-4M.bin"
    TOC_FILE=${ARVIK_EXT}_${RANDOM}_bin4
    chmodAndTimeStampFiles
    NESTED_BIN=${TOC_FILE}_J2.${ARVIK_EXT}
    F4=${TOC_FILE}_J2.${ARVIK_EXT}
    touch -t 198012050303.06 ${F1} ${F2} ${F3}
    ${JPROG} ${F1} -xxxxcttttttf ${F4} ${F2} -c ${F3} 

    chmodAndTimeStampFiles
    touch -t 198012050303.06 ${F1} ${F2} ${F3}
    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} ${F1} -xxxxcttttttf ${TOC_FILE}_S2.${ARVIK_EXT} ${F2} -c ${F3} 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "4 testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "    Command was: ${SPROG} ${F1} -xxxxcttttttf ${TOC_FILE}_S2.${ARVIK_EXT} ${F2} -c ${F3}"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    JSUM=$(${SUM} ${TOC_FILE}_J2.${ARVIK_EXT} | awk '{print $1;}')
    SSUM=$(${SUM} ${TOC_FILE}_S2.${ARVIK_EXT} | awk '{print $1;}')
    if [ ${JSUM} != ${SSUM} ]
    then
        echo ">>> FAILED multi-binary archive member, with "
        echo ">>> Fix this before trying more tests"
        echo "    Command was: ${SPROG} ${F1} -xxxxcttttttf ${TOC_FILE}_S2.${ARVIK_EXT} ${F2} -c ${F3}"
        echo -e "    Try this:\n\tcmp ${F4} ${TOC_FILE}_S2.${ARVIK_EXT}"
        return
    fi
    ((POINTS+=${POINTS_PER_TEST}))
    echo -e "\tPassed binary nested archive members, with \n\t\t${F1} ${F2} and ${F3} \n\tPOINTS=${POINTS}"


    echo -e "\tStarting work with biiiiiig files"
    #################################################################################
    BIN_FILES="odd_file.bin even_file.bin zero_big.bin odd_file.txt even_file.txt"
    TOC_FILE=${ARVIK_EXT}_${RANDOM}_bin5
    chmodAndTimeStampFiles
    export BIG_BIN=${TOC_FILE}_J2.${ARVIK_EXT}
    F5=${BIG_BIN}
    ${JPROG} ${BIN_FILES} -c > ${F5}
    touch -t 198012050303.06 ${F5}
    echo -e "\t\tstep 1 done"
    
    chmodAndTimeStampFiles
    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} ${BIN_FILES} -c > ${TOC_FILE}_S2.${ARVIK_EXT} 2> /dev/null " ; }
    touch -t 198012050303.06 ${TOC_FILE}_S2.${ARVIK_EXT}
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "5 testing create archive with binary files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "    Command was: ${SPROG} ${BIN_FILES} -c > ${TOC_FILE}_S2.${ARVIK_EXT}"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    echo -e "\t\tstep 2 done"
    
    JSUM=$(${SUM} ${TOC_FILE}_J2.${ARVIK_EXT} | awk '{print $1;}')
    SSUM=$(${SUM} ${TOC_FILE}_S2.${ARVIK_EXT} | awk '{print $1;}')
    if [ ${JSUM} != ${SSUM} ]
    then
        echo ">>> FAILED big binary archive member, with "
        echo ">>> Fix this before trying more tests"
        echo "    Command was: ${SPROG} ${BIN_FILES} -c > ${TOC_FILE}_S2.${ARVIK_EXT}"
        echo -e "    Try this:\n\tcmp ${F5} ${TOC_FILE}_S2.${ARVIK_EXT}"
        return
    fi
    ((POINTS+=${POINTS_PER_TEST}))
    ((POINTS+=${POINTS_PER_TEST}))
    echo -e "\tPassed big binary archive members, with \n\t\t${BIN_FILES} \n\tPOINTS=${POINTS}"

    
    echo "** create bin archive tests done..."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

testExtractFiles()
{
    echo "Testing extract from archive files..."

    #chmodAndTimeStampFiles
    BIN_FILES="1-s.txt 2-s.txt random-24M.bin random-2M.bin 3-s.txt 5-s.txt random-333.bin 4-s.txt zeroes-1M.bin 6-s.txt zeroes-4M.bin text-5k.txt text-75k.txt"
    TOC_FILE=${ARVIK_EXT}_${RANDOM}_mix1
    chmodAndTimeStampFiles
    F1=${TOC_FILE}_J3.${ARVIK_EXT}
    ${JPROG} -cf ${F1} ${BIN_FILES}
    #TOC_FILE=${ARVIK_EXT}_${RANDOM}_mix2
    #chmodAndTimeStampFiles
    #${JPROG} -cf ${TOC_FILE}_J3.${ARVIK_EXT} ${BIN_FILES}

    if [ -d jar_ex ]
    then
        rm -rf jar_ex
    fi
    mkdir jar_ex
    cd jar_ex

    #set -x

    rm -f *.{ar,arvik,arv,txt,bin}
    ln -f ../${F1} .
    ${JPROG} -xf ${F1}
    rm -f ${F1}
    sync ;  sync ; sleep 2 ; sync ; sync ; sleep 2
    ls -l | grep -v total | awk '{print $1, $2, $3, $4, $5, $9;}' > ../${TOC_FILE}_jar.ls
    ${SUM} * > ../${TOC_FILE}_jar.md5
    echo -e "\t\tstep 1 done"

    rm -f *.{ar,arvik,arv,txt,bin}
    # find this in the testCreateTextFiles function
    ln -f ../${NESTED_TEXT} .
    ${JPROG} -xf ${NESTED_TEXT}
    rm -f ${NESTED_TEXT}
    sync
    ls -l | grep -v total | awk '{print $1, $2, $3, $4, $5, $9;}' > ../${TOC_FILE}_jarT.ls
    ${SUM} * > ../${TOC_FILE}_jarT.md5
    echo -e "\t\tstep 2 done"

    rm -f *.{ar,arvik,arv,txt,bin}
    ln -f ../${NESTED_BIN} .
    ${JPROG} -xf ${NESTED_BIN}
    rm -f ${NESTED_BIN}
    sync ;  sync ; sleep 2 ; sync ; sync ; sleep 2
    ls -l | grep -v total | awk '{print $1, $2, $3, $4, $5, $9;}' > ../${TOC_FILE}_jarB.ls
    ${SUM} * > ../${TOC_FILE}_jarB.md5
    echo -e "\t\tstep 3 done"
    
    cd ..

    #################
    # Student file extraction
    #################
    
    #exit

    if [ -d sar_ex ]
    then
        rm -rf sar_ex
    fi
    mkdir sar_ex
    cd sar_ex
    rm -f *.{ar,arvik,arv,txt,bin}
    ln -f ../${F1} .
    #ln -f ../${NESTED_TEXT} .
    #ln -f ../${NESTED_BIN} .
    #${SPROG} -f ${TOC_FILE}_ar.${ARVIK_EXT} -x
    { timeout ${TOS} ${TO} bash -c "exec ../${SPROG} -ctVx < ${F1} > /dev/null 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "1 testing extract archive files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    rm -f ${F1}
    sync ;  sync ; sleep 2 ; sync ; sync ; sleep 2
    ls -l | grep -v total | awk '{print $1, $2, $3, $4, $5, $9;}' > ../${TOC_FILE}_sar.ls
    ${SUM} * > ../${TOC_FILE}_sar.md5
    echo -e "\t\tstep 4 done"
    
    rm -f *.{ar,arvik,arv,txt,bin}
    ln -f ../${NESTED_TEXT} .
    { timeout ${TOS} ${TO} bash -c "exec ../${SPROG} -V -x -f fake1 -cf ${NESTED_TEXT} -x > /dev/null 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "1 testing nested text extract archive files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    rm -f ${NESTED_TEXT}
    sync ;  sync ; sleep 2 ; sync ; sync ; sleep 2
    ls -l | grep -v total | awk '{print $1, $2, $3, $4, $5, $9;}' > ../${TOC_FILE}_sarT.ls
    ${SUM} * > ../${TOC_FILE}_sarT.md5
    #cat ${TOC_FILE}_sarT.ls
    echo -e "\t\tstep 5 done"

    rm -f *.{ar,arvik,arv,txt,bin}
    ln -f ../${NESTED_BIN} .
    { timeout ${TOS} ${TO} bash -c "exec ../${SPROG} -tvVxc -f ${NESTED_BIN} -x > /dev/null 2> /dev/null " ; }
    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "1 testing nested binary extract archive files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    rm -f ${NESTED_BIN}
    sync ;  sync ; sleep 2 ; sync ; sync ; sleep 2
    ls -l | grep -v total | awk '{print $1, $2, $3, $4, $5, $9;}' > ../${TOC_FILE}_sarB.ls
    ${SUM} * > ../${TOC_FILE}_sarB.md5
    #cat ${TOC_FILE}_sarB.ls
    echo -e "\t\tstep 6 done"

    cd ..

    ${DIFF} -q ${DIFF_OPTIONS} ${TOC_FILE}_sar.ls ${TOC_FILE}_jar.ls > /dev/null 2> /dev/null
    LS_EXIT=$?
    ${DIFF} -q ${DIFF_OPTIONS} ${TOC_FILE}_sar.md5 ${TOC_FILE}_jar.md5 > /dev/null 2> /dev/null
    MD5_EXIT=$?
    if [ ${LS_EXIT} -eq 0 -a ${MD5_EXIT} -eq 0 ]
    then
        ((POINTS+=${POINTS_PER_TEST}))
        ((POINTS+=${POINTS_PER_TEST}))
        echo -e "\tPassed extract archive members, with ${BIN_FILES}\n\tPOINTS=${POINTS}"
    else
        echo ">>> FAILED extract archive member"
        echo ">>> Fix this before trying more tests"
        echo "    Try this: ${DIFF} -y ${DIFF_OPTIONS} ${TOC_FILE}_sar.ls ${TOC_FILE}_jar.ls"
        echo "     Or this: ${VIMDIFF} ${VIMDIFF_OPTIONS} ${TOC_FILE}_sar.ls ${TOC_FILE}_jar.ls"
        echo "    Try this: ${DIFF} -y ${DIFF_OPTIONS} ${TOC_FILE}_sar.md5 ${TOC_FILE}_jar.md5"
        echo "     Or this: ${VIMDIFF} ${VIMDIFF_OPTIONS} ${TOC_FILE}_sar.md5 ${TOC_FILE}_jar.md5"
        exit
    fi

    ${DIFF} ${DIFF_OPTIONS} ${TOC_FILE}_sarT.ls ${TOC_FILE}_jarT.ls > /dev/null 2> /dev/null
    LS_EXIT=$?
    ${DIFF} -q ${DIFF_OPTIONS} ${TOC_FILE}_sarT.md5 ${TOC_FILE}_jarT.md5 > /dev/null 2> /dev/null
    MD5_EXIT=$?
    if [ ${LS_EXIT} -eq 0 -a ${MD5_EXIT} -eq 0 ]
    then
        ((POINTS+=${POINTS_PER_TEST}))
        ((POINTS+=${POINTS_PER_TEST}))
        echo -e "\tPassed extract nested text archive members, with ${NESTED_TEXT} \n\tPOINTS=${POINTS}"
    else
        echo ">>> FAILED extract archive member"
        echo ">>> Fix this before trying more tests"
        echo "    Try this: ${DIFF} -y ${DIFF_OPTIONS} ${TOC_FILE}_sarT.ls ${TOC_FILE}_jarT.ls"
        echo "     Or this: ${VIMDIFF} ${VIMDIFF_OPTIONS} ${TOC_FILE}_sarT.ls ${TOC_FILE}_jarT.ls"
        echo "    Try this: ${DIFF} -y ${DIFF_OPTIONS} ${TOC_FILE}_sarT.md5 ${TOC_FILE}_jarT.md5"
        echo "     Or this: ${VIMDIFF} ${VIMDIFF_OPTIONS} ${TOC_FILE}_sarT.md5 ${TOC_FILE}_jarT.md5"
    fi

    ${DIFF} ${DIFF_OPTIONS} ${TOC_FILE}_sarB.ls ${TOC_FILE}_jarB.ls > /dev/null 2> /dev/null
    LS_EXIT=$?
    ${DIFF} -q ${DIFF_OPTIONS} ${TOC_FILE}_sarB.md5 ${TOC_FILE}_jarB.md5 > /dev/null 2> /dev/null
    MD5_EXIT=$?
    if [ ${LS_EXIT} -eq 0 -a ${MD5_EXIT} -eq 0 ]
    then
        ((POINTS+=${POINTS_PER_TEST}))
        ((POINTS+=${POINTS_PER_TEST}))
        echo -e "\tPassed extract nested binary archive members, with ${NESTED_BIN}\n\tPOINTS=${POINTS}"
    else
        echo ">>> FAILED extract archive member"
        echo "    Try this: ${DIFF} -y ${DIFF_OPTIONS} ${TOC_FILE}_sarB.ls ${TOC_FILE}_jarB.ls"
        echo "     Or this: ${VIMDIFF} ${VIMDIFF_OPTIONS} ${TOC_FILE}_sarB.ls ${TOC_FILE}_jarB.ls"
        echo "    Try this: ${DIFF} -y ${DIFF_OPTIONS} ${TOC_FILE}_sarB.md5 ${TOC_FILE}_jarB.md5"
        echo "     Or this: ${VIMDIFF} ${VIMDIFF_OPTIONS} ${TOC_FILE}_sarB.md5 ${TOC_FILE}_jarB.md5"
        echo ">>> Fix this before trying more tests"

    fi

    
    echo "** extract from archive file passed."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}


testExtractBIGFiles()
{
    echo "Testing extract from BIG archive files..."

    ##chmodAndTimeStampFiles
    #BIN_FILES="1-s.txt 2-s.txt random-24M.bin random-2M.bin 3-s.txt 5-s.txt random-333.bin 4-s.txt zeroes-1M.bin 6-s.txt zeroes-4M.bin text-5k.txt text-75k.txt"
    #TOC_FILE=${ARVIK_EXT}_${RANDOM}_mix1
    #chmodAndTimeStampFiles
    #F1=${TOC_FILE}_J3.${ARVIK_EXT}
    #${JPROG} -cf ${F1} ${BIN_FILES}
    TOC_FILE=${ARVIK_EXT}_${RANDOM}_BIG
    ##chmodAndTimeStampFiles
    ##${JPROG} -cf ${TOC_FILE}_J3.${ARVIK_EXT} ${BIN_FILES}

    if [ -d jar_ex ]
    then
        rm -rf jar_ex
    fi
    mkdir jar_ex
    cd jar_ex
    
    echo -e "\t\tstep 1 done   BIG_BIN=${BIG_BIN}"

    #set -x

    rm -f *.{ar,arvik,arv,txt,bin}
    if [ -z "${BIG_BIN}" ]
    then
        echo "**************************************************************"
        echo "**************************************************************"
        echo "**************************************************************"
        echo "WARNING the BIG_BIN variable is not set **********************"
        echo "**************************************************************"
        echo "**************************************************************"
        echo "**************************************************************"
        return
    fi
    ln -f ../${BIG_BIN} .
    ${JPROG} -xf ${BIG_BIN}

    rm -f ${BIG_BIN}
    sync ;  sync ; sleep 2 ; sync ; sync ; sleep 2
    ls -l | grep -v total | awk '{print $1, $2, $3, $4, $5, $9;}' > ../${TOC_FILE}_jar.ls
    ${SUM} * > ../${TOC_FILE}_jar.md5
    echo -e "\t\tstep 2 done"
    
    cd ..

    #################
    # Student file extraction
    #################
    
    #exit

    if [ -d sar_ex ]
    then
        rm -rf sar_ex
    fi
    mkdir sar_ex
    cd sar_ex
    
    echo -e "\t\tstep 3 done"
    
    rm -f *.{ar,arvik,arv,txt,bin}
    ln -f ../${BIG_BIN} .
    { timeout ${TOS} ${TO} bash -c "exec ../${SPROG} -ctcx < ${BIG_BIN} > /dev/null 2> /dev/null " ; }

    CORE_DUMP=$?
    coreDumpMessage ${CORE_DUMP} "testing extract archive BIG files"
    if [ ${CORE_DUMP} -ne 0 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        exit 1
    fi
    rm -f ${BIG_BIN}
    sync ;  sync ; sleep 2 ; sync ; sync ; sleep 2
    ls -l | grep -v total | awk '{print $1, $2, $3, $4, $5, $9;}' > ../${TOC_FILE}_sar.ls
    ${SUM} * > ../${TOC_FILE}_sar.md5
    echo -e "\t\tstep 4 done"
    
    cd ..
    
    ${DIFF} -q ${DIFF_OPTIONS} ${TOC_FILE}_sar.ls ${TOC_FILE}_jar.ls > /dev/null 2> /dev/null
    LS_EXIT=$?
    ${DIFF} -q ${DIFF_OPTIONS} ${TOC_FILE}_sar.md5 ${TOC_FILE}_jar.md5 > /dev/null 2> /dev/null
    MD5_EXIT=$?
    echo -e "\t\tstep 5 done"

    
    #cat ${TOC_FILE}_jar.ls
    #cat ${TOC_FILE}_jar.md5

    
    #echo "exit values ls exit = ${LS_EXIT}    md5sum exit = ${MD5_EXIT}"
    
    if [ ${LS_EXIT} -eq 0 -a ${MD5_EXIT} -eq 0 ]
    then
        ((POINTS+=${POINTS_PER_TEST}))
        ((POINTS+=${POINTS_PER_TEST}))
        echo -e "\tPassed extract archive BIG files, with ${BIN_FILES}\n\tPOINTS=${POINTS}"
    else
        echo ">>> FAILED extract archive BIG files"
        echo ">>> Fix this before trying more tests"
        echo "    Try this: ${DIFF} -y ${DIFF_OPTIONS} ${TOC_FILE}_sar.ls ${TOC_FILE}_jar.ls"
        echo "     Or this: ${VIMDIFF} ${VIMDIFF_OPTIONS} ${TOC_FILE}_sar.ls ${TOC_FILE}_jar.ls"
        echo "    Try this: ${DIFF} -y ${DIFF_OPTIONS} ${TOC_FILE}_sar.md5 ${TOC_FILE}_jar.md5"
        echo "     Or this: ${VIMDIFF} ${VIMDIFF_OPTIONS} ${TOC_FILE}_sar.md5 ${TOC_FILE}_jar.md5"
        exit
    fi
    
    
    echo "** extract from archive BIG file passed."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

testBadFiles()
{
    echo "Testing bad file detect..."    
    
    chmodAndTimeStampFiles
    TOC_FILE=${ARVIK_EXT}_${RANDOM}_b1
    CORRUPT_FILE=${TOC_FILE}.${ARVIK_EXT}
    # corrupt the file tag
    ${JPROG} -Ct -cf ${CORRUPT_FILE} [1-9]-s.txt

    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} -Vv < ${CORRUPT_FILE} 1> ${TOC_FILE}.out 2> ${TOC_FILE}.err " ; }
    CORE_DUMP=$?
    #echo "exit value: ${CORE_DUMP}"
    coreDumpMessage ${CORE_DUMP} "testing bad tag detect..."
    if [ ${CORE_DUMP} -gt 100 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        CLEANUP=0
        exit 1
    fi
    if [ ${CORE_DUMP} -eq 0 ]
    then
        echo ">>> FAILED bad tag detect: found ${CORE_DUMP} expected 8"
        CLEANUP=0
    else
        ((POINTS+=${POINTS_PER_BAD}))
        echo -e "\tPassed bad tag detect\n\tPOINTS=${POINTS}"
    fi

    ##############################################################
    chmodAndTimeStampFiles
    TOC_FILE=${ARVIK_EXT}_${RANDOM}_b2
    CORRUPT_FILE=${TOC_FILE}.${ARVIK_EXT}
    # corrupt data
    ${JPROG} -Cd -cf ${CORRUPT_FILE} [1-9]-s.txt

    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} -Vv < ${CORRUPT_FILE} 1> ${TOC_FILE}.out  2> ${TOC_FILE}.err " ; }
    CORE_DUMP=$?
    #echo "exit value: ${CORE_DUMP}"
    coreDumpMessage ${CORE_DUMP} "testing bad data detect..."
    if [ ${CORE_DUMP} -gt 100 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        CLEANUP=0
        exit 1
    fi
    if [ ${CORE_DUMP} -eq 0 ]
    then
        echo ">>> FAILED bad data detect: found ${CORE_DUMP}"
        CLEANUP=0
    else
        ((POINTS+=${POINTS_PER_BAD}))
        echo -e "\tPassed bad data detect\n\tPOINTS=${POINTS}"
    fi

    ##############################################################
    chmodAndTimeStampFiles
    TOC_FILE=${ARVIK_EXT}_${RANDOM}_b3
    CORRUPT_FILE=${TOC_FILE}.${ARVIK_EXT}
    # bad size
    ${JPROG} -Cs -cf ${CORRUPT_FILE} 1-s.txt

    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} -Vv < ${CORRUPT_FILE} 1> ${TOC_FILE}.out  2> ${TOC_FILE}.err " ; }
    CORE_DUMP=$?
    #echo "exit value: ${CORE_DUMP}"
    coreDumpMessage ${CORE_DUMP} "testing bad file size detect..."
    if [ ${CORE_DUMP} -gt 100 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        CLEANUP=0
        exit 1
    fi
    if [ ${CORE_DUMP} -eq 0 ]
    then
        echo ">>> FAILED bad file size detect: found ${CORE_DUMP}"
        CLEANUP=0
    else
        ((POINTS+=${POINTS_PER_BAD}))
        echo -e "\tPassed bad file size detect\n\tPOINTS=${POINTS}"
    fi


    ##############################################################
    chmodAndTimeStampFiles
    TOC_FILE=${ARVIK_EXT}_${RANDOM}_b4
    CORRUPT_FILE=${TOC_FILE}.${ARVIK_EXT}
    # bad md4 for header
    ${JPROG} -CH -cf ${CORRUPT_FILE} [1-9]-s.txt

    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} -Vv < ${CORRUPT_FILE} 1> ${TOC_FILE}.out  2> ${TOC_FILE}.err " ; }
    CORE_DUMP=$?
    #echo "exit value: ${CORE_DUMP}"
    coreDumpMessage ${CORE_DUMP} "testing bad md4 value for header in the footer detect..."
    if [ ${CORE_DUMP} -gt 100 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        CLEANUP=0
        exit 1
    fi
    if [ ${CORE_DUMP} -eq 0 ]
    then
        echo ">>> FAILED bad md4 value for header in the footer detect: found ${CORE_DUMP} expected 10"
        CLEANUP=0
    else
        ((POINTS+=${POINTS_PER_BAD}))
        echo -e "\tPassed bad md4 value for header in the footer detect\n\tPOINTS=${POINTS}"
    fi


    ##############################################################
    chmodAndTimeStampFiles
    TOC_FILE=${ARVIK_EXT}_${RANDOM}_b5
    CORRUPT_FILE=${TOC_FILE}.${ARVIK_EXT}
    # bad md4 for data
    ${JPROG} -CD -cf ${CORRUPT_FILE} [1-9]-s.txt

    { timeout ${TOS} ${TO} bash -c "exec ${SPROG} -Vv < ${CORRUPT_FILE} 1> ${TOC_FILE}.out  2> ${TOC_FILE}.err " ; }
    CORE_DUMP=$?
    #echo "exit value: ${CORE_DUMP}"
    coreDumpMessage ${CORE_DUMP} "testing bad md4 value for data in the footer detect..."
    if [ ${CORE_DUMP} -gt 100 ]
    then
        echo ">>> Segmentation faults are not okay <<<"
        echo ">>> Testing ends here"
        echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
        CLEANUP=0
        exit 1
    fi
    if [ ${CORE_DUMP} -eq 0 ]
    then
        echo ">>> FAILED bad md4 value for data in the footer detect: found ${CORE_DUMP} expected 10"
        CLEANUP=0
    else
        ((POINTS+=${POINTS_PER_BAD}))
        echo -e "\tPassed bad md4 value for data in the footer detect\n\tPOINTS=${POINTS}"
    fi

    
    echo "** bad file detect done."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

testValgrind()
{
    local LEAK_COUNT=0
    
    echo "Testing with valgrind for memory leaks..."

    TOC_FILE=${ARVIK_EXT}_${RANDOM}_val1
    ${JPROG} -cf ${TOC_FILE}.${ARVIK_EXT} [0-9]-s.txt zeroes-?M.bin

    
    ${VALGRIND} ${SPROG} -f fake1 -t -ffake2 -f ${TOC_FILE}.${ARVIK_EXT} > ${TOC_FILE}_v1.out 2> ${TOC_FILE}_v1.err
    LEAKS=$(grep "${NOLEAKS}" ${TOC_FILE}_v1.err | wc -l)
    #echo "No leak count ${LEAKS}"
    if [ ${LEAKS} -eq 1 ]
    then
        echo -e "\tNo leaks found in short TOC. Excellent."
    else
        echo ">>> Leaks found in short TOC."
        LEAKS_FOUND=1
        ((LEAK_COUNT++))
        CLEANUP=0
    fi

    TOC_FILE=${ARVIK_EXT}_${RANDOM}_val2
    ${VALGRIND} ${SPROG} -f fake1 -ffake2 -tf ${TOC_FILE}.${ARVIK_EXT} -v > ${TOC_FILE}_v1.out 2> ${TOC_FILE}_v1.err
    LEAKS=$(grep "${NOLEAKS}" ${TOC_FILE}_v1.err | wc -l)
    #echo "No leak count ${LEAKS}"
    if [ ${LEAKS} -eq 1 ]
    then
        echo -e "\tNo leaks found in long TOC. Excellent."
    else
        echo ">>> Leaks found in long TOC."
        LEAKS_FOUND=1
        ((LEAK_COUNT++))
        CLEANUP=0
    fi

    TOC_FILE=${ARVIK_EXT}_${RANDOM}_val3
    ${VALGRIND} ${SPROG} -f fake1 -ffake2 -cf ${TOC_FILE}.${ARVIK_EXT} ?-s.txt > ${TOC_FILE}_v2.out 2> ${TOC_FILE}_v2.err
    LEAKS=$(grep "${NOLEAKS}" ${TOC_FILE}_v2.err | wc -l)
    #echo "No leak count ${LEAKS}"
    if [ ${LEAKS} -eq 1 ]
    then
        echo -e "\tNo leaks found in create. Excellent."
    else
        echo ">>> Leaks found in create."
        LEAKS_FOUND=1
        ((LEAK_COUNT++))
        CLEANUP=0
    fi

    TOC_FILE=${ARVIK_EXT}_${RANDOM}_val4
    ${JPROG} -cf ${TOC_FILE}.${ARVIK_EXT} [0-9]-s.txt zeroes-?M.bin > /dev/null 2> /dev/null
    if [ -d arvik_val ]
    then
        rm -rf arvik_val
    fi    
    mkdir arvik_val
    cd arvik_val
    rm -f [0-9]-s.txt zeroes-?M.bin
    ln -f ../${TOC_FILE}.${ARVIK_EXT} . 
    ${VALGRIND} ../${SPROG} -x < ${TOC_FILE}.${ARVIK_EXT} > ${TOC_FILE}_v3.out 2> ${TOC_FILE}_v3.err
    LEAKS=$(grep "${NOLEAKS}" ${TOC_FILE}_v3.err | wc -l)
    #echo "No leak count ${LEAKS}"
    if [ ${LEAKS} -eq 1 ]
    then
        echo -e "\tNo leaks found in extract. Excellent."
    else
        echo ">>> Leaks found in extract."
        LEAKS_FOUND=1
        ((LEAK_COUNT++))
        CLEANUP=0
        echo "Command: ${VALGRIND} ${SPROG} -x < ${TOC_FILE}.${ARVIK_EXT}"
    fi
    cd ..

    TOC_FILE=${ARVIK_EXT}_${RANDOM}_val3
    ${VALGRIND} ${SPROG} -f fake1 -ffake2 -Vf ${TOC_FILE}.${ARVIK_EXT} > ${TOC_FILE}_v2.out 2> ${TOC_FILE}_v2.err
    LEAKS=$(grep "${NOLEAKS}" ${TOC_FILE}_v2.err | wc -l)
    #echo "No leak count ${LEAKS}"
    if [ ${LEAKS} -eq 1 ]
    then
        echo -e "\tNo leaks found in validate. Excellent."
    else
        echo ">>> Leaks found in validate."
        LEAKS_FOUND=1
        ((LEAK_COUNT++))
        CLEANUP=0
        ${COWSAY} -f ghostbusters "OUCH!!! That hurts! Where is my leak detector?"
    fi

    if [ ${LEAK_COUNT} -eq 0 ]
    then
        ${COWSAY} -f moofasa The memory lords praise you
    else
        ${COWSAY} -f eyes Keep your eyes on memory leaks
    fi
    
    echo "** Done with Testing valgrind."
    echo "*** Points so far ${POINTS} of ${TOTAL_POINTS}"
}

#testNested()
#{
#    echo "Testing nested archive files..."
#
#    chmodAndTimeStampFiles
#
#    TEXT1_FILES=$(shuf -e [1234]-s.txt | paste -s)
#    TEXT2_FILES=$(shuf -e [56]-s.txt text-5k.txt text-75k.txt | paste -s)
#    BIN_FILES=$(shuf random-*.bin zeroes-4M.bin | paste -s)
#}

cleanTestFiles()
{
    #if [ ${CLEANUP} -eq 1 ]
    if [ $1 -eq 1 ]
    then
        echo "*** Cleaning up old test files"

        # XXX leave these files in the directory XXX
        # i recreate/chmod these below. they could have been modified
        rm -f Constitution.txt Iliad.txt jargon.txt text-*k.txt words.txt
        rm -f [0-9]-s.txt NOTES.txt WARN.out
        rm -f random-*.bin
        rm -f zeroes-?M.bin 

        rm -f {${PROG},${ARVIK_EXT}}_*_[JS].${PROG}
        rm -f ${PROG}_*_[JS].ar
        rm -f ${PROG}_*_[JS].{out,err}

        rm -f valgrindTest*.err WARN.err validate*.err

        rm -f ${ARVIK_EXT}_*_toc[12].{ar,stoc,diff} ${ARVIK_EXT}_*_toc[12]_*.{ar,stoc,diff}
        rm -f ${ARVIK_EXT}_*_text[1-9]_*.{ar,stoc,ltoc,diff,arvik} ${ARVIK_EXT}_*.ltoc

        rm -f ar_*_bin[0-9]_ar*.ar ar_*_bin[0-9]_sprog*.arvik

        rm -rf ar_exD arvik_exD ar_exU arvik_exU arvik_val
        rm -f *.${ARVIK_EXT} *.${PROG} *.out *.err

        ln -fs ${JDIR}/${DIR}.h .

        rm -f *.ls *.md5 ${BIG_BIN}
        rm -rf jar_ex sar_ex

        #make clean 1> /dev/null 2> /dev/null
    else
        echo "*** Skipping cleanup"
    fi

    # i should recreate/chmod these files. they could have been deleted
    # or modified during some of the tests
    #copyTestFiles
}


while getopts "xChl" opt
do
    case $opt in
        x)
            # If you really, really, REALLY want to watch what is going on.
            echo "Hang on for a wild ride."
            set -x
            ;;
        C)
            # Skip removal of data files
            CLEANUP=0
            ;;
        h)
            echo "$0 [-h] [-C] [-x]"
            echo "  -h  Display this amazing help message"
            echo "  -C  Do not remove all the test files"
            echo "  -x  Show LOTS and LOTS and LOTS of text about what is happening"
            echo "  -l  When used with -x, line numbers are prefixed to diagnostic output"
            exit 0
            ;;
        l)
            PS4='Line ${LINENO}: '
            ;;
        \?)
            echo "Invalid option" >&2
            echo ""
            exit 1
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            exit 1
            ;;
    esac
done

HOST=$(hostname -s)
if [ ${HOST} != ${FILE_HOST} ]
then
    echo "This script MUST be run on ${FILE_HOST}"
    exit 1
fi

BDATE=$(date)

build
if [ ${BuildFailed} -ne 0 ]
then
    echo "Since the program build failed (using make), ending test"
    echo "Points = 0"
    exit 1
else
    echo "Build success!"
fi

trap 'signalCaught;' SIGTERM SIGQUIT SIGKILL SIGSEGV
trap 'signalCtrlC;' SIGINT
#trap 'signalSegFault;' SIGCHLD

cleanTestFiles 1
#rm -f test_[0-9][1-9]_[JS].{${PROG},ar}


copyTestFiles

testHelp

testTOC

testCreateTextFiles

testCreateBinFiles

testExtractFiles

testBadFiles

testExtractBIGFiles

testValgrind

cleanTestFiles ${CLEANUP}

# get things back to a consistant initial state
copyTestFiles



EDATE=$(date)

echo -e "\n\n*********************************************************"
echo "*********************************************************"
echo "Done with Testing."
echo "Points so far ${POINTS} of ${TOTAL_POINTS}"
echo "This does not include the points from the Makefile-test.bash script"


# this is for testing the testing script
# it tests reduction in points for memory leaks or compiler warnings
#WARNINGS=1
#LEAKS_FOUND=1


if [ ${LEAKS_FOUND} -ne 0 ]
then
    echo -e "\n\n*********************************************************************"
    echo -e "**** But.... Memory leaks were found. That is a 20% deduction. ******"
    POINTS=$(echo ${POINTS} | awk '{printf "%.1f\n", $1 * 0.8;}')
    echo "Points with leak deductions ${POINTS} of ${TOTAL_POINTS}"
    echo -e "OUCH!!! That hurts! Where is my leak detector?"
    echo -e "*********************************************************************"
fi

if [ ${WARNINGS} -ne 0 ]
then
    echo -e "\n\n************************************************************************"
    echo -e "**** But.... Compiler warnings were found. That is a 20% deduction. ****"
    POINTS=$(echo ${POINTS} | awk '{printf "%.1f\n", $1 * 0.8;}')
    echo "Points with compiler warning deductions ${POINTS} of ${TOTAL_POINTS}"
    echo -e "OUCH!!! That hurts! Where is that compiler warnings fixer-upper?"
    echo -e "************************************************************************"
fi
echo -e "\n\nThis does not take into account any late penalty that may apply."

echo -e "\n"
echo "Test begun at     ${BDATE}"
echo "Test completed at ${EDATE}"
echo -e "\n"
echo "+++ TOTAL_POINTS    = ${POINTS} of ${TOTAL_POINTS} ***"
