#include "components.h"
#include "handlers/request_add.h"
#include "handlers/request_result.h"

#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>

namespace wj_service_template
{
  userver::components::ComponentList MakeComponents()
  {
    auto component_list = userver::components::MinimalServerComponentList()
                            .Append<userver::server::handlers::Ping>()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::components::HttpClient>()
                            .Append<userver::server::handlers::TestsControl>()
                            .Append<userver::clients::dns::Component>()
                            .Append<userver::components::Postgres>("postgres-db-1");

    wj_service_template::AppendRequestAdd(component_list);
    wj_service_template::AppendRequestResult(component_list);

    return component_list;
  }
}