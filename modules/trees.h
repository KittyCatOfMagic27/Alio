#ifndef TREES_H
#define TREES_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cassert>

using namespace std;

namespace kt{
  template<class T>
  class Tree{
    public:
      
      vector<T> storage;
      unordered_map<T*, vector<T*>> connections;
      
      Tree(vector<T> _storage){
        storage = _storage;
      }
      
      Tree(){}
      
      void push_back(T x){
        storage.push_back(x);
      }
      void pop_back(){
        T* x = &storage[storage.size()-1];
        connections.erase(x);
        storage.pop_back();
      }
      void connect(T* x, T* y){
        connections[x].push_back(y);
      }
      vector<T> get_children(T* x){
        auto iter = find(storage.begin(), storage.end(), *x);
        assert(iter!=storage.end());
        vector<T*> children = connections[x];
        vector<T> result;
        for(T* &i : children) result.push_back(*i);
        return result;
      }
      T* operator[](int index) {
        return &storage[index];
      }
      vector<T*>& operator[](T* item) {
        return connections[item];
      }
  };
}
#endif