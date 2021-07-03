cd ~/workspace
rm -rf Micro-XRCE-DDS-Agent Micro-XRCE-DDS
git clone https://github.com/eProsima/Micro-XRCE-DDS-Agent.git
cd Micro-XRCE-DDS-Agent
mkdir build && cd build
cmake ..
make
sudo make install

