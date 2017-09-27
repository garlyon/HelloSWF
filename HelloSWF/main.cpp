#include <aws/core/Aws.h>


int main( int argc, char** argv )
{
  Aws::SDKOptions options;

  options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
  
  Aws::InitAPI( options );

  {
    // make your SDK calls here.
    void decider();
    decider();
  }

  Aws::ShutdownAPI( options );
  
  return 0;
}