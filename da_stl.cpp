#include <iostream>
#include <map>
#include <string>

using namespace std;

int main(){
    cin.tie(0);
    ios::sync_with_stdio(false);
    map<string, unsigned long long> tree;
    char command;
    string key;
    unsigned long long value;
    while (cin >> command){
        switch (command)
        {
        case '+':
            cin >> key >> value;
            if (tree.insert(make_pair(key, value)).second) cout << "OK\n";
            else cout << "Exist\n";
            break;
        case '-':{
                cin >> key;
                auto v = tree.find(key);
                if (v != tree.end()){
                    tree.erase(v);
                    cout << "OK\n";
                }else cout << "NoSuchWord\n";
            }
        default:{
                cin.unget();
                cin >> key;
                auto v = tree.find(key);
                if (v != tree.end()){
                    cout << "OK: " << (*v).second << "\n";
                }else cout << "NoSuchWord\n";
            }
            break;
        }
    }
}