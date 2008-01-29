#include "pydim_utils.cpp"


using namespace std;

    
char good_format01[]="i:3;F:2;C";
char good_format02[]="I:3;F:2;C:4;";
char good_format03[]="D";
char good_format04[]="I:1;D:1;F:1;X:1;S:1";
char good_format05[]="C:13;X:2;C:1";
char good_format06[]="i:3;s:1;x:1;S:2;C:5";

char bad_format01[] ="D:";
char bad_format02[] ="D:4C";
char bad_format03[] ="D:;I";
char bad_format04[] ="i:1;V:";
char bad_format05[] ="i:123;i:289;:21";

    
struct test01{
    int i1;
    int i2;
    int i3;
    float f1;
    float f2;
    char c1;        
} __attribute__((__packed__));

struct test011{
    int i1;
    int i2;
    int i3;
    float f1;
    float f2;
    char c1[20];        
} __attribute__((__packed__));
    
struct test02{
    int i1;
    double d1;        
    float f1;
    long long ll1;
    short s1;        
} __attribute__((__packed__));

int main () {
    
    // Testing verify_dim_format()

    
    

    print ("testing verify_dim_format()...");
    if (verify_dim_format(good_format01)) {
       print("SUCCESS: Validity test for %s (good format) PASSED", 
              good_format01);
    } else {
       print("ERROR  : Validity test for %s (good format) FAILED", 
              good_format01);
    }
        
    if (verify_dim_format(good_format02)){
       print("SUCCESS: Validity test for %s (good format) PASSED", 
              good_format02);
    } else {
       print("ERROR  : Validity test for %s (good format) FAILED", 
              good_format02);
    }

    if (verify_dim_format(good_format03)){
       print("SUCCESS: Validity test for %s (good format) PASSED", 
              good_format03);
    } else {
       print("ERROR  : Validity test for %s (good format) FAILED", 
              good_format03);
    }

    if (!verify_dim_format(bad_format01)) {        
       print("SUCCESS: Validity test for %s (bad format) PASSED", 
              bad_format01);
    } else {
       print("ERROR  : Validity test for %s (bad format) FAILED", 
              bad_format01);
    }

    if (!verify_dim_format(bad_format02)) {
       print("SUCCESS: Validity test for %s (bad format) PASSED", 
              bad_format02);
    } else {
       print("ERROR  : Validity test for %s (bad format) FAILED", 
              bad_format02);
    }

    if (!verify_dim_format(bad_format03)) {
       print("SUCCESS: Validity test for %s (bad format) PASSED", 
              bad_format03);
    } else {
       print("ERROR  : Validity test for %s (bad format) FAILED", 
              bad_format03);
    }

    if (!verify_dim_format(bad_format04)) {
       print("SUCCESS: Validity test for %s (bad format) PASSED", 
              bad_format04);
    } else {
       print("ERROR  : Validity test for %s (bad format) FAILED", 
              bad_format04);
    }

    if (!verify_dim_format(bad_format05)) {
       print("SUCCESS: Validity test for %s (bad format) PASSED\n\n", 
              bad_format05);
    } else {
       print("ERROR  : Validity test for %s (bad format) FAILED\n\n", 
              bad_format05);
    }
    
    
    // testing next_element()
    
    int type, multiplicity, internal_ptr=0, failed=0;
     
    print ("testing next_element()...");
    // TEST 1         
    if (!next_element(good_format01, &internal_ptr, &type, &multiplicity))
        failed=1;
    if (internal_ptr!=4 || type!=_DIM_INT || multiplicity!=3)
        failed=2;
    if (!next_element(good_format01, &internal_ptr, &type, &multiplicity))
        failed=3;
    if (internal_ptr!=8 || type!=_DIM_FLOAT || multiplicity!=2)
        failed=4;
    if (!next_element(good_format01, &internal_ptr, &type, &multiplicity))
        failed=5;        
    if (internal_ptr!=9 || type!=_DIM_STRING || multiplicity!=-1)
        failed=6;
    if (next_element(good_format01, &internal_ptr, &type, &multiplicity))
        failed=7;
    if (failed) { 
        print("ERROR  : next_element subtest %d FAILED for %s", 
              failed, good_format01);
    } else {
        print("SUCCESS: next_element tests PASSED for %s", good_format01);
    }         
//    printf("Internal ptr: %d,  Type: %d,  Multiplicity: %d\n",
//            internal_ptr, type, multiplicity);

    // TEST 2
    internal_ptr=0;
    failed=0;
    
    if (!next_element(good_format02, &internal_ptr, &type, &multiplicity))
        failed=1;
    if (internal_ptr!=4 || type!=_DIM_INT || multiplicity!=3)
        failed=2;
    if (!next_element(good_format02, &internal_ptr, &type, &multiplicity))
        failed=3;
    if (internal_ptr!=8 || type!=_DIM_FLOAT || multiplicity!=2)
        failed=4;
    if (!next_element(good_format02, &internal_ptr, &type, &multiplicity))
        failed=5;        
    if (internal_ptr!=12 || type!=_DIM_STRING || multiplicity!=4)
        failed=6;
//    printf("Internal ptr: %d,  Type: %d,  Multiplicity: %d\n",
//            internal_ptr, type, multiplicity);
    
    if (next_element(good_format01, &internal_ptr, &type, &multiplicity))
        failed=7;
    if (failed) { 
        print("ERROR  : next_element subtest %d FAILED for %s", 
               failed, good_format02);
    } else {
        print("SUCCESS: next_element tests PASSED for %s", good_format02);
    } 
    
    // TEST 3
    internal_ptr=0;
    failed=0;
    
    if (!next_element(good_format03, &internal_ptr, &type, &multiplicity))
        failed=1;
    if (internal_ptr!=1 || type!=_DIM_DOUBLE || multiplicity!=-1)
        failed=2;
//    printf("Internal ptr: %d,  Type: %d,  Multiplicity: %d\n",
//            internal_ptr, type, multiplicity);
    
    if (next_element(good_format03, &internal_ptr, &type, &multiplicity))
        failed=3;
    if (failed) {
        print("ERROR  : next_element subtest %d FAILED for %s", 
               failed, good_format03);
    } else {
        print("SUCCESS: next_elemnt tests PASSED for %s", good_format03);
    }
    
    
    // testing next_element()
        
    PyObject *list1, *list2, *list3;


    test01 teststrct1, teststrct2;
    teststrct1.i1=10240;
    teststrct1.i2=102400;
    teststrct1.i3=1024000;
    teststrct1.f1=10240.1023;
    teststrct1.f2=11111111.11111;
    teststrct1.c1='X';
    

   
    test02 teststrct3, teststrct4;
    teststrct3.i1=255255;
    teststrct3.d1=1.1234567;
    teststrct3.f1=123456.654321;
    teststrct3.ll1=1234567123;
    teststrct3.s1=1023;
    printf("\n\n");
    print ("testing dim_buf_to_list()...");       
    list1 = dim_buf_to_list(good_format01, (const char*)&teststrct1, 
                            sizeof(teststrct1));
    if (list1) {
        //printPyObject(list1);
        if (!iterator_to_buffer(list1,(char*)&teststrct2, 
                    sizeof(test01), good_format01)) {
            print("FAIL  : Could not convert tuple object to C buffer");
        } else {
            if (!memcmp(&teststrct1, &teststrct2, sizeof(test01))) {
                print("SUCCESS: The two structures are the same for arguments converted with format %s", good_format01);
            } else {
                print("FAIL  :The two structures differ");        
            }
        }
    } else {
        print("FAIL  : Could not convert C buffer to Python list");
    }
    
    list2 = dim_buf_to_list(good_format04, (const char*)&teststrct3, 
                            sizeof(teststrct3));
    if (list2) {
        //printPyObject(list2);
        if (!iterator_to_buffer(list2,(char*)&teststrct4, 
                    sizeof(test02), good_format04)) {
            print("FAIL  : Could not convert tuple object to C buffer");
        } else {
            if (!memcmp(&teststrct3, &teststrct4, sizeof(test02))) {
                print("SUCCESS: The two structures are the same for arguments converted with format %s", good_format04);
            } else {
                print("FAIL  : The two structures differ");        
            }
        }
    } else {
        print("FAIL  : Could not convert C buffer to Python list");
    }
    
    double d1=100.101, d2;
    list3 = dim_buf_to_list("d", (const char*)&d1, sizeof(d1));
    if (list3) {
        //printPyObject(list2);
        if (!iterator_to_buffer(list3,(char*)&d2, sizeof(d2), "D:1")) {
            print("FAIL  : Could not convert tuple object to C buffer");
        } else {
            if (d1==d2) {
                print("SUCCESS: Two doubles are the same after back and forth conversion");
            } else {
                print("FAIL  : The two doubles");        
            }
        }
    } else {
        print("FAIL  : Could not convert C buffer to Python list");
    }
    
    
    printf("\n\n");
    print ("testing getElemNrFromFormat()...");
    
    if (getElemNrFromFormat(good_format02)==6) {
       print("SUCCESS: Elem nr. test for %s (good format) PASSED", 
              good_format02);
    } else {
       print("ERROR  : Elem nr. test for %s (good format) FAILED.",
             good_format02);
       print("-->Got result %d, expected 6", 
             getElemNrFromFormat(good_format02));
       
    }
        
    if (getElemNrFromFormat(good_format04)==5) {
       print("SUCCESS: Elem nr. test for %s (good format) PASSED", 
              good_format04);
    } else {
       print("ERROR  : Elem nr. test for %s (good format) FAILED", 
              good_format04);
    }
    
    if (getElemNrFromFormat(good_format05)==4) {
       print("SUCCESS: Elem nr. test for %s (good format) PASSED", 
              good_format05);
    } else {
       print("ERROR  : Elem nr. test for %s (good format) FAILED", 
              good_format05);
    }
    
    if (getElemNrFromFormat(good_format06)==8) {
       print("SUCCESS: Elem nr. test for %s (good format) PASSED", 
              good_format06);
    } else {
       print("ERROR  : Elem nr. test for %s (good format) FAILED", 
              good_format06);
    } 
    
    
    printf("\n\n");
    print("Testing iterator_to_allocated_buffer...");
    
    double test04d1[]={100.101, 123}, *test04d2;
    unsigned int test04s=0;
    
    list3 = dim_buf_to_list("d", (const char*)&test04d1, sizeof(test04d1));
    if (list3) {
        //printPyObject(list2);
        if (!iterator_to_allocated_buffer(list3,"D", (char**)&test04d2, &test04s)) {
            print("FAIL  : Could not convert tuple object to C buffer");
        } else {
            if (test04d1[0]==test04d2[0] && test04d1[1]==test04d2[1]) {
                print("SUCCESS: Two doubles are the same after back and forth conversion using dynamic allocated buffers");                
            } else {
                print("FAIL  : The two doubles differ");                        
            }
            delete test04d2;
        }
    } else {
        print("FAIL  : Could not convert C buffer to Python list");
    }
    
    test011 teststrct0401, *teststrct0402;
    teststrct0401.i1=10240;
    teststrct0401.i2=102400;
    teststrct0401.i3=1024000;
    teststrct0401.f1=10240.1023;
    teststrct0401.f2=11111111.11111;
    memcpy(teststrct0401.c1, "ALA BALA PORTOCALA\0\0", 20);
    unsigned int test04s02=0;
    
    list3 = dim_buf_to_list(good_format01, (const char*)&teststrct0401, sizeof(test011));
    if (list3) {
        //printPyObject(list3);
        if (!iterator_to_allocated_buffer(list3, good_format01, (char**)&teststrct0402, &test04s02)) {
            print("FAIL  : Could not convert tuple object to C buffer");
        } else {
            if (!memcmp(&teststrct0401, teststrct0402, sizeof(test011))) {
                print("SUCCESS: Structures are the same after back and forth conversion using dynamic allocated buffers");                
            } else {
                print("FAIL  : The two structures differ");                        
            }
            delete teststrct0402;
        }
    } else {
        print("FAIL  : Could not convert C buffer to Python list");
    }
    
    memcpy(teststrct0401.c1, "CUC\0", 4);
    list3 = dim_buf_to_list(good_format02, (const char*)&teststrct0401, sizeof(test011));
    if (list3) {
        //printPyObject(list3);
        if (!iterator_to_allocated_buffer(list3, good_format02, (char**)&teststrct0402, &test04s02)) {
            print("FAIL  : Could not convert tuple object to C buffer");
        } else {
            if (!memcmp(&teststrct0401, teststrct0402, sizeof(test011)-16) ) {
                print("SUCCESS: Structures are the same after back and forth conversion using dynamic allocated buffers");                
            } else {
                print("FAIL  : The two structures differ");                        
            }
            delete teststrct0402;
        }
    } else {
        print("FAIL  : Could not convert C buffer to Python list");
    }
}
