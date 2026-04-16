/*
 * Q2: Simulation & Analysis of Memory Allocation Algorithms
 *
 * Test Cases:
 *  TC1 – Allocate 5 fixed processes (64, 128, 32, 256, 16 KB)
 *  TC2 – Free some, reallocate
 *  TC3 – Stress test: 20+ random allocations/deallocations
 *
 * Metrics tracked:
 *  - Success rate  (% of successful allocations)
 *  - Memory utilisation (used / total)
 *  - Internal fragmentation (wasted inside allocated blocks)
 *  - External fragmentation (usable free memory split across blocks)
 *  - Execution time (clock ticks per algorithm)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ─── Configuration ──────────────────────────── */
#define MEMORY_SIZE        1024   /* KB */
#define QUICK_FIT_CLASSES     4
#define MAX_PROCESSES       100
#define STRESS_ALLOC_COUNT   25
#define MIN_PROC_SIZE        16   /* KB */
#define MAX_PROC_SIZE       256   /* KB */

int quick_fit_sizes[QUICK_FIT_CLASSES] = {32, 64, 128, 256};

/* ─── Data Structures ────────────────────────── */
typedef struct MemBlock {
    int block_id;
    int process_id;
    int start;
    int size;
    int allocated_size;
    struct MemBlock *next;
    struct MemBlock *prev;
} MemBlock;

typedef struct {
    MemBlock *head;
    MemBlock *last_allocated;
    int total_size;
} MemoryManager;

typedef struct QuickNode {
    MemBlock *block;
    struct QuickNode *next;
} QuickNode;

QuickNode *quick_lists[QUICK_FIT_CLASSES];

/* ─── Metrics ────────────────────────────────── */
typedef struct {
    int   total_requests;
    int   successful;
    int   failed;
    double utilisation_pct;
    int   internal_frag_kb;
    int   external_frag_kb;
    double exec_time_ms;
} Metrics;

/* ─── Helpers ────────────────────────────────── */
static int blk_counter = 1;

MemBlock *create_block(int start, int size) {
    MemBlock *b = (MemBlock *)malloc(sizeof(MemBlock));
    b->block_id       = blk_counter++;
    b->process_id     = -1;
    b->start          = start;
    b->size           = size;
    b->allocated_size = 0;
    b->next = b->prev = NULL;
    return b;
}

void reset_quick_lists(void) {
    for (int i = 0; i < QUICK_FIT_CLASSES; i++) {
        QuickNode *q = quick_lists[i];
        while (q) { QuickNode *t = q; q = q->next; free(t); }
        quick_lists[i] = NULL;
    }
}

void free_list(MemoryManager *mm) {
    MemBlock *c = mm->head;
    while (c) { MemBlock *t = c; c = c->next; free(t); }
    mm->head = NULL;
}

void initialize_memory(MemoryManager *mm) {
    mm->head = NULL; mm->last_allocated = NULL;
    blk_counter = 1;
    free_list(mm);
    reset_quick_lists();
    mm->head           = create_block(0, MEMORY_SIZE);
    mm->last_allocated = NULL;
    mm->total_size     = MEMORY_SIZE;
}

/* ─── Split & Allocate ───────────────────────── */
MemBlock *split_alloc(MemoryManager *mm, MemBlock *blk, int size, int pid) {
    if (blk->size > size + 1) {
        MemBlock *rem = create_block(blk->start + size, blk->size - size);
        rem->next = blk->next;
        rem->prev = blk;
        if (blk->next) blk->next->prev = rem;
        blk->next = rem;
        blk->size = size;
    }
    blk->process_id     = pid;
    blk->allocated_size = size;
    mm->last_allocated  = blk;
    return blk;
}

/* ─── Algorithm Implementations ─────────────── */
MemBlock *first_fit(MemoryManager *mm, int size, int pid) {
    for (MemBlock *c = mm->head; c; c = c->next)
        if (c->process_id == -1 && c->size >= size)
            return split_alloc(mm, c, size, pid);
    return NULL;
}

MemBlock *next_fit(MemoryManager *mm, int size, int pid) {
    MemBlock *start = mm->last_allocated
                      ? (mm->last_allocated->next ? mm->last_allocated->next
                                                   : mm->head)
                      : mm->head;
    for (MemBlock *c = start; c; c = c->next)
        if (c->process_id == -1 && c->size >= size)
            return split_alloc(mm, c, size, pid);
    for (MemBlock *c = mm->head; c && c != start; c = c->next)
        if (c->process_id == -1 && c->size >= size)
            return split_alloc(mm, c, size, pid);
    return NULL;
}

