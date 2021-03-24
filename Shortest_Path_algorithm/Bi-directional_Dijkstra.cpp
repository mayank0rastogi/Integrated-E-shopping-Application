#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include<time.h>

#define int long long int

#ifndef ONLINE_JUDGE
#define freopen freopen("input.txt","r",stdin);freopen("output1.txt","w",stdout);
#else
#define freopen //comment
#endif

struct adjacencylist
{
   int nodeindex;
   int weight;
  struct adjacencylist* adj;
};
struct minheap_f
{
    int mindist;
    int vertex;
};

struct minheap_b
{
    int mindist;
    int vertex;
};

#define INF 1e18
struct minheap_f* heap1;
struct minheap_b* heap2;

struct adjacencylist *graph[100002],*traverse;
int nodes,edges,distance,a,b,update,parent,pick,cur,ok=0,adjacent,edge,c1,c2,small,stop,source,heapsize1,heapsize2;
int index,answer_exist,prev,cnt,dist,common;
struct adjacencylist *ptr;

int visit[100002],forwardist[100002],backwardist[100002],previous[100002],after[100002];
int adjac[100002],map1[100002],map2[100002];

int swap(int *a,int *b){ int temp=*a; *a=*b; *b=temp;}
int minimum(int a, int b){ if(a<=b)return a; else return b;}

int add_edge(int a,int b,int distance)
{
    traverse=(struct adjacencylist*)malloc(sizeof(struct adjacencylist));
    traverse->adj=graph[a];
    traverse->nodeindex=b;
    traverse->weight=distance;
    graph[a]=traverse;
}
int build_minheap_b()
{
    cnt=1;
    heap2[1].mindist=backwardist[nodes];
    heap2[1].vertex=nodes;
    map2[heap2[1].vertex]=1;
    cnt++;
    for(int i=1;i<=nodes;i++)
    {
      if(i==nodes){previous[i]=1;continue;}
      else if(i!=nodes)previous[i]=-1;
      heap2[cnt].mindist=backwardist[i];
      heap2[cnt].vertex=i;
      map2[heap2[cnt].vertex]=cnt;
      cnt++;
    }
     heapsize2=cnt-1;
}
int build_minheap_f()
{
    cnt=1;
    heap1[1].mindist=forwardist[source];
    heap1[1].vertex=source;
    map1[heap1[1].vertex]=1;
    cnt++;
    for(int i=1;i<=nodes;i++)
    {
      if(i==source){previous[i]=1;continue;}
      else if(i!=source)previous[i]=-1;
      heap1[cnt].mindist=forwardist[i];
      heap1[cnt].vertex=i;
      map1[heap1[cnt].vertex]=cnt;
      cnt++;
    }
    heapsize1=cnt-1;
}
int extract_min_B()
{
      map2[heap2[heapsize2].vertex]=map2[heap2[1].vertex];
      heap2[1].mindist=heap2[heapsize2].mindist;
      heap2[1].vertex=heap2[heapsize2].vertex;
      heapsize2--;
      //Compare the first node with it's child nodes
      cur=1;
      while(1)
      {
          c1=2*cur;  //left child
          c2=(2*cur)+1; //right child
          small=LLONG_MAX;

          if(c1<=heapsize2 && heap2[c1].mindist<heap2[cur].mindist)small=c1;
          if(c2<=heapsize2 && heap2[c2].mindist<heap2[cur].mindist)
          {
              if(small!=LLONG_MAX){if(heap2[c2].mindist<heap2[small].mindist)small=c2;}
              else if(small==LLONG_MAX)small=c2;
          }
          if(small==LLONG_MAX)break;

          swap(&map2[heap2[small].vertex],&map2[heap2[cur].vertex]);
          swap(&heap2[small].mindist,&heap2[cur].mindist);
          swap(&heap2[small].vertex,&heap2[cur].vertex);
          cur=small;
      }
}
int extract_min_F()
{
      map1[heap1[heapsize1].vertex]=map1[heap1[1].vertex];
      heap1[1].mindist=heap1[heapsize1].mindist;
      heap1[1].vertex=heap1[heapsize1].vertex;
      heapsize1--;
      //Compare the first node with it's child nodes
      cur=1;
      while(1)
      {
          c1=2*cur;  //left child
          c2=(2*cur)+1; //right child
          small=LLONG_MAX;

          if(c1<=heapsize1 && heap1[c1].mindist<heap1[cur].mindist)small=c1;
          if(c2<=heapsize1 && heap1[c2].mindist<heap1[cur].mindist)
          {
              if(small!=LLONG_MAX){if(heap1[c2].mindist<heap1[small].mindist)small=c2;}
              else if(small==LLONG_MAX)small=c2;
          }
          if(small==LLONG_MAX)break;

          swap(&map1[heap1[small].vertex],&map1[heap1[cur].vertex]);
          swap(&heap1[small].mindist,&heap1[cur].mindist);
          swap(&heap1[small].vertex,&heap1[cur].vertex);
          cur=small;
      }
}
int decrease_key_B()
{
    parent=update/2;
    while(heap2[update].mindist<heap2[parent].mindist && parent>=1 && update>=1)
    {
      swap(&map2[heap2[parent].vertex],&map2[heap2[update].vertex]);
      swap(&heap2[update].mindist,&heap2[parent].mindist);
      swap(&heap2[update].vertex,&heap2[parent].vertex);

      update=parent;
      parent=update/2;
    }
}
int decrease_key_F()
{
    parent=update/2;
    while(heap1[update].mindist<heap1[parent].mindist && parent>=1 && update>=1)
    {
      swap(&map1[heap1[parent].vertex],&map1[heap1[update].vertex]);
      swap(&heap1[update].mindist,&heap1[parent].mindist);
      swap(&heap1[update].vertex,&heap1[parent].vertex);

      update=parent;
      parent=update/2;
    }
}
int bidirectional_dijkstra_minheap()
{
     forwardist[1]=0;
     backwardist[nodes]=0;


    while(1)
    {
      //---------------------------
        //   FORWARD DIJKSTRA STARTS
      // -----------------------------
      
      pick=heap1[1].vertex;
      extract_min_F();

      ptr=graph[pick];
      ok=0,adjacent,edge;
      stop=adjac[pick];
      while(stop>0 && forwardist[pick]!=LLONG_MAX)
      {
        adjacent=ptr->nodeindex;
        edge=ptr->weight;
        if(map1[adjacent]<=heapsize1 && forwardist[pick]+edge<forwardist[adjacent] && forwardist[pick]!=LLONG_MAX && edge>=0)
        {
             forwardist[adjacent]=forwardist[pick]+edge;//relaxation of nodes adjacent to picked vertex
             previous[adjacent]=pick;
             update=map1[adjacent];
             heap1[update].mindist=forwardist[pick]+edge;
             decrease_key_F();
        }
        ptr=ptr->adj;
        stop--;
      }


       if(dist>forwardist[pick]+backwardist[pick])
        {
            dist=forwardist[pick]+backwardist[pick];
            common=pick;
        }

        if(visit[pick]==1){break;}
        visit[pick]=1;

        //---------
           //FORWARD DIJKSTRA ENDS
         //  ---------
         

         // ---------------------------
           //BACKWARD DIJKSTRA STARTS
          //-----------------------------
         

      pick=heap2[1].vertex;
      extract_min_B();

      ptr=graph[pick];
      ok=0,adjacent,edge;
      stop=adjac[pick];
      while(stop>0 && backwardist[pick]!=LLONG_MAX)
      {
        adjacent=ptr->nodeindex;
        edge=ptr->weight;
        if(map2[adjacent]<=heapsize2 && backwardist[pick]+edge<backwardist[adjacent] && backwardist[pick]!=LLONG_MAX && edge>=0)
        {
             backwardist[adjacent]=backwardist[pick]+edge;//relaxation of nodes adjacent to picked vertex
             after[adjacent]=pick;
             update=map2[adjacent];
             heap2[update].mindist=backwardist[pick]+edge;
             decrease_key_B();
        }
        ptr=ptr->adj;
        stop--;
      }

       if(dist>forwardist[pick]+backwardist[pick])
        {
            dist=forwardist[pick]+backwardist[pick];
            common=pick;
        }

        if(visit[pick]==1){break;}
        visit[pick]=1;

        if(heapsize1==0 || heapsize2==0){break;}

    }
}

