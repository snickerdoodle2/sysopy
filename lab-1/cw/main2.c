#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>

int main() {
	void *test = dlopen("./libbibl1.so", RTLD_LAZY);
	void (*f2)();
	f2 = dlsym(test, "test");
	if(dlerror()!=NULL){printf("blad fun2\n"); return 1;}
	f2();
	dlclose(test);
	return 0;
}