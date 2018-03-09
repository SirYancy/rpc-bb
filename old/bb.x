program BULLETIN_PROG {
    version BULLETIN_VERSION {
        bool Post(string user, string Article) = 1;
        string Read() = 2;
        string Choose(int id) = 3;
        bool Reply(int id, string user, string Article) = 4;
    } = 1;
} = 0x13131345;
