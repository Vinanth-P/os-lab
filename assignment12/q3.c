/*
 * Q3: Comparison & Report
 *
 * Runs ALL test cases for ALL strategies, aggregates metrics, and
 * prints a formatted comparison table with analysis commentary.
 *
 * Compile: gcc -O2 q3_comparison.c -o q3 && ./q3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* ─── Configuration ──────────────────────────── */
#define MEMORY_SIZE        1024
#define QUICK_FIT_CLASSES     4
#define MAX_PROCESSES       100
#define NUM_STRATEGIES        5
#define STRESS_COUNT         25
#define REPEAT_RUNS           5   /* Average over multiple runs for timing */

int qf_sizes[QUICK_FIT_CLASSES] = {32, 64, 128, 256};

/* ─── Data Structures ────────────────────────── */
typedef struct MemBlock {
    int block_id, process_id, start, size, allocated_size;
    struct MemBlock *next, *prev;
} MemBlock;

typedef struct { MemBlock *head, *last_alloc; } MM;

typedef struct QuickNode { MemBlock *b; struct QuickNode *next; } QN;
QN *ql[QUICK_FIT_CLASSES];

typedef struct {
    double success_rate;    /* 0-100 */
    double utilisation;     /* 0-100 */
    double int_frag;        /* KB    */
    double ext_frag;        /* KB    */
    double exec_ms;         /* ms    */
    int    n_runs;
} AggMetrics;

/* ─── Core (identical to Q1/Q2 – self-contained) ─ */
static int bid = 1;

MemBlock *mkblk(int s, int sz) {
    MemBlock *b = calloc(1, sizeof(MemBlock));
    b->block_id = bid++; b->process_id = -1;
    b->start = s; b->size = sz;
    return b;
}

void reset_ql(void) {
    for (int i=0;i<QUICK_FIT_CLASSES;i++) {
        QN *q=ql[i]; while(q){QN*t=q;q=q->next;free(t);} ql[i]=NULL;
    }
}
void freelist(MM *mm) {
    MemBlock *c=mm->head;
    while(c){MemBlock*t=c;c=c->next;free(t);}
    mm->head=NULL;
}
void init_mm(MM *mm) {
    mm->head=NULL; mm->last_alloc=NULL;
    bid=1; freelist(mm); reset_ql();
    mm->head=mkblk(0,MEMORY_SIZE); mm->last_alloc=NULL;
}

MemBlock *split(MM *mm, MemBlock *b, int sz, int pid) {
    if (b->size > sz+1) {
        MemBlock *r=mkblk(b->start+sz, b->size-sz);
        r->next=b->next; r->prev=b;
        if(b->next) b->next->prev=r;
        b->next=r; b->size=sz;
    }
    b->process_id=pid; b->allocated_size=sz;
    mm->last_alloc=b; return b;
}

MemBlock *ff(MM *mm,int sz,int pid){
    for(MemBlock*c=mm->head;c;c=c->next)
        if(c->process_id==-1&&c->size>=sz) return split(mm,c,sz,pid);
    return NULL;
}
MemBlock *nf(MM *mm,int sz,int pid){
    MemBlock *s=mm->last_alloc?(mm->last_alloc->next?mm->last_alloc->next:mm->head):mm->head;
    for(MemBlock*c=s;c;c=c->next)
        if(c->process_id==-1&&c->size>=sz) return split(mm,c,sz,pid);
    for(MemBlock*c=mm->head;c&&c!=s;c=c->next)
        if(c->process_id==-1&&c->size>=sz) return split(mm,c,sz,pid);
    return NULL;
}
MemBlock *bf(MM *mm,int sz,int pid){
    MemBlock *best=NULL;
    for(MemBlock*c=mm->head;c;c=c->next)
        if(c->process_id==-1&&c->size>=sz&&(!best||c->size<best->size)) best=c;
    return best?split(mm,best,sz,pid):NULL;
}
MemBlock *wf(MM *mm,int sz,int pid){
    MemBlock *worst=NULL;
    for(MemBlock*c=mm->head;c;c=c->next)
        if(c->process_id==-1&&c->size>=sz&&(!worst||c->size>worst->size)) worst=c;
    return worst?split(mm,worst,sz,pid):NULL;
}
void qadd(MemBlock *b){
    int idx=QUICK_FIT_CLASSES-1;
    for(int i=0;i<QUICK_FIT_CLASSES;i++) if(b->size<=qf_sizes[i]){idx=i;break;}
    QN *q=malloc(sizeof(QN)); q->b=b; q->next=ql[idx]; ql[idx]=q;
}
void qrm(MemBlock *b){
    for(int i=0;i<QUICK_FIT_CLASSES;i++){
        QN **c=&ql[i];
        while(*c){if((*c)->b==b){QN*t=*c;*c=t->next;free(t);return;} c=&(*c)->next;}
    }
}
MemBlock *qf_alloc(MM *mm,int sz,int pid){
    for(int i=0;i<QUICK_FIT_CLASSES;i++){
        if(sz<=qf_sizes[i]){
            QN **c=&ql[i];
            while(*c){
                MemBlock*b=(*c)->b;
                if(b->process_id==-1&&b->size>=sz){
                    QN*t=*c;*c=t->next;free(t);
                    return split(mm,b,sz,pid);
                }
                c=&(*c)->next;
            }
        }
    }
    return ff(mm,sz,pid);
}

