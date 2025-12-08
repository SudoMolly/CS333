#!/bin/bash
echo "making all"
make all
echo "coping execs"
cp ./client/rockem_client ./testing/client/rockem_client
cp ./server/rockem_server ./testing/server/rockem_server
echo "sleeping then clearing"
clear