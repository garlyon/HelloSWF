#include "run.h"
#include <aws/swf/SWFClient.h>
#include <aws/swf/model/PollForDecisionTaskRequest.h>
#include <aws/swf/model/RespondDecisionTaskCompletedRequest.h>
//#include <aws/swf/model/CompleteWorkflowExecutionDecisionAttributes.h>
#include <aws/swf/model/Decision.h>
#include <aws/core/utils/Outcome.h>
#include <iostream>


namespace X = Aws::SWF::Model;


std::string f( const std::string& s )
{
  std::clog << "input is [" << s << "]" << std::endl;
  return "MyDecider schedules activity with string <" + s + ">";
}


std::string g( const std::string& s )
{
  std::clog << "result is [" << s << "]" << std::endl;
  return "MyDecider completes workflow with string <" + s + ">";
}


void onWorkflowExecutionStarted( Aws::SWF::SWFClient& client, const X::PollForDecisionTaskResult& res, const X::HistoryEvent& e )
{
  auto out = f( e.GetWorkflowExecutionStartedEventAttributes().GetInput() );

  auto respond = client.RespondDecisionTaskCompleted( X::RespondDecisionTaskCompletedRequest{}
    .WithTaskToken( res.GetTaskToken() )
    .WithDecisions(
    {
      X::Decision{}
      .WithDecisionType( X::DecisionType::ScheduleActivityTask )
        .WithScheduleActivityTaskDecisionAttributes( X::ScheduleActivityTaskDecisionAttributes{}
          .WithInput( out )
          .WithActivityId( "some activity id" )
          .WithActivityType( X::ActivityType{}.WithName( "TestActivity" ).WithVersion( "1.0.0" ) ) )
    } ) );

  if( !respond.IsSuccess() )
  {
    std::cerr << respond.GetError().GetMessage() << std::endl;
  }
}


void onActivityTaskCompleted( Aws::SWF::SWFClient& client, const X::PollForDecisionTaskResult& res, const X::HistoryEvent& e )
{
  auto out = g( e.GetActivityTaskCompletedEventAttributes().GetResult() );

  auto respond = client.RespondDecisionTaskCompleted( X::RespondDecisionTaskCompletedRequest{}
    .WithTaskToken( res.GetTaskToken() )
    .WithDecisions(
    {
      X::Decision{}
      .WithDecisionType( X::DecisionType::CompleteWorkflowExecution )
        .WithCompleteWorkflowExecutionDecisionAttributes( X::CompleteWorkflowExecutionDecisionAttributes{}
          .WithResult( out ) )
    } ) );

  if( !respond.IsSuccess() )
  {
    std::cerr << respond.GetError().GetMessage() << std::endl;
    return;
  }
}


void eventSwitch( Aws::SWF::SWFClient& client, const X::PollForDecisionTaskResult& res )
{
  std::clog << "run event switch" << std::endl;
  for( auto eid = res.GetPreviousStartedEventId(); eid != res.GetStartedEventId(); ++eid )
  {
    const auto& e = res.GetEvents().at( eid );

    std::clog << "process event #" << eid << " = [" << X::EventTypeMapper::GetNameForEventType( e.GetEventType() ) << "]" << std::endl;
    
    switch( e.GetEventType() )
    {
    case X::EventType::WorkflowExecutionStarted:
      onWorkflowExecutionStarted( client, res, e );
      break;

    case X::EventType::ActivityTaskCompleted:
      onActivityTaskCompleted( client, res, e );
      break;

    default:
      std::cerr << "ignore event type [" << X::EventTypeMapper::GetNameForEventType( e.GetEventType() ) << "]" << std::endl;
      break;
    }
  }
}


void run()
{
  Aws::SWF::SWFClient client;

  auto task = client.PollForDecisionTask( X::PollForDecisionTaskRequest{}
    .WithDomain( "HelloWorld" )
    .WithTaskList( X::TaskList{}.WithName( "TaskList" ) ) );

  if( !task.IsSuccess() )
  {
    std::cerr << task.GetError().GetMessage() << std::endl;
    return;
  }

  eventSwitch( client, task.GetResult() );
}
