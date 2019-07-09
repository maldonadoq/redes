#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include "inc/utils.h"

using namespace std;

int main(int argc, char const *argv[]){
    srand(time(NULL));

    int n = 5;
    int r = 10;

    if(argc == 3){
        n = stoi(argv[1]);
        r = stoi(argv[2]);
    }

    int i = 0;
    vector<string> nodes;
    vector<pair<string, string> > relations;

    string node, tnode;

    ofstream file("db/init.txt");

    while(i < n){
        node = generate(3);
        if(!nfind(nodes, node)){
            nodes.push_back(node);
            file << "I N " << node << " " << rand()%100 << "\n";
            i++;
        }
    }

    i = 0;
    while(i < r){
        node = nodes[rand()%(int)nodes.size()];
        tnode = nodes[rand()%(int)nodes.size()];

        if(node != tnode){
            if(!rfind(relations, node, tnode)){
                relations.push_back({node, tnode});
                if(i == r-1){
                    file << "I R " << node << " " << tnode;
                }
                else{
                    file << "I R " << node << " " << tnode << "\n";
                }
                i++;
            }
        }
    }


    cout << sizeof(uint) << "\n";
    // print_vect(nodes);
    // print_vect(relations);
    return 0;
}
