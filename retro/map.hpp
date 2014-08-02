/*! \file map.hpp
 *  \brief Implementation of a fully retroactive associative map.
 */

#pragma once

#include <list>
#include <map>
#include <set>

#include "retro/detail/ordered_list.hpp"

namespace retro
{

/*! \brief Specifies the operations available for a map.
 */
enum class map
{
  /*! \brief Represents inserting an element into the map.
   */
  insert,

  /*! \brief Represents deleting an element from the map.
   */
  erase
};

namespace detail
{
  //! Checks if a particular key is in the map at the present.
  template <class MapIterator>
  bool key_exists(MapIterator map_it);

  //! Checks if a particular key is in the map at some previous time point.
  template <class MapIterator, class EventIterator>
  bool key_exists(MapIterator map_it, EventIterator event_it);

  template <class Compare, class T>
  struct comp_wrapper
  {
    comp_wrapper(const Compare &comp)
      : comp(comp) { }

    bool operator()(const T &a, const T &b) const
    {
      return comp(a, b);
    }

    Compare comp;
  };
} // end detail

/*! \brief Represents a fully retroactive ordered associative map.
 */
template <class Key, class T, class Compare = std::less<Key>>
class full_map
{
  private:
    struct event;

    typedef std::list<std::pair<Key, T>> data_container;
    typedef typename data_container::iterator data_iterator;

    typedef detail::ordered_list<event> event_container;
    typedef typename event_container::iterator event_iterator;

    typedef std::map<std::reference_wrapper<const Key>,
                     std::set<event_iterator>,
                     detail::comp_wrapper<Compare, Key>> map_container;
    typedef typename map_container::iterator map_iterator;

  public:
    typedef Key key_type;
    typedef T mapped_type;
    typedef std::pair<const key_type, mapped_type> value_type;
    typedef Compare key_compare;
    typedef typename map_container::size_type size_type;

    /*! Represents an operation performed on the data structure at some point
     *  in time.
     */
    class time_point
    {
      public:
        /*! Get the operation that was performed.
         */
        map operation() const { return op; }

      private:
        time_point(event_iterator event)
          : event(event)
        {
        }

        time_point(map op, event_iterator event)
          : op(op), event(event)
        {
        }

        map op;
        event_iterator event;

        friend class full_map<Key, T, Compare>;
    };

    class iterator
      : public std::iterator<std::bidirectional_iterator_tag, value_type>
    {
      public:
        typedef value_type& reference;
        typedef data_iterator pointer;

        reference operator*()
        {
          return (*std::prev(base->second.end()))->data;
        }

        pointer operator->()
        {
          return (*std::prev(base->second.end()))->data;
        }

        iterator &operator++()
        {
          // Find the next key that has not been erased.
          do { ++base; }
          while (base != last && !key_exists(base));

          return *this;
        }

        iterator operator++(int)
        {
          iterator old = *this;
          ++(*this);
          return old;
        }

        iterator &operator--()
        {
          // Find the next key that has not been erased.
          do { --base; }
          while (!key_exists(base));
          return *this;
        }

        iterator operator--(int)
        {
          iterator old = *this;
          --(*this);
          return old;
        }

        bool operator==(const iterator &other) const
        {
          return base == other.base;
        }

        bool operator!=(const iterator &other) const
        {
          return !(*this == other);
        }

      private:
        iterator(map_iterator last, map_iterator base)
          : last(last), base(base)
        {
          // Make sure that this iterator points to a valid element.
          if (base != last && !key_exists(base)) ++(*this);
        }

        map_iterator last;
        map_iterator base;

        friend class full_map<Key, T, Compare>;
    };

