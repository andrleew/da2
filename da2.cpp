#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>

using namespace std;

const size_t STRING_SIZE            = 256;

class MyRedBlackTree_Node{
public:    
    MyRedBlackTree_Node();
    MyRedBlackTree_Node(char * key, unsigned long long value);
    MyRedBlackTree_Node(char * key, unsigned long long value, MyRedBlackTree_Node * parent);

    void Save(ofstream& file);
    void Load(size_t key_length, std::ifstream& ifs);
    size_t Size();

    ~MyRedBlackTree_Node();

    char * key;
    unsigned long long value;
    MyRedBlackTree_Node * left;
    MyRedBlackTree_Node * right;
    MyRedBlackTree_Node * parent;
    bool color;
};

class MyRedBlackTree{
public:
    const static bool BLACK = true;
    const static bool RED = false;

    struct AnswerFind{
        MyRedBlackTree_Node * node;
        bool is_found;
        bool is_left_child;
    };

    MyRedBlackTree();

    struct AnswerFind Find(char * key);
    void Search(char * key);
    bool Insert(char * key, unsigned long long value);
    bool Remove(char * key);
    bool Load(char * file);
    bool Save(char * path);
    size_t Size();

    ~MyRedBlackTree();

private:
    MyRedBlackTree_Node * FindMin(MyRedBlackTree_Node * node);
    void RightRotate(MyRedBlackTree_Node * node);
    void LeftRotate(MyRedBlackTree_Node * node);
    void FixInsert(MyRedBlackTree_Node * node);
    void FixRemove(MyRedBlackTree_Node * node);

    MyRedBlackTree_Node * root;
};

MyRedBlackTree_Node::MyRedBlackTree_Node(){
    value = 0;
    key = nullptr;
    left = nullptr;
    right = nullptr;
    parent = nullptr;
    color = MyRedBlackTree::BLACK;
}

MyRedBlackTree_Node::MyRedBlackTree_Node(char * key, unsigned long long value){
    int size = strlen(key);
    this->value = value;
    this->key = new char[size + 1];
    memcpy(this->key, key, size * sizeof(char));
    left = nullptr;
    right = nullptr;
    parent = nullptr;
    color = MyRedBlackTree::BLACK;
}

MyRedBlackTree_Node::MyRedBlackTree_Node(char * key, unsigned long long value, MyRedBlackTree_Node * parent){
    int size = strlen(key);
    this->value = value;
    this->key = new char[size + 1];
    memcpy(this->key, key, size);
    this->key[size] = '\0';
    left = nullptr;
    right = nullptr;
    this->parent = parent;
    color = MyRedBlackTree::RED;
}

size_t
MyRedBlackTree_Node::Size(){
    int res = 1;
    if (left) res += left->Size();
    if (right) res += right->Size();
    return res;
}

void 
MyRedBlackTree_Node::
Save(ofstream& file){
    file << strlen(key) << "\t" << key << "\t" << value << "\t" << color << "\n";
    if (left) left->Save(file);
    else file << "0\n";
    if (right) right->Save(file);
    else file << "0\n";
}

void
MyRedBlackTree_Node::
Load(size_t key_length, std::ifstream& ifs){
    key = new char[key_length + 1];
    ifs >> key >> value >> color >> key_length;
    if (key_length){
        left = new MyRedBlackTree_Node();
        left->Load(key_length, ifs);
        left->parent = this;
    }
    ifs >> key_length;
    if (key_length){
        right = new MyRedBlackTree_Node();
        right->Load(key_length, ifs);
        right->parent = this;
    }
}

MyRedBlackTree_Node::~MyRedBlackTree_Node(){
    delete [] key;
    if (left) delete left;
    if (right) delete right;
}

// ! ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MyRedBlackTree::MyRedBlackTree(){
    root = nullptr;
}

void
MyRedBlackTree::Search(char * key){
    if (!root){
        cout << "NoSuchWord\n";
        return;
    }
    MyRedBlackTree_Node * node = root;
    int res;
    while (node){
        res = strcmp(node->key, key);
        if (res > 0){
            node = node->left;
        }else if (res < 0){
            node = node->right;
        }else{
            cout << "OK: " << node->value << "\n";
            return;
        }
    }
    cout << "NoSuchWord\n";
}

struct MyRedBlackTree::AnswerFind
MyRedBlackTree::Find(char * key){
    MyRedBlackTree_Node * node = root;
    bool is_left = false;
    if (!node) return {node, false, false};
    int res;
    for(;;){
        res = strcmp(node->key, key);
        if (res > 0){
            is_left = true;
            if (node->left) node = node->left;
            else return {node, false, true};
        }else if (res < 0){
            is_left = false;
            if (node->right) node = node->right;
            else return {node, false, false};
        }else return {node, true, is_left};
    }
}

