class Noncopy {
private:
    Noncopy(const Noncopy&)            = delete;
    Noncopy& operator=(const Noncopy&) = delete;
};

class Nomove {
private:
    Nomove(Nomove&&)            = delete;
    Nomove& operator=(Nomove&&) = delete;
};

enum Uninitializer {
    UNINIT
};

class Noinit {
public:
    Noinit(Uninitializer) {}
};
