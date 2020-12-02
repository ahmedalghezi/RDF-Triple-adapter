//
// chat_message.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>

class chat_message
{
public:
  enum { header_length = 4 };
  enum { max_body_length = 2512 };

 // enum { sequence_nr_length = 3 };
  //enum { sequence_nr_total_length = 3 };
  
  chat_message()
    : body_length_(0) 
  {
  }

  const char* data() const
  {
    return data_;
  }

  char* data()
  {
    return data_;
  }

  size_t length() const
  {
    return header_length + body_length_;
  }

  const char* body() const
  {
    return data_ + header_length;
  }

  char* body()
  {
    return data_ + header_length;
  }

  size_t body_length() const
  {
    return body_length_;
  }

  void body_length(size_t new_length)
  {
    body_length_ = new_length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
  }

  bool decode_header()
  {
    using namespace std; // For strncat and atoi.
    char header[header_length + 1] = "";
    strncat(header, data_, header_length);
    body_length_ = atoi(header);
    if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }
    return true;
  }
  
   void encode_header()
  {
    using namespace std; // For sprintf and memcpy.
    char header[header_length + 1] = "";
    sprintf(header, "%4d", static_cast<int>(body_length_));
    memcpy(data_, header, header_length);
  }

  /*
  bool decode_header()
  {
    using namespace std; // For strncat and atoi.
    
    char header[header_length + 1] = "";
    const int l = header_length-(sequence_nr_length+sequence_nr_total_length);
    char bodyLength[l+1] = "";
    char sequeNr[sequence_nr_length +1] = "";
    char sequeNrTotal[sequence_nr_total_length+1] = "";
  
    strncat(bodyLength, data_, l);
    body_length_ = atoi(bodyLength);
    strncat(sequeNr, data_+l, sequence_nr_length);
    squenceNr_ = atoi(sequeNr);
    strncat(sequeNrTotal, data_+l+sequence_nr_length, sequence_nr_total_length);
    sequeNrTotal_ = atoi(sequeNrTotal);
    if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }
    return true;
  }
  
  void encode_header()
  {
    using namespace std; // For sprintf and memcpy.
    char header[header_length + 1] = "";
    const int l = header_length-(sequence_nr_length+sequence_nr_total_length);
    char bodyLength[l+1] = "";
    char sequeNr[sequence_nr_length +1] = "";
    char sequeNrTotal[sequence_nr_total_length+1] = "";
    sprintf(bodyLength, "%4d", static_cast<int>(body_length_));
    sprintf(sequeNr, "%2d", static_cast<int>(squenceNr_));
    sprintf(sequeNrTotal, "%2d", static_cast<int>(sequeNrTotal_));
    
    memcpy(data_, bodyLength, l );
    memcpy(data_+l, sequeNr, sequence_nr_length );
    memcpy(data_+l+sequence_nr_length, sequeNrTotal, sequence_nr_total_length );
    
  }
  
  void setSequenceNr(size_t seqnr , size_t seqNrTotal){
      squenceNr_ = seqnr;
      sequeNrTotal_ = seqNrTotal;
  }
*/
private:
  char data_[header_length + max_body_length];
  size_t body_length_;
  //size_t squenceNr_;
 // size_t sequeNrTotal_;
};

#endif // CHAT_MESSAGE_HPP
