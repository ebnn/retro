namespace retro
{

namespace detail
{

template <class T, class LabelType>
  ordered_list<T, LabelType>
    ::ordered_list(void)
{
  // The upper list has sentinel nodes at the beginning and end of the list.
  // Both containly solely the before-the-start and past-the-end lower nodes
  // respectively.
  upper_.push_back(upper_node(0)); 
  upper_.push_back(upper_node(M() - 1));
  last_upper_ = std::prev(upper_.end());

  // Create sentinels for the before-the-start and past-the-end lower nodes.
  lower_.push_back(lower_node(upper_.begin(), 0));
  lower_.push_back(lower_node(last_upper_, M() - 1));
  last_lower_ = std::prev(lower_.end());

  // Create one more pair of sentinel upper and lower nodes in the middle 
  // to represent the "root" of the list. The first real element inserted into
  // the list will link to the upper root.
  root_ = lower_.insert(last_lower_,
                        lower_node(insert_upper(upper_.begin()), MSTART()));
}

template <class T, class LabelType>
  typename ordered_list<T, LabelType>::size_type
    ordered_list<T, LabelType>
      ::size(void) const
{
  return lower_.size() - 3; // Don't count the sentinels and root.
}

template <class T, class LabelType>
  bool ordered_list<T, LabelType>
    ::empty() const
{
  return size() == 0;
}

template <class T, class LabelType>
  typename ordered_list<T, LabelType>::size_type
    ordered_list<T, LabelType>
      ::max_size(void) const
{
  return std::min((label_type)lower_.max_size(), // Size of linked list
                  (label_type)((M() - 1) * LOGM())); // Size of label universe 
}

template <class T, class LabelType>
  typename ordered_list<T, LabelType>::iterator
    ordered_list<T, LabelType>
      ::begin(void)
{
  return iterator(std::next(root_));
}

template <class T, class LabelType>
  typename ordered_list<T, LabelType>::iterator
    ordered_list<T, LabelType>
      ::end(void)
{
  return iterator(last_lower_);
}

template <class T, class LabelType>
  typename ordered_list<T, LabelType>::reference
    ordered_list<T, LabelType>
      ::front(void)
{
  return *begin();
}

template <class T, class LabelType>
  typename ordered_list<T, LabelType>::reference
    ordered_list<T, LabelType>
      ::back(void)
{
  return *std::prev(end());
}

template <class T, class LabelType>
  typename ordered_list<T, LabelType>::iterator
    ordered_list<T, LabelType>
      ::insert(iterator it, const T &val)
{
  // Get the iterators to the current node (the node we are inserting before)
  // and the node's predecessor (this is guaranteed to exist because of the
  // sentinel).
  lower_iterator cur = it.lower;
  lower_iterator prev = std::prev(cur);

  // Get the upper node which this new node will link to.
  upper_iterator upper = prev->upper;

  // Create a new lower node just before cur.
  lower_iterator result = lower_.insert(cur, lower_node(upper, 0, val));

  // Check if we can give the new lower node a label
  if (prev->label + 1 >= cur->label)
  {
    // There is no more available labels left for this node, so we have
    // to relabel the sublist by dividing it into several sublists.
 
    // Find the exclusive boundaries of the sublist (all nodes who have the
    // same upper node as prev). Note that no bounds checking is required
    // because of the sentinels at the start and end of the lower list.
    lower_iterator begin = std::prev(prev);
    while (begin->upper == upper) --begin;

    lower_iterator end = cur;
    while (end->upper == upper) ++end;

    // Redistribute nodes starting at the first node in the sublist.
    cur = std::next(begin);

    while (true)
    {
      label_type label = MSTART();
      for (label_type j = 0; j < LOGM(); ++j, ++cur, label += MSTEP())
      {
        if (cur == end)
          return result; // We've finished relabeling the sublist

        cur->label = label;
        cur->upper = upper;
      }

      upper = insert_upper(upper);
    }
  }
  else
  {
    result->label = (cur->label + prev->label) / 2;
    return result;
  }
}

template <class T, class LabelType>
  void ordered_list<T, LabelType>
    ::push_back(const T &val)
{
  insert(end(), val);
}

template <class T, class LabelType>
  void ordered_list<T, LabelType>
    ::push_front(const T &val)
{
  insert(begin(), val);
}

template <class T, class LabelType>
  typename ordered_list<T, LabelType>::upper_iterator
    ordered_list<T, LabelType>
      ::insert_upper(upper_iterator it)
{
  upper_iterator cur = std::next(it);

  // Find all the nodes that need to be relabelled.
  label_type n = 1;
  label_type start_label = it->label;
  while (cur != last_upper_ && cur->label - start_label <= n * n)
  {
    ++n; ++cur;
  }

  // Relabel these nodes.
  if (!relabel_upper(it, cur, n))
  {
    // Relabeling was not successful, need to rebuild entire upper list.
    relabel_upper(upper_.begin(), upper_.end(), upper_.size());
  }

  // The label of the new node is the mean of the two adjacent to it.
  start_label = it->label;
  ++it;
  return upper_.insert(it, upper_node((start_label + it->label) / 2));
}

template <class T, class LabelType>
  bool ordered_list<T, LabelType>
    ::relabel_upper(upper_iterator from, upper_iterator to,
      typename upper_iterator::difference_type n)
{
  label_type gap = (to->label - from->label) / n;
  if (gap == (label_type)1) return false;

  // Relabel the sequence as arithmetic sequence starting at the label of
  // the first node and incrementing by 'gap' per node.
  for (label_type label = from->label; n--; label += gap, ++from)
    from->label = label;
  return true;
}

}

}