MemBlock *alloc(MM *mm,int strat,int sz,int pid){
    switch(strat){
        case 1: return ff(mm,sz,pid);
        case 2: return nf(mm,sz,pid);
        case 3: return bf(mm,sz,pid);
        case 4: return wf(mm,sz,pid);
        case 5: return qf_alloc(mm,sz,pid);
    }
    return NULL;
}

void coalesce(MM *mm){
    MemBlock *c=mm->head;
    while(c&&c->next){
        if(c->process_id==-1&&c->next->process_id==-1){
            MemBlock*n=c->next; c->size+=n->size;
            c->next=n->next; if(n->next)n->next->prev=c; free(n);
        } else c=c->next;
    }
}

void release(MM *mm,int pid){
    for(MemBlock*c=mm->head;c;c=c->next)
        if(c->process_id==pid){c->process_id=-1;c->allocated_size=0;qadd(c);}
    coalesce(mm);
}

void get_metrics(MM *mm, double *util, double *ifrag, double *efrag){
    int used=0,free_mem=0,ifr=0,largest=0,fblks=0;
    for(MemBlock*c=mm->head;c;c=c->next){
        if(c->process_id==-1){
            free_mem+=c->size; fblks++;
            if(c->size>largest) largest=c->size;
        } else { used+=c->allocated_size; ifr+=c->size-c->allocated_size; }
    }
    *util  = (double)used/MEMORY_SIZE*100.0;
    *ifrag = ifr;
    *efrag = (fblks>1)?(double)(free_mem-largest):0.0;
}

/* ─── Scenario Runner ────────────────────────── */

/* Returns: success_rate, util, ifrag, efrag */
void run_scenario(int strat, int scenario,
                  double *sr, double *util, double *ifrag, double *efrag) {
    MM mm; init_mm(&mm);
    int ok=0, total=0, pid=1000;
    int alive[MAX_PROCESSES]; int na=0;

    srand(42);  /* reproducible */

    if (scenario == 1) {
        /* TC1: 5 fixed processes */
        int sizes[]={64,128,32,256,16};
        for(int i=0;i<5;i++){
            MemBlock*r=alloc(&mm,strat,sizes[i],pid++);
            total++; if(r){ok++;alive[na++]=pid-1;}
        }
    } else if (scenario == 2) {
        /* TC2: Allocate 5, free 2, reallocate 2 */
        int sizes[]={64,128,32,256,16};
        int pids[5];
        for(int i=0;i<5;i++){
            pids[i]=pid++;
            MemBlock*r=alloc(&mm,strat,sizes[i],pids[i]);
            total++; if(r) ok++;
        }
        release(&mm,pids[0]); release(&mm,pids[2]);
        int realloc[]={80,24};
        for(int i=0;i<2;i++){
            MemBlock*r=alloc(&mm,strat,realloc[i],pid++);
            total++; if(r) ok++;
        }
    } else {
        /* TC3: Stress test */
        for(int i=0;i<STRESS_COUNT;i++){
            if(na>3&&rand()%4==0){
                int idx=rand()%na;
                release(&mm,alive[idx]);
                alive[idx]=alive[--na];
            }
            int sz=16+rand()%(256-16+1);
            MemBlock*r=alloc(&mm,strat,sz,pid++);
            total++; if(r){ok++;if(na<MAX_PROCESSES)alive[na++]=pid-1;}
        }
    }

    *sr    = total>0?(double)ok/total*100.0:0.0;
    get_metrics(&mm, util, ifrag, efrag);
    freelist(&mm); reset_ql();
}

