#!/bin/sh
echo "Deploying..."
CWD=$PWD

COMMONS="so-commons-library"
git clone "https://github.com/sisoputnfrba/${COMMONS}.git" $COMMONS
cd $COMMONS
sudo make uninstall
make all
sudo make install

cd $CWD
sudo rm -rf ${COMMONS}
sudo apt-get install libreadline6 libreadline6-dev
make all