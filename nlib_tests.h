/*
 * Creator: Naman Dixit
 * Notice: © Copyright 2021 Naman Dixit
 */

#if !defined(NLIB_TEST_H_INCLUDE_GUARD)

# define NLIB_TESTS
# include "nlib.h"

header_function
void claimTest (void)
{
    utSuite("Claim", true) {
        utBlock("true") {
            claim(true);
        }
    }
}

header_function
void internUnitTest (void)
{
    utSuite("Intern String", true) {
        Intern_String is = internStringCreate();

        Char x[] = "Hello";
        Char y[] = "Hello";

        utBlock("Compare pointers of literal equal string") {
            utTest(x != y);
        }

        Char *y_interned = internString(&is, y);
        Char *x_interned = internString(&is, x);

        utBlock("Compare reference of interned equal strings") {
            utTest(x_interned == y_interned);
        }

        Char z[] = "World";
        Char *z_interned = internString(&is, z);

        utBlock("Compare reference of interned unequal strings") {
            utTest(x_interned != z_interned);
        }

        Char p[] = "Hello!!";
        Char *p_interned = internString(&is, p);

        utBlock("Compare reference of interned prefix strings") {
            utTest(x_interned != p_interned);
        }

        // TODO(naman): Write tests to see what happens if two strings with same hash are interned.
    }

    return;
}

header_function
void raUnitTest (void)
{
    utSuite("ra(S32)", true) {
        ra(S32) buf = raCreate(buf);

        utBlock("Memory allocation on creation") {
            utTest(ra_IsNULL(buf) == false);
        }

        utBlock("raAdd()") {
            raAdd(buf, 42);
            utTest(ra_IsNULL(buf) == false);

            raAdd(buf, 1234);

            utTest(raElemin(buf) == 2);
            utTest(raMaxElemin(buf) >= raElemin(buf));

            utTest(buf[0] == 42);
            utTest(buf[1] == 1234);
        }

        utBlock("raDelete()") {
            raDelete(buf);

            utTest(ra_IsNULL(buf));
        }
    }
}

header_function
void htUnitTest (void)
{
    utSuite("Hash Table", true) {
        Hash_Table ht = htCreate();

        utBlock("No Entries") {
            utTest(htLookup(&ht, 0) == 0);
            utTest(htLookup(&ht, 1) == 0);
            utTest(htLookup(&ht, 2) == 0);
        }

        Size f0 = ht.slot_filled;

        utBlock("Trying to insert zeroes as keys/values") {
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
        }

        utBlock("Inserting normal keys/values") {
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
        }

        utBlock("Duplicate Key") {
            U64 v1 = htInsert(&ht, 2, 24);
            utTest(v1 == 42);
            utTest(htLookup(&ht, 0) == 0);
            utTest(htLookup(&ht, 1) == 10);
            utTest(htLookup(&ht, 2) == 24);
        }

        utBlock("Iteration Test") {
            U64 k_i, v_i, c_i = 0;
            htForEach (&ht, k_i, v_i) {
                switch (k_i) {
                    case 0: utTest(v_i == 0);  c_i++; break;
                    case 1: utTest(v_i = 1);   c_i++; break;
                    case 2: utTest(v_i == 24); c_i++; break;
                }
            }
            utTest(c_i == 2); // Only keys 1 and 2 wil be returned to us
        }

        utBlock("Removal Test") {
            U64 v2 = htRemove(&ht, 2);
            utTest(v2 == 24);
            utTest(htLookup(&ht, 2) == 0);

            U64 v3 = htRemove(&ht, 1);
            utTest(v3 == 10);
            utTest(htLookup(&ht, 1) == 0);
        }

        utBlock("NULL Check") {
            Size f1 = ht.slot_filled;
            htInsert(&ht, 0, 1);
            utTest(ht.slot_filled == f1);
            utTest(htLookup(&ht, 0) == 0);

            Size f2 = ht.slot_filled;
            htRemove(&ht, 0);
            utTest(ht.slot_filled == f2);
            utTest(htLookup(&ht, 0) == 0);
        }

        utBlock("Expansion Test") {
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
        }

        utBlock("Removal after Expansion") {
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
        }

            htDelete(ht);
    }

    utSuite("Hash Table (forcing allocations)", true) {
        Hash_Table htalloc = htCreate();

        utBlock("Inserts") {
            for (Size i = 1; i < 100; i++) {
                htInsert(&htalloc, i, 200 + i);
                utTest(htLookup(&htalloc, i) == (200 + i));
            }
        }

        utBlock("Lookups") {
            for (Size i = 1; i < 100; i++) {
                utTest(htLookup(&htalloc, i) == (200 + i));
            }
        }

        htDelete(htalloc);
    }

    return;
}

header_function
void mapUnitTest (void)
{
    utSuite("map(F32)", true) {
        map(F32) fm = mapCreate(fm);

        utBlock("mapExists() on empty map") {
            utTest(mapExists(fm, 0) == false);
            utTest(mapExists(fm, 1) == false);
            utTest(mapExists(fm, 2) == false);
        }

        mapInsert(fm, 1, 1.0f);

        map_DataPtrType(F32) fmd = map_GetDataPtr(fm);

        Size fh0 = fmd->table.slot_filled - 1;
        Size fs0 = map_DirtySlots(fm) - 1;

        utBlock("Insertion") {
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
        }

        utBlock("Duplicate Key") {
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
        }

        utBlock("Removal Test") {
            Size fh_r = fmd->table.slot_filled;

            mapRemove(fm, 2);
            utTest(mapExists(fm, 2) == false);
            utTest(fmd->table.slot_filled == fh_r - 1);

            mapRemove(fm, 1);
            utTest(mapExists(fm, 1) == false);
            utTest(fmd->table.slot_filled == fh_r - 2);
        }

        utBlock("Zero insertion (Disabled, because we just crash") {
            // NOTE(naman): We just crash on trying to use 0 as key
            // FIXME(naman): Figure out a better way to deal with zero insertion
            /* Size fh1 = fmd->table.slot_filled; */
            /* Size fs1 = map_DirtySlots(fm); */
            /* mapInsert(fm, 0, 13.0f); */
            /* utTest(fmd->table.slot_filled == fh1); */
            /* utTest(map_DirtySlots(fm) == fs1); */
            /* utTest(mapExists(fm, 0) == false); */
        }

        utBlock("Zero removal") {
            Size fh2 = fmd->table.slot_filled;
            Size fs2 = map_DirtySlots(fm);
            mapRemove(fm, 0);
            utTest(fmd->table.slot_filled == fh2);
            utTest(map_DirtySlots(fm) == fs2);
            utTest(mapExists(fm, 0) == false);
        }

        mapDelete(fm);
    }

    return;
}


#define NLIB_TEST_H_INCLUDE_GUARD
#endif
