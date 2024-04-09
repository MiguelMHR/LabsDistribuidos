struct message {
    int key;
    char value1[256];
    int n_value2;
    double v_value2[32];
};

program TUPLAS {
    version TUPLASVER {
        int init() = 1;
        int set_value(message msg) = 2;
        message get_value(int key) = 3;
        int modify_value(message msg) = 4;
        int delete_key(int key) = 5;
        int exist(int key) = 6;
    } = 1;
} = 51258;