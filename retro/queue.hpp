/*! \file queue.hpp
 *  \brief Implementation of a partially retroactive queue.
 */

#pragma once

#include <list>
#include <utility>

namespace retro
{

/*! \brief Specifies the operations available for a queue.
 */
struct queue
{
  enum
  {
    /*! \brief Represents pushing (enqueuing) an element into the queue.
     */
    push,

    /*! \brief Represents popping (dequeuing) an element from the queue.
     */
    pop
  };
};

/*! \brief Represents a partially retroactive queue.
 *  \p This container allows similar functionality to a STL queue, except that
 *     push and pop operations can be performed retroactively (in the past).
 *     Past operations can also be reverted. However, queries can only be
 *     performed on the current state of the queue.
 *
 *  \tparam The type of elements to store in the container.
 */
template <class T>
class partial_queue
{
  public:
    typedef T value_type;
    typedef std::list<value_type> container_type;
    typedef typename container_type::reference reference;
    typedef typename container_type::const_reference const_reference;
    typedef typename container_type::size_type size_type;
    typedef std::list<std::pair<value_type, bool>> inner_container_type;
    typedef typename inner_container_type::iterator inner_iterator;

    /*! Represents an operation performed at some point in time.
     *  \tparam Op The operation that was performed (one of the operations in
     *             retro::queue).
     */
    template <std::size_t Op> 
    class time_point
    {
      private:
        time_point(const inner_iterator &it)
          : it(it)
        {
        }

        time_point(inner_iterator&& it)
          : it(std::move(it))
        {
        }

        // Iterator to the element in the linked list that represents
        // the value that was pushed/popped by this operation.
        inner_iterator it;

        friend class partial_queue<T>;
    };

    /*! Construct an empty partially retroactive queue.
     */
    partial_queue(void)
      : size_(0), front_(data_.begin())
    {
    }

    /*! Copy an existing queue.
     */
    partial_queue(const partial_queue &other) = default;

    /*! Acquire the state of an existing queue by moving it.
     */
    partial_queue(partial_queue&& other)
      : size_(other.size_), front_(other.front_), data_(std::move(other.data_))
    {
    }

    /*! Returns the number of elements in the container at present.
     */
    size_type size(void) const
    {
      return size_;
    }

    /*! Returns the maximum number of elements that this container can store.
     */
    size_type max_size(void) const
    {
      return data_.max_size();
    }

    /*! Return whether the container is empty.
     */
    bool empty(void) const
    {
      return size() == 0;
    }

    /*! Returns the element at the front of the container at present.
     *  \return A reference to the first (oldest) element.
     */
    reference front()
    {
      return front_->first;
    }

    /*! Returns the element at the front of the container at present.
     *  \return A const reference to the first (oldest) element.
     */
    const_reference front() const
    {
      return front();
    }

    /*! Returns the element at the back of the queue at present.
     *  \return A reference to the last (newest) element.
     */
    reference back()
    {
      return data_.back().first;
    }

    /*! Returns the element at the back of the queue at present.
     *  \return A const reference to the last (newest) element.
     */
    const_reference back() const
    {
      return back();
    }

    /*! Insert an element to the end of the queue in its present state
     *         by moving it.
     *  \brief val The value to move.
     *  \return A new time point representing this operation.
     */
    time_point<queue::push> push(T&& val)
    {
      // Add the new element to the back of the queue.
      data_.emplace_back(std::move(val), false); ++size_;

      // Find an iterator to the latest time point.
      inner_iterator last = std::prev(data_.end());

      // Set front to be the first (and only) element if the queue only
      // contains one element.
      if (front_ == data_.end()) front_ = last;

      return time_point<queue::push>(std::move(last));
    }

    /*! Insert an element to the end of the queue in its present state.
     *  \param val The value to insert.
     *  \return A new time point representing this operation.
     */
    time_point<queue::push> push(const T &val)
    {
      return push(val);
    }

    /*! Retroactively insert an element to the end of the queue before a
     *         previous time point by moving it.
     *  \param t The time point of the operation just before this one.
     *  \param val The value to move.
     *  \return A new time point representing this retroactive operation.
     */
    template <std::size_t Op>
    time_point<queue::push> push(const time_point<Op> &t, T&& val)
    {
      inner_iterator it = t.it;
      if (it == data_.begin())
      {
        // Inserting before the first element is a special case
        data_.push_front(std::make_pair(std::move(val), true)); 
        move_front_pred();
      }
      else if (it == front_)
      {
        // Inserting before the front is also a special case
        data_.insert(front_, std::make_pair(std::move(val), false));
      }
      else
      {
        // Whether this element is before the front pointer is determined
        // by the element before this one.
        inner_iterator before = std::prev(it);
        data_.insert(it, std::make_pair(std::move(val), before->second));
      }

      // Point iterator to the newly inserted element.
      --it; ++size_;

      // Move the front to the left if this operation is before the front
      // because we pushed an element that should've been popped before.
      if (it->second) move_front_pred();

      // Return an iterator to the new operation.
      return time_point<queue::push>(std::move(it));
    }

    /*! Retroactively insert an element to the end of the queue before a
     *         previous time point.
     *  \param t The time point of the operation just before this one.
     *  \param val The value to insert.
     *  \return A new time point representing this retroactive operation.
     */
    template <std::size_t Op>
    time_point<queue::push> push(const time_point<Op> &t, const T &val)
    {
      return push(t, val);
    }

    /*! Pop an element from the front of the queue in its present state.
     *  \return A new time point representing this operation.
     */
    time_point<queue::pop> pop(void)
    {
      --size_;

      time_point<queue::pop> old(front_); 
      move_front_succ();
      return old;
    }

    /*! Retroactively pop an element from the front of the queue before
     *         a previous time point.
     *  \param t The time point of the operation just before this one.
     *  \return A new time point representing this operation.
     */
    template <std::size_t Op>
    time_point<queue::pop> pop(const time_point<Op> &)
    {
      return pop();
    }

    /*! Swap the contents of this queue with another.
     *  \param other The queue to swap with.
     */
    void swap(partial_queue &other)
    {
      std::swap(size_, other.size_);
      std::swap(front_, other.front_);
      std::swap(data_, other.data_);
    }

    /*! Retroactively revert a previous push operation.
     *  \param t The time point to revert.
     */
    void revert(const time_point<queue::push> &t)
    {
      // This element was never pushed.
      --size_;

      // If t occurs before front, then that means we've previously popped an
      // element that no longer exists, so it should have popped the element
      // after it. Hence, move the front to its successor.
      if (t.it->second)
      {
        move_front_succ();
      }
      else if (front_ == t.it)
      {
        // We're removing the front, so move it to the right to ensure validity.
        move_front_succ();
      }

      // Remove the element corresponding to the push.
      data_.erase(t.it);
    }

    /*! Retroactively revert a previous pop operation.
     *  \param t The time point to revert.
     */
    void revert(const time_point<queue::pop> &t)
    {
      move_front_pred();
      ++size_; // One less pop means the size increases by 1
    }

  private:
    void move_front_succ(void)
    {
      // When moving the front pointer to the right, the current front
      // would be 'before' the new front.
      front_->second = true; ++front_;
      if (front_ != data_.end()) front_->second = false;
    }

    void move_front_pred(void)
    {
      // When moving the front pointer to the left, the element before
      // the current front would no longer be 'before' the new front
      // (as it will *be* the front).
      front_->second = false; --front_;
      if (front_ != data_.end()) front_->second = false;
    }

    size_type size_;
    inner_container_type data_;
    inner_iterator front_;

}; // end partial_queue

} // end retro
