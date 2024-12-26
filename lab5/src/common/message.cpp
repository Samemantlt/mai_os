#include "message.h"



Message *create_message_by_type(MessageType type) {
    switch (type) {
        case MessageType::SumNumbersRequest:
            return new SumNumbersRequest();
        case MessageType::SumNumbersResponse:
            return new SumNumbersResponse();
        case MessageType::CreateNodeRequest:
            return new CreateNodeRequest();
        case MessageType::CreateNodeResponse:
            return new CreateNodeResponse();
        case MessageType::PingRequest:
            return new PingRequest();
        case MessageType::PingResponse:
            return new PingResponse();
        case MessageType::ErrorResponse:
            return new ErrorResponse();
        default:
            throw std::runtime_error("Unknown type");
    }
}

void SumNumbersRequest::accept(MessageVisitor &visitor) { visitor.handle(*this); }
void SumNumbersResponse::accept(MessageVisitor &visitor) { visitor.handle(*this); }

void CreateNodeRequest::accept(MessageVisitor &visitor) { visitor.handle(*this); }
void CreateNodeResponse::accept(MessageVisitor &visitor) { visitor.handle(*this); }

void PingRequest::accept(MessageVisitor &visitor) { visitor.handle(*this); }
void PingResponse::accept(MessageVisitor &visitor) { visitor.handle_pong(*this); }

void ErrorResponse::accept(MessageVisitor &visitor) { visitor.handle(*this); }
