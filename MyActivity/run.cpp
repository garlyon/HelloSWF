#include "run.h"
#include <aws/swf/SWFClient.h>
#include <aws/swf/model/PollForActivityTaskRequest.h>
#include <aws/swf/model/RespondActivityTaskCompletedRequest.h>
#include <aws/core/utils/Outcome.h>
#include <iostream>


std::string f( const std::string& s )
{
  std::clog << "input is [" << s << "]" << std::endl;
  return "MyActivity reply is <" + s + ">";
}


void run()
{
  namespace X = Aws::SWF::Model;

  Aws::SWF::SWFClient client;

  auto task = client.PollForActivityTask( X::PollForActivityTaskRequest{}
    .WithDomain( "HelloWorld" )
    .WithTaskList( X::TaskList{}.WithName( "TaskList" ) ) );

  if( !task.IsSuccess() )
  {
    std::cerr << task.GetError().GetMessage() << std::endl;
    return;
  }
  
  auto out = f( task.GetResult().GetInput() );

  auto respond = client.RespondActivityTaskCompleted( X::RespondActivityTaskCompletedRequest{}
    .WithTaskToken( task.GetResult().GetTaskToken() )
    .WithResult( out ) );

  if( !respond.IsSuccess() )
  {
    std::cerr << respond.GetError().GetMessage() << std::endl;
    return;
  }
}