    class retro_iterator
      : public std::iterator<std::bidirectional_iterator_tag, value_type>
    {
      public:
        typedef value_type& reference;
        typedef data_iterator pointer;

        reference operator*()
        {
          return cur->data;
        }

        pointer operator->()
        {
          return cur->data;
        }

        retro_iterator operator++()
        {
          // Find the next key that has not been erased.
          do { ++base; }
          while (base != last && !key_exists(base, event));
          if (base != last) cur = *std::prev(base->second.lower_bound(event));
          return *this;
        }

        retro_iterator operator--()
        {
          // Find the next key that has not been erased.
          do { --base; }
          while (!key_exists(base, event));
          if (base != last) cur = *std::prev(base->second.lower_bound(event));
          return *this;
        }

        bool operator==(const retro_iterator &other) const
        {
          return base == other.base && event == other.event;
        }

        bool operator!=(const retro_iterator &other) const
        {
          return !(*this == other);
        }

      private:
        retro_iterator(map_iterator last, map_iterator base,
            event_iterator event)
          : last(last), base(base), event(event), cur(event)
        {
          if (base != last)
          {
            // Iterate to the first valid key
            if (!key_exists(base, event)) ++(*this);
            cur = *std::prev(base->second.lower_bound(event));
          }
        }

        map_iterator last;
        map_iterator base;
        event_iterator event;
        event_iterator cur;

        friend class full_map<Key, T, Compare>;
    };

    /*! Construct an empty fully retroactive map.
     */
    explicit full_map(const key_compare &comp = key_compare());

    /*! Copy an existing map.
     */
    full_map(const full_map &other) = default;

    /*! Construct a map by acquiring the state of an existing map.
     */
    full_map(full_map&& other);

    /*! Return the number of elements in the container at present.
     */
    size_type size(void) const;

    /*! Return the number of elements in the container just before some time
     *  point.
     *  \param t The time point to query.
     */
    size_type size(const time_point &t) const;

    /*! Return the maximum number of elements that this container can store.
     */
    size_type max_size(void) const;

    /*! Return whether the container is empty at present.
     */
    bool empty(void) const;

    /*! Return whether the container is empty just before some time point.
     *  \param t The time point to query.
     */
    bool empty(const time_point &t) const;

    /*! Return an iterator referring to the first element in the container at
     *  present.
     */
    iterator begin(void);

    /*! Return an iterator referring to the first element in the container just
     *  before some time point.
     *  \param t The time point to query.
     */
    retro_iterator begin(const time_point &t);

    /*! Return an iterator referring to the past-the-end element in the
     *  container at present.
     */
    iterator end(void);

    /*! Return an iterator referring to the past-the-end element in the
     *  container just before some time point.
     *  \param t The time point to query.
     */
    retro_iterator end(const time_point &t);

    /*! Insert a new element into the container in its present state.
     *  The new element is not inserted if the key already exists.
     *  \param val The new value to insert.
     */
    time_point insert(const value_type &val);

    /*! Retroactively insert a new element into the container in just before
     *  some time point.
     *
     *  The new element is not inserted if the key already existed just before
     *  the specified time point.
     *
     *  \param t The time point of the operation just before this new one.
     *  \param val The new value to insert.
     *  \return A new time point representing this retroactive operation.
     */
    time_point insert(const time_point &t, const value_type &val);

    /*! Search the container for a specific element in its present state.
     *  \param key The key of the element to search for.
     *  \return An iterator to the element if it is found, or full_map::end()
     *          otherwise.
     */
    iterator find(const key_type &key);

    /*! Search the container for a specific element just before some time point.
     *  \param t The time point to query.
     *  \param key The key of the element to search for.
     *  \return An iterator to the element if it is found, or full_map::end(t)
     *          otherwise.
     */
    retro_iterator find(const time_point &t, const key_type &key);
  
  private:
    struct event
    {
      event()
      {
      }

      event(map op, data_iterator data)
        : op(op), data(data)
      {
      }

      map op;
      data_iterator data;
    };

    data_container data_;
    event_container events_;
    map_container map_;
}; // end full_map

} // end retro

#include "retro/map.inl"
