#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sched.h>
#include "th.h"

#define NR_CPU      1
#define STACK_SIZE  8192

typedef void (*ctx_func_t)(void);

static struct cpu   g_cpu[NR_CPU];
int                 g_end = 0;

static void get_wait_ts(struct cpu *cpu, struct timespec *ts)
{
    struct lh   *lh;
    struct th   *th;

    assert(!l_empty(&cpu->sleep));

    lh = l_first(&cpu->sleep);
    th = l_entry(lh, struct th, node);
    ts->tv_sec = th->sleep.tv_sec;
    ts->tv_nsec = th->sleep.tv_nsec;
}

static void wakeup_sleep_if_need(struct cpu *cpu)
{
    struct timespec ts, *s;
    struct lh       *lh;
    struct th       *th;

    if (l_empty(&cpu->sleep))
        return;

    lh = l_first(&cpu->sleep);
    th = l_entry(lh, struct th, node);
    s = &th->sleep;
    
    clock_gettime(CLOCK_REALTIME, &ts);
    if (s->tv_sec<ts.tv_sec || (s->tv_sec==ts.tv_sec && s->tv_nsec<=ts.tv_nsec)) {
        l_del(lh);
        l_add_tail(lh, &cpu->ready);
    }
}

static void sched(struct cpu *cpu)
{
    struct lh   *lh;
    struct th   *old = cpu->run;

    pthread_mutex_lock(&cpu->mutex); 
    wakeup_sleep_if_need(cpu);
    while (!g_end && l_empty(&cpu->ready)) {
        if (!l_empty(&cpu->sleep))  {
            struct timespec ts;
            get_wait_ts(cpu, &ts);
            pthread_cond_timedwait(&cpu->cond, &cpu->mutex, &ts);
        } else
            pthread_cond_wait(&cpu->cond, &cpu->mutex);
        wakeup_sleep_if_need(cpu);
    }

    lh = l_first(&cpu->ready);
    l_del(lh);
    cpu->run = l_entry(lh, struct th, node);
    pthread_mutex_unlock(&cpu->mutex); 

    if (old && old!=cpu->run) {
        swapcontext(&old->uc, &cpu->run->uc);
    } else 
        setcontext(&cpu->run->uc);
}

static void *cpu_func(void *arg)
{
    struct cpu      *cpu = (struct cpu *)arg; 
    unsigned long   nr_switch = 0;
    struct lh       *lh;
    struct th       *th;
    cpu_set_t       cpuset;
    int             ret;

    CPU_ZERO(&cpuset);
    CPU_SET(cpu->id, &cpuset);
    ret = pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    assert(ret==0);

    getcontext(&cpu->uc);
    if (nr_switch) {
        /* th exit */
        th = cpu->run;
        cpu->run = NULL;
        th->done = 1;
        pthread_cond_signal(&cpu->cond);
        /* th free in th_join() */
    }
    nr_switch ++;

    sched(cpu);

    return NULL;
}

static void init_cpu(void)
{
    int         i;
    struct cpu  *cpu;

    for (i=0; i<NR_CPU; i++) {
        cpu = &g_cpu[i];
        cpu->id = i;
        cpu->run = NULL;
        l_init(&cpu->ready);
        l_init(&cpu->sleep);
        pthread_mutex_init(&cpu->mutex, NULL);
        pthread_cond_init(&cpu->cond, NULL);
        pthread_create(&cpu->pth, NULL, cpu_func, cpu);
    }
}

struct th *th_create(int cpu_id, void (*func)(struct th *), void *arg)
{
    struct th   *th;
    char        *buf;
    struct cpu  *cpu = &g_cpu[cpu_id];

    buf = malloc(sizeof(struct th) + STACK_SIZE);
    assert(buf);

    th = (struct th *)buf;
    getcontext(&th->uc);
    th->uc.uc_stack.ss_sp = buf + STACK_SIZE;
    th->uc.uc_stack.ss_size = STACK_SIZE;
    th->uc.uc_link = &cpu->uc;
    th->cpu = cpu_id;
    th->done = 0;
    th->arg = arg;
    l_init(&th->node);
    makecontext(&th->uc, (ctx_func_t)func, 1, th);
    pthread_mutex_lock(&cpu->mutex);
    l_add_tail(&th->node, &cpu->ready);
    pthread_cond_signal(&cpu->cond);
    pthread_mutex_unlock(&cpu->mutex);

    return th;
}

static struct th *th_self(void)
{
    struct cpu  *cpu = &g_cpu[sched_getcpu()];
    return cpu->run;
}

