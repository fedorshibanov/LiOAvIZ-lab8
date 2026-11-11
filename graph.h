#ifndef GRAPH_H
#define GRAPH_H
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <ctime>
#include <queue>

struct Node { int vertex; Node *next; };
struct Graph { int n; int *a; Node **adj; };
void *allocate(size_t n, size_t size) {
    void *p = calloc(n, size);
    if (!p) { fputs("Недостаточно памяти\n", stderr); exit(1); }
    return p;
}
Graph graph_new(int n) {
    Graph g = {n, (int *)allocate((size_t)n*n,sizeof(int)),
               (Node **)allocate(n,sizeof(Node *))};
    return g;
}
void graph_lists(Graph *g) {
    for(int u=0;u<g->n;++u) {
        for(int v=g->n-1;v>=0;--v) if(g->a[u*g->n+v]) {
            Node *p=(Node *)allocate(1,sizeof(Node));
            p->vertex=v;
            p->next=g->adj[u];
            g->adj[u]=p;
        }
    }
}
void graph_free(Graph *g) {
    for(int i=0;i<g->n;++i) {
        Node *p=g->adj[i];
        while(p) { Node *next=p->next; free(p); p=next; }
    }
    free(g->adj); free(g->a);
}
Graph generate(int n,int probability) {
    Graph g=graph_new(n);
    for(int u=0;u<n;++u) for(int v=u+1;v<n;++v) {
        if(rand()%100<probability)
            g.a[u*n+v]=g.a[v*n+u]=1;
    }
    graph_lists(&g);
    return g;
}
void print_matrix(const Graph *g) {
    puts("Матрица смежности:"); printf("    ");
    for(int i=0;i<g->n;++i) printf("%4d",i+1);
    puts("");
    for(int u=0;u<g->n;++u) {
        printf("%3d ",u+1);
        for(int v=0;v<g->n;++v) printf("%4d",g->a[u*g->n+v]);
        puts("");
    }
}
void print_lists(const Graph *g) {
    puts("Списки смежности:");
    for(int u=0;u<g->n;++u) {
        printf("%d:",u+1);
        for(Node *p=g->adj[u];p;p=p->next) printf(" %d",p->vertex+1);
        puts(" -> NULL");
    }
}
void print_vector(const char *label,const int *d,int n) {
    printf("%s",label);
    for(int i=0;i<n;++i) printf(" %d",d[i]);
    puts("");
}
long number(const char *s,long low,long high) {
    char *end=nullptr; errno=0; long x=strtol(s,&end,10);
    if(errno || !*s || *end || x<low || x>high) {
        fprintf(stderr,"Недопустимое число: %s (ожидается %ld..%ld)\n",s,low,high); exit(1);
    }
    return x;
}
double now_ms() {
    return 1000.0 * clock() / CLOCKS_PER_SEC;
}
#endif
