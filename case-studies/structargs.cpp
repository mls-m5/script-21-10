struct Apa {
    int x;
    float y;
    int z;
};

int sendStruct(Apa apa) {
    return apa.x;
}

int sendRef(Apa &apa) {
    return apa.x;
}

int sendPointer(Apa *apa) {
    return apa->x;
}

int main() {
    auto apa = Apa{};

    return sendStruct(apa) + sendRef(apa) + sendPointer(&apa);
}