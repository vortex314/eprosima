cd ~/workspace
git clone --recursive https://github.com/eProsima/Fast-DDS-Gen.git
cd Fast-DDS-Gen
export JAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk-amd64
./gradlew assemble
