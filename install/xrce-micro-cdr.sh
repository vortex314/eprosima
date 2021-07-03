
cd ~/workspace
git clone https://github.com/eProsima/Micro-CDR.git
cd Micro-CDR
mkdir build 
cd build
cmake ..
make
sudo make install
sudo ldconfig /usr/local/lib/
