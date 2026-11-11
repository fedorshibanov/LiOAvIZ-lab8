#include "graph.h"


struct LinkedQueue {
    Node *head=nullptr, *tail=nullptr;
    void push(int v) {
        Node *p=(Node *)allocate(1,sizeof(Node)); p->vertex=v;
        if(tail) tail->next=p; else head=p;
        tail=p;
    }
    int pop() {
        Node *p=head; int v=p->vertex;
        head=p->next; if(!head) tail=nullptr; free(p); return v;
    }
    bool empty() const { return head==nullptr; }
};
void traverse(const Graph *g,int start,int mode,int *order) {
    int *seen=(int *)allocate(g->n,sizeof(int)), count=0;
    for(int k=0;k<=g->n;++k) {
        int root=k==0 ? start : k-1;
        if(seen[root]) continue;
        std::queue<int> q; LinkedQueue own;
        seen[root]=1; if(mode==1) own.push(root); else q.push(root);
        while(mode==1 ? !own.empty() : !q.empty()) {
            int u;
            if(mode==1) u=own.pop(); else {u=q.front(); q.pop();}
            order[count++]=u;
            if(mode==2) {
                for(Node *p=g->adj[u];p;p=p->next) if(!seen[p->vertex]) {
                    seen[p->vertex]=1; q.push(p->vertex);
                }
            } else for(int v=0;v<g->n;++v) if(g->a[u*g->n+v] && !seen[v]) {
                seen[v]=1; if(mode==1) own.push(v); else q.push(v);
            }
        }
    }
    free(seen);
}


void benchmark(const Graph *g,int *order) {
    puts("Среднее время CPU за 100 обходов, мс");
    puts("n; matrix_std_ms; matrix_linked_ms");
    printf("%d",g->n);
    for(int mode=0;mode<2;++mode) {
        double start=now_ms();
        for(int r=0;r<100;++r) traverse(g,0,mode,order);
        printf("; %.6f",(now_ms()-start)/100);
    }
    puts("");
}
int main() {
    srand((unsigned)time(NULL));
    char size[32], probability[32];
    printf("Введите число вершин (1..2000): ");
    fflush(stdout);
    if(scanf("%31s",size)!=1) {
        fputs("Не удалось прочитать число вершин\n",stderr);
        return 1;
    }
    int n=(int)number(size,1,2000);

    printf("Введите вероятность ребра в процентах (0..100): ");
    fflush(stdout);
    if(scanf("%31s",probability)!=1) {
        fputs("Не удалось прочитать вероятность ребра\n",stderr);
        return 1;
    }
    int p=(int)number(probability,0,100);
    Graph g=generate(n,p);
    printf("ЛР8. Обход в ширину. n=%d, p=%d%%, start=%d\n",g.n,p,1);
    print_matrix(&g);
    print_lists(&g);
    int *order=(int *)allocate(g.n,sizeof(int));
    const char *names[]={"BFS матрица / std::queue:","BFS матрица / своя очередь:","BFS списки / std::queue:"};
    for(int mode=0;mode<3;++mode) {
        traverse(&g,0,mode,order);
        for(int i=0;i<g.n;++i) ++order[i];
        print_vector(names[mode],order,g.n);
    }
    benchmark(&g,order);
    free(order);
    graph_free(&g);
    return 0;
}
