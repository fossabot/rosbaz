#pragma once

#include <cstdint>
#include <set>

#include <boost/optional.hpp>
#include <ros/time.h>
#include <rosbag/query.h>
#include <rosbag/structures.h>

#include "rosbaz/bag.h"
#include "rosbaz/query.h"
#include "rosbaz/message_instance.h"

namespace rosbaz
{
class View;

struct MessageRange
{
  MessageRange(std::multiset<rosbag::IndexEntry>::const_iterator _begin,
               std::multiset<rosbag::IndexEntry>::const_iterator _end, const rosbag::ConnectionInfo& _connection_info,
               const BagQuery& _bag_query);

  std::multiset<rosbag::IndexEntry>::const_iterator begin;
  std::multiset<rosbag::IndexEntry>::const_iterator end;
  const rosbag::ConnectionInfo* connection_info;
  const BagQuery* bag_query;
};

struct ViewIterHelper
{
  ViewIterHelper(std::multiset<rosbag::IndexEntry>::const_iterator _iter, const MessageRange& _range);

  std::multiset<rosbag::IndexEntry>::const_iterator iter;
  const MessageRange* range;
};

struct ViewIterHelperCompare
{
  bool operator()(ViewIterHelper const& a, ViewIterHelper const& b);
};

struct View
{
public:
  struct iterator : public std::iterator<std::forward_iterator_tag, MessageInstance>
  {
  public:
    using const_pointer = const MessageInstance*;

  private:
    friend class View;

    iterator(View& view, bool end = false);

    void populate();
    void populateSeek(std::multiset<rosbag::IndexEntry>::const_iterator iter);

    void increment();

  public:
    iterator() = default;
    iterator(iterator const& i);
    iterator& operator=(iterator const& i);

    iterator& operator++();
    iterator operator++(int);

    bool operator==(const iterator& other) const;
    bool operator!=(const iterator& rhs) const;

    value_type operator*() const;
    const_pointer operator->() const;

  protected:
    View* view_{ nullptr };
    std::vector<ViewIterHelper> iters_{};
    uint32_t view_revision_;

    mutable boost::optional<MessageInstance> message_instance_{};
  };

  explicit View(const Bag& bag, const ros::Time& start_time = ros::TIME_MIN, const ros::Time& end_time = ros::TIME_MAX);

  explicit View(const Bag& bag, std::function<bool(rosbag::ConnectionInfo const*)> query,
                const ros::Time& start_time = ros::TIME_MIN, const ros::Time& end_time = ros::TIME_MAX);

  size_t size();

  iterator begin();
  iterator end();

  ros::Time getBeginTime();
  ros::Time getEndTime();

  /// Add a query to a view
  ///
  /// \param[in] bag        The bag file on which to run this query
  /// \param[in] start_time The beginning of the time range for the query
  /// \param[in] end_time   The end of the time range for the query
  void addQuery(const Bag& bag, const ros::Time& start_time = ros::TIME_MIN, const ros::Time& end_time = ros::TIME_MAX);

  /// Add a query to a view
  ///
  /// \param[in] bag        The bag file on which to run this query
  /// \param[in] query      The actual query to evaluate which connections to include
  /// \param[in] start_time The beginning of the time range for the query
  /// \param[in] end_time   The end of the time range for the query
  void addQuery(Bag const& bag, boost::function<bool(rosbag::ConnectionInfo const*)> query,
                const ros::Time& start_time = ros::TIME_MIN, const ros::Time& end_time = ros::TIME_MAX);

  std::vector<const rosbag::ConnectionInfo*> getConnections() const;

private:
  void updateQueries(BagQuery& q);
  void update();

  std::vector<MessageRange> m_ranges{};

  std::vector<BagQuery> m_queries{};
  uint32_t m_view_revision = 0;

  uint32_t m_size_cache = 0;
  uint32_t m_size_revision = 0;
};
}  // namespace rosbaz