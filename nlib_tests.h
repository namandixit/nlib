/*
 * Creator: Naman Dixit
 * Notice: © Copyright 2021 Naman Dixit
 */

#if !defined(NLIB_TEST_H_INCLUDE_GUARD)

# define NLIB_TESTS
# include "nlib.h"

# define CHECK_END(str) do { utTest(streq(buf, (str)) && (cast_val(ret, Uint) == strlen(str))); memset(buf, 0, elemin(buf)); } while(0)
# define SPRINTF(b, ...) printString(b, 1024, __VA_ARGS__)
# define SNPRINTF(b, s, ...) cast_val(printString(b, 1024, __VA_ARGS__), Sint)

# define CHECK9(str, v1, v2, v3, v4, v5, v6, v7, v8, v9) do { Size ret = SPRINTF(buf, v1, v2, v3, v4, v5, v6, v7, v8, v9); CHECK_END(str); } while (0)
# define CHECK8(str, v1, v2, v3, v4, v5, v6, v7, v8    ) do { Size ret = SPRINTF(buf, v1, v2, v3, v4, v5, v6, v7, v8    ); CHECK_END(str); } while (0)
# define CHECK7(str, v1, v2, v3, v4, v5, v6, v7        ) do { Size ret = SPRINTF(buf, v1, v2, v3, v4, v5, v6, v7        ); CHECK_END(str); } while (0)
# define CHECK6(str, v1, v2, v3, v4, v5, v6            ) do { Size ret = SPRINTF(buf, v1, v2, v3, v4, v5, v6            ); CHECK_END(str); } while (0)
# define CHECK5(str, v1, v2, v3, v4, v5                ) do { Size ret = SPRINTF(buf, v1, v2, v3, v4, v5                ); CHECK_END(str); } while (0)
# define CHECK4(str, v1, v2, v3, v4                    ) do { Size ret = SPRINTF(buf, v1, v2, v3, v4                    ); CHECK_END(str); } while (0)
# define CHECK3(str, v1, v2, v3                        ) do { Size ret = SPRINTF(buf, v1, v2, v3                        ); CHECK_END(str); } while (0)
# define CHECK2(str, v1, v2                            ) do { Size ret = SPRINTF(buf, v1, v2                            ); CHECK_END(str); } while (0)
# define CHECK1(str, v1                                ) do { Size ret = SPRINTF(buf, v1                                ); CHECK_END(str); } while (0)

