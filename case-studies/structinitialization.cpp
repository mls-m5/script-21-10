
struct Apa {
    int x = 0;
    int y = 0;
};

int main() {
    auto apa = Apa{10, 20};

    // apa = {30, 40};

    auto bepa = apa;

    // return apa.x + apa.y;
}