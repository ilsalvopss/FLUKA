#!/bin/sh

# @ilsalvopss

if [ -z "$1" ]
  then
    echo "usage: $0 input.flair"

    exit 1
fi

export PATH=$PATH:/usr/local/fluka/bin
flair --run "<default>" $1