header_function
void printUnitTest (void)
{
    Char buf[1024];
    Sint n = 0;
//    double pow_2_75 = 37778931862957161709568.0;

    // integers
    CHECK4("a b     1", "%c %s     %d", 'a', "b", 1);
    CHECK2("abc     ", "%-8.3s", "abcdefgh");
    CHECK2("+5", "%+2d", 5);
    CHECK2("  6", "% 3d", 6);
    CHECK2("-7  ", "%-4d", -7);
    CHECK2("+0", "%+d", 0);
    CHECK3("     00003:     00004", "%10.5d:%10.5d", 3, 4);
    CHECK2("-100006789", "%d", -100006789);
    CHECK3("20 0020", "%u %04u", 20u, 20u);
    CHECK4("12 1e 3C", "%o %x %X", 10u, 30u, 60u);
    CHECK4(" 12 1e 3C ", "%3o %2x %-3X", 10u, 30u, 60u);
    CHECK5("0o12 0O24 0x1e 0X3C", "%#o %#O %#x %#X", 10u, 20u, 30u, 60u);
    CHECK2("", "%.0x", 0);
    CHECK2("",  "%.0d", 0);  // glibc gives "" as specified by C99(?)
    CHECK3("33 5551", "%d %ld", cast_val(33, S16), cast_val(5551, S64));
//    CHECK2("9888777666", "%llu", 9888777666llu); // TODO(naman): Implement %llu
    CHECK4("2 -3 %.", "%zd %td %.", cast_val(2, S64), cast_val(-3, Dptr), 23);

# if !defined(NLIB_PRINT_NO_FLOAT)
    // floating-point numbers
    CHECK2("-3.000000", "%f", -3.0);
    CHECK2("-8.8888888800", "%.10f", -8.88888888);
    CHECK2("880.0888888800", "%.10f", 880.08888888);
    CHECK2("4.1", "%.1f", 4.1);
    CHECK2(" 0", "% .0f", 0.1);
    CHECK2("0.00", "%.2f", 1e-4);
    CHECK2("0.000000499999999999999977", "%.24f", 5e-7);
    CHECK2("-5.20", "%+4.2f", -5.2);
    CHECK2("0.0       ", "%-10.1f", 0.);
    CHECK2("-0.000000", "%f", -0.);
    CHECK2("0.000001", "%f", 9.09834e-07);
    CHECK2("0.000000000000000020000000", "%.24f", 2e-17);
    CHECK3("0.0000000100 100000000", "%.10f %.0f", 1e-8, 1e+8);
    CHECK2("100056789.0", "%.1f", 100056789.0);
    CHECK4(" 1.23 %", "%*.*f %%", 5, 2, 1.23);
    CHECK2("-3.000000e+00", "%e", -3.0);
    CHECK2("4.1E+00", "%.1E", 4.1);
    CHECK2("-5.20e+00", "%+4.2e", -5.2);
#  if !defined(NLIB_PRINT_BAD_FLOAT)
    const double pow_2_85 = 38685626227668133590597632.0;
    CHECK2("38685626227668133590597632.0", "%.1f", pow_2_85); // exact
#  endif

#  if 0 // TODO(naman): Disabled until support for %g is added
    CHECK3("+0.3 -3", "%+g %+g", 0.3, -3.0);
    CHECK2("4", "%.1G", 4.1);
    CHECK2("-5.2", "%+4.2g", -5.2);
    CHECK2("3e-300", "%g", 3e-300);
    CHECK2("1", "%.0g", 1.2);
    CHECK3(" 3.7 3.71", "% .3g %.3g", 3.704, 3.706);
    CHECK3("2e-315:1e+308", "%g:%g", 2e-315, 1e+308);
#  endif

    CHECK4("inf INF nan", "%f %F %f", (double)INFINITY, (double)INFINITY, (double)NAN);
    CHECK2("nan", "%.1f", (double)NAN);

    // hex floats
    CHECK2("0x1.8ffb72e8p+6", "%a", 0x1.fedcbap0+98);
    CHECK2("0x1.375a015a32bc4p+1", "%a", 2.43243424324234234);
    CHECK2("0x1.fedcbap+98", "%a", 0x1.fedcbap+98);
    CHECK2("0x1.999999999999a0p-4", "%.14a", 0.1);
    CHECK2("0x0.ff8p-1022", "%a", 0x1.ffp-1023);
    CHECK2("0x1.0p-1022", "%.1a", 0x1.ffp-1023);
    CHECK2("0x1.0091177587f83p-1022", "%a", 2.23e-308);
    CHECK2("-0X1.AB0P-5", "%.3A", -0X1.abp-5);
# endif // defined(NLIB_PRINT_NO_FLOAT)

    // %p
    CHECK2("0000000000000000", "%p", cast_val(NLIB_NULL, void*));

    // %n
    CHECK3("aaa ", "%.3s %n", "aaaaaaaaaaaaa", &n);
    utTest(n == 4);

    // snprintf
    claim(SNPRINTF(buf, 100, " %s     %d",  "b", 123) == 10);
    claim(strcmp(buf, " b     123") == 0);
    n = SNPRINTF(buf, 0, "7 chars");
    claim(n == 7);
    // stb_sprintf uses internal buffer of 512 chars - test longer string
    claim(SPRINTF(buf, "%d  %600s", 3, "abc") == 603);
    claim(strlen(buf) == 603);
    // TODO(naman): Implement snprintf interface
    //(void)SNPRINTF(buf, 550, "%d  %600s", 3, "abc");
    //claim(strlen(buf) == 549);
    claim(SNPRINTF(buf, 600, "%510s     %c", "a", 'b') == 516);

# if !defined(NLIB_PRINT_NO_FLOAT) && !defined(NLIB_PRINT_BAD_FLOAT)
    const double pow_2_75 = 37778931862957161709568.0;
    claim(SNPRINTF(buf, 100, "%f", pow_2_75) == 30);
    claim(strncmp(buf, "37778931862957161709568.000000", 17) == 0);
    n = SNPRINTF(buf, 10, "number %f", 123.456789);
    // TODO(naman): Implement snprintf interface
    // claim(strcmp(buf, "number 12") == 0);
    claim(n == 17);  // written vs would-be written bytes
# endif

    // length check
    claim(SNPRINTF(NLIB_NULL, 0, " %s     %d",  "b", 123) == 10);

# if defined(NLIB_PRINT_TEST_AGAINST_LIBC)
    setlocale(LC_NUMERIC, "");  // C locale does not group digits
# endif
    // ' modifier. Non-standard, but supported by glibc.
    // TODO(naman): Implement ' modifier
# if 0
    CHECK2("1,200,000", "%'d", 1200000);
    CHECK2("-100,006,789", "%'d", -100006789);
    CHECK2("9,888,777,666", "%'lld", 9888777666ll);
    CHECK2("200,000,000.000000", "%'18f", 2e8);
    CHECK2("100,056,789", "%'.0f", 100056789.0);
    CHECK2("100,056,789.0", "%'.1f", 100056789.0);
    CHECK2("0000001,200,000", "%'015d", 1200000);
# endif

    // things not supported by glibc
    CHECK2("null", "%s", cast_val(NLIB_NULL, Char*));
    CHECK2("100000000", "%b", 256);
    CHECK3("0b10 0B11", "%#b %#B", 2, 3);
# if 0
    CHECK2("123,4abc:", "%'x:", 0x1234ABC);
    CHECK4("2 3 4", "%I64d %I32d %Id", 2ll, 3, 4ll);
    CHECK3("1k 2.54 M", "%$_d %$.2d", 1000, 2536000);
    CHECK3("2.42 Mi 2.4 M", "%$$.2d %$$$d", 2536000, 2536000);

    // different separators
    stbsp_set_separators(' ', ',');
    CHECK2("12 345,678900", "%'f", 12345.6789);
# endif
}

