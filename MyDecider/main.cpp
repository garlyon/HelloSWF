#include "run.h"
#include <aws/core/Aws.h>
#include <iostream>


int main()
{
  Aws::SDKOptions options;
  Aws::InitAPI( options );

  std::cout << "start to poll decisions" << std::endl;

  while( true )
  {
    run();
  }

  Aws::ShutdownAPI( options );
  return 0;
}

