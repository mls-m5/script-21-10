
struct str {
    char *data;
    unsigned long len;
};

unsigned long f(const str *s) {
    return s->len;
}

int main() {
    auto c = "hello";
    auto s = str{(char *)c, sizeof(c)};
    return f(&s);
}
