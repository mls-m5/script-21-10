
struct Apa {
    int x = 0;
    float y = 0;
};

struct Bepa {
    Apa apa;
    float y = 0;
};

int hello (Bepa &b) {
   return b.apa.y;
}
