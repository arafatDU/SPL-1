#include<bits/stdc++.h>
using namespace std;

#define MAX_TREE_NODES 256


struct node{
    char c;
    int freq;
    struct node* left;
    struct node* right;
};

struct node* create_node(char data,int freq);
void swap_node(struct node** a,struct node** b);
void sort_nodes(struct node* nodes[],int n);
struct node* build_huffman_tree(char data[],int freq[],int n);
void print_codes(struct node* root,int arr[],int top );


int main(){

    char data[]={'a', 'b', 'c', 'd', 'e'};
    int freq[]={5,9,12,10,16};

    int n=sizeof(data)/sizeof(data[0]);

    struct node* root = build_huffman_tree(data, freq, n);
    int arr[MAX_TREE_NODES], top = 0;
    printf("Huffman Codes:\n");
    print_codes(root, arr, top);

    return 0;
}

struct node* create_node(char data,int freq){
    struct node* newNode=(struct node*)malloc(sizeof(struct node));
    newNode->c=data;
    newNode->freq=freq;
    newNode->left=NULL;
    newNode->right=NULL;
    return newNode;

}

void swap_node(struct node** a,struct node** b){
    struct node* temp=*a;
    *a=*b;
    *b=temp;
}
void sort_nodes(struct node* nodes[],int n){
    for(int i=0;i<n-1;i++){
        for(int j=i+1;j<n;j++){
            if(nodes[i]->freq>nodes[j]->freq){
                swap_node(&nodes[i],&nodes[j]);
            }
        }
    }
}
struct node* build_huffman_tree(char data[],int freq[],int n){
    struct node* nodes[MAX_TREE_NODES];
    for(int i=0;i<n;i++){
        nodes[i]=create_node(data[i],freq[i]);
    }

    int remaining_nodes=n;
    while(remaining_nodes>1){
        sort_nodes(nodes,remaining_nodes);

        struct node* leftNode=nodes[0];
        struct node* rightNode=nodes[1];
        struct node* parent=create_node('x',leftNode->freq+rightNode->freq);

        parent->left=leftNode;
        parent->right=rightNode;

        nodes[0] = parent;
        nodes[1]=nodes[remaining_nodes-1];
        remaining_nodes--;


    }
    

    return nodes[0];
}
void print_codes(struct node* root, int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        print_codes(root->left, arr, top + 1);
    }

    if (root->right) {
        arr[top] = 1;
        print_codes(root->right, arr, top + 1);
    }

    if (!root->left && !root->right) {
        printf("'%c': ", root->c);
        for (int i = 0; i < top; i++) {
            printf("%d", arr[i]);
        }
        printf("\n");
    }
}
