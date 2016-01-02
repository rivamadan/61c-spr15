#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <CUnit/Basic.h>

#include "src/utils.h"
#include "src/tables.h"
#include "src/translate_utils.h"
#include "src/translate.h"
#include <string.h>
#include "assembler.h"

#define DEBUG 1 //Change this value if you want to revert to 

#if defined( DEBUG ) && DEBUG == 1


    /* This CU_ASSERT_EQUAL is for comparing two integers. */
    #undef CU_ASSERT_EQUAL
    #define CU_ASSERT_EQUAL(actual, expected) {char condition[100];\
    sprintf(condition, "CU_ASSERT_EQUAL(actual: %d, expected: %d)", (int)actual, (int)expected);\
    CU_assertImplementation((actual == expected), __LINE__, condition, __FILE__,"", CU_FALSE); }


    /* This CU_PASS changes default behavior. It adds one to the "FAILED" tests 
       but does not stop execution of tests. Useful for printing. */
    #undef CU_PASS
    #define CU_PASS(msg) {char condition[100];\
    sprintf(condition, "CU_PASS(%s)", msg);\
    CU_assertImplementation(CU_FALSE, __LINE__, condition, __FILE__, "", CU_FALSE); }


    /* This CU_PASS changes default behavior. It adds one to the "FAILED" tests 
       but does not stop execution of tests. Useful for printing. */
    #undef CU_PASS
    #define CU_PASS(msg) {char condition[100];\
    sprintf(condition, "CU_PASS(%s)", msg);\
    CU_assertImplementation(CU_FALSE, __LINE__, condition, __FILE__, "", CU_FALSE); }


    /* Asserts strings equal, otherwise shows the two comparisons. */
    #undef CU_ASSERT_STRING_EQUAL
    #define CU_ASSERT_STRING_EQUAL(actual, expected) { int size = strlen(actual)\
    + strlen(expected);char condition[size + 20]; sprintf(condition,\
    "CU_ASSERT_STRING_EQUAL(actual: \"%s\", expected: \"%s\")", actual, expected);\
    CU_assertImplementation(!(strcmp((const char*)(actual), (const char*)(expected))),\
    __LINE__, condition, __FILE__, "", CU_FALSE); }

    /* Asserts strings are not equal, otherwise shows the two comparisons. */
    #undef CU_ASSERT_STRING_NOT_EQUAL
    #define CU_ASSERT_STRING_NOT_EQUAL(actual, expected) { int size = strlen(actual)\
    + strlen(expected);char condition[size + 20]; sprintf(condition,\
    "CU_ASSERT_STRING_NOT_EQUAL(actual: \"%s\", expected: \"%s\")", actual, expected);\
    CU_assertImplementation((strcmp((const char*)(actual), (const char*)(expected))),\
    __LINE__, condition, __FILE__, "", CU_FALSE); }

#endif

const char* TMP_FILE = "test_output.txt";
const int BUF_SIZE2 = 1024;

/****************************************
 *  Helper functions 
 ****************************************/

int do_nothing() {
    return 0;
}

int init_log_file() {
    set_log_file(TMP_FILE);
    return 0;
}

int check_lines_equal(char **arr, int num) {
    char buf[BUF_SIZE2];

    FILE *f = fopen(TMP_FILE, "r");
    if (!f) {
        CU_FAIL("Could not open temporary file");
        return 0;
    }
    for (int i = 0; i < num; i++) {
        if (!fgets(buf, BUF_SIZE2, f)) {
            CU_FAIL("Reached end of file");
            return 0;
        }
        CU_ASSERT(!strncmp(buf, arr[i], strlen(arr[i])));
    }
    fclose(f);
    return 0;
}

/****************************************
 *  Test cases for translate_utils.c 
 ****************************************/

