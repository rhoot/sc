#!/bin/bash -ex

# Install newer version of cmake
sudo add-apt-repository ppa:george-edison55/cmake-3.x -y
sudo apt-get update -q
sudo apt-get install cmake -y
