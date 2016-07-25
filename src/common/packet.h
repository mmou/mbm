// adapted from m lab packet.cc ????

// Copyright 2013 M-Lab. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//#ifndef _MLAB_PACKET_H_
//#define _MLAB_PACKET_H_

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>

namespace mlab {

// TODO(dominich): Should this take care of htonl/ntohl calls?
class Packet {
 public:
  explicit Packet(const std::vector<uint8_t>& data);
  explicit Packet(const std::string& data);
  Packet(const char* buffer, size_t length);

  template<typename T> Packet(const T& data) {
    const uint8_t* buffer = reinterpret_cast<const uint8_t*>(&data);
    size_t length = sizeof(T) / sizeof(uint8_t);
    data_.assign(buffer, buffer + length);
  }

  std::string str() const {
    return length() == 0 ? std::string() : std::string(buffer(), 0, length());
  }

  const char* buffer() const {
    return length() == 0 ? NULL : reinterpret_cast<const char*>(&data_[0]);
  }
  const size_t length() const { return data_.size(); }

  const std::vector<uint8_t>& data() const { return data_; }

  template<typename T> T as() const {
    return length() == 0 ? T() : *(reinterpret_cast<const T*>(&data_[0]));
  }
 private:
  std::vector<uint8_t> data_;
};

}  // namespace mlab

//#endif  // _MLAB_PACKET_H_

