#!/bin/sh

# @ilsalvopss
# NOTE: fluka download permitted to authorized users only

fatal(){
  echo "FATAL ERROR: $1"
  echo "exiting now.."

  exit 1
}

if [ -z "$1" ]
  then
    fatal "no FLUKA deb provided, specify it as first argument. (download it from https://fluka.cern/download/latest-fluka-release)"
fi

# install FLUKA deb
sudo apt install ./$1

if [ $? -ne 0 ]
  then
    fatal "apt failed to install FLUKA"
fi

# install flair repository pubkey
wget -q -O - https://flair.web.cern.ch/flair/download/ubuntu/flair.gpg | sudo tee /etc/apt/trusted.gpg.d/flair.asc

sudo add-apt-repository 'deb [arch=all,amd64] https://cern.ch/flair/download/ubuntu/22.10 /'

sudo apt install flair

if [ $? -ne 0 ]
  then
    fatal "flair installation failed"
fi

#TODO: apply flair patch

patch -d /usr/local/flair flair-3.2.patch

if [ $? -ne 0 ]
  then
    fatal "!!failed to patch flair!!"
fi