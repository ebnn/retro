namespace retro
{

template <class Key, class T, class Compare>
  full_map<Key, T, Compare>::full_map(const key_compare &comp)
    : map_(detail::comp_wrapper<key_compare, Key>(comp))
{
}

template <class Key, class T, class Compare>
  typename full_map<Key, T, Compare>::iterator
    full_map<Key, T, Compare>::begin(void)
{
  return iterator(map_.end(), map_.begin());
}

template <class Key, class T, class Compare>
  typename full_map<Key, T, Compare>::retro_iterator
    full_map<Key, T, Compare>::begin(const time_point &t)
{
  return retro_iterator(map_.end(), map_.begin(), t.event);
}

template <class Key, class T, class Compare>
  typename full_map<Key, T, Compare>::iterator
    full_map<Key, T, Compare>::end(void)
{
  return iterator(map_.end(), map_.end());
}

template <class Key, class T, class Compare>
  typename full_map<Key, T, Compare>::retro_iterator
    full_map<Key, T, Compare>::end(const time_point &t)
{
  return retro_iterator(map_.end(), map_.end(), t.event);
}

template <class Key, class T, class Compare>
  typename full_map<Key, T, Compare>::time_point
    full_map<Key, T, Compare>::insert(const value_type &val)
{
  // Insert this value into the data map because even if this key already
  // exists, it may be used if the previous insert for this key is revoked.
  auto data_it = data_.insert(data_.end(), val);

  // Insert this event in the ordered list
  auto event_it = events_.insert(events_.end(), event(map::insert, data_it));

  // Reference this event in the event map (insert to the end of the set).
  map_[val.first].insert(event_it);

  return time_point(map::insert, event_it);
}

template <class Key, class T, class Compare>
  typename full_map<Key, T, Compare>::time_point
    full_map<Key, T, Compare>::insert(const time_point &t, const value_type &val)
{
  // Insert this value into the data map because even if this key already
  // exists, it may be used if the previous insert for this key is revoked.
  auto data_it = data_.insert(data_.end(), val);

  // Insert this event in the ordered list
  auto event_it = events_.insert(t.event, event(map::insert, data_it));

  // Reference this event in the event map. 
  map_[val.first].insert(event_it);

  return time_point(map::insert, event_it);
}

template <class Key, class T, class Compare>
  typename full_map<Key, T, Compare>::iterator
    full_map<Key, T, Compare>::find(const key_type &key)
{
  auto it = map_.find(key);
  if (it != map_.end() && detail::key_exists(it))
    return iterator(map_.end(), it);
  return end();
}

template <class Key, class T, class Compare>
  typename full_map<Key, T, Compare>::retro_iterator
    full_map<Key, T, Compare>::find(const time_point &t, const key_type &key)
{
  auto it = map_.find(key);
  if (it != map_.end() && key_exists(it, t.event))
    return retro_iterator(map_.end(), it, t.event);
  return end(t);
}

namespace detail
{
  template <class MapIterator>
  bool key_exists(MapIterator map_it)
  {
    return map_it->second.size() > 0 &&
           (*std::prev(map_it->second.end()))->op == map::insert;
  }

  template <class MapIterator, class EventIterator>
  bool key_exists(MapIterator map_it, EventIterator event_it)
  {
    auto it = map_it->second.lower_bound(event_it);
    return it != map_it->second.begin() && // There exists a predecessor
           (*std::prev(it))->op == map::insert; // Predecessor is an insert
  }
} // end detail

} // end retro