void test_translate_reg() {
    CU_ASSERT_EQUAL(translate_reg("$0"), 0);
    CU_ASSERT_EQUAL(translate_reg("$at"), 1);
    CU_ASSERT_EQUAL(translate_reg("$v0"), 2);
    CU_ASSERT_EQUAL(translate_reg("$a0"), 4);
    CU_ASSERT_EQUAL(translate_reg("$a1"), 5);
    CU_ASSERT_EQUAL(translate_reg("$a2"), 6);
    CU_ASSERT_EQUAL(translate_reg("$a3"), 7);
    CU_ASSERT_EQUAL(translate_reg("$t0"), 8);
    CU_ASSERT_EQUAL(translate_reg("$t1"), 9);
    CU_ASSERT_EQUAL(translate_reg("$t2"), 10);
    CU_ASSERT_EQUAL(translate_reg("$t3"), 11);
    CU_ASSERT_EQUAL(translate_reg("$s0"), 16);
    CU_ASSERT_EQUAL(translate_reg("$s1"), 17);
    CU_ASSERT_EQUAL(translate_reg("$3"), -1);
    CU_ASSERT_EQUAL(translate_reg("asdf"), -1);
    CU_ASSERT_EQUAL(translate_reg("hey there"), -1);
}

void test_translate_num() {
    long int output;

    CU_ASSERT_EQUAL(translate_num(&output, "35", -1000, 1000), 0);
    CU_ASSERT_EQUAL(output, 35);
    CU_ASSERT_EQUAL(translate_num(&output, "145634236", 0, 9000000000), 0);
    CU_ASSERT_EQUAL(output, 145634236);
    CU_ASSERT_EQUAL(translate_num(&output, "0xC0FFEE", -9000000000, 9000000000), 0);
    CU_ASSERT_EQUAL(output, 12648430);
    CU_ASSERT_EQUAL(translate_num(&output, "72", -16, 72), 0);
    CU_ASSERT_EQUAL(output, 72);
    CU_ASSERT_EQUAL(translate_num(&output, "72", -16, 71), -1);
    CU_ASSERT_EQUAL(translate_num(&output, "72", 72, 150), 0);
    CU_ASSERT_EQUAL(output, 72);
    CU_ASSERT_EQUAL(translate_num(&output, "72", 73, 150), -1);
    CU_ASSERT_EQUAL(translate_num(&output, "35x", -100, 100), -1);
    CU_ASSERT_EQUAL(translate_num(&output, "0xx", -100, 100), -1);
    CU_ASSERT_EQUAL(translate_num(&output, "zzz", -100, 100), -1);
    CU_ASSERT_EQUAL(translate_num(&output, "0", -100, 100), 0);
    CU_ASSERT_EQUAL(output, 0);

}

/****************************************
 *  Test cases for tables.c 
 ****************************************/

void test_table_1() {
    int retval;

    SymbolTable* tbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(tbl);

    retval = add_to_table(tbl, "abc", 8);
    CU_ASSERT_EQUAL(retval, 0);
    retval = add_to_table(tbl, "efg", 12);
    CU_ASSERT_EQUAL(retval, 0);
    retval = add_to_table(tbl, "q45", 16);
    CU_ASSERT_EQUAL(retval, 0);
    retval = add_to_table(tbl, "q45", 24); 
    CU_ASSERT_EQUAL(retval, -1); 
    retval = add_to_table(tbl, "bob", 14); 
    CU_ASSERT_EQUAL(retval, -1); 

    retval = get_addr_for_symbol(tbl, "abc");
    CU_ASSERT_EQUAL(retval, 8); 
    retval = get_addr_for_symbol(tbl, "q45");
    CU_ASSERT_EQUAL(retval, 16); 
    retval = get_addr_for_symbol(tbl, "ef");
    CU_ASSERT_EQUAL(retval, -1);

    FILE* test_output = fopen("test_write_table.txt", "w");
    write_table(tbl, test_output);
    fclose(test_output);
    
    free_table(tbl);

    char* arr[] = { "Error: name 'q45' already exists in table.",
                    "Error: address is not a multiple of 4." };
    check_lines_equal(arr, 2);

    SymbolTable* tbl2 = create_table(SYMTBL_NON_UNIQUE);
    CU_ASSERT_PTR_NOT_NULL(tbl2);

    retval = add_to_table(tbl2, "q45", 16);
    CU_ASSERT_EQUAL(retval, 0);
    retval = add_to_table(tbl2, "q45", 24); 
    CU_ASSERT_EQUAL(retval, 0);

    free_table(tbl2);
}

