#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include <array>

#include <Printable.h>

namespace protocol
{

  union Command;
  class ProtocolMessage;

  enum class CommandType : char
  {
    MUTE = 's',
    UNMUTE = 'u',
    HELP = 'h',
    ALARM = 'a',
    INFO = 'i',
  };

  class AlarmMessage final
  {
  public:
    static const size_t MAX_LENGTH = 80;

    using Buffer = std::array<char, AlarmMessage::MAX_LENGTH>;

  private:
    const size_t messageLength;
    const std::array<char, AlarmMessage::MAX_LENGTH> message;

  public:
    explicit AlarmMessage(const size_t messageLength, const Buffer message)
        : messageLength(messageLength), message(std::move(message)) {};

    AlarmMessage(AlarmMessage &&other) = default;
    AlarmMessage operator=(AlarmMessage &&other) { return std::move(other); }
    AlarmMessage(const AlarmMessage &&other)
        : message(std::move(other.message)), messageLength(other.messageLength) {}
    const AlarmMessage operator=(const AlarmMessage &&other)
    {
      return std::move(other);
    }

    AlarmMessage() = delete;
    AlarmMessage(AlarmMessage &other) = delete;
    AlarmMessage(const AlarmMessage &other) = delete;
  };

  class AlarmMessageId final
  {
  public:
    // TODO: Find out what this value is SUPPOSED to be.
    static const size_t MAX_LENGTH = 10;

    using Buffer = std::array<char, AlarmMessageId::MAX_LENGTH>;

  private:
    static const size_t TOTAL_MAX_LENGTH = AlarmMessageId::MAX_LENGTH + 1;

  public:
    const size_t idLength;
    const std::array<char, AlarmMessageId::TOTAL_MAX_LENGTH> id;

  public:
    explicit AlarmMessageId(const size_t idLength, const Buffer id);
    AlarmMessageId(AlarmMessageId &&other) = default;
    AlarmMessageId(const AlarmMessageId &&other)
        : id(std::move(other.id)), idLength(other.idLength) {}
    AlarmMessageId operator=(AlarmMessageId &&other) { return std::move(other); }
    AlarmMessageId operator=(const AlarmMessageId &&other)
    {
      return std::move(other);
    }

    AlarmMessageId() = delete;
    AlarmMessageId(AlarmMessageId &other) = delete;
    AlarmMessageId(const AlarmMessageId &other) = delete;

  private:
    static std::array<char, AlarmMessageId::TOTAL_MAX_LENGTH>
    validateId(const size_t idLength,
               const Buffer);
  };

  class AlarmTypeDesignator final : Printable
  {
  public:
    static const size_t DESIGNATOR_LENGTH = 3;
    using Buffer = std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH>;

  private:
    static const size_t TOTAL_DESIGNATOR_LENGTH =
        AlarmTypeDesignator::DESIGNATOR_LENGTH + 1;
    const std::array<char, AlarmTypeDesignator::DESIGNATOR_LENGTH + 1> designator;
    const bool empty;

  public:
    AlarmTypeDesignator(const Buffer designator, const bool empty);

    AlarmTypeDesignator(AlarmTypeDesignator &&other) = default;
    AlarmTypeDesignator(const AlarmTypeDesignator &&other)
        : designator(std::move(other.designator)), empty(other.empty) {}
    AlarmTypeDesignator operator=(AlarmTypeDesignator &&source)
    {
      return std::move(source);
    }
    AlarmTypeDesignator operator=(const AlarmTypeDesignator &&source)
    {
      return std::move(source);
    }

    AlarmTypeDesignator() = delete;
    AlarmTypeDesignator(AlarmTypeDesignator &other) = delete;
    AlarmTypeDesignator(const AlarmTypeDesignator &other) = delete;

    virtual ~AlarmTypeDesignator();
    // Methods
  public:
    const char *const getValue() const;

    size_t printTo(Print &print) const override;

  private:
    static std::array<char, AlarmTypeDesignator::TOTAL_DESIGNATOR_LENGTH>
    validateDesignator(const Buffer buffer, const bool empty);
  };

  class AlarmCommand final
  {
  public:
    enum class Level : char
    {
      Level1 = '1',
      Level2 = '2',
      Level3 = '3',
      Level4 = '4',
      Level5 = '5',
    };

    const Level level;
    const AlarmMessageId messageId;
    const AlarmTypeDesignator typeDesignator;
    const AlarmMessage message;

