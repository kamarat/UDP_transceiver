#ifndef CIRCULAR_BUFFER_HPP
#define CIRCULAR_BUFFER_HPP

//#include <iostream>

using namespace std;

template < typename T, const uint8_t SIZE >
class CircularBuffer
{
  private:
    T buffer_[ SIZE ];
    T *writePosition_;
    T *readPosition_;
    uint8_t filled_;

  public:
    CircularBuffer() : writePosition_( buffer_ ), readPosition_( buffer_ )
    {
      filled_ = 0;
    }

    ~CircularBuffer() {};
    void push( const T & buf );
    T pop();
    T read();
    uint8_t isNonEmpty();
    uint8_t isFull();
};

template < typename T, const uint8_t SIZE >
void CircularBuffer< T, SIZE >::push( const T & buf )
{
  if ( isFull() )
    pop();

  *writePosition_++ = buf;
  filled_++;

  if ( writePosition_ == ( buffer_ + SIZE ))
    writePosition_ = buffer_;
}

template < typename T, const uint8_t SIZE >
T CircularBuffer< T, SIZE >::pop()
{
  T buf = *readPosition_++;
  filled_--;

  if ( readPosition_ == ( buffer_ + SIZE ))
    readPosition_ = buffer_;

  return buf;
}

template < typename T, const uint8_t SIZE >
T CircularBuffer< T, SIZE >::read()
{
  return *readPosition_;
}

template < typename T, const uint8_t SIZE >
uint8_t CircularBuffer< T, SIZE >::isNonEmpty()
{
  return filled_;
}

template < typename T, const uint8_t SIZE >
uint8_t CircularBuffer< T, SIZE >::isFull()
{
  return filled_ == SIZE;
}

#endif  // #ifndef CIRCULAR_BUFFER_HPP
