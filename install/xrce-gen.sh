cd ~/workspace
git clone https://github.com/eProsima/Micro-XRCE-DDS-Gen.git
cd Micro-XRCE-DDS-Gen
git submodule init
git submodule update
gradle build -Dbranch=v1.2.5