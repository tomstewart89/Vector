/*
 * vector.h for Arduino ESP boards
 * 
 * This file is part of Simple vector library for Arduino ESP boards: https://github.com/BojanJurca/Cplusplus-vectors-for-Arduino
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
 * vectorType __elements__ :   | | |5|4|3|2|1|0|5|6|7|8|9| | | | | | | |
 *                             |    |<--- __size__ ---->|              |
 *                             | __beginning__        __end__          |
 *                             |<-------------- __capacity__ --------->|     
 * 
 *  Bojan Jurca, December 25, 2022
 *  
 */


#ifndef __VECTOR_H__
  #define __VECTOR_H__


  // uncommnet the following line if you want vectors to throw exceptions, which would cause the restart in case of errors 
  // since Arduino can not handle the exceptions. Enabling the exceptions may help finding problems the code.
  // #define __VECTOR_H_EXCEPTIONS__

  #ifdef __VECTOR_H_EXCEPTIONS__
    // Exceptions that vector can throw. It doesn't really matter what it throws - everithing will cause a reset since Arduino can't catch exceptions.
    #define out_of_range 1
    #define bad_alloc 2
  #endif


  template <class vectorType> class vector {

    public:

      /*
       *  Constructor of vector with no elements allows the following kinds of creation of vectors: 
       *  
       *    vector<int> A;
       *    vector<int> B (10); // with increment of 10 elements when vector grows, to reduce how many times __elements__ will be resized (which is time consuming)
       *    vector<int> C = { 100 };
       */
       
      vector (int increment = 1) {
        if (increment < 1) increment = 1;
        __increment__ = increment;
      }


      /*
       *  Constructor of vector from brace enclosed initializer list allows the following kinds of creation of vectors: 
       *  
       *     vector<int> D = { 200, 300, 400 };
       *     vector<int> E ( { 500, 600} );
       */

      vector (std::initializer_list<vectorType> il) {
        if (!reserve (il.size ())) return;
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
       *  Returns current storage capacity = the number of elements that can fit into the vector without needing to resize the storage
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
        if (newCapacity < __size__) return false; // can't change capacity without loosing some elements
        if (newCapacity > __size__) {
          if (__changeCapacity__ (newCapacity)) {
            __reserved__ = newCapacity;
            return true;
          } else {
            return false;
          }
        }
        return true; // no change in capacity
      }


      // checks if vector is empty
      bool empty () {
        return __size__ == 0;
      }


      // clears all the elements from the vector
      void clear () {
        __reserved__ = 0;
        if (__elements__ != NULL) __changeCapacity__ (0);
      }


      /*
       *  [] operator enables elements of vector to be addressed by their positions (indexes) like:
       *  
       *    for (int i = 0; i < E.size (); i++)
       *      Serial.printf ("E [%i] = %i\n", i, E [i]);    
       *    
       *    or
       *    
       *     E [0] = E [1];
       *     
       *  If the index is not a valid index, the result is unpredictable
       */

      vectorType &operator [] (int position) {
      
        #ifdef __VECTOR_H_EXCEPTIONS__
          if (position < 0 || position >= __size__) throw out_of_range;
        #endif
        
        return __elements__ [(__beginning__ + position) % __capacity__];
      }


      /*
       *  Same as [] operator, so it is not really needed but added here because it is supported in standard C++ vectors
       */      

      vectorType &at (int position) {

        #ifdef __VECTOR_H_EXCEPTIONS__
          if (position < 0 || position >= __size__) throw out_of_range;
        #endif

        return __elements__ [(__beginning__ + position) % __capacity__];
      }


      /*
       *  Copy-constructor of vector allows the following kinds of creation of vectors: 
       *  
       *     vector<int> F = E;
       *     
       *  Without properly handling it, = operator would probably just copy one instance over another which would result in crash when instances will be distroyed.
       *  
       *  Calling program should check size member function after constructor is beeing called for possible errors
       */

      vector<vectorType> (vector<vectorType>& other) {
        // clear existing elements if needed
        this->clear ();
        if (!this->reserve (other.size ())) return; // prevent resizing __elements__ for each element beeing pushed back
        // copy other's elements - storage will not get resized meanwhile
        for (auto e: other)
          this->push_back (e);

        // or
        // for (int i = 0; i < other.size (); i++)
        //   this->push_back (other [i]);       
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
        this->clear ();
        if (!this->reserve (other.size ())) return this; // prevent resizing __elements__ for each element beeing pushed back
        // copy other's elements - storege will not get resized meanwhile
        for (auto e: other)
          this->push_back (e);

        // or    
        // for (int i = 0; i < other.size (); i++)
        //   this->push_back (other [i]);               
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
          if (this->__elements__ [e] != other [i])
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
        if (__size__ == 0) {

          #ifdef __VECTOR_H_EXCEPTIONS__
            if (__size__ == 0) throw out_of_range;
          #endif

          return false;
        }
        
        // remove last element
        __end__ = (__end__ + __capacity__ - 1) % __capacity__; // __end__ - 1
        __size__ --;

        // do we have to free the space occupied by deleted element?
        if (__capacity__ > __size__ + __increment__ - 1) 
          if (__reserved__ == 0 || __size__ >= __reserved__)
            __changeCapacity__ (__size__); // doesn't matter if it does't succeed, the elekent is deleted anyway
        return true;
      }


      // this is not a standard C++ vector member function
      bool pop_front () {
        if (__size__ == 0) {
          
          #ifdef __VECTOR_H_EXCEPTIONS__
            if (__size__ == 0) throw out_of_range;
          #endif
                    
          return false;
        }
        
        // remove first element
        __beginning__ = (__beginning__ + 1) % __capacity__; // __beginning__ + 1
        __size__ --;

        // do we have to free the space occupied by deleted element?
        if (__capacity__ > __size__ + __increment__ - 1) 
          if (__reserved__ == 0 || __size__ >= __reserved__)
            __changeCapacity__ (__size__);  // doesn't matter if it does't succeed, the elekent is deleted anyway
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
          if (__elements__ [e] == element) return i;
          e = (e + 1) % __capacity__;
        }
        return -1;
      }


      /*
       *  Erases the element occupying at the position from the vector
       *  
       *  Returns true if succeeds and false in case of error:
       *    - element does't exist
       */

      bool erase (int position) {
        // is position a valid index?
        if (position < 0 || position >= __size__) {
          
          #ifdef __VECTOR_H_EXCEPTIONS__
            throw out_of_range;
          #endif
                    
          return false;
        }

        // try 2 faster options first
        if (position == __size__ - 1)                    return pop_back ();
        if (position == 0)                               return pop_front (); 

        // do we have to free the space occupied by the element to be deleted? This is the slowest option
        if (__capacity__ > __size__ - 1 + __increment__ - 1)    return __changeCapacity__ (__size__ - 1, position, -1);

        // we have to reposition the elements, weather from the __beginning__ or from the __end__, which is faster
        if (position < __size__ - position) {
          // move elements form position to 1
          int e1 = (__beginning__ + position) % __capacity__;
          for (int i = position; i > 0; i --) {
            int e2 = (e1 + __capacity__ - 1) % __capacity__; // e1 - 1
            __elements__ [e1] = __elements__ [e2];
            e1 = e2;
          }
          // delete the first element now
          pop_front (); // tere is no reason why this wouldn't succeed now
          return true;
        } else {
          // move elements from __size__ - 1 to position
          int e1 = position;
          for (int i = position; i < __size__ - 1; i ++) {
            int e2 = (e1 + 1) % __capacity__; // e2 = e1 + 1
            __elements__ [e1] = __elements__ [e2];
            e1 = e2;
          }
          // delete the last element now
          pop_back (); // tere is no reason why this wouldn't succeed now
          return true;          
        }
      }


      /*
       *  Inserts a new element at the position into the vector
       *  
       *  Returns true if succeeds and false in case of error:
       *    - could not allocate enough memory for requested storage
       */

      bool insert (int position, vectorType element) {
        // is position a valid index?
        if (position < 0 || position >= __size__) {
          
          #ifdef __VECTOR_H_EXCEPTIONS__
            throw out_of_range;
          #endif
                    
          return false;
        }

        // try 2 faster options first
        if (position == __size__ - 1)                                        return push_back (element); 
        if (position == 0)                                                   return push_front (element); 

        // do we have to resize the space occupied by existing the elements? This is the slowest option
        if (__capacity__ < __size__ + 1) {
          if (!__changeCapacity__ (__size__ + __increment__, -1, position))  return false;
          __elements__ [position] = element;                                 return true; 
        }

        // we have to reposition the elements, weather from the __beginning__ or from the __end__, which is faster

        if (position < __size__ - position) {
          // move elements form 0 to position
          __beginning__ = (__beginning__ + __capacity__ - 1) % __capacity__; // __beginning__ - 1
          __size__ ++;
          int e1 = __beginning__;
          for (int i = 0; i <= position; i++) {
            int e2 = (e1 + 1) % __capacity__; 
            __elements__ [e1] = __elements__ [e2];
            e1 = e2;
          }
          // insert new element now
          __elements__ [position] = element;
          return true;
        } else {
          // move elements from __size__ - 1 to position
          __end__ = (__end__ + 1) % __capacity__; // __end__ + 1
          __size__ ++;
          int e1 = __end__;
          for (int i = __size__ - 1; i > position; i--) {
            int e2 = (e1 + __capacity__ - 1) % __capacity__; // e2 = e1 - 1
            __elements__ [e1] = __elements__ [e2];
            e1 = e2;
          }
          // insert the new element now
          __elements__ [position] = element;        
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
          Iterator (vector *vect, int position) {
            __vector__ = vect;
            __position__ = position;
          }
          
          // * operator
          vectorType& operator *() const { 
            return *(__vector__->__elements__ + __position__); 
          }
      
          // ++ (prefix) increment
          Iterator& operator ++ () { 
            // this is a little tricky - we should allow iterator to go past the last element in a vector
            if (__vector__->__end__ >= __vector__->__beginning__) ++ __position__;
            else                                                  __position__ = (__position__ + 1) % __vector__->__capacity__; 
            return *this; 
          }  
      
          friend bool operator != (const Iterator& a, const Iterator& b) { return a.__position__ != b.__position__; };     
      
      private:
      
          vector *__vector__;
          int __position__;
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
      int __capacity__ = 0;             // the number of positions (occupied by elements and free ones) in __elements__
      int __increment__ = 1;            // by default, increment elements buffer for one element when needed
      int __reserved__ = 0;             // reserved capacity (if reserve member function is called)
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
        
        #ifdef __VECTOR_H_EXCEPTIONS__
          vectorType *newElements = new vectorType [newCapacity]; // allocate space for newCapacity elements
        #else
          vectorType *newElements = new (std::nothrow) vectorType [newCapacity]; // allocate space for newCapacity elements
        #endif
        
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
