struct message {
    int key_or_return;
    char value1[256];
    int n_value2;
    double v_value2[32];
};

program TUPLAS {
    version TUPLASVER {
        int init_tuplas() = 1;
        int set_value_tuplas(message msg) = 2;
        message get_value_tuplas(int key) = 3;
        int modify_value_tuplas(message msg) = 4;
        int delete_key_tuplas(int key) = 5;
        int exist_tuplas(int key) = 6;
    } = 1;
} = 451258;