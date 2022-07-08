# Alio #
"Another" language written in C++
  
Written for Debian Linux.

## Setup ##
Compiling and using alio for now, this will be changed ALOT in the future.

```
usr@penguin:~/ALIO$ g++ alio.cpp -o alio
usr@penguin:~/ALIO$ ./alio --help
usr@penguin:~/ALIO$ ./alio -f main.alio
### TEMPORARY BECAUSE SOMETHING THAT SHOULD WORK DOESN'T WORK ###
usr@penguin:~/ALIO$ nasm -f elf64 -o main.o main.asm
usr@penguin:~/ALIO$ ld -m elf_x86_64 -e main -o main main.o
usr@penguin:~/ALIO$ ./main
```

## Docs ##

### Procedures ###
   
Procedures are Alio's version of a funtion. They consist of two parts, the head and body. The head is the portion of code in between the label of the procedure and the begin keyword. In this part of the procedure we specify the in and out variables and other attributes of the procedure. After the begin keyword is the body of the procedure, this is the code that is executed when the procedure is called. The final thing about procedures is that they end (like all blocks in Alio) with the end keyword. For now the entry point is main, later this will have to be specified at the begining of the program.

```
proc main
in uint argc
in ptr argv
begin
  # Your code here :D
end
```

### Types ###
These are the current types and things about them in Alio.
    
- uint
- ptr
- int (currently treated as a uint, will be fixed soon)
- char
- bool
- long (not available in 32bit mode)
- string
  
Something interesting about Alio is that most operators are procedually created at compile time. So the 5 basic oparators (+, -, *, /, %) can be used with any combination of the existing types except for strings.

### Pointers ###
Pointers store an address to a place in memory. Most of the time this is to a variable. They have two unique operators. The first is the '&' operator used to get the address of a variable. The second is the '@' operartor, this is used to get the value at the address.
```
proc main
begin
  int x 5
  ptr y &x
  int z @x
end
```

### Static ###
Static variables are stored in the bss and data sections. This means they have some different properties from normal variables and are slightly slower to access.
```
;;include static <stdlib>

proc main
begin
  static string hello "Hello World!"
  ptr z &hello
  uint length strlen(z)
  SYS_write(fd z length)
end
```

### Syscalls ###
System Calls are used to interface with the linux kernel. The first argument is the syscall ID and the rest are the arguments to the call. Syscalls can also return values.
```
proc SYS_write
in uint fd
in ptr buffer
in uint size
begin
  syscall(1 fd buffer size)
end
```

### While ###
The keyword while instantiates a loop, currently it runs while the checked value is not 0. This will be changed in the future to support expressions with the addition of ifs, elses, and elifs.
```
;;include static <stdlib>

proc main
begin
  static string hi "Hello World"
  ptr str &hi
  # strlen
  ptr beginning str
  char c @str
  while(c)
    str++
    c @str
  end
  uint len str-beginning
  # Print
  str &hi
  SYS_write(STDOUT str length)
end
```

### If ###
If activates the code within the block if and only if the input is not zero or null. Just like while loops, expressions within the if comparison are not implemented yet.
```
proc main
begin
  uint x 69
  bool eq x = 420
  if(eq)
    static string y "Dank"
    ptr str &y
    uint length strlen(str)
    SYS_write(STDOUT str length)
  end
end
```

## Goals ##
- resupport x86 mode (elf32/i386)
- Add else and elif based off of while (already added if)
- Add %tmp1, %tmp2, %tmp3... for expression expansions in intermediate lang
- Use %tmp's for conditionals in while, if, elif, ect.
