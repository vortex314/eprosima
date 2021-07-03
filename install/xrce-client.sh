
cd ~/workspace
git clone https://github.com/eProsima/Micro-XRCE-DDS-Client.git
cd Micro-XRCE-DDS-Client
mkdir build 
cd build
cmake ..
make
sudo make install
sudo ldconfig /usr/local/lib/
