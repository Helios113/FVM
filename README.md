# FVM - Finite Volume Method solver, Poisson equation

# Cpp Style guide
We follow the Google C++ Style Guide [[link](https://google.github.io/styleguide/cppguide.html)].  
Style is an important part of software development. It makes sure that the code stays legible and easy to understand for everyone. Style also helps prevent any dangerous constructs or any constructs which the average developer will struggle to maintain. The main idea of style is to make life easier for the reader not the writer of the code.

A summary of the style guide is given here:

## Naming

Naming informs us what sort of thing the named entity is without finding the declaration. The rules are arbitrary but their consistency is important.

| Category            | Naming convention                                        |
|---------------------|----------------------------------------------------------|
| Source file         | my_useful_source.cc                                      |
| Header file         | my_useful_header.h                                       |
| Type names          | class PoissonMaterial - "PascalCase"                                           |
| Type defs           | typedef hash_map<UrlTable *, std::string> PropertiesMap; |
| Variable names      | std::string table_name;                                  |
| Class Data Members  | std::string table_name_;  // underscore at end.          |
| Struct Data Members | std::string name; // no underscore                       |
|Constant Names      | const int kDaysInAWeek = 7; // leading k and "PascalCase"  |
|Pointer Names      | int* p_to_int = &table_name; // leading p |
| Function Names      | AddTableEntry()                                          |
| Comments            | //                                                       |
| File Comments       | Start each file with license boilerplate                 |


## Namespaces

What are namespaces. Add 

1. Namespace std

    **Never use** 
    ```c++
    using namespace std;
    ``` 
2. Long namepsaces  
    You can alias a long namespace with a shorter one similar to
    how one does in python:
    ```python
    import numpy as np
    np.array([1,2,3])
    ```
    Similarly, in c++ one can use:
    ```c++
    #include <boost/program_options.h>
    namespace po = boost::program_options;
    po::value<vector<int>>();
    ```
3. Using directive  
    The use of the using directive as in:
    ```c++
    using namespace boost::program_options; //forbidden
    ```
    is forbidden.
4. Namespaces in general  move to first bullet point
    All code must be in a namespace

    When definitions in a .cc file do not need to be referenced outside that file, they should be placed in a unnamed namespace or made static. This should not be done in header files.
    ```c++
    namespace{
        int function_which_is_only_used_internally()
        {
            return 0;
        }
    }
    ```

## Project structure


## Passing by reference/value and pointers

This chapter is for everyone who is a bit confused on what this whole pass by reference thing is.
This is a tl;dr, so I encourage everyone to read up in more detail after this explanation.

C++ is a strongly typed language. This means that every variable and function must have a data type. Data types simply describe what the thing at hand is. 

In c++ there are three main categories of data types: 
**Primitive, Derived and User Defined**. We will look at all three separately. [[full article](https://www.geeksforgeeks.org/c-data-types/)]

### Primitive data types
The primitive data types are:

| type    | size [bit] | name                         |
|---------|------------|------------------------------|
| int     | 32         | Integer                      |
| char    | 8          | Character (ASCII)            |
| bool    | 1          | Boolean                      |
| float   | 32         | Floating Point               |
| double  | 64         | Double Precision float       |
| void    | N/A        | Valueless                    |
| wchar_t | 16 to 32   | Wide Character [Not popular] |

These are the primitive types we use everyday. As you can see I given all of them sizes. However, the size of c++ types is defined by the complier so these values are typical values rather than concrete values. Unlike newer languages like Java and Python, which define the language specification and force every interpreter to follow, c++ relies on the compilers to standardize things. This leads to issues with portability when compiling on different systems and between compilers. For modern projects this should not be an issue and is just mentioned for completeness' sake.

## Derived data types
Derived data types, as the name suggest, are made with reference to primitive data types. One can consider the following four not strictly as data types, but semantically they are considered data types.

| name       | size                                                                    |
|------------|-------------------------------------------------------------------------|
| function   | same size as function type                                              |
| array      | size of array times array type                                          |
| pointer    | same size as the system architecture for 64-bit the pointers are 64 bit |
| references | same size as the system architecture for 64-bit the pointers are 64 bit |


Here we touch on a few important subjects in c++. 

First, we saw that functions are in fact data types. This might seem odd at first, but consider this example:
```c++
int AddTwoNumbers(int a, int b)
{
    return a+b;
}


int c = AddTwoNumbers(1,2);
```
Here, we can replace the function with any other integer and the compiler will be happy, thus intrinsically this function acts like a variable. Functions have any other aspects but those are outside our current scope.


Secondly, we will consider arrays. Arrays are derived types which have a constant size. Once an array is defined its size cannot be changed. This is due to the operational method of arrays. An array is just multiple blocks of **sequential** memory. Thus, once an array is defined, there is no guarantee that the next memory space will be free. Thus, the ability to extend the array is not guaranteed. The sequential memory part of arrays is important. This makes arrays very quick and memory efficient.

Next are pointers. Pointers are directly related to the way memory operates. We will consider a very simple memory model of a computer. The computer will have a CPU which does operations and a big memory table. 
Each table cell is one byte and has a 64-bit address which starts with 0x and then 12 hexadecimal numbers (0 to F) like 0x7ffebb020e5c.
In c++ pointers also have types which show to which type the pointer is pointing to. These are enforced by the compiler and not by the CPU. Thus, no extra space is taken by them in memory. 

To define a pointer one can write:
```c++
// three ways to define a pointer

int* a; //preferred way to define pointer

int *a; //pointers are types so adding the star to the variable makes it less readable

int * a; //bad formatting 
```

Here a important distinction needs to be made.   
**The star, when used in a definition, defines a pointer; however the star is also an operator which works on pointers.**

The final derived type is the reference. A reference is similar to a pointer. It gives the memory location of a variable, however, unlike the pointer, its memory location cannot be changed. So it can be examined as a alias for a variable. Additionally, one cannot define a reference without initializing it.

To define a reference one can write:
```c++
int a = 0;

int& b; //compiler error

int& b = a; //preferred way to declare references; 

int &b = a; //available but discouraged method;

int & b = a; //same as above
```

Here is some sample code on how pointers, references and their respective operators function:

```c++
int n = 5; //initialized int

int* a; //uninitialized int pointer

int& b = n; //initialized int reference

a = &n; // &-address operator
        // it is used to get the address of a variable
        // thus can be used to populate pointers

int c = 3+(*a); // *-dereference operator
                // get the value at address

*a = 7; //dereference operator can be used on both r and l sides
        //this line changes the value at address a to 7

cout<<n<<endl; // this will give 7

cout<<&(*a); // the address of the value pointed to by a

// VERY DANGEROUS
    int* d_angerous;
    *d_angerous = 55; // very dangerous
             // this sets the value of a random memory address to 55

int* e_rror = NULL; // this is the infamous null pointer
cout<<*e_rror; // this will cause an error (STATUS_ACCESS_VIOLATION exception)
```


### References vs. Pointers


1. A reference is a name constant for an address. You can never change the address it is pointing to. Thus the type of the reference is set, however its value can change.

2. Pointers need the address of and dereference operators to get values. References do all of this implicitly and no extra operators are need.

Here is again some code which demonstrates this:

```c++
int n1 = 1, n2 = 2;
 
// Create a pointer pointing to n1
int* p_n1 = &n1;  // Explicit referencing

*p_n1 = 99;             // Explicit dereferencing
cout << *p_n1 << endl;  // 99
cout << &n1 << endl;    // 0x7ffebb020e5c
cout << p_n1 << endl;   // 0x7ffebb020e5c (content of the pointer variable - same as above)
cout << &p_n1 << endl;  // 0x7ffe0987f7dc (address of the pointer variable)

p_n1 = &n2;             // Pointer can be reassigned to store another address
 
// Create a reference (alias) to n1
int& r_n1 = n1;         // Implicit referencing (NOT &n1)
r_n1 = 11;              // Implicit dereferencing (NOT *r_n1)
cout << r_n1 << endl;   // 11
cout << &n1 << endl;    // 0x22f0x7ffebb020e5cf18
cout << &r_n1 << endl;  // 0x7ffebb020e5c
//r_n1 = &n2;           // Error! Reference cannot be re-assigned
                        // error: invalid conversion from 'int*' to 'int'

r_n1 = n2;              // r_n1 is still an alias to n1.
                        // Assign value of n2 (2) to r_n1 (and n1).
n2++;   
cout << r_n1 << endl;   // 2
cout << n1 << endl;     // 2
cout << n2 << endl;     // 3
```

Then, once we know about pointers and references, we can start using them in function calls to make our code work as intended. A in depth reference to all the things I will cover can be found [[here](https://www3.ntu.edu.sg/home/ehchua/programming/cpp/cp4_PointerReference.html)].


## Passing data to functions

### Pass-by-Value

The most basic way of passing information to a function is through the variable's value. In this scenario, the data from one variables is copied into a new variable into the target function and there is no link between the two. This is a good approach if you do not want to modify original variable. However, if the data is large, copying it each time a function is called can become very slow. In addition, this can hinder parallelization, as the data will need to be copied many times over for a single calculation. Regardless, pass by value is the default way of passing info.

Here is a code example of how this works:
```c++ 
int square(int);
 
int main() {
   int number = 8;
   cout <<  "In main(): " << &number << endl;  // 0x22ff1c
   cout << number << endl;                     // 8
   cout << square(number) << endl;             // 64
   cout << number << endl;                     // 8 - no change
}

int square(int n) {                            // non-const
   cout <<  "In square(): " << &n << endl;     // 0x22ff00
   n *= n;                        // clone modified inside the function
   return n;
}
```

### Pass-by-Reference
There are two types of pass by reference in c++. Both help programmers save time by not coping data and also helps reduce memory usage.

The first type is passing an address into the function and receiving it with a pointer as:

```c++
void square(int*);

int main() {
   int number = 8;
   cout <<  "In main(): " << &number << endl;  // 0x22ff1c
   cout << number << endl;   // 8
   square(&number);          // Explicit referencing to pass an address
   cout << number << endl;   // 64
}
 
void square(int* pNumber) {  // Function takes an int pointer (non-const)
   cout <<"In square():"<< pNumber << endl;  // 0x22ff1c
   *pNumber *= *pNumber;      // Explicit de-referencing to get the value pointed-to
}
```

The second types is by passing the variable itself and receiving a reference in the function body as:

```c++
void square(int&);
 
int main() {
   int number = 8;
   cout <<  "In main(): " << &number << endl;  // 0x22ff1c
   cout << number << endl;  // 8
   square(number);          // Implicit referencing (without '&')
   cout << number << endl;  // 64
}
 
void square(int& rNumber) {  // Function takes an int reference (non-const)
   cout <<  "In square(): " << &rNumber << endl;  // 0x22ff1c
   rNumber *= rNumber;        // Implicit de-referencing (without '*')
}
```
This is a basic overview of how pass by reference and pass by value work there is more detail on how one can return values and how to make the operations safe. Currently, there is no need to use more complex constructions than the ones shown above.


## User Defined Data types
The final data types which will be shown here are user defied data:  

    - Class  
    - Structure  
    - Union  
    - Enumeration     
    - Typedef defined Datatype 

These data types are entirely defined by the user. They should be used sparingly as they can introduce confusion and overhead. In general one of the advantages of C++ over C and Fortran is the fact that it supports classes and enables OOP to be used for large software projects. However, everything that can be done with OOP can also be executed using procedural programming.

OOP becomes useful when there is a logical connection between data and function which modify it. In general OOP should be used when an object is to be reused many times by different parts of the code. If a method or procedure is to be used only once and in one part of the code one can use procedural programming instead. If data integrity is important (e.g., the code user should not change the data without performing checks or validations) then OOP should be used to encapsulate data and only expose safe modifying methods.