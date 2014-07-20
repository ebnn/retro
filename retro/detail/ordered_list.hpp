/*! \file ordered_list.hpp
 *  \brief Implementation of an ordered list, which is an efficient data
 *         structure similar to a linked list, but enables constant-time
 *         queries about the ordering of two iterators from the list.
 */

#pragma once

#include <list>
#include <iterator>
#include <algorithm>
#include <cmath>

namespace retro
{

namespace detail
{

/*! \brief Represents a list which allows constant-time queries about the
 *  ordering of two iterators.
 *  \p This is an implementation based on Bender[02].
 *
 *  \tparam T The type of elements to store.
 *  \tparam LabelType The integer type used to store labels for each element.
 *                    Using label types with larger ranges improves the speed
 *                    of insertions.
 */
template <class T, class LabelType = unsigned long long int>
class ordered_list
{
  private:
    struct upper_node;
    struct lower_node;

    typedef std::list<upper_node> upper_container;
    typedef std::list<lower_node> lower_container;
    typedef typename upper_container::iterator upper_iterator;
    typedef typename lower_container::iterator lower_iterator;

  public:
    typedef T value_type;
    typedef LabelType label_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef typename lower_container::size_type size_type;

    /*! Bidirectional iterator that traverses the list in order.
     */
    class iterator
      : public std::iterator<std::bidirectional_iterator_tag, T>
    {
      public:
        typedef T* pointer;

        reference operator*()
        {
          return lower->value;
        }

        pointer operator->() const
        {
          return &lower->value;
        }

        iterator &operator++()
        {
          ++lower;
          return *this;
        }

        iterator &operator--()
        {
          --lower;
          return *this;
        }

        bool operator==(const iterator &other) const
        {
          return lower == other.lower;
        }

        bool operator!=(const iterator &other) const
        {
          return !(*this == other);
        }

        bool operator<(const iterator &other) const
        {
          if (lower->upper->label == other.lower->upper->label)
            return lower->label < other.lower->label; // Same sublist
          return lower->upper->label < other.lower->upper->label;
        }

        bool operator>(const iterator &other) const
        {
          return !(*this == other || *this < other);
        }

      private:
        iterator(lower_iterator lower)
          : lower(lower)
        {
        }

        lower_iterator lower;

        friend class ordered_list<T, LabelType>;
    }; // end iterator

    /*! Construct an empty ordered list.
     */
    ordered_list(void);

    /*! Construct a ordered list with default constructed elements.
     *  \param n The number of elements to create.
     */
    explicit ordered_list(size_type n);

    /*! Construct a ordered list from copies of a single value.
     *  \param n The number of elements to create.
     *  \param value The value to fill the list with.
     */
    ordered_list(size_type n, const_reference &value);

    /*! Returns the number of elements in the list.
     */
    size_type size(void) const;

    /*! Returns the maximum number of elements that this container can store.
     */
    size_type max_size(void) const;

    /*! Returns whether the container is empty.
     */
    bool empty(void) const;

    /*! Get an iterator to the beginning of the list.
     */
    iterator begin(void);

    /*! Get an iterator to the end of the list.
     */
    iterator end(void);

    /*! Returns the element at the front of the container.
     *  \return A reference to the first element.
     */
    reference front(void);

    /*! Returns the element at the back of the container.
     *  \return A reference to the last element.
     */
    reference back(void);

    /*! Insert an element into the list before a specified position.
     *  \param it An iterator to the position that this new value comes before.
     *  \param val The value to insert.
     */
    iterator insert(iterator it, const T &val);

    /*! Insert an element to the back of the list. 
     *  \param it An iterator to the position that this new value comes before.
     *  \param val The value to insert.
     */
    void push_back(const T &val);

    void push_front(const T &val);

  private:
    constexpr LabelType M() { return std::numeric_limits<LabelType>::max() / 2; }

    constexpr LabelType LOGM() { return std::log2(M()); }

    constexpr LabelType MSTART() { return M() / 2; }

    constexpr LabelType MSTEP() { return MSTART() / LOGM(); }

    struct upper_node
    {
      upper_node(label_type label)
        : label(label)
      {
      }

      label_type label;
    };

    struct lower_node
    {
      lower_node(upper_iterator upper, const T &value)
        : upper(upper), label(-1), value(value)
      {
      }

      upper_iterator upper;
      label_type label;
      value_type value;
    };

    upper_iterator insert_upper(upper_iterator it);

    void relabel_upper(upper_iterator from, upper_iterator to,
        typename upper_iterator::difference_type n);

    upper_container upper_list;
    lower_container lower_list;
    upper_iterator last_upper_;
}; // end ordered_list

} // end detail

} // end retro

#include <retro/detail/ordered_list.inl>