int th_usleep(unsigned long usec)
{
    struct th       *th = th_self();
    struct timespec *ts = &th->sleep;
    struct cpu      *cpu = &g_cpu[th->cpu];

    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_nsec += usec * 1000;
    if (ts->tv_nsec >= 1000000000UL) {
        unsigned long   sec = ts->tv_nsec / 1000000000UL;
        ts->tv_sec += sec;
        ts->tv_nsec -= sec * 1000000000UL;
    }

    pthread_mutex_lock(&cpu->mutex);
    do {
        struct lh   *lh; 
        struct th   *c; 
        struct timespec *t;
        for (lh=cpu->sleep.next; lh!=&cpu->sleep; lh=lh->next) {
            c = l_entry(lh, struct th, node);
            t = &c->sleep;
            if (t->tv_sec<ts->tv_sec || (t->tv_sec==ts->tv_sec && t->tv_nsec<ts->tv_nsec))
                break;
        }
        l_add_raw(&th->node, lh->prev, lh);
    }while(0);
    pthread_mutex_unlock(&cpu->mutex);
    sched(cpu);
}


int th_yield(void)
{
    struct th   *th = th_self();
    struct cpu  *cpu = &g_cpu[th->cpu];
    struct lh   *lh;

    pthread_mutex_lock(&cpu->mutex);

    assert(th==cpu->run);
    
    if (l_empty(&cpu->ready)) {
        pthread_mutex_unlock(&cpu->mutex);
        return 0;
    }

    l_add_tail(&th->node, &cpu->ready);
    pthread_mutex_unlock(&cpu->mutex);

    sched(cpu);
    
    return 1;
}


int th_join(struct th *th)
{
    struct cpu  *cpu = &g_cpu[th->cpu];

    pthread_mutex_lock(&cpu->mutex);

    while (!th->done)
        pthread_cond_wait(&cpu->cond, &cpu->mutex);

    free(th);
    
    pthread_mutex_unlock(&cpu->mutex);
}

int th_lk_init(struct lk *lk)
{
    lk->locked = 0;
    l_init(&lk->wait);
    pthread_mutex_init(&lk->mutex, NULL);
}

int th_lk_lock(struct lk *lk)
{
    pthread_mutex_lock(&lk->mutex);
    while (lk->locked) {
        struct th   *th = th_self();

        l_add_tail(&th->node, &lk->wait);
        pthread_mutex_unlock(&lk->mutex);
        sched(&g_cpu[th->cpu]);
        pthread_mutex_lock(&lk->mutex);
    }
    lk->locked = 1;
    pthread_mutex_unlock(&lk->mutex);
}

int th_lk_unlock(struct lk *lk)
{
    pthread_mutex_lock(&lk->mutex);
    assert(lk->locked);
    if (!l_empty(&lk->wait)) {
        struct th   *th = l_entry(l_first(&lk->wait), struct th, node);
        struct cpu  *cpu = &g_cpu[th->cpu];

        l_del(&th->node);
        l_add_tail(&th->node, &cpu->ready);
        pthread_cond_signal(&cpu->cond);
    }
    pthread_mutex_unlock(&lk->mutex);
}

int th_cd_init(struct cd *cd)
{
    cd->val = 0;
    l_init(&cd->wait);
    pthread_mutex_init(&cd->mutex, NULL);
}

int th_cd_wait(struct cd *cd)
{
    pthread_mutex_lock(&cd->mutex);
    while (cd->val<=0) {
        struct th   *th = th_self();
        
        l_add_tail(&th->node, &cd->wait);
        sched(&g_cpu[th->cpu]);
    }
    cd->val --;
    pthread_mutex_unlock(&cd->mutex);
}

int th_cd_signal(struct cd *cd)
{
    pthread_mutex_lock(&cd->mutex);
    cd->val ++;
    if (!l_empty(&cd->wait)) {
        struct th   *th = l_entry(l_first(&cd->wait), struct th, node);
        struct cpu  *cpu = &g_cpu[th->cpu];

        l_del(&th->node);
        l_add_tail(&th->node, &cpu->ready);
        pthread_cond_signal(&cpu->cond);
    }
    pthread_mutex_unlock(&cd->mutex);
}

int th_cd_bcast(struct cd *cd)
{
    pthread_mutex_lock(&cd->mutex);
    cd->val = 0;
    while (!l_empty(&cd->wait)) {
        struct th   *th = l_entry(l_first(&cd->wait), struct th, node);
        struct cpu  *cpu = &g_cpu[th->cpu];

        cd->val ++;
        l_del(&th->node);
        l_add_tail(&th->node, &cpu->ready);
        pthread_cond_signal(&cpu->cond);
    }
    pthread_mutex_unlock(&cd->mutex);
}

void test_func2(struct th *th)
{
    char    *name = (char *)th->arg;
    int     i = 0;
   
    for (i=0; i<10; i++) {
        printf("name = %s, %d\n", name, i);
        th_yield();
    }
}

void test_func1(struct th *th)
{
    char    *name = (char *)th->arg;
    int     i = 0;
   
    for (i=0; i<10; i++) {
        printf("name = %s, %d\n", name, i);
        th_usleep(100000UL);
    }
    printf("end\n");
}

int main()
{
    struct th   *th_1, *th_2;

    init_cpu();

    th_1 = th_create(0, test_func1, "test 1");
    th_2 = th_create(0, test_func2, "test 2");
    th_join(th_1);
    th_join(th_2);

    printf("line: %d\n", __LINE__);
    while(1) {
        int     c;
        c = getchar();
        if (c=='q')
                break;
    }
    return 0;
}