MemBlock *best_fit(MemoryManager *mm, int size, int pid) {
    MemBlock *best = NULL;
    for (MemBlock *c = mm->head; c; c = c->next)
        if (c->process_id == -1 && c->size >= size)
            if (!best || c->size < best->size) best = c;
    return best ? split_alloc(mm, best, size, pid) : NULL;
}

MemBlock *worst_fit(MemoryManager *mm, int size, int pid) {
    MemBlock *worst = NULL;
    for (MemBlock *c = mm->head; c; c = c->next)
        if (c->process_id == -1 && c->size >= size)
            if (!worst || c->size > worst->size) worst = c;
    return worst ? split_alloc(mm, worst, size, pid) : NULL;
}

void quick_add(MemBlock *blk) {
    int idx = QUICK_FIT_CLASSES - 1;
    for (int i = 0; i < QUICK_FIT_CLASSES; i++)
        if (blk->size <= quick_fit_sizes[i]) { idx = i; break; }
    QuickNode *qn = (QuickNode *)malloc(sizeof(QuickNode));
    qn->block = blk; qn->next = quick_lists[idx];
    quick_lists[idx] = qn;
}

void quick_remove(MemBlock *blk) {
    for (int i = 0; i < QUICK_FIT_CLASSES; i++) {
        QuickNode **c = &quick_lists[i];
        while (*c) {
            if ((*c)->block == blk) {
                QuickNode *t = *c; *c = t->next; free(t); return;
            }
            c = &(*c)->next;
        }
    }
}

MemBlock *quick_fit(MemoryManager *mm, int size, int pid) {
    for (int i = 0; i < QUICK_FIT_CLASSES; i++) {
        if (size <= quick_fit_sizes[i]) {
            QuickNode **c = &quick_lists[i];
            while (*c) {
                MemBlock *b = (*c)->block;
                if (b->process_id == -1 && b->size >= size) {
                    QuickNode *t = *c; *c = t->next; free(t);
                    return split_alloc(mm, b, size, pid);
                }
                c = &(*c)->next;
            }
        }
    }
    return first_fit(mm, size, pid);
}

/* ─── Allocate / Free / Coalesce ─────────────── */
MemBlock *allocate_memory(MemoryManager *mm, int strat, int size, int pid) {
    switch (strat) {
        case 1: return first_fit (mm, size, pid);
        case 2: return next_fit  (mm, size, pid);
        case 3: return best_fit  (mm, size, pid);
        case 4: return worst_fit (mm, size, pid);
        case 5: return quick_fit (mm, size, pid);
    }
    return NULL;
}

void coalesce_free_blocks(MemoryManager *mm) {
    MemBlock *c = mm->head;
    while (c && c->next) {
        if (c->process_id == -1 && c->next->process_id == -1) {
            MemBlock *n = c->next;
            c->size += n->size;
            c->next  = n->next;
            if (n->next) n->next->prev = c;
            free(n);
        } else c = c->next;
    }
}

void free_memory(MemoryManager *mm, int pid) {
    for (MemBlock *c = mm->head; c; c = c->next) {
        if (c->process_id == pid) {
            c->process_id = -1; c->allocated_size = 0;
            quick_add(c);
        }
    }
    coalesce_free_blocks(mm);
}

/* ─── Metrics Calculation ────────────────────── */
void compute_metrics(MemoryManager *mm, Metrics *m) {
    int used = 0, free_mem = 0, int_frag = 0;
    int largest_free = 0, free_blocks = 0;
    for (MemBlock *c = mm->head; c; c = c->next) {
        if (c->process_id == -1) {
            free_mem += c->size;
            free_blocks++;
            if (c->size > largest_free) largest_free = c->size;
        } else {
            used     += c->allocated_size;
            int_frag += c->size - c->allocated_size;
        }
    }
    m->utilisation_pct  = (double)used / mm->total_size * 100.0;
    m->internal_frag_kb = int_frag;
    /* External fragmentation = free memory that cannot be used for
       the largest possible request due to fragmentation            */
    m->external_frag_kb = (free_blocks > 1) ? (free_mem - largest_free) : 0;
}

void display_memory(MemoryManager *mm) {
    printf("  %-6s %-6s %-10s %-10s %s\n",
           "BlkID","PID","Start(KB)","Size(KB)","Status");
    printf("  %s\n","---------------------------------------------------");
    for (MemBlock *c = mm->head; c; c = c->next)
        printf("  %-6d %-6d %-10d %-10d %s\n",
               c->block_id, c->process_id, c->start, c->size,
               c->process_id==-1?"FREE":"USED");
    printf("\n");
}

