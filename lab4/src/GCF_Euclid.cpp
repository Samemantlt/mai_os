extern "C" int gcf(int a, int b) {
    if (a == b)
        return a;
    if (a > b)
        return gcf(a - b, b);
    return gcf(a, b - a);
}