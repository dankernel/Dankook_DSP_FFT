#gcc -finput-charset=UTF-8 -o main main.c
ctags -R
g++ -m32 -L/usr/lib32/ -lstdc++ -o main main.cpp
./main $1
