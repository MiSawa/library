
void reader(char &x){ scanf("%c", &x); }
void reader(int &x){ scanf("%d", &x); }
void reader(long long &x){ scanf("%" SCNd64, &x); }
void reader(double &x){ scanf("%lf", &x); }
template<class T, class S, class ...U> void reader(T &x, S &y, U &...u){ reader(x); reader(y, u...); }


