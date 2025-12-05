#!/bin/bash
# R. Jesse Chaney
# rchaney@pdx.edu


DIR=.
#DIR=${PWD}
#echo "DIR=${DIR}"


rm -f FUNNYnoaccess
touch FUNNYnoaccess
chmod a-rwx FUNNYnoaccess
touch -t 198012050303.06 FUNNYnoaccess


rm -rf FUNNYdir
if [ ! -d FUNNYdir ]
then
    mkdir FUNNYdir
fi
chmod a+r FUNNYdir
touch -t 199912122358.59 FUNNYdir


rm -f FUNNYregfile FUNNYhardlink FUNNYsymlink
#touch FUNNYregfile
who > FUNNYregfile
ln FUNNYregfile FUNNYhardlink
ln -sf ${DIR}/FUNNYregfile FUNNYsymlink
chmod a+rx FUNNYregfile
touch -t 196608040311.36 FUNNYregfile


rm -f FUNNYpipe
mkfifo FUNNYpipe
chmod a+r FUNNYpipe
touch -t 197805082150.59 FUNNYpipe


FILE_NAME=FUNNYsocket
rm -f ${FILE_NAME}
PY_CODE="import socket as s; sock = s.socket(s.AF_UNIX); sock.bind('"${FILE_NAME}"')"
python3 -c "${PY_CODE}"
chmod og+rw ${FILE_NAME}
touch -t 195301011532.57 ${FILE_NAME}


rm -f DOES_NOT_EXIST FUNNYbroken
touch DOES_NOT_EXIST
ln -sf ${DIR}/DOES_NOT_EXIST FUNNYbroken
rm -f DOES_NOT_EXIST
