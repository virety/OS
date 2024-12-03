sudo apt update && sudo apt upgrade -y
sudo apt install ubuntu-desktop
reboot
sudo apt install spice-vdagent spice-webdavd
reboot
sudo apt install cmake
cmake --version
mkdir test-hello && cd test-hello
sudo nano CMakeLists.txt
