# C++ vectors for Arduino (ESP boards)


vector.h is based on [Tom Stewart's work](https://github.com/tomstewart89/Vector) with some differences:


 - Syntax is closer to standard C++ vectors,
 - Error handling added,
 - Internal storage structure is different and also the logic for handling capacity,
 - Supports also Arduino Strings.


I have tried supporting vectors similar to STL C++ vectors but not all vector member functions are supported (yet). Furthermore vectors are not completely general regarding all possible data types since many things can go wrong with them, especially when the controller is running out of memory. All the exceptions need to be handled somehow.


Arduino does not have try {} catch () {} C++ functionality so the error handling (like running out of memory, etc) needed a different approach.

Errors can be checked for each individual function call ...

```C++
    vector<int> v1;
    vector<int>::errorCode e = v1.push_back (100);
    if (e == vector<int>::OK)
        Serial.println ("push_back succeded");
    else
        Serial.println ("push_back error " + String (e));
```

... or once after multiple operations:

```C++
    vector<int> v2;
    for (int i = 1; i <= 100; i++)
        v2.push_back (i);
    if (v2.lastErrorCode == v2.OK)
        Serial.println ("100 push_backs succeded");
    else {
        Serial.println ("100 push_backs error " + String (v2.lastErrorCode));
        v2.clearLastErrorCode (); // clear lastErrorCode before next operations
    }
```

All possible Arduino String errors need to be handled within the code. There are more things that can go wrong when dealing with Stings than when dealing with integers, for example. Consider the following two vector.find cases. In the integer case find can only return -1 (NOT_FOUND) or a valid index of the element being found, whereas in the String case, it is also possible that the controller can't even construct an argument String that gets passed to find function due to the lack of memory. Find function can't even start searching. After returning we can not be sure if the element exists in the vector or not. 

```C++
    vector<int> vi = { 1, 2, 3 };
    if (vi.lastErrorCode != vi.OK) {
        Serial.printf ("failed to initialize the vector\n"); // What can go wrong? Controller may not have enough free memory to put all the elements into the vector.
    } else {
        int f = vi.find (2);
        if (f < 0) Serial.printf ("element not found\n"); // Well, the element 2 will in this case always be found at position 1, since vector initialization succeeded.
        else Serial.printf ("element found at position %i\n", f);
    }

    vector<String> vstring = { "one", "two", "tree" };
    if (vstring.lastErrorCode != vstring.OK) {
        Serial.printf ("failed to initialize the vector\n"); // What can go wrong? Controller may not have enough free memory to put all the elements into the vector.
    } else {
        int f = vstring.find ("two");
        switch (f) {
          case vstring.NOT_FOUND:   Serial.printf ("element not found\n"); // The element "two" should be found at possition 1 if there is no other error
                                    break;
          case vstring.BAD_ALLOC:   Serial.printf ("find failed, that doesn't mean that the element is not there\n"); // Creation of find parameter failed due to lack of memory so find couldn't even start searching
                                    break;
          default:                  Serial.printf ("element found at position %i\n", f);
                                    break;
        }
    }
```


