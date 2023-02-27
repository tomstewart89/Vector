# C++ vectors for Arduino (ESP boards)


vector.h is based on [Tom Stewart's work](https://github.com/tomstewart89/Vector) with some differences:


 - syntax is closer to standard C++ vectors
 - error handling added
 - internal storage structure is different and also the logic for handling capacity.


I have tried supporting vectors similar to STL C++ vectors but not all vector member functions are supported (yet). 

Error handling (like running out of memory, etc) needed a different approach since try {} catch () {} C++ functionality is not supported by Arduino. 


Checking error of each function call:

```C++
    vector<int> v1;
    vector<int>::errorCode e = v1.push_back (100);
    if (e == vector<int>::OK)
        Serial.println ("push_back succeded");
    else
        Serial.println ("push_back error " + String (e));
```

Checking error of multiple operations:

```C++
    vector<int> v2;
    for (int i = 1000; i < 1100; i++)
        v2.push_back (i);
    if (v2.lastErrorCode == vector<int>::OK)
        Serial.println ("100 push_backs succeded");
    else {
        Serial.println ("100 push_backs error " + String (v2.lastErrorCode));
        v2.clearLastErrorCode (); // clear lastErrorCode before next operations
    }
```
