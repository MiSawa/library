
/*
 * zip.add(hoge), zip.add(all(v)) => 座標の追加
 * zip.compile() => 圧縮実行(しないでも番号付けはする)
 * zip(元の座標...) => 圧縮後の座標
 * zip[圧縮後] => 元の座標
 */
template<class ...Ts> struct Zipper{//{{{
    using Key = typename std::conditional<std::tuple_size<std::tuple<Ts...>>::value == 1, std::tuple_element<0, std::tuple<Ts...>>, std::tuple<Ts...>>::type;

    mutable map<Key, int> zip;
    mutable vector<Key> unzip;
    template<class It> void add(It b, It e){ while(b != e) add(*b++); }
    void add(const Ts &...ts){ this->operator()(ts...); }
    void compile(){
        int i = 0;
        for(auto &x : zip) unzip[x.second = i++] = x.first;
    }
    const Key operator[](const int &i) const { return unzip[i]; }
    const int operator()(const Ts &...ts) const {
        const Key key(ts...);
        auto it = zip.find(key);
        if(it != std::end(zip)) return it->second;
        const int n = unzip.size();
        unzip.emplace_back(key);
        zip.emplace(std::move(key), n);
        return n;
    }
    const int size() const { return unzip.size(); }
    typename vector<Key>::const_iterator begin(){ return unzip.begin(); }
    typename vector<Key>::const_iterator end(){ return unzip.end(); }
};//}}}

// vim:set foldmethod=marker commentstring=//%s:
