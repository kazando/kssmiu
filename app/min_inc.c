#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "common/list.h"
#include "common/matrix.h"
#include "common/tree.h"
#include "common/linkage.h"
#include "kss/kss.h"

static int parse_mode(const char *arg) {
    if (arg == NULL) {
        return 0;
    }
    if (strcmp(arg, "l1") == 0 || strcmp(arg, "1") == 0) {
        return 1;
    }
    if (strcmp(arg, "l2") == 0 || strcmp(arg, "2") == 0) {
        return 2;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int i, j, k_num;
    int mode = 1;
    int mode_arg_index = 1;
    Matrix M, D, X, A, K, H;
    IntMatrix LCA;
    Tree T;
    List order, Renew, order2;

    char file_name1[1024];
    char tree_outfile[1024] = "";
    int write_tree = 0;

    if (argc < 3 || argc > 5) {
        printf("Usage: %s [l1|l2] <input file> <k> [tree output file]\n", argv[0]);
        printf("       %s <input file> <k> [tree output file]\n", argv[0]);
        return 0;
    }

    if (parse_mode(argv[1]) != 0) {
        if (argc < 4) {
            printf("Usage: %s [l1|l2] <input file> <k> [tree output file]\n", argv[0]);
            return 0;
        }
        mode = parse_mode(argv[1]);
        mode_arg_index = 2;
    } else {
        if (argc > 4) {
            printf("Usage: %s [l1|l2] <input file> <k> [tree output file]\n", argv[0]);
            return 0;
        }
        mode = 1;
        mode_arg_index = 1;
    }

    strcpy(file_name1, argv[mode_arg_index]);
    read_matrix_phylip(file_name1, &M);
    k_num = atoi(argv[mode_arg_index + 1]);
    if (argc == mode_arg_index + 3) {
        strcpy(tree_outfile, argv[mode_arg_index + 2]);
        write_tree = 1;
    }
    if (M.n < k_num) {
        printf("Error: <k> must be at most order of M.\n");
        return 0;
    }

    T.n = 2 * M.n - 1;
    T.m = 2 * M.n - 2;
    initialize_tree(&T, T.n, T.m);

    init_matrix(&D, T.n);
    init_matrix(&X, M.n);
    init_matrix2(&LCA, T.n);
    init_matrix(&A, T.n);
    init_matrix(&K, T.n);
    if (mode == 2) {
        init_matrix(&H, T.n);
    }
    for (i = 1; i <= T.n; i++) {
        for (j = 1; j <= T.n; j++) {
            A.cpn[i][j] = K.cpn[i][j] = 0;
            if (mode == 2) {
                H.cpn[i][j] = 0;
            }
        }
    }

    initialize(&order);
    for (i = 1; i <= T.n; i++) {
        insertBefore(order.tail, i);
    }
    initialize(&order2);
    for (i = 1; i <= T.n; i++) {
        insertBefore(order2.tail, i);
    }
    initialize(&Renew);

    c_link(M, &D, &T);
    make_parent(&T);
    make_numleaves(&T, M.n);
    make_brother(&T);

    if (mode == 2) {
        algo_ope2(M, &X, &A, &K, &H, &LCA, &T, &order, &order2, &Renew, k_num, file_name1);
    } else {
        algo_ope(M, &X, &A, &K, &LCA, &T, &order, &order2, &Renew, k_num, file_name1);
    }

    if (write_tree) {
        write_tree_newick_with_height(&M, &T, tree_outfile);
    }

    terminate_tree(&T);
    terminate_list(&order);
    terminate_list(&order2);
    terminate_list(&Renew);
    del_matrix(&M);
    del_matrix(&D);
    del_matrix(&X);
    del_matrix(&A);
    del_matrix(&K);
    if (mode == 2) {
        del_matrix(&H);
    }
    del_matrix2(&LCA);

    return 0;
}
