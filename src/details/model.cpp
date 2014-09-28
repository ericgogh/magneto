#include "../model.h"

namespace magneto {

void Talk::Assign(
    size_t no_talk_arg,
    Category category_arg, 
    const Service* service_arg, 
    Protocol::Category protocol_category,
    const std::pair<const char*, int>* buf,
    time_t timeo_ms,
    int fd_arg, 
    const Remote* remote_arg) {
  int ret;
  no_talk = no_talk_arg;
  category = category_arg;
  service = service_arg;
  starttime_ms = Time::GetCurrentMsec(false);
  endtime_ms = starttime_ms + timeo_ms;
  error = ErrorNo::kOk;
  fail_remotes.clear();
  fd = fd_arg;
  remote = remote_arg;
  if (NULL!=service) {
    retry = service->retry;
  } else {
    retry=0;
  }

  switch (category) {
    case kWriteAndRead : {
      protocol_write = PoolProtocols::GetWrite(protocol_category);
      protocol_write->Reset(buf->first, buf->second);
      ret = protocol_write->Encode();
      if (true!=ret) {
        PoolProtocols::FreeWrite(protocol_write);
        fd = -1;
        error = ErrorNo::kEncode;
        return;
      }
      protocol_read = PoolProtocols::GetRead(protocol_category);
      protocol_read->Reset();
      status = Talk::kWrite;
      break;
    }
    case kWriteOnly : {
      protocol_write = PoolProtocols::GetWrite(protocol_category);
      protocol_write->Reset(buf->first, buf->second);
      ret = protocol_write->Encode();
      if (true!=ret) {
        PoolProtocols::FreeWrite(protocol_write);
        fd = -1;
        error = ErrorNo::kEncode;
        return;
      }
      protocol_read = NULL;
      status = Talk::kWrite;
      break;
    }
    case kReadOnly : {
      protocol_write = NULL;
      protocol_read = PoolProtocols::GetRead(protocol_category);
      protocol_read->Reset();
      status = Talk::kRead;
      break;
    }
    default :
      MAG_BUG(true)
  }
}

}