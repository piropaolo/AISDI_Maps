#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

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

private:
  struct HashNode
  {
    value_type data;
    HashNode *prev;
    HashNode *next;
    HashNode(key_type k, mapped_type m) : data(std::make_pair(k, m)), prev(nullptr), next(nullptr) {}
    HashNode(key_type k, mapped_type m, HashNode *prev) : HashNode (k, m) { this->prev = prev; }
    ~HashNode() { delete next; }
  };

  HashNode **table;
  size_type SIZE;
  const size_type TABLE_SIZE;

  size_type hashFunction(const key_type& key) const
  {
    return std::hash<key_type>()(key) % TABLE_SIZE;
  }

  void erase()
  {
    for(size_type i=0; i<TABLE_SIZE; ++i)
    {
      delete table[i];
      table[i] = nullptr;
    }
    SIZE = 0;
  }

  HashNode* findNode(const key_type& key) const
  {
    HashNode *node = table[hashFunction(key)];
    while(node != nullptr)
    {
      if(node->data.first == key) return node;
    }
    return node;
  }

  void eraseNode(HashNode* node, const key_type& key)
  {
    if(node->prev == nullptr)
      table[hashFunction(key)] = node->next;
    else
      node->prev->next = node->next;

    if(node->next != nullptr)
      node->next->prev = node->prev;

    node->next = nullptr;
    delete node;
    --SIZE;
  }

  std::pair<HashNode*, size_type> findFirstNode() const
  {
    size_type index = 0;
    HashNode *node = table[index];
    while(node == nullptr && index < TABLE_SIZE)
      node = table[++index];
    return std::make_pair(node, index);
  }