header_function
void internUnitTest (void)
{
    Char x[] = "Hello";
    Char y[] = "Hello";

    Intern_String is = internStringCreate();

    utTest(x != y);

    Char *y_interned = internString(&is, y);
    Char *x_interned = internString(&is, x);
    utTest(x_interned == y_interned);

    Char z[] = "World";
    Char *z_interned = internString(&is, z);
    utTest(x_interned != z_interned);

    Char p[] = "Hello!!";
    Char *p_interned = internString(&is, p);
    utTest(x_interned != p_interned);

    // TODO(naman): Write tests to see what happens if two strings with same hash are interned.

    return;
}

header_function
void raUnitTest (void)
{
    ra(S32) buf = raCreate(buf);
    utTest(ra_IsNULL(buf) == false);

    raAdd(buf, 42);
    utTest(ra_IsNULL(buf) == false);

    raAdd(buf, 1234);

    utTest(raElemin(buf) == 2);
    utTest(raMaxElemin(buf) >= raElemin(buf));

    utTest(buf[0] == 42);
    utTest(buf[1] == 1234);

    raDelete(buf);

    utTest(ra_IsNULL(buf));

    String_Builder stream = sbCreate();
    utTest(sbIsEmpty(stream) == true);
    sbPrint(stream, "Hello, %s\n", "World!");
    sbPrint(stream, "Still here? %d\n", 420);
    sbPrint(stream, "GO AWAY!!!\n");
    utTest(streq(raPtr(stream.str), "Hello, World!\nStill here? 420\nGO AWAY!!!\n"));
    utTest(sbIsEmpty(stream) == false);
    sbDelete(stream);

    /* Char *stream2 = NLIB_NULL; */
    /* Char *string = "Naman Dixit"; */
    /* sbufPrintSized(stream2, 5, "%s", string); */
    /* printf("%s\n%s\n", stream2, string); */
    /* utTest(streq(stream2, "Naman")); */
    /* sbDelete(stream); */
}

