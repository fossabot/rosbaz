#include "rosbaz/blob_url.h"

#include "ros/console.h"

#include <regex>

namespace rosbaz {

AzBlobUrl AzBlobUrl::parse(const std::string& url) {
  std::regex url_rgx("^(https*)://(.+?)\\.([^/]+)/([^/]+)/(.+?)(\\?.+)?$");

  std::smatch matches;

  if (!std::regex_search(url, matches, url_rgx)) {
    throw std::runtime_error("Invalid url provided");
  }

  AzBlobUrl result;
  result.schema = matches[1].str();
  result.account_name = matches[2].str();
  std::stringstream endpoint;
  endpoint << result.account_name << "." << matches[3].str();
  result.blob_endpoint = endpoint.str();
  result.container_name = matches[4].str();
  result.blob_name = matches[5].str();
  if (matches.size() == 7) {
    result.sas_token = matches[6].str();
  }

  return result;
}
}  // namespace rosbaz