bool 
MyRedBlackTree::Insert(char * key, unsigned long long value){
    MyRedBlackTree_Node * node = root;
    bool is_left = false;
    int res;
    for(;;){
        if (!node) break;
        res = strcmp(node->key, key);
        if (res > 0){
            is_left = true;
            if (node->left) node = node->left;
            else break;
        }else if (res < 0){
            is_left = false;
            if (node->right) node = node->right;
            else break;
        }else return false;
    }
    MyRedBlackTree_Node * new_node = new MyRedBlackTree_Node(key, value, node);
    if (node){
        if (is_left) node->left = new_node;
        else node->right = new_node;
        FixInsert(new_node);
    }else{
        new_node->color = BLACK;
        root = new_node;
    }
    return true;
}

template <class T>
void 
Swap(T& v1, T& v2){
    T v = v1;
    v1 = v2;
    v2 = v;
}

bool
MyRedBlackTree::
Remove(char * key){
    struct AnswerFind find = Find(key);
    if (!find.is_found) return false;
    if (find.node->left && find.node->right){
        MyRedBlackTree_Node* node = FindMin(find.node->right);
        Swap(find.node->key, node->key);
        Swap(find.node->value, node->value);
        find.is_left_child = (node == find.node->right 
                            ? false 
                            : true);
        find.node = node;
    }
    if (find.node->left){
        find.node->left->parent = find.node->parent;
        if (find.node->parent){
            if (find.is_left_child)
                find.node->parent->left = find.node->left;
            else
                find.node->parent->right = find.node->left;
        }else root = find.node->left;
        if (find.node->color == BLACK){
            if (find.node->left->color == RED)
                find.node->left->color = BLACK;
            else FixRemove(find.node->left);
        }
    }else if (find.node->right){
        find.node->right->parent = find.node->parent;
        if (find.node->parent){
            if (find.is_left_child) 
                find.node->parent->left = find.node->right;
            else 
                find.node->parent->right = find.node->right;            
        }else root = find.node->right;
        if (find.node->color == BLACK){
            if (find.node->right->color == RED)
                find.node->right->color = BLACK;
            else FixRemove(find.node->right);
        }
    }else{
        if (find.node->color == BLACK) FixRemove(find.node);
        if (find.node->parent){
            if (find.is_left_child)
                find.node->parent->left = nullptr;
            else
                find.node->parent->right = nullptr;            
        }else root = nullptr;
    }
    if (root) root->color = BLACK;
    return true;
}


//dfs
bool
MyRedBlackTree::
Save(char * path){
    ofstream file;
    try{
        file.open(path);
    }catch(...){
        perror("ERROR");
        return false;
    }
    if (!root){
        file << "0\n";
        return true;
    }
    root->Save(file);
    file.close();
    return true;
}

//dfs
bool
MyRedBlackTree::
Load(char * path){
    ifstream file;
    try{
        file.open(path);
    }catch(...){
        perror("ERROR");
        return false;
    }
    if (root){
        root->~MyRedBlackTree_Node();
        root = nullptr;
    }
    size_t key_length;
    file >> key_length;
    if (key_length){
        root = new MyRedBlackTree_Node();
        root->Load(key_length, file);
    }
    file.close();
    return true;
}

void 
MyRedBlackTree::
FixInsert(MyRedBlackTree_Node * node){
    MyRedBlackTree_Node *uncle, *grandparent;
    while(node->color == RED && node->parent && node->parent->color == RED){
        grandparent = node->parent->parent;
        uncle = (grandparent->left == node->parent ? grandparent->right : grandparent->left);
        if (uncle && uncle->color == RED){
            node->parent->color = BLACK;
            uncle->color = BLACK;
            grandparent->color = RED;
            node = grandparent;
        }else{
            if (node->parent == grandparent->left && node == node->parent->right){
                node = node->parent;
                LeftRotate(node);
            }else if (node->parent == grandparent->right && node == node->parent->left){
                node = node->parent;
                RightRotate(node);
            }
            node->parent->color = BLACK;
            grandparent->color = RED;
            if (node->parent == grandparent->left) RightRotate(grandparent);
            else LeftRotate(grandparent);
        }
    }
    root->color = BLACK;
}

