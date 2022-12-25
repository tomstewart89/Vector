/*
 * vector.h for Arduino ESP boards
 * 
 * This file is part of Simple vector library for Arduino ESP boards: https://github.com/BojanJurca/Vectors-for-Arduino-ESP-boards
 * 
 * vector.h is based on Tom Stewart's work: https://github.com/tomstewart89/Vector
 * with some differences:
 * 
 * - syntax is closer to standard C++ vectors
 * - error handling added
 * - internal storage structure is different and also the logic for handling capacity
 * 
 * Vector internal storage is implemented as circular queue. It may have some free slots to let the vector breath a litle without having 
 * to resize it all the time. Normally a vector would require additional chunk of memory when it runs out of free slots. How much memory
 * would it require depends on increment parameter of a constuctor which is normally 1 but can also be some larger number.
 * 
 * Picture of vector intgernal storge:
 * 
 * vectorType __elements__ :   | | |0|1|2|3|4|5|6|7|8|9|0| | | | | | | |
 *                             |    |<--- __size__ ----->|             |
 *                             | __beginning__         __end__         |
 *                             |<-------------- __capacity__ --------->|     
 * 
 *  Bojan Jurca, December 25, 2022
 *  
 */


#ifndef __VECTOR_H__
  #define __VECTOR_H__


  // exception codes
  #define out_of_range  1
  #define bad_alloc     2
  #define length_error  3

  template <class vectorType> class vector {

    public:

      /*
       *  Constructor of vector with no elements allows the following kinds of creations of vectors: 
       *  
       *    vector<int> A;
       *    vector<int> B (10); // with increment of 10 elements when vector grows, to reduce how many times __elements__ will be resized
       *    vector<int> C = { 100 };
       */
       
      vector (int increment = 1) {
        if (increment < 1) increment = 1;
        __increment__ = increment;
      }


      /*
       *  Constructor of vector from brace enclosed initializer list allows the following kinds of creations of vectors: 
       *  
       *     vector<int> D = { 200, 300, 400 };
       *     vector<int> E ( { 500, 600} );
       */

      vector (std::initializer_list<vectorType> il) {
        __increment__ = il.size (); // this will make __elements__ to be resized only once
        for (auto i: il)
          push_back (i);
      }

      
      // destructor
      ~vector () {
        if (__elements__ != NULL) delete [] __elements__;
      }


      // returns number of elements in vector
      int size () {
        return __size__;
      }

      /*
       *  Returns current storage capacity = the number of elements that can fit io the vector without resizing the storage
       */

      int capacity () {
        return __capacity__;
      }


      /*
       *  Request a change in storage capacity
       *  
       *  Returns true if succeeds and false in case of error:
       *    - requested capacity is less than current vector size
       *    - could not allocate enough memory for requested storage
       */
       
      bool reserve (int newCapacity) {
        if (newCapacity < __size__) return false;
        if (newCapacity > __size__) return __changeCapacity__ (newCapacity);
        return true;
      }


      // checks if vector is empty
      int empty () {
        return __size__ == 0;
      }


      // clears all the elements of the vector
      void clear () {
        if (__elements__ != NULL) __changeCapacity__ (0);
      }


      /*
       *  [] operator enables elements of vector to be addressed by their indexes like:
       *  
       *    for (int i = 0; i < E.size (); i++)
       *      Serial.printf ("E [%i] = %i\n", i, E [i]);    
       *      
       *     E [0] = E [1];
       *     
       *  If the index is not a valid index, the result is unpredictable
       */

      vectorType &operator [] (int elementPosition) {
        return __elements__ [(__beginning__ + elementPosition) % __capacity__];
      }


      /*
       *  Same as [] operator, so it is not really needed but added here because it is supported in standard C++ vectors
       */      

      vectorType &at (int elementPosition) {
        return __elements__ [(__beginning__ + elementPosition) % __capacity__];
      }


      /*
       *  Copy-constructor of vector allows the following kinds of creations of vectors: 
       *  
       *     vector<int> F = E;
       *     
       *  Without properly handling it, = operator would probably just copy one instance over another which would result in crash when instances will be distroyed.
       *  
       *  Calling program should check size member function after constructor is beeing called for possible errors
       */

      vector<vectorType> (vector<vectorType>& other) {
        // clear existing elements if needed
        clear ();
        if (!reserve (other.size ())) return; // prevent resizing __elements__ for each element beeing pushed back
        // copy other's elements - storege will not get resized meanwhile
        for (int i = 0; i < other.size (); i++)
          this->push_back (other [i]);       
      }


      /*
       *  Assignment operator of vector allows the following kinds of assignements of vectors: 
       *  
       *     vector<int> F;
       *     F = { 1, 2, 3 }; or F = {};
       *     
       *  Without properly handling it, = operator would probably just copy one instance over another which would result in crash when instances will be distroyed.
       */

      vector<vectorType>* operator = (vector<vectorType> other) {
        // clear existing elements if needed
        clear ();
        if (!this->reserve (other.size ())) return this; // prevent resizing __elements__ for each element beeing pushed back
        // copy other's elements - storege will not get resized meanwhile
        for (int i = 0; i < other.size (); i++)
          this->push_back (other [i]);               
        return this;
      }


      /*
       * == operator allows comparison of vectors, like:
       * 
       *  Serial.println (F == E ? "vectors are equal" : "vectors are different");
       */

      bool operator == (vector& other) {
        if (this->__size__ != other.size ()) return false;
        int e = this->__beginning__;
        for (int i = 0; i < this->__size__; i++) {
          if (__elements__ [e] != other [i])
            return false;
          e = (e + 1) % this->__capacity__;
        }
        return true;
      }


      /*
       *  Adds element to the end of a vector, like:
       *  
       *    E.push_back (700);
       *    
       *  Returns true if succeeds and false in case of error:
       *    - could not allocate enough memory for requested storage
       */

      bool push_back (vectorType element) {
        // do we have to resize __elements__ first?
        if (__size__ == __capacity__) 
          if (!__changeCapacity__ (__capacity__ + __increment__)) {
            return false;
          }

        // add the new element at the end, at this point we can be sure that there is enough space in __elements__
        __end__ = (__end__ + 1) % __capacity__; // __end__ + 1
        __elements__ [__end__] = element;
        __size__ ++;
        if (__size__ == 1) __beginning__ = __end__;
        return true;
      }


      // this is not a standard C++ vector member function
      bool push_front (vectorType element) {
        // do we have to resize __elements__ first?
        if (__size__ == __capacity__) 
          if (!__changeCapacity__ (__capacity__ + __increment__))
            return false;

        // add the new element at the beginning, at this point we can be sure that there is enough space in __elements__
        __beginning__ = (__beginning__ + __capacity__ - 1) % __capacity__; // __beginning__ - 1
        __elements__ [__beginning__] = element;
        __size__ ++;
        if (__size__ == 1) __end__ = __beginning__;
        return true;
      }


      /*
       *  Deletes last element from the end of a vector, like:
       *  
       *    E.pop_back ();
       *    
       *  Returns true if succeeds and false in case of error:
       *    - element does't exist
       */

      bool pop_back () {
        if (__size__ == 0) return false;
        
        // remove last element
        __end__ = (__end__ + __capacity__ - 1) % __capacity__; // __end__ - 1
        __size__ --;

        // do we have to free the space occupied by deleted element?
        if (__capacity__ > __size__ + __increment__ - 1) __changeCapacity__ (__size__); // doesn't matter if it does't succeed, the elekent is deleted anyway
        return true;
      }


      // this is not a standard C++ vector member function
      bool pop_front () {
        if (__size__ == 0) return false;
        
        // remove first element
        __beginning__ = (__beginning__ + 1) % __capacity__; // __beginning__ + 1
        __size__ --;

        // do we have to free the space occupied by deleted element?
        if (__capacity__ > __size__ + __increment__ - 1) __changeCapacity__ (__size__);  // doesn't matter if it does't succeed, the elekent is deleted anyway
        return true;
      }


      /*
       *  Returns a position (index) of the first occurence of the element in the vector if it exists, -1 otherwise. Example:
       *  
       *  Serial.println (D.find (400));
       *  Serial.println (D.find (500));
       */

      int find (vectorType element) {
        int e = __beginning__;
        for (int i = 0; i < __size__; i++) {
          if (__elements__ [i] == element) return e;
          e = (e + 1) % __capacity__;
        }
        return -1;
      }


      /*
       *  Erases the element occupying position elementPosition from the vector
       *  
       *  Returns true if succeeds and false in case of error:
       *    - element does't exist
       */

      bool erase (int elementPosition) {
        // is elementPosition a valid index?
        if (elementPosition < 0 || elementPosition >= __size__) return false;

        // try 2 faster options first
        if (elementPosition == __size__ - 1)                    return pop_back ();
        if (elementPosition == 0)                               return pop_front (); 

        // do we have to free the space occupied by the element to be deleted? This is the slowest option
        if (__capacity__ > __size__ - 1 + __increment__ - 1)    return __changeCapacity__ (__size__ - 1, elementPosition, -1);

        // we have to reposition the elements, weather from the __beginning__ or from the __end__, which is faster
        if (elementPosition < __size__ - elementPosition) {
          // move elements form elementPosition to 1
          int e1 = (__beginning__ + elementPosition) % __capacity__;
          for (int i = elementPosition; i > 0; i --) {
            int e2 = (e1 + __capacity__ - 1) % __capacity__; // e1 - 1
            __elements__ [e1] = __elements__ [e2];
            e1 = e2;
          }
          // delete the first element now
          pop_front (); // tere is no reason why it wouldn't succeed now
          return true;
        } else {
          // move elements from __size__ - 1 to elementPosition
          int e1 = elementPosition;
          for (int i = elementPosition; i < __size__ - 1; i ++) {
            int e2 = (e1 + 1) % __capacity__; // e2 = e1 + 1
            __elements__ [e1] = __elements__ [e2];
            e1 = e2;
          }
          // delete the last element now
          pop_back (); // tere is no reason why it wouldn't succeed now
          return true;          
        }
      }


      /*
       *  Inserts a new element at position elementPosition to the vector
       *  
       *  Returns true if succeeds and false in case of error:
       *    - could not allocate enough memory for requested storage
       */

      bool insert (int elementPosition, vectorType element) {
        // is elementPosition a valid index?
        if (elementPosition < 0 || elementPosition >= __size__)                     return false;

        // try 2 faster options first
        if (elementPosition == __size__ - 1)                                        return push_back (element); 
        if (elementPosition == 0)                                                   return push_front (element); 

        // do we have to resize the space occupied by existing the elements? This is the slowest option
        if (__capacity__ < __size__ + 1) {
          if (!__changeCapacity__ (__size__ + __increment__, -1, elementPosition))  return false;
          __elements__ [elementPosition] = element;                                 return true; 
        }

        // we have to reposition the elements, weather from the __beginning__ or from the __end__, which is faster

        if (elementPosition < __size__ - elementPosition) {
          // move elements form 0 to elementPosition
          __beginning__ = (__beginning__ + __capacity__ - 1) % __capacity__; // __beginning__ - 1
          __size__ ++;
          int e1 = __beginning__;
          for (int i = 0; i <= elementPosition; i++) {
            int e2 = (e1 + 1) % __capacity__; 
            __elements__ [e1] = __elements__ [e2];
            e1 = e2;
          }
          // insert new element now
          __elements__ [elementPosition] = element;
          return true;
        } else {
          // move elements from __size__ - 1 to elementPosition
          __end__ = (__end__ + 1) % __capacity__; // __end__ + 1
          __size__ ++;
          int e1 = __end__;
          for (int i = __size__ - 1; i > elementPosition; i--) {
            int e2 = (e1 + __capacity__ - 1) % __capacity__; // e2 = e1 - 1
            __elements__ [e1] = __elements__ [e2];
            e1 = e2;
          }
          // insert the new element now
          __elements__ [elementPosition] = element;        
          return true;
        }
      }


      /*
       *  Iterator is needed in order for standard C++ for each loop to work. 
       *  A good source for iterators is: https://www.internalpointers.com/post/writing-custom-iterators-modern-cpp
       *  
       *  Example:
       *  
       *    for (auto element: A) 
       *      Serial.println (element);
       */

      class Iterator {
        public:
          // constructor
          Iterator (vector* vect, int elementPosition) {
            __vector__ = vect;
            __elementPosition__ = elementPosition;
          }
          
          // * operator
          vectorType& operator * () const { 
            return *(__vector__->__elements__ + __elementPosition__); 
          }
      
          // ++ (previx) increment
          Iterator& operator ++ () { 
            // this is a little tricky - we should allow iterator to go past the last element in a vector
            if (__vector__->__end__ >= __vector__->__beginning__) ++ __elementPosition__;
            else                                                  __elementPosition__ = (__elementPosition__ + 1) % __vector__->__capacity__; 
            return *this; 
          }  
      
          friend bool operator!= (const Iterator& a, const Iterator& b) { return a.__elementPosition__ != b.__elementPosition__; };     
      
      private:
      
          vector* __vector__;
          int __elementPosition__;
      };      

      Iterator begin () { return Iterator (this, __beginning__); }
      Iterator end ()   { return Iterator (this, __end__ + 1); } // past the last element


      // displays vector's internal structure
      /*      
      void debug () {
        Serial.printf ("\n\n"
                       "__elements__  = %p\n"
                       "__capacity__  = %i\n"
                       "__increment__ = %i\n"
                       "__size__      = %i\n"
                       "__beginning__ = %i\n"
                       "__end__       = %i\n\n",
                       __elements__, __capacity__, __increment__, __size__, __beginning__, __end__);
        for (int i = 0; i < __capacity__; i++) {
          Serial.printf ("   %i. [", i); Serial.print (__elements__ [i]); Serial.printf ("] ");
          if (i == __beginning__) Serial.printf (" <-__beginning__ (front)");
          if (i == __end__) Serial.printf (" <-__end__ (back)");
          Serial.printf ("\n");
        }
        Serial.printf ("\n");
      }
      */

      
    private:

      friend Iterator;

      vectorType *__elements__ = NULL;  // initially the vector has no elements, __elements__ buffer is empty
      int __capacity__ = 0;             // initial number of elements (or not occupied slots) in __elements__
      int __increment__ = 1;            // by default, increment elements buffer for one element when needed
      int __size__ = 0;                 // initially there are not elements in __elements__
      int __beginning__ = 0;            // points to the first element in __elements__
      int __end__ = -1;                 // points to the last element in __elements__


      /*
       *  Resizes __elements__ to new capacity with the option of deleting and adding an element meanwhile
       *  
       *  Returns true if succeeds and false in case of error:
       *    - could not allocate enough memory for requested storage
       */

      bool __changeCapacity__ (int newCapacity, int deleteElementAtPosition = -1, int leaveFreeSlotAtPosition = -1) {
        if (newCapacity == 0) {
          // delete old buffer
          if (__elements__ != NULL) delete [] __elements__;
          
          // update internal variables
          __capacity__ = 0;
          __elements__ = NULL;
          __size__ = 0;
          __beginning__ = 0;
          __end__ = -1;
          return true;
        } 
        // else
        vectorType *newElements = new (std::nothrow) vectorType [newCapacity]; // allocate space for newCapacity elements
        if (newElements == NULL) return false;
        
        // copy existing elements to the new buffer
        if (deleteElementAtPosition >= 0) __size__ --;      // one element will be deleted
        if (leaveFreeSlotAtPosition >= 0) __size__ ++;      // a slot for 1 element will be added
        if (__size__ > newCapacity) __size__ = newCapacity; // shouldn't really happen
        
        int e = __beginning__;
        for (int i = 0; i < __size__; i++) {

          // is i-th element supposed to be deleted? Don't copy it then ...
          if (i == deleteElementAtPosition) e = (e + 1) % __capacity__; // e ++
          
          // do we have to leave a free slot for a new element at i-th place? Continue with the next index ...
          if (i == leaveFreeSlotAtPosition) continue;
          
          newElements [i] = __elements__ [e];
          e = (e + 1) % __capacity__;
        }
        
        // delete the old elements' buffer
        if (__elements__ != NULL) delete [] __elements__;
        
        // update internal variables
        __capacity__ = newCapacity;
        __elements__ = newElements;
        __beginning__ = 0;
        __end__ = __size__ - 1;
        return true;
      }

  };

#endif
