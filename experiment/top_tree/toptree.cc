#include <bits/stdc++.h>
#define all(x) begin(x),end(x)
#define rall(x) (x).rbegin(),(x).rend()
#define REP(i,b,n) for(int i=(int)(b);i<(int)(n);++i)
#define rep(i,n) REP(i,0,n)
#define rrep(i,n) for(int i=(int)(n)-1;i>=0;--i)
#define repsz(i,v) rep(i,(v).size())
#define aur auto&
#define bit(n) (1LL<<(n))
#define eb emplace_back
#define mt make_tuple
#define fst first
#define snd second
using namespace std;
typedef long long ll;
//#define int long long
template<class C>int size(const C &c){ return c.size(); }
template<class T,class U>bool chmin(T&a,const U&b){if(a<=b)return false;a=b;return true;}
template<class T,class U>bool chmax(T&a,const U&b){if(a>=b)return false;a=b;return true;}


namespace LinkCutTree{//{{{
    const int INF = 1<<10;
    typedef int Cap;
    struct Node{
        typedef Node* Ptr;
        Ptr ch[2]; // {left, right}. left : ancestor
        Ptr par;
        //Node() : par(0){ ch[0] = ch[1] = 0; }
        Node(Cap v) : par(0), size(1){ ch[0] = ch[1] = 0; init(v); }
        int name; // 頂点名
        // path に対する操作で実装する所. //{{{
        // 注: splay の root は常に push 済みであるように.

        // タイは祖先(ch[0]) 優先.
        Ptr min_node;
        // val, min_val にはまだ lazy が適用されていない.
        Cap val, min_val, lazy;
        bool reversed;

        inline void init(Cap v){ reversed = false; min_node = this; val = min_val = v; lazy = 0;}

        // 遅延値を下に送る
        inline void push(){//{{{
            val += lazy; min_val += lazy;
            rep(i, 2) if(ch[i]) ch[i]->lazy += lazy;
            lazy = 0;
            if(reversed){
                swap(ch[0], ch[1]);
                reversed = false;
                rep(i, 2) if(ch[i]) ch[i]->reversed ^= true;
            }
        }//}}}
        // ch と辻褄をあわせる
        inline void update(){//{{{
            push();
            if(ch[0]){
                min_val = ch[0]->min_val + ch[0]->lazy;
                min_node = ch[0]->min_node;
            }
            if(!ch[0] || min_val > val){
                min_val = val; min_node = this;
            }
            if(ch[1] && min_val > ch[1]->min_val + ch[1]->lazy){
                min_val = ch[1]->min_val + ch[1]->lazy;
                min_node = ch[1]->min_node;
            }
        }//}}}

        inline void set_value(Cap v = -INF){ expose(); val = v; update(); }
        inline Cap get_value(){ expose(); return val; }
        inline Cap get_min_value(){ expose(); return min_val; }
        inline Ptr get_min_node_on_path(){ expose(); return min_node; }
        inline void add_to_path(Cap c){//{{{
            expose();
            if(ch[1]){
                Ptr p = ch[1];
                remove_ch(p, 1);
                add_ch(p, 0);
            }
            lazy += c;
            push();
        }//}}}
        //}}}
        // 部分木に対する操作で実装する所.//{{{
        int size;
        inline int get_size(){ expose(); return size - (ch[0] ? ch[0]->size : 0); }
        inline void remove_ch(Ptr p, int lr){//{{{
            push();
            if(lr) ch[lr>0] = 0;
            if(p){
                size -= p->size;
                p->par = 0;
                p->push();
            }
            update();
        }//}}}
        inline void add_ch(Ptr p, int lr){//{{{
            push();
            if(p && p->par) p->par->remove_ch(p, p->state());
            if(p){
                size += p->size;
            }
            if(lr) ch[lr>0] = p;
            if(p) p->par = this;
            update();
        }//}}}
        //}}}
        // splay 木に対する操作. 外から弄らない. //{{{
        inline int state(){//{{{
            if(par && par->ch[0] == this) return -1;
            if(par && par->ch[1] == this) return +1;
            return 0;
        }//}}}
        inline void rotate(){//{{{
            Ptr p = par, pp = p->par;
            if(pp) pp->push(); p->push(); push();
            int lr = state(), plr = p->state();
            Ptr c = ch[lr<0];
            if(pp) pp->remove_ch(p, plr); //
            p->remove_ch(this, lr);
            remove_ch(c, -lr); //
            p->add_ch(c, lr);
            add_ch(p, -lr);
            if(pp) pp->add_ch(this, plr);
        }//}}}
        inline void splay(){//{{{
            while(state()){
                int s = state() * par->state();
                if(s) (s == 1 ? par : this)->rotate();
                rotate();
            }
            push();
        }//}}}
        //}}}
        // path 分解に対する操作. 外から呼んでよい. //{{{
        // コイツを仮想木の親までもって行く.
        // 右の子は切る. i.e. この仮想木はコイツから根までの path.
        inline Ptr expose(){//{{{
            Ptr res = 0;
            for(Ptr p = this; p; p = p->par){
                p->splay();
                p->add_ch(res, 1);
                res = p;
            }
            splay();
            add_ch(ch[1], 0);
            return res;
        }//}}}
        // コイツが根になるよう, (元の木を) reroot する.
        inline void evert(){//{{{
            expose();
            reversed ^= true;
        }//}}}
        // コイツを根とする部分木を切り出す.
        inline void cut(){//{{{
            expose();
            if(ch[0]) remove_ch(ch[0], -1);
        }//}}}
        // p の下にコイツを含む木をくっつける.
        inline void link_to(Ptr p){//{{{
            p->expose();
            expose();
            p->add_ch(this, 0);
        }//}}}
        // コイツを含む木の根を探す.
        inline Ptr find_root(){//{{{
            expose();
            Ptr p = this;
            while(p->ch[0]) p = p->ch[0];
            p->expose();
            return p;
        }//}}}
        // あればコイツと u の LCA. 無ければ 0.
        inline Ptr lca(Ptr u){//{{{
            expose();
            Ptr res = u->expose();
            return par ? res : 0;
        }//}}}
        //}}}
    };
};//}}}

