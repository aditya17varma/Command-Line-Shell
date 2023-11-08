# Command Line Shell


This project is an attempt to build a basic shell.
The main concepts used are: elastic array, pipes, forks, signal handling, and commincicating between multiple files.
The shell uses execvp to execute the commands the user passes in.
Piping is supported.

All the user commands are saved in an array, which can be accessed with a builtin function.
Builtins supported are: "exit", "cd", "!", "history".

## Building
To compile and run:

```bash
make
./endsWithSH
```

## Included Files

* **shell.c** -- The main shell function, tokenizes and executes commands received from ui.c
* **ui.c** -- Sets up the user interface of the shell, parses input into commands and passes them into shell.c
* **history.c** -- Contains the elastic array that stores all the commands.

## Testing

To execute the test cases, use `make test`. To pull in updated test cases, run `make testupdate`. You can also run a specific test case instead of all of them:

```
# Run all test cases:
make test

# Run a specific test case:
make test run=4

# Run a few specific test cases (4, 8, and 12 in this case):
make test run='4 8 12'

# Run a test case in gdb:
make test run=4 debug=on
```


## Demo Run
```

****************************************************************************


        ****  ARE YOU READY TO ENTER THE DRAGON??  ****


                                ^    ^
                               / \  //\ 
                 |\___/|      /   \//  \ 
                 /0  0  \__  /    //  | \ \ 
                /     /  \/_/    //   |  \  \ 
                @_^_@'/   \/_   //    |   \   \ 
                //_^_/     \/_ //     |    \    \ 
             ( //) |        \///      |     \     \ 
           ( / /) _|_ /   )  //       |      \     _\ 
         ( // /) '/,_ _ _/  ( ; -.    |    _ _\.-~        .-~~~^-. 
        (( / / )) ,-{        _      `-.|.-~-.           .~         `. 
      (( // / ))  '/\      /                 ~-. _ .-~      .-~^-.  \ 
      (( /// ))      `.   {            }                   /      \  \ 
       (( / ))     .----~-.\        \-'                 .~         \  `. \^-. 
                  ///.----..>        \             _ -~             `.  ^-`  ^-__ 
                    ///-._ _ _ _ _ _ _}^ - - - - ~                     ~-- ,.-~ 
                                                                        /.-~

****************************************************************************
ui.c:56:init_ui(): Initializing UI...
ui.c:59:init_ui(): Setting locale: en_US.UTF-8
elist.c:45:elist_create(): Created new elist. Size = 0, capacity = 100, start address = 0x55d30f6d3410
elist.c:45:elist_create(): Created new elist. Size = 0, capacity = 10, start address = 0x55d30f6d3760
>>-[😌]-[0]-[avkunatharaju@TheSevenCs:~/P3-aditya17varma]-> ls
shell.c:200:main(): Input command: ls
a.out  elist.c  elist.h  elist.o  endsWithSH  history.c  history.h  history.o  libshell.so  logger.h  Makefile  README.md  shell.c  shell.o  test-outputs  tests  ui.c  ui.h  ui.o
>>-[😌]-[1]-[avkunatharaju@TheSevenCs:~/P3-aditya17varma]-> ls | sort
shell.c:200:main(): Input command: ls | sort
a.out
elist.c
elist.h
elist.o
endsWithSH
history.c
history.h
history.o
libshell.so
logger.h
Makefile
README.md
shell.c
shell.o
test-outputs
tests
ui.c
ui.h
ui.o
>>-[😌]-[2]-[avkunatharaju@TheSevenCs:~/P3-aditya17varma]-> cd
shell.c:200:main(): Input command: cd
shell.c:265:main(): builtin: cd cd
>>-[😌]-[3]-[avkunatharaju@TheSevenCs:~]->
```
