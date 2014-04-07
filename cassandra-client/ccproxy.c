#include <stdio.h>
#include <stdlib.h>
#include <jni.h>

JNIEnv* create_vm(JavaVM ** jvm) {
    JNIEnv *env;
    JavaVMInitArgs vm_args;

    JavaVMOption options; 
    //Path to the java source code     
    options.optionString = "-Djava.class.path=D:\\Java Src\\TestStruct"; 
    vm_args.version = JNI_VERSION_1_6; //JDK version. This indicates version 1.6
    vm_args.nOptions = 1;
    vm_args.options = &options;
    vm_args.ignoreUnrecognized = 0;
    
    JNI_CreateJavaVM(jvm, (void**)&env, &vm_args);
        
    return env;
}

int main(void) {
	return 0;
}
