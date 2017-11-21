/* -*- mode: c++; c-basic-offset: 2; -*- */
extern "C" {
int qklee_main(int argc, char **argv, char **envp);
int qklee_exit(void);
}

int main(int argc, char **argv, char **envp) {  
    qklee_main(argc, argv, envp);

    qklee_exit();
}
