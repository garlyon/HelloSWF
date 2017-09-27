#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/swf/SWFClient.h>
#include <aws/swf/SWFErrors.h>

#include <aws/swf/model/ListDomainsRequest.h>
#include <aws/swf/model/ListDomainsResult.h>


void domain()
{
  Aws::SWF::SWFClient client;

  Aws::SWF::Model::ListDomainsRequest q;

  q.WithRegistrationStatus( Aws::SWF::Model::RegistrationStatus::REGISTERED );

  auto a = client.ListDomains( q );

  if( a.IsSuccess() )
  {
    auto res = a.GetResult();
    return;
  }
  else
  {
    auto err = a.GetError();
    return;
  }
}