/* ─── Test Case Runners ───────────────────────── */

void run_test_case(int tc, int strategy, const char *sname,
                   int *req_sizes, int n_req,
                   int *free_pids,  int n_free,
                   int *realloc_sizes, int n_realloc,
                   int verbose) {
    MemoryManager mm;
    initialize_memory(&mm);

    Metrics m = {0};
    clock_t t_start = clock();

    /* Initial allocations */
    int pid_counter = 200;
    int pids[MAX_PROCESSES];
    for (int i = 0; i < n_req && i < MAX_PROCESSES; i++) {
        pids[i] = pid_counter++;
        MemBlock *r = allocate_memory(&mm, strategy, req_sizes[i], pids[i]);
        m.total_requests++;
        if (r) m.successful++; else m.failed++;
    }

    if (verbose) { printf("  After initial allocations:\n"); display_memory(&mm); }

    /* Frees */
    for (int i = 0; i < n_free; i++)
        free_memory(&mm, free_pids[i]);

    if (n_free > 0 && verbose) {
        printf("  After freeing PIDs:");
        for (int i=0;i<n_free;i++) printf(" %d",free_pids[i]);
        printf(":\n"); display_memory(&mm);
    }

    /* Reallocations */
    for (int i = 0; i < n_realloc; i++) {
        int np = pid_counter++;
        MemBlock *r = allocate_memory(&mm, strategy, realloc_sizes[i], np);
        m.total_requests++;
        if (r) m.successful++; else m.failed++;
    }

    if (n_realloc > 0 && verbose) {
        printf("  After reallocations:\n"); display_memory(&mm);
    }

    clock_t t_end = clock();
    m.exec_time_ms = (double)(t_end - t_start) / CLOCKS_PER_SEC * 1000.0;
    compute_metrics(&mm, &m);

    printf("  [TC%d | %-10s] Requests:%3d | Success:%3d (%5.1f%%) | "
           "Utilisation:%5.1f%% | IntFrag:%4d KB | ExtFrag:%4d KB | "
           "Time:%.3f ms\n",
           tc, sname,
           m.total_requests, m.successful,
           m.total_requests > 0
             ? (double)m.successful/m.total_requests*100.0 : 0.0,
           m.utilisation_pct,
           m.internal_frag_kb, m.external_frag_kb,
           m.exec_time_ms);

    free_list(&mm);
    reset_quick_lists();
}