void test_table_2() {
    int retval, max = 100;

    SymbolTable* tbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(tbl);

    char buf[10];
    for (int i = 0; i < max; i++) {
        sprintf(buf, "%d", i);
        retval = add_to_table(tbl, buf, 4 * i);
        CU_ASSERT_EQUAL(retval, 0);
    }

    for (int i = 0; i < max; i++) {
        sprintf(buf, "%d", i);
        retval = get_addr_for_symbol(tbl, buf);
        CU_ASSERT_EQUAL(retval, 4 * i);
    }

    free_table(tbl);
}



/****************************************
 * Testing translate_inst
 ****************************************/

void test_rtype() {
    int retval;

    SymbolTable* symtbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(symtbl);

    SymbolTable* reltbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(reltbl);

    FILE* test_output = fopen("test_rtype.txt", "w");
    uint32_t addr = 4; 
    char* name = "addu";

    char *args[3] = {"$v0", "$a0", "$a1"};
    retval = translate_inst(test_output, name, args, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, 0);

    char *args1[3] = {"$v0", "$a0"};
    retval = translate_inst(test_output, name, args1, 2, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args2[3] = {"$v0", "$a0", "$1"};
    retval = translate_inst(test_output, name, args2, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);
    
    fclose(test_output);
}

void test_shift() {
    int retval;

    SymbolTable* symtbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(symtbl);

    SymbolTable* reltbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(reltbl);

    FILE* test_output = fopen("test_shift.txt", "w");
    uint32_t addr = 4; 
    char* name = "sll";

    char *args[3] = {"$v0", "$a0", "2"};
    retval = translate_inst(test_output, name, args, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, 0);

    char *args1[3] = {"$v0", "$a0", "32"};
    retval = translate_inst(test_output, name, args1, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args2[3] = {"$t0", "1"};
    retval = translate_inst(test_output, name, args2, 2, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args3[3] = {"$t1", "$a0", "$s1"};
    retval = translate_inst(test_output, name, args3, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args4[3] = {"$t1", "$1", "1"};
    retval = translate_inst(test_output, name, args4, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    fclose(test_output);
}

void test_addiu() {
    int retval;

    SymbolTable* symtbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(symtbl);

    SymbolTable* reltbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(reltbl);

    FILE* test_output = fopen("test_addiu.txt", "w");
    uint32_t addr = 4; 
    char* name = "addiu";

    char *args[3] = {"$v0", "$t0", "0x8000"};
    retval = translate_inst(test_output, name, args, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args1[3] = {"$s0", "$t1", "15"};
    retval = translate_inst(test_output, name, args1, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, 0);

    char *args2[3] = {"$t0", "1"};
    retval = translate_inst(test_output, name, args2, 2, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args3[3] = {"$t1", "$a0", "$s1"};
    retval = translate_inst(test_output, name, args3, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args4[3] = {"$t1", "$1", "1"};
    retval = translate_inst(test_output, name, args4, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    fclose(test_output);
    free_table(symtbl);
    free_table(reltbl);
}

void test_ori() {
    int retval;

    SymbolTable* symtbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(symtbl);

    SymbolTable* reltbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(reltbl);

    FILE* test_output = fopen("test_ori.txt", "w");
    uint32_t addr = 4; 
    char* name = "ori";

    char *args[3] = {"$v0", "$a0", "2"};
    retval = translate_inst(test_output, name, args, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, 0);

    char *args1[3] = {"$v0", "$a0", "-1"};
    retval = translate_inst(test_output, name, args1, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args2[3] = {"$t0", "1"};
    retval = translate_inst(test_output, name, args2, 2, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args3[3] = {"$t1", "$a0", "$s1"};
    retval = translate_inst(test_output, name, args3, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args4[3] = {"$t1", "$1", "1"};
    retval = translate_inst(test_output, name, args4, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    fclose(test_output);
    free_table(symtbl);
    free_table(reltbl);
}

void test_lui() {
    int retval;

    SymbolTable* symtbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(symtbl);

    SymbolTable* reltbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(reltbl);

    FILE* test_output = fopen("test_lui.txt", "w");
    uint32_t addr = 4; 
    char* name = "lui";

    char *args[3] = {"$at","2"};
    retval = translate_inst(test_output, name, args, 2, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, 0);

    char *args1[3] = {"$at","0x8000"};
    retval = translate_inst(test_output, name, args1, 2, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args2[3] = {"1"};
    retval = translate_inst(test_output, name, args2, 1, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args3[3] = {"$t1", "$a0", "$s1"};
    retval = translate_inst(test_output, name, args3, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    fclose(test_output);
    free_table(symtbl);
    free_table(reltbl);
}

void test_mem() {
    int retval;

    SymbolTable* symtbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(symtbl);

    SymbolTable* reltbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(reltbl);

    FILE* test_output = fopen("test_mem.txt", "w");
    uint32_t addr = 4; 
    char* name = "lw";

    char *args[3] = {"$v0", "0", "$a0"};
    retval = translate_inst(test_output, name, args, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, 0);

    char *args1[3] = {"$v0", "$a0", "1"};
    retval = translate_inst(test_output, name, args1, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args2[3] = {"$t0", "1"};
    retval = translate_inst(test_output, name, args2, 2, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args3[3] = {"$t1", "$a0", "$s1"};
    retval = translate_inst(test_output, name, args3, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args4[3] = {"$t1", "$1", "1"};
    retval = translate_inst(test_output, name, args4, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    fclose(test_output);
    free_table(symtbl);
    free_table(reltbl);
}

void test_jr() {
    int retval;

    SymbolTable* symtbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(symtbl);

    SymbolTable* reltbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(reltbl);

    FILE* test_output = fopen("test_jr.txt", "w");
    uint32_t addr = 4; 
    char* name = "jr";

    char *args[3] = {"$ra"};
    retval = translate_inst(test_output, name, args, 1, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, 0);

    char *args1[3] = {"-1"};
    retval = translate_inst(test_output, name, args1, 1, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args2[3] = {"$t0", "1"};
    retval = translate_inst(test_output, name, args2, 2, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args3[3] = {"$t1", "$a0", "$s1"};
    retval = translate_inst(test_output, name, args3, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    fclose(test_output);
    free_table(symtbl);
    free_table(reltbl);
}

void test_jump() {
    int retval;

    SymbolTable* symtbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(symtbl);

    SymbolTable* reltbl = create_table(SYMTBL_NON_UNIQUE);
    CU_ASSERT_PTR_NOT_NULL(reltbl);

    FILE* test_output = fopen("test_jump.txt", "w");
    uint32_t addr = 4;
    char* name = "jal";

    char *args[3] = {"label"};
    retval = translate_inst(test_output, name, args, 1, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, 0);

    retval = get_addr_for_symbol(reltbl, "label");
    CU_ASSERT_EQUAL(retval, 4); 

    addr = 8; 
    char *args1[3] = {"label"};
    retval = translate_inst(test_output, name, args1, 1, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, 0);

    addr = 12; 
    char *args2[3] = {"new_label"};
    retval = translate_inst(test_output, name, args2, 1, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, 0);

    addr = 7; 
    char *args3[3] = {"label"};
    retval = translate_inst(test_output, name, args3, 1, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args4[3] = {"one", "2"};
    retval = translate_inst(test_output, name, args4, 2, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    fclose(test_output);
    free_table(symtbl);
    free_table(reltbl);
}

void test_branch() {
    int retval;

    SymbolTable* symtbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(symtbl);

    retval = add_to_table(symtbl, "abc", 8);
    CU_ASSERT_EQUAL(retval, 0);
    retval = add_to_table(symtbl, "efg", 12);
    CU_ASSERT_EQUAL(retval, 0);

    SymbolTable* reltbl = create_table(SYMTBL_UNIQUE_NAME);
    CU_ASSERT_PTR_NOT_NULL(reltbl);

    FILE* test_output = fopen("test_branch.txt", "w");
    uint32_t addr = 4; 
    char* name = "beq";

    char *args[3] = {"$t0", "$0", "abc"};
    retval = translate_inst(test_output, name, args, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, 0);

    char *args1[3] = {"$t0", "$0", "no"};
    retval = translate_inst(test_output, name, args1, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args2[3] = {"$t0", "1"};
    retval = translate_inst(test_output, name, args2, 2, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    char *args3[3] = {"$t1", "$a", "label"};
    retval = translate_inst(test_output, name, args3, 3, addr, symtbl, reltbl);
    CU_ASSERT_EQUAL(retval, -1);

    fclose(test_output);
    free_table(symtbl);
    free_table(reltbl);
}

/****************************************
 * Testing pass one and pass two
 ****************************************/

// void test_pass_one() {
//     int retval; 

//     SymbolTable* symtbl = create_table(SYMTBL_UNIQUE_NAME);
//     CU_ASSERT_PTR_NOT_NULL(symtbl);

//     SymbolTable* reltbl = create_table(SYMTBL_NON_UNIQUE);
//     CU_ASSERT_PTR_NOT_NULL(reltbl);

//     FILE* test_input = fopen("/input/simple.s", "r");
//     FILE* test_output = fopen("/out/simple.int", "w");

//     retval = pass_one(test_input, test_output, symtbl);
//     CU_ASSERT_EQUAL(retval, 0); 


//     fclose(test_output);
//     free_table(symtbl);
//     free_table(reltbl);
// }

// void test_pass_two() {
//     int retval; 

//     SymbolTable* symtbl = create_table(SYMTBL_UNIQUE_NAME);
//     CU_ASSERT_PTR_NOT_NULL(symtbl);

//     SymbolTable* reltbl = create_table(SYMTBL_NON_UNIQUE);
//     CU_ASSERT_PTR_NOT_NULL(reltbl);


//     FILE* test_input = fopen("/out/ref/simple_ref.int", "r");
//     FILE* test_output = fopen("/out/simple.out", "w");


//     retval = pass_two(test_input, test_output, symtbl, reltbl);
//     CU_ASSERT_EQUAL(retval, 0); 


//     fclose(test_input);
//     fclose(test_output);
//     free_table(symtbl);
//     free_table(reltbl);
// }


/****************************************
 * Testing ADD TEST NAME
 ****************************************/

int main(int argc, char** argv) {
    CU_pSuite pSuite1 = NULL, pSuite2 = NULL;

    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    /* Suite 1 */
    pSuite1 = CU_add_suite("Testing translate_utils.c", NULL, NULL);
    if (!pSuite1) {
        goto exit;
    }
    if (!CU_add_test(pSuite1, "test_translate_reg", test_translate_reg)) {
        goto exit;
    }
    if (!CU_add_test(pSuite1, "test_translate_num", test_translate_num)) {
        goto exit;
    }

    /* Suite 2 */
    pSuite2 = CU_add_suite("Testing tables.c", init_log_file, NULL);
    if (!pSuite2) {
        goto exit;
    }
    if (!CU_add_test(pSuite2, "test_table_1", test_table_1)) {
        goto exit;
    }
    if (!CU_add_test(pSuite2, "test_table_2", test_table_2)) {
        goto exit;
    }

    CU_pSuite pSuite3 = NULL;


    /* Suite 3 */
    pSuite3 = CU_add_suite("Testing translate_inst", NULL, NULL);
      if (!pSuite3) {
        goto exit;
      }

    if (!CU_add_test(pSuite3, "test_rtype", test_rtype)) {
        goto exit;
    }

    if (!CU_add_test(pSuite3, "test_shift", test_shift)) {
        goto exit;
    }

    if (!CU_add_test(pSuite3, "test_addiu", test_addiu)) {
        goto exit;
    }

    if (!CU_add_test(pSuite3, "test_ori", test_ori)) {
        goto exit;
    }

    if (!CU_add_test(pSuite3, "test_mem", test_mem)) {
        goto exit;
    }

    if (!CU_add_test(pSuite3, "test_lui", test_lui)) {
        goto exit;
    }

    if (!CU_add_test(pSuite3, "test_jr", test_jr)) {
        goto exit;
    }

    if (!CU_add_test(pSuite3, "test_jump", test_jump)) {
        goto exit;
    }

    if (!CU_add_test(pSuite3, "test_branch", test_branch)) {
        goto exit;
    }

    // CU_pSuite pSuite5 = NULL;

    // pSuite5 = CU_add_suite("Testing pass_one and pass_two", NULL, NULL); 
    // if (!pSuite5) {
    //     goto exit;
    // }
    // if (!CU_add_test(pSuite5, "test_pass_one", test_pass_one)) {
    //     goto exit;
    // }
    // if (!CU_add_test(pSuite5, "test_pass_two", test_pass_two)) {
    //     goto exit; 
    // }


    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

exit:
    CU_cleanup_registry();
    return CU_get_error();;
}
