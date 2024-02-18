g++ -fPIC -c -o sniff.o sniff.cc -ldl
gcc -fPIC -shared -o sniff.so sniff.o -ldl
