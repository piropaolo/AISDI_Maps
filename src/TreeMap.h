#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
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
  struct Node
  {
    value_type value;
    Node *left, *right, *parent;
    int height;
    Node(key_type key, mapped_type mapped)
      : value(std::make_pair(key, mapped)), left(nullptr), right(nullptr), parent(nullptr), height(1) {}
    Node(value_type it) : Node(it.first,it.second) {}
    ~Node() { delete left; delete right; }
  };
  Node* root;
  size_type size;

  ///metody pomocnicze

  void erase()
  {
    delete root;
    root = nullptr;
    size = 0;
  }

  void insert(Node* node)
  {
    if (root == nullptr) root = node;
    else {
      Node* temp = root;
      while (true) {
        if (node->value.first > temp->value.first) {
          if (temp->right == nullptr) {
            temp->right = node;
            node->parent = temp;
            break;
          }
          else  temp = temp->right;
        }
        else if (node->value.first < temp->value.first) {
          if (temp->left == nullptr) {
            temp->left = node;
            node->parent = temp;
            break;
          }
          else  temp = temp->left;
        }
        else {
          delete node;
          return;
        }
      }
    }
    ++size;
    return;
  }

  void remove(Node* node)
  {
    if (node->left == nullptr)  change(node, node->right); ///jeśli nic nie ma po lewej to wstawiamy prawe poddrzewo
    else if (node->right == nullptr)  change(node, node->left); ///jeśli nie ma nic po prawej to wstawiamy lewe poddrzewo
    else {  ///dwójka dzieci
      auto temp = node->right;
      while (temp->left != nullptr) temp = temp->left; ///raz w prawo, do końca w lewo
      change(temp, temp->right);
      change(node, temp);
    }
    --size;
    delete node;
  }

  void change(Node* kono, Node* sono)
  {
    if (kono->parent == nullptr)  root = sono;
    else if (kono == kono->parent->left)  kono->parent->left = sono;
    else  kono->parent->right = sono;

    if (sono != nullptr) {
      sono->parent = kono->parent;
      if(kono->right != nullptr && kono->right != sono) sono->right = kono->right;
      if(kono->left != nullptr && kono->left != sono) sono->left = kono->left;
    }
    kono->parent = kono->left   = kono->right  = nullptr;
  }

  Node* getNode(const key_type& key) const
  {
    Node* node = root;
    while (node != nullptr) {
      if (key > node->value.first)  node = node->right;
      else if (key < node->value.first) node = node->left;
      else  break;
    }
    return node;
  }

  Node* getFirst(Node* node) const
  {
    if(node != nullptr)
      while(node->left != nullptr)
        node = node->left;
    return node;
  }

public:
  TreeMap() : root(nullptr), size(0) {}

  TreeMap(std::initializer_list<value_type> list) : TreeMap()
  {
    for (auto it = list.begin(); it != list.end(); ++it)
      insert(new Node(*it));
  }

  TreeMap(const TreeMap& other) : TreeMap() ///konstruktor kopiujący
  {
    *this = other;
  }

  TreeMap(TreeMap&& other) : TreeMap()  ///konstruktor przenoszący
  {
    *this = std::move(other);
  }

  ~TreeMap()
  {
    erase();
  }

  TreeMap& operator=(const TreeMap& other)  ///operator przypisania
  {
    if(this != &other) {
        erase();
        for (auto it = other.begin(); it != other.end(); ++it)
          insert(new Node(*it));
    }
    return *this;
  }

  TreeMap& operator=(TreeMap&& other) ///przenoszący operator przypisania
  {
    if(this != &other) {
      erase();

      root = other.root;
      size = other.size;

      other.root = nullptr;
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
    Node* temp = getNode(key);
    if( temp == nullptr ) {
      temp = new Node(key, mapped_type());
      insert(temp);
    }
    return temp->value.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    const Node* temp = getNode(key);
    if(temp == nullptr)  throw std::out_of_range("ValueOf is out of range.");
    return temp->value.second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    Node* temp = getNode(key);
    if(temp == nullptr)  throw std::out_of_range("ValueOf is out of range.");
    return temp->value.second;
  }

  const_iterator find(const key_type& key) const
  {
    return const_iterator(this, getNode(key));
  }

  iterator find(const key_type& key)
  {
    return iterator(this, getNode(key));
  }

  void remove(const key_type& key)
  {
    remove(find(key));
  }

  void remove(const const_iterator& it)
  {
    if(this != it.tree || it == end())  throw std::out_of_range ("Remove is out of range.");
    remove(it.pointee);
  }

  size_type getSize() const
  {
    return size;
  }

  bool operator==(const TreeMap& other) const
  {
    if(size != other.size)  return false;
    for(auto it = begin(), it2 = other.begin(); it != end(); ++it, ++it2) {
      if(*it != *it2) return false;
    }
    return true;
  }

  bool operator!=(const TreeMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    return iterator(this, getFirst(root));
  }

  iterator end()
  {
    return iterator(this);
  }

  const_iterator cbegin() const
  {
    return const_iterator(this, getFirst(root));
  }

  const_iterator cend() const
  {
    return const_iterator(this);
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

template <typename KeyType, typename ValueType> ///operatory do poprawy
class TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;

  using pointer = const typename TreeMap::value_type*;

protected:
  const TreeMap *tree;
  Node *pointee;
  friend void TreeMap<KeyType, ValueType>::remove(const const_iterator&);

public:
  explicit ConstIterator(const TreeMap *tree = nullptr, Node *pointee = nullptr)
  : tree(tree), pointee(pointee) {}

  ConstIterator(const ConstIterator& other)
  : ConstIterator(other.tree, other.pointee) {}

  ConstIterator& operator++()
  {
    if(pointee == nullptr || tree == nullptr) {
      throw std::out_of_range("Operator++ is out of range.");
    }
    else if( pointee->right != nullptr ) {
      pointee = pointee->right;
      while(pointee->left != nullptr)
        pointee = pointee->left;
    }
    else {
      while(true) {
        if(pointee->parent == nullptr) {
          pointee = nullptr;
          break;
        }
        if(pointee->parent->left == pointee) {
          pointee = pointee->parent;
          break;
        }
        pointee = pointee->parent;
      }
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
    if(tree == nullptr || tree->root == nullptr) {
      throw std::out_of_range("Operator-- is out of range.");
    }
    else if(pointee == nullptr) {
      pointee = tree->root;
      while(pointee->right != nullptr)
        pointee = pointee->right;
    }
    else if(pointee->left != nullptr) {
      pointee = pointee->left;
      while(pointee->right != nullptr)
        pointee = pointee->right;
    }
    else {
      while(true) {
        if(pointee->parent == nullptr) {
          throw std::out_of_range("Operator-- is out of range.");
        }
        if(pointee->parent->right == pointee) {
          pointee = pointee->parent;
          break;
        }
        pointee = pointee->parent;
      }
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
    if(pointee == nullptr || tree == nullptr)  throw std::out_of_range("Operator* is out of range.");
    return pointee->value;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return tree == other.tree && pointee == other.pointee;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator ///zrobione
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

  explicit Iterator(TreeMap *tree = nullptr, Node *pointee = nullptr)
  : ConstIterator(tree, pointee) {}

  Iterator(const ConstIterator& other)
  : ConstIterator(other) {}

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

#endif /* AISDI_MAPS_MAP_H */
