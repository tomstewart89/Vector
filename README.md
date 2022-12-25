# A simple vector library for Arduino ESP boards


vector.h is based on [Tom Stewart's work](https://github.com/tomstewart89/Vector) with some differences:


 - syntax is closer to standard C++ vectors
 - error handling added
 - internal storage structure is different and also the logic for handling capacity.


I have tried supporting vectors similar to standard C++ vectors but not all vector member functions are supported (yet). Error handling needed a different approach since  try {} catch () {} C++ functionality is not supported by Arduino. 
