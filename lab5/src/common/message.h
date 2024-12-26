#pragma once

#include <vector>
#include <sstream>
#include <chrono>

typedef int Number;

enum class MessageType : int {
    SumNumbersRequest,
    SumNumbersResponse,

    CreateNodeRequest,
    CreateNodeResponse,

    PingRequest,
    PingResponse,

    ErrorResponse
};

class MessageVisitor;

class Message {
public:
    int trace_id{};
    int slave_id{};
    std::chrono::system_clock::time_point sent = std::chrono::system_clock::now();

    virtual ~Message() = default;

    virtual void serialize(std::ostream& stream) {
        int64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(sent).time_since_epoch()).count();
        stream << slave_id << " " << trace_id << " " << ms << " ";
    }

    virtual void deserialize(std::istream& stream) {
        int64_t ms;
        stream >> slave_id >> trace_id >> ms;
        sent = std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds(std::chrono::duration<long>(ms)));
    }

    virtual void accept(MessageVisitor& visitor) = 0;
    virtual MessageType get_message_type() = 0;
};

class SumNumbersRequest : public Message {
public:
    std::vector<Number> numbers{};

    void deserialize(std::istream& stream) override {
        int length;
        stream >> length;
        numbers.resize(length);
        for (int i = 0; i < length; i++) {
            stream >> numbers[i];
        }
        Message::deserialize(stream);
    }

    void serialize(std::ostream& stream) override {
        stream << numbers.size() << " ";
        for (int number : numbers) {
            stream << number << " ";
        }
        Message::serialize(stream);
    }

    void accept(MessageVisitor &visitor) override;
    MessageType get_message_type() override { return MessageType::SumNumbersRequest; }
};

class SumNumbersResponse : public Message {
public:
    Number sum{};

    SumNumbersResponse() = default;
    explicit SumNumbersResponse(Number sum) : sum(sum) {}

    void deserialize(std::istream& stream) override {
        stream >> sum;
        Message::deserialize(stream);
    }

    void serialize(std::ostream& stream) override {
        stream << sum << " ";
        Message::serialize(stream);
    }

    void accept(MessageVisitor &visitor) override;
    MessageType get_message_type() override { return MessageType::SumNumbersResponse; }
};

class CreateNodeRequest : public Message {
public:
    int id{};
    int heartbeat{};

    void deserialize(std::istream& stream) override {
        stream >> id >> heartbeat;
        Message::deserialize(stream);
    }

    void serialize(std::ostream& stream) override {
        stream << id << " " << heartbeat << " ";
        Message::serialize(stream);
    }

    void accept(MessageVisitor &visitor) override;
    MessageType get_message_type() override { return MessageType::CreateNodeRequest; }
};

class CreateNodeResponse : public Message {
public:
    int parent_id{};
    int pid{};

    void deserialize(std::istream& stream) override {
        stream >> pid >> parent_id;
        Message::deserialize(stream);
    }

    void serialize(std::ostream& stream) override {
        stream << pid << " " << parent_id << " ";
        Message::serialize(stream);
    }

    void accept(MessageVisitor &visitor) override;
    MessageType get_message_type() override { return MessageType::CreateNodeResponse; }
};

class ErrorResponse : public Message {
public:
    std::string message;

    void deserialize(std::istream& stream) override {
        stream >> message;
        Message::deserialize(stream);
    }

    void serialize(std::ostream& stream) override {
        stream << message << " ";
        Message::serialize(stream);
    }

    void accept(MessageVisitor &visitor) override;
    MessageType get_message_type() override { return MessageType::ErrorResponse; }
};

class PingRequest : public Message {
public:
    int time{};

    void deserialize(std::istream &stream) override {
        stream >> time;
        Message::deserialize(stream);
    }

    void serialize(std::ostream& stream) override {
        stream << time << " ";
        Message::serialize(stream);
    }

    void accept(MessageVisitor &visitor) override;
    MessageType get_message_type() override { return MessageType::PingRequest; }
};

class PingResponse : public Message {
public:
    void accept(MessageVisitor &visitor) override;
    MessageType get_message_type() override { return MessageType::PingResponse; }
};

Message* create_message_by_type(MessageType type);

class MessageVisitor {
public:
    virtual void handle(SumNumbersRequest& request) = 0;
    virtual void handle(SumNumbersResponse& request) = 0;

    virtual void handle(CreateNodeRequest& request) = 0;
    virtual void handle(CreateNodeResponse& request) = 0;

    virtual void handle(PingRequest& request) = 0;
    virtual void handle_pong(PingResponse& request) = 0;

    virtual void handle(ErrorResponse& request) = 0;

    virtual void handle_timeout(int slave_id) = 0;
    virtual void handle_error(int slave_id, std::string error) = 0;
};