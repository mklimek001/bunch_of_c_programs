static:
	gcc -c bibl1.c
	ar rcs libbibl1.a bibl1.o
	gcc -c main.c
	gcc main.o libbibl1.a -o main_start

shared:
	gcc -fPIC -c bibl1.c
	gcc -shared bibl1.o -o libbibl1.so
	gcc -c main.c
	gcc main.o -L. -lbibl1 -Wl,-rpath=. -o main_wspoldz

dynamic:
	gcc -fPIC -c bibl1.c
	gcc -shared bibl1.o -o libbibl1.so
	gcc main2.c -o main_dynamic -ldl

tests_static:
	gcc -c bibl1.c
	ar rcs libbibl1.a bibl1.o
	gcc -c tests.c
	gcc tests.o libbibl1.a -o tests_static

tests_shared:
	gcc -fPIC -c bibl1.c
	gcc -shared bibl1.o -o libbibl1.so
	gcc -c tests.c
	gcc tests.o -L. -lbibl1 -Wl,-rpath=. -o tests_wspoldz

tests_dynamic:
	gcc -fPIC -c bibl1.c
	gcc -shared bibl1.o -o libbibl1.so
	gcc tests2.c -o tests2_dynamic -ldl

opt_tests_o:
	gcc -c bibl1.c
	ar rcs libbibl1.a bibl1.o
	gcc -c opt_tests.c
	gcc opt_tests.o libbibl1.a -o op_tests_o

opt_tests_o1:
	gcc -c bibl1.c
	ar rcs libbibl1.a bibl1.o
	gcc -c opt_tests.c
	gcc opt_tests.o libbibl1.a -o1 op_tests_o

opt_tests_os:
	gcc -c bibl1.c
	ar rcs libbibl1.a bibl1.o
	gcc -c opt_tests.c
	gcc opt_tests.o libbibl1.a -os op_tests_o


