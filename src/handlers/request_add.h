#pragma once

#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace wj_service_template
{
  namespace
  {
    class RequestAdd final : public userver::server::handlers::HttpHandlerBase
    {
    public:
      static constexpr std::string_view kName = "handler-request-add";

      RequestAdd(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& component_context);

      std::string HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                     userver::server::request::RequestContext&) const override;

    private:
      userver::storages::postgres::ClusterPtr pg_cluster_;
    };
  }

  void AppendRequestAdd(userver::components::ComponentList& component_list);
}