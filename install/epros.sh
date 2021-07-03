sudo apt-get install libasio-dev libtinyxml2-dev
cd ~/workspace
rm -rf Fast-CDR foonathan_memory_vendor Fast-DDS
git clone https://github.com/eProsima/Fast-CDR.git
mkdir Fast-CDR/build && cd Fast-CDR/build
cmake ..
sudo cmake --build . --target install
cd ~/workspace
git clone https://github.com/eProsima/foonathan_memory_vendor.git
cd foonathan_memory_vendor
mkdir build && cd build
cmake ..
sudo cmake --build . --target install
cd ~/workspace
git clone https://github.com/eProsima/Fast-DDS.git
mkdir Fast-DDS/build && cd Fast-DDS/build
cmake ..
sudo cmake --build . --target install