  public:
    explicit AlarmCommand(const AlarmCommand::Level alarmLevel,
                          const AlarmMessage alarmMessage,
                          const AlarmMessageId messageId,
                          const AlarmTypeDesignator typeDesignator)
        : level(alarmLevel), message(std::move(alarmMessage)),
          messageId(std::move(messageId)),
          typeDesignator(std::move(typeDesignator)) {}

    AlarmCommand(AlarmCommand &&other) = default;
    AlarmCommand(const AlarmCommand &&other)
        : level(other.level), message(std::move(other.message)),
          messageId(std::move(other.messageId)),
          typeDesignator(std::move(other.typeDesignator)) {}
    AlarmCommand operator=(AlarmCommand &&other) noexcept
    {
      return std::move(other);
    }
    AlarmCommand operator=(const AlarmCommand &&other)
    {
      return std::move(other);
    }

    AlarmCommand() = delete;
    AlarmCommand(AlarmCommand &other) = delete;
    AlarmCommand(const AlarmCommand &other) = delete;

    char alarmLevelIntoChar() const;
  };

  class InfoCommand final
  {
  public:
    explicit InfoCommand() = default;
    InfoCommand(InfoCommand &&other) = default;
    InfoCommand operator=(InfoCommand &&other) noexcept
    {
      return std::move(other);
    }

    InfoCommand(const InfoCommand &&other) {}
    InfoCommand operator=(const InfoCommand &&other) { return std::move(other); }

    InfoCommand(InfoCommand &other) = delete;
    InfoCommand(const InfoCommand &other) = delete;
  };

  struct ProtocolMessage final
  {
  public:
    static const size_t BUFFER_LENGTH = 131;

    const union
    {
      AlarmCommand alarm;
      InfoCommand info;
    };

    const CommandType commandType;

    using ProtocolBuffer = std::array<char, ProtocolMessage::BUFFER_LENGTH>;

    // Constructors and operator overloads
  public:
    ProtocolMessage(AlarmCommand alarmCommand)
        : commandType(CommandType::ALARM), alarm(std::move(alarmCommand)) {}
    ProtocolMessage(InfoCommand infoCommand)
        : commandType(CommandType::INFO), info(std::move(infoCommand)) {}

    ProtocolMessage operator=(ProtocolMessage &&other) noexcept
    {
      return std::move(other);
    }
    ProtocolMessage(const ProtocolMessage &&other)
        : commandType(other.commandType)
    {
      switch (this->commandType)
      {
      case CommandType::ALARM:
        this->alarm = std::move(other.alarm);
        break;
      case CommandType::INFO:
        this->info = std::move(other.info);
        break;
      }
    }
    const ProtocolMessage operator=(const ProtocolMessage &&other)
    {
      return std::move(other);
    }

    ProtocolMessage() = delete;
    ProtocolMessage(ProtocolMessage &other) = delete;

    ~ProtocolMessage() {}

    // TODO: This needs to throw an error if the message could not be deserialized
    static ProtocolMessage
    deserialize(const char *const buffer, const size_t numBytes);
  };

  class AlarmCommandBuilder final
  {
  private:
    static const char ID_START_CHARACTER = '{';
    static const char ID_END_CHARACTER = '}';

    static const char DESIGNATOR_START_CHARACTER = '[';
    static const char DESIGNATOR_END_CHARACTER = ']';

  private:
    AlarmCommand::Level level;

    size_t idLength;
    AlarmMessageId::Buffer idBuffer;

    size_t designatorLength;
    AlarmTypeDesignator::Buffer designatorBuffer;

    size_t messageLength;
    AlarmMessage::Buffer messageBuffer;

  private:
    AlarmCommandBuilder();

    size_t deserializeLevelBytes(const char *const buffer, size_t numBytes);
    size_t deserializeIdBytes(const char *const buffer, const size_t numBytes);
    size_t deserializeTypeDesignatorBytes(const char *const buffer, const size_t numBytes);
    size_t deserializeMessageBytes(const char *const buffer, const size_t numBytes);

  public:
    static AlarmCommand buildAlarmCommand(const char *const buffer, const size_t numBytes);

    AlarmCommandBuilder(AlarmCommandBuilder &&other) = default;
    AlarmCommandBuilder operator=(AlarmCommandBuilder &&other)
    {
      return std::move(other);
    }

    AlarmCommandBuilder(AlarmCommandBuilder &other) = delete;
    AlarmCommandBuilder operator=(AlarmCommandBuilder &other) = delete;
  };

} // namespace protocol

#endif