/* ─── Main ───────────────────────────────────── */
int main(void) {
    srand((unsigned)time(NULL));

    const char *names[] = {"","First Fit","Next Fit",
                           "Best Fit","Worst Fit","Quick Fit"};

    printf("=======================================================\n");
    printf("   Q2: Simulation & Analysis of Memory Allocation      \n");
    printf("=======================================================\n\n");

    /* ── Test Case 1: 5 fixed-size processes ──── */
    printf("─── Test Case 1: Allocate 5 Fixed Processes ───────────\n");
    int tc1_sizes[]   = {64, 128, 32, 256, 16};
    int tc1_no_free[] = {};
    int tc1_no_real[] = {};
    for (int s = 1; s <= 5; s++) {
        /* Verbose for first strategy only */
        if (s == 1) {
            printf("\n  [Detailed view – %s]\n", names[s]);
            MemoryManager mm; initialize_memory(&mm);
            int pids[] = {201,202,203,204,205};
            for (int i=0;i<5;i++)
                allocate_memory(&mm, s, tc1_sizes[i], pids[i]);
            display_memory(&mm);
            free_list(&mm); reset_quick_lists();
        }
        run_test_case(1, s, names[s],
                      tc1_sizes, 5,
                      tc1_no_free, 0,
                      tc1_no_real, 0, 0);
    }

    /* ── Test Case 2: Free & Reallocate ─────────*/
    printf("\n─── Test Case 2: Free Some Processes & Reallocate ─────\n");
    int tc2_sizes[]      = {64, 128, 32, 256, 16};
    int tc2_free_pids[]  = {301, 303};          /* PIDs of 64KB and 32KB */
    int tc2_realloc[]    = {80, 24};

    /* Determine actual PIDs by tracking allocation order */
    /* We'll pass pid offsets: first 5 allocs get PIDs 300..304           */
    /* Free PIDs: alloc[0]=300→64KB, alloc[2]=302→32KB                   */
    int tc2_free_actual[] = {300, 302};

    for (int s = 1; s <= 5; s++) {
        /* Reset pid so free PIDs are predictable */
        blk_counter = 1;
        MemoryManager mm; initialize_memory(&mm);
        Metrics m = {0};
        clock_t t0 = clock();

        int base_pid = 300;
        int pids[5];
        for (int i=0;i<5;i++) {
            pids[i] = base_pid + i;
            MemBlock *r = allocate_memory(&mm, s, tc2_sizes[i], pids[i]);
            m.total_requests++;
            if (r) m.successful++; else m.failed++;
        }
        free_memory(&mm, pids[0]);   /* free 64 KB block  */
        free_memory(&mm, pids[2]);   /* free 32 KB block  */

        int np1 = base_pid + 10;
        int np2 = base_pid + 11;
        MemBlock *r1 = allocate_memory(&mm, s, 80, np1);
        MemBlock *r2 = allocate_memory(&mm, s, 24, np2);
        m.total_requests += 2;
        if (r1) m.successful++; else m.failed++;
        if (r2) m.successful++; else m.failed++;

        clock_t t1 = clock();
        m.exec_time_ms = (double)(t1-t0)/CLOCKS_PER_SEC*1000.0;
        compute_metrics(&mm, &m);

        if (s == 1) {
            printf("\n  [Detailed view – %s]\n", names[s]);
            display_memory(&mm);
        }
        printf("  [TC2 | %-10s] Requests:%3d | Success:%3d (%5.1f%%) | "
               "Utilisation:%5.1f%% | IntFrag:%4d KB | ExtFrag:%4d KB | "
               "Time:%.3f ms\n",
               names[s], m.total_requests, m.successful,
               (double)m.successful/m.total_requests*100.0,
               m.utilisation_pct, m.internal_frag_kb,
               m.external_frag_kb, m.exec_time_ms);

        free_list(&mm); reset_quick_lists();
    }

    /* ── Test Case 3: Stress Test (20+ random) ── */
    printf("\n─── Test Case 3: Stress Test (%d Random Allocs) ────────\n",
           STRESS_ALLOC_COUNT);

    /* Generate deterministic random sizes (seed fixed for reproducibility) */
    srand(42);
    int stress_sizes[STRESS_ALLOC_COUNT];
    for (int i=0;i<STRESS_ALLOC_COUNT;i++)
        stress_sizes[i] = MIN_PROC_SIZE +
                          rand() % (MAX_PROC_SIZE - MIN_PROC_SIZE + 1);

    printf("  Process sizes (KB):");
    for (int i=0;i<STRESS_ALLOC_COUNT;i++) printf(" %d", stress_sizes[i]);
    printf("\n\n");

    for (int s = 1; s <= 5; s++) {
        blk_counter = 1;
        MemoryManager mm; initialize_memory(&mm);
        Metrics m = {0};
        clock_t t0 = clock();

        int pid_base = 500;
        int alive[MAX_PROCESSES];
        int n_alive = 0;

        for (int i = 0; i < STRESS_ALLOC_COUNT; i++) {
            /* Randomly free an existing process every ~4 allocs */
            if (n_alive > 3 && rand() % 4 == 0) {
                int idx = rand() % n_alive;
                free_memory(&mm, alive[idx]);
                alive[idx] = alive[--n_alive];
            }
            int npid = pid_base + i;
            MemBlock *r = allocate_memory(&mm, s, stress_sizes[i], npid);
            m.total_requests++;
            if (r) {
                m.successful++;
                if (n_alive < MAX_PROCESSES) alive[n_alive++] = npid;
            } else {
                m.failed++;
            }
        }
        clock_t t1 = clock();
        m.exec_time_ms = (double)(t1-t0)/CLOCKS_PER_SEC*1000.0;
        compute_metrics(&mm, &m);

        printf("  [TC3 | %-10s] Requests:%3d | Success:%3d (%5.1f%%) | "
               "Utilisation:%5.1f%% | IntFrag:%4d KB | ExtFrag:%4d KB | "
               "Time:%.3f ms\n",
               names[s], m.total_requests, m.successful,
               (double)m.successful/m.total_requests*100.0,
               m.utilisation_pct, m.internal_frag_kb,
               m.external_frag_kb, m.exec_time_ms);

        free_list(&mm); reset_quick_lists();
    }

    printf("\n  Done. Compile with: gcc -O2 q2_simulation.c -o q2 && ./q2\n");
    return 0;
}