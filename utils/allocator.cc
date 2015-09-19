#include <bits/stdc++.h>
template<typename T>
class AutoAlloc{
    struct AutoAllocImpl{
        enum { initial_size = 1024 };
        std::allocator<T> allocator;
        std::vector<T *> pool, freed;
        std::vector<size_t> pool_size;
        size_t i, p;
        AutoAllocImpl() : i(0), p(0) { reserve(initial_size); }
        ~AutoAllocImpl(){
            for(size_t i = 0; i < pool.size(); ++i)
                allocator.deallocate(pool[i], pool_size[i]);
        }
        inline void reserve(const size_t &n){
            pool_size.emplace_back(n);
            if(pool.empty()) pool.emplace_back(allocator.allocate(n));
            else             pool.emplace_back(allocator.allocate(n, pool.back()));
        }
        inline void clear_all(){ i = p = 0; freed.clear(); }
        inline T *allocate(){
            if(not freed.empty()){
                T * const res = freed.back(); freed.pop_back();
                return res;
            }
            if(p == pool_size[i]) if(++i == pool.size()){
                p = 0; reserve(pool_size.back() * 2);
            }
            return pool[i] + p++;
        }
        inline void deallocate(void * const ptr){
            freed.emplace_back(static_cast<T*>(ptr));
        }
    };
    static inline AutoAllocImpl &get_impl(){
        static AutoAllocImpl impl; return impl;
    }
public:
    static void *operator new(const size_t size){
        return static_cast<void *>(get_impl().allocate());
    }
    static void operator delete(void * const ptr, const size_t size){
        get_impl().deallocate(ptr);
    }
    static void clear_all_allocated(){ get_impl().clear_all(); }
};

struct Node : AutoAlloc<Node>{
    Node *l, *r;
    Node(){}
    Node(Node *l, Node *r) : l(l), r(r){}
};
#define rep(i, n) for(int i = 0; i < n; ++i)
int main(){
    Node a, b;
    Node *p = new Node;
    rep(_, 10000){
        Node *p = new Node(&a, &b);
        delete p;
    }
    return 0;
}


/*
template<size_t size, size_t align>
class Allocator_impl{
private:
    struct state_t{
        vector<void *> pool, freed, buf;
        vector<size_t> pool_size;
        size_t i, p;
        inline void alloc(const size_t &n){
            const size_t s = n * size + align;
            buf.emplace_back(::operator new(s));
            pool.emplace_back(std::align(align, size, buf.back(), s));
            pool_size.emplace_back(n);
        }
        inline void clear_all(){
            i = p = 0; freed.clear();
        }
        inline void *succ(){
            if(not freed.empty()){
                void *res = freed.back(); freed.pop_back();
                return res;
            }
            if(p == pool_size[i]) if(++i == pool.size()){
                p = 0; alloc(pool_size.back() * 2);
            }
            return pool[i] + size * p;
        }
    };
    static inline void *new_one(){
        static size_t i = 0, p = 0;
        if(not get_freed().empty()){
            void *res = get_freed().back(); get_freed.pop_back();
            return res;
        }
        if(get_pool_begin()[i] == get_pool_end()[i]) if(++i == get_buf().size()){
        }
    }
public:
    
};
template<typename T>
using Allocator<T> = Allocator_impl<sizeof(T), alignof(T)>;



template<typename T>
struct Allocator{//{{{
private:
    struct Handler{//{{{
        Handler(){ Allocator::constructor(); }
        ~Handler(){ Allocator::destructor(); }
    };//}}}

    using Ptr = T *;
    static vector<Ptr> pool;
    static vector<size_t> sizes;
    static vector<Ptr> frees;

    static inline void constructor(){ allocate(1024); }
    static inline void destructor(){ for(auto &p : pool) ::operator delete(p); }

    static inline Ptr get(const bool &clear = false){//{{{
        static Handler h;
        static size_t i = 0, p = 0;
        if(clear){ i = p = 0; frees.clear(); return nullptr; }
        if(!frees.empty()){
            Ptr res = frees.back(); frees.pop_back();
            return res;
        }
        if(p == sizes[i]) if(++i == sizes.size()){
            p = 0;
            allocate(sizes.back() * 2);
        }
        return &pool[i][p++];
    }//}}}
    static inline void reserve(const size_t &n){ allocate(n); }
    static inline void allocate(const size_t &n){//{{{
        pool.emplace_back(static_cast<Ptr>(::operator new(n * sizeof(T))));
        sizes.emplace_back(n);
    }//}}}
public:
    static void *operator new(size_t size){
        return static_cast<void *>(get());
    }
    template<typename Arg, typename ...Args>
    static void *operator new(size_t size, Arg arg, Args ...args){
        return ::operator new(size, arg, args...);
    }
    static void operator delete(void *ptr){
        frees.emplace_back(static_cast<Ptr>(ptr));
    }
    static void clear_allocated(){ get(true); }
};//}}}
template<typename T> vector<T*> Allocator<T>::pool;
template<typename T> vector<size_t> Allocator<T>::sizes;
template<typename T> vector<T*> Allocator<T>::frees;
*/


