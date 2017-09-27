#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/swf/SWFClient.h>
#include <aws/swf/SWFErrors.h>

#include <aws/swf/model/PollForDecisionTaskRequest.h>
#include <aws/swf/model/RespondDecisionTaskCompletedRequest.h>


void decider()
{
  Aws::SWF::SWFClient client;

  auto in = client.PollForDecisionTask( Aws::SWF::Model::PollForDecisionTaskRequest{}
    .WithDomain( "HelloWorld" )
    .WithTaskList( Aws::SWF::Model::TaskList{}.WithName( "TaskList" ) ) );

  if( !in.IsSuccess() )
  {
    std::cerr << in.GetError().GetMessage() << std::endl;
    return;
  }

  if( in.GetResult().GetEvents().empty() )
  {
    return;
  }

  {
    auto event = in.GetResult().GetEvents().front();
    event.GetEventType();
    auto res = in.GetResult().GetEvents().front().GetWorkflowExecutionStartedEventAttributes().GetInput();
  }

  auto out = client.RespondDecisionTaskCompleted( Aws::SWF::Model::RespondDecisionTaskCompletedRequest{}
    .WithTaskToken( in.GetResult().GetTaskToken() ) );

  if( !out.IsSuccess() )
  {
    std::cerr << out.GetError().GetMessage() << std::endl;
    return;
  }

  return;
}