header_function
void htUnitTest (void)
{
    Hash_Table ht = htCreate();

    /* No Entries */
    utTest(htLookup(&ht, 0) == 0);
    utTest(htLookup(&ht, 1) == 0);
    utTest(htLookup(&ht, 2) == 0);

    /* Insertion Test */
    Size f0 = ht.slot_filled;

    htInsert(&ht, 0, 0);
    utTest(ht.slot_filled == f0);
    utTest(htLookup(&ht, 0) == 0);
    utTest(htLookup(&ht, 1) == 0);
    utTest(htLookup(&ht, 2) == 0);

    htInsert(&ht, 0, 1);
    utTest(ht.slot_filled == f0);
    utTest(htLookup(&ht, 0) == 0);
    utTest(htLookup(&ht, 1) == 0);
    utTest(htLookup(&ht, 2) == 0);

    htInsert(&ht, 1, 0);
    utTest(ht.slot_filled == f0);
    utTest(htLookup(&ht, 0) == 0);
    utTest(htLookup(&ht, 1) == 0);
    utTest(htLookup(&ht, 2) == 0);

    htInsert(&ht, 1, 1);
    utTest(ht.slot_filled == (f0 + 1));
    utTest(htLookup(&ht, 0) == 0);
    utTest(htLookup(&ht, 1) == 1);
    utTest(htLookup(&ht, 2) == 0);

    htInsert(&ht, 1, 0); // Equivalent to htRemove
    utTest(ht.slot_filled == f0);
    utTest(htLookup(&ht, 0) == 0);
    utTest(htLookup(&ht, 1) == 0);
    utTest(htLookup(&ht, 2) == 0);

    htInsert(&ht, 1, 10);
    utTest(ht.slot_filled == (f0 + 1));
    utTest(htLookup(&ht, 0) == 0);
    utTest(htLookup(&ht, 1) == 10);
    utTest(htLookup(&ht, 2) == 0);

    htInsert(&ht, 2, 42);
    utTest(ht.slot_filled == (f0 + 2));
    utTest(htLookup(&ht, 0) == 0);
    utTest(htLookup(&ht, 1) == 10);
    utTest(htLookup(&ht, 2) == 42);

    /* Duplicate Key */
    U64 v1 = htInsert(&ht, 2, 24);
    utTest(v1 == 42);
    utTest(htLookup(&ht, 0) == 0);
    utTest(htLookup(&ht, 1) == 10);
    utTest(htLookup(&ht, 2) == 24);

    /* Iteration Test */
    U64 k_i, v_i, c_i = 0;
    htForEach (&ht, k_i, v_i) {
        switch (k_i) {
            case 0: utTest(v_i == 0);  c_i++; break;
            case 1: utTest(v_i = 1);   c_i++; break;
            case 2: utTest(v_i == 24); c_i++; break;
        }
    }
    utTest(c_i == 2); // Only keys 1 and 2 wil be returned to us

    /* Removal Test */
    U64 v2 = htRemove(&ht, 2);
    utTest(v2 == 24);
    utTest(htLookup(&ht, 2) == 0);

    U64 v3 = htRemove(&ht, 1);
    utTest(v3 == 10);
    utTest(htLookup(&ht, 1) == 0);

    /* NULL Check */
    Size f1 = ht.slot_filled;
    htInsert(&ht, 0, 1);
    utTest(ht.slot_filled == f1);
    utTest(htLookup(&ht, 0) == 0);

    Size f2 = ht.slot_filled;
    htRemove(&ht, 0);
    utTest(ht.slot_filled == f2);
    utTest(htLookup(&ht, 0) == 0);

    /* Expansion Test */
    htInsert(&ht, 3, 33);
    utTest(htLookup(&ht, 3) == 33);

    htInsert(&ht, 4, 44);
    utTest(htLookup(&ht, 4) == 44);

    htInsert(&ht, 5, 55);
    utTest(htLookup(&ht, 5) == 55);

    htInsert(&ht, 6, 66);
    utTest(htLookup(&ht, 6) == 66);

    htInsert(&ht, 7, 77);
    utTest(htLookup(&ht, 7) == 77);

    htInsert(&ht, 8, 88);
    utTest(htLookup(&ht, 8) == 88);

    htInsert(&ht, 9, 99);
    utTest(htLookup(&ht, 9) == 99);

    /* Removal after Expansion */

    htRemove(&ht, 3);
    utTest(htLookup(&ht, 3) == 0);

    htRemove(&ht, 4);
    utTest(htLookup(&ht, 4) == 0);

    htRemove(&ht, 5);
    utTest(htLookup(&ht, 5) == 0);

    htRemove(&ht, 6);
    utTest(htLookup(&ht, 6) == 0);

    htRemove(&ht, 7);
    utTest(htLookup(&ht, 7) == 0);

    htRemove(&ht, 8);
    utTest(htLookup(&ht, 8) == 0);

    htRemove(&ht, 9);
    utTest(htLookup(&ht, 9) == 0);

    htDelete(ht);

    Hash_Table htalloc = htCreate();
    for (Size i = 1; i < 100; i++) {
        htInsert(&htalloc, i, 200 + i);
        claim(htLookup(&htalloc, i) == (200 + i));
    }
    for (Size i = 1; i < 100; i++) {
        claim(htLookup(&htalloc, i) == (200 + i));
    }
    htDelete(htalloc);

    return;
}

