/*********************************************************************
    Blosc - Blocked Shuffling and Compression Library

    Copyright (C) 2021  The Blosc Developers <blosc@blosc.org>
    https://blosc.org
    License: BSD 3-Clause (see LICENSE.txt)

    See LICENSE.txt for details about copyright and rights to use.

    Test program demonstrating use of the Blosc filter from C code.
    To compile this program:

    $ gcc -O test_ndcell.c -o test_ndcell -lblosc2

    To run:

    $ ./test_ndcell
    Blosc version info: 2.0.0a6.dev ($Date:: 2018-05-18 #$)
    Using 1 thread
    Using ZSTD compressor
    Successful roundtrip!
    Compression: 41472 -> 3992 (10.4x)
    rand: 37480 obtained

    Successful roundtrip!
    Compression: 1792 -> 979 (1.8x)
    same_cells: 813 obtained

    Successful roundtrip!
    Compression: 16128 -> 1438 (11.2x)
    some_matches: 14690 obtained

**********************************************************************/

#include <stdio.h>
#include "ndcell.h"
#include <inttypes.h>
#include "blosc2/filters-registry.h"

static int test_ndcell(blosc2_schunk* schunk) {

    int nchunks = schunk->nchunks;
    int32_t chunksize = (int32_t) (schunk->chunksize);
    //   int isize = (int) array->extchunknitems * typesize;
    uint8_t *data_in = malloc(chunksize);
    int decompressed;
    int64_t csize;
    int64_t dsize;
    int64_t csize_f = 0;
    uint8_t *data_out = malloc(chunksize + BLOSC_MAX_OVERHEAD);
    uint8_t *data_dest = malloc(chunksize);

    /* Create a context for compression */
    blosc2_cparams cparams = BLOSC2_CPARAMS_DEFAULTS;
    cparams.splitmode = BLOSC_ALWAYS_SPLIT;
    cparams.typesize = schunk->typesize;
    cparams.compcode = BLOSC_ZSTD;
    cparams.filters[4] = BLOSC_FILTER_NDCELL;
    cparams.filters_meta[4] =4;
    cparams.filters[BLOSC2_MAX_FILTERS - 1] = BLOSC_SHUFFLE;
    cparams.clevel = 9;
    cparams.nthreads = 1;
    cparams.blocksize = schunk->blocksize;
    cparams.schunk = schunk;
    blosc2_context *cctx;
    cctx = blosc2_create_cctx(cparams);

    blosc2_dparams dparams = BLOSC2_DPARAMS_DEFAULTS;
    dparams.nthreads = 1;
    dparams.schunk = schunk;
    blosc2_context *dctx;
    dctx = blosc2_create_dctx(dparams);

    for (int ci = 0; ci < nchunks; ci++) {

        decompressed = blosc2_schunk_decompress_chunk(schunk, ci, data_in, chunksize);
        if (decompressed < 0) {
            printf("Error decompressing chunk \n");
            return -1;
        }

        /* Compress with clevel=5 and shuffle active  */
        csize = blosc2_compress_ctx(cctx, data_in, chunksize, data_out, chunksize + BLOSC_MAX_OVERHEAD);
        if (csize == 0) {
            printf("Buffer is uncompressible.  Giving up.\n");
            return 0;
        } else if (csize < 0) {
            printf("Compression error.  Error code: %" PRId64 "\n", csize);
            return (int) csize;
        }
        csize_f += csize;

        /* Decompress  */
        dsize = blosc2_decompress_ctx(dctx, data_out, chunksize + BLOSC_MAX_OVERHEAD, data_dest, chunksize);
        if (dsize <= 0) {
            printf("Decompression error.  Error code: %" PRId64 "\n", dsize);
            return (int) dsize;
        }

        for (int i = 0; i < chunksize; i++) {
            if (data_in[i] != data_dest[i]) {
                printf("i: %d, data %u, dest %u", i, data_in[i], data_dest[i]);
                printf("\n Decompressed data differs from original!\n");
                return -1;
            }
        }
    }
    csize_f = csize_f / nchunks;

    free(data_in);
    free(data_out);
    free(data_dest);
    blosc2_free_ctx(cctx);
    blosc2_free_ctx(dctx);

    printf("Successful roundtrip!\n");
    printf("Compression: %d -> %" PRId64 " (%.1fx)\n", chunksize, csize_f, (1. * chunksize) / csize_f);
    return (int) (chunksize - csize_f);
}


int rand_() {
    blosc2_schunk *schunk = blosc2_schunk_open("example_rand.caterva");

    /* Run the test. */
    int result = test_ndcell(schunk);
    blosc2_schunk_free(schunk);
    return result;
}

int same_cells() {
    blosc2_schunk *schunk = blosc2_schunk_open("example_same_cells.caterva");

    /* Run the test. */
    int result = test_ndcell(schunk);
    blosc2_schunk_free(schunk);
    return result;
}

int some_matches() {
    blosc2_schunk *schunk = blosc2_schunk_open("example_some_matches.caterva");

    /* Run the test. */
    int result = test_ndcell(schunk);
    blosc2_schunk_free(schunk);
    return result;
}


int main(void) {

    int result;
    blosc_init();
    result = rand_();
    printf("rand: %d obtained \n \n", result);
    result = same_cells();
    printf("same_cells: %d obtained \n \n", result);
    result = some_matches();
    printf("some_matches: %d obtained \n \n", result);
    blosc_destroy();
    return BLOSC2_ERROR_SUCCESS;
}
