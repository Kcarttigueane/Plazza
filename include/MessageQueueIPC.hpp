/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** MessageQueueIPC.hpp
*/

#pragma once

#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>

#include "Plazza.hpp"

#define BUFFER_SIZE 1024

/**
** @class MessageQueueIPC
** @brief A class for managing IPC using POSIX Message Queues.
**
** This class provides an easy-to-use interface for sending and receiving
** messages between processes using POSIX Message Queues.
**/
class MessageQueueIPC {
  private:
    mqd_t _messageQueueId;
    std::string _queueName;
    bool _shouldUnlink;

  public:
    /**
    ** @brief Constructs a new MessageQueueIPC object.
    **
    ** @param queueName The name of the message queue.
    ** @param create A flag indicating whether to create a new message queue (default: true).
    **/
    MessageQueueIPC(const std::string& queueName, bool create = true)
        : _queueName(queueName), _shouldUnlink(create)
    {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 10;
        attr.mq_msgsize = BUFFER_SIZE;
        attr.mq_curmsgs = 0;

        if (create) {
            _messageQueueId = mq_open(queueName.c_str(), O_CREAT | O_RDWR, 0666, &attr);
        } else {
            _messageQueueId = mq_open(queueName.c_str(), O_RDWR);
        }

        if (_messageQueueId == (mqd_t)-1) {
            std::cerr << "Error creating message queue." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    /**
    ** @brief Destructor; closes and unlinks the message queue if needed.
    **/
    ~MessageQueueIPC()
    {
        mq_close(_messageQueueId);
        if (_shouldUnlink) {
            mq_unlink(_queueName.c_str());
        }
    }

    // ! Getters:

    /**
    ** @brief Gets the number of messages currently in the queue.
    **
    ** @return int The number of messages in the queue, or FAILURE on error.
    **/
    int getQueueLength() const
    {
        struct mq_attr attr;
        if (mq_getattr(_messageQueueId, &attr) == FAILURE) {
            std::cerr << "Error getting message queue information." << std::endl;
            return FAILURE;
        }
        return attr.mq_curmsgs;
    }

    /**
    ** @brief Gets the message queue identifier.
    **
    ** @return mqd_t The message queue identifier.
    **/
    mqd_t getMessageQueueId() const { return _messageQueueId; }

    // ! Methods:

    /**
    ** @brief Sends a message to the message queue.
    **
    ** @param message The message to send.
    ** @return bool true if the message was sent successfully, false otherwise.
    **/
    bool send(const std::string& message)
    {
        if (mq_send(_messageQueueId, message.c_str(), message.size(), 0) == FAILURE) {
            std::cerr << "Error sending message." << std::endl;
            return false;
        }
        return true;
    }

    /**
    ** @brief Receives a message from the message queue.
    **
    ** @return std::string The received message, or an empty string on error.
    **/
    std::string receive()
    {
        char buffer[BUFFER_SIZE + 1];
        ssize_t bytesRead;

        bytesRead = mq_receive(_messageQueueId, buffer, BUFFER_SIZE, NULL);
        if (bytesRead == FAILURE) {
            std::cerr << "Error receiving message." << std::endl;
            return "";
        }

        buffer[bytesRead] = '\0';
        return std::string(buffer);
    }

    // ! Overload Operators:

    /**
    ** @brief Overloaded << operator for sending messages.
    **
    ** @param message The message to send.
    ** @return MessageQueueIPC& A reference to the current object, for chaining.
    **/
    MessageQueueIPC& operator<<(const std::string& message)
    {
        send(message);
        return *this;
    }

    /**
    ** @brief Overloaded >> operator for receiving messages.
    **
    ** @param message A reference to a string where the received message will be stored.
    ** @return MessageQueueIPC& A reference to the current object, for chaining.
    **/
    MessageQueueIPC& operator>>(std::string& message)
    {
        message = receive();
        return *this;
    }
};
