#! /bin/zsh/

cd src
make
./htbench www.example.com 80 5 10 10
make clean
