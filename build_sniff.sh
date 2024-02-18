g++ -fPIC -c -o sniff.o sniff.cc -ldl -I. -I./test
gcc -fPIC -shared -o sniff.so sniff.o -ldl -I. -I./test
