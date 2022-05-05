#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <time.h>
#include <sys/time.h>

using namespace std;

struct EDGE{
    int a;
    int b;
};

struct VERT{
    int vert_id;
    int n_edge;
    int * edge_ls;
};

struct EL{
    int score;
    int* bitstring;
};

int ctoi(char c){
    int n = c - '0';
    return n;
}

float rand0to1(){
    float r = ((float) rand() / (RAND_MAX));
    return r;
}

int rand0or1(){
    int r = rand()%2;
    return r;
}

int compare_ls(int *a,int *b, int sz){
    int same = 1;
    for (int i = 0; i<sz; i++){
        if(a[i] != b[i]){
            same = 0;
            break;
        }
    }
    return same;
}

void mutation(struct EL *el, float r_mutation, int n_bits){
    for (int i = 0 ;i < n_bits; i++){
        if (rand0to1() < r_mutation)
            el->bitstring[i] = 1 - el->bitstring[i];
    }
}

void initialization(struct EL *elem, int n_bits){
    int *ptr = (int*)malloc(n_bits * sizeof(int));
    for (int i = 0; i < n_bits; i++){
        ptr[i] = rand0or1();
    }
    elem->bitstring = ptr;
    elem->score = 0;
}


int evaluate(int*x, int*y, int n){
    int sz = n*8;
    int sc;
    int t[sz];
    int temp[8];
    int s[8] = {1,1,1,1,1,1,1,1};
    for(int i = 0; i<sz; i++){
        t[i] = (x[i] == y[i]);
    }
    int score = 0;
    for(int i = 0; i<n; i++){
        if (i%2 == 0)
            sc = 5;
        else
            sc = 8;
        for (int j = 0; j<8; j++) 
            temp[j] = t[i*8 + j];
        if(compare_ls(temp, s, 8)){
            score = score + sc;
        }
    }
   return score;
}

int select(struct EL * pop, int n_pop, int k){
    int sel_idx = rand()%n_pop;
    int candidate[k-1];
    for (int i = 0; i < k-1; i++){
        int idx = rand()%n_pop;
        if(pop[idx].score > pop[sel_idx].score){
            sel_idx = idx;
        }
    }
    return sel_idx;
}

void crossover(struct EL * p1, struct EL * p2, struct EL * result, float r_cross, int n_bits){
    int *c1_ptr = (int*)malloc(n_bits * sizeof(int));
    int *c2_ptr = (int*)malloc(n_bits * sizeof(int));
    
    for(int i = 0; i < n_bits; i++){
        c1_ptr[i] = p1->bitstring[i];
        c2_ptr[i] = p2->bitstring[i];
    }

    if (rand0to1() < r_cross){
        int pt = (rand()%(n_bits-1))+1;
        for(int i = 0; i< pt; i++){
            c1_ptr[i] = p2->bitstring[i];
            c2_ptr[i] = p1->bitstring[i];
        }
    }
    result[0].bitstring = c1_ptr;
    result[1].bitstring = c2_ptr;
}

void evaluate_el(struct EL * el, int*y, int n){
    el->score = evaluate(el->bitstring, y, n);
}

void evaluate_all(struct EL * pop, int*y, int n, int n_pop){
    for(int i = 0; i <n_pop; i++){
        evaluate_el(&pop[i], y, n);
    }
}

float get_highest_idx(struct EL * pop, int n_pop){
    float highest_score = 0;
    int idx = 0;
    for(int i = 0; i <n_pop; i++){
        float tmp_score = pop[i].score;
        if (tmp_score > highest_score){
            highest_score = tmp_score;
            idx = i;
        }
    }
    return idx;
}
float get_average_score(struct EL * pop, int n_pop){
    float total_score = 0;
    for(int i = 0; i <n_pop; i++){
        total_score += pop[i].score;
    }
    return float(total_score/n_pop);
}

struct EL genetic_algorithm(int n_bits, int n_iter, int n_population, float r_cross, float r_mutation, int n, int y[], int k){
    time_t start, end;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    start = tv.tv_sec;

    struct EL * pop;
    struct EL init [n_population];
    pop = init;
    //cout<<"n_pop: "<<n_population<<endl;
    //Initialize population
    for (int i = 0; i<n_population; i++){
        initialization(&pop[i],n_bits);
    }

    for(int gen = 0; gen<n_iter; gen++){
        gettimeofday(&tv, NULL);
        end = tv.tv_sec;
        if(end - start >= 29){
            //cout<<"TIMEOVER"<<endl;
            break;
        }
        evaluate_all(pop, y, n, n_population);
        //if(gen%10==0)cout<<"gen: "<<gen<<" score: "<<get_highest_score(pop,n_population)<<endl;
        
        //select parents
        int parent_idx[n_population];
        struct EL children[n_population];
        for(int i = 0; i < n_population; i++){
            parent_idx[i] = select(pop,n_population,k);
        }
        //do crossover and mutation
        for(int i = 0; i < n_population; i=i+2){
            struct EL p1 = pop[parent_idx[i]];
            struct EL p2 = pop[parent_idx[i+1]];
            struct EL two_childrens[2];
            crossover(&p1, &p2, two_childrens, r_cross, n_bits);
            for(int j = 0; j < 2; j++){
                mutation(&two_childrens[j],r_mutation,n_bits);
                children[i+j] = two_childrens[j];
            }
        }
        for(int i =0; i<n_population; i++){
            free(pop[i].bitstring);
        }
        pop = children;
    }
    evaluate_all(pop, y, n, n_population);
    int highest_idx = get_highest_idx(pop, n_population);
    struct EL * highest_elem = &pop[highest_idx];
    cout<<highest_elem->score<<" ";
    for(int i =0; i<n_bits; i++){
        cout<<highest_elem->bitstring[i];
    }
}

int main(int argc, char**argv) {
    srand(time(NULL));

    //input n, y
    string n_in, y_in;

    cin >> n_in;
    cin >> y_in;
    //
    int n;
    stringstream ssInt(n_in);
    ssInt >> n;

    int n_bits = n*8;
    int n_iter = 99999;
    int n_population = n_bits * 10;
    float r_cross = 0.8;
    float r_mutation = 1 / float(n_bits);
    int y[n_bits];
    int k = 4;

    for (int i =0; i<n_bits; i++){
        y[i] = ctoi(y_in[i]);
    }

    //for (int i =0; i<n_bits; i++){
    //    cout<<y[i];
    //}
    //cout<<endl;
    //n = 1;

    genetic_algorithm(n_bits,n_iter,n_population,r_cross,r_mutation,n,y,k); 

}