void 
MyRedBlackTree::
FixRemove(MyRedBlackTree_Node * node){
    MyRedBlackTree_Node * brother;
    MyRedBlackTree_Node * parent;
    while (node->parent && node->color == BLACK){
        parent = node->parent;
        if (node == parent->left){
            brother = parent->right;
            if (brother->color == RED){
                brother->color = BLACK;
                parent->color = RED;
                LeftRotate(parent);
                parent = node->parent;
                brother = parent->right;
            }
            if ((!brother->left || brother->left->color == BLACK) &&
                (!brother->right || brother->right->color == BLACK)){
                    brother->color = RED;
                    node = parent;
            }else{
                if (!brother->right || brother->right->color == BLACK){
                    brother->left->color = BLACK;
                    brother->color = RED;
                    RightRotate(brother);
                    brother = parent->right;
                }
                brother->color = parent->color;
                parent->color = BLACK;
                if(brother->right) brother->right->color = BLACK;
                LeftRotate(parent);
                break;
            }
        }else{
            brother = parent->left;
            if (brother->color == RED){
                brother->color = BLACK;
                parent->color = RED;
                RightRotate(parent);
                parent = node->parent;
                brother = parent->left;
            }
            if ((!brother->left || brother->left->color == BLACK) &&
                (!brother->right || brother->right->color == BLACK)){
                    brother->color = RED;
                    node = parent;
            }else{
                if ((!brother->left || brother->left->color == BLACK) &&
                    brother->right){
                    brother->right->color = BLACK;
                    brother->color = RED;
                    LeftRotate(brother);
                    brother = parent->left;
                }
                brother->color = parent->color;
                parent->color = BLACK;
                if (brother->left) brother->left->color = BLACK;
                RightRotate(parent);
                break;
            }
        }
    }
    node->color = BLACK;
    root->color = BLACK;
}

void 
MyRedBlackTree::LeftRotate(MyRedBlackTree_Node * node){
    MyRedBlackTree_Node * right = node->right;
    MyRedBlackTree_Node * parent = node->parent;
    node->parent = right;
    node->right = right->left;
    right->left = node;
    if (node->right) node->right->parent = node;
    right->parent = parent;
    if (parent){
        if (parent->left == node) parent->left = right;
        else parent->right = right;
    }else root = right;
}

void 
MyRedBlackTree::RightRotate(MyRedBlackTree_Node * node){
    MyRedBlackTree_Node * left = node->left;
    MyRedBlackTree_Node * parent = node->parent;
    node->parent = left;
    node->left = left->right;
    left->right = node;
    if (node->left) node->left->parent = node;
    left->parent = parent;
    if (parent){
        if (parent->left == node) parent->left = left;
        else parent->right = left;
    }else root = left;
}

MyRedBlackTree_Node* 
MyRedBlackTree::FindMin(MyRedBlackTree_Node * node){
    while (node->left) node = node->left;
    return node;
}

size_t 
MyRedBlackTree::Size(){
    return (
        root
        ? root->Size()
        : 0
        );
}

MyRedBlackTree::~MyRedBlackTree(){
    if (root) delete root;
}

int main(){
    cin.tie(0);
    ios::sync_with_stdio(false);
    MyRedBlackTree tree;
    char * key = new char[STRING_SIZE + 1];
    unsigned long long value;
    char comand;
    while (cin >> comand){
        switch (comand)
        {
            case '+':
                cin >> key >> value;
                for (size_t i = 0; i < strlen(key); ++i)
                    key[i] = (key[i] < 'a' 
                                ? key[i] + 'a' - 'A' 
                                : key[i]);
                if (tree.Insert(key, value)) 
                    cout << "OK\n";
                else cout << "Exist\n";
                break;
            case '-':
                cin >> key;
                for (size_t i = 0; i < strlen(key); ++i)
                    key[i] = (key[i] < 'a' 
                            ? key[i] + 'a' - 'A' 
                            : key[i]);
                if(tree.Remove(key)) cout << "OK\n";
                else cout << "NoSuchWord\n";
                break;
            case '!':
                cin >> key;
                if (key[0] == 'S'){
                    cin >> key;
                    if (tree.Save(key)) cout << "OK\n";
                }else{
                    cin >> key;
                    if (tree.Load(key)) cout << "OK\n";
                }
                break;
            default:
                if (!isspace(comand)){
                    cin.unget();
                    cin >> key;
                    for (size_t i = 0; i < strlen(key); ++i)
                        key[i] = (key[i] < 'a' ? key[i] + 'a' - 'A' : key[i]);
                    tree.Search(key);
                }
                break;
        }
    }
    delete [] key;
    return 0;
}
