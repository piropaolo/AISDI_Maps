#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

#include <iostream>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

protected:
  struct HashNode
  {
    value_type value;
    HashNode *next;
    HashNode *prev;
    HashNode(key_type key, mapped_type mapped) : value(std::make_pair(key, mapped)), next(nullptr), prev(nullptr) {}
    HashNode(key_type key, mapped_type mapped, HashNode *prev) : HashNode(key, mapped) { this->prev = prev; }
    HashNode(key_type key, mapped_type mapped, HashNode *next, HashNode *prev) : HashNode(key, mapped) { this->next = next, this->prev = prev; }
    ~HashNode() { delete next; }
  };
  HashNode **table;
  size_type size;
  size_type real_size;

  ///metody pomocnicze

  size_type hashFunction(const key_type& key) const
  {
    return std::hash<key_type>()(key) % real_size;
  }

  void erase()
  {
    if(size) {
      for(size_type i = 0; i < real_size; ++i) {
        delete table[i];
        table[i] = nullptr;
      }
    }
    size = 0;
  }

  void remove(HashNode* node, const key_type& key)
  {
    if(node->prev == nullptr) table[hashFunction(key)] = node->next;
    else  node->prev->next = node->next;

    if(node->next != nullptr) node->next->prev = node->prev;

    node->next = nullptr;
    delete node;
    --size;
  }

  HashNode* getNode(const key_type& key) const
  {
    HashNode *node = table[hashFunction(key)];
    while(node != nullptr)
      if(node->value.first == key) return node;
    return node;
  }

  std::pair<HashNode*, size_type> getFirst() const
  {
    size_type index = 0;

    while(index < real_size && table[index] == nullptr)
      ++index;

    HashNode *node = nullptr;
    if(index < real_size) node = table[index];

    return std::make_pair(node, index);
  }

public:
  HashMap() : table(nullptr), size(0), real_size(1000)
  { table = new HashNode* [real_size]{nullptr}; }

  HashMap(std::initializer_list<value_type> list) : HashMap()
  {
    for (auto it = list.begin(); it != list.end(); ++it)
      operator[]((*it).first) = (*it).second;
  }

  HashMap(const HashMap& other) : HashMap() ///konstruktor kopiujący
  {
    *this = other;
  }

  HashMap(HashMap&& other) : HashMap()  ///konstruktur przenoszący
  {
    *this = std::move(other);
  }

  ~HashMap()
  {
    erase();
    delete[] table;
  }

  HashMap& operator=(const HashMap& other) ///operator przypisania
  {
    if(this != &other) {
      erase();
      for (auto it = other.begin(); it != other.end(); ++it)
        operator[]((*it).first) = (*it).second;
    }
    return *this;
  }

  HashMap& operator=(HashMap&& other) ///przenoszący operator przypisania
  {
    if(this != &other) {
      erase();
      auto temp = table;

      table = other.table;
      size = other.size;

      other.table = temp;
      other.size = 0;
    }
    return *this;
  }

  bool isEmpty() const
  {
    return !size;
  }

  mapped_type& operator[](const key_type& key)
  {
    size_type hashKey = hashFunction(key);
    HashNode *node = table[hashKey];

    if(node == nullptr) {
      table[hashKey] = new HashNode(key, mapped_type());
      node = table[hashKey];
      ++size;
    }
    else if(node->value.first != key){
      while(node->next != nullptr) {
        if(node->next->value.first == key) break;
        else  node = node->next;
      }
      if(node->next == nullptr) {
        node->next = new HashNode(key, mapped_type(), node);
        ++size;
      }
      node = node->next;
    }
    return node->value.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    HashNode* node = getNode(key);
    if(node == nullptr)
      throw std::out_of_range("ValueOf is out of range.");
    return node->value.second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    HashNode* node = getNode(key);
    if(node == nullptr)
      throw std::out_of_range("ValueOf is out of range.");
    return node->value.second;
  }

  const_iterator find(const key_type& key) const
  {
    return const_iterator(this, getNode(key), hashFunction(key));
  }

  iterator find(const key_type& key)
  {
    return iterator(this, getNode(key), hashFunction(key));
  }

  void remove(const key_type& key)
  {
    remove(find(key));
  }

  void remove(const const_iterator& it)
  {
    if(this != it.mappu || it == end())
      throw std::out_of_range("Remove is out of range.");
    remove(it.pointee, it.index);
  }

  size_type getSize() const
  {
    return size;
  }

  bool operator==(const HashMap& other) const
  {
    if(size != other.size)  return false;
    for(auto it = begin(), it2 = other.begin(); it!=end(); ++it, ++it2) {
      if(*it != *it2) return false;
    }
    return true;
  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    auto result = getFirst();
    return iterator (this, result.first, result.second);
  }

  iterator end()
  {
    iterator it(this);
    return it;
  }

  const_iterator cbegin() const
  {
    auto result = getFirst();
    return const_iterator (this, result.first, result.second);
  }

  const_iterator cend() const
  {
    const_iterator it(this);
    return it;
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;

protected:
  const HashMap *mappu;
  HashNode *pointee;
  size_type index;
  friend void HashMap<KeyType, ValueType>::remove(const const_iterator&);

public:
  explicit ConstIterator(const HashMap *mappu = nullptr, HashNode *pointee = nullptr, size_type index = 0)
  : mappu(mappu), pointee(pointee), index(index)
  {
    if(pointee == nullptr && mappu != nullptr)  this->index = mappu->real_size;
  }

  ConstIterator(const ConstIterator& other)
  : ConstIterator(other.mappu, other.pointee, other.index)
  {}

  ConstIterator& operator++()
  {
    if(mappu == nullptr || pointee == nullptr)  throw std::out_of_range("Operator++ is out of range.");
    else if(pointee->next != nullptr){
      pointee = pointee->next;
    }
    else {
      index++;
      while(index < mappu->real_size && mappu->table[index] == nullptr)  index++;
      if(index >= mappu->real_size ) pointee = nullptr;
      else  pointee = mappu->table[index];
    }
    return *this;
  }

  ConstIterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  ConstIterator& operator--()
  {
    if(mappu == nullptr)  throw std::out_of_range("Operator-- is out of range.");
    else if(pointee == nullptr || pointee == mappu->table[index]) {
      index--;
      while(index > 0 && mappu->table[index] == nullptr) index--;
      if(index == 0 && mappu->table[index] == nullptr)  throw std::out_of_range("Operator-- is out of range.");
      pointee = mappu->table[index];
      while (pointee->next != nullptr)  pointee = pointee->next;
    }
    else{
      pointee = pointee->prev;
    }
    return *this;
  }

  ConstIterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  reference operator*() const
  {
    if(pointee == nullptr)  throw std::out_of_range("Operator* is out of range.");
    return pointee->value;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return mappu == other.mappu && pointee == other.pointee && index == other.index;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType> ///zrobione
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;

  explicit Iterator(HashMap *mappu = nullptr, HashNode *pointee = nullptr, size_type index = 0)
  : ConstIterator(mappu, pointee, index)
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_HASHMAP_H */