/* ─── Pretty Print Helpers ───────────────────── */
void print_separator(char c, int w){
    for(int i=0;i<w;i++) putchar(c); putchar('\n');
}

void print_bar(double pct, int width) {
    int fill = (int)(pct / 100.0 * width);
    putchar('[');
    for(int i=0;i<width;i++) putchar(i<fill?'#':'-');
    putchar(']');
}

/* ─── Main ───────────────────────────────────── */
int main(void) {
    const char *names[] = {"","First Fit","Next Fit",
                           "Best Fit","Worst Fit","Quick Fit"};
    const char *tc_names[] = {"TC1 Fixed","TC2 Realloc","TC3 Stress"};

    printf("\n");
    print_separator('=', 72);
    printf("   Q3: Memory Allocation Algorithm – Comparison Report\n");
    print_separator('=', 72);

    /* ── Per-Test-Case Detailed Tables ─────────── */
    for (int tc = 1; tc <= 3; tc++) {
        printf("\n┌─────────────────────────────────────────────────────────────────┐\n");
        printf("│  Test Case %d: %-52s│\n", tc, tc_names[tc-1]);
        printf("├────────────┬──────────┬─────────────┬──────────┬──────────────┤\n");
        printf("│ %-10s │ Succ.Rate│ Utilisation │ IntFrag  │ ExtFrag      │\n",
               "Algorithm");
        printf("├────────────┼──────────┼─────────────┼──────────┼──────────────┤\n");

        for (int s = 1; s <= NUM_STRATEGIES; s++) {
            double sr, util, ifrag, efrag;
            run_scenario(s, tc, &sr, &util, &ifrag, &efrag);
            printf("│ %-10s │ %6.1f%%  │ %9.1f%%  │ %5.0f KB │ %7.0f KB   │\n",
                   names[s], sr, util, ifrag, efrag);
        }
        printf("└────────────┴──────────┴─────────────┴──────────┴──────────────┘\n");
    }

    /* ── Aggregate Score Table ──────────────────── */
    printf("\n");
    print_separator('=', 72);
    printf("  AGGREGATE PERFORMANCE SUMMARY (avg across TC1 + TC2 + TC3)\n");
    print_separator('=', 72);
    printf("  %-12s %-12s %-12s %-12s %-12s\n",
           "Algorithm", "Avg Succ%", "Avg Util%", "AvgIFrag", "AvgEFrag");
    print_separator('-', 72);

    double agg_sr[6]={}, agg_ut[6]={}, agg_if[6]={}, agg_ef[6]={};
    for (int s=1;s<=NUM_STRATEGIES;s++) {
        for (int tc=1;tc<=3;tc++) {
            double sr,ut,ifr,efr;
            run_scenario(s,tc,&sr,&ut,&ifr,&efr);
            agg_sr[s]+=sr; agg_ut[s]+=ut;
            agg_if[s]+=ifr; agg_ef[s]+=efr;
        }
        agg_sr[s]/=3; agg_ut[s]/=3;
        agg_if[s]/=3; agg_ef[s]/=3;
        printf("  %-12s %9.1f%%  %9.1f%%  %7.1f KB  %7.1f KB\n",
               names[s], agg_sr[s], agg_ut[s], agg_if[s], agg_ef[s]);
    }

    /* ── Visual Utilisation Bar Chart ───────────── */
    printf("\n");
    print_separator('=', 72);
    printf("  MEMORY UTILISATION BAR CHART (TC3 Stress)\n");
    print_separator('=', 72);
    for (int s=1;s<=NUM_STRATEGIES;s++) {
        double sr,ut,ifr,efr;
        run_scenario(s,3,&sr,&ut,&ifr,&efr);
        printf("  %-10s ", names[s]);
        print_bar(ut, 40);
        printf(" %.1f%%\n", ut);
    }

    /* ── Timing Benchmark ───────────────────────── */
    printf("\n");
    print_separator('=', 72);
    printf("  EXECUTION TIME BENCHMARK (%d runs of TC3 each)\n", REPEAT_RUNS);
    print_separator('=', 72);
    printf("  %-12s %-16s\n","Algorithm","Avg Time (ms)");
    print_separator('-', 40);
    for (int s=1;s<=NUM_STRATEGIES;s++) {
        double total_ms = 0.0;
        for (int r=0;r<REPEAT_RUNS;r++) {
            MM mm; init_mm(&mm);
            int pid=2000, ok=0, tot=0;
            int alive[MAX_PROCESSES]; int na=0;
            srand(r*7+13);
            clock_t t0 = clock();
            for(int i=0;i<STRESS_COUNT;i++){
                if(na>3&&rand()%4==0){
                    int idx=rand()%na;
                    release(&mm,alive[idx]);
                    alive[idx]=alive[--na];
                }
                int sz=16+rand()%(256-16+1);
                MemBlock *res=alloc(&mm,s,sz,pid++);
                tot++; if(res){ok++;if(na<MAX_PROCESSES)alive[na++]=pid-1;}
            }
            clock_t t1 = clock();
            total_ms += (double)(t1-t0)/CLOCKS_PER_SEC*1000.0;
            freelist(&mm); reset_ql();
        }
        printf("  %-12s %.4f ms\n", names[s], total_ms/REPEAT_RUNS);
    }

    /* ── Analysis & Recommendations ─────────────── */
    printf("\n");
    print_separator('=', 72);
    printf("  ANALYSIS & RECOMMENDATIONS\n");
    print_separator('=', 72);
    printf(
    "\n"
    "  1. FIRST FIT\n"
    "     + Fastest allocation (linear scan, stops at first fit).\n"
    "     + Simple to implement and low overhead.\n"
    "     - Creates heavy external fragmentation over time (clusters at\n"
    "       the start of memory, leaving small gaps throughout).\n"
    "     * Best scenario: Short-lived systems or when speed > fragmentation.\n"
    "\n"
    "  2. NEXT FIT\n"
    "     + Similar speed to First Fit; distributes allocations more evenly.\n"
    "     - Still produces external fragmentation, sometimes worse than FF\n"
    "       because the cursor keeps advancing, fragmenting the entire space.\n"
    "     * Best scenario: When allocation patterns are uniform in size.\n"
    "\n"
    "  3. BEST FIT\n"
    "     + Minimises wasted space inside allocated blocks (low internal\n"
    "       fragmentation).\n"
    "     - Slower (must scan all free blocks); leaves many tiny unusable\n"
    "       fragments (high external fragmentation).\n"
    "     * Best scenario: Fixed or predictable process sizes.\n"
    "\n"
    "  4. WORST FIT\n"
    "     + Leaves larger free blocks after each allocation, reducing the\n"
    "       number of unusably small fragments short-term.\n"
    "     - Quickly exhausts large contiguous free regions; poor overall\n"
    "       utilisation under stress.\n"
    "     * Best scenario: When all processes are small and uniformly sized.\n"
    "\n"
    "  5. QUICK FIT\n"
    "     + O(1) allocation for requests matching a predefined size class;\n"
    "       excellent throughput in real-time systems.\n"
    "     + Low internal fragmentation when size classes are well-tuned.\n"
    "     - Extra memory overhead for maintaining separate free lists.\n"
    "     - Falls back to First Fit for non-matching sizes.\n"
    "     * Best scenario: Systems with well-known, recurring request sizes\n"
    "       (e.g., network packet buffers, slab allocators).\n"
    "\n"
    "  OVERALL WINNER by scenario:\n"
    "  ┌──────────────────────────────────────┬───────────────┐\n"
    "  │ Scenario                             │ Best Strategy │\n"
    "  ├──────────────────────────────────────┼───────────────┤\n"
    "  │ Minimum fragmentation                │ Best Fit      │\n"
    "  │ Maximum speed                        │ Quick Fit     │\n"
    "  │ Maximum utilisation (mixed sizes)    │ First Fit     │\n"
    "  │ Long-running / many alloc-dealloc    │ Best Fit      │\n"
    "  │ Real-time / known size classes       │ Quick Fit     │\n"
    "  └──────────────────────────────────────┴───────────────┘\n"
    "\n"
    "  NOTE: Coalescing adjacent free blocks after every deallocation\n"
    "  (implemented in all strategies above) is essential to combat\n"
    "  external fragmentation regardless of the chosen strategy.\n"
    "\n"
    "  Compile: gcc -O2 q3_comparison.c -o q3 -lm && ./q3\n"
    );

    return 0;
}