header_function
void mapUnitTest (void)
{
    map(F32) fm = mapCreate(fm);

    /* No Entries */
    utTest(mapExists(fm, 0) == false);
    utTest(mapExists(fm, 1) == false);
    utTest(mapExists(fm, 2) == false);

    /* Insertion Test */

    mapInsert(fm, 1, 1.0f);

    map_DataPtrType(F32) fmd = map_GetDataPtr(fm);
    Size fh0 = fmd->table.slot_filled - 1;
    Size fs0 = map_DirtySlots(fm) - 1;

    utTest(fmd->table.slot_filled == (fh0 + 1));
    utTest(map_DirtySlots(fm) == (fs0 + 1));
    utTest(mapExists(fm, 0) == false);
    utTest(mapExists(fm, 1) == true);
    utTest(mapLookup(fm, 1) == 1.0f);
    utTest(mapExists(fm, 2) == false);

    mapInsert(fm, 2, 42.0f);
    utTest(fmd->table.slot_filled == (fh0 + 2));
    utTest(map_DirtySlots(fm) == (fs0 + 2));
    utTest(mapExists(fm, 0) == false);
    utTest(mapExists(fm, 1) == true);
    utTest(mapLookup(fm, 1) == 1.0f);
    utTest(mapExists(fm, 2) == true);
    utTest(mapLookup(fm, 2) == 42.0f);

    /* Duplicate Key */
    mapInsert(fm, 2, 24.0f);
    utTest(mapLookup(fm, 1) == 1.0f);
    utTest(mapLookup(fm, 2) == 24.0f);

    F32 v_i = 0.0f;
    U64 k_i = 0;
    Size c_i = 0;
    mapForEach(fm, k_i, v_i) {
        switch (k_i) {
            case 1: utTest(v_i == 1.0f);  c_i++; break;
            case 2: utTest(v_i == 24.0f); c_i++; break;
        }
    }
    utTest(c_i == 2);

    /* Removal Test */
    Size fh_r = fmd->table.slot_filled;

    mapRemove(fm, 2);
    utTest(mapExists(fm, 2) == false);
    utTest(fmd->table.slot_filled == fh_r - 1);

    mapRemove(fm, 1);
    utTest(mapExists(fm, 1) == false);
    utTest(fmd->table.slot_filled == fh_r - 2);

    /* NULL Check */
    // NOTE(naman): We just crash on trying to use 0 as key
    /* Size fh1 = fmd->table.slot_filled; */
    /* Size fs1 = map_DirtySlots(fm); */
    /* mapInsert(fm, 0, 13.0f); */
    /* utTest(fmd->table.slot_filled == fh1); */
    /* utTest(map_DirtySlots(fm) == fs1); */
    /* utTest(mapExists(fm, 0) == false); */

    Size fh2 = fmd->table.slot_filled;
    Size fs2 = map_DirtySlots(fm);
    mapRemove(fm, 0);
    utTest(fmd->table.slot_filled == fh2);
    utTest(map_DirtySlots(fm) == fs2);
    utTest(mapExists(fm, 0) == false);

    mapDelete(fm);

    return;
}

