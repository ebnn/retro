namespace retro
{

namespace detail
{

template <class T, class LabelType>
  ordered_list<T, LabelType>
    ::ordered_list(void)
{
  // Create the root upper node and a sentinel for the end of the upper list
  upper_list.push_back(upper_node(0)); 
  upper_list.push_back(upper_node(M() - 1));
  last_upper_ = std::prev(upper_list.end());
}

template <class T, class LabelType>
  typename ordered_list<T, LabelType>::size_type
    ordered_list<T, LabelType>
      ::size(void) const
{
  return lower_list.size();
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
  return std::min((label_type)lower_list.max_size(), (label_type)((M() - 1) * LOGM()));
}

template <class T, class LabelType>
  typename ordered_list<T, LabelType>::iterator
    ordered_list<T, LabelType>
      ::begin(void)
{
  return iterator(lower_list.begin());
}

template <class T, class LabelType>
  typename ordered_list<T, LabelType>::iterator
    ordered_list<T, LabelType>
      ::end(void)
{
  return iterator(lower_list.end());
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
  bool is_end = (it == end());

  lower_iterator cur = is_end ? lower_list.end() : it.lower;
  upper_iterator cur_upper = is_end ? std::prev(last_upper_) : cur->upper;
  label_type cur_label = is_end ? M() - 1 : cur->label;

  // If there's a node before cur, get its label.
  label_type prev_label = (cur == lower_list.begin()) ? 0 : std::prev(cur)->label;

  // Create a new lower node just before cur
  lower_iterator result = lower_list.insert(cur, lower_node(cur_upper, val));

  // Check if we can give the new lower node a label
  if (prev_label + 1 >= cur_label)
  {
    // There is no more available labels left for this node, so we have
    // to relabel the sublist by dividing it into several sublists.

    // Predicate to find the boundaries of the sublist
    auto pred = [&cur_upper](const lower_node &node) -> bool
    { return node.upper == cur_upper; };

    // Find the boundaries of the sublist
    auto begin = std::find_if_not(std::reverse_iterator<lower_iterator>(cur),
        lower_list.rend(), pred);
    auto end = std::find_if_not(cur, lower_list.end(), pred);

    // Redistribute nodes
    cur = begin.base();

    while (true)
    {
      label_type label = MSTART();
      for (label_type j = 0; j < LOGM(); ++j, ++cur, label += MSTEP())
      {
        if (cur == end)
          return result; // We've finished relabeling the sublist

        cur->label = label;
        cur->upper = cur_upper;
      }

      cur_upper = insert_upper(cur_upper);
    }
  }
  else
  {
    result->label = cur_label / 2 + prev_label / 2;
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
  label_type n = 1;
  upper_iterator cur = std::next(it);

  label_type v0 = it->label;

  while (cur != last_upper_ && cur->label - v0 <= n * n)
  {
    ++n; ++cur;
  }

  if (n > 0)
    relabel_upper(it, cur, n);

  // TODO: check this more concisely
  ++it;
  label_type new_label = v0 / 2 + it->label / 2;
  if (v0 + 1 >= new_label || new_label + 1 >= it->label)
  {
    relabel_upper(upper_list.begin(), last_upper_, upper_list.size() - 1);
  }

  return upper_list.insert(it, upper_node(v0 / 2 + it->label / 2));
}

template <class T, class LabelType>
  void ordered_list<T, LabelType>
    ::relabel_upper(upper_iterator from, upper_iterator to,
      typename upper_iterator::difference_type n)
{
  label_type gap = (to->label - from->label) / n;

  // Relabel the sequence as arithmetic sequence starting at the label of
  // the first node and incrementing by 'gap' per node.
  label_type label = from->label;
  for (typename upper_iterator::difference_type k = 0; k < n; ++k, label += gap, ++from)
    from->label = label;
}

}

}