// 部分木のデータは
// tree = vertex + ((edge + tree) + (edge + tree) + ...)
//     or vertex
// path のデータは
// path = path + vertex + path
//     or edge

// 必要な型は,
// tree = vertex, path = edge と思えば, 必要な型は
// vertex, open tree (i.e. edge + tree), edge,
// lazy_path, lazy_tree (vertex の更新も edge の更新もまとめてやって)

// 必要な関数は,
//   open_tree (edge, tree)
//   open_tree (open_tree, open_tree)
//   tree (vertex, open_tree)
//   path (path, vertex, path)
//   closed_path (vertex, path, vertex)
//
//   void (path &, lazy_path)
//   void (tree &, lazy_tree)
//   void (open_tree &, lazy_tree)
//   void (lazy_path &, lazy_path)
//   void (lazy_tree &, lazy_tree)


struct Handler{//{{{
    using Path     = int;
    using Tree     = int;
    using OpenTree = int;
    using LazyPath = int;
    using LazyTree = int;

    static void apply_path(Path &a, const LazyPath &b){
        a += b;
    }
    static void push_path(LazyPath &a, const LazyPath &b){
        a += b;
    }
    static Path merge_path(const Path &l, const Path &r){
        return l + r;
    }
    static void apply_tree(Tree &a, const LazyTree &b){
        a += b;
    }
    static void apply_open_tree(OpenTree &a, const LazyTree &b){
        a += b;
    }
    static void push_tree(LazyTree &a, const LazyTree &b){
        a += b;
    }
    static Tree merge_tree(const Tree &l, const Tree &r){
        return l + r;
    }
};//}}}

// Node に端点を explicit に持ったほうが良い.

struct TopTree{
    struct NodeData : public Handler{//{{{
        Path path;
        union {
            Tree tree;
            OpenTree open_tree;
        } tree;
        LazyPath lazy_path;
        LazyTree lazy_tree;
    };//}}}

    struct Node{//{{{
        using Ref = Node *;
        Ref par, _ch[3];
        int id;       // rake node の時は -1
        int src, dst; // compress node の時は端点(dst が root 側)
                      // rake node の時は (下の頂点, 上の頂点)
        NodeData data[2]; // data と reversed data
        bool has_lazy_path, has_lazy_tree, reversed; // rake node は常に not reversed.

        Node() : par(nullptr){ fill(begin(_ch), end(_ch), nullptr); }
        Ref &ch(int idx){ return _ch[idx + 1]; }
        bool is_rake() const { return id == -1; }
        int state() const {//{{{
            if(par && par->ch(-1) == this) return -1;
            if(par && par->ch(+1) == this) return +1;
            return 0;
        }//}}}
        void rotate(){//{{{
            Ref p = par, pp = p->par;
            int lr = state(), plr = p->state();
            Ref c = ch(lr);

            p->ch(lr) = c; c->par = p;
            ch(-lr) = p;   p->par = this;
            if(pp) pp->ch(plr) = this; par = pp;
        }//}}}
        void splay(){//{{{
            while(int s = state()){
                s *= par->state();
                if(s) (s == 1 ? par : this)->rotate();
                rotate();
            }
        }//}}}