# if defined(LANG_C)

#  if 1
#   define ringLocked__TestPP(x) x
#  else
#   define ringLocked__TestPP(x) do {x; int e; sem_getvalue(&ringLocked__GetHead(r)->empty_count, &e); int f; sem_getvalue(&ringLocked__GetHead(r)->fill_count, &f); printf("%s\te:%d f:%d\n", #x, e, f); fflush(stdout); } while (0)
#  endif

header_function
void ringLockedUnitTest (void)
{
#  if defined(OS_WINDOWS)
    report("ringLocked not implemented on Windows\n");
#  else
    Size *r = ringLockedCreate(Size, 4);

    ringLocked__TestPP(ringLockedPush(r, 1));
    ringLocked__TestPP(ringLockedPush(r, 2));
    ringLocked__TestPP(ringLockedPush(r, 3));

    Size rr;

    ringLocked__TestPP(ringLockedPull(r, &rr)); utTest(rr == 1);
    ringLocked__TestPP(ringLockedPull(r, &rr)); utTest(rr == 2);
    ringLocked__TestPP(ringLockedPull(r, &rr)); utTest(rr == 3);

    ringLocked__TestPP(ringLockedPush(r, 4));
    ringLocked__TestPP(ringLockedPush(r, 5));
    ringLocked__TestPP(ringLockedPush(r, 6));
    ringLocked__TestPP(ringLockedPush(r, 7));

    ringLocked__TestPP(ringLockedPull(r, &rr)); utTest(rr == 4);
    ringLocked__TestPP(ringLockedPull(r, &rr)); utTest(rr == 5);
    ringLocked__TestPP(ringLockedPull(r, &rr)); utTest(rr == 6);
    ringLocked__TestPP(ringLockedPull(r, &rr)); utTest(rr == 7);

    ringLockedDelete(r);
#  endif
}
# endif

# if defined(LANG_C)
header_function
void queueLockedUnitTest (void)
{
#  if defined(OS_WINDOWS)
    report("queueLocked not implemented on Windows\n");
#  else
    typedef struct QE {
        Queue_Locked_Entry entry;
        Size s;
    } QE;

    Queue_Locked_Entry *q = queueLockedCreate();

    QE qe1 = {.s = 1};
    queueLockedEnqueue(q, &qe1.entry);
    QE qe2 = {.s = 2};
    queueLockedEnqueue(q, &qe2.entry);
    QE qe3 = {.s = 3};
    queueLockedEnqueue(q, &qe3.entry);

    Queue_Locked_Entry *qr;
    queueLockedDequeue(q, qr); utTest(containerof(qr, QE, entry)->s == 1);
    queueLockedDequeue(q, qr); utTest(containerof(qr, QE, entry)->s == 2);
    queueLockedDequeue(q, qr); utTest(containerof(qr, QE, entry)->s == 3);

    QE qe4 = {.s = 4};
    queueLockedEnqueue(q, &qe4.entry);
    QE qe5 = {.s = 5};
    queueLockedEnqueue(q, &qe5.entry);
    QE qe6 = {.s = 6};
    queueLockedEnqueue(q, &qe6.entry);
    QE qe7 = {.s = 7};
    queueLockedEnqueue(q, &qe7.entry);

    queueLockedDequeue(q, qr); utTest(containerof(qr, QE, entry)->s == 4);
    queueLockedDequeue(q, qr); utTest(containerof(qr, QE, entry)->s == 5);
    queueLockedDequeue(q, qr); utTest(containerof(qr, QE, entry)->s == 6);
    queueLockedDequeue(q, qr); utTest(containerof(qr, QE, entry)->s == 7);

    queueLockedDelete(q);
#  endif
}
# endif

#define NLIB_TEST_H_INCLUDE_GUARD
#endif
