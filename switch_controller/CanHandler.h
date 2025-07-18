
#ifndef CAN_HANDLER_H
#define CAN_HANDLER_H

#include <mcp_can.h>

class CanHandler {
public:
    CanHandler(MCP_CAN& can) : m_can(can) {}
    
    bool receive(long* id, byte* len, byte* buf) {
        if(m_can.checkReceive() == CAN_MSGAVAIL) {
            m_can.readMsgBuf(id, len, buf);
            return true;
        }
        return false;
    }

private:
    MCP_CAN& m_can;
};




#endif CAN_HANDLER_H