        // splay とかする前にやっとく
        // TODO: lazy を rake に送る時は data[i] どうするんだろう.
        void push_lazy(){//{{{
            if(!reversed and !has_lazy_path and !has_lazy_tree) return;
            if(has_lazy_path){
                for(int s = -1; s <= +1; s += 2)
                    if(const Ref &c = ch(s)) for(int r = 0; r < 2; ++r)
                        Handler::push_path(c->data[r].lazy_path,
                                data[r ^ (reversed == c->reversed)].lazy_path);
                for(int r = 0; r < 2; ++r)
                    Handler::apply_path(data[r].path, data[r].lazy_path);
            }
            if(has_lazy_tree){
                for(int s = -1; s <= +1; ++s)
                    if(const Ref &c = ch(s)) for(int r = 0; r < 2; ++r)
                        Handler::push_tree(c->data[r].lazy_tree,
                                data[r ^ (reversed == c->reversed)].lazy_tree);
                if(is_rake()) for(int r = 0; r < 2; ++r)
                    Handler::apply_open_tree(data[r].tree.open_tree, data[r].lazy_tree);
                else for(int r = 0; r < 2; ++r)
                    Handler::apply_tree(data[r].tree.tree, data[r].lazy_tree);
            }
            if(reversed){
                swap(src, dst);
                swap(data[0], data[1]);
                swap(ch(-1), ch(+1));
                ch(-1)->reversed ^= true;
                ch(+1)->reversed ^= true;
            }
            has_lazy_path = has_lazy_tree = reversed = false;
        }//}}}

        // つなぎ変えた後に呼ぶ.
        void pull_data(){//{{{
            assert(!has_lazy_tree and !has_lazy_path and !reversed);
            // push_lazy();
            for(int s = -1; s <= 1; ++s) if(Ref c = ch(s)) c->push_lazy();
            if(!is_rake()){
                // TODO src と dst の修正
            }
            if(is_rake()){
                data[0].tree.open_tree = NodeData::make_open_tree(
                        0 /* TODO: E(src, dst) */,
                        ch(0)->data[ch(0)->reversed].tree.tree);
                if(ch(-1))
                    data[0].tree.open_tree = NodeData::merge_open_tree(
                            data[0].tree.open_tree,
                            ch(-1)->data[r].tree.open_tree);
                if(ch(+1))
                    data[0].tree.open_tree = NodeData::merge_open_tree(
                            data[0].tree.open_tree,
                            ch(+1)->data[r].tree.open_tree);
            }else{
                for(int r = 0; r < 2; ++r){
                    bool first = true;
                    for(int c = -1; c <= 1; ++c){
                        if(first){
                            data[r].tree.open_tree = ch(c)->data[0]
                        }
                    }
                    data[r].tree.tree = NodeData::make_open_tree(
                            0 /* TODO */ ,
                            ch(0)->data[ch(0)->reversed].tree.open_tree);
                    if(ch(-1))
                        data[r].tree.open_tree = NodeData::merge_open_tree(
                                data[r].tree.open_tree,
                                ch(-1)->data[r].tree.open_tree);
                    if(ch(+1))
                        data[r].tree.open_tree = NodeData::merge_open_tree(
                                data[r].tree.open_tree,
                                ch(+1)->data[r].tree.open_tree);
                }
            }
            // TODO
            //   open_tree (edge, tree)
            //   open_tree (open_tree, open_tree)
            //   tree (vertex, open_tree)
            //   path (path, vertex, path)
            //   closed_path (vertex, path, vertex)

            //   Path path;
            //   union {
            //       Tree tree;
            //       OpenTree open_tree;
            //   } tree;
        }//}}}

        static Ref join(Ref u, Ref v){//{{{
            if(u == nullptr or v == nullptr) return u == nullptr ? v : u;
            u->push_lazy();
            for(u->push_lazy(); u->ch(1); u->push_lazy()) u = u->ch(1);
            u->splay();
            u->ch(1) = v;
            v->par = u;
            return u;
        }//}}}
    };//}}}
    using Ref = Node::Ref;

    vector<Ref> handle;
    TopTree(const int n) : handle(n, nullptr) {}

    void gc(Ref a){
        // ガベコレ
    }
    Ref expose(int v){//{{{
        for(Ref a = handle[v]; ; a = a->par){
            a->splay();
            Ref p = a->par;
            if(!p) break;
            p->splay();
            Ref pp = p->par;

            if(pp->ch(1)){
                p->ch(0) = pp->ch(1);
                if(p->ch(0)) p->ch(0)->par = p;
            }else{
                pp->ch(0) = Node::join(p->ch(-1), p->ch(+1));
                if(pp->ch(0)) pp->ch(0)->par = pp;
                gc(p);
            }
            pp->ch(1) = a;
            a->par = pp;
        }
        handle[v]->splay();
        if(handle[v]->ch(1)){
            // handle[v]->ch(1) を ch(0) に持つノードを新たに作る.
            // handle[v]->ch(0) に insert する.
        }
        return handle[v];
    }//}}}
    void evert(int v){//{{{
        expose(v);
        handle[v]->reversed ^= true;
    }//}}}

    void cut(int v){
        Ref a = expose(v);
        if(a->ch(-1)){
            a->ch(-1)->par = nullptr;
            a->ch(-1) = nullptr;
        }
    }
    Ref link(int u, int v){
        return nullptr;
    }
};




bool solve(){
    
    return true;
}
signed main(){
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);
    cout << std::fixed << std::setprecision(10);
    solve();
    return 0;
}
// vim:set foldmethod=marker commentstring=//%s:
