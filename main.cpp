#include "graph.h"


/* Самостоятельная FIFO-очередь на односвязном списке. */
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
/* mode: 0 — матрица/std::queue, 1 — матрица/своя очередь,
   2 — списки/std::queue. Обход всего графа, включая изолированные вершины. */
int traverse(const Graph *g,int start,int mode,int *order) {
    int *seen=(int *)allocate(g->n,sizeof(int)), count=0, components=0;
    for(int k=0;k<=g->n;++k) {
        int root=k==0 ? start : k-1;
        if(seen[root]) continue;
        ++components; std::queue<int> q; LinkedQueue own;
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
    free(seen); return components;
}


void benchmark() {
    puts("Замеры: среднее 100 обходов; p=20%; Время CPU, мс.");
    puts("n; matrix_std_ms; matrix_linked_ms; list_std_ms");
    int sizes[]={100,300,600,1000};
    long checksum=0;
    for(int i=0;i<4;++i) {
        int n=sizes[i];
        Graph g=generate(n,20,0,0);
        int *order=(int *)allocate(n,sizeof(int));
        printf("%d",n);
        for(int mode=0;mode<3;++mode) {
            double start=now_ms();
            for(int r=0;r<100;++r) {
                traverse(&g,0,mode,order); checksum+=order[n-1];
            }
            printf("; %.6f",(now_ms()-start)/100);
        }
        puts(""); free(order); graph_free(&g);
    }
    printf("Контрольная сумма: %ld\n",checksum);
}
int main(int argc,char **argv) {
    srand((unsigned)time(NULL));
    Options o=options(argc,argv);
    if(o.bench) {benchmark(); return 0;}
    Graph g=load_graph(&o,false,false);
    printf("ЛР8. Обход в ширину. n=%d, p=%d%%, start=%d\n",g.n,o.p,o.start);
    print_matrix(&g); print_lists(&g);
    int *order=(int *)allocate(g.n,sizeof(int));
    const char *names[]={"BFS матрица / std::queue:","BFS матрица / своя очередь:","BFS списки / std::queue:"};
    for(int mode=0;mode<3;++mode) {
        int components=traverse(&g,o.start-1,mode,order);
        for(int i=0;i<g.n;++i) ++order[i];
        print_vector(names[mode],order,g.n);
        printf("Компонент связности: %d\n",components);
    }
    free(order); graph_free(&g); return 0;
}