public:
  HashMap() : table(nullptr), SIZE(0), TABLE_SIZE(20)
  {
    table = new HashNode *[TABLE_SIZE]{nullptr};
  }

  ~HashMap()
  {
    /*for (size_type i = 0; i < TABLE_SIZE; ++i)
    {
      HashNode *entry = table[i];
      while (entry != nullptr)
      {
        HashNode *prev = entry;
        entry = entry->next;
        delete prev;
      }
    table[i] = nullptr;
    }*/
    delete [] table;
  }

  HashMap(std::initializer_list<value_type> list) : HashMap()
  {
    for (auto it = list.begin(); it != list.end(); ++it)
      operator[]((*it).first) = (*it).second;
    //(void)list;
    //throw std::runtime_error("TODO");
  }

  HashMap(const HashMap& other) : HashMap()
  {
    *this = other;
    //(void)other;
    //throw std::runtime_error("TODO");
  }

  HashMap(HashMap&& other) : HashMap()
  {
    *this = std::move(other);
    //(void)other;
    //throw std::runtime_error("TODO");
  }

  HashMap& operator=(const HashMap& other)
  {
    if(this != &other)
    {
      erase();
      for (auto it = other.begin(); it != other.end(); ++it)
        operator[]((*it).first) = (*it).second;
    }
    return *this;
    //(void)other;
    //throw std::runtime_error("TODO");
  }

  HashMap& operator=(HashMap&& other)
  {
    if(this != &other) {
      erase();

      table = other.table;
      SIZE = other.SIZE;

      other.table = nullptr;
      other.SIZE = 0;
    }
    return *this;
    //(void)other;
    //throw std::runtime_error("TODO");
  }

  bool isEmpty() const
  {
    return !SIZE;
    //throw std::runtime_error("TODO");
  }

  mapped_type& operator[](const key_type& key)
  {
    size_type hashValue = hashFunction(key);
    HashNode *node = table[hashValue];

    if (node == nullptr)
    {
      table[hashValue] = new HashNode(key, mapped_type());
      node = table[hashValue];
      ++SIZE;
    }
    else if (node->data.first != key)
    {
      while(node->next != nullptr)
      {
        if (node->next->data.first == key)  break;
        else node = node->next;
      }
      if (node->next == nullptr)
      {
        node->next = new HashNode(key, mapped_type(), node);
        ++SIZE;
      }
      node = node->next;
    }
    return node->data.second;
    //(void)key;
    //throw std::runtime_error("TODO");
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    HashNode* node = findNode(key);
    if(node == nullptr)
      throw std::out_of_range("valueOf");
    return node->data.second;
    //(void)key;
    //throw std::runtime_error("TODO");
  }

  mapped_type& valueOf(const key_type& key)
  {
    HashNode* node = findNode(key);
    if(node == nullptr)
      throw std::out_of_range("valueOf");
    return node->data.second;
    //(void)key;
    //throw std::runtime_error("TODO");
  }

  const_iterator find(const key_type& key) const
  {
    return const_iterator(this, findNode(key), hashFunction(key));
    //(void)key;
    //throw std::runtime_error("TODO");
  }

  iterator find(const key_type& key)
  {
    return iterator(this, findNode(key), hashFunction(key));
    //(void)key;
    //throw std::runtime_error("TODO");
  }

  void remove(const key_type& key)
  {
    remove(find(key));
    //(void)key;
    //throw std::runtime_error("TODO");
  }

  void remove(const const_iterator& it)
  {
    if(it.pointee == nullptr)
      throw std::out_of_range("remove");
    eraseNode(it.pointee,it.index);
    //(void)it;
    //throw std::runtime_error("TODO");
  }

  size_type getSize() const
  {
    return SIZE;
    //throw std::runtime_error("TODO");
  }

  bool operator==(const HashMap& other) const
  {
    if(SIZE != other.SIZE)
      return false;

    for(auto it = begin(), it2 = other.begin(); it!=end(); ++it, ++it2)
    {
      if(*it != *it2)
        return false;
    }
    return true;
    //(void)other;
    //throw std::runtime_error("TODO");
  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    auto node = findFirstNode();
    return iterator (this, node.first, node.second);
    //throw std::runtime_error("TODO");
  }

  iterator end()
  {
    iterator it(this);
    return it;
    //throw std::runtime_error("TODO");
  }

  const_iterator cbegin() const
  {
    auto node = findFirstNode();
    return const_iterator (this, node.first, node.second);
    //throw std::runtime_error("TODO");
  }

  const_iterator cend() const
  {
    const_iterator it(this);
    return it;
    //throw std::runtime_error("TODO");
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

private:
  const HashMap *mappu;
  HashNode *pointee;
  size_type index;
  friend void HashMap<KeyType, ValueType>::remove(const const_iterator&);

public:
  explicit ConstIterator(const HashMap *mappu = nullptr, HashNode *pointee = nullptr, size_type index = 0)
  : mappu(mappu), pointee(pointee), index(index)
  {
    if(pointee == nullptr && mappu != nullptr)
      this->index = mappu->TABLE_SIZE;
  }

  ConstIterator(const ConstIterator& other) : ConstIterator(other.mappu, other.pointee, other.index)
  {
    //(void)other;
    //throw std::runtime_error("TODO");
  }

  ConstIterator& operator++()
  {
    if(mappu == nullptr || pointee == nullptr)
      throw std::out_of_range("operator++");
    else if(pointee->next != nullptr){
      pointee = pointee->next;
    }
    else {
      index++;
      while( index < mappu->TABLE_SIZE && mappu->table[index] == nullptr )
        index++;

      if(index >= mappu->TABLE_SIZE )
        pointee = nullptr;
      else
        pointee = mappu->table[index];
    }
    return *this;
    //throw std::runtime_error("TODO");
  }

  ConstIterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
    //throw std::runtime_error("TODO");
  }

  ConstIterator& operator--()
  {
    if(mappu == nullptr)
      throw std::out_of_range("operator--");
    else if(pointee == nullptr || pointee == mappu->table[index]) {
      index--;
      while( index >0 && mappu->table[index] == nullptr )
        index--;

      if(index == 0 && mappu->table[index] == nullptr)
        throw std::out_of_range("operator--");

      pointee = mappu->table[index];
      while (pointee->next != nullptr)
        pointee = pointee->next;
    }
    else{
      pointee = pointee->prev;
    }

    return *this;
    //throw std::runtime_error("TODO");
  }

  ConstIterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
    //throw std::runtime_error("TODO");
  }

  reference operator*() const
  {
    if(pointee == nullptr)
      throw std::out_of_range("operator*");
    return pointee->data;
    //throw std::runtime_error("TODO");
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return mappu == other.mappu && pointee == other.pointee && index == other.index;
    //(void)other;
    //throw std::runtime_error("TODO");
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;

  explicit Iterator(const HashMap *mappu = nullptr, HashNode *pointee = nullptr, size_type index = 0)
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