int path()
{
    if(dist==INF){printf("-1\n");return 0;}
   int prev=common,ind=0,flag=1,j=-1;
   int shortpath[nodes+1],fin[nodes+1];
   int siz;

   while(1)
   {
       shortpath[ind]=prev;
       if(prev==1){siz=ind;break;}
       prev=previous[prev];
       if(prev==1){shortpath[ind+1]=prev;siz=ind+1;break;}
       if(prev==-1){siz=ind;flag=0;break;}
       ind++;
   }

   if(flag==0){printf("-1\n");return 0;}

   for(int i=siz;i>=0;i--)
   {
       j++;
       fin[j]=shortpath[i];
       //cout<<"fin[j]="<<fin[j]<<endl;
       shortpath[i]=0;
   }
  //cout<<"Common="<<common<<endl;
   if(common!=nodes)
   {
       int aft=after[common];
       if(aft==-1){printf("-1\n");return 0;}
       ind=0,flag=1;

    while(1)
    {
     shortpath[ind]=aft;
     if(aft==nodes){siz=ind;break;}
     aft=after[aft];
     //cout<<"shortpath[ind]="<<shortpath[ind]<<endl;
     if(aft==nodes){shortpath[ind+1]=aft;siz=ind+1;break;}
     if(aft==-1){siz=ind;flag=0;break;}
     ind++;
    }

   if(flag==0){printf("-1\n");return 0;}

    for(int i=0;i<=siz;i++)
    {
        j++;
        fin[j]=shortpath[i];
    }
   }

   for(int i=0;i<=j;i++)
   {
       printf("%lld ",fin[i]);
   }
   printf("\n");

}
main()
{
    freopen
    
    scanf("%lld%lld",&nodes,&edges); //nodes and edges
    memset(adjac,0,sizeof(adjac));
    dist=3e18;
    heap1=(struct minheap_f*)malloc((100001)*sizeof(struct minheap_f));
    heap2=(struct minheap_b*)malloc((100001)*sizeof(struct minheap_b));

    //graph input in linked list begins
    for(int i=0;i<edges;i++)
    {
     scanf("%lld%lld%lld",&a,&b,&distance);
     adjac[a]++;
     add_edge(a,b,distance);
     add_edge(b,a,distance);
     adjac[b]++;
    }
    //Djkstra algorithm
    source=1;

       for(int i=1;i<=nodes;i++)
        {
            visit[i]=0;
            forwardist[i]=INF;
            backwardist[i]=INF;
        }
   
    time_t start,end;
    start=clock();
    //Building Minimum Heap
    build_minheap_f();
    build_minheap_b();

    //dijkstra
    bidirectional_dijkstra_minheap();
    end=clock();
    //printf("dist=%d\n",dist);
    //printing path from source to node to destination node 'n'
    printf("Shortest path from node 1 to node 100000:\n");
    path();
    printf("\nCost of the shortest path:%lld units",dist);

    double num=end-start;
    double den=CLOCKS_PER_SEC;
    double time=num/den;
    printf("\nTime=%f milliseconds",time*1000);

    
